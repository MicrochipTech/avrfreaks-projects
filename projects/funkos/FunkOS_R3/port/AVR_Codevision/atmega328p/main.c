//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*
	file:	timers.c

	This demo shows how to use the lightweight timer threads in FunkOS.
*/

#include "kernelcfg.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "task.h"
#include "taskport.h"
#include "timer.h"

#include "types.h"



//---------------------------------------------------------------------------
//Define a struct to contain all of the first task's task data in one place
typedef struct
{
	TASK_STRUCT stTask;
	USHORT ausStack[64];
} MY_TASK_STRUCT;

//---------------------------------------------------------------------------
// Define a struct to contain the task data etc. for the idle task
typedef struct
{
	TASK_STRUCT stTask;
	USHORT ausStack[64];
} IDLE_TASK_STRUCT;



//---------------------------------------------------------------------------
// functions for the tasks used in this demo
void MyTask(void *pstThis_);
void IdleTask(void *pstIdle_);

//---------------------------------------------------------------------------
// Timer thread function declarations
void TimerThread1(void);
void TimerThread2(void);
void TimerThread3(void);
//---------------------------------------------------------------------------
MY_TASK_STRUCT 		stMyTask;					// Application Task
IDLE_TASK_STRUCT 	stIdleTask;	


void main(void)
{
	//-----------------------------------------------------------------------
	// This stuff is common to most of the demos
	//-----------------------------------------------------------------------
	Task_Init();									// Initialize the RTOS
	
	KernelSWI_Config();								// Configure the software interrupts
	KernelTimer_Config();							// Configure the kernel timer
	
	// Create the application task
	Task_CreateTask(&(stMyTask.stTask), 			// Pointer to the task
					(BYTE *)"Hello World!",					// Task name
					(UCHAR*)(stMyTask.ausStack),	// Task stack pointer
					128,								// Task Size
					1,								// Task Priority
					(TASK_FUNC) MyTask);					// Task function pointer
	
	// Create the idle task 
	Task_CreateTask(&(stIdleTask.stTask), 
					(BYTE *)"Idle Task",
					(UCHAR*)(stIdleTask.ausStack),
					128,
					0,								// !! Task priority is 0 for idle task !!
					(TASK_FUNC) IdleTask);	

	//-----------------------------------------------------------------------
	// !! Timer specific portion of the demo !!
	//-----------------------------------------------------------------------
	Timer_Init();								
	
	// Create three separate timer threads that execute every 30 ticks.
	// The second thread will trigger 10 ticks after the first thread.
	// The third thread will trigger 20 ticks after the first thread, as shown
	// in the following execution graph:
	//     30........40........50........60........70........80........90
	// T1: x.........|.........|.........x.........|.........|.........x
	// T2: |.........x.........|.........|.........x.........|.........|
	// T3: |.........|.........x.........|.........|.........x.........|
	
	Timer_Start(Timer_Add(30, 0, TimerThread1));	
	Timer_Start(Timer_Add(30, 10, TimerThread2));	// Phase lag of 10 ticks
	Timer_Start(Timer_Add(30, 20, TimerThread3));	// Phase lag of 20 ticks
					
	//-----------------------------------------------------------------------
	// More usual start-up code
	//-----------------------------------------------------------------------
	Task_Add((TASK_STRUCT*)&stMyTask);				// Add the tasks to the scheduler
	Task_Add((TASK_STRUCT*)&stIdleTask);

	Task_Start((TASK_STRUCT*)&stMyTask);			// Start the tasks
	Task_Start((TASK_STRUCT*)&stIdleTask);
	
	Task_StartTasks();								// Start the scheduler 
	//--------------------------------------
	// Scheduler takes over - never returns
	//--------------------------------------
	
	return ;
}

//---------------------------------------------------------------------------
// These are the lightweight threads used in the timer example.  
//
// Timer threads are all run-to-completion functions that should be written
// to execute as quickly as possible, because they are called from the 
// timer tick interrupt.  
//---------------------------------------------------------------------------
void TimerThread1(void)
{
	static  BOOL bValue = FALSE;
	
	if (bValue == FALSE)
	{
		bValue = TRUE;
		// Try setting an LED on here
	}
	else
	{
		bValue = FALSE;
		// Try setting an LED off here
	}
}
//---------------------------------------------------------------------------
void TimerThread2(void)
{
	static  BOOL bValue = FALSE;
	
	if (bValue == FALSE)
	{
		bValue = TRUE;
		// Try setting an LED on here
	}
	else
	{
		bValue = FALSE;
		// Try setting an LED off here
	}
}
//---------------------------------------------------------------------------
void TimerThread3(void)
{
	static  BOOL bValue = FALSE;
	
	if (bValue == FALSE)
	{
		bValue = TRUE;
		// Try setting an LED on here
	}
	else
	{
		bValue = FALSE;
		// Try setting an LED off here
	}
}

//---------------------------------------------------------------------------
// Regular demo threads
//---------------------------------------------------------------------------
// Entry Function for the user task
void MyTask(void *pstThis_)
{
	// Increment a counter every 10ms.
	ULONG iCount = 0;
	while(1)
	{
		iCount++;
		Task_Sleep(2);
	}
}
//---------------------------------------------------------------------------
// Idle task - doesn't do anything useful
void IdleTask(void *pstIdle_)
{
	volatile USHORT i = 0;
	while(1)
	{
		/* Do nothing */
		i = i;
	}
}
