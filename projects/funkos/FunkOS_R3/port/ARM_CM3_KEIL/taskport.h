//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  taskport.h

	Description:
		Cortex M-3 port of task switching macros and function headers
*/
//--------------------------------------------------------------------------- 

#ifndef __TASKPORT_H_
#define __TASKPORT_H_

#include "task.h"

//---------------------------------------------------------------------------
//! NVIC Register defines
#define TASK_NVIC_ICT_REG				*((volatile ULONG*)(0xE000E004))
#define TASK_NVIC_ICS_REG				*((volatile ULONG*)(0xE000ED04))
#define TASK_NVIC_VECTOR_REG			*((volatile ULONG*)(0xE000ED08))

//---------------------------------------------------------------------------
#define TASK_NVIC_SYSTICK_CSR_REG		*((volatile ULONG*)(0xE000E010))
#define TASK_NVIC_SYSTICK_RELOAD_REG	*((volatile ULONG*)(0xE000E014))
#define TASK_NVIC_SYSTICK_CURRENT_REG	*((volatile ULONG*)(0xE000E018))

//---------------------------------------------------------------------------
#define TASK_NVIC_PRIORITY1				*((volatile ULONG*)(0xE000ED18))
#define TASK_NVIC_PRIORITY2				*((volatile ULONG*)(0xE000ED1C))
#define TASK_NVIC_PRIORITY3				*((volatile ULONG*)(0xE000ED20))

//---------------------------------------------------------------------------
#define TASK_PENDSV_PRIORITY			(ULONG)(0x000000FF)
#define TASK_PENDSV_PRI_REGISTER		TASK_NVIC_PRIORITY3
#define TASK_PENDSV_PRI_SHIFT			(16)

#define TASK_SYSTICK_PRIORITY			(ULONG)(0x000000FF)
#define TASK_SYSTICK_PRI_REGISTER		TASK_NVIC_PRIORITY3
#define TASK_SYSTICK_PRI_SHIFT			(24)

#define TASK_SVCALL_PRIORITY			(ULONG)(0x000000FF)
#define TASK_SVCALL_PRI_REGISTER		TASK_NVIC_PRIORITY2
#define TASK_SVCALL_PRI_SHIFT			(24)

//---------------------------------------------------------------------------
#define TASK_PENDSV_SETPRI()			(TASK_PENDSV_PRI_REGISTER |= (TASK_PENDSV_PRIORITY << TASK_PENDSV_PRI_SHIFT))
#define TASK_SYSTICK_SETPRI()			(TASK_SYSTICK_PRI_REGISTER |= (TASK_SYSTICK_PRIORITY << TASK_SYSTICK_PRI_SHIFT))
#define TASK_SVCALL_SETPRI()			(TASK_SVCALL_PRI_REGISTER |= (TASK_SVCALL_PRIORITY << TASK_SVCALL_PRI_SHIFT))

//---------------------------------------------------------------------------
#define TASK_MAX_SYSCALL_PRI			0xFF			// Can be configured...

//---------------------------------------------------------------------------
typedef struct 
{
	ULONG ulGlobalStackAddr;
	ULONG ulReset_ISR;
	ULONG ulNMI_ISR;
	ULONG ulFault_ISR;
	ULONG ulMPU_ISR;
	ULONG ulBusFault_ISR;
	ULONG aulReserved[4];
	ULONG ulSVC_ISR;
	ULONG ulDebug_ISR;
	ULONG ulReserved2;
	ULONG ulPendSV_ISR;
	ULONG ulSysTick_ISR;
} TASK_VECTOR_TABLE;

//---------------------------------------------------------------------------
//! ASM Macro - simplify the use of ASM directive in C 
#define ASM(x)		__asm(x)

//---------------------------------------------------------------------------
//! Macro to find the top of a stack given its size and top address
#define TOP_OF_STACK(x, y)		(ULONG*) (((ULONG)x) + (y-1) )
//! Push a value y to the stack pointer x and decrement the stack pointer
#define PUSH_TO_STACK(x, y)		*(ULONG*)x = (ULONG)y; (ULONG*)x--;

//---------------------------------------------------------------------------
void ENABLE_INTS(void);
void DISABLE_INTS(void);

//---------------------------------------------------------------------------
#define CS_ENTER()			DISABLE_INTS()
#define CS_EXIT()			ENABLE_INTS()

//------------------------------------------------------------------------
void Task_Sleep(TASK_STRUCT *pstTask_, USHORT usTime_);
void Task_InitStack(TASK_STRUCT *pstTask_);
void Task_StartTasks(void);

#endif //__TASKPORT_H_
