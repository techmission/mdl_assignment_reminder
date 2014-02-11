CREATE VIEW view_count_assignments_by_week_curterm AS SELECT `u`.`firstname`    AS `firstname`, 
       `u`.`lastname`     AS `lastname`, 
       `c`.`fullname`     AS `fullname`, 
       `cs`.`section`     AS `week_num`, 
       COUNT(`subs`.`id`) AS `num_subs` 
FROM   `mdl_course` `c` 
       JOIN `mdl_context` `ctx` 
         ON `ctx`.`instanceid` = `c`.`id` 
       JOIN `mdl_role_assignments` `ra` 
         ON `ra`.`contextid` = `ctx`.`id` 
       JOIN `mdl_user` `u` 
         ON `u`.`id` = `ra`.`userid` 
       JOIN `mdl_assign` `a` 
         ON `a`.`course` = `c`.`id`
       JOIN `view_assign_plugin_config` `pc`
         ON `a`.`id` = `pc`.`id`
       JOIN `mdl_course_modules` `cm` 
         ON `cm`.`instance` = `a`.`id` 
       JOIN `mdl_course_sections` `cs` 
         ON `cs`.`id` = `cm`.`section`
       LEFT JOIN `mdl_assign_submission` `subs` 
         ON `subs`.`userid` = `u`.`id` 
            AND `subs`.`assignment` = `a`.`id` 
        LEFT JOIN `mdl_assign_grades` `ag`
          ON `ag`.`assignment` = `a`.`id`
            AND `ag`.`userid` = `u`.`id`
WHERE    `ra`.`roleid` = 5 
         AND `cm`.`visible` = 1 
         AND `a`.`grade` > 0
         AND `ag`.`grade` > 0 
         AND `u`.`firstname` <> '' 
         AND `u`.`lastname` <> '' 
         AND NOT `c`.`fullname` LIKE '%demo%' 
         AND NOT `c`.`fullname` LIKE '%test%'
			AND `c`.`shortname` LIKE  '%spring%'
			AND `c`.`shortname` LIKE '%2014%'
GROUP  BY `cs`.`id`, 
          `u`.`id` 
ORDER  BY `u`.`lastname`, 
          `c`.`id`, 
          `cs`.`section`

CREATE VIEW view_assign_plugin_config AS SELECT DISTINCT `pc`.`assignment` AS `id`
FROM   `mdl_assign_plugin_config` `pc`
WHERE  ( ( ( `pc`.`plugin` = 'onlinetext' )
            OR ( `pc`.`plugin` = 'file' ) )
         AND ( `pc`.`subtype` = 'assignsubmission' )
         AND ( `pc`.`name` = 'enabled' )
         AND ( `pc`.`value` = 1 ) ) 

CREATE VIEW view_assignments_by_week_curterm AS SELECT `u`.`firstname`                      AS `firstname`,
       `u`.`lastname`                       AS `lastname`,
       `c`.`fullname`                       AS `fullname`,
       `cs`.`section`                       AS `week_num`,
       `a`.`name`                           AS `name`,
       `a`.`grade`                          AS `max_grade`,
       `ag`.`grade`                         AS `grade`,
       From_unixtime(`subs`.`timecreated`)  AS `from_unixtime(subs.timecreated)`
       ,
       From_unixtime(`subs`.`timemodified`) AS
       `from_unixtime(subs.timemodified)`
FROM   (((((((((`mdl_course` `c`
                JOIN `mdl_context` `ctx`
                  ON(( `ctx`.`instanceid` = `c`.`id` )))
               JOIN `mdl_role_assignments` `ra`
                 ON(( `ra`.`contextid` = `ctx`.`id` )))
              JOIN `mdl_user` `u`
                ON(( `u`.`id` = `ra`.`userid` )))
             JOIN `mdl_assign` `a`
               ON(( `a`.`course` = `c`.`id` )))
            JOIN `mdl_assign_grades` `ag`
              ON(( `ag`.`assignment` = `a`.`id` )))
           JOIN `view_assign_plugin_config` `pc`
             ON(( `a`.`id` = `pc`.`id` )))
          JOIN `mdl_course_modules` `cm`
            ON(( `cm`.`instance` = `a`.`id` )))
         JOIN `mdl_course_sections` `cs`
           ON(( `cs`.`id` = `cm`.`section` )))
        LEFT JOIN `mdl_assign_submission` `subs`
               ON(( ( `subs`.`userid` = `u`.`id` )
                    AND ( `subs`.`assignment` = `a`.`id` ) )))
