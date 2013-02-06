<?php

/**
 * Main function for cron script: 
 * Runs the cron script that iterates through the users
 * in the courses in the current term, sending an email for each,
 * if there are upcoming assignments in 3 days
 * or late assignments that are either 1 or 8 days overdue.
 */
function run_cron() {
  /* 1) Collect the information on upcoming assignment notifications. */
  $assignment_array = run_assignment_queries();
  $row_count = $assignment_array['total_rows'];
  $result_rows = $assignment_array['rows'];
  // Free some memory.
  unset($assignment_array['rows']);
  
  /* 2) Build the array used for sending out email notifications. */
  $notifications_array = get_notifications_array($result_rows);
  // Free some memory.
  unset($result_rows);

  /* 3) Send the emails for each. */
  $email_result_array = send_emails($notifications_array['data_array']);
  
  /* 4) Do logging to database. */
  cron_db_log($assignment_array, $notifications_array, $email_result_array);
  
  /* 5) Do debugging to screen (only when in development). */
  if(IN_DEVELOPMENT == TRUE) {
    cron_debug(CUR_DEBUG_TYPE, $assignment_array, $notifications_array, $email_result_array);
  }
}

/* Step 1: Query the database for assignments. */
function run_assignment_queries() {
  // Execute query for upcoming assignments.
  $sql = 'SELECT * FROM view_upcoming_assignments_report order by lastname asc, time_due asc, assignment asc';
  $results = db_query($sql);
  // Create array to store the upcoming and previous assignments.
  $num_rows_upcoming = 0;
  $rows = array();
  // Set the rows array.
  while($row = db_fetch_array($results)) {
    // Correct the field names from the database table. (Legacy.)
    $row = set_correct_field_names($row);
	$rows[] = $row;
	$num_rows_upcoming++;
  }
  // Execute query for previous assignments.
  $sql = 'SELECT * FROM view_missing_assignments_ids_report order by lastname asc, time_due asc, assignment asc';
  $results = db_query($sql);
  $num_rows_missing = 0;
  // Add in the previous assignments.
  while($row = db_fetch_array($results)) {
    // Correct the field names from the database table. (Legacy.)
    $row = set_correct_field_names($row);
    $rows[] = $row;
	$num_rows_missing++;
  }
  $total_rows = $num_rows_upcoming + $num_rows_missing;
  return array('rows' => $rows, 'total_rows' => $total_rows, 'upcoming_rows' => $num_rows_upcoming, 'missing_rows' => $num_rows_missing);
}

/* Step 2: Build the array used to indicate the people who should be notified about assignments. */
function get_notifications_array($rows) {
  // Build data array, one level at a time.
  $data_array = array();
  $valid_array = array();
  $invalid_array = array();
  $valid_rows = 0;
  $invalid_rows = 0;
  // @todo: Make the following array checking code less fragile (in case of database returning no results.)
  foreach($rows as $row_index => $row) {
    // Shift the time when in test mode, in order to see different results.
    if(IN_DEVELOPMENT == TRUE) {
      $time_now = time_shift_days(SHIFT_DAYS_AMOUNT, SHIFT_POSITIVE);
    }
    else {
      $time_now = time();
    }
    $date_interval = get_date_interval($row['time_due'], $time_now);
	// Only add to the array if the date interval is valid.
	if($date_interval['type'] != INVALID_INTERVAL) {
	  // Get the row for logging valid data from this time.
	  $valid_array[] = get_log_row_data($row, $date_interval, $time_now);
	  // Add the student id.
          if(!in_array($row['studentid'], $data_array)) {
	    $data_array[$row['studentid']]['username'] = array($row['firstname'], $row['lastname']);
	    $data_array[$row['studentid']]['mailto'] = $row['mailto'];
            $data_array[$row['studentid']]['cc_email'] = $row['cc_email'];
	  }
	  // Add the course id.
	  if(!in_array($row['courseid'], $data_array[$row['studentid']][$date_interval['type']])) {
	    $data_array[$row['studentid']][$date_interval['type']][$row['courseid']]['coursename'] = $row['course_name'];
	  }
	  // Add the assignment.
	  $data_array[$row['studentid']][$date_interval['type']][$row['courseid']]['assignments'][] = $row['assignment'];
	  if(empty($row['studentid'])) {
	    echo '<p>Invalid row:</p><pre>' . print_r($row, TRUE) . '</pre>';
	  }
	  $valid_rows++;
	}
	else {
	  // Get the row for logging invalid data from this time.
	  // Invalid data will only be printed to screen in test mode, never written to the database.
	  $invalid_array[] = get_log_row_data($row, $date_interval, $time_now);
	  $invalid_rows++;
	}
  }
  return array('data_array' => $data_array, 'valid_rows' => $valid_rows, 'invalid_rows' => $invalid_rows, 'valid_array' => $valid_array, 'invalid_array' => $invalid_array);
}

