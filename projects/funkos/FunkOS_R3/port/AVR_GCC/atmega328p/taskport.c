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

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
TASK_STRUCT *pstCurrentTask;
SHORT sCriticalCount;

//---------------------------------------------------------------------------
/*!
	This function initializes the stack of a task
	\fn void Task_InitStack(TASK_STRUCT *pstTask_)
	\param *pstTask_ - the task whose stack we wish to initialize.
*/
void Task_InitStack(TASK_STRUCT *pstTask_)
{
	// Initialize the stack for the task.
	USHORT usAddr;
	UCHAR *pucStack;
	USHORT i;

	// Get the stack pointer.
	pucStack = (UCHAR*)pstTask_->pwTopStack;
	
	// Get the "return" address for the function
	usAddr = (USHORT)(pstTask_->pfTaskFunc);
	
	// clear the stack...
	for (i = 0; i < pstTask_->usStackSize; i++)
	{
		pstTask_->pwStack[i] = 0xFF;
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
	PUSH_TO_STACK(pucStack, (UCHAR)((USHORT)pstTask_) & 0x00FF);	//R24
	PUSH_TO_STACK(pucStack, (UCHAR)(((USHORT)pstTask_)>>8) & 0x00FF); //R25

	// Push the rest of the fake stack variables.
	for (i = 26; i <=31; i++)
	{
		PUSH_TO_STACK(pucStack, i);
	}
	// Set the top o' the stack
	pstTask_->pwTopStack = pucStack;
}

//---------------------------------------------------------------------------
/*!
	Call this function at the end of main() to start the scheduler.
	\fn void Task_StartTasks(void)
*/
void Task_StartTasks(void)
{	
	sCriticalCount = 0;
	KernelSWI_Config();
	KernelTimer_Config();
	KernelTimer_Start();
	KernelSWI_Start();
	Task_SetScheduler(TRUE);
	Task_Switch();
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
	UCHAR ucSR;

	//--[Nested interrupts...]-----------------------------------------------
	// Disable this interrupt first
	KernelTimer_DI();
	
	// Disable the scheduler while in this context...
	bSchedulerState = Task_IsSchedulerEnabled();
	Task_SetScheduler(FALSE);

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
			Timer_Tick();
#endif
			// Run the tick if the scheduler's enabled...
			bRunScheduler = TRUE;
			if (bSchedulerState)
			{
				Task_Tick();
			}

#if KERNEL_NESTED_TICK
		}
		
		// Disable nesting - restore interrupts.
		_SFR_IO8(SR_) = ucSR;
	}
#endif

	// Restore the interrupts
	KernelTimer_EI();
	
	// Restore the scheduler state
	Task_SetScheduler(bSchedulerState);
	
	// Trigger a context switch if necessary
	if (bRunScheduler && bSchedulerState)
	{
		Task_YieldSWI();
	}
}

