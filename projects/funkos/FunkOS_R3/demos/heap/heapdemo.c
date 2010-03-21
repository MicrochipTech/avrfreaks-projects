//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*
	file:	myfirstapp.c

	This demo is basic starting point demonstrating how to set up the system
	and create tasks prior to starting 
*/

#include "kernelcfg.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "task.h"
#include "taskport.h"
#include "types.h"
#include "heap.h"

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
void Task1(MY_TASK_STRUCT *pstThis_);
void Task2(MY_TASK_STRUCT *pstThis_);
void Task3(MY_TASK_STRUCT *pstThis_);
void IdleTask(IDLE_TASK_STRUCT *pstIdle_);

static MY_TASK_STRUCT 		stTask1;					// Application Tasks
static MY_TASK_STRUCT 		stTask2;					
static MY_TASK_STRUCT 		stTask3;					
static IDLE_TASK_STRUCT 	stIdleTask;					// Idle Task

//---------------------------------------------------------------------------
int main(void)
{
	Task_Init();									// Initialize the RTOS
	
	// Create the application task
	Task_CreateTask(&(stTask1.stTask), 			// Pointer to the task
					"Task1",					// Task name
					(UCHAR*)(stTask1.ausStack),	// Task stack pointer
					64,							// Task Size
					1,							// Task Priority
					(void*)Task1);				// Task function pointer
	
	Task_CreateTask(&(stTask2.stTask), 			// Pointer to the task
					"Task2",					// Task name
					(UCHAR*)(stTask2.ausStack),	// Task stack pointer
					64,							// Task Size
					2,							// Task Priority
					(void*)Task2);				// Task function pointer
	
	Task_CreateTask(&(stTask3.stTask), 			// Pointer to the task
					"Task3",					// Task name
					(UCHAR*)(stTask3.ausStack),	// Task stack pointer
					64,							// Task Size
					3,							// Task Priority
					(void*)Task3);				// Task function pointer
		
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
	
	Heap_Init();	// !! Initialize the heap before use
	
	Task_StartTasks();								// Start the scheduler 
	
	//--------------------------------------
	// Scheduler takes over - never returns
	//--------------------------------------
	
	return 0;
}
//---------------------------------------------------------------------------
// Entry Function for the user task
void Task1(MY_TASK_STRUCT *pstThis_)
{
	// Set of pointers for dynamic allocation
	UCHAR *pucData1;
	USHORT *pusData2;
	ULONG *pulData3;
	while(1)
	{
		// Allocate data in a staggered way to interleave the allocation 
		// amongst different tasks.
		pucData1 = (UCHAR*)Heap_Alloc(sizoof(UCHAR) * 4);
		Task_Sleep(1);
		pusData2 = (UCHAR*)Heap_Alloc(sizoof(UCHAR) * 4);
		Task_Sleep(2);
		pulData3 = (UCHAR*)Heap_Alloc(sizoof(UCHAR) * 4);		
		Task_Sleep(3);
		
		// Deallocate the memory in a staggered way as well to interleave
		// operations with the different tasks
		Heap_Free(pucData1);
		Task_Sleep(3);
		Heap_Free(pucData2);
		Task_Sleep(2);
		Heap_Free(pucData3);		
		Task_Sleep(1);
	}
}

void Task2(MY_TASK_STRUCT *pstThis_)
{
	// Set of pointers for dynamic allocation
	UCHAR *pucData1;
	USHORT *pusData2;
	ULONG *pulData3;
	while(1)
	{
		// Allocate data in a staggered way to interleave the allocation 
		// amongst different tasks.
		pucData1 = (UCHAR*)Heap_Alloc(sizoof(UCHAR) * 12);
		Task_Sleep(1);
		pusData2 = (UCHAR*)Heap_Alloc(sizoof(UCHAR) * 12);
		Task_Sleep(2);
		pulData3 = (UCHAR*)Heap_Alloc(sizoof(UCHAR) * 2);		
		Task_Sleep(3);
		
		// Deallocate the memory in a staggered way as well to interleave
		// operations with the different tasks
		Heap_Free(pucData1);
		Task_Sleep(3);
		Heap_Free(pucData2);
		Task_Sleep(2);
		Heap_Free(pucData3);		
		Task_Sleep(1);
	}
}

void Task3(MY_TASK_STRUCT *pstThis_)
{
	// Set of pointers for dynamic allocation
	UCHAR *pucData1;
	USHORT *pusData2;
	ULONG *pulData3;
	while(1)
	{
		// Allocate data in a staggered way to interleave the allocation 
		// amongst different tasks.
		pucData1 = (UCHAR*)Heap_Alloc(sizoof(UCHAR) * 3);
		Task_Sleep(1);
		pusData2 = (UCHAR*)Heap_Alloc(sizoof(UCHAR) * 6);
		Task_Sleep(2);
		pulData3 = (UCHAR*)Heap_Alloc(sizoof(UCHAR) * 12);		
		Task_Sleep(3);
		
		// Deallocate the memory in a staggered way as well to interleave
		// operations with the different tasks
		Heap_Free(pucData1);
		Task_Sleep(3);
		Heap_Free(pucData2);
		Task_Sleep(2);
		Heap_Free(pucData3);		
		Task_Sleep(1);
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
