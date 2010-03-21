//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  task.h
	
	Description:
		Portable task structure and function interface
*/
//--------------------------------------------------------------------------- 

#ifndef __TASK_H_
#define __TASK_H_

#include "types.h"		
#include "kernelcfg.h"

//---------------------------------------------------------------------------
//! Definition of the infinite timeout interval of the system
#define TIME_FOREVER		((USHORT)65535)		
	
//---------------------------------------------------------------------------
//! Enumerated type that defines the different running states of the system
typedef enum
{
	TASK_UNINIT = 0,
	TASK_READY,
	TASK_BLOCKED,
	TASK_SLEEP,
	TASK_STOPPED,
	TASK_TIMEOUT,
	TASK_STATES
} TASK_STATE;

//---------------------------------------------------------------------------
//! Task priority enumeration - basic definitions for simple systems
typedef enum
{
	TASK_PRI_IDLE = 0,
	TASK_PRI_LOW,
	TASK_PRI_MED,
	TASK_PRI_HIGH,
	TASK_PRI_MAX = 255
}TASK_PRIORITY;

//---------------------------------------------------------------------------
//! Task function type definition
typedef void (*TASK_FUNC)(void *pvThis_);

//---------------------------------------------------------------------------
typedef struct Task_Struct
{
/*! This is the basic task control block in the RTOS.  It contains parameters
	and state information required for a task, including stack, priority, 
	timeouts, entry funcitons, and task pending semaphore.
*/	
	//--[Task Control Block Entries]-----------------------------------------
	WORD *pwTopStack;		//!< Pointer to current stack top 
	WORD *pwStack;			//!< Stack pointer, defined by the task.	
	USHORT usStackSize;		//!< Size of the stack in MAU

#if KERNEL_STACK_DEBUG	
	USHORT usStackMargin;	//!< Runtime stack margin
#endif

	//--[Task Definitions]---------------------------------------------------
	BYTE *pacName;			//!< Pointer to the name of the task (ASCII)
	
	TASK_FUNC pfTaskFunc;	//!< Pointer to the entry function
	
	UCHAR ucPriority;		//!< Task priority
	TASK_STATE eState;		//!< Current task state

	USHORT usTimeLeft;		//!< Ticks remaining in blocked/sleep state	

#if KERNEL_USE_QUANTUM
	USHORT usQuantum;		//!< Time to execute task before switching
#endif

	BOOL bTimeout;			//!< Indicates that an IO operation timed out
	//--[List Pointers]------------------------------------------------------
#if KERNEL_USE_MUTEX
	UCHAR ucMutexPriority;		//!< Priority inheritence used by mutex
#endif

	struct Task_Struct *pstNext; 		//!< Pointer to the next task (handled by scheduler)

#if KERNEL_USE_MUTEX || KERNEL_USE_SEMAPHORE
	struct Task_Struct *pstNextList;	//!< List pointer provided for Mutex and Semaphore
#endif
} TASK_STRUCT;

//---------------------------------------------------------------------------
//--!!One Time Init!!--------------------------------------------------------
void Task_Init(void);

//--[User functions]---------------------------------------------------------
//---------------------------------------------------------------------------
void Task_CreateTask(	TASK_STRUCT *pstTask_, 
						BYTE *pcName_,
						WORD *pwStack_,
						USHORT usStackSize_,
						UCHAR ucPriority_,
						TASK_FUNC pfFunction_);

void Task_SetQuantum(TASK_STRUCT *pstTask_, USHORT usTicks_);

BOOL Task_IsSchedulerEnabled(void);
void Task_SetScheduler(BOOL bEnable_);

BOOL Task_Add(TASK_STRUCT *pstTask_);
BOOL Task_Remove(TASK_STRUCT *pstTask_);
void Task_Start(TASK_STRUCT *pstTask_);
void Task_Stop(TASK_STRUCT *pstTask_);			
void Task_Sleep(USHORT usTime_);
void Task_Switch(void);
BOOL Task_Tick(void);
BOOL Task_SchedulerDisable(void);
void Task_SchedulerRestore(BOOL bState_);
void Task_YieldSWI(void);
TASK_STRUCT* Task_GetCurrentTask(void);
//---------------------------------------------------------------------------

#endif //__TASK_H_

