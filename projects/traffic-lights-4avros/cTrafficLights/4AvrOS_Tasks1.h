
//----------------------------------------------------------------------------
// 4AvrOS
// Coop Scheduler
//
// by curtvm
//                                                                            
// Copyright (c) 2007,2008, Curt Van Maanen                                   
//                                                                            
// Permission to use, copy, modify, and/or distribute this software for any   
// purpose with or without fee is hereby granted, provided that the above     
// copyright notice and this permission notice appear in all copies.          
//                                                                            
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES   
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF           
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR    
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES     
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN      
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF    
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.             
//                                                                            
// Version 2007.10.06                                                         
//----------------------------------------------------------------------------
//  4AvrOS_Tasks1.h                                                            
//----------------------------------------------------------------------------

#ifndef _4AVROS_TASKS_H
#define _4AVROS_TASKS_H

//----------------------------------------------------------------------------
// DEFINE TASKS
/*----------------------------------------------------------------------------
  Set up your tasks here, up to 16 (__Task0 to __Task15) 
  More can be added if needed- change 4AvrOS.c (taskAddress variable 
  initialization) and defines below

      #define __Taskn                 NameOfTaskFunction
      #define __Taskn_priority        priority (0-7)
      #define __Taskn_init            RTR_ON or RTR_OFF
----------------------------------------------------------------------------*/

#define __Task0                 Day_Lights_Task
#define __Task0_priority        4
#define __Task0_init            RTR_OFF

#define __Task1                 Send_Status_Msg_Task
#define __Task1_priority        7
#define __Task1_init            RTR_ON

#define __Task2                 System_Clock_Task
#define __Task2_priority        0
#define __Task2_init            RTR_ON

#define __Task3                 EW_Veh_Sensor_Task
#define __Task3_priority        2
#define __Task3_init            RTR_OFF

#define __Task4                 NS_Veh_Sensor_Task
#define __Task4_priority        2
#define __Task4_init            RTR_OFF

#define __Task5                 Command_Task
#define __Task5_priority        5
#define __Task5_init            RTR_OFF

#define __Task6                 Four_Way_Stop_Task
#define __Task6_priority        4
#define __Task6_init            RTR_ON

#define __Task7                 Night_Lights_Task
#define __Task7_priority        4
#define __Task7_init            RTR_OFF

#define __Task8                 Mixed_Lights_Task
#define __Task8_priority        3
#define __Task8_init            RTR_OFF

#define __Task9                 Ped_Button_Task
#define __Task9_priority        2
#define __Task9_init            RTR_OFF

#define __Task10                Ped_Cross_Task
#define __Task10_priority       3
#define __Task10_init           RTR_OFF

//----------------------------------------------------------------------------
// DEFINE SEMAPHORES

/*----------------------------------------------------------------------------
  Create semaphores here, (up to 15)- there is no semaphore 0 (only 1-15)
  enum {do_not_remove,firstSema,nSemas};
  do_not_remove used to prevent using first one (0)
----------------------------------------------------------------------------*/

#define SEMAPHORES_ON  1                // 0=off, 1=on

enum {do_not_remove, semaSMAQueue, semaTime, semaTX, 
      semaAlarmTime, semaModeSwitch, nSemas};

//  semaSMAQueue for protecting access to status msg address queue

//----------------------------------------------------------------------------
// FUNCTION PROTOTYPES

void init_mystuff(void);              // used by 4AvrOS.c

//----------------------------------------------------------------------------
// DEFINES


/*----------------------------------------------------------------------------
   Set up function prototype, task number enum
   goto_label (gl) is passed to task, used for goto statement
   Need 2 macros, so 'name' is expanded first
----------------------------------------------------------------------------*/

#define SETUP_TASK2(name,num)   void name(uint8_t *gl);\
                                  enum{name ## NUMBER = num};
#define SETUP_TASK(name,num)  SETUP_TASK2(name,num)

/*----------------------------------------------------------------------------
  SETUP_TASK(taskname,num) for each task (sets up function prototype and enum)
  up to 16 tasks, can expand if more needed
  nTasks will automatically be set to the number of tasks
----------------------------------------------------------------------------*/

#define nTasks 0                         // set to no tasks defined

#ifdef __Task0                           // if was defined
  SETUP_TASK(__Task0,nTasks)             // setup the task (nTasks=0)
#undef nTasks                            // undefine nTasks
#define nTasks 1                         // set it to 1 now
#endif
    
#ifdef __Task1                           // if was defined
  SETUP_TASK(__Task1,nTasks)             // setup the task (nTasks=1)
#undef nTasks                            // undefine nTasks
#define nTasks 2                         // set it to 2 now
#endif

#ifdef __Task2                           // and so on
  SETUP_TASK(__Task2,nTasks)
#undef nTasks
#define nTasks 3
#endif

#ifdef __Task3
  SETUP_TASK(__Task3,nTasks)
#undef nTasks
#define nTasks 4
#endif

#ifdef __Task4
  SETUP_TASK(__Task4,nTasks)
#undef nTasks
#define nTasks 5
#endif

#ifdef __Task5
  SETUP_TASK(__Task5,nTasks)
#undef nTasks
#define nTasks 6
#endif

#ifdef __Task6
  SETUP_TASK(__Task6,nTasks)
#undef nTasks
#define nTasks 7
#endif

#ifdef __Task7
  SETUP_TASK(__Task7,nTasks)
#undef nTasks
#define nTasks 8
#endif

#ifdef __Task8
  SETUP_TASK(__Task8,nTasks)
#undef nTasks
#define nTasks 9
#endif

#ifdef __Task9
  SETUP_TASK(__Task9,nTasks)
#undef nTasks
#define nTasks 10
#endif

#ifdef __Task10
  SETUP_TASK(__Task10,nTasks)
#undef nTasks
#define nTasks 11
#endif

#ifdef __Task11
  SETUP_TASK(__Task11,nTasks)
#undef nTasks
#define nTasks 12
#endif

#ifdef __Task12
  SETUP_TASK(__Task12,nTasks)
#undef nTasks
#define nTasks 13
#endif

#ifdef __Task13
  SETUP_TASK(__Task13,nTasks)
#undef nTasks
#define nTasks 14
#endif

#ifdef __Task14
  SETUP_TASK(__Task14,nTasks)
#undef nTasks
#define nTasks 15
#endif

#ifdef __Task15
  SETUP_TASK(__Task15,nTasks)
#undef nTasks
#define nTasks 16
#endif

#if (!(nTasks))
#error No Tasks Defined in 4AvrOS_Tasks.h (you want a taskless os?)
#endif

//----------------------------------------------------------------------------

#endif // _4AVROS_TASKS_H



