//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*
	file:	plumberdemo.c

	This demo shows how to use the plumber for inter-process communications in the 
	system.
*/

#include "kernelcfg.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "task.h"
#include "taskport.h"
#include "plumber.h"
#include "types.h"

//---------------------------------------------------------------------------
//Define a struct to contain all of the first task's task data in one place
typedef struct
{
	TASK_STRUCT stTask;
	USHORT ausStack[128];
	SEMAPHORE_STRUCT stSemaphore;
} MY_TASK_STRUCT;

//---------------------------------------------------------------------------
// Define a struct to contain the task data etc. for the idle task
typedef struct
{
	TASK_STRUCT stTask;
	USHORT ausStack[128];
} IDLE_TASK_STRUCT;

//---------------------------------------------------------------------------
// functions for the tasks used in this demo
void Producer(MY_TASK_STRUCT *pstThis_);
void Consumer1(MY_TASK_STRUCT *pstThis_);
void Consumer2(MY_TASK_STRUCT *pstThis_);
void Consumer3(MY_TASK_STRUCT *pstThis_);
void IdleTask(IDLE_TASK_STRUCT *pstIdle_);

//---------------------------------------------------------------------------
static MY_TASK_STRUCT 		stProducer;					// Application Tasks
static MY_TASK_STRUCT		stConsumer1;			
static MY_TASK_STRUCT		stConsumer2;			
static MY_TASK_STRUCT		stConsumer3;			
static IDLE_TASK_STRUCT 	stIdleTask;					// Idle Task
//---------------------------------------------------------------------------
int main(void)
{
	Task_Init();									// Initialize the RTOS
	
	// Create the application task
	Task_CreateTask(&(stProducer.stTask), 			// Pointer to the task
					"Producer",						// Task name
					(UCHAR*)(stProducer.ausStack),	// Task stack pointer
					128,								// Task Size
					4,								// Task Priority
					(void*)Producer);					// Task function pointer
	
	Task_CreateTask(&(stConsumer1.stTask), 			// Pointer to the task
					"Consumer 1",					// Task name
					(UCHAR*)(stConsumer1.ausStack),	// Task stack pointer
					128,								// Task Size
					1,								// Task Priority
					(void*)Consumer1);					// Task function pointer

	Task_CreateTask(&(stConsumer2.stTask), 			// Pointer to the task
					"Consumer 2",					// Task name
					(UCHAR*)(stConsumer2.ausStack),	// Task stack pointer
					128,								// Task Size
					2,								// Task Priority
					(void*)Consumer2);					// Task function pointer
	
	Task_CreateTask(&(stConsumer3.stTask), 			// Pointer to the task
					"Consumer 3",					// Task name
					(UCHAR*)(stConsumer3.ausStack),	// Task stack pointer
					128,								// Task Size
					3,								// Task Priority
					(void*)Consumer3);					// Task function pointer
	
	// Create the idle task 
	Task_CreateTask(&(stIdleTask.stTask), 
					"Idle Task",
					(UCHAR*)(stIdleTask.ausStack),
					128,
					0,								// !! Task priority is 0 for idle task !!
					(void*)IdleTask);	

	Task_Add((TASK_STRUCT*)&stProducer);			// Add the tasks to the scheduler
	Task_Add((TASK_STRUCT*)&stConsumer1);			// Add the tasks to the scheduler
	Task_Add((TASK_STRUCT*)&stConsumer2);			// Add the tasks to the scheduler
	Task_Add((TASK_STRUCT*)&stConsumer3);			// Add the tasks to the scheduler
	Task_Add((TASK_STRUCT*)&stIdleTask);

	Plumber_CreateTask();							// Initialize and add the plumber task
	
	Task_Start((TASK_STRUCT*)&stProducer);			// Start the tasks
	Task_Start((TASK_STRUCT*)&stConsumer1);
	Task_Start((TASK_STRUCT*)&stConsumer2);
	Task_Start((TASK_STRUCT*)&stConsumer3);
	Task_Start((TASK_STRUCT*)&stIdleTask);
	
	Semaphore_Init(&stConsumer1.stSemaphore);		// Initialize the semaphores for each
	Semaphore_Init(&stConsumer2.stSemaphore);
	Semaphore_Init(&stConsumer3.stSemaphore);
	
	Task_StartTasks();								// Start the scheduler 
	
	//--------------------------------------
	// Scheduler takes over - never returns
	//--------------------------------------
	
	return 0;
}
//---------------------------------------------------------------------------
void Producer(MY_TASK_STRUCT *pstThis_)
{
	UCHAR *pcData1 = (UCHAR*)"Hello Port 21!";
	UCHAR *pcData2 = (UCHAR*)"Hello Port 80!";
	
	// Before entering the task loop, set-up the consumer tasks and ports
	// Consumer task 1 will respond to messages on port 80
	// Consumer tasks 2 and 3 both respond to messages on port 21
	Plumber_AddToList((TASK_STRUCT*)&stConsumer1, 80, &stConsumer1.stSemaphore);
	Plumber_AddToList((TASK_STRUCT*)&stConsumer2, 21, &stConsumer2.stSemaphore);
	Plumber_AddToList((TASK_STRUCT*)&stConsumer3, 21, &stConsumer3.stSemaphore);
	
	// Open the ports in question
	Plumber_OpenPort(21);
	Plumber_OpenPort(80);
	
	// Task loop
	while(1)
	{
		// Write port 21 - task 1 will process the data
		Plumber_WriteToPort(pcData1, 14, 21);
		// Write port 80 - tasks 2&3 will each get its own copy 
		
		Plumber_WriteToPort(pcData2, 14, 80);
		
		// Go to sleep and let the consumer tasks process the data
		Task_Sleep((TASK_STRUCT*)pstThis_, 1);
	}
}
//---------------------------------------------------------------------------
void Consumer1(MY_TASK_STRUCT *pstThis_)
{
	PLUMB_STRUCT *pstPipe;
	while(1)
	{
		// Wait until we get a message.
		Semaphore_Pend(&(pstThis_->stSemaphore), TIME_FOREVER);

		// Claim pipes for this task/port...
		pstPipe = Plumber_ClaimPipe((TASK_STRUCT*)pstThis_, 80);

		// While there's data to be processed...
		while (pstPipe != NULL)
		{
		
			// Process the data held in the pipe

			//---------------------
			// !! Do Something !!
			//---------------------

			// Release the pipe when finished with it.
			Plumber_FreePipe(pstPipe);

			// See if there's more data on the port for this task.
			pstPipe = Plumber_ClaimPipe((TASK_STRUCT*)pstThis_, 80);		
		}
	}
}
//---------------------------------------------------------------------------
void Consumer2(MY_TASK_STRUCT *pstThis_)
{
	PLUMB_STRUCT *pstPipe;
	while(1)
	{
		// Wait until we get a message.
		Semaphore_Pend(&(pstThis_->stSemaphore), TIME_FOREVER);

		// Claim pipes for this task/port...
		pstPipe = Plumber_ClaimPipe((TASK_STRUCT*)pstThis_, 21);

		// While there's data to be processed...
		while (pstPipe != NULL)
		{
		
			// Process the data held in the pipe

			//---------------------
			// !! Do Something !!
			//---------------------

			// Release the pipe when finished with it.
			Plumber_FreePipe(pstPipe);

			// See if there's more data on the port for this task.
			pstPipe = Plumber_ClaimPipe((TASK_STRUCT*)pstThis_, 21);		
		}
	}
}
//---------------------------------------------------------------------------
void Consumer3(MY_TASK_STRUCT *pstThis_)
{
	PLUMB_STRUCT *pstPipe;
	while(1)
	{
		// Wait until we get a message.
		Semaphore_Pend(&(pstThis_->stSemaphore), TIME_FOREVER);

		// Claim pipes for this task/port...
		pstPipe = Plumber_ClaimPipe((TASK_STRUCT*)pstThis_, 21);

		// While there's data to be processed...
		while (pstPipe != NULL)
		{
		
			// Process the data held in the pipe

			//---------------------
			// !! Do Something !!
			//---------------------

			// Release the pipe when finished with it.
			Plumber_FreePipe(pstPipe);

			// See if there's more data on the port for this task.
			pstPipe = Plumber_ClaimPipe((TASK_STRUCT*)pstThis_, 21);		
		}
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
