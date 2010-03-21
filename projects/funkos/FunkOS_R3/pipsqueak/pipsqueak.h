//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  pipsqueak.h

	Description:
	The FunkOS Pipsqueak RTOS header file
*/
//---------------------------------------------------------------------------

#ifndef __PIPSQUEAK_H_
#define __PIPSQUEAK_H_

//---------------------------------------------------------------------------
#include "types.h"

//---------------------------------------------------------------------------
typedef void (*TASK_FUNC)(void *pstThis_);		//!< Task handler function pointer type 

//---------------------------------------------------------------------------
typedef struct _Task_Struct
{
	WORD *pwStack;								//!< Pointer to the task's stack
	USHORT usStackSize;							//!< Size of the stack (in minimum address units)
	USHORT usTicks;								//!< Maximum number of ticks to execute before switching to the next task
	TASK_FUNC pfHandler;						//!< Task's handler function pointer
	struct _Task_Struct *pstNext;				//!< Pointer to the next task in the list (automatically handled by Task_AddTask)
} TASK_STRUCT;

//---------------------------------------------------------------------------
void Task_Init(void);
void Task_SetIdleTask(TASK_STRUCT *pstTask_);
void Task_AddTask(TASK_STRUCT *pstTask_);
void Task_GoIdle(void);
void Task_Tick(void);
void Task_Switch(void);

#endif
