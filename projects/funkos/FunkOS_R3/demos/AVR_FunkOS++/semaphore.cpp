//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  semaphore.cpp

	Description:
		Binary/Counting semaphore implementation for FunkOS.
*/
#include "semaphore.h"
#include "types.h"
#include "task.h"
#include "taskport.h"
#include "kernelcfg.h"

#if KERNEL_USE_SEMAPHORE

//---------------------------------------------------------------------------
/*!
	Initialize a binary semaphore by setting the semaphore value to 0 and emptying
	the task list.  Run this function for each semaphore prior to use.  By default
    a semaphore is initialized as a binary semaphore.
	\fn FunkOS_Semaphore::FunkOS_Semaphore()
*/
FunkOS_Semaphore::FunkOS_Semaphore()
{
	m_usSem = 0;
	m_usMax = 1;				//!< set this manually to implement counting semaphores.
	m_pclTask = (FunkOS_Task*)NULL;
}

//---------------------------------------------------------------------------
/*!
	Initialize a binary semaphore by setting the semaphore value to 0 and emptying
	the task list.  Run this function for each semaphore prior to use.  By specifying
    a paramter value, the semaphore is initialized as a counting semaphore.
	\fn FunkOS_Semaphore::FunkOS_Semaphore(USHORT usMaxVal_)
*/
FunkOS_Semaphore::FunkOS_Semaphore(USHORT usMaxVal_)
{
	m_usSem = 0;
	m_usMax = usMaxVal_;				
	m_pclTask = (FunkOS_Task*)NULL;
}

//---------------------------------------------------------------------------
/*!
	Signal a task to wait for a semaphore.  If the semaphore is not available,
	the task will block and wait until the semaphore becomes available.
	\fn BOOL FunkOS_Semaphore::Pend(USHORT usTime_)
	\param usTime_ - the time limit to wait for the semaphore. Integer number of ticks or TIME_FOREVER
	\return BOOL - TRUE on success, FALSE on timeout
*/
BOOL FunkOS_Semaphore::Pend(USHORT usTime_)
{
	BOOL bTaskWait = FALSE;
	BOOL bReturn = TRUE;
	FunkOS_Task *pclTask;
	//!! Disable Scheduler !!
	CS_ENTER();
	pclTask = FunkOS_Scheduler::GetCurrentTask();
	
	if (m_usSem != 0)
	{
		// Semaphore isn't claimed, claim it.
		m_usSem--;
	}
	else
	{	
		// Remove the task from the ready state - blocking on semaphore
		pclTask->m_eState = TASK_BLOCKED;
		pclTask->m_usTimeLeft = usTime_;

		// Add the task to the semaphore's waiting list.
		AddToList(pclTask);
	
		bTaskWait = TRUE;
	}
	CS_EXIT();
	
	if (bTaskWait)
	{
		// Switch tasks immediately
		FunkOS_Task::Yield();
	
		CS_ENTER();
		if (pclTask->m_bTimeout)
		{
			// If there was a timeout waiting for the semaphore
			bReturn = FALSE;
			DeleteFromList(pclTask);
			pclTask->m_bTimeout = FALSE;	// Reset the task timeout flag				
		}
		CS_EXIT();
	}
	
	return bReturn;
}
//---------------------------------------------------------------------------
/*!
	Post (release) a semaphore.  If there are other tasks waiting for this 
	semaphore, the highest-priority task in the semaphore wait list will claim
	the semaphore and run.
	\fn void FunkOS_Semaphore::Post(void)
*/
void FunkOS_Semaphore::Post(void)
{	
	BOOL bTaskWait = FALSE;
	
	//!! Disable scheduler + interrupts (callable from ISR)
	CS_ENTER();
	// Nothing is pending
	if (m_pclTask == NULL)
	{
		// Re-initialize the semaphore
		if (m_usSem < m_usMax)
		{
			m_usSem++;
		}
	}
	else
	{
		// Wake the highest priority task pending on the semaphore
		WakeNextPending();
		bTaskWait = TRUE;
	}	
	CS_EXIT();
	
	if (bTaskWait)
	{
		FunkOS_Task::Yield();
	}
}
//---------------------------------------------------------------------------
/*!
	Adds a task to the semaphore wait list.  Used internally by the semaphore library.
	\fn void FunkOS_Semaphore::AddToList(FunkOS_Task *pclTask_)
	\param pclTask_ - pointer to the task to add to the semaphore list
*/
void FunkOS_Semaphore::AddToList(FunkOS_Task *pclTask_)
{	
	if (m_pclTask == NULL) // List is empty
	{
		// No task list to chain
		pclTask_->m_pclNextList = (FunkOS_Task*)NULL; 
	}	
	else // List isn't empty
	{		
		// Chain the existing task list to the new task
		pclTask_->m_pclNextList = m_pclTask;		
	}
	m_pclTask = pclTask_; // Assign list to semaphore
}
//---------------------------------------------------------------------------
/*!
	Remove a task from the semaphore wait list.  
	\fn void FunkOS_Semaphore::DeleteFromList(FunkOS_Task *pclTask_)
	\param pclTask_ - pointer to the task to remove from the wait list
*/
void FunkOS_Semaphore::DeleteFromList(FunkOS_Task *pclTask_)
{
	FunkOS_Task *pclCurrent;
	FunkOS_Task *pclPrev;
	
	// Check to see if there's anything in the task list.
	pclCurrent = m_pclTask;

	// Find the item in the list
	pclPrev = m_pclTask;
	while ((pclCurrent != pclTask_) && (pclCurrent != NULL))
	{
		pclPrev = pclCurrent;
		pclCurrent = pclCurrent->m_pclNextList;
	}

	// If the item wasn't found in the list, bail.
	if (pclCurrent == NULL)
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
	Wakes the next highest priority task waiting on the semaphore.
	\fn void FunkOS_Semaphore::WakeNextPending(void)
*/
void FunkOS_Semaphore::WakeNextPending(void)
{
	FunkOS_Task *pclCurrent;
	FunkOS_Task *pclChosenOne;
	
	UCHAR ucMaxPri = 0;

	// Check to see if there's anything in the task list.
	pclCurrent = m_pclTask;
	pclChosenOne = m_pclTask;	

	// Find the highest priority item in the list	
	while (pclCurrent != (FunkOS_Task*)NULL)
	{
		if (pclCurrent->m_ucPriority > ucMaxPri)
		{
			ucMaxPri = pclCurrent->m_ucPriority;
			pclChosenOne = pclCurrent;
		}

		// Iterate through the list of pending semaphores
		pclCurrent = pclCurrent->m_pclNextList;
	}
	
	// Remove the chosen one from the linked list.
	DeleteFromList(pclChosenOne);	

	// Move task to the ready state
	pclChosenOne->m_eState = TASK_READY;	
}
//---------------------------------------------------------------------------
#endif //KERNEL_USE_SEMAPHORE