WHERE  ( ( `ra`.`roleid` = 5 )
         AND ( `cm`.`visible` = 1 )
         AND ( `a`.`grade` > 0 )
         AND ( `ag`.`grade` > 0 )
         AND ( `u`.`firstname` <> '' )
         AND ( `u`.`lastname` <> '' )
         AND ( NOT(( `c`.`fullname` LIKE '%demo%' )) )
         AND ( NOT(( `c`.`fullname` LIKE '%test%' )) )
         AND ( ( Unix_timestamp(Now()) - ( ( ( ( 60 * 60 ) * 24 ) * 7 ) * 8 ) )
               <
                   `c`.`startdate` ) )
GROUP  BY `u`.`firstname`,
          `u`.`lastname`,
          `c`.`fullname`,
          `a`.`name`
ORDER  BY `u`.`lastname`,
          `c`.`id`,
          `cs`.`section` 

CREATE VIEW view_due_date_table AS
SELECT mdl_course.id                                  AS courseid,
       mdl_assign.id                              AS activity_id,
       'assignment'                                   AS activity_type,
       mdl_assign.duedate                         AS moodle_deadline,
       mdl_assign.duedate + ( 60 * 60 * 24 * 3 )  AS soft_deadline,
       mdl_assign.duedate + ( 60 * 60 * 24 * 7 )  AS soft_deadline_end,
       mdl_assign.duedate + ( 60 * 60 * 24 * 14 ) AS hard_deadline
FROM   mdl_assign
       JOIN mdl_course
         ON mdl_assign.course = mdl_course.id
UNION
SELECT mdl_course.id                              AS courseid,
       mdl_quiz.id                                AS activity_id,
       'quiz'                                     AS activity_type,
       mdl_quiz.timeclose                         AS moodle_deadline,
       mdl_quiz.timeclose + ( 60 * 60 * 24 * 3 )  AS soft_deadline,
       mdl_quiz.timeclose + ( 60 * 60 * 24 * 7 )  AS soft_deadline_end,
       mdl_quiz.timeclose + ( 60 * 60 * 24 * 14 ) AS hard_deadline
FROM   mdl_quiz
       JOIN mdl_course
         ON mdl_quiz.course = mdl_course.id
 
CREATE VIEW view_late_assignments_all AS
SELECT mdl_user.id                          AS userid,
       mdl_course.id                        AS courseid,
       Count(mdl_assign_submission.id) AS late_assignments
FROM   mdl_assign_submission
       JOIN mdl_user
         ON mdl_assign_submission.userid = mdl_user.id
       JOIN mdl_assign
         ON mdl_assign_submission.assignment = mdl_assign.id
       JOIN mdl_course
         ON mdl_assign.course = mdl_course.id
       JOIN view_assign_plugin_config
         ON view_assign_plugin_config.id = mdl_assign.id
WHERE  mdl_assign_submission.timemodified > mdl_assign.duedate
       AND mdl_assign.grade > 0
GROUP  BY mdl_course.id,
          mdl_user.id 

CREATE VIEW view_late_assignments_fivedays AS
SELECT mdl_user.id                          AS userid,
       mdl_course.id                        AS courseid,
       Count(mdl_assign_submission.id) AS late_assignments
FROM   mdl_assign_submission
       JOIN mdl_user
         ON mdl_assign_submission.userid = mdl_user.id
       JOIN mdl_assign
         ON mdl_assign_submission.assignment = mdl_assign.id
       JOIN mdl_course
         ON mdl_assign.course = mdl_course.id
       JOIN view_assign_plugin_config
         ON view_assign_plugin_config.id = mdl_assign.id
WHERE  mdl_assign_submission.timemodified > (mdl_assign.duedate + (60 * 60 * 24 * 5))
       AND mdl_assign.grade > 0
GROUP  BY mdl_course.id,
          mdl_user.id 

CREATE VIEW view_late_assignments_twelvedays AS
SELECT mdl_user.id                          AS userid,
       mdl_course.id                        AS courseid,
       Count(mdl_assign_submission.id) AS late_assignments
FROM   mdl_assign_submission
       JOIN mdl_user
         ON mdl_assign_submission.userid = mdl_user.id
       JOIN mdl_assign
         ON mdl_assign_submission.assignment = mdl_assign.id
       JOIN mdl_course
         ON mdl_assign.course = mdl_course.id
       JOIN view_assign_plugin_config
         ON view_assign_plugin_config.id = mdl_assign.id
