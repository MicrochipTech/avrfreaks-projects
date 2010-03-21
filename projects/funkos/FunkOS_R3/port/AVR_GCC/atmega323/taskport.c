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
#include "taskcfg.h"
#include "kernelswi.h"
#include "kerneltimer.h"

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
TASK_STRUCT *pstCurrentTask;
SHORT sCriticalCount;
//---------------------------------------------------------------------------
/*!
	This function initializes the stack of a task
	\fn void Task_InitStack(TASK_STRUCT *pstTask_)
	\param TASK_STRUCT *pstTask_ - the task whose stack we wish to initialize.
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
	for (i = 2; i < 32; i++)
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
	This is the function that is called from the SWI or other system-level
	functions that yields from a task, and switches context.
	\fn void Task_Yield(void) __attribute__ ( (naked) )
*/

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
	
	{
		// Get the status register
		ucSR = _SFR_IO8(SR_);
		
		{
			// Enable nesting while ticking...
			ENABLE_INTS();
		
			// Run the tick...
			bRunScheduler = Task_Tick();
		}
		
		// Disable nesting - restore interrupts.
		_SFR_IO8(SR_) = ucSR;
	}
	
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

