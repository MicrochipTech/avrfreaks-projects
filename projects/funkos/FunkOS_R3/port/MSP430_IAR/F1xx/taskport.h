//---------------------------------------------------------------------------
// RAVR RTK - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  taskport.h

	Description:
		AVRMega port of task switching macros and function headers
*/
//--------------------------------------------------------------------------- 

#ifndef __TASKPORT_H_
#define __TASKPORT_H_

#include "task.h"
#include <io430.h>
#include <in430.h>

//---------------------------------------------------------------------------
extern SHORT sCriticalCount;
//---------------------------------------------------------------------------
extern TASK_STRUCT *pstCurrentTask;	//! extern'd in taskport.h

//---------------------------------------------------------------------------
//! ASM Macro - simplify the use of ASM directive in C 
#define ASM(x)		asm(x);
//---------------------------------------------------------------------------
//! Macro to find the top of a stack given its size and top address
#define TOP_OF_STACK(x, y)		(USHORT*) ((USHORT)x + y - 2)
//! Push a value y to the stack pointer x and decrement the stack pointer
#define PUSH_TO_STACK(x, y)		*x = y; x--;

//---------------------------------------------------------------------------
//! Save the context of the task
#define Task_SaveContext() \
    ASM("push	r4\n");	\
    ASM("push	r5\n");	\
    ASM("push	r6\n");	\
    ASM("push	r7\n");	\
    ASM("push	r8\n");	\
    ASM("push	r9\n");	\
    ASM("push	r10\n"); \
    ASM("push	r11\n"); \
    ASM("push	r12\n"); \
    ASM("push	r13\n"); \
    ASM("push	r14\n"); \
    ASM("push	r15\n"); \
    ASM("mov.w	&pstCurrentTask, r12\n"); \
    ASM("mov.w	r1, 0(r12)\n");
//---------------------------------------------------------------------------
//! Restore the context of the task
#define Task_RestoreContext() \
	ASM("mov.w  &pstCurrentTask, r12"); \
	ASM("mov.w  @r12, r1"); \
	ASM("pop	r15"); \
	ASM("pop	r14"); \
	ASM("pop	r13"); \
	ASM("pop	r12"); \
	ASM("pop	r11"); \
	ASM("pop	r10"); \
	ASM("pop 	r9"); \
	ASM("pop	r8"); \
	ASM("pop	r7"); \
	ASM("pop	r6"); \
	ASM("pop	r5"); \
	ASM("pop	r4"); \
    ASM("bic.w	#0x00F0, 0(SP)");

//------------------------------------------------------------------------
//! These macros *must* be used in pairs !
#define ENABLE_INTS()   __enable_interrupt()
#define DISABLE_INTS()  __disable_interrupt()

//------------------------------------------------------------------------
//! Enter critical section (copy status register, disable interrupts)
#define CS_ENTER()	__disable_interrupt()
           
//------------------------------------------------------------------------
//! Exit critical section (restore status register)
#define CS_EXIT()       __enable_interrupt()

//------------------------------------------------------------------------
void Task_Sleep(USHORT usTime_);
void Task_InitStack(TASK_STRUCT *pstTask_);
__task void Task_StartTasks(void);
#endif //__TASKPORT_H_
