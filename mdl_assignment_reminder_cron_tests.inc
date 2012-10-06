<?php

/* Framework for tests. */
function run_tests() {
  run_date_interval_tests();
}

// Basic assertion: That a condition is TRUE.
function test_assert($condition, $suite_name, $test_case_name, $test_case_description, $is_echo = TRUE) {
  if($condition == TRUE) {
    $state = 'SUCCESS';
  }
  else {
    $state = 'FAILURE';
  }
  $assertion_value = $suite_name . ': ' . $test_case_name . ':' . $test_case_description . '. ' . $state . '. Expected: TRUE. Actual: FALSE.';
  if($is_echo == TRUE) {
    echo $assertion_value . '<br/>';
  }
  else {
    return $assertion_value;
  }
}

// Assert two values are equal.
function test_assert_equal($expected, $actual, $suite_name, $test_case_name, $test_case_description, $is_echo = TRUE) {
  if($expected == $actual) {
    $state = 'SUCCESS';
  }
  else {
    $state = 'FAILURE';
  }
  if(is_array($expected)) {
    $expected_str = '<pre>' . print_r($expected, TRUE) . '</pre>';
  }
  else {
    $expected_str = (string) $expected;
  }
  if(is_array($actual)) {
    $actual_str = '<pre>' . print_r($actual, TRUE) . '</pre>';
  }
  else {
    $actual_str = (string) $actual;
  }
  $assertion_value = $suite_name . ': ' . $test_case_name . ':' . $test_case_description . ' ' . $state . '. Expected: ' . $expected_str . ' Actual: ' . $actual_str . '.';
  if($is_echo == TRUE) {
    echo $assertion_value . '<br/>';
  }
  else {
    return $assertion_value;
  }
}

// Assert two values are equal.
function test_assert_not_equal($expected, $actual, $suite_name, $test_case_name, $test_case_description, $is_echo = TRUE) {
  if($expected != $actual) {
    $state = 'SUCCESS';
  }
  else {
    $state = 'FAILURE';
  }
  if(is_array($expected)) {
    $expected_str = '<pre>' . print_r($expected, TRUE) . '</pre>';
  }
  else {
    $expected_str = (string) $expected;
  }
  if(is_array($actual)) {
    $actual_str = '<pre>' . print_r($actual, TRUE) . '</pre>';
  }
  else {
    $actual_str = (string) $actual;
  }
  $assertion_value = $suite_name . ': ' . $test_case_name . ':' . $test_case_description . ' ' . $state . '. Expected: ' . $expected_str . ' Actual: ' . $actual_str . '.';
  if($is_echo == TRUE) {
    echo $assertion_value . '<br/>';
  }
  else {
    return $assertion_value;
  }
}

/* Utility functions used in tests. */

/* Shift a unix time. */
function time_shift_days($num_days = 0, $shift_type = SHIFT_NONE) {
  $time = time();
  if($shift_type == SHIFT_POSITIVE) {
    $time = $time + (60*60*24*$num_days);
  }
  else if($shift_type == SHIFT_NEGATIVE) {
    $time = $time - (60*60*$num_days);
  }
  return $time;
}

/* Take the number of rows iterated over and return a time. */
function get_iterative_unixtime($num_rows) {
  // Use a modulus operator to return numbers 0,1,2,3 in sequence for iteration.
  $interval = ($num_rows % 4);
  //echo $interval . '<br/>';
  $unix_time = time();
  // Return a matching time for each of the three cases, or else a non-modified, non-matching time.
  switch($interval) {
    case THREE_DAYS_BEFORE:
	  $unix_time = $unix_time - (60*60*24*3);
	  break;
	case ONE_DAY_AFTER:
	  $unix_time = $unix_time + (60*60*24*1);
	  break;
	case EIGHT_DAYS_AFTER:
	  $unix_time = $unix_time + (60*60*24*8);
	  break;
	case INVALID_INTERVAL:
	default:
	  $unix_time = $unix_time;
   }
   //echo $unix_time . '<br/>';
   return $unix_time;
}

/* Setup. */

function get_data_array_structure() {
  $row = array(
    'userid' => 1,
    'firstname' => 'John',
    'lastname' => 'Smith',
    'courseid' => 2,
    'coursename' => 'Test Course',
    'assignment' => 'Test Assignment',
    'time_due' => 1346734500,
  );

  $test_data_array =
    array($row['userid'] => 
      array('username' => 
	    array($row['firstname'], $row['lastname']),
         THREE_DAYS_BEFORE => 
	     array($row['courseid'] => 
		   array('coursename' => $row['coursename'], 
	             'assignments' => array($row['assignment']),
		    )
	    ),
       ONE_DAY_AFTER => array($row['courseid'] => 
		   array('coursename' => $row['coursename'], 
	             'assignments' => array($row['assignment']),
		    )
	    ),
        EIGHT_DAYS_AFTER => array($row['courseid'] => 
		   array('coursename' => $row['coursename'], 
	             'assignments' => array($row['assignment']),
		    )
	    ),
      ),
  );
  
  return $data_array;
}

/* Tests */

/* Tests of get_date_interval function. */

function run_date_interval_tests() {
  /* Test case 1: Time is three days before, and that is requested. */
  $unix_time = CUR_TIME - (60*60*24*3);
  $date_interval = get_date_interval($unix_time, CUR_TIME);
  test_assert_equal($date_interval['type'], THREE_DAYS_BEFORE, 'get_date_interval', 'Case 1', 'Time is three days before, and that is requested');

  /* Test case 2: Time is three days before, and that is not what is requested. */
  $unix_time = CUR_TIME - (60*60*24*3);
  $date_interval = get_date_interval($unix_time, CUR_TIME);
  test_assert_not_equal($date_interval['type'], ONE_DAY_AFTER, 'get_date_interval', 'Case 2', 'Time is three days before, and that is not what is requested');

  /* Test case 3: Time is one day after. */
  $unix_time = CUR_TIME+ (60*60*24*1);
  $date_interval = get_date_interval($unix_time, CUR_TIME);
  test_assert_equal($date_interval['type'], ONE_DAY_AFTER, 'get_date_interval', 'Case 3', 'Time is one day after');

  /* Test case 4: Time is eight days after. */
  $unix_time = CUR_TIME + (60*60*24*8);
  $date_interval = get_date_interval($unix_time, CUR_TIME);
  test_assert_equal($date_interval['type'], EIGHT_DAYS_AFTER, 'get_date_interval', 'Case 4', 'Time is eight days after');

  /* Test case 5: Time is nine days after (not a match). */
  $unix_time = CUR_TIME + (60*60*24*9);
  $date_interval = get_date_interval($unix_time, CUR_TIME);
  test_assert_not_equal($date_interval['type'], THREE_DAYS_BEFORE, 'get_date_interval', 'Case 5', 'Time is nine days after (not a match)');

  /* Test case 6: Time is not a valid time. */
  $unix_time = 'a';
  $date_interval = date_interval_match($unix_time, CUR_TIME);
  test_assert_equal($date_interval['type'], INVALID_INPUT, 'get_date_interval', 'Case 6', 'Time is not a valid time');
}