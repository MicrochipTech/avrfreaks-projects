//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*
	file:	myfirstapp.c

	Demonstrate FunkOS's round-robin task scheduling behaviour, including 
	time quantum for each task.
*/

#include "kernelcfg.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "task.h"
#include "taskport.h"

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
void Task1Func(MY_TASK_STRUCT *pstThis_);
void Task2Func(MY_TASK_STRUCT *pstThis_);
void Task3Func(MY_TASK_STRUCT *pstThis_);

void IdleTask(IDLE_TASK_STRUCT *pstIdle_);

//---------------------------------------------------------------------------
// Declare tasks globally 
static MY_TASK_STRUCT 		stTask1;					// Application Task
static MY_TASK_STRUCT 		stTask2;					// Application Task
static MY_TASK_STRUCT 		stTask3;					// Application Task
static IDLE_TASK_STRUCT 	stIdleTask;					// Idle Task

//---------------------------------------------------------------------------
int main(void)
{
	Task_Init();									// Initialize the RTOS
	
	//-----------------------------------------------------------------------	
	// Create three application tasks at the same priority
	//-----------------------------------------------------------------------
	Task_CreateTask(&(stTask1.stTask), 				// Pointer to the task
					"RR Task1",						// Task name
					(UCHAR*)(stTask1.ausStack),		// Task stack pointer
					64,								// Task Size
					1,								// Task Priority
					(void*)Task1Func);				// Task function pointer

	Task_CreateTask(&(stTask2.stTask), 				// Pointer to the task
					"RR Task2",						// Task name
					(UCHAR*)(stTask2.ausStack),		// Task stack pointer
					64,								// Task Size
					1,								// Task Priority
					(void*)Task2Func);				// Task function pointer
	
	Task_CreateTask(&(stTask3.stTask), 				// Pointer to the task
					"RR Task3",						// Task name
					(UCHAR*)(stTask3.ausStack),		// Task stack pointer
					64,								// Task Size
					1,								// Task Priority
					(void*)Task3Func);				// Task function pointer
	
					
	// Create the idle task - always need this
	Task_CreateTask(&(stIdleTask.stTask), 
					"Idle Task",
					(UCHAR*)(stIdleTask.ausStack),
					64,
					0,								// !! Task priority is 0 for idle task !!
					(void*)IdleTask);	

	Task_Add((TASK_STRUCT*)&stTask1);				// Add the tasks to the scheduler
	Task_Add((TASK_STRUCT*)&stTask2);				// Add the tasks to the scheduler
	Task_Add((TASK_STRUCT*)&stTask3);				// Add the tasks to the scheduler	
	Task_Add((TASK_STRUCT*)&stIdleTask);
	
	
	//-----------------------------------------------------------------------	
	// Set the time quantum for each task:
	//
	// Each task will get a fixed % of CPU time based on the quantum values
	// set here, assuming that tasks aren't sleeping or pending.
	// The portion of CPU time given to each task is the ratio of the task's
	// quantum over the sum of each task's quantum.
	//
	// In this example Task 1 will get 5/(5 + 10 + 20) = 5/35 = 14.28% CPU Time
	// Similarly, Task 2 will get 10/35 = 28.57%, and Task 3 will get 20/35 =
	// 57.14% CPU time.  
	//
	// Note that these times do not take into account events like interrupts
	// or other IO operations that eat cycles or switch contexts outside of 
	// the tasks.  
	//-----------------------------------------------------------------------
		
	Task_SetQuantum((TASK_STRUCT)&stTask1, 5);		// Execute continuously for 5 ticks
	Task_SetQuantum((TASK_STRUCT)&stTask2, 10);		// Execute continuously for 10 ticks
	Task_SetQuantum((TASK_STRUCT)&stTask3, 20);		// Execute continuously for 20 ticks
		
	Task_Start((TASK_STRUCT*)&stTask1);				// Start the tasks
	Task_Start((TASK_STRUCT*)&stTask2);				// Start the tasks
	Task_Start((TASK_STRUCT*)&stTask3);				// Start the tasks	
	Task_Start((TASK_STRUCT*)&stIdleTask);
	
	Task_StartTasks();								// Start the scheduler 
	
	//--------------------------------------
	// Scheduler takes over - never returns
	//--------------------------------------
	
	return 0;
}
//---------------------------------------------------------------------------
// Entry Functions for the user tasks
// After letting the system run for a long time, the ratio of count values
// for each task should approach the time sharing values based on setting
// the quantums. 
//---------------------------------------------------------------------------

void Task1Func(MY_TASK_STRUCT *pstThis_)
{
	ULONG iCount = 0;
	while(1)
	{
		iCount++;	
	}
}
//---------------------------------------------------------------------------
void Task2Func(MY_TASK_STRUCT *pstThis_)
{
	ULONG iCount = 0;
	while(1)
	{
		iCount++;		
	}
}
//---------------------------------------------------------------------------
void Task3Func(MY_TASK_STRUCT *pstThis_)
{	
	ULONG iCount = 0;
	while(1)
	{
		iCount++;		
	}
}

//---------------------------------------------------------------------------
// Idle task - doesn't do anything useful
void IdleTask(IDLE_TASK_STRUCT *pstIdle_)
{
	volatile USHORT i = 0;
	while(1)
	{
		/* Do nothing */
		i = i;
	}
}
