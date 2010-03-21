//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*
	file:	myfirstapp.c

	This demo is the basic starting point demonstrating how to set up the RTOS -
	create tasks, initialize the libraries, and start the scheduler.
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
void MyTask(MY_TASK_STRUCT *pstThis_);
void IdleTask(IDLE_TASK_STRUCT *pstIdle_);

MY_TASK_STRUCT 		stMyTask;					// Application Task
IDLE_TASK_STRUCT 	stIdleTask;					// Idle Task
//---------------------------------------------------------------------------
int main(void)
{
	Task_Init();									// Initialize the RTOS
	
	// Create the application task
	Task_CreateTask(&(stMyTask.stTask), 			// Pointer to the task
					"Hello World!",					// Task name
					(UCHAR*)(stMyTask.ausStack),	// Task stack pointer
					64,								// Task Size
					1,								// Task Priority
					(TASK_FUNC)MyTask);					// Task function pointer
	
	// Create the idle task 
	Task_CreateTask(&(stIdleTask.stTask), 
					"Idle Task",
					(UCHAR*)(stIdleTask.ausStack),
					64,
					0,								// !! Task priority is 0 for idle task !!
					(TASK_FUNC)IdleTask);	

	Task_Add((TASK_STRUCT*)&stMyTask);				// Add the tasks to the scheduler
	Task_Add((TASK_STRUCT*)&stIdleTask);

	Task_Start((TASK_STRUCT*)&stMyTask);			// Start the tasks
	Task_Start((TASK_STRUCT*)&stIdleTask);
	
	Task_StartTasks();								// Start the scheduler 
	
	//--------------------------------------
	// Scheduler takes over - never returns
	//--------------------------------------
	
	return 0;
}
//---------------------------------------------------------------------------
// Entry Function for the user task
void MyTask(MY_TASK_STRUCT *pstThis_)
{
	// Increment a counter every 10ms.
	ULONG iCount = 0;
	while(1)
	{
		iCount++;
		Task_Sleep(10);
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
