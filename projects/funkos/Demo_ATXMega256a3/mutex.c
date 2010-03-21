//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  mutex.c

	Description:
		Mutual exclusion semaphore implementation for FunkOS
*/
#include "mutex.h"
#include "types.h"
#include "task.h"
#include "taskport.h"
#include "kernelcfg.h"

#if KERNEL_USE_MUTEX

//---------------------------------------------------------------------------
static void Mutex_WakeNextPending(MUTEX_STRUCT *pstSem_);
static void Mutex_DeleteFromList(MUTEX_STRUCT *pstMutex_, TASK_STRUCT *pstTask_);
static void Mutex_AddToList(MUTEX_STRUCT *pstMutex_, TASK_STRUCT *pstTask_);
static void Mutex_InheritPriority(MUTEX_STRUCT *pstMutex_, UCHAR ucPriority_);

//---------------------------------------------------------------------------
/*!
	Initialize a mutual exclusion semaphore by setting the semaphore value to 1 
	emptying the task list, and resetting the maximum priority value.
	\fn BOOL Mutex_Init(MUTEX_STRUCT *pstMutex_)
	\param pstMutex_ - pointer to the mutex to initialize
*/
void Mutex_Init(MUTEX_STRUCT *pstMutex_)
{
	pstMutex_->usSem = 1;
	pstMutex_->ucMaxPri = 0;
	pstMutex_->pstTask = NULL;
}
//---------------------------------------------------------------------------
/*!
	Signal a task to claim a mutex semaphore.  If the mutex is not available,
	the task will block and wait until the resource becomes available.
	\fn BOOL Mutex_Claim(MUTEX_STRUCT *pstMutex_, USHORT usTime_)
	\param pstMutex_ - pointer to the mutex to claim
	\param usTime_ - the time limit to wait for the mutex. Integer number of ticks or TIME_FOREVER
	\return BOOL - TRUE on success, FALSE on timeout
*/
BOOL Mutex_Claim(MUTEX_STRUCT *pstMutex_, USHORT usTime_)
{
	BOOL bReturn = TRUE;
	TASK_STRUCT *pstTask;
	// Disable the scheduler while doing the pend...	
	Task_SchedulerDisable();
	pstTask = Task_GetCurrentTask();
	// Backup the priority for the task (mangled by priority inverison protection)
	pstTask->ucMutexPriority = pstTask->ucPriority;
	if (pstMutex_->usSem != 0)
	{
		// Mutex isn't claimed, claim it.
		pstMutex_->usSem = 0;
		pstMutex_->ucMaxPri = pstTask->ucPriority;
	}
	else
	{
		CS_ENTER();
		// Reset the task state and timeout...
		pstTask->eState = TASK_BLOCKED;
		pstTask->usTimeLeft = usTime_;

		CS_EXIT();

		// Add task to mutex.
		Mutex_AddToList(pstMutex_, pstTask);
		
		// Check if priority inheritence is necessary
		Mutex_InheritPriority(pstMutex_, pstTask->ucPriority);
		
		// enable scheduler
		Task_SchedulerRestore(TRUE);
		
		// Switch tasks
		Task_YieldSWI();
		
		//!! disable scheduler		
		Task_SchedulerDisable();
		
		// Check the state...
		if (pstTask->bTimeout)
		{
			bReturn = FALSE;			
			// Remove task from the list.
			Mutex_DeleteFromList(pstMutex_, pstTask);			
			// Reset the task timeout
			pstTask->bTimeout = FALSE;
		}
	}
	//!! enable scheduler
	Task_SchedulerRestore(TRUE);
	return bReturn;
}

//---------------------------------------------------------------------------
/*!
	Releases a mutex, signalling that another task can utilize the associate
	resource.
	\fn void Mutex_Release(MUTEX_STRUCT *pstMutex_)
	\param pstMutex_ - pointer to the mutex to release
*/
void Mutex_Release(MUTEX_STRUCT *pstMutex_)
{	
	TASK_STRUCT *pstTask;
	//!! Disable scheduler
	Task_SchedulerDisable();
	pstTask = Task_GetCurrentTask();

	pstTask->ucPriority = pstTask->ucMutexPriority; // Reset the task priority 
	pstTask->ucMutexPriority = 0;

	// Nothing is pending
	if (pstMutex_->pstTask == NULL)
	{
		// Re-initialize the mutex
		pstMutex_->usSem = 1;
		
		//! Re-enable scheduler
		Task_SchedulerRestore(TRUE);
	}
	else
	{
		// Wake the highest priority task pending on the mutex
		Mutex_WakeNextPending(pstMutex_);
		//!! Re-enable the scheduler
		Task_SchedulerRestore(TRUE);
		// Switch tasks
		Task_YieldSWI();		
	}	
}

