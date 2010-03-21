//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  taskport.c
	
	Description:
		ARM Cortex M-3 port of the kernel
*/
//--------------------------------------------------------------------------- 

#include "types.h"
#include "taskport.h"
#include "task.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "kernelcfg.h"
#include "timer.h"

//---------------------------------------------------------------------------
void Task_SVC_ISR(void) __attribute__ ((naked, interrupt));
//---------------------------------------------------------------------------
TASK_STRUCT *pstCurrentTask;
SHORT sCriticalCount;

//---------------------------------------------------------------------------
static void Task_InitVectors(void);
//---------------------------------------------------------------------------
// Set up the vector table
static void Task_InitVectors(void)
{
	// Install the SVC vector handler
	TASK_VECTOR_TABLE *pstTable;
	ULONG ulTableAddr;

	// Get the address stored in the vector table address register
	ulTableAddr = TASK_NVIC_VECTOR_REG;

	// This address is the location of the vector table
	pstTable = (TASK_VECTOR_TABLE*)ulTableAddr;
	pstTable->ulSVC_ISR = (ULONG)Task_SVC_ISR;
}

//---------------------------------------------------------------------------
/*!
	This function initializes the stack of a task
	\fn void Task_InitStack(TASK_STRUCT *pstTask_)
	\param *pstTask_ - the task whose stack we wish to initialize.
*/
void Task_InitStack(TASK_STRUCT *pstTask_)
{
	// Initialize the stack for the task.
	ULONG ulAddr;
	ULONG *pulStack;
	USHORT i;

	// Get the stack pointer.
	pulStack = (ULONG*)pstTask_->pwTopStack;
	
	// Get the "return" address for the function
	ulAddr = (ULONG)(pstTask_->pfTaskFunc);
	
	// clear the stack...
	for (i = 0; i < pstTask_->usStackSize / sizeof(WORD); i++)
	{
		pstTask_->pwStack[i] = 0xFFFFFFFF;
	}
	//-----------------------------------------------------------------------
	// Eight preserved registers as in figure 5-1 in ARM Document DDI0377E
	//-----------------------------------------------------------------------
	
	// 1) Push the XPSR register		r15
	PUSH_TO_STACK(pulStack, TASK_DEFAULT_XPSR);
	// 2) Push the PC					r13
	PUSH_TO_STACK(pulStack, ulAddr);
	// 3) push the Link register		r14
	PUSH_TO_STACK(pulStack, TASK_EXMODE);
	
	// 4) Other auto-pushed registers 
	PUSH_TO_STACK(pulStack, 0x12121212);		// r12
	PUSH_TO_STACK(pulStack, 0x03030303);		// r3
	PUSH_TO_STACK(pulStack, 0x02020202);		// r2
	PUSH_TO_STACK(pulStack, 0x01010101);		// r1
	
	// 5) Assume that the task handle is the first and only function parameter to a task
	PUSH_TO_STACK(pulStack, (ULONG)pstTask_);	// r0
	
	//-----------------------------------------------------------------------
	// Push the other registers (+ an extra copy of the link register...)
	//-----------------------------------------------------------------------
	
	// 6) Another copy of the default LR (used in task switch)
	PUSH_TO_STACK(pulStack, TASK_EXMODE);	// lr
	
	// 7) Save the rest of the non auto-pushed register
	PUSH_TO_STACK(pulStack, 0x11111111);		// r11
	PUSH_TO_STACK(pulStack, 0x10101010);		// r10
	PUSH_TO_STACK(pulStack, 0x09090909);		// r9
	PUSH_TO_STACK(pulStack, 0x08080808);		// r8
	PUSH_TO_STACK(pulStack, 0x07070707);		// r7
	PUSH_TO_STACK(pulStack, 0x06060606);		// r6
	PUSH_TO_STACK(pulStack, 0x05050505);		// r5
	PUSH_TO_STACK(pulStack, 0x04040404);		// r4
	
	pstTask_->pwTopStack = (WORD*)(pulStack+1);
}

//---------------------------------------------------------------------------
/*!
	Call this function at the end of main() to start the scheduler.
	\fn void Task_StartTasks(void)
*/
void Task_StartTasks(void)
{	
	// Install the SVC0 vector
	Task_InitVectors();
	// Set up the initial task by calling SVC 0
	asm volatile("SVC 0\n\t");
}

//---------------------------------------------------------------------------
void DISABLE_INTS(void) __attribute__ ((naked));
void DISABLE_INTS(void)
{
	asm volatile("\n\t"
	"PUSH {r0}							\n\t"
	"MOV r0, %0							\n\t"
	"MSR basepri, r0					\n\t"
	"POP {r0}							\n\t"
	"BX lr								\n\t"
	::"i"(TASK_MAX_SYSCALL_PRI));
}
//---------------------------------------------------------------------------
void ENABLE_INTS(void) __attribute__ ((naked));
void ENABLE_INTS(void)
{
	asm volatile("\n\t"
	"PUSH {r0}							\n\t"
	"MOV r0, #0							\n\t"
	"MSR basepri, r0					\n\t"
	"POP {r0}							\n\t"
	"BX lr								\n\t");
}

//---------------------------------------------------------------------------
void Task_SVC_ISR(void)
{
	//-----------------------------------------------------------------------
	// based on example 5-5 pp120, ARM document DDI0337E
	// Changes thread-mode privelege (can only be done using exceptions)...
	// Once this has been called, a "real" SVC SWI handler can be installed
	// in its place.
	//-----------------------------------------------------------------------

	asm volatile("\n\t"
	// Disable interrupts (must be guaranteed)

	"MOV r0, %0						\n\t"
	"MSR basepri, r0				\n\t"
	
	// Get the first element from the task pointed to in the current task pointer 
	// (which is the task stack pointer)
	"LDR r1, %1						\n\t"

	"LDR r1, [r1]					\n\t"
	"LDR r1, [r1]					\n\t"
	
	// Pop the "fake" stack registers from the task stack in r1
	"LDMIA r1!, {r4-r11, lr}		\n\t"
	
	// Set the process stack pointer to the task stack pointer 
	"MSR psp, r1					\n\t"
	
	// Set up the execution mode of the task (!We should not need this!)
	// ORR lr, lr, #TASK_EXMODE
	
	// Enable interrupts
	"MOV r0, #0						\n\t"
	"MSR basepri, r0				\n\t"
	
	// Return from exception into thread mode
	"BX lr							\n\t"
	".align 4						\n\t"
	::"i"(TASK_MAX_SYSCALL_PRI), "m"(pstCurrentTask));
}
