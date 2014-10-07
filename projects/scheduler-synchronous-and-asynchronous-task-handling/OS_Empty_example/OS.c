/********************************************************************
*                        Microprocessor                             *
*                elr(a)kea.dk, October 7, 2014                      *   
*    example of empty project using the standard KEA scheduler      *
********************************************************************/

// include files
#include    <avr/io.h>
#include    <avr/interrupt.h>
#include    <stdio.h>
#include    "OS.h"
#include    "Timer.h"
#include    "Ports.h"
#include    "module_A.h"
#include    "module_B.h"
#include    "module_C.h"

// interval for synchronous task operation
volatile uint8_t intervalA;         // initialize to pre-defined value
volatile uint8_t intervalB;
volatile uint8_t intervalC;

// Count value controls the interval between each time the task is called
volatile uint8_t countA;            // unit = 10 ms
volatile uint8_t countB;            // uninitialized, set value from task init
volatile uint8_t countC;

// the run-flag is used to start each task. Set when countX reaches zero
volatile bool runA = false;         // start value = don't run yet
volatile bool runB = false;
volatile bool runC = false;

// the enable flag is used to turn synchronous task operation on / off
volatile bool enableA = false;      // start value = all tasks off
volatile bool enableB = false;
volatile bool enableC = false;

/******************************************************************************/
// Function name: SetAsyncFlag
// Function task: Set the run flag for any task. For asynchronous start of task
// Input: Task from list of task names defined in OS.h
// Return: nil
/******************************************************************************/
extern void SetAsyncFlag (tasks task)
{
   switch (task)
   {
      case taskA: 
         runA = true; 
         break;
      case taskB: 
         runB = true; 
         break;
      case taskC: 
         runC = true; 
         break;
   }
}

/******************************************************************************/
// Function name: SetInterval
// Function task: Set the interval for the task. For synchronous operation
// Input: Task from list of task names defined in OS.h and interval: unit = 10 ms
// Return: nil
/******************************************************************************/
extern void SetTaskInterval(tasks task, uint8_t interval)
{
   switch (task)
   {
      case taskA: 
         intervalA = interval; 
         break;
      case taskB: 
         intervalB = interval; 
         break;
      case taskC: 
         intervalC = interval; 
         break;
   }
}

/******************************************************************************/
// Function name: EnableSyncTask
// Function task: Set the enable flag for any task. Only for synchronous calls
// Input: Task from list of task names defined in OS.h, True / false = enable / disable
// Return: nil
/******************************************************************************/
extern void EnableSyncTask (tasks task, bool onOff)
{
   switch (task)
   {
      case taskA: 
         enableA = onOff; 
         break;
      case taskB: 
         enableB = onOff; 
         break;
      case taskC: 
         enableC = onOff; 
         break;
   }
}

/******************************************************************************/
// Function name: ISR (library function from interrupt.h)
// Function task: ISR for timer 1 overflow. interrupt every 10 ms
// Input: interrupt vector as specified in iom32.h
// Return: nil
/******************************************************************************/
ISR (TIMER1_COMPA_vect)
{
   if (enableA && ((--countA) == 0))    // test for active task and count = zero
   {
      countA = intervalA;               // reload the counter
      runA = true;                      // set the ready-flag for the task
   }
   if (enableB && ((--countB) == 0))
   {
      countB = intervalB;
      runB = true;        
   }
   if (enableC && ((--countC) == 0))
   {
      countC = intervalC;
      runC = true;        
   }
}

/******************************************************************************/
// Function name: main 
// Function task: init the display and timer, start intr-service and count
// Input: 
// Return: dummy value to please the compiler
/******************************************************************************/
extern int main (void)              // return value to please the compiler
{
   Timer1Init();                    // initialize system 
   IoInit();
   
   Ainit();                         // initialize each task
   Binit();
   Cinit();
   
   sei();                           // start the interrupts from the timer

   while(true)                      // start the scheduler
   {
      NEXT:
      if (runA)                     // first task has highest priority
      {
         Atask();                   // call the task function
         runA = false;              // clear the run flag
         goto NEXT;
      }
      if (runB)
      {
         Btask();         
         runB = false;    
         goto NEXT;
      }
      if (runC)                     // last task has lowest priority
      {
         Ctask();         
         runC = false;    
         goto NEXT;
      }
   }
}


