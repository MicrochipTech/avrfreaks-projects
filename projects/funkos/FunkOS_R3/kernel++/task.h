//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  task.h

	Description:
        Definition of the task class, used by all of the kernel API's.
*/
//---------------------------------------------------------------------------

#ifndef __TASK_HPP_
#define __TASK_HPP_

//---------------------------------------------------------------------------
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
//! Task exection function type definition
typedef void (*TASK_FUNC)(void *pvThis_);

//---------------------------------------------------------------------------
class FunkOS_Task
{
public:
/*! This is the basic task control block in the RTOS.  It contains parameters
	and state information required for a task, including stack, priority, 
	timeouts, entry funcitons, and task pending semaphore.
*/	
	//--[Task Control Block Entries]-----------------------------------------
	WORD *m_pwTopStack;		//!< Pointer to current stack top 
	WORD *m_pwStack;			//!< Stack pointer, defined by the task.	
	USHORT m_usStackSize;		//!< Size of the stack in MAU

#if KERNEL_STACK_DEBUG	
	USHORT m_usStackMargin;	//!< Runtime stack margin
#endif

	//--[Task Definitions]---------------------------------------------------
	BYTE *m_pacName;			//!< Pointer to the name of the task (ASCII)
		
	USHORT m_usTimeLeft;		//!< Ticks remaining in blocked/sleep state	

#if KERNEL_USE_QUANTUM
	USHORT m_usQuantum;		//!< Time to execute task before switching
#endif

	BOOL m_bTimeout;			//!< Indicates that an IO operation timed out

	TASK_STATE m_eState;		//!< Current task state

	UCHAR m_ucPriority;		//!< Task priority

	//--[List Pointers]------------------------------------------------------
#if KERNEL_USE_MUTEX
	UCHAR m_ucMutexPriority;		//!< Priority inheritence used by mutex
#endif
	
	TASK_FUNC m_pfFunc;				//!< Pointer to the handler function

	FunkOS_Task *m_pclNext; 		//!< Pointer to the next task (handled by scheduler)

#if KERNEL_USE_MUTEX || KERNEL_USE_SEMAPHORE
	FunkOS_Task *m_pclNextList;	//!< List pointer provided for Mutex and Semaphore
#endif

public:
	FunkOS_Task();
	void CreateTask(BYTE *pcName_,
					UCHAR ucPriority_);

	void SetQuantum(USHORT usTicks_);
	void Start(void);
	void Stop(void);			
	void Sleep(USHORT usTime_);
	static void Yield(void);

	void SetState(TASK_STATE eState_);
	void SetNextTask(FunkOS_Task *pclTask_);
	FunkOS_Task* GetNextTask(void);
	
		
private:	
	//-- Implemented in the port.
	void InitStack(void);	
};

#endif
