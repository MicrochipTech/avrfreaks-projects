//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  mutex.cpp

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
/*!
	Initialize a mutual exclusion semaphore by setting the semaphore value to 1 
	emptying the task list, and resetting the maximum priority value.
	\fn void FunkOS_Mutex::Init(void)
*/
void FunkOS_Mutex::Init(void)
{
	m_usSem = 1;
	m_ucMaxPri = 0;
	m_pclTask = (FunkOS_Task*)NULL;
}
//---------------------------------------------------------------------------
/*!
	Signal a task to claim a mutex semaphore.  If the mutex is not available,
	the task will block and wait until the resource becomes available.
	\fn BOOL FunkOS_Mutex::Claim(USHORT usTime_)
	\param usTime_ - the time limit to wait for the mutex. Integer number of ticks or TIME_FOREVER
	\return BOOL - TRUE on success, FALSE on timeout
*/
BOOL FunkOS_Mutex::Claim(USHORT usTime_)
{
	BOOL bReturn = TRUE;
	FunkOS_Task *pclTask;
	
	// Disable the scheduler while doing the pend...	
	FunkOS_Scheduler::Disable();
	pclTask = FunkOS_Scheduler::GetCurrentTask();
	
	// Backup the priority for the task (mangled by priority inverison protection)
	pclTask->m_ucMutexPriority = pclTask->m_ucPriority;
	if (m_usSem != 0)
	{
		// Mutex isn't claimed, claim it.
		m_usSem = 0;
		m_ucMaxPri = pclTask->m_ucPriority;
	}
	else
	{
		CS_ENTER();
		// Reset the task state and timeout...
		pclTask->m_eState = TASK_BLOCKED;
		pclTask->m_usTimeLeft = usTime_;

		CS_EXIT();

		// Add task to mutex.
		AddToList(pclTask);
		
		// Check if priority inheritence is necessary
		InheritPriority(pclTask->m_ucPriority);
		
		// enable scheduler
		FunkOS_Scheduler::Restore(TRUE);
		
		// Switch tasks
		FunkOS_Task::Yield();
		
		//!! disable scheduler		
		FunkOS_Scheduler::Disable();
		
		// Check the state...
		if (pclTask->m_bTimeout)
		{
			bReturn = FALSE;			
			// Remove task from the list.
			DeleteFromList(pclTask);			
			// Reset the task timeout
			pclTask->m_bTimeout = FALSE;
		}
	}
	
	//!! enable scheduler
	FunkOS_Scheduler::Restore(TRUE);
	return bReturn;
}

//---------------------------------------------------------------------------
/*!
	Releases a mutex, signalling that another task can utilize the associate
	resource.
	\fn void FunkOS_Mutex::Release(void)
*/
void FunkOS_Mutex::Release(void)
{	
	FunkOS_Task *pclTask;
	//!! Disable scheduler
	FunkOS_Scheduler::Disable();
	pclTask = FunkOS_Scheduler::GetCurrentTask();

	pclTask->m_ucPriority = pclTask->m_ucMutexPriority; // Reset the task priority 
	pclTask->m_ucMutexPriority = 0;

	// Nothing is pending
	if (m_pclTask == NULL)
	{
		// Re-initialize the mutex
		m_usSem = 1;
		
		//! Re-enable scheduler
		FunkOS_Scheduler::Restore(TRUE);
	}
	else
	{
		// Wake the highest priority task pending on the mutex
		WakeNextPending();
		//!! Re-enable the scheduler
		FunkOS_Scheduler::Restore(TRUE);
		// Switch tasks
		FunkOS_Task::Yield();		
	}	
}