WHERE  mdl_assign_submission.timemodified > (mdl_assign.duedate + (60 * 60 * 24 * 12))
       AND mdl_assign.grade > 0
GROUP  BY mdl_course.id,
          mdl_user.id

CREATE VIEW view_missed_assignments AS
SELECT `mdl_user`.`id`              AS `userid`,
       `mdl_course`.`id`            AS `courseid`,
       Count(`mdl_assign`.`id`) AS `missed_assignments`
FROM   (((((`mdl_assign`
            LEFT JOIN `mdl_assign_submission`
                   ON(( `mdl_assign`.`id` =
           `mdl_assign_submission`.`assignment` )))
           JOIN `mdl_course`
             ON(( `mdl_course`.`id` = `mdl_assign`.`course` )))
          JOIN `mdl_context`
            ON(( `mdl_context`.`instanceid` = `mdl_course`.`id` )))
         JOIN `mdl_role_assignments`
           ON(( `mdl_role_assignments`.`contextid` = `mdl_context`.`id` )))
        JOIN `mdl_user`
          ON(( `mdl_role_assignments`.`userid` = `mdl_user`.`id` )))
          JOIN view_assign_plugin_config ON mdl_assign.id = view_assign_plugin_config.id
WHERE  ( ( `mdl_role_assignments`.`roleid` = 5 )
         AND ( `mdl_assign`.`grade` > 0 )
         AND Isnull(`mdl_assign_submission`.`id`) )
GROUP  BY `mdl_course`.`id`,
          `mdl_user`.`id` 

CREATE VIEW view_missed_assignments_late AS 
SELECT `mdl_user`.`id`              AS `userid`,
       `mdl_course`.`id`            AS `courseid`,
       Count(`mdl_assign`.`id`) AS `missed_assignments`
FROM   (((((`mdl_assign`
            LEFT JOIN `mdl_assign_submission`
                   ON(( `mdl_assign`.`id` =
           `mdl_assign_submission`.`assignment` )))
           JOIN `mdl_course`
             ON(( `mdl_course`.`id` = `mdl_assign`.`course` )))
          JOIN `mdl_context`
            ON(( `mdl_context`.`instanceid` = `mdl_course`.`id` )))
         JOIN `mdl_role_assignments`
           ON(( `mdl_role_assignments`.`contextid` = `mdl_context`.`id` )))
        JOIN `mdl_user`
          ON(( `mdl_role_assignments`.`userid` = `mdl_user`.`id` )))
          JOIN view_assign_plugin_config ON mdl_assign.id = view_assign_plugin_config.id
WHERE  ( ( `mdl_role_assignments`.`roleid` = 5 )
         AND ( `mdl_assign`.`grade` > 0 )
         AND Isnull(`mdl_assign_submission`.`id`) )
         AND Unix_timestamp(Now()) > mdl_assign.duedate
GROUP  BY `mdl_course`.`id`,
          `mdl_user`.`id`

CREATE VIEW view_missed_assignments_late_fivedays AS
SELECT `mdl_user`.`id`              AS `userid`,
       `mdl_course`.`id`            AS `courseid`,
       Count(`mdl_assign`.`id`) AS `missed_assignments`
FROM   (((((`mdl_assign`
            LEFT JOIN `mdl_assign_submission`
                   ON(( `mdl_assign`.`id` =
           `mdl_assign_submission`.`assignment` )))
           JOIN `mdl_course`
             ON(( `mdl_course`.`id` = `mdl_assign`.`course` )))
          JOIN `mdl_context`
            ON(( `mdl_context`.`instanceid` = `mdl_course`.`id` )))
         JOIN `mdl_role_assignments`
           ON(( `mdl_role_assignments`.`contextid` = `mdl_context`.`id` )))
        JOIN `mdl_user`
          ON(( `mdl_role_assignments`.`userid` = `mdl_user`.`id` )))
          JOIN view_assign_plugin_config ON mdl_assign.id = view_assign_plugin_config.id
WHERE  ( ( `mdl_role_assignments`.`roleid` = 5 )
         AND ( `mdl_assign`.`grade` > 0 )
         AND Isnull(`mdl_assign_submission`.`id`) )
         AND Unix_timestamp(Now()) > mdl_assign.duedate
         AND (Unix_timestamp(Now()) - mdl_assign.duedate) > (60*60*24*5)
GROUP  BY `mdl_course`.`id`,
          `mdl_user`.`id`

CREATE VIEW view_missed_assignments_late_twelvedays AS
SELECT `mdl_user`.`id`              AS `userid`,
       `mdl_course`.`id`            AS `courseid`,
       Count(`mdl_assign`.`id`) AS `missed_assignments`
