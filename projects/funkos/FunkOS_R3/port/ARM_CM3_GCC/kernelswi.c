//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// Implement the "SWI" for the kernel task switch using the PendSV interrupt
// on the Cortex M3

#include "types.h"
#include "kernelcfg.h"
#include "task.h"
#include "taskport.h"

//---------------------------------------------------------------------------
#define TASK_PENDSV_SET					((ULONG)(0x10000000))

//---------------------------------------------------------------------------
extern TASK_STRUCT *pstCurrentTask;

//---------------------------------------------------------------------------
void PendSV_ISR(void) __attribute__ ((naked, interrupt));

//---------------------------------------------------------------------------
void KernelSWI_Config(void)
{
	// Install the software interrupt handler
	TASK_VECTOR_TABLE *pstTable;
	ULONG ulTableAddr;

	// Get the address stored in the vector table address register
	ulTableAddr = TASK_NVIC_VECTOR_REG;

	// This address is the location of the vector table
	pstTable = (TASK_VECTOR_TABLE*)ulTableAddr;
	pstTable->ulPendSV_ISR = (ULONG)PendSV_ISR;
	
	// Set the interrupt priority for the "Pend" SWI
	TASK_PENDSV_SETPRI();
}

//---------------------------------------------------------------------------
void KernelSWI_Start(void)
{
	// Stub
}

//---------------------------------------------------------------------------
void KernelSWI_Stop(void)
{
	// Stub
}

//---------------------------------------------------------------------------
void KernelSWI_Clear(void)
{
	// Cleared automatically...
}

//---------------------------------------------------------------------------
void KernelSWI_Trigger(void)
{
	if (Task_IsSchedulerEnabled())
	{
		// Cause a context switch interrupt by writing to the interrupt control
		// state register
		TASK_NVIC_ICS_REG |= TASK_PENDSV_SET;
	}
}

//---------------------------------------------------------------------------
void PendSV_ISR(void)
{

	//-----------------------------------------------------------------------
	// Context switch from Pend ISR 
	//-----------------------------------------------------------------------
	Task_SaveContext();
	Task_Switch();
	Task_RestoreContext();
}