//---------------------------------------------------------------------------
/*!
	Removes a task from the mutex wait list
	\fn void FunkOS_Mutex::DeleteFromList(FunkOS_Task *pclTask_)
	\param m_pclTask_ - pointer to the owner task
*/
void FunkOS_Mutex::DeleteFromList(FunkOS_Task *pclTask_)
{
	FunkOS_Task *pclCurrent = (FunkOS_Task*)NULL;
	FunkOS_Task *pclPrev = (FunkOS_Task*)NULL;

	// Maybe check the validity of the sem/task objects?!
	
	// Check to see if there's anything in the task list.
	pclCurrent = m_pclTask;

	// Find the item in the list
	pclPrev = m_pclTask;
	while ((pclCurrent != pclTask_) && (pclCurrent != (FunkOS_Task*)NULL))
	{
		pclPrev = pclCurrent;
		pclCurrent = pclCurrent->m_pclNextList;
	}

	// If the item wasn't found in the list, bail.
	if (pclCurrent == (FunkOS_Task*)NULL)
	{
		return;
	}

	// Unlink the task from the list
	if (pclPrev != m_pclTask) // Not the head of the list
	{
		pclPrev->m_pclNextList = pclCurrent->m_pclNextList;
	}
	else // The head of the list
	{
		m_pclTask = pclCurrent->m_pclNextList;
	}
	
	pclTask_->m_pclNextList = (FunkOS_Task*)NULL; // reset the next semaphore pointer	
}

//---------------------------------------------------------------------------
/*!
	Wakes the highest priority task waiting for the mutex.
	\fn void FunkOS_Mutex::WakeNextPending(void)
*/
void FunkOS_Mutex::WakeNextPending(void)
{
	FunkOS_Task *pclCurrent = (FunkOS_Task*)NULL;
	FunkOS_Task *pclChosenOne = (FunkOS_Task*)NULL;
	
	UCHAR ucMaxPri = 0;

	// Check to see if there's anything in the task list.
	pclCurrent = m_pclTask;

	if (pclCurrent == (FunkOS_Task*)NULL)
	{
		// Reset the mutex - this is the last task waiting on the resource
		m_ucMaxPri = 0;
		m_usSem = 1;
		m_pclTask = (FunkOS_Task*)NULL;
	}
	else
	{
		// Start with first item chosen
		pclChosenOne = pclCurrent;
		
		while (pclCurrent != (FunkOS_Task*)NULL)
		{
			// Search by original priority
			if (pclCurrent->m_ucMutexPriority > ucMaxPri)
			{
				ucMaxPri = pclCurrent->m_ucMutexPriority;
				pclChosenOne = pclCurrent;
			}

			// Iterate through the list of pending tasks
			pclCurrent = pclCurrent->m_pclNextList;
		}

		// Remove the chosen one from the list
		DeleteFromList(pclChosenOne);
		
		// Move task to the ready state
		pclChosenOne->m_eState = TASK_READY;
	}
}
//---------------------------------------------------------------------------
/*!
	Adds a task to the specified mutex wait list.
    \fn void FunkOS_Mutex::AddToList(FunkOS_Task *pclTask_)
	\param pclTask_ - pointer to the task to put in the wait list
*/
void FunkOS_Mutex::AddToList(FunkOS_Task *pclTask_)
{
	if (m_pclTask == (FunkOS_Task*)NULL) // List is empty
	{
		// No task list to chain
		m_pclTask->m_pclNextList = (FunkOS_Task*)NULL; 
	}	
	else // List isn't empty
	{		
		// Chain the existing task list to the new task
		pclTask_->m_pclNextList = m_pclTask;		
	}
	m_pclTask = pclTask_; // Assign list to mutex
}
//---------------------------------------------------------------------------
/*!
	Checks the maximum priority in the mutex wait list.  If the new specified
	priority is	higher than the current maximum, it updates the mutex task list 
	and modifies the task priorities to the new level to prevent priority 
	inversion.
	\fn void FunkOS_Mutex::InheritPriority(UCHAR ucPriority_)
	\param ucPriority_ - priority to inherit
*/
void FunkOS_Mutex::InheritPriority(UCHAR ucPriority_)
{
	FunkOS_Task *pclCurrent;
	
	if (ucPriority_ > m_ucMaxPri)
	{	
		// Set the new maximum priority
		m_ucMaxPri = ucPriority_;

		// Go through and set the priorities for every task in the list to
		// the new maximum level.
		pclCurrent = m_pclTask;

		while (pclCurrent != NULL)
		{
			pclCurrent->m_ucPriority = ucPriority_;
			pclCurrent = pclCurrent->m_pclNextList;
		}	
	}
}
#endif //KERNEL_USE_MUTEX