FROM   (((((`mdl_assign`
            LEFT JOIN `mdl_assign_submission`
                   ON(( `mdl_assign`.`id` =
           `mdl_assign_submission`.`assignment` )))
           JOIN `mdl_course`
             ON(( `mdl_course`.`id` = `mdl_assign`.`course` )))
          JOIN `mdl_context`
            ON(( `mdl_context`.`instanceid` = `mdl_course`.`id` )))
         JOIN `mdl_role_assignments`
           ON(( `mdl_role_assignments`.`contextid` = `mdl_context`.`id` )))
        JOIN `mdl_user`
          ON(( `mdl_role_assignments`.`userid` = `mdl_user`.`id` )))
          JOIN view_assign_plugin_config ON mdl_assign.id = view_assign_plugin_config.id
WHERE  ( ( `mdl_role_assignments`.`roleid` = 5 )
         AND ( `mdl_assign`.`grade` > 0 )
         AND Isnull(`mdl_assign_submission`.`id`) )
         AND Unix_timestamp(Now()) > mdl_assign.duedate
         AND (Unix_timestamp(Now()) - mdl_assign.duedate) > (60*60*24*12)
GROUP  BY `mdl_course`.`id`,
          `mdl_user`.`id`

CREATE VIEW view_missed_assignments_report AS
select view_missed_assignments.userid, view_missed_assignments.courseid, view_missed_assignments.missed_assignments as missed_assignments_all, 
view_missed_assignments_late.missed_assignments as missed_assignments_overdue,
view_missed_assignments_late_fivedays.missed_assignments as missed_assignments_overdue_fivedays, 
view_missed_assignments_late_twelvedays.missed_assignments as missed_assignments_overdue_twelvedays
from view_missed_assignments
left join view_missed_assignments_late on 
view_missed_assignments.userid = view_missed_assignments_late.userid and
view_missed_assignments.courseid = view_missed_assignments_late.courseid
left join view_missed_assignments_late_fivedays on 
view_missed_assignments.userid = view_missed_assignments_late_fivedays.userid and
view_missed_assignments.courseid = view_missed_assignments_late_fivedays.courseid
left join view_missed_assignments_late_twelvedays on 
view_missed_assignments.userid = view_missed_assignments_late_twelvedays.userid and
view_missed_assignments.courseid = view_missed_assignments_late_twelvedays.courseid 

CREATE VIEW view_missing_assignments_curterm_report AS
select * from view_missing_assignments_graded_curterm
union
select * from view_missing_assignments_ungraded_curterm
order by student_name asc

CREATE VIEW view_late_assignments_report AS 
select view_late_assignments_all.userid, view_late_assignments_all.courseid, view_late_assignments_all.late_assignments as late_assignments_all,
view_late_assignments_fivedays.late_assignments as late_assignments_fivedays, view_late_assignments_twelvedays.late_assignments as late_assignments_twelvedays
from view_late_assignments_all
left join view_late_assignments_fivedays on 
view_late_assignments_all.userid = view_late_assignments_fivedays.userid and
view_late_assignments_all.courseid = view_late_assignments_fivedays.courseid
left join view_late_assignments_twelvedays on 
view_late_assignments_all.userid = view_late_assignments_twelvedays.userid and
view_late_assignments_all.courseid = view_late_assignments_twelvedays.courseid 

CREATE VIEW view_missing_assignments_curterm_ids AS
SELECT `student`.`id`              AS `userid`,
       `student`.`firstname`       AS `firstname`,
       `student`.`lastname`        AS `lastname`,
       `mdl_course`.`id`           AS `courseid`,
       `mdl_assign_grades`.`grade` AS `grade`,
       `student`.`email`           AS `mailto`,
       `student`.`data`            AS `cc_email`,
       `mdl_course`.`fullname`     AS `coursename`,
       `mdl_assign`.`name`         AS `assignment`,
       `mdl_assign`.`duedate`      AS `time_due`
