//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  task.cpp

	Description:
        Implementation of the task class, used by all of the kernel API's.
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
FunkOS_Task::FunkOS_Task()
{
	// Default constructor - stub
}

//---------------------------------------------------------------------------
/*!
	This function populates a given task structure with the parameters given.
	The stack is then initialized, and the task is set as stopped, and all
	state variables and timeouts reset.  Once the task is created, it must
	be added using the scheduler's Add() function, and started using Start()
	\fn void FunkOS_Task::CreateTask(BYTE *pcName_,			
						UCHAR ucPriority_)
	\brief Add a task from the scheduler list
	\param pcName_ - pointer to the task name
	\param ucPriority_ - priority of the task (0-255)
	\return (BOOL) TRUE on success, FAIL on failure
*/
void FunkOS_Task::CreateTask(BYTE *pcName_,			
						UCHAR ucPriority_)
{
	// Assign the task name
	m_pacName = pcName_;
	
	// Assign the task priority
	m_ucPriority = ucPriority_;
	
	// Set the task to the "stopped" state
	m_eState = TASK_STOPPED;	
	
	// Set the stack pointer to the top of the task -> grow down/up depending on architecture.
	m_pwTopStack = (WORD*)TOP_OF_STACK(m_pwStack , m_usStackSize);
	
	// Wait time to 0 on init.
	m_usTimeLeft = 0;	

	// Set the quantum to 1 tick
#if KERNEL_USE_QUANTUM
	m_usQuantum = 1;
#endif // KERNEL_USE_QUANTUM
	
#if KERNEL_USE_SEMAPHORE
	m_pclNextList = (FunkOS_Task*)NULL;
#endif // KERNEL_USE_SEMAPHORE

#if KERNEL_USE_MUTEX
	m_pclNextList = (FunkOS_Task*)NULL;
	m_ucMutexPriority = 0;
#endif // KERNEL_USE_MUTEX

	// Initialize the stack...
	InitStack();
}

#if KERNEL_USE_QUANTUM
//---------------------------------------------------------------------------
/*!
	Set the time between context switches when multiple tasks are executing
	at the same priority level
	\fn void FunkOS_Task::SetQuantum(USHORT usTicks_)
	\param usTicks_ - the number of ticks inbetween switching
*/
void FunkOS_Task::SetQuantum(USHORT usTicks_)
{	
	m_usQuantum = usTicks_;
}

#endif // KERNEL_USE_QUANTUM

//---------------------------------------------------------------------------
/*!
	Set a task as ready to run
	\fn void FunkOS_Task::Start(void)
*/
void FunkOS_Task::Start(void)
{
	// Place task in the ready state
	m_eState = TASK_READY;	

#if KERNEL_USE_QUANTUM
	m_usTimeLeft = m_usQuantum;
#endif // KERNEL_USE_QUANTUM
}

//---------------------------------------------------------------------------
/*!
	Stop a task from running, but does not remove it from the scheduler if
    it is already connected.
	\fn void FunkOS_Task::Stop(void)
*/
void FunkOS_Task::Stop(void)
{
	// Place task in the stopped state
	m_eState = TASK_STOPPED;
}

//---------------------------------------------------------------------------
/*!
	Set a task to sleep for a period of time specified in the arguments
	\fn void FunkOS_Task::Sleep(USHORT usTime_)
	\param usTime_ - the time period in RTOS ticks to sleep through
*/
void FunkOS_Task::Sleep(USHORT usTime_)
{
	// Do this in a scheduler-disabled context
	CS_ENTER();
		
	m_eState = TASK_SLEEP;
	m_usTimeLeft = usTime_;
	
	CS_EXIT();
		
	// Call the scheduler immediately.
	Yield();
}

//---------------------------------------------------------------------------
/*!
	Trigger a software interrupt that will invoke the context switch.  This
	is the best mechanism for triggering a context switch from an application
	or system-service level, as it is completely thread-safe and can be
	invoked from an interrupt.  
	
	\fn void FunkOS_Task::Yield(void)
*/
void FunkOS_Task::Yield(void)
{
	KernelSWI::Trigger();
}
