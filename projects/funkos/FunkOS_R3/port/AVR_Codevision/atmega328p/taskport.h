//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  taskport.h

	Description:
		AVRMega port of task switching macros and function headers
*/
//--------------------------------------------------------------------------- 
// This PORT to codevision C Compiler was made by Ahmad Shawqy(ahmad.shawqyatgmaildotcom)
// hoping it will be of use
// and to thank the original author of FUNKOS
// regards to Mark and engoy Funkos
//--------------------------------------------------------------------------- 

#ifndef __TASKPORT_H_
#define __TASKPORT_H_

#include "task.h"

#include <mega8.h>
//#include <avr\interrupt.h>

//---------------------------------------------------------------------------
//! #asm Macro - simplify the use of #asm directive in C 

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
#asm("st	-y, r0"); \
#asm("in	r0, 0x3F"); \
#asm("cli"); \
#asm("st	-y, r0"); \
#asm("in	r0, 0x3D"); \
#asm("st	-y, r0"); \
#asm("in	r0, 0x3E"); \
#asm("st	-y, r0"); \
#asm("st	-y, r1"); \
#asm("st	-y, r2"); \
#asm("st	-y, r3"); \
#asm("st	-y, r4"); \
#asm("st	-y, r5"); \
#asm("st	-y, r6"); \
#asm("st	-y, r7"); \
#asm("st	-y, r8"); \
#asm("st	-y, r9"); \
#asm("st	-y, r10"); \
#asm("st	-y, r11"); \
#asm("st	-y, r12"); \
#asm("st	-y, r13"); \
#asm("st	-y, r14"); \
#asm("st	-y, r15"); \
#asm("st	-y, r16"); \
#asm("st	-y, r17"); \
#asm("st	-y, r18"); \
#asm("st	-y, r19"); \
#asm("st	-y, r20"); \
#asm("st	-y, r21"); \
#asm("st	-y, r22"); \
#asm("st	-y, r23"); \
#asm("st	-y, r24"); \
#asm("st	-y, r25"); \
#asm("st	-y, r26"); \
#asm("st	-y, r27"); \
#asm("st	-y, r30"); \
#asm("st	-y, r31"); \
#asm("lds r26, _pstCurrentTask"); \
#asm("lds r27, _pstCurrentTask + 1"); \
#asm("st	x+, r28"); \
#asm("st	x+, r29");


//---------------------------------------------------------------------------
//! Restore the context of the task

#define Task_RestoreContext() \
#asm("lds r26, _pstCurrentTask"); \
#asm("lds r27, _pstCurrentTask + 1");\
#asm("ld	r28, x+"); \
#asm("ld	r29, x+"); \
#asm("ld	r31, y+"); \
#asm("ld	r30, y+"); \
#asm("ld 	r27, y+"); \
#asm("ld	r26, y+"); \
#asm("ld	r25, y+"); \
#asm("ld	r24, y+"); \
#asm("ld	r23, y+"); \
#asm("ld	r22, y+"); \
#asm("ld	r21, y+"); \
#asm("ld	r20, y+"); \
#asm("ld	r19, y+"); \
#asm("ld	r18, y+"); \
#asm("ld	r17, y+"); \
#asm("ld	r16, y+"); \
#asm("ld	r15, y+"); \
#asm("ld	r14, y+"); \
#asm("ld	r13, y+"); \
#asm("ld	r12, y+"); \
#asm("ld	r11, y+"); \
#asm("ld	r10, y+"); \
#asm("ld	r9, y+"); \
#asm("ld	r8, y+"); \
#asm("ld	r7, y+"); \
#asm("ld	r6, y+"); \
#asm("ld	r5, y+"); \
#asm("ld	r4, y+"); \
#asm("ld	r3, y+"); \
#asm("ld	r2, y+"); \
#asm("ld	r1, y+"); \
#asm("ld	r0, y+"); \
#asm("out 0x3E, r0"); \
#asm("ld	r0, y+"); \
#asm("out 0x3D, r0"); \
#asm("ld	r0, y+"); \
#asm("out 0x3F, r0");\
#asm("ld	r0, y+");
//------------------------------------------------------------------------
//! These macros *must* be used in pairs !
//------------------------------------------------------------------------
//! Enter critical section (copy status register, disable interrupts)
#define CS_ENTER()\
{\
volatile UCHAR x;\
x = SREG; \
#asm("cli"); 
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

#define ENABLE_INTS()		#asm("sei");
#define DISABLE_INTS()		#asm("cli");

//------------------------------------------------------------------------
void Task_InitStack(TASK_STRUCT *pstTask_);
void Task_StartTasks(void);

#endif //__TASKPORT_H_
