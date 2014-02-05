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
define('EMAIL_TEST_ADDR', 'outofegypt+test@gmail.com');
// Who the emails should be cc'd to
define('EMAIL_CC_ADDR', 'techie.techmission+cc@gmail.com');
// define('EMAIL_CC_ADDR', 'outofegypt+test@gmail.com');


/* Used in debugging only. */

// Whether the script is in development (used to tweak various values)
define('IN_DEVELOPMENT', TRUE);
// Whether tests only should run. (Note tests may not currently work, since refactored.)
define('TESTS_ONLY', FALSE);
// Current mode for debugging. (Controls what, if anything, is output to screen.)
define('CUR_DEBUG_TYPE', DEBUG_DATA_BUILD);

// How many days to shift
define('SHIFT_DAYS_AMOUNT', 0);
// Current debugging shift type
define('CUR_DEBUG_SHIFT_TYPE', SHIFT_NEGATIVE);
