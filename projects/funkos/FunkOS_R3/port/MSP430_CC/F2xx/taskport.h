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
//------------------------------------------------------------------------
//! These macros *must* be used in pairs !
#define ENABLE_INTS()   __enable_interrupt()
#define DISABLE_INTS()  __disable_interrupt()

//------------------------------------------------------------------------
//! Enter critical section (copy status register, disable interrupts)
#define CS_ENTER()	__disable_interrupt()
//------------------------------------------------------------------------
//! Exit critical section (restore status register)
#define CS_EXIT()   __enable_interrupt()

//------------------------------------------------------------------------
void Task_Sleep(USHORT usTime_);
void Task_InitStack(TASK_STRUCT *pstTask_);

#endif //__TASKPORT_H_
