# Moodle Assignment Reminder Cron (mdl_assignment_reminder)

This is a cron script for Moodle that notifies users of upcoming assignments in 3 days,
as well as late assignments that are either 1 or 8 days overdue.

## Download

This project's canonical version is available via [Github](https://github.com/techmission/mdl_assignment_reminder)

## Release Status & Bug Reporting

This is currently alpha quality code. It sends out emails, but is not tested in all use cases.

No guarantees are made as to how this will affect your Moodle system, so use on development only, 
until you are satisfied with the results.

## Prerequisites

* A Drupal installation above the root of this directory (version <7). 
* A Moodle database in which to install the tables in the script's SQL file
* Courses with assignments, and users enrolled in courses (or the queries will return no results)

## Installation

1. Clone the repository from Github, and move into a subdirectory of a Drupal installation. Make all files readable by the Web server.
2. Add a connection string to the Drupal settings.php file for Moodle database. Ensure permissions are correct.
3. Run the SQL install script from the command line.
4. Configure the script as described in Configuration, below.
5. Add mdl_assignment_reminder_cron.php to your cron tab. Set to run no more & no less than once a day.

## Configuration

Currently, all values are adjusted in the mdl_assignment_reminder_cron_config.inc.

The code comments document what they do.

Some configuration is required in order for the script to behave as expected.

If desired, you may modify the email texts in mdl_assignment_reminder_cron_emails.inc to suit your institution's needs.

## Usage

Once installed, the script should run by itself.

You can check the cron_run_count and cron_run_item tables for results.

Also, if you have EMAIL_CC_ON set to TRUE, you will receive the notifications at the configured EMAIL_CC_ADDR.

## Known Issues

Most known issues are documented in the code.

The main one I know of right now is that the script will throw many PHP errors if the database queries fail, or return no results.

## Further Development

v.1:

* Ensure that script behaves as expected (sending of emails at proper times).

v.2:

* Modify script to only be dependent on Moodle.
* Modify script to run off Moodle DB layer functions.

v.3:

* Modify script to be a Moodle plugin for 2.x, in the /local directory.

v.4:

* Fix tests to be Moodle tests

Note that I would appreciate if someone else could work on v.2 & v.3 & v.4, since I don't have experience with Moodle plugins or DB layer.

## Licensing & Collaboration

This project is licensed under the [GNU GPL v2](http://www.gnu.org/licenses/gpl-2.0.html).

You are free to redistribute and modify as you wish, as long as you maintain a readme file with the original credit (including email), as below.

## Credit & Contact

Evan Donovan, on behalf of [TechMission](http://www.techmission.org) and [City Vision College](http://www.cityvision.edu)
Email: firstname at techmission dot org

You may contact him with any questions. Bug reports and feature requests should be done via Github.
