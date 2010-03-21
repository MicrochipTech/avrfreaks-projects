//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  taskport.c
	
	Description:
		AVRMega implementation of context switching
*/
//--------------------------------------------------------------------------- 
// Port contributed by Ahmad Shawqy(ahmad.shawqyatgmaildotcom)
//--------------------------------------------------------------------------- 

#include "types.h"
#include "taskport.h"
#include "task.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "timer.h"

#define HARDWARE_STACK_DEPTH	(10)

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
    USHORT usStackTop;

	// Get the stack pointer.
	pucStack = (UCHAR*)pstTask_->pwTopStack;
	
	// Get the "return" address for the function
	usAddr = (USHORT)(pstTask_->pfTaskFunc);
	
	// clear the stack...
	for (i = 0; i < pstTask_->usStackSize; i++)
	{
		pstTask_->pwStack[i] = 0xFF;
	}
    
	// Push function address...
	PUSH_TO_STACK(pucStack, (UCHAR)(usAddr & 0x00FF));
	PUSH_TO_STACK(pucStack, (UCHAR)((usAddr >> 8) & 0x00FF));
	
	usStackTop = (USHORT)pucStack;
	
    // for second stack.  -2 because the address of the task's entry
	// function is already on the call stack...
	pucStack -= (HARDWARE_STACK_DEPTH- 2); 
	
	// Push task argument (pointer to itself...)
	usAddr = (USHORT)pstTask_;
	PUSH_TO_STACK(pucStack, (UCHAR)(usAddr & 0x00FF));
	PUSH_TO_STACK(pucStack, (UCHAR)((usAddr >> 8) & 0x00FF));

	// Registers:  1st - push R0
    PUSH_TO_STACK(pucStack, 0x00);	// R0
	
	// Push status register
	PUSH_TO_STACK(pucStack, 0x80);  // SR
	
	// Push call stack address to the stack
	PUSH_TO_STACK(pucStack, (UCHAR)(usStackTop & 0x00FF));
	PUSH_TO_STACK(pucStack, (UCHAR)((usStackTop >> 8) & 0x00FF));	

	// Push other registers
	for (i = 1; i < 29; i++)
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
	Task_SetScheduler(TRUE);
	Task_Switch();
	KernelSWI_Start();
	KernelTimer_Start();
	// Restore the context...
	Task_RestoreContext();
	// Reti here because of dual stack model... Will also enable ints.
	#asm("reti");
}

//---------------------------------------------------------------------------
/*!
	Timer interrupt ISR - causes a tick, which may cause a context switch
	\fn ISR(TIMER1_COMPA_vect) ;
*/
//---------------------------------------------------------------------------

interrupt [7] void TIMER1_COMPA_ISR( void )
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
		ucSR = SREG;
		
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
		SREG = ucSR;
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

#pragma savereg-

interrupt [2] void EXT_INT0_ISR( void )
{
	Task_SaveContext(); 
	Task_Switch();
	Task_RestoreContext();
}

#pragma savereg+
