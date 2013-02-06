<?php

/* Constants for configuration of script. CHANGE THESE BEFORE RUNNING. */

/* Used in connection to database. */

// SET THE FOLLOWING TO YOUR DB CONNECTION STRING NAME IN DRUPAL
define('DB_CONNECTION_NAME', 'cvedu_moodle');

/* Used for configuring emails. */

// SET THE FOLLOWING TO YOUR COLLEGE'S NAME AND EMAIL
define('COLLEGE_NAME', 'City Vision College');
define('COLLEGE_EMAIL', 'info@cityvision.edu');

// Whether the emails should be sent. 
// SET TO TRUE WHEN IN PRODUCTION.
define('SEND_EMAILS', TRUE);
// Whether the emails should be cc'd to someone (used for first few weeks, in development)
define('EMAIL_CC_ON', TRUE);
// Who should receive emails in test mode.
define('EMAIL_TEST_ADDR', 'evan@techmission.org');
// Who the emails should be cc'd to
define('EMAIL_CC_ADDR', 'outofegypt@gmail.com');


/* Used in debugging only. */

// Whether the script is in development (used to tweak various values)
define('IN_DEVELOPMENT', FALSE);
// Whether tests only should run. (Note tests may not currently work, since refactored.)
define('TESTS_ONLY', FALSE);
// Current mode for debugging. (Controls what, if anything, is output to screen.)
define('CUR_DEBUG_TYPE', DEBUG_EMAIL_TEXT);

// How many days to shift.
// On 10/5, in our install, 1 day got 3 days before notices, 
// 5 days got 1 day overdue, 12 days got 8 days overdue, 
// for all records currently in the tables.
define('SHIFT_DAYS_AMOUNT', 0);
