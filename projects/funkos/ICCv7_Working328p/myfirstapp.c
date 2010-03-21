//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*
	file:	myfirstapp.c

	This demo is the basic starting point demonstrating how to set up the RTOS -
	create tasks, initialize the libraries, and start the scheduler.

	Project configuration:
	Target:	ATMega328p
	Speed: 	8MHz

	This version of MyFirstApp is assembled as a minimal ICC project for use
	as a starting point, demonstrating how to build FunkOS into an application.  
	This project is also set up to be simulated using the V2 simulator, so you can
	see how the kernel works without needing actual hardware.	
	
	To modify this project for other targets, copy the contents from the appropriate
	port folder (somewhere under "ports\AVR_ICC") into the project directory, select
	the appropriate device from the Project->Options dialog, and modify the Debug 
	platform in the Debug->Select Platform and Device dialog.

*/

#include "kernelcfg.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "task.h"
#include "taskport.h"

#include "types.h"

#define TASK_STACK_SIZE			  (128)
#define IDLE_STACK_SIZE			  (96)

//---------------------------------------------------------------------------
//Define a struct to contain all of the first task's task data in one place
typedef struct
{
	TASK_STRUCT stTask;
	USHORT ausStack[TASK_STACK_SIZE/sizeof(USHORT)];
} MY_TASK_STRUCT;

//---------------------------------------------------------------------------
// Define a struct to contain the task data etc. for the idle task
typedef struct
{
	TASK_STRUCT stTask;
	USHORT ausStack[IDLE_STACK_SIZE/sizeof(USHORT)];
} IDLE_TASK_STRUCT;

MY_TASK_STRUCT 		stMyTask;					// Application Task
IDLE_TASK_STRUCT 	stIdleTask;					// Idle Task

//---------------------------------------------------------------------------
static void MyTask(void *pvThis_);
static void IdleTask(void *pvThis_);

//---------------------------------------------------------------------------
int main(void)
{
	Task_Init();									// Initialize the RTOS
	KernelSWI_Config();								// Configure the software interrupts
	KernelTimer_Config();							// Configure the kernel timer
	// Create the application task

	Task_CreateTask(&(stMyTask.stTask), 			// Pointer to the task
					"Hello World!",					// Task name
					(UCHAR*)(stMyTask.ausStack),	// Task stack pointer
					TASK_STACK_SIZE,				// Task Size
					1,								// Task Priority
					(void*)*(FROM_FLASH)MyTask);	//Task function pointer

	// Create the idle task 
	Task_CreateTask(&(stIdleTask.stTask), 
					"Idle Task",
					(UCHAR*)(stIdleTask.ausStack),
					IDLE_STACK_SIZE,
					0,
					(void*)*(FROM_FLASH)IdleTask);	// !! Task priority is 0 for idle task !!
	

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
static void MyTask(void *pvThis_)
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
static void IdleTask(void *pvThis_)
{
	volatile USHORT i = 0;
	while(1)
	{
		/* Do nothing */
		i = i;
	}
}
