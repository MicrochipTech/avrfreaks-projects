//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*
	file:	sem_demo.c

	This demo shows the use of semaphores for task synchronization in FunkOS.

	There are two application tasks in this demo, along with the idle task.
	The first task (producer) performs some operations, and when it is complete
	it posts the binary semaphore on which the second (consumer) task pends.  
	
	Since the consumer task has a higher priority than the first task, posting
	the semaphore in the producer task will  trigger a context switch to the 
	consumer task.
*/

#include "kernelcfg.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "task.h"
#include "taskport.h"
#include "semaphore.h"

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
static MY_TASK_STRUCT 		stProducerTask;				// Application Task
static MY_TASK_STRUCT 		stConsumerTask;				// Application Task
static IDLE_TASK_STRUCT 	stIdleTask;					// Idle Task

static SEMAPHORE_STRUCT		stSemaphore;				// Synchronization object

//---------------------------------------------------------------------------
// functions for the tasks used in this demo
void Produce(MY_TASK_STRUCT *pstThis_);
void Consume(MY_TASK_STRUCT *pstThis_);

void IdleTask(IDLE_TASK_STRUCT *pstIdle_);
//---------------------------------------------------------------------------
int main(void)
{
	Task_Init();									// Initialize the RTOS
	
	// Create the application task
	Task_CreateTask(&(stProducerTask.stTask), 		// Pointer to the task
					"Producer",						// Task name
					(UCHAR*)(stProducerTask.ausStack),	// Task stack pointer
					64,								// Task Size
					1,								// Task Priority
					(void*)Produce);				// Task function pointer

	// Create the application task
	Task_CreateTask(&(stConsumerTask.stTask), 		// Pointer to the task
					"Consumer",						// Task name
					(UCHAR*)(stConsumerTask.ausStack),	// Task stack pointer
					64,								// Task Size
					2,								// Task Priority
					(void*)Consume);				// Task function pointer
					
	// Create the idle task 
	Task_CreateTask(&(stIdleTask.stTask), 
					"Idle Task",
					(UCHAR*)(stIdleTask.ausStack),
					64,
					0,								// !! Task priority is 0 for idle task !!
					(void*)IdleTask);	

	Task_Add((TASK_STRUCT*)&stConsumerTask);		// Add the tasks to the scheduler
	Task_Add((TASK_STRUCT*)&stProducerTask);		// Add the tasks to the scheduler
	Task_Add((TASK_STRUCT*)&stIdleTask);

	Task_Start((TASK_STRUCT*)&stConsumerTask);		// Start the tasks
	Task_Start((TASK_STRUCT*)&stProducerTask);		// Start the tasks
	Task_Start((TASK_STRUCT*)&stIdleTask);

	Semaphore_Init(&stSemaphore);
	
	Task_StartTasks();								// Start the scheduler 
	
	//--------------------------------------
	// Scheduler takes over - never returns
	//--------------------------------------
	
	return 0;
}
//---------------------------------------------------------------------------
// Entry Function for the user task
void Produce(MY_TASK_STRUCT *pstThis_)
{
	while(1)
	{
		// Wait for 10 ticks, simulate doing something intensive
		Task_Sleep(10);
		
		// Post the semaphore
		Semaphore_Post(&stSemaphore);
	}
}
//---------------------------------------------------------------------------
void Consume(MY_TASK_STRUCT *pstThis_)
{
	ULONG ulTick = 0;
	while(1)
	{
		// Wait forever for the semaphore		
		Semaphore_Pend(&stSemaphore, TIME_FOREVER);
		
		// Simulate doing something useful.
		ulTick++;
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
