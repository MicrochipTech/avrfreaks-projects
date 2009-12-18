/*



*/
/*********************************************************/
/*             SCHWED-OSEK operating system              */
/*                                                       */
/*   (C) by Joerg Schwedes 2009, All rights reserved     */
/*********************************************************/

#include <os.h>

/*********** OS TASK ************/

/* TASK ID definitions. Take care that IDs fit to TASK_PROFILE definitions below !.
TASK ID has the same order then the profile, starting with zero. */

#define TASK_1_ID     0 /* ID 0 /Prio 7 (15 if OS_USE_MAX_16TASKS is defined) */
#define TASK_2_ID     1
#define TASK_3_ID     2
#define TASK_IDLE_ID  3

/* Reset / Init Task for Applikation Init and TASK Activation */
/* Executed by OS Hook "OS_FuncInitHook" / defined in micro specific module (port) like ATemgaXX,c */

void TASK_RST(void)
{
  DDRB = 0xFF; /*PortB to Output*/
  PORTB = 0;



  ActivateTask(TASK_IDLE_ID); /* IDLE Task is now ready to run */
}
/* OSEK TASK definition starts here. */
TASK (TASK_1)
{
  if (PORTB & 1)
  {
    PORTB &= ~1;
  }
  else
  {
    PORTB |=1;
  }
  //PORTB_Bit0 = !PORTB_Bit0;

  TerminateTask();
}
TASK (TASK_2)
{
  if (PORTB & 2)
  {
    PORTB &= ~2;
  }
  else
  {
    PORTB |=2;
  }
  TerminateTask();
}
TASK (TASK_3)
{
  if (PORTB & 4)
  {
    PORTB &= ~4;
  }
  else
  {
    PORTB |=4;
  }
  TerminateTask();
}
TASK (TASK_IDLE)
{
  if (PORTB & 8)
  {
    PORTB &= ~8;
  }
  else
  {
    PORTB |=8;
  }
  ChainTask(TASK_IDLE_ID); /* idle Task chains itself (allowed because lowest priority task) */
}
/**************************************************************************************************/
/* TASK Profile:

Name:                             Name of the Task defined with TASK(name)
Auto-Schedule (16-Bit option):    Auto Scheduled, 0 = No, / 1-65535 = 1ms - 65535ms
or Auto-Schedule (8-Bit):         Auto Scheduled, 0 = No, / 1-255 = 1ms - 255ms
first activation:                 First activation after reset [ms]. Set only to 0 if task is not
                                  Auto-scheduled !.
Preemptive:                       Preemptive Task, 0 = COOPERATIVE, 1= PREEMPTIVE

 if OS_USE_PREEMP_TASK_OPTION is not defined, preemptive option is ignored. All tasks will be 
 scheduled cooperative */


/* Begin the TASK PROFILE configuration. A maximum of 8 tasks can be configured or 16 tasks
   if OS_USE_MAX_16TASKS is enabled. */
BEGIN_TASK_PROFILE

/*            Name      Auto-Schedule[ms]   first activation[ms]    Preemptive   */
TASK_PROFILE( TASK_1,          1,             1,                   1),  /* Prio 7(15) (highest), ID0 */
TASK_PROFILE( TASK_2,         500,             3,                  0),  /* Prio 6(14), ID1           */
TASK_PROFILE( TASK_3,        1000,            10,                  0),  /* Prio 5(13), ID2           */
TASK_PROFILE( TASK_IDLE,       0,             0,                   0)   /* Prio 4(12), ID3           */
/*                                                                   |                               */
/*                                                               No comma at last task position!!    */

END_TASK_PROFILE

/* The Priority of a task is given by the order of definitions in the TASK Profile.
Highest position has the highest Priority 7 or 15 (if 16 task option is configured).
Also the task in the highest position has the TASK_ID 0, the second position has 
TASK_ID 1 etc.
For runtime reason its recommended to define the preemptive tasks with highest priority
top in the TASK_PROFILE table */

