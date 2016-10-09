/* ********** SCHEDULER APP ********** */
/*                                     */
/* Contains the definitions for each   */
/* task so that they can be run by the */
/* Scheduler.                          */
/*                                     */
/* Author: Dan Vahle - Secaplius       */
/*                                     */
/* ********** May 10, 2016 *********** */

#ifndef SCHEDULER_H
#define SCHEDULER_H

/* List of task names and their bit values */
typedef enum
{
     NO_TASKS_TO_RUN = 0x00,
     LED_TASK = 0x01,
     BUTTON_TASK = 0x02,
     MODE_TASK = 0x04
}T_TASK_NAME;

/* *** ScheduleTask ****************** */
/*                                     */
/* Called by the tasks to schedule     */
/* their task functions to be executed */
/*                                     */
/* task_to_schedule - task name to be  */
/*                   scheduled to run  */
/* *********************************** */
void ScheduleTask(T_TASK_NAME task_to_schedule);

/* *** ScheduleTaskISR *************** */
/*                                     */
/* Called by the ISRs to schedule      */
/* their task functions to be executed */
/*                                     */
/* task_to_schedule - task name to be  */
/*    scheduled to run                 */
/* *********************************** */
void ScheduleTaskISR(T_TASK_NAME task_to_schedule);

/* *** ScheduleTime ****************** */
/*                                     */
/* Returns the system tick count       */
/*                                     */
/* *********************************** */
uint16_t ScheduleTime(void);



#endif /* SCHEDULER_H */
