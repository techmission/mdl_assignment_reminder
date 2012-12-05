<?php

/* Code specifically to build emails. */
/* @todo: Make this more easily configurable? */

/* Helper function to send an individual notification email. */
function send_email($username, $mailto, $notification_type, $notification_data) {
  $email_vars = preprocess_email_vars($username, $notification_type, $notification_data);
  $message_body = get_message_body($email_vars);
  $message_subject = get_message_subject($email_vars, $notification_type);
  $headers = 'From: ' . COLLEGE_NAME . ' Student Services <' . COLLEGE_EMAIL .'>' . "\r\n";
  $headers .= 'Reply-To: ' . COLLEGE_EMAIL . "\r\n";
  $headers .= 'X-Mailer: PHP/' . phpversion() . "\r\n";
  $headers .= "MIME-Version: 1.0\r\n";
  $headers .= "Content-Type: text/html; charset=ISO-8859-1\r\n";
  $success = FALSE;
  // Override mailto in testing, if it actually is going to send emails.
  if(IN_DEVELOPMENT == TRUE && SEND_EMAILS == TRUE) {
    $mailto = EMAIL_TEST_ADDR;
  }
  // If set to cc someone, add in the header for that.
  if(EMAIL_CC_ON == TRUE) {
    $cc_addr = EMAIL_CC_ADDR;
	$headers .= 'Cc: ' . $cc_addr . "\r\n";
  }
  // Send the email itself.
  if(SEND_EMAILS == TRUE) {
    $success = mail($mailto, $message_subject, $message_body, $headers);
  }
  else {
    $success = TRUE;
  }
  if(IN_DEVELOPMENT == TRUE && CUR_DEBUG_TYPE == DEBUG_EMAIL_TEXT) {
    echo '<pre>' . print_r(array('mailto' => $mailto, 'message_subject' => $message_subject, 'message_body' => $message_body, 'headers' => $headers), TRUE) . '</pre>';
  }
  return $success;
}

/* Functions specifically for formatting of emails. */

// Main preprocess function: builds variables needed in email subject and body.
function preprocess_email_vars($username, $notification_type, $notification_data) {
  $email_vars = array();
  // Use the user's first name in greeting.
  $email_vars['salutation'] = 'Dear ' . $username[0] . ',';
  // Set the notification message
  $email_vars['notification_message'] = get_notification_message($notification_type);
  // Set the late assignment policy notice.
  $email_vars['late_assignment_policy'] = get_late_assignment_policy();
  // Set the signature line.
  $email_vars['signature'] = get_signature();
  // Build an array for each course that can be used to indicate how many assignments are due in each.
  $email_vars['assignments_count'] = 0;
  $email_vars['assignment_list_raw'] = array();
  foreach($notification_data as $course_id => $course_data) {
    $email_vars['assignment_list_raw'][$course_id]['coursename'] = $course_data['coursename'];
	$assignment_list = '<ul>';
	foreach($course_data['assignments'] as $assignment) {
	  $assignment = abbrev($assignment, 320);
	  $assignment_list .= '<li>' . $assignment . '</li>';
	  // Increment the count used in the subject line to indicate how many assignments there are.
	  $email_vars['assignments_count']++;
	}
        $assignment_list .= '</ul>';
	$email_vars['assignment_list_raw'][$course_id]['assignments'] = $assignment_list;
  }
  // Format this array into a series of lists, grouped by course.
  $assignment_list_full = '<h3>Assignments Due:</h3>';
  foreach($email_vars['assignment_list_raw'] as $course_id => $course_data) {
    $assignment_list_full .= '<h4>' . $course_data['coursename'] . ':</h4>';
	$assignment_list_full .= $course_data['assignments']; 
  }
  $email_vars['assignment_list'] = $assignment_list_full;
  return $email_vars;
}

function abbrev($text, $length) {
  if(strlen($text) > $length) {
    $text = substr($text, 0, $length) . '...';
  }
  return $text;
}

// Concatenates preprocessed variables into the email body, and returns as string.
function get_message_body($email_vars) {
  $message_body = '<html><body>';
  $message_body .= '<p>' . $email_vars['salutation'] . "</p>";
  $message_body .= $email_vars['notification_message'];
  $message_body .= $email_vars['assignment_list'];
  $message_body .= $email_vars['late_assignment_policy'];
  $message_body .= $email_vars['signature'];
  $message_body .= '</body></html>';
  return $message_body;
}

