//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  taskport.c
	
	Description:
		AVRMega implementation of context switching
*/
//--------------------------------------------------------------------------- 

#include "types.h"
#include "taskport.h"
#include "task.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "kernelcfg.h"
#include "timer.h"
#include "scheduler.h"

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
/*!
	This function initializes the stack of a task
	\fn void Task_InitStack(TASK_STRUCT *pstTask_)
	\param *pstTask_ - the task whose stack we wish to initialize.
*/
void FunkOS_Task::InitStack(void)
{
	// Initialize the stack for the task.
	USHORT usAddr;
	UCHAR *pucStack;
	USHORT i;

	// Get the stack pointer.
	pucStack = (UCHAR*)m_pwTopStack;
	
	// Get the "return" address for the function
	usAddr = (USHORT)((void*)m_pfFunc);
	
	// clear the stack...
	for (i = 0; i < m_usStackSize; i++)
	{
		m_pwStack[i] = 0xFF;
	}
		
	// 1st - push start address...
	PUSH_TO_STACK(pucStack, (UCHAR)(usAddr & 0x00FF));
	PUSH_TO_STACK(pucStack, (UCHAR)((usAddr >> 8) & 0x00FF));
	
	// 2nd - push R0
	PUSH_TO_STACK(pucStack, 0x00);	// R0
	
	// Push status register
	PUSH_TO_STACK(pucStack, 0x80);  // SR
	PUSH_TO_STACK(pucStack, 0x00);  // R1

	// Push other registers
	for (i = 2; i <= 23; i++) //R2-R23
	{
		PUSH_TO_STACK(pucStack, i);
	}

	// Assume that the task handle is the first and only function parameter to a task
	PUSH_TO_STACK(pucStack, (UCHAR)((USHORT)this) & 0x00FF);	//R24
	PUSH_TO_STACK(pucStack, (UCHAR)(((USHORT)this)>>8) & 0x00FF); //R25

	// Push the rest of the fake stack variables.
	for (i = 26; i <=31; i++)
	{
		PUSH_TO_STACK(pucStack, i);
	}
	// Set the top o' the stack
	m_pwTopStack = pucStack;
}

//---------------------------------------------------------------------------
/*!
	Call this function at the end of main() to start the scheduler.
	\fn void Task_StartTasks(void)
*/
void FunkOS_Scheduler::StartTasks(void)
{		
	KernelSWI::Config();
	KernelTimer::Config();
	KernelTimer::Start();
	KernelSWI::Start();
	FunkOS_Scheduler::Set(TRUE);
	FunkOS_Scheduler::Switch();
	// Restore the context...
	Task_RestoreContext();
	ASM("reti");
}

//---------------------------------------------------------------------------
/*!
	SWI using INT0 - used to trigger a context switch
	\fn ISR(INT0_vect) __attribute__ ( ( signal, naked ) );
*/
//---------------------------------------------------------------------------
ISR(INT0_vect) __attribute__ ( ( signal, naked ) );
ISR(INT0_vect)
{
	// Clear the INT0 flag by writing it.		
 	Task_Yield();		
	ASM("reti");
}

//---------------------------------------------------------------------------
/*!
	Timer interrupt ISR - causes a tick, which may cause a context switch
	\fn ISR(TIMER1_COMPA_vect) ;
*/
//---------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect)
{
	BOOL bRunScheduler;
	BOOL bSchedulerState;
#if KERNEL_NESTED_TICK	
	UCHAR ucSR;
#endif

	//--[Nested interrupts...]-----------------------------------------------
	// Disable this interrupt first
	KernelTimer::DI();
	
	// Disable the scheduler while in this context...
	bSchedulerState = FunkOS_Scheduler::IsEnabled();
	FunkOS_Scheduler::Set(FALSE);

#if KERNEL_NESTED_TICK
	{
		// Get the status register
		ucSR = _SFR_IO8(SR_);
		
		{
			// Enable nesting while ticking...
			ENABLE_INTS();
#endif


#if KERNEL_USE_TIMERS

			// Run the timers regardless...
			FunkOS_TimerList::Tick();
#endif
			// Run the tick if the scheduler's enabled...
			bRunScheduler = TRUE;
			if (bSchedulerState)
			{
				FunkOS_Scheduler::Tick();
			}

#if KERNEL_NESTED_TICK
		}
		
		// Disable nesting - restore interrupts.
		_SFR_IO8(SR_) = ucSR;
	}
#endif

	// Restore the interrupts
	KernelTimer::EI();
	
	// Restore the scheduler state
	FunkOS_Scheduler::Set(bSchedulerState);
	
	// Trigger a context switch if necessary
	if (bRunScheduler && bSchedulerState)
	{
		FunkOS_Task::Yield();
	}
}

