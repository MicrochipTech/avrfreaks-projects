//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  task.h

	Description:
		Portable task module - tick and task switching implementation
*/
//---------------------------------------------------------------------------

#include "types.h"
#include "kernelcfg.h"
#include "task.h"
#include "taskport.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "kernelcfg.h"

//---------------------------------------------------------------------------
extern TASK_STRUCT *pstCurrentTask;
extern SHORT sCriticalCount;
static BOOL bGlobalTaskEnable = FALSE;
//---------------------------------------------------------------------------
#if KERNEL_STACK_DEBUG
static void Task_StackCheck(void);
#endif //KERNEL_STACK_DEBUG
//---------------------------------------------------------------------------
/*!
	Initialize the module's internal variables.
	\sa Task_Init()
*/
void Task_Init(void)
{
	pstCurrentTask = NULL;
    sCriticalCount = 0;
}
//---------------------------------------------------------------------------
/*!
	Add a task to the scheduler list
	\fn BOOL Task_Add(TASK_STRUCT *pstTask_)
	\sa Task_Remove()
	\param pstTask_ - pointer to the task structure to add
	\return (BOOL) TRUE on success, FAIL on failure
*/
BOOL Task_Add(TASK_STRUCT *pstTask_)
{
	CS_ENTER();
	
	// init the task state
	pstTask_->eState = TASK_READY;
		
	// If the list is empty...
	if (pstCurrentTask == NULL)
	{
		// Circularly linked list...
		pstCurrentTask = pstTask_;
		pstCurrentTask->pstNext = pstCurrentTask;
	}
	// The list isn't empty, insert it somewhere.
	else 
	{
		// reconnect the links...
		pstTask_->pstNext = pstCurrentTask->pstNext;
		pstCurrentTask->pstNext = pstTask_;
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
	\fn BOOL Task_Remove(TASK_STRUCT *pstTask_)
	\sa Task_Add()
	\param pstTask_ - pointer to the task structure to remove
	\return (BOOL) TRUE on success, FAIL on failure
*/
BOOL Task_Remove(TASK_STRUCT *pstTask_)
{	
	TASK_STRUCT *pstPrev;
	TASK_STRUCT *pstTarget;
	
	CS_ENTER();
	
	// initialize the target
	pstPrev = pstTask_;
	pstTarget = pstTask_->pstNext;
	
	// Look through the circular list of tasks and find the connectors.
	while (pstTarget != pstTask_)
	{
		// iterate through...
		pstPrev = pstTarget;
		pstTarget = pstTarget->pstNext;
	}
	// Remove the target from the circularly linked list
	pstPrev->pstNext = pstTask_->pstNext;
	
	// De-initialize the "next" pointer in the task list.
	pstTask_->pstNext = NULL;
	
	// Set the task as uninitialized (require initialization before reuse)
	pstTask_->eState = TASK_UNINIT;
	
	CS_EXIT();
	
	// Call a context switch.
	Task_YieldSWI();
	
	// Will not return if the thread deletes itself.	
	return FALSE;
}

//---------------------------------------------------------------------------
/*!
	This function populates a given task structure with the parameters given.
	The stack is then initialized, and the task is set as stopped, and all
	state variables and timeouts reset.  Once the task is created, it must
	be added using Task_Add(), and started using Tast_Start()
	\fn void Task_CreateTask(	TASK_STRUCT *pstTask_, 
						BYTE *pcName_,
						WORD* pwStack_,
						USHORT usStackSize_,
						UCHAR ucPriority_,
						TASK_FUNC pfFunction_)
	\sa Task_Add(), Task_Start()
	\brief Add a task from the scheduler list
	\sa Task_Remove()
	\param pstTask_ - pointer to the task structure to build
	\param pcName_ - pointer to the task name
	\param pwStack_ - pointer to the task's stack
	\param usStackSize_ - the length of the stack in bytes
	\param ucPriority_ - priority of the task (0-255)
	\param pfFunction_ - pointer to the task's entry function
	\return (BOOL) TRUE on success, FAIL on failure
*/
void Task_CreateTask(	TASK_STRUCT *pstTask_, 
						BYTE *pcName_,
						WORD* pwStack_,
						USHORT usStackSize_,
						UCHAR ucPriority_,
						TASK_FUNC pfFunction_)
{
	// Assign the task name
	pstTask_->pacName = pcName_;
	
	// Stack size
	pstTask_->usStackSize = usStackSize_;
	
	// Assign the task priority
	pstTask_->ucPriority = ucPriority_;
	
	// Set the task to the "stopped" state
	pstTask_->eState = TASK_STOPPED;	
	
	// Set the stack pointer to the top of the task -> grow down/up depending on architecture.
	pstTask_->pwTopStack = (WORD*)TOP_OF_STACK(pwStack_, usStackSize_);
	pstTask_->pwStack = pwStack_;
	// Set the task entry function to the one provided
	pstTask_->pfTaskFunc = pfFunction_;
	
	// Wait time to 0 on init.
	pstTask_->usTimeLeft = 0;	

	// Set the quantum to 1 tick
#if KERNEL_USE_QUANTUM
	pstTask_->usQuantum = 1;
#endif // KERNEL_USE_QUANTUM
	
#if KERNEL_USE_SEMAPHORE
	pstTask_->pstNextList = NULL;
#endif // KERNEL_USE_SEMAPHORE

#if KERNEL_USE_MUTEX
	pstTask_->pstNextList = NULL;
	pstTask_->ucMutexPriority = 0;
#endif // KERNEL_USE_MUTEX

	// Initialize the stack...
	Task_InitStack(pstTask_);
}

#if KERNEL_USE_QUANTUM
//---------------------------------------------------------------------------
/*!
	Set the time between context switches when multiple tasks are executing
	at the same priority level
	\fn void Task_SetQuantum(TASK_STRUCT *pstTask_, USHORT usTicks_)
	\param pstTask_ - pointer to the task to start
	\param usTicks_ - the number of ticks inbetween switching
*/
void Task_SetQuantum(TASK_STRUCT *pstTask_, USHORT usTicks_)
{
	if (pstTask_ != NULL)
	{
		pstTask_->usQuantum = usTicks_;
	}
}

#endif // KERNEL_USE_QUANTUM

//---------------------------------------------------------------------------
/*!
	Set a task as ready to run
	\fn BOOL Task_Start(TASK_STRUCT *pstTask_)
	\sa Task_Stop()
	\param pstTask_ - pointer to the task to start
*/
void Task_Start(TASK_STRUCT *pstTask_)
{
	// Place task in the ready state
	pstTask_->eState = TASK_READY;	

#if KERNEL_USE_QUANTUM
	pstTask_->usTimeLeft = pstTask_->usQuantum;
#endif // KERNEL_USE_QUANTUM
}

//---------------------------------------------------------------------------
/*!
	Disable a task from running
	\fn BOOL Task_Stop(TASK_STRUCT *pstTask_)
	\sa Task_Start()
	\param pstTask_ - pointer to the task to stop
*/
void Task_Stop(TASK_STRUCT *pstTask_)
{
	// Place task in the stopped state
	pstTask_->eState = TASK_STOPPED;
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
	
	\sa Task_Tick()
	\fn void Task_Switch(void)
*/
void Task_Switch(void) 
{
	int iHighPri;	
	TASK_STRUCT *pstEnd;
	TASK_STRUCT *pstNextTask;

	// Set highest priority to idle at first.
	iHighPri = 0;

	//-----------------------------------------------------------------------
	// Set the next task level to run as that of the highest of the ready or 
	// running tasks.
	//-----------------------------------------------------------------------
	pstEnd = pstCurrentTask;
	pstNextTask = pstCurrentTask;
	do 
	{
		// If the task is ready to run, or it's currently running...
		if (pstNextTask->eState == TASK_READY)
		{
			// and the priority is > highest priority
			if (pstNextTask->ucPriority > iHighPri)
			{
				// Assign the priority level to run
				iHighPri = pstNextTask->ucPriority;
			}
		}

		// Next in the list...
		pstNextTask = pstNextTask->pstNext;

	} while (pstEnd != pstNextTask);	// Haven't gone through the entire list.

	//-----------------------------------------------------------------------
	// if there are multiple tasks in the task list that is able to run at
	// the current highest level, then round-robin schedule among the priority 
	// group.
	//-----------------------------------------------------------------------
	pstNextTask = pstCurrentTask->pstNext;
	pstEnd = pstNextTask;

	do
	{
		// Task is capable of running
		if (pstNextTask->eState == TASK_READY)
		{
			// Or has the same priority as the highest priority task
			if (pstNextTask->ucPriority == iHighPri)
			{
				// Set the new task
				break;
			}
		}
		
		// Next...
		pstNextTask = pstNextTask->pstNext;
	} while (pstEnd != pstNextTask);

	// Set the task pointer...		
	pstCurrentTask = pstNextTask;

#if KERNEL_USE_QUANTUM
	pstCurrentTask->usTimeLeft = pstCurrentTask->usQuantum;
#endif // KERNEL_USE_QUANTUM

#if KERNEL_STACK_DEBUG
	Task_StackCheck();
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
	
	\sa Task_Switch()
	\fn BOOL Task_Tick(void)
*/
BOOL Task_Tick(void)
{
	TASK_STRUCT *pstEnd;
	TASK_STRUCT *pstTemp;

	BOOL bRunScheduler = FALSE;
	
	pstEnd = pstCurrentTask;
	pstTemp = pstCurrentTask;

#if KERNEL_USE_QUANTUM
	// Decrement time from the current task
	if (pstCurrentTask->usTimeLeft)
	{
		if (pstCurrentTask->usTimeLeft != TIME_FOREVER)
		{
			// Decrement the quantum timer
			pstCurrentTask->usTimeLeft--;
		
			// Timer expired?
			if (pstCurrentTask->usTimeLeft == 0)
			{
				// Run the scheduler
				bRunScheduler = TRUE;
				// Set the task run quantum how long the task will run when multiple
				// tasks of the same priority are waiting...
				pstCurrentTask->usTimeLeft = pstCurrentTask->usQuantum;
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
		if (pstTemp->eState == TASK_BLOCKED)
		{						
			// And the time left is valid (not zero and not infinity)
			if ((pstTemp->usTimeLeft != TIME_FOREVER)
				&&(pstTemp->usTimeLeft != 0))
			{
				// decrement the time
				pstTemp->usTimeLeft--;
			}
			// If the time has expired
			else if (pstTemp->usTimeLeft == 0)
			{
				// Time expired - set as TIMED OUT (only semaphore or mutex does this...)
				pstTemp->eState = TASK_READY;
				pstTemp->bTimeout = TRUE;

#if KERNEL_USE_QUANTUM
				pstTemp->usTimeLeft = pstTemp->usQuantum;
#endif // KERNEL_USE_QUANTUM
				
				// If the task is higher priority than the current task, run the scheduler
				if (pstTemp->ucPriority > pstCurrentTask->ucPriority)
				{
					bRunScheduler = TRUE;
				}
			}			
		}
		else 
#endif //KERNEL_USE_SEMAPHORE || KERNEL_USE_MUTEX
		if (pstTemp->eState == TASK_SLEEP)
		{
			// Check just the time
			if (pstTemp->usTimeLeft != TIME_FOREVER)
			{
				pstTemp->usTimeLeft--;
			}

			if (pstTemp->usTimeLeft == 0)
			{				

				// Time expired - go to ready state.
				pstTemp->eState = TASK_READY;

#if KERNEL_USE_QUANTUM
				pstTemp->usTimeLeft = pstTemp->usQuantum;
#endif // KERNEL_USE_QUANTUM
				
				// If the task is higher priority than the current task, run the scheduler
				if (pstTemp->ucPriority > pstCurrentTask->ucPriority)
				{
					bRunScheduler = TRUE;
				}
			}
		}			

		pstTemp = pstTemp->pstNext;
	} while (pstEnd != pstTemp);
	
	// Don't ask to run the scheduler if it's disabled...
	if (bGlobalTaskEnable == FALSE)
	{
		return FALSE;
	}
	
	// If this is false, don't run the scheduler... there's nothing to do.
	return bRunScheduler;
}
//---------------------------------------------------------------------------
/*!
	Set a task to sleep for a period of time specified in the arguments
	\sa Task_Tick()
	\fn void Task_Sleep(USHORT usTime_)
	\param usTime_ - the time period in RTOS ticks to sleep through
*/
void Task_Sleep(USHORT usTime_)
{
	// Do this in a scheduler-disabled context
	CS_ENTER();
		
	pstCurrentTask->eState = TASK_SLEEP;
	pstCurrentTask->usTimeLeft = usTime_;
	
	CS_EXIT();
		
	// Call the scheduler immediately.
	Task_YieldSWI();
}

//---------------------------------------------------------------------------
/*!
	Trigger a software interrupt that will invoke the context switch.  This
	is the best mechanism for triggering a context switch from an application
	or system-service level, as it is completely thread-safe and can be
	invoked from an interrupt.  
	
	\fn Task_YieldSWI()
	\sa Task_Switch()
*/
void Task_YieldSWI(void)
{
	KernelSWI_Trigger();
}

#if KERNEL_STACK_DEBUG
//---------------------------------------------------------------------------
/*!
	Check the current task for its stack depth.  Useful for debugging where
	the slack needs to be known - especially for systems with multiple 
	levels of nested interrupts.

	\fn Task_StackCheck(void)
*/
static void Task_StackCheck(void)
{
	SHORT i;
	USHORT usMargin = 0;

	UCHAR *pucStack;
	pucStack = (UCHAR*)pstCurrentTask->pwStack;

	for (i = 0; i < pstCurrentTask->usStackSize; i++)
	{
		if (pucStack[i] != 0xFF)
		{
			break;
		}
		else
		{
			usMargin++;
		}
	}

	pstCurrentTask->usStackMargin = usMargin;
}
#endif // KERNEL_STACK_DEBUG
//---------------------------------------------------------------------------
/*!
	Returns the current state of the scheduler.

	\fn Task_IsSchedulerEnabled(void)
	\return BOOL - TRUE if enabled, FALSE if disabled
*/
BOOL Task_IsSchedulerEnabled(void)
{
	return bGlobalTaskEnable;
}
//---------------------------------------------------------------------------
/*!
	Set the state of the scheduler.

	\fn Task_SetScheduler(BOOL bEnable_)
	\param bEnable_ - TRUE to enable the scheduler, FALSE to disable
*/
void Task_SetScheduler(BOOL bEnable_)
{
	bGlobalTaskEnable = bEnable_;
}
//---------------------------------------------------------------------------
/*!
	Disables the scheduler, and returns the scheduler's previous state.  This
	is used in combination with Task_SchedulerRestore() to provide the 
	scheduler-disabled context.

	\fn Task_SchedulerDisable(void)
	\return BOOL - the previous state of the scheduler (TRUE = ENABLED)
*/
BOOL Task_SchedulerDisable(void)
{
	BOOL bReturn;
	
	CS_ENTER();
	bReturn = Task_IsSchedulerEnabled();
	Task_SetScheduler(FALSE);
	CS_EXIT();
	
	return bReturn;
}
//---------------------------------------------------------------------------
/*!
	Used to restore the state of the scheduler after performing an operation
	that operates in a scheduler-disabled context

	\fn Task_SchedulerRestore(BOOL bState_)
	\param bState_ - TRUE to enable the scheduler, FALSE to disable the scheduler
*/
void Task_SchedulerRestore(BOOL bState_)
{
	CS_ENTER();
	Task_SetScheduler(bState_);
	CS_EXIT();
}
//---------------------------------------------------------------------------
/*!
	Return the pointer to the task struct of the currently running task.

	\fn Task_GetCurrentTask(void)
	\return TASK_STRUCT * - Pointer to the currently running task
*/
TASK_STRUCT* Task_GetCurrentTask(void)
{
	return pstCurrentTask;
}
