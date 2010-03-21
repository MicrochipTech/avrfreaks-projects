//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
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

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
//! ASM Macro - simplify the use of ASM directive in C 
#define ASM(x)		asm volatile(x);
//! Status register define - map to 0x003F
#define SR_			0x3F
//! Stack pointer define
#define SPH_		0x3E
#define SPL_		0x3D

//---------------------------------------------------------------------------
//! Macro to find the top of a stack given its size and top address
#define TOP_OF_STACK(x, y)		(UCHAR*) ( ((USHORT)x) + (y-1) )
//! Push a value y to the stack pointer x and decrement the stack pointer
#define PUSH_TO_STACK(x, y)		*x = y; x--;

//---------------------------------------------------------------------------
//! Save the context of the task
#define Task_SaveContext() \
ASM("push r0"); \
ASM("in r0, __SREG__"); \
ASM("cli"); \
ASM("push r0"); \
ASM("push r1"); \
ASM("clr r1"); \
ASM("push r2"); \
ASM("push r3"); \
ASM("push r4"); \
ASM("push r5"); \
ASM("push r6"); \
ASM("push r7"); \
ASM("push r8"); \
ASM("push r9"); \
ASM("push r10"); \
ASM("push r11"); \
ASM("push r12"); \
ASM("push r13"); \
ASM("push r14"); \
ASM("push r15"); \
ASM("push r16"); \
ASM("push r17"); \
ASM("push r18"); \
ASM("push r19"); \
ASM("push r20"); \
ASM("push r21"); \
ASM("push r22"); \
ASM("push r23"); \
ASM("push r24"); \
ASM("push r25"); \
ASM("push r26"); \
ASM("push r27"); \
ASM("push r28"); \
ASM("push r29"); \
ASM("push r30"); \
ASM("push r31"); \
ASM("in r0, 0x39"); \
ASM("push r0");\
ASM("in r0, 0x3A"); \
ASM("push r0");\
ASM("in r0, 0x3B"); \
ASM("push r0");\
ASM("in r0, 0x38"); \
ASM("push r0");\
ASM("in r0, 0x3C"); \
ASM("push r0");\
ASM("lds r26, pstCurrentTask"); \
ASM("lds r27, pstCurrentTask + 1"); \
ASM("in	r0, 0x3D"); \
ASM("st	x+, r0"); \
ASM("in	r0, 0x3E"); \
ASM("st	x+, r0"); 

//---------------------------------------------------------------------------
//! Restore the context of the task
#define Task_RestoreContext() \
ASM("lds r26, pstCurrentTask"); \
ASM("lds r27, pstCurrentTask + 1");\
ASM("ld	 r28, x+"); \
ASM("out 0x3D, r28"); \
ASM("ld	 r29, x+"); \
ASM("out 0x3E, r29"); \
ASM("pop r0");\
ASM("out 0x3C, r0"); \
ASM("pop r0");\
ASM("out 0x38, r0"); \
ASM("pop r0");\
ASM("out 0x3B, r0"); \
ASM("pop r0");\
ASM("out 0x3A, r0"); \
ASM("pop r0");\
ASM("out 0x39, r0"); \
ASM("pop r31"); \
ASM("pop r30"); \
ASM("pop r29"); \
ASM("pop r28"); \
ASM("pop r27"); \
ASM("pop r26"); \
ASM("pop r25"); \
ASM("pop r24"); \
ASM("pop r23"); \
ASM("pop r22"); \
ASM("pop r21"); \
ASM("pop r20"); \
ASM("pop r19"); \
ASM("pop r18"); \
ASM("pop r17"); \
ASM("pop r16"); \
ASM("pop r15"); \
ASM("pop r14"); \
ASM("pop r13"); \
ASM("pop r12"); \
ASM("pop r11"); \
ASM("pop r10"); \
ASM("pop r9"); \
ASM("pop r8"); \
ASM("pop r7"); \
ASM("pop r6"); \
ASM("pop r5"); \
ASM("pop r4"); \
ASM("pop r3"); \
ASM("pop r2"); \
ASM("pop r1"); \
ASM("pop r0"); \
ASM("out __SREG__, r0"); \
ASM("pop r0"); 

//------------------------------------------------------------------------
//! These macros *must* be used in pairs !
//------------------------------------------------------------------------
//! Enter critical section (copy status register, disable interrupts)
#define CS_ENTER()	\
{ \
volatile UCHAR x; \
x = _SFR_IO8(SR_); \
ASM("cli"); 
//------------------------------------------------------------------------
//! Exit critical section (restore status register)
#define CS_EXIT() \
_SFR_IO8(SR_) = x;\
}
	
//------------------------------------------------------------------------
//! Initiate a contex switch without using the SWI
#define Task_Yield() \
Task_SaveContext(); \
Task_Switch(); \
Task_RestoreContext();

#define ENABLE_INTS()		ASM("sei");
#define DISABLE_INTS()		ASM("cli");

//------------------------------------------------------------------------
void Task_InitStack(TASK_STRUCT *pstTask_);
void Task_StartTasks(void);
#endif //__TASKPORT_H_
