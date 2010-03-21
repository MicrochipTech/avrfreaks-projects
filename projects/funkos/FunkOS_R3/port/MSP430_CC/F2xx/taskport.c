//---------------------------------------------------------------------------
// RAVR RTK - Copyright (c) 2009, Funkenstein Software, See license.txt for details
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
#include "kernelcfg.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "timer.h"

#include <msp430.h>

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
	USHORT *pusStack;
	USHORT i;

	// Get the stack pointer.
	pusStack = (USHORT*)pstTask_->pwTopStack;
	
	// Get the "return" address for the function
	usAddr = (USHORT)(pstTask_->pfTaskFunc);
	
	// clear the stack...
	for (i = 0; i < pstTask_->usStackSize; i++)
	{
		pstTask_->pwStack[i] = 0xFF;
	}
		
	// 1st - push start address...
	PUSH_TO_STACK(pusStack, usAddr);
	
	// 2nd - Interrupts enabled status register
	PUSH_TO_STACK(pusStack, 0x08);	
	
	// Push other registers
	for (i = 4; i < 12; i++)
	{
		PUSH_TO_STACK(pusStack, i);
	}

    // Function parameter
    PUSH_TO_STACK(pusStack, (USHORT)pstTask_);  

    // Push other registers
	for (i = 13; i < 16; i++)
	{
		PUSH_TO_STACK(pusStack, i);
	}
	pstTask_->pwTopStack = (WORD*)(pusStack+1);
}

//---------------------------------------------------------------------------
// This isn't really the ISR, the ISR calls this function
#pragma vector=TIMERA0_VECTOR
interrupt void Task_TimerISR(void)
{
#if KERNEL_USE_TIMERS
	Timer_Tick();
#endif

	if (Task_Tick())
	{
		Task_YieldSWI();
	}	
}
