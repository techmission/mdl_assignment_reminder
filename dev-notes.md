# Development Notes

These notes are somewhat out of date.

## Cron Script Logic

Basically the script builds an array that has information queried from the database about users, their courses, 
and the assignments in those courses, so that I can send out an email about them, replacing in the values.

This is then passed to the notification loop which collects for the purposes of emailing.
It calls out to a separate function that sends the emails that accepts the parameters:

email_type, data_array where data_array is the inner part of the notification array (by userid)

1.	Determine what should be the upcoming assignment notifications
a.	“SELECT * FROM view_upcoming_assignments_report”
b.	Iterate through the list of records
i.	For each record, see if the current date - time due = 3 days
ii.	If so, add to the three-day notification array
iii.	Else, check if the current date - time due = 1 day
iv.	If so, add to the one-day notification array
2.	Determine what should be the notifications for previous assignments
a.	“SELECT * FROM view_missing_assignments_ids_report”
b.	Iterate through the list of records
i.	For each record, see if the current date - time due = 3 days
ii.	If so, add to the three-day notification array
iii.	Else, check if the current date - time due = 1 day
iv.	If so, add to the one-day notification array
3.	Send the emails for each
a.	Iterate through the notification array, sending 4 separate emails for each user
b.	Track the email successes & failures in an array
4.	Record to the database which emails were sent successfully, and which failed

### Structure of notification array:

array(‘[userid’] => ‘info’ => (‘firstname’, ‘lastname’))
       THREE_DAYS_BEFORE => array(‘[courseid]’ =>array( ‘info’ => (name’), ‘[assignment_id]’ => (‘name’, ‘time_due_human’))),
       ONE_DAY_BEFORE => array(‘[courseid]’ =>array( ‘info’ => (name’), ‘[assignment_id]’ => (‘name’, ‘time_due_human’))),
       FIVE_DAYS_AFTER => array(‘[courseid]’ =>array( ‘info’ => (name’), ‘[assignment_id]’ => (‘name’, ‘time_due_human’))),
      TWELVE_DAYS_AFTER => array(‘[courseid]’ =>array( ‘info’ => (name’), ‘[assignment_id]’ => (‘name’, ‘time_due_human’))),
)