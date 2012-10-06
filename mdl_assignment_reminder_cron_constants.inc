<?php

/* Define constants crucial to code functioning. DO NOT CHANGE THESE. */

// Invalid input into date interval function.
define('INVALID_INPUT', -1);
// Invalid date interval for reminders.
define('INVALID_INTERVAL', 0);
// Valid date intervals for reminders.
define('THREE_DAYS_BEFORE', 1);
define('ONE_DAY_AFTER', 2);
define('EIGHT_DAYS_AFTER', 3);

// Current time (as Unix timestamp).
define('CUR_TIME', time());

// Types of time shift (+, -, none)
// This is only used in a test function.
define('SHIFT_NONE', 0);
define('SHIFT_POSITIVE', 1);
define('SHIFT_NEGATIVE', -1);

/* Constants used to control debug mode. */

// Types of debugging to print to screen. 
// Used in cron_debug() function, if in development mode.
define('DEBUG_NONE', 0);
define('DEBUG_QUERIES', 1);
define('DEBUG_DATA_BUILD', 2);
define('DEBUG_EMAIL_SEND', 3);
// Used in printing the email texts themselves.
define('DEBUG_EMAIL_TEXT', 4); 