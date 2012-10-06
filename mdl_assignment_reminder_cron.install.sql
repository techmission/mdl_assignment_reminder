/*
 * On the next line, put in the name of your Moodle database.
 * Ensure that the database user running this script has full permissions on that database.
 * This script only needs to be executed once, at installation.
 */
USE DATABASE `cvedu_moodle`;

CREATE TABLE `cron_run_count` (
	`crid` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
	`time_complete` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	`total_queried` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	`total_queried_upcoming` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	`total_queried_missing` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	`total_to_notify` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	`total_no_notifications` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	`total_sent_success` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	`total_sent_failure` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (`crid`)
)
COLLATE='utf8_general_ci'
ENGINE=MyISAM
AUTO_INCREMENT=0;

CREATE TABLE `cron_run_item` (
	`criid` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
	`crid` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	`userid` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	`username` VARCHAR(255) NOT NULL DEFAULT '',
	`mailto` VARCHAR(255) NOT NULL DEFAULT '',
	`time_sent` INT(10) UNSIGNED NULL DEFAULT 0,
	`notification_type` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (`criid`),
	INDEX `crid` (`crid`),
	INDEX `userid` (`userid`)
)
COLLATE='utf8_general_ci'
ENGINE=MyISAM
AUTO_INCREMENT=0;

create or replace view upcoming_assignments_report as
select `student`.`id` AS `studentid`,`student`.`firstname` AS `firstname`,`student`.`lastname` AS `lastname`,`student`.`email` as `mailto`,`mdl_course`.`id` AS `courseid`,`mdl_course`.`fullname` AS `course_name`,`mdl_assignment`.`name` AS `assignment`,`mdl_assignment`.`timedue` AS `time_due`,date_format(from_unixtime(`mdl_assignment`.`timedue`),'%b. %e, %Y at %l:%i %p') AS `time_due_human`,(to_days(from_unixtime(`mdl_assignment`.`timedue`)) - to_days(now())) AS `days_till_due` from (((((`mdl_assignment` left join `mdl_assignment_submissions` on((`mdl_assignment_submissions`.`assignment` = `mdl_assignment`.`id`))) join `mdl_course` on((`mdl_course`.`id` = `mdl_assignment`.`course`))) join `mdl_context` on((`mdl_context`.`instanceid` = `mdl_course`.`id`))) join `mdl_role_assignments` on((`mdl_role_assignments`.`contextid` = `mdl_context`.`id`))) join `mdl_user` `student` on((`mdl_role_assignments`.`userid` = `student`.`id`))) where (isnull(`mdl_assignment_submissions`.`id`) and (`mdl_assignment`.`assignmenttype` <> 'offline') and (`mdl_assignment`.`grade` > 0) and (not((`mdl_course`.`fullname` like '%demo%'))) and (not((`mdl_course`.`fullname` like '%test%'))) and ((unix_timestamp(now()) - ((((60 * 60) * 24) * 7) * 8)) < `mdl_course`.`startdate`) and (`mdl_role_assignments`.`roleid` = 5)); 

create or replace view view_missing_assignments_graded_curterm_ids as
select `student`.`id` AS `userid`,`student`.`firstname` AS `firstname`,`student`.`lastname` AS `lastname`,`mdl_course`.`id` AS `courseid`,`student`.`email` as `mailto`, `mdl_course`.`fullname` AS `coursename`,`mdl_assignment`.`name` AS `assignment`,`mdl_assignment`.`timedue` AS `time_due` from ((((`mdl_assignment_submissions` join `mdl_assignment` on((`mdl_assignment_submissions`.`assignment` = `mdl_assignment`.`id`))) join `mdl_course` on((`mdl_course`.`id` = `mdl_assignment`.`course`))) join `mdl_user` `student` on((`student`.`id` = `mdl_assignment_submissions`.`userid`))) join `mdl_user` `professor` on((`professor`.`id` = `mdl_assignment_submissions`.`teacher`))) where ((`mdl_assignment_submissions`.`numfiles` = 0) and ((unix_timestamp(now()) - ((((60 * 60) * 24) * 7) * 8)) < `mdl_course`.`startdate`) and isnull(`mdl_assignment_submissions`.`data1`) and (`mdl_assignment`.`assignmenttype` <> 'offline') and (`mdl_assignment_submissions`.`grade` < 1) and (`mdl_assignment`.`grade` > 0) and (not((`mdl_course`.`fullname` like '%demo%'))) and (not((`mdl_course`.`fullname` like '%test%')))) order by `student`.`lastname`,`student`.`firstname`; 

