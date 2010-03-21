//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*
	file:	pipsqueak_demo.c

	This demo is the basic starting point demonstrating how to set up an application
	using the Pipsqueak real-time kernel included as part of FunkOS.  This is a
	smaller, faster alternative to the regular FunkOS kernel, sacrificing features
	for speed and size.
*/
#include "types.h"
#include "pipsqueak.h"
#include "taskport.h"

//---------------------------------------------------------------------------
//!! Define task handler functions for 2 round-robin tasks and an idle task
//---------------------------------------------------------------------------
void Task1(void);
void Task2(void);
void Idle(void);

//---------------------------------------------------------------------------
//!! Define task structures for 2 RR tasks and an idle task
//---------------------------------------------------------------------------
TASK_STRUCT stTask1;
TASK_STRUCT stTask2;
TASK_STRUCT stIdleTask;

//---------------------------------------------------------------------------
//!! Define stack space for 2 round-robin tasks and an idle task
//---------------------------------------------------------------------------
WORD awStack1[128];
WORD awStack2[128];
WORD awIdleStack[128];

//---------------------------------------------------------------------------
int main(void)
{
	//------------------------------------
	// Initialize the kernel
	Task_Init();

	//------------------------------------
	// Intialize the task objects
	stTask1.pfHandler = (TASK_FUNC)Task1;	// Task's entry function
	stTask1.pwStack = awStack1;				// Assign the stack
	stTask1.usTicks = 3;					// 3 ticks per turn
	stTask1.usStackSize = 128;				// Size of the stack in bytes
	
	stTask2.pfHandler = (TASK_FUNC)Task2;
	stTask2.pwStack = awStack2;
	stTask2.usTicks = 3;
	stTask2.usStackSize = 128;
	
	stIdleTask.pfHandler = (TASK_FUNC)Idle;
	stIdleTask.pwStack = awIdleStack;
	stIdleTask.usTicks = 3;
	stIdleTask.usStackSize = 128;
	
	//------------------------------------
	// Initialize the stack for each task
	Task_InitStack(&stTask1);
	Task_InitStack(&stTask2);
	Task_InitStack(&stIdleTask);

	//------------------------------------
	// Add the two round-robin tasks to the scheduler
	Task_AddTask(&stTask1);
	Task_AddTask(&stTask2);
	
	// Add the idle task to the scheduler
	Task_SetIdleTask(&stIdleTask);

	// Start the scheduler
	Task_StartTasks();

	// Program will never get this far...
	while(1){};
	return 1;
}

//---------------------------------------------------------------------------
// Task 1:  Do something for a while, then go to the next task immediately
//---------------------------------------------------------------------------
void Task1(void)
{
	int i = 0;
	while(1)
	{
		while (i < 10)
		{
			i++;
		}
		i = 0;
		KernelSWI_Trigger();	// Go to the next task immediately (don't wait)
	}
}

//---------------------------------------------------------------------------
// Task 2:  Do something for a while, then go to sleep for the remaining ticks
//---------------------------------------------------------------------------
void Task2(void)
{
	int i = 0;
	while(1)
	{
		while (i < 10)
		{
			i++;
		}
		i = 0;

		Task_GoIdle();			// Go to the idle thread
	}
}
//---------------------------------------------------------------------------
void Idle(void)
{
	volatile int i = 0;
	while(1)
	{
		i++;
	}
}