//---------------------------------------------------------------------------
/*!
	Removes a task from the mutex wait list
	\fn void Mutex_DeleteFromList(MUTEX_STRUCT *pstMutex_, TASK_STRUCT *pstTask_)
	\param pstMutex_ - pointer to the mutex to remove from the list
	\param pstTask_ - pointer to the owner task
*/
static void Mutex_DeleteFromList(MUTEX_STRUCT *pstMutex_, TASK_STRUCT *pstTask_)
{
	TASK_STRUCT *pstCurrent = NULL;
	TASK_STRUCT *pstPrev = NULL;

	// Maybe check the validity of the sem/task objects?!
	
	// Check to see if there's anything in the task list.
	pstCurrent = pstMutex_->pstTask;

	// Find the item in the list
	pstPrev = pstMutex_->pstTask;
	while ((pstCurrent != pstTask_) && (pstCurrent != NULL))
	{
		pstPrev = pstCurrent;
		pstCurrent = pstCurrent->pstNextList;
	}

	// If the item wasn't found in the list, bail.
	if (pstCurrent == NULL)
	{
		return;
	}

	// Unlink the task from the list
	if (pstPrev != pstMutex_->pstTask) // Not the head of the list
	{
		pstPrev->pstNextList = pstCurrent->pstNextList;
	}
	else // The head of the list
	{
		pstMutex_->pstTask = pstCurrent->pstNextList;
	}
	
	pstTask_->pstNextList = NULL; // reset the next semaphore pointer	
}

//---------------------------------------------------------------------------
/*!
	Wakes the highest priority task waiting for the mutex.
	\fn void Mutex_WakeNextPending(MUTEX_STRUCT *pstMutex_)
	\param pstMutex_ - pointer to the mutex to manipulate
*/
static void Mutex_WakeNextPending(MUTEX_STRUCT *pstMutex_)
{
	TASK_STRUCT *pstCurrent = NULL;
	TASK_STRUCT *pstChosenOne = NULL;
	
	UCHAR ucMaxPri = 0;

	// Check to see if there's anything in the task list.
	pstCurrent = pstMutex_->pstTask;

	if (pstCurrent == NULL)
	{
		// Reset the mutex - this is the last task waiting on the resource
		pstMutex_->ucMaxPri = 0;
		pstMutex_->usSem = 1;
		pstMutex_->pstTask = NULL;
	}
	else
	{
		// Start with first item chosen
		pstChosenOne = pstCurrent;
		
		while (pstCurrent != NULL)
		{
			// Search by original priority
			if (pstCurrent->ucMutexPriority > ucMaxPri)
			{
				ucMaxPri = pstCurrent->ucMutexPriority;
				pstChosenOne = pstCurrent;
			}

			// Iterate through the list of pending tasks
			pstCurrent = pstCurrent->pstNextList;
		}

		// Remove the chosen one from the list
		Mutex_DeleteFromList(pstMutex_, pstChosenOne);
		
		// Move task to the ready state
		pstChosenOne->eState = TASK_READY;
	}
}
//---------------------------------------------------------------------------
/*!
	Adds a task to the specified mutex wait list.
	\fn static void Mutex_AddToList(MUTEX_STRUCT *pstMutex_, TASK_STRUCT *pstTask_)
	\param pstMutex_ - pointer to the mutex 
	\param pstTask_ - pointer to the task to put in the wait list
*/
static void Mutex_AddToList(MUTEX_STRUCT *pstMutex_, TASK_STRUCT *pstTask_)
{
	if (pstMutex_->pstTask == NULL) // List is empty
	{
		// No task list to chain
		pstTask_->pstNextList = NULL; 
	}	
	else // List isn't empty
	{		
		// Chain the existing task list to the new task
		pstTask_->pstNextList = pstMutex_->pstTask;		
	}
	pstMutex_->pstTask = pstTask_; // Assign list to mutex
}
//---------------------------------------------------------------------------
/*!
	Checks the maximum priority in the mutex wait list.  If the new specified
	priority is	higher than the current maximum, it updates the mutex task list 
	and modifies the task priorities to the new level to prevent priority 
	inversion.
	\fn static void Mutex_InheritPriority(MUTEX_STRUCT *pstMutex_, UCHAR ucPriority_)
	\param pstMutex_ - pointer to the mutex to release
	\param ucPriority_ - priority to inherit
*/
static void Mutex_InheritPriority(MUTEX_STRUCT *pstMutex_, UCHAR ucPriority_)
{
	TASK_STRUCT *pstCurrent;
	
	if (ucPriority_ > pstMutex_->ucMaxPri)
	{	
		// Set the new maximum priority
		pstMutex_->ucMaxPri = ucPriority_;

		// Go through and set the priorities for every task in the list to
		// the new maximum level.
		pstCurrent = pstMutex_->pstTask;

		while (pstCurrent != NULL)
		{
			pstCurrent->ucPriority = ucPriority_;
			pstCurrent = pstCurrent->pstNextList;
		}	
	}
}
#endif //KERNEL_USE_MUTEX

