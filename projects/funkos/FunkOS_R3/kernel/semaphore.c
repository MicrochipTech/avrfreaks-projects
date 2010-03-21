//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  semaphore.c

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
static void Semaphore_WakeNextPending(SEMAPHORE_STRUCT *pstSem_);
static void Semaphore_DeleteFromList(SEMAPHORE_STRUCT *pstSem_, TASK_STRUCT *pstTask_);
static void Semaphore_AddToList(SEMAPHORE_STRUCT *pstSem_, TASK_STRUCT *pstTask_);

//---------------------------------------------------------------------------
/*!
	Initialize a binary semaphore by setting the semaphore value to 0 and emptying
	the task list.  Run this function for each semaphore prior to use.
	\fn BOOL Semaphore_Init(SEMAPHORE_STRUCT *pstSem_)
	\param pstSem_ - pointer to the semaphore to initialize
*/
void Semaphore_Init(SEMAPHORE_STRUCT *pstSem_)
{
	pstSem_->usSem = 0;
	pstSem_->usMax = 1;				//!< set this manually to implement counting semaphores.
	pstSem_->pstTask = NULL;
}

//---------------------------------------------------------------------------
/*!
	Signal a task to wait for a semaphore.  If the semaphore is not available,
	the task will block and wait until the semaphore becomes available.
	\fn BOOL Semaphore_Pend(SEMAPHORE_STRUCT *pstSem_, USHORT usTime_)
	\param pstSem_ - pointer to the semaphore to pend on
	\param usTime_ - the time limit to wait for the semaphore. Integer number of ticks or TIME_FOREVER
	\return BOOL - TRUE on success, FALSE on timeout
*/
BOOL Semaphore_Pend(SEMAPHORE_STRUCT *pstSem_,  USHORT usTime_)
{
	BOOL bTaskWait = FALSE;
	BOOL bReturn = TRUE;
	TASK_STRUCT *pstTask;
	//!! Disable Scheduler !!
	CS_ENTER();
	pstTask = Task_GetCurrentTask();
	if (pstSem_->usSem != 0)
	{
		// Semaphore isn't claimed, claim it.
		pstSem_->usSem--;
	}
	else
	{	
		// Remove the task from the ready state - blocking on semaphore
		pstTask->eState = TASK_BLOCKED;
		pstTask->usTimeLeft = usTime_;

		// Add the task to the semaphore's waiting list.
		Semaphore_AddToList(pstSem_, pstTask);
	
		bTaskWait = TRUE;
	}
	CS_EXIT();
	
	if (bTaskWait)
	{
		// Switch tasks immediately
		Task_YieldSWI();
	
		CS_ENTER();
		if (pstTask->bTimeout)
		{
			// If there was a timeout waiting for the semaphore
			bReturn = FALSE;
			Semaphore_DeleteFromList(pstSem_, pstTask);
			pstTask->bTimeout = FALSE;	// Reset the task timeout flag				
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
	\fn BOOL Semaphore_Post(SEMAPHORE_STRUCT *pstSem_)
	\param pstSem_ - pointer to the semaphore to post
*/
void Semaphore_Post(SEMAPHORE_STRUCT *pstSem_)
{	
	BOOL bTaskWait = FALSE;
	
	//!! Disable scheduler + interrupts (callable from ISR)
	CS_ENTER();
	// Nothing is pending
	if (pstSem_->pstTask == NULL)
	{
		// Re-initialize the semaphore
		if (pstSem_->usSem < pstSem_->usMax)
		{
			pstSem_->usSem++;
		}
	}
	else
	{
		// Wake the highest priority task pending on the semaphore
		Semaphore_WakeNextPending(pstSem_);
		bTaskWait = TRUE;
	}	
	CS_EXIT();
	
	if (bTaskWait)
	{
		Task_YieldSWI();
	}
}
//---------------------------------------------------------------------------
/*!
	Adds a task to the semaphore wait list.  Used internally by the semaphore library.
	\fn static void Semaphore_AddToList(SEMAPHORE_STRUCT *pstSem_, TASK_STRUCT *pstTask_)
	\param pstSem_ - pointer to the semaphore 
	\param pstTask_ - pointer to the task to add to the semaphore list
*/
static void Semaphore_AddToList(SEMAPHORE_STRUCT *pstSem_, TASK_STRUCT *pstTask_)
{	
	if (pstSem_->pstTask == NULL) // List is empty
	{
		// No task list to chain
		pstTask_->pstNextList = NULL; 
	}	
	else // List isn't empty
	{		
		// Chain the existing task list to the new task
		pstTask_->pstNextList = pstSem_->pstTask;		
	}
	pstSem_->pstTask = pstTask_; // Assign list to semaphore
}
//---------------------------------------------------------------------------
/*!
	Remove a task from the semaphore wait list.  
	\fn static void Semaphore_DeleteFromList(SEMAPHORE_STRUCT *pstSem_, TASK_STRUCT *pstTask_)
	\param pstSem_ - pointer to the semaphore 
	\param pstTask_ - pointer to the task to remove from the wait list
*/
static void Semaphore_DeleteFromList(SEMAPHORE_STRUCT *pstSem_, TASK_STRUCT *pstTask_)
{
	TASK_STRUCT *pstCurrent;
	TASK_STRUCT *pstPrev;
	
	// Check to see if there's anything in the task list.
	pstCurrent = pstSem_->pstTask;

	// Find the item in the list
	pstPrev = pstSem_->pstTask;
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
	if (pstPrev != pstSem_->pstTask) // Not the head of the list
	{
		pstPrev->pstNextList = pstCurrent->pstNextList;
	}
	else // The head of the list
	{
		pstSem_->pstTask = pstCurrent->pstNextList;
	}

	pstTask_->pstNextList = NULL; // reset the next semaphore pointer	
}
//---------------------------------------------------------------------------
/*!
	Wakes the next highest priority task waiting on the semaphore.
	\fn static void Semaphore_WakeNextPending(SEMAPHORE_STRUCT *pstSem_)
	\param pstSem_ - pointer to the semaphore 
*/
static void Semaphore_WakeNextPending(SEMAPHORE_STRUCT *pstSem_)
{
	TASK_STRUCT *pstCurrent;
	TASK_STRUCT *pstChosenOne;
	
	UCHAR ucMaxPri = 0;

	// Check to see if there's anything in the task list.
	pstCurrent = pstSem_->pstTask;
	pstChosenOne = pstSem_->pstTask;	

	// Find the highest priority item in the list	
	while (pstCurrent != NULL)
	{
		if (pstCurrent->ucPriority > ucMaxPri)
		{
			ucMaxPri = pstCurrent->ucPriority;
			pstChosenOne = pstCurrent;
		}

		// Iterate through the list of pending semaphores
		pstCurrent = pstCurrent->pstNextList;
	}
	
	// Remove the chosen one from the linked list.
	Semaphore_DeleteFromList(pstSem_, pstChosenOne);	

	// Move task to the ready state
	pstChosenOne->eState = TASK_READY;	
}
//---------------------------------------------------------------------------
#endif //KERNEL_USE_SEMAPHORE
