//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*
	file:	sem_demo.c

	This demo shows the use of mutual exclusion semaphores for resource 
	protection in FunkOS.

	There are three application tasks in this demo, along with the idle task.
	The three tasks each wish to modify the value of the shared resource
	(ucShared).  However, only one object can do this at a time.  As a result,
	each task must first claim the mutex successfully before it can use the
	resource - and while the resource is claimed, no other task can attempt
	to claim it.
	
	When multiple tasks wish to claim the semaphore, priority inheritence is
	used to ensure that lower priority tasks cannot possible cause a priority
	inversion when tasks of different priorities are waiting to access the
	shared resource.
	
*/

#include "kernelcfg.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "task.h"
#include "taskport.h"
#include "mutex.h"

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
static MY_TASK_STRUCT 		stTask1;					// Application Task
static MY_TASK_STRUCT 		stTask2;					// Application Task
static MY_TASK_STRUCT 		stTask3;					// Application Task
static IDLE_TASK_STRUCT 	stIdleTask;					// Idle Task

static MUTEX_STRUCT			stMutex;					// Mutual-exclusion object
static volatile UCHAR		ucShared = 0;				// Resource shared between threads
//---------------------------------------------------------------------------
// functions for the tasks used in this demo
void AccessorTask1(MY_TASK_STRUCT *pstThis_);
void AccessorTask2(MY_TASK_STRUCT *pstThis_);
void AccessorTask3(MY_TASK_STRUCT *pstThis_);

void IdleTask(IDLE_TASK_STRUCT *pstIdle_);

//---------------------------------------------------------------------------
// Function implementing the resource protected block

//---------------------------------------------------------------------------
int main(void)
{
	Task_Init();									// Initialize the RTOS
	
	// Create the application task
	Task_CreateTask(&(stTask1.stTask), 				// Pointer to the task
					"Task 1",						// Task name
					(UCHAR*)(stTask1.ausStack),		// Task stack pointer
					64,								// Task Size
					1,								// Task Priority
					(void*)AccessorTask1);			// Task function pointer

	Task_CreateTask(&(stTask2.stTask), 				// Pointer to the task
					"Task 2",						// Task name
					(UCHAR*)(stTask2.ausStack),		// Task stack pointer
					64,								// Task Size
					2,								// Task Priority
					(void*)AccessorTask2);			// Task function pointer

	Task_CreateTask(&(stTask3.stTask), 				// Pointer to the task
					"Task 3",						// Task name
					(UCHAR*)(stTask3.ausStack),		// Task stack pointer
					64,								// Task Size
					3,								// Task Priority
					(void*)AccessorTask3);			// Task function pointer

	// Create the idle task 
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

	Task_Start((TASK_STRUCT*)&stTask1);				// Start the tasks
	Task_Start((TASK_STRUCT*)&stTask2);				// Start the tasks
	Task_Start((TASK_STRUCT*)&stTask3);				// Start the tasks
	Task_Start((TASK_STRUCT*)&stIdleTask);

	Mutex_Init(&stMutex);
	
	Task_StartTasks();								// Start the scheduler 
	
	//--------------------------------------
	// Scheduler takes over - never returns
	//--------------------------------------
	
	return 0;
}
//---------------------------------------------------------------------------
// Entry Function for the user task
void AccessorTask1(MY_TASK_STRUCT *pstThis_)
{
	while(1)
	{
		// Wait until we have permission to use the resource	
		Mutex_Claim(&stMutex, TIME_FOREVER);

		// Do something with the shared resource, set the variable to our
		// task number.
		ucShared = 1;
		Task_Sleep(20);
		
		// release the resource
		Mutex_Release(&stMutex);
	}
}
//---------------------------------------------------------------------------
void AccessorTask2(MY_TASK_STRUCT *pstThis_)
{
	while(1)
	{
		Mutex_Claim(&stMutex, TIME_FOREVER);
		ucShared = 2;
		Task_Sleep(10);
		Mutex_Release(&stMutex);
	}
}
//---------------------------------------------------------------------------
void AccessorTask3(MY_TASK_STRUCT *pstThis_)
{
	while(1)
	{
		Mutex_Claim(&stMutex, TIME_FOREVER);
		ucShared = 3;
		Task_Sleep(5);
		Mutex_Release(&stMutex);
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
