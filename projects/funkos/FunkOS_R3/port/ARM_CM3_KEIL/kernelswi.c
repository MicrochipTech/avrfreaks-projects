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

static void PendSV_ISR(void);

//---------------------------------------------------------------------------
void KernelSWI_Config(void)
{
	// Install the vector handler
	TASK_VECTOR_TABLE *pstTable;
	ULONG ulTableAddr;

	// Get the address stored in the vector table address register
	ulTableAddr = TASK_NVIC_VECTOR_REG;

	// This address is the location of the vector table
	pstTable = (TASK_VECTOR_TABLE*)ulTableAddr;
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
__asm void PendSV_ISR(void)
{
	//-----------------------------------------------------------------------
	// Context switch from Pend ISR 
	//-----------------------------------------------------------------------
	extern void Task_SwitchContext(void);
	extern TASK_STRUCT *pstCurrentTask;
	
	PRESERVE8
	
	//-----------------------------------------------------------------------
	// based on example 5-5 pp120, ARM document DDI0337E
	//-----------------------------------------------------------------------
	
	MOV r1, #TASK_MAX_SYSCALL_PRI	// Disable other interrupts
	MSR basepri, r1			
	
	LDR r12, =pstCurrentTask		// load task pointer
	LDR r3, [r12]					// load task stack pointer
	
	MRS r2, psp						// Get the current process stack pointer
	STMDB r2!, {r4-r11, lr}			// Save the non-preserved registers 
	STR r2, [r3]					// Store SP into TCB
	
	BL Task_SwitchContext			// Select a new task (clobbers registers)
	
	LDR r0, =pstCurrentTask			// reload task pointer
	LDR r0, [r0]					// reload task stack pointer
	LDR r0, [r0]					// task stack
	
	LDMIA r0!, {r4-r11, lr}			// restore the non-preserved registers
	
	MSR psp, r0						// restore the stack (preserved-registers pop'd on return)
	
	MOV r1, #0						// Enable interrupts on return
	MSR basepri, r1

	BX lr							// return from exception back to thread mode
}