/* Step 3: Email sending function. */
function send_emails($data_array) {
  $num_success = 0;
  $num_failure = 0;
  $success_array = array();
  $failure_array = array();
  $emails_sent_array = array();
  // Iterate over the data array to determine what emails to send.
  foreach($data_array as $userid => $user_notifications) {
    // Only send for valid types (not for the username).
    foreach($user_notifications as $notification_type => $notification_data) {
	  $valid_types = get_valid_intervals();
	  if(in_array($notification_type, $valid_types)) {
            if($data_array[$userid]['cc_email'] == NULL) {
              $data_array[$userid]['cc_email'] = '';
            }
	    $success = send_email($data_array[$userid]['username'], $data_array[$userid]['mailto'], $notification_type, $notification_data, $data_array[$userid]['cc_email']);
		if($success == TRUE) {
		  $num_success++;
		  $success_array[] = get_log_email_data($userid, $data_array[$userid]['username'], $data_array[$userid]['mailto'], $notification_type);
		}
		else {
		  $num_failure++;
		  $failure_array[] = get_log_email_data($userid, $data_array[$userid]['username'], $data_array[$userid]['mailto'], $notification_type);
		}
	  }
	}
  }
  $email_result_array = array('num_success' => $num_success, 'num_failure' => $num_failure, 'success_array' => $success_array, 'failure_array' => $failure_array);
  return $email_result_array;
}

/* Step 4: Database logging function. */
function cron_db_log($assignment_array = array(), $notifications_array = array(), $email_result_array = array()) {
  // Log the totals.
  $sql = 'insert into cron_run_count(time_complete, total_queried, total_queried_upcoming, total_queried_missing, total_to_notify, total_no_notifications, total_sent_success, total_sent_failure)' .
         'values (%d, %d, %d, %d, %d, %d, %d, %d)';
  $result = db_query($sql, time(), $assignment_array['total_rows'], $assignment_array['upcoming_rows'], $assignment_array['missing_rows'], 
    $notifications_array['valid_rows'], $notifications_array['invalid_rows'], $email_result_array['num_success'], $email_result_array['num_failure']);
  // Get the last inserted id.
  // @see http://api.drupal.org/api/drupal/includes%21database.mysql-common.inc/function/db_last_insert_id/6
  $crid = db_last_insert_id('cron_run_count', 'crid');
  // Log the individual emails sent.
  // @todo: Consider whether to do this as a multi-insert for DB performance reasons.
  foreach($email_result_array['success_array'] as $email_sent) {
    _cron_db_log($crid, $email_sent);
  }
}

/* Called in inner loop to log each row of emails sent. */
function _cron_db_log($crid, $email_sent) {
  $sql = 'insert into cron_run_item(crid, userid, username, mailto, time_sent, notification_type) values (%d, %d, "%s", "%s", %d, %d)';
  $result = db_query($sql, $crid, $email_sent['userid'], $email_sent['username'], $email_sent['mailto'], $email_sent['time_now'], $email_sent['notification_type_id']);
}

/* Step 5: Cron script debugging function. */
function cron_debug($debug_type, $assignment_array = array(), $notifications_array = array(), $email_result_array = array()) {
  switch($debug_type) {
    case DEBUG_QUERIES:
      // Debug of queries.
      echo 'Total rows: ' . $assignment_array['total_rows'] . '<br/>';
      echo 'Upcoming: ' . $assignment_array['upcoming_rows'] . '<br/>';
      echo 'Missing: ' . $assignment_array['missing_rows'] . '<br/>';
	  break;
	case DEBUG_DATA_BUILD:
      // Debug of grouping into notification type bundles.
      echo 'Valid rows: ' . $notifications_array['valid_rows'] . '<br/>';
      echo 'Invalid rows: ' . $notifications_array['invalid_rows'] . '<br/>';
      echo '<p>Valid data:</p>';
      echo '<pre>' . print_r($notifications_array['valid_array'], TRUE) . '</pre>';
      echo '<p>Invalid data:</p>';
      echo '<pre>' . print_r($notifications_array['invalid_array'], TRUE) . '</pre>';
      echo '<p>Built notifications array:</p>';
      echo '<pre>' . print_r($notifications_array['data_array'], TRUE) . '</pre>';
	  break;
	case DEBUG_EMAIL_SEND:
      // Debug of email functionality.
      echo 'Emails sent: ' . $email_result_array['num_success'] . '<br/>';
      echo 'Emails failed to send: ' . $email_result_array['num_failure'] . '<br/>';
      echo '<p>Email successes:</p>';
      echo '<pre>' . print_r($email_result_array['success_array'], TRUE) . '</pre>';
      echo '<p>Email failures:</p>';
      echo '<pre>' . print_r($email_result_array['failure_array'], TRUE) . '</pre>';
	  break;
  }
  return;
}

