/*



*/
/*********************************************************/
/*             SCHWED-OSEK operating system              */
/*                                                       */
/*   (C) by Joerg Schwedes 2009, All rights reserved     */
/*********************************************************/

#include <os.h>

#define OS_KERNEL_VERSION 2000 /* kernel version 2.0.0.0 */
#if OS_KERNEL_VERSION != OS_VERSION
#error OS: Wrong OS.H version 
#endif
extern void OS_FuncInitHook(void);
extern __OS_CONST OS_TASK_PROFILE_STRUCT os_task_profile[];
extern __OS_CONST os_u8 os_num_tsk;

OS_TASK last_run;
os_u8 OS_nested_ctr;

extern OS_TIME os_timetable[];

volatile os_u16 OS_time_1ms;
#ifdef OS_USE_PREEMP_TASK_OPTION
OS_TASK preemp_mask;
os_flag os_preemptive_act;
#endif


typedef struct{
volatile OS_TASK tsk_ready; /* the task is ready to run (1) or suspended (0) */
         OS_TASK tsk_run;   /* this task is currently running */
} OS_TASK_STATES_STRUCT;

OS_TASK_STATES_STRUCT os_tsk;

#ifdef OS_USE_PREEMP_TASK_OPTION
static inline void OS_WRAPPER(void)
{
  /*
  OS_WRAPPER Service
  */
  os_u8 iw=0;

  last_run = os_tsk.tsk_run;
  do{
    if ((os_tsk.tsk_ready & preemp_mask) & (1<<iw))
    {
      os_tsk.tsk_run = ((OS_TASK)1<<iw); /* mark task as running */
      OS_EnaInt();/* wrap */
      os_task_profile[iw].tsk();   /* execute */
      iw=0;
      OS_DisInt();/* wrap */
    }
    else
    {
      iw++;
    }
  }
  while ((os_tsk.tsk_ready & preemp_mask) && (iw < os_num_tsk));
  os_tsk.tsk_run = last_run;
}
#endif
void SuspendAllInterrupts(void)
{
  /*
  Disable Int
  */
  if (OS_nested_ctr)
  {

  }
  else
  {
    OS_DisInt();
  }
  OS_nested_ctr++;
}
void ResumeAllInterrupts(void)
{
  /*
  Enable Int
  */
  if (--OS_nested_ctr == 0)
  {
    OS_EnaInt();
  }
  else
  {
    OS_nested_ctr--;
  }
}
os_u16 GetStopWatch(void)
{
  /*
   Get free running ms timer value
  */
  /* get time in milli seconds */


  os_u16 temp;
  os_u8 _t = OS_SaveInt();
  OS_DisInt();
  temp=OS_time_1ms;
  OS_RestoreInt(_t);
  return(temp);
}
void ActivateTask(os_u8 TASK_ID)
{
  /*
  ActivateTask(u8 TASK_ID)
  */
  os_u8 _t = OS_SaveInt();

  OS_DisInt();
  os_tsk.tsk_ready |= ((OS_TASK)1<<(TASK_ID));

  OS_RestoreInt(_t);
}
void TerminateTask(void)
{
  /*
  TerminateTask
  */
  /* find running task and terminate */
  os_u8 _t= OS_SaveInt();
  OS_DisInt();
  os_tsk.tsk_ready &= ~os_tsk.tsk_run; /* Task now suspended */
  os_tsk.tsk_run = 0; /* no task running */
  OS_RestoreInt(_t);
}
void OS_TIMEBASE(void)
{
  /*
  OS_TIMEBASE Service
  */
  os_u8 i;
  /* ms timer */
  OS_time_1ms++;

  for (i=0;i < os_num_tsk;i++)
  {
    if (os_timetable[i] > 0)
    {
      os_timetable[i]--;
      if (os_timetable[i] == 0)
      {
        os_tsk.tsk_ready |= ((OS_TASK)1<<i);
        os_timetable[i] = os_task_profile[i].tim_sched;
      }
    }
  }
#ifdef OS_USE_PREEMP_TASK_OPTION
  if (!os_preemptive_act)
  {
    if (os_tsk.tsk_ready & preemp_mask)
    {
      os_preemptive_act = 1;

      OS_WRAPPER();
      os_preemptive_act = 0;
    }
  }
#endif
}
__OS_NO_RETURN(main) /* DISPATCHER */
{
  /*
  Dispatcher for cooperative tasks
  */
  os_u8 i;

  OS_nested_ctr=1; /* prevent interrupt enable before OS starts */
  OS_FuncInitHook();
  OS_nested_ctr=0;
  /* load the timetable for auto scheduling */
  for (i=0;i < os_num_tsk;i++)
  {
    os_timetable[i] = os_task_profile[i].first_sched;
#ifdef OS_USE_PREEMP_TASK_OPTION
    preemp_mask |= ((OS_TASK)os_task_profile[i].preemp << i); /* mask for preemp tasks */
#endif
  }
  i=0;
  while (1)
  {
    /* Dispatcher for cooperative tasks */

    OS_DisInt();
    if (
          #ifdef OS_USE_PREEMP_TASK_OPTION
          (os_task_profile[i].preemp == 0) && /* is this a cooperative task ? */
          #endif
          (os_tsk.tsk_ready & (1<<i)) /* task in ready state ? */
       )
    {
      os_tsk.tsk_run |= ((OS_TASK)1<<i); /* mark task as running */
      OS_EnaInt();
      os_task_profile[i].tsk();   /* execute */
      i=0;

    }
    else
    {
      OS_EnaInt();
      if (++i >= os_num_tsk)
      {
        i=0;
      }
    }
  }
}