// Returns a string of the email subject, using the email variables.
function get_message_subject($email_vars, $notification_type) {
  $message_subject = '';
  $item_name = ($email_vars['assignments_count'] == 1) ? 'assignment' : 'assignments';
  $being_verb = ($email_vars['assignments_count'] == 1) ? 'is' : 'are';
  switch($notification_type) {
    case THREE_DAYS_BEFORE:
	  $message_subject = 'You have ' . $email_vars['assignments_count'] . ' ' . $item_name . ' due in 3 days for ' . COLLEGE_NAME;
	  break;
	case ONE_DAY_AFTER:
	  $message_subject = 'You have ' . $email_vars['assignments_count'] . ' ' . $item_name . ' that ' . $being_verb . ' 1 day overdue for ' . COLLEGE_NAME;
	  break;
	case EIGHT_DAYS_AFTER:
	  $message_subject = 'You have ' . $email_vars['assignments_count'] . ' ' . $item_name . ' that ' . $being_verb . ' 8 days overdue for ' . COLLEGE_NAME;
	  break;
  }
  return $message_subject;
}

// Return the notification message based on the type of notification, as string.
function get_notification_message($notification_type) {
  $notification_message = '';
  $notification_message .= '<p>You have not yet submitted the assignments listed below.</p>';
  if($notification_type == THREE_DAYS_BEFORE) {
    $notification_message .= '<p>It is currently ' . display_interval_type($notification_type) . ' these assignments are due.</p>';
  }
  else {
    $notification_message .= '<p>These assignments are currently <strong>' . display_interval_type($notification_type) . '</strong>.</p>';
  }
  switch($notification_type) {
    case THREE_DAYS_BEFORE:
	  $notification_message .= '<p>If you do not submit the assignments within three days, they will be considered late.</p>';
	  $notification_message .= '<p>If you do not submit them within one week, they will be marked down one letter grade,';
	  $notification_message .= 'as per our late assignment policy which is given in full at the bottom of this email.</p>';
	  break;
	case ONE_DAY_AFTER:
	  $notification_message .= '<p>If you do not submit the assignments before the <strong>end of this week</strong>, they will be marked down <strong>one letter grade</strong>.</p>';
	  $notification_message .= '<p>If you do not submit them within 2 weeks, they will be marked down two letter grades.</p>';
	  $notification_message .= '<p>For more details, see our late assignment policy which is given in full at the bottom of this email.</p>';
	  break;
	case EIGHT_DAYS_AFTER:
	  $notification_message .= '<p>They have already been marked down one letter grade.</p>';
	  $notification_message .= '<p>If you do not submit them by the <strong>end of this week</strong>, they will be marked down <strong>two letter grades</strong>, if submitted.</p>';
	  $notification_message .= '<p>For more details, see our late assignment policy which is given in full at the bottom of this email.</p>';
	  break;
  }
  return $notification_message;
}

// Return the late assignment policy, as string.
 function get_late_assignment_policy() {
  // Use heredoc syntax for simplicity.
  $policy = <<<EOT
<h3>Policy on Late Assignments:</h3>
  
<ol><li>Each assignment is due by the end of the day on Sunday
in the week it was assigned.</li>
<li>Assignments submitted more than 1 week late (after the following Sunday)
will lose 1 letter grade (i.e. "A" becomes a "B")</li>
<li>Assignments submitted more than 2 weeks late will
lose 2 letter grades (i.e. "A" becomes a "C")</li>
<li>All assignments and quizzes must be submitted by
the week after the term ends or they will receive a failing grade.</li>
<li>Extensions: professors may grant an extension 
if the student has a prolonged sickness or major family crisis. 
The length of the extension is up to the professor's discretion.</li></ol>

<p>This policy is also viewable on the college Web site, 
on our <a href="http://www.cityvision.edu/cms/cv/academic-policies">
Academic Policies page</a>.</p>

<h2>IMPORTANT NOTICE FOR FINANCIAL AID STUDENTS (Pell or Federal Loans)</h2>
<ol><li>You MUST complete all assignments for the first two weeks to receive any financial aid.</li>
<li>You MUST be completely up to date and have submitted all assignments and completed all quizzes through the end of WEEK 5 or you will only receive a partial financial aid award.</li></ol>
EOT;
  return $policy;
}

// Return the email's signature lines, as string.
function get_signature() {
  // Use heredoc syntax for simplicity.
  $signature = <<<EOT
<p>If you have any questions or need assistance, 
please feel free to contact the college support team.</p>
<p>Thanks,<br/>
City Vision College Student Services<br/>
(617) 282-9798 x116<br/>
info@cityvision.edu</p>  
EOT;
  return $signature;
}
