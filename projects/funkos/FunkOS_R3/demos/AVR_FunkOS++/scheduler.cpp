//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  scheduler.cpp

	Description:
		C++ RTOS scheduler implementation
*/
//---------------------------------------------------------------------------

#include "types.h"
#include "kernelcfg.h"
#include "task.h"
#include "taskport.h"
#include "scheduler.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "kernelcfg.h"

FunkOS_Task *m_pclCurrentTask;

static SHORT m_sCriticalCount;
static BOOL m_bGlobalTaskEnable;

void FunkOS_Scheduler::Init(void)
{
	m_pclCurrentTask = (FunkOS_Task*)NULL;
	m_sCriticalCount = 0;
	m_bGlobalTaskEnable = FALSE; 
}

//---------------------------------------------------------------------------
/*!
	Add a task to the scheduler list
	\fn BOOL FunkOS_Scheduler::Add(FunkOS_Task *pclTask_)
	\param pclTask_ - pointer to the task to add
	\return (BOOL) TRUE on success, FAIL on failure
*/
BOOL FunkOS_Scheduler::Add(FunkOS_Task *pclTask_)
{
	CS_ENTER();
	
	// init the task state
	pclTask_->m_eState = TASK_READY;
		
	// If the list is empty...
	if (m_pclCurrentTask == NULL)
	{
		// Circularly linked list...
		m_pclCurrentTask = pclTask_;
		m_pclCurrentTask->m_pclNext = m_pclCurrentTask;
	}
	// The list isn't empty, insert it somewhere.
	else 
	{
		// reconnect the links...
		pclTask_->m_pclNext = m_pclCurrentTask->m_pclNext;
		m_pclCurrentTask->m_pclNext = pclTask_;
	}
	
	CS_EXIT();
	
	// It's all good - no maximum items in the list...
	return TRUE;
}
//---------------------------------------------------------------------------
/*!
	Remove a task from the scheduler list.  Before attempting to remove the
	task, the owner should ensure that all connections of this task to other
	objects (plumber, semaphore, etc.) are removed PRIOR to calling this
	function, otherwise system corruption will likely occur.
	\fn BOOL FunkOS_Scheduler::Remove(FunkOS_Task *pclTask_)
	\sa Task_Add()
	\param pclTask_ - pointer to the task to remove
	\return (BOOL) TRUE on success, FAIL on failure
*/
BOOL FunkOS_Scheduler::Remove(FunkOS_Task *pclTask_)
{	
	FunkOS_Task *pclPrev;
	FunkOS_Task *pclTarget;
	
	CS_ENTER();
	
	// initialize the target
	pclPrev = pclTask_;
	pclTarget = pclTask_->m_pclNext;
	
	// Look through the circular list of tasks and find the connectors.
	while (pclTarget != pclTask_)
	{
		// iterate through...
		pclPrev = pclTarget;
		pclTarget = pclTarget->m_pclNext;
	}
	// Remove the target from the circularly linked list
	pclPrev->m_pclNext = pclTask_->m_pclNext;
	
	// De-initialize the "next" pointer in the task list.
	pclTask_->m_pclNext = (FunkOS_Task*)NULL;
	
	// Set the task as uninitialized (require initialization before reuse)
	pclTask_->m_eState = TASK_UNINIT;
	
	CS_EXIT();
	
	// Call a context switch.
	FunkOS_Task::Yield();
	
	// Will not return if the thread deletes itself.	
	return FALSE;
}

//---------------------------------------------------------------------------
/*!
	This, along with the RTOS Tick is the heart of the kernel.  This scheduler
	is called whenever a context switch needs to take place, which is anytime
	a Semaphore is set, a task manually yields control, an RTOS tick takes 
	place, a task is set to sleep, or a task is left to pend on a semaphore.
	
	The scheduler is priority-based, and is fully pre-emptive.  Where there 
	are multiple tasks sharing the same priority, the scheduler is round-robin,
	alternating through all *ready* tasks of the same priority group.
	
	\fn void FunkOS_Scheduler::Switch(void) 
*/

