/*



*/
/*********************************************************/
/*             SCHWED-OSEK operating system              */
/*                                                       */
/*   (C) by Joerg Schwedes 2009, All rights reserved     */
/*********************************************************/
#ifndef OS_H
#define OS_H

#include <os_cnf.h>
#include <ostypes.h>

/* SCHWED-OSEK version */
#define OS_VERSION 2000 /* OS version 2.0.0.0 */

/**************************************************************/
/* RTOS TIMEBASE and WRAPPER                                  */
extern void OS_TIMEBASE(void);

/**************************************************************/
/* OS API                                                     */

/* Sevices for Interrupt handling                             */
extern void SuspendAllInterrupts(void);
extern void ResumeAllInterrupts(void);

/* Get time (ms) service                                      */
extern os_u16 GetStopWatch(void); /* get Millisecond  timer      */

/* task services                                              */

extern void ActivateTask(os_u8 TASK_ID);
extern void TerminateTask(void);

#define ChainTask(TASK_ID)  {TerminateTask();ActivateTask(TASK_ID);}

#define Schedule() /* shit happens :) */

/* Task Definition ********************************************/
#define TASK(name) void os_task_ ## name(void)

/* Task Profile:                                                
task_name:   Name of the Task defined with TASK(name)           
tim_sched (16Bit):  Auto scheduled, 0 = No, 1-65535 = 1ms - 65535ms
tim_sched (8Bit):   Auto scheduled, 0 = No, 1-255 = 1ms - 255ms

preemp:      Preemptive Task, 0 = COOPERATIVE, 1= PREEMPTIVE 
*/

#ifdef OS_USE_16BIT_TIMETABLE_OPTION
typedef os_u16 OS_TIME; 
#else
typedef os_u8 OS_TIME;
#endif
#ifdef OS_USE_MAX_16TASKS_OPTION
typedef os_u16 OS_TASK;
#else
/* 8 tasks */
typedef os_u8 OS_TASK;
#endif
typedef struct {
         void (* tsk)(void);
          OS_TIME tim_sched;
          OS_TIME first_sched;
          os_u8 preemp;
} OS_TASK_PROFILE_STRUCT;


#define TASK_PROFILE(name, tim_sched,first_sched, preemp)    \
{ os_task_ ## name, tim_sched, first_sched, preemp }

#define BEGIN_TASK_PROFILE \
__OS_CONST OS_TASK_PROFILE_STRUCT os_task_profile[]={


#define END_TASK_PROFILE };__OS_CONST os_u8 os_num_tsk = \
 (sizeof(os_task_profile)/sizeof(os_task_profile[0]));         \
OS_TIME os_timetable[sizeof(os_task_profile)/sizeof(os_task_profile[0])];

/* make SCHWED-OSEK compatible to older SCHWED-OS */

#define OS_GetTimeMs()  GetStopWatch()
#define OS_SuspendInt() SuspendAllInterrupts()
#define OS_ResumeInt()  ResumeAllInterrupts()
#endif