FROM   (((((((`mdl_assign_grades`
              JOIN `mdl_assign`
                ON(( `mdl_assign_grades`.`assignment` = `mdl_assign`.`id` )))
             JOIN `mdl_course`
               ON(( `mdl_course`.`id` = `mdl_assign`.`course` )))
            JOIN `view_student` `student`
              ON(( `student`.`id` = `mdl_assign_grades`.`userid` )))
           JOIN `mdl_enrol`
             ON(( `mdl_enrol`.`courseid` = `mdl_course`.`id` )))
          JOIN `mdl_user_enrolments` `ue`
            ON(( ( `ue`.`enrolid` = `mdl_enrol`.`id` )
                 AND ( `ue`.`userid` = `student`.`id` ) )))
         JOIN `view_assign_plugin_config`
           ON(( ( `view_assign_plugin_config`.`id` = `mdl_assign`.`id` )
                AND ( `mdl_assign_grades`.`userid` = `student`.`id` ) )))
        LEFT JOIN `mdl_assign_submission`
               ON(( ( `mdl_assign_submission`.`assignment` = `mdl_assign`.`id` )
                    AND ( `mdl_assign_submission`.`userid` =
                          `mdl_assign_grades`.`userid` ) )))
WHERE  ( ( ( Unix_timestamp(Now()) - ( ( ( ( 60 * 60 ) * 24 ) * 7 ) * 8 ) ) <
                    `mdl_course`.`startdate` )
         AND ( `mdl_assign_grades`.`grade` < 1 )
         AND ( `mdl_assign`.`grade` > 0 )
         AND Isnull(`mdl_assign_submission`.`id`)
         AND ( NOT(( `mdl_course`.`fullname` LIKE '%demo%' )) )
         AND ( NOT(( `mdl_course`.`fullname` LIKE '%test%' )) )
         AND ( `ue`.`status` = 0 ) )
ORDER  BY `student`.`lastname`,
          `student`.`firstname` 

CREATE VIEW view_missing_assignments_ungraded_curterm_ids AS
SELECT `student`.`id`                        AS `userid`,
       `student`.`firstname`                 AS `firstname`,
       `student`.`lastname`                  AS `lastname`,
       `student`.`email`                     AS `mailto`,
       `student`.`data`                      AS `cc_email`,
       `mdl_course`.`id`                     AS `courseid`,
       `mdl_course`.`fullname`               AS `coursename`,
       `mdl_assign`.`name`                   AS `assignment`,
       `mdl_assign`.`duedate`                AS `time_due`,
       From_unixtime(`mdl_assign`.`duedate`) AS `time_due_human`
FROM   (((((((((`mdl_assign`
                JOIN `mdl_course`
                  ON(( `mdl_course`.`id` = `mdl_assign`.`course` )))
               JOIN `mdl_context`
                 ON(( `mdl_context`.`instanceid` = `mdl_course`.`id` )))
              JOIN `mdl_role_assignments`
                ON(( `mdl_role_assignments`.`contextid` = `mdl_context`.`id` )))
             JOIN `view_student` `student`
               ON(( `mdl_role_assignments`.`userid` = `student`.`id` )))
            JOIN `mdl_enrol`
              ON(( `mdl_enrol`.`courseid` = `mdl_course`.`id` )))
           JOIN `mdl_user_enrolments` `ue`
             ON(( ( `ue`.`enrolid` = `mdl_enrol`.`id` )
                  AND ( `ue`.`userid` = `student`.`id` ) )))
          JOIN `view_assign_plugin_config`
            ON(( `view_assign_plugin_config`.`id` = `mdl_assign`.`id` )))
         LEFT JOIN `mdl_assign_submission`
                ON(( ( `mdl_assign_submission`.`assignment` =
                     `mdl_assign`.`id` )
                     AND ( `student`.`id` =
                  `mdl_assign_submission`.`userid` ) )))
        LEFT JOIN `mdl_assign_grades`
               ON(( ( `mdl_assign_grades`.`assignment` = `mdl_assign`.`id` )
                    AND ( `mdl_assign_grades`.`userid` = `student`.`id` ) )))
WHERE  ( Isnull(`mdl_assign_submission`.`id`)
         AND Isnull(`mdl_assign_grades`.`id`)
         AND ( `mdl_assign`.`grade` > 0 )
         AND ( Unix_timestamp(Now()) > `mdl_assign`.`duedate` )
         AND ( NOT(( `mdl_course`.`fullname` LIKE '%demo%' )) )
         AND ( NOT(( `mdl_course`.`fullname` LIKE '%test%' )) )
         AND ( NOT(( `mdl_course`.`fullname` LIKE '%CVC 101%' )) )
         AND ( ( Unix_timestamp(Now()) - ( ( ( ( 60 * 60 ) * 24 ) * 7 ) * 8 ) )
               <
                   `mdl_course`.`startdate` )
         AND ( `mdl_role_assignments`.`roleid` = 5 ) )
ORDER  BY `student`.`lastname`,
          `student`.`firstname` 

