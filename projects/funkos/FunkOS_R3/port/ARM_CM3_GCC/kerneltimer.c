//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
#include "types.h"
#include "kernelcfg.h"
#include "taskport.h"
#include "timer.h"
#include "task.h"

//---------------------------------------------------------------------------
#define TASK_NVIC_SYSTICK_CSR_CORE_CLK	(0x00000004)
#define TASK_NVIC_SYSTICK_CSR_INT		(0x00000002)
#define TASK_NVIC_SYSTICK_CSR_ENABLE	(0x00000001)

//---------------------------------------------------------------------------
void SysTick_ISR(void);

//---------------------------------------------------------------------------
void KernelTimer_Config(void)
{		
	// Install the timer interrupt handler
	TASK_VECTOR_TABLE *pstTable;
	ULONG ulTableAddr;

	// Get the address stored in the vector table address register
	ulTableAddr = TASK_NVIC_VECTOR_REG;

	// This address is the location of the vector table
	pstTable = (TASK_VECTOR_TABLE*)ulTableAddr;
	pstTable->ulSysTick_ISR = (ULONG)SysTick_ISR;
	
	// Interrupt on count = 0
	TASK_NVIC_SYSTICK_CSR_REG |= TASK_NVIC_SYSTICK_CSR_INT;	
	
	// Use the internal core clock as reference
	TASK_NVIC_SYSTICK_CSR_REG |= TASK_NVIC_SYSTICK_CSR_CORE_CLK;	
	
	// Set the reload register to a value based on system frequency and timer frequency
	TASK_NVIC_SYSTICK_RELOAD_REG = ((ULONG)SYSTEM_FREQ / (ULONG)TICK_FREQ) - 1;
	
	// Set the priority of the tick interrupt
	TASK_SYSTICK_SETPRI();
}

//---------------------------------------------------------------------------
void KernelTimer_Start(void)
{
	TASK_NVIC_SYSTICK_CURRENT_REG = 0;	// clear the timer
	TASK_NVIC_SYSTICK_CSR_REG |= TASK_NVIC_SYSTICK_CSR_ENABLE; // enable timer
}

//---------------------------------------------------------------------------
 void KernelTimer_Stop(void)
{
	TASK_NVIC_SYSTICK_CSR_REG &= ~TASK_NVIC_SYSTICK_CSR_ENABLE; // disable timer 
}

//---------------------------------------------------------------------------
void KernelTimer_DI(void)
{
	// Stub.
}

//---------------------------------------------------------------------------
void KernelTimer_EI(void)
{
	// Stub.
}

//!! Timer tick

//---------------------------------------------------------------------------
void SysTick_ISR(void) __attribute__ ((interrupt));
void SysTick_ISR(void)
{
	BOOL bSchedulerState;

	// Disable the scheduler while in this context...
	bSchedulerState = Task_IsSchedulerEnabled();
	Task_SetScheduler(FALSE);

#if KERNEL_USE_TIMERS

	// Run the timers regardless...
	Timer_Tick();
#endif
	// Run the tick if the scheduler's enabled...
	if (bSchedulerState)
	{
		Task_Tick();
	}
	
	// Restore the scheduler state
	Task_SetScheduler(bSchedulerState);
	
	// Trigger a context switch if necessary
	if (bSchedulerState)
	{
		Task_YieldSWI();
	}
}
