//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  pipsqueak.c

	Description:
	The FunkOS Pipsqueak RTOS kernel
*/
//---------------------------------------------------------------------------
#include "types.h"
#include "pipsqueak.h"
#include "taskport.h"
#include "kernelswi.h"

//---------------------------------------------------------------------------
//! Global variable used by the kernel
TASK_STRUCT *pstCurrent;

//---------------------------------------------------------------------------
//! Module-local variables used by the task manager 
static TASK_STRUCT *pstIdle;
static volatile USHORT usTimeLeft;
static volatile BOOL bIdle;

//---------------------------------------------------------------------------
/*!
	Initializes the pipsqueak multithreading kernel.  This must be called 
	before calling any other function in this module.
	\fn void Task_Init(void)	
*/
//---------------------------------------------------------------------------
void Task_Init(void)
{
	pstCurrent = NULL;
	pstIdle = NULL;
	usTimeLeft = 0;
	bIdle = FALSE;
}
//---------------------------------------------------------------------------
/*!
	Set the scheduler's idle task to the specified task.  This must be called
	if the idle task can ever possibly be invoked.
	\fn void Task_SetIdleTask(TASK_STRUCT *pstTask_)
	\param pstTask_ - pointer to the task to set as the system's idle task 
*/
//---------------------------------------------------------------------------
void Task_SetIdleTask(TASK_STRUCT *pstTask_)
{
	pstIdle = pstTask_;
}
//---------------------------------------------------------------------------
/*!
	Add a task to the scheduler.
	\fn void Task_AddTask(TASK_STRUCT *pstTask_)
	\param pstTask_ - pointer to the task to add to the scheduler
*/
//---------------------------------------------------------------------------
void Task_AddTask(TASK_STRUCT *pstTask_)
{
	if (pstCurrent == NULL)
	{
		// No tasks currently assigned, initialize the list
		pstCurrent = pstTask_;
		pstCurrent->pstNext = pstTask_;
	}
	else
	{
		// Insert a task into the list after the currently assigned task
		TASK_STRUCT *pstTemp;
		pstTemp = pstCurrent->pstNext;
		pstCurrent->pstNext = pstTask_;
		pstTask_->pstNext = pstTemp;
	}
}
//---------------------------------------------------------------------------
/*!
	Switches to the idle thread until the current task's quantum has expired.
	Once the quantum expires, the next active task in the list is executed.
	\fn void Task_GoIdle(void)
*/
//---------------------------------------------------------------------------
void Task_GoIdle(void)
{
	if (pstIdle != NULL)
	{
		CS_ENTER();
		pstIdle->usTicks = pstCurrent->usTicks;
		pstIdle->pstNext = pstCurrent->pstNext;	
		bIdle = TRUE;
		CS_EXIT();
		KernelSWI_Trigger();
	}
}

//---------------------------------------------------------------------------
/*!
	Increments the task quantum counter, and switches to the next task when the
	quantum has expired.
	\fn void Task_Tick(void)
*/
//---------------------------------------------------------------------------
void Task_Tick(void)
{	
	if (++usTimeLeft >= pstCurrent->usTicks)
	{
		usTimeLeft = 0;
		KernelSWI_Trigger();		
	}
}

//---------------------------------------------------------------------------
/*!
	Switches to the next task in the circular list of tasks.  If the idle task
	has been selected, switch to the idle task instead.
	\fn void Task_Switch(void)
*/
//---------------------------------------------------------------------------
void Task_Switch(void)
{
	if (bIdle == TRUE)
	{
		pstCurrent = pstIdle;
		bIdle = FALSE;
	}
	else
	{
		pstCurrent = pstCurrent->pstNext;	
	}
}
