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

#include <iom328p.h>

//---------------------------------------------------------------------------
//!! ICCv7 specific defines
#define KERNEL_CALLSTACK_SIZE	(40)
#define FROM_FLASH				__flash signed char * __flash


//---------------------------------------------------------------------------
//! ASM Macro - simplify the use of ASM directive in C 
#define ASM(x)		asm(x);
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
#define Task_SaveContext_C() \
ASM("st -y, r0"); \
ASM("in r0, 0x3F"); \
ASM("cli"); \
ASM("st -y, r0"); \
ASM("in r0, 0x3D"); \
ASM("st -y, r0"); \
ASM("in r0, 0x3E"); \
ASM("st -y, r0"); \
ASM("st -y, r1"); \
ASM("st -y, r2"); \
ASM("st -y, r3"); \
ASM("st -y, r4"); \
ASM("st -y, r5"); \
ASM("st -y, r6"); \
ASM("st -y, r7"); \
ASM("st -y, r8"); \
ASM("st -y, r9"); \
ASM("st -y, r10"); \
ASM("st -y, r11"); \
ASM("st -y, r12"); \
ASM("st -y, r13"); \
ASM("st -y, r14"); \
ASM("st -y, r15"); \
ASM("st -y, r16"); \
ASM("st -y, r17"); \
ASM("st -y, r18"); \
ASM("st -y, r19"); \
ASM("st -y, r20"); \
ASM("st -y, r21"); \
ASM("st -y, r22"); \
ASM("st -y, r23"); \
ASM("st -y, r24"); \
ASM("st -y, r25"); \
ASM("st -y, r26"); \
ASM("st -y, r27"); \
ASM("st -y, r30"); \
ASM("st -y, r31"); \
ASM("lds r26, pstCurrentTask"); \
ASM("lds r27, pstCurrentTask + 1"); \
ASM("st	x+, r28"); \
ASM("st	x+, r29"); 

//---------------------------------------------------------------------------
//! Restore the context of the task
#define Task_RestoreContext_C() \
ASM("lds r26, pstCurrentTask"); \
ASM("lds r27, pstCurrentTask + 1");\
ASM("ld r28, x+"); \
ASM("ld r29, x+"); \
ASM("ld r31, y+"); \
ASM("ld r30, y+"); \
ASM("ld r27, y+"); \
ASM("ld r26, y+"); \
ASM("ld r25, y+"); \
ASM("ld r24, y+"); \
ASM("ld r23, y+"); \
ASM("ld r22, y+"); \
ASM("ld r21, y+"); \
ASM("ld r20, y+"); \
ASM("ld r19, y+"); \
ASM("ld r18, y+"); \
ASM("ld r17, y+"); \
ASM("ld r16, y+"); \
ASM("ld r15, y+"); \
ASM("ld r14, y+"); \
ASM("ld r13, y+"); \
ASM("ld r12, y+"); \
ASM("ld r11, y+"); \
ASM("ld r10, y+"); \
ASM("ld r9, y+"); \
ASM("ld r8, y+"); \
ASM("ld r7, y+"); \
ASM("ld r6, y+"); \
ASM("ld r5, y+"); \
ASM("ld r4, y+"); \
ASM("ld r3, y+"); \
ASM("ld r2, y+"); \
ASM("ld r1, y+"); \
ASM("ld r0, y+"); \
ASM("out 0x3E, r0"); \
ASM("ld r0, y+"); \
ASM("out 0x3D, r0"); \
ASM("ld r0, y+"); \
ASM("out 0x3F, r0"); \
ASM("ld r0, y+"); 

//------------------------------------------------------------------------
//! These macros *must* be used in pairs !
//------------------------------------------------------------------------
//! Enter critical section (copy status register, disable interrupts)
#define CS_ENTER() \
{ \
 volatile UCHAR x; \
 x = SREG; \
 ASM("cli"); 


//------------------------------------------------------------------------
//! Exit critical section (restore status register)
#define CS_EXIT() \
SREG = x;\
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
