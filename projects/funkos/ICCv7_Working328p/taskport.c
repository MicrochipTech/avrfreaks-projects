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

#include <iom328pv.h>

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
	UCHAR *pucHWStack;
	USHORT i;

	// Get the stack pointer.
	pucStack = (UCHAR*)pstTask_->pwTopStack;
	pucHWStack = (UCHAR*)pstTask_->pwTopStack;
	
	// Get the "return" address for the function
	usAddr = (USHORT)(pstTask_->pfTaskFunc);
	
	// clear the stack...
	for (i = 0; i < pstTask_->usStackSize; i++)
	{
		pstTask_->pwStack[i] = 0xFF;
	}
		
	// 1st - push start address to the hardware stack
	PUSH_TO_STACK(pucStack, (UCHAR)(usAddr & 0x00FF));
	PUSH_TO_STACK(pucStack, (UCHAR)((usAddr >> 8) & 0x00FF));
	
	// Adjust for the size of the hardware stack before continuing.
	pucStack -= (KERNEL_CALLSTACK_SIZE - 2);	// Size of an address pointer
	
	// 2nd - push R0
	PUSH_TO_STACK(pucStack, 0x00);	// R0
	
	// Push status register
	PUSH_TO_STACK(pucStack, 0x80);  // SR - with interrupts enabled
	
	// Hardware stack address
	pucHWStack -= 2;
	usAddr = (USHORT)(pucHWStack);
	
	PUSH_TO_STACK(pucStack, (UCHAR)(usAddr & 0x00FF));
	PUSH_TO_STACK(pucStack, (UCHAR)((usAddr >> 8) & 0x00FF));
	
	// Push other registers
	for (i = 1; i <= 15; i++) //R2-R15
	{
		PUSH_TO_STACK(pucStack, i);
	}

	// Assume that the task handle is the first and only function parameter to a task
	PUSH_TO_STACK(pucStack, (UCHAR)((USHORT)pstTask_) & 0x00FF);	//R16
	PUSH_TO_STACK(pucStack, (UCHAR)(((USHORT)pstTask_)>>8) & 0x00FF); //RR17
		
	for (i = 18; i <= 27; i++) //R18-R27
	{
		PUSH_TO_STACK(pucStack, i);
	}

	PUSH_TO_STACK(pucStack, 30); // R30, R31
	PUSH_TO_STACK(pucStack, 31);
	
	// Set the top o' the stack
	pstTask_->pwTopStack = pucStack + 1;
}

//---------------------------------------------------------------------------
/*!
	Call this function at the end of main() to start the scheduler.
	\fn void Task_StartTasks(void)
*/
void Task_StartTasks(void)
{	
	sCriticalCount = 0;
	Task_SetScheduler(TRUE);
	Task_Switch();
	KernelSWI_Start();
	KernelTimer_Start();
	// Restore the context...
	Task_RestoreContext();
	// Reti here because of dual stack model... Will also enable ints.
	ASM("reti");
}

//---------------------------------------------------------------------------
/*!
	SWI using INT0 - used to trigger a context switch
	\fn ISR(INT0_vect) __attribute__ ( ( signal, naked ) );
*/
//---------------------------------------------------------------------------
#pragma interrupt_handler RTOS_TASK_HANDLER:iv_INT0
void RTOS_TASK_HANDLER(void)
{
	// Clear the INT0 flag by writing it.		
	Task_Yield();
}

//---------------------------------------------------------------------------
/*!
	Timer interrupt ISR - causes a tick, which may cause a context switch
	\fn ISR(TIMER1_COMPA_vect) ;
*/
//---------------------------------------------------------------------------
#pragma interrupt_handler RTOS_TICK_HANDLER:iv_TIMER1_COMPA
void RTOS_TICK_HANDLER(void)
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