void FunkOS_Scheduler::Switch(void) 
{
	int iHighPri;	
	FunkOS_Task *pclEnd;
	FunkOS_Task *pclNextTask;

	// Set highest priority to idle at first.
	iHighPri = 0;

	//-----------------------------------------------------------------------
	// Set the next task level to run as that of the highest of the ready or 
	// running tasks.
	//-----------------------------------------------------------------------
	pclEnd = m_pclCurrentTask;
	pclNextTask = m_pclCurrentTask;
	
	do 
	{
		// If the task is ready to run, or it's currently running...
		if (pclNextTask->m_eState == TASK_READY)
		{
			// and the priority is > highest priority
			if (pclNextTask->m_ucPriority > iHighPri)
			{
				// Assign the priority level to run
				iHighPri = pclNextTask->m_ucPriority;
			}
		}

		// Next in the list...
		pclNextTask = pclNextTask->m_pclNext;

	} while (pclEnd != pclNextTask);	// Haven't gone through the entire list.

	//-----------------------------------------------------------------------
	// if there are multiple tasks in the task list that is able to run at
	// the current highest level, then round-robin schedule among the priority 
	// group.
	//-----------------------------------------------------------------------
	pclNextTask = m_pclCurrentTask->m_pclNext;
	pclEnd = pclNextTask;

	do
	{
		// Task is capable of running
		if (pclNextTask->m_eState == TASK_READY)
		{
			// Or has the same priority as the highest priority task
			if (pclNextTask->m_ucPriority == iHighPri)
			{
				// Set the new task
				break;
			}
		}
		
		// Next...
		pclNextTask = pclNextTask->m_pclNext;
	} while (pclEnd != pclNextTask);

	// Set the task pointer...		
	m_pclCurrentTask = pclNextTask;

#if KERNEL_USE_QUANTUM
	m_pclCurrentTask->m_usTimeLeft = m_pclCurrentTask->m_usQuantum;
#endif // KERNEL_USE_QUANTUM

#if KERNEL_STACK_DEBUG
	//Task_StackCheck();
#endif // KERNEL_STACK_DEBUG
}
//---------------------------------------------------------------------------
/*!
	This function is called at a regular interval (the RTOS Tick interval) and
	is used to update the task time for each task in the system.
	
	For tasks that are blocked on semaphores, the semaphores are checked - if
	the semaphore is now available, the function will take the semaphore, and
	place the task in the ready state.  If a timeout occurs on a semaphore, 
	the semaphore is set to NULL - this condition should be checked by the
	application to ensure that correct error handling occurs.
	
	Tasks that are sleeping have their sleep time decremented, and are placed
	in the ready state when the timeout expires.  
	
	Tasks in the ready state have their starvation interval incremented.  
	
	\fn BOOL FunkOS_Scheduler::Tick(void)
*/
BOOL FunkOS_Scheduler::Tick(void)
{
	FunkOS_Task *pclEnd;
	FunkOS_Task *pclTemp;

	BOOL bRunScheduler = FALSE;
	
	pclEnd = m_pclCurrentTask;
	pclTemp = m_pclCurrentTask;

#if KERNEL_USE_QUANTUM
	// Decrement time from the current task
	if (m_pclCurrentTask->m_usTimeLeft)
	{
		if (m_pclCurrentTask->m_usTimeLeft != TIME_FOREVER)
		{
			// Decrement the quantum timer
			m_pclCurrentTask->m_usTimeLeft--;
		
			// Timer expired?
			if (m_pclCurrentTask->m_usTimeLeft == 0)
			{
				// Run the scheduler
				bRunScheduler = TRUE;
				// Set the task run quantum how long the task will run when multiple
				// tasks of the same priority are waiting...
				m_pclCurrentTask->m_usTimeLeft = m_pclCurrentTask->m_usQuantum;
			}
		}
	}
#endif // KERNEL_USE_QUANTUM

	// For each task in the list, update the times, change states if necessary
	// Note that these operations aren't explicitly done in a critical section
	// because this task is an INTERRUPT CONTEXT event, and thus the highest
	// priority event in the system.
	do
	{
#if KERNEL_USE_SEMAPHORE || KERNEL_USE_MUTEX
		if (pclTemp->m_eState == TASK_BLOCKED)
		{						
			// And the time left is valid (not zero and not infinity)
			if ((pclTemp->m_usTimeLeft != TIME_FOREVER)
				&&(pclTemp->m_usTimeLeft != 0))
			{
				// decrement the time
				pclTemp->m_usTimeLeft--;
			}
			// If the time has expired
			else if (pclTemp->m_usTimeLeft == 0)
			{
				// Time expired - set as TIMED OUT (only semaphore or mutex does this...)
				pclTemp->m_eState = TASK_READY;
				pclTemp->m_bTimeout = TRUE;

#if KERNEL_USE_QUANTUM
				pclTemp->m_usTimeLeft = pclTemp->m_usQuantum;
#endif // KERNEL_USE_QUANTUM
				
				// If the task is higher priority than the current task, run the scheduler
				if (pclTemp->m_ucPriority > m_pclCurrentTask->m_ucPriority)
				{
					bRunScheduler = TRUE;
				}
			}			
		}
		else 
#endif //KERNEL_USE_SEMAPHORE || KERNEL_USE_MUTEX
		if (pclTemp->m_eState == TASK_SLEEP)
		{
			// Check just the time
			if (pclTemp->m_usTimeLeft != TIME_FOREVER)
			{
				pclTemp->m_usTimeLeft--;
			}

			if (pclTemp->m_usTimeLeft == 0)
			{				

				// Time expired - go to ready state.
				pclTemp->m_eState = TASK_READY;

#if KERNEL_USE_QUANTUM
				pclTemp->m_usTimeLeft = pclTemp->m_usQuantum;
#endif // KERNEL_USE_QUANTUM
				
				// If the task is higher priority than the current task, run the scheduler
				if (pclTemp->m_ucPriority > m_pclCurrentTask->m_ucPriority)
				{
					bRunScheduler = TRUE;
				}
			}
		}			

		pclTemp = pclTemp->m_pclNext;
	} while (pclEnd != pclTemp);
	
	// Don't ask to run the scheduler if it's disabled...
	if (m_bGlobalTaskEnable == FALSE)
	{
		return FALSE;
	}
	
	// If this is false, don't run the scheduler... there's nothing to do.
	return bRunScheduler;
}
//---------------------------------------------------------------------------
/*!
	Returns the current state of the scheduler.

	\fn BOOL FunkOS_Scheduler::IsEnabled(void)
	\return BOOL - TRUE if enabled, FALSE if disabled
*/
BOOL FunkOS_Scheduler::IsEnabled(void)
{
	return m_bGlobalTaskEnable;
}
//---------------------------------------------------------------------------
/*!
	Set the state of the scheduler.

	\fn void FunkOS_Scheduler::Set(BOOL bEnable_)
	\param bEnable_ - TRUE to enable the scheduler, FALSE to disable
*/
void FunkOS_Scheduler::Set(BOOL bEnable_)
{
	m_bGlobalTaskEnable = bEnable_;
}
//---------------------------------------------------------------------------
/*!
	Disables the scheduler, and returns the scheduler's previous state.  This
	is used in combination with Task_SchedulerRestore() to provide the 
	scheduler-disabled context.

	\fn BOOL FunkOS_Scheduler::Disable(void)
	\return BOOL - the previous state of the scheduler (TRUE = ENABLED)
*/
BOOL FunkOS_Scheduler::Disable(void)
{
	BOOL bReturn;
	
	CS_ENTER();
	bReturn = IsEnabled();
	Set(FALSE);
	CS_EXIT();
	
	return bReturn;
}
//---------------------------------------------------------------------------
/*!
	Used to restore the state of the scheduler after performing an operation
	that operates in a scheduler-disabled context

	\fn void FunkOS_Scheduler::Restore(BOOL bState_)
	\param bState_ - TRUE to enable the scheduler, FALSE to disable the scheduler
*/
void FunkOS_Scheduler::Restore(BOOL bState_)
{
	CS_ENTER();
	Set(bState_);
	CS_EXIT();
}

//---------------------------------------------------------------------------
/*!
    Returns the scheduler's currently running task

	\fn FunkOS_Task* FunkOS_Scheduler::GetCurrentTask(void)
	\param FunkOS_Task* - the pointer to the currently running task.

*/
FunkOS_Task* FunkOS_Scheduler::GetCurrentTask(void)
{
	FunkOS_Task *pclCurrent;
	CS_ENTER();
	pclCurrent = m_pclCurrentTask;
	CS_EXIT();
	
	return pclCurrent;
}
