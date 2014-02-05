<?php

/**
 * This is the main file which you would add to your crontab, to execute the Moodle assignment reminder cron.
 */
 
/* Includes required scripts. */
require_once './mdl_assignment_reminder_cron.inc';            // Main cron script functions
require_once './mdl_assignment_reminder_cron_constants.inc';  // Constants for core script functionality
require_once './mdl_assignment_reminder_cron_config.inc';     // User configuration constants
require_once './mdl_assignment_reminder_cron_emails.inc';     // Email-building functions
require_once './mdl_assignment_reminder_cron_tests.inc';      // Used in testing only. (Haven't tested test framework yet.)
// Go down a directory to include Drupal framework, for DB layer functions.
// CRUCIAL: This script requires a full Drupal installation in the directory below.
chdir('..');
require_once './includes/bootstrap.inc';                       // Drupal (for database layer)

/* Execute cron. */

// Drupal bootstrap - only database functions needed. 
// However, currently doesn't work without full.
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

// Switch to the Moodle database.
// CRUCIAL: This connection must be defined in the Drupal settings.php file for this script to work.
// Also, permissions must be granted to the appropriate database user.
db_set_active(DB_CONNECTION_NAME);

// Run the reminder cron script, or tests if in tests-only mode.
// CRUCIAL: This script will only work if the database tables have been created first.
if(TESTS_ONLY == FALSE) {
  $success = run_cron();
}
else {
  $success = run_tests();
}