/**
 * Used to fix inconsistencies in query tables:
 *   userid on missing_assignments, studentid on upcoming_assignments, 
 *   course_name on missing_assignments, coursename on upcoming_assignments
 * Used in step 1: run_assignment_queries().
 * Note: These inconsistencies were resolved prior to the install SQL script,
 * so technically, this is no longer needed.
 */
function set_correct_field_names($row) {
  // Correct the field names from the database table.
  if(!isset($row['coursename']) && isset($row['course_name'])) {
	$row['coursename'] = $row['course_name'];
  }
  if(isset($row['studentid']) && !isset($row['userid'])) {
	$row['userid'] = $row['studentid'];
  }
  if(!isset($row['course_name']) && isset($row['coursename'])) {
	$row['course_name'] = $row['coursename'];
  }
  if(isset($row['userid']) && !isset($row['studentid'])) {
	$row['studentid'] = $row['userid'];
  }
  return $row;
}

/**
 * Used to create the structure of single row for query result used in database logging. 
 * Used in step 2: get_notifications_array.
 */
function get_log_row_data($row, $date_interval, $time_now) {
  $log_row = array(
    'userid' => $row['userid'],
    'username' => $row['firstname'] . ' ' . $row['lastname'],
    'time_now' => $time_now,
    'time_due' => $row['time_due'],
    'interval' => $date_interval,
    'courseid' => $row['courseid'],
    'coursename' => $row['course_name'],
    'assignment' => $row['assignment'],
  );
  return $log_row;
}

/**
 * Used to create the structure of single row for email used in database logging. 
 * Used in step 3: send_emails.
 */
function get_log_email_data($userid, $username, $mailto, $notification_type) {
  $log_row = array(
    'userid' => $userid,
	'username' => implode(' ', $username),
	'mailto' => $mailto,
	'time_now' => time(),
	'notification_type_id' => $notification_type,
	'notification_type' => display_interval_type($notification_type),
  );
  return $log_row;
}

/* Date handling functions (used in step 2: get_notifications_array) */

/**
 * Utility function: Round a unixtime to the day, and return as string
 * (so DateInterval will work properly in counting days.)
 */
function get_date($unix_time) {
 return strftime('%Y-%m-%d', $unix_time);
}

/* Utility function: used to track whether an interval is valid. */
function get_valid_intervals() {
  return array(THREE_DAYS_BEFORE, ONE_DAY_AFTER, EIGHT_DAYS_AFTER);
}

/* Returns which of the date intervals the time matches, if any. */
function get_date_interval($time_due, $time_now = CUR_TIME) {
  $date_interval = INVALID_INTERVAL;
  // Do date interval comparison to the current year/month/day.
  $date_due = new DateTime(get_date($time_due));
  $cur_date = new DateTime(get_date($time_now));
  if(!is_object($date_due) || !is_object($cur_date)) {
    $date_interval = INVALID_INPUT;
  }
  $date_interval_raw = $date_due->diff($cur_date);
  //print_r($date_interval_raw);
  // Only works when it is in the same month.
  if($date_interval_raw->y == 0 && $date_interval_raw->m == 0) {
    $time_diff = $date_interval_raw->d;
    // Compare the dates to see which is later.
	// Note: The invert property of DateInterval appears to be useless.
	$is_after = FALSE;
    if($time_due < $time_now) {
      $is_overdue = TRUE;
	}
    if($time_diff == 3 && $is_overdue == FALSE) {
      $date_interval = THREE_DAYS_BEFORE;
    }
    else if($time_diff == 1 && $is_overdue == TRUE) {
      $date_interval = ONE_DAY_AFTER;
    }
    else if($time_diff == 8 && $is_overdue == TRUE) {
      $date_interval = EIGHT_DAYS_AFTER;
    }
  }
  return array('type' => $date_interval, 'is_overdue' => $is_overdue, 'raw' => $date_interval_raw);
}

/* Uses the get_date_interval() function to check whether this is a valid date interval. */
function is_valid_date_interval($unix_time, $cur_time = CUR_TIME) {
  $is_valid = FALSE;
  $date_interval = get_date_interval($unix_time, $cur_time);
  if($date_interval['type'] != INVALID_INTERVAL && $date_interval['type'] != INVALID_INPUT) {
    $is_valid = TRUE;
  }
  return $is_valid;
}

/* Used for emails and debugging: textual form of what type of interval these assignments are at. */
function display_interval_type($date_interval) {
  $interval = '';
  if(is_array($date_interval) && !empty($date_interval['type'])) {
    $date_interval = $date_interval['type'];
  }
  switch($date_interval) {
    case THREE_DAYS_BEFORE:
	  $interval = 'three days before';
	  break;
	case ONE_DAY_AFTER:
	  $interval = 'one day overdue';
	  break;
	case EIGHT_DAYS_AFTER:
	  $interval = 'eight days overdue';
	  break;
	case INVALID_INTERVAL:
	default:
	  $interval = 'invalid interval';
	  break;
  }
  return $interval;
}
	 