create or replace view view_missing_assignments_ungraded_curterm_ids as
select `student`.`id` AS `userid`,`student`.`firstname` AS `firstname`,`student`.`lastname` AS `lastname`,`student`.`email` as `mailto`, `mdl_course`.`id` AS `courseid`,`mdl_course`.`fullname` AS `coursename`,`mdl_assignment`.`name` AS `assignment`,`mdl_assignment`.`timedue` AS `time_due` from (((((`mdl_assignment` left join `mdl_assignment_submissions` on((`mdl_assignment_submissions`.`assignment` = `mdl_assignment`.`id`))) join `mdl_course` on((`mdl_course`.`id` = `mdl_assignment`.`course`))) join `mdl_context` on((`mdl_context`.`instanceid` = `mdl_course`.`id`))) join `mdl_role_assignments` on((`mdl_role_assignments`.`contextid` = `mdl_context`.`id`))) join `mdl_user` `student` on((`mdl_role_assignments`.`userid` = `student`.`id`))) where (isnull(`mdl_assignment_submissions`.`id`) and (`mdl_assignment`.`assignmenttype` <> 'offline') and (`mdl_assignment`.`grade` > 0) and (unix_timestamp(now()) > `mdl_assignment`.`timedue`) and (not((`mdl_course`.`fullname` like '%demo%'))) and (not((`mdl_course`.`fullname` like '%test%'))) and ((unix_timestamp(now()) - ((((60 * 60) * 24) * 7) * 8)) < `mdl_course`.`startdate`) and (`mdl_role_assignments`.`roleid` = 5)) order by `student`.`lastname`,`student`.`firstname`; 

create or replace view view_missing_assignments_ids_report as 
select `view_missing_assignments_ungraded_curterm_ids`.`userid` AS `studentid`,`view_missing_assignments_ungraded_curterm_ids`.`firstname` AS `firstname`,`view_missing_assignments_ungraded_curterm_ids`.`lastname` AS `lastname`,`view_missing_assignments_ungraded_curterm_ids`.`mailto` AS `mailto`,`view_missing_assignments_ungraded_curterm_ids`.`courseid` AS `courseid`,`view_missing_assignments_ungraded_curterm_ids`.`coursename` AS `course_name`,`view_missing_assignments_ungraded_curterm_ids`.`assignment` AS `assignment`,`view_missing_assignments_ungraded_curterm_ids`.`time_due` AS `time_due` from `view_missing_assignments_ungraded_curterm_ids` union select `view_missing_assignments_graded_curterm_ids`.`userid` AS `studentid`,`view_missing_assignments_graded_curterm_ids`.`firstname` AS `firstname`,`view_missing_assignments_graded_curterm_ids`.`lastname` AS `lastname`,`view_missing_assignments_graded_curterm_ids`.`mailto` AS `mailto`,`view_missing_assignments_graded_curterm_ids`.`courseid` AS `courseid`,`view_missing_assignments_graded_curterm_ids`.`coursename` AS `course_name`,`view_missing_assignments_graded_curterm_ids`.`assignment` AS `assignment`,`view_missing_assignments_graded_curterm_ids`.`time_due` AS `time_due` from `view_missing_assignments_graded_curterm_ids`; 