//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  watchdog.h

	Description:
		Portable multitasking watchdog timer interface
*/
//---------------------------------------------------------------------------
#ifndef __WATCHDOG_H_
#define __WATCHDOG_H_

#include "types.h"

//---------------------------------------------------------------------------
typedef enum
{
	WDT_TASK_UNINIT = 0,
	WDT_TASK_IDLE,
	WDT_TASK_ACTIVE
} WATCHDOG_STATE;

//---------------------------------------------------------------------------
typedef struct Watchdog_Task
{
	USHORT usTimeout;
	USHORT usCurrentTime;
	WATCHDOG_STATE eState;
} WATCHDOG_TASK;

//---------------------------------------------------------------------------
#define NUM_WDT_ENTRIES					((USHORT) 20)
#define TIME_PER_KICK_MS				((USHORT) 200)

//---------------------------------------------------------------------------
void Watchdog_Init(void);
BOOL Watchdog_AddTask(WATCHDOG_TASK *pstTask_, USHORT usTime_);
void Watchdog_RemoveTask(WATCHDOG_TASK *pstTask_);
void Watchdog_StartTask(WATCHDOG_TASK *pstTask_);
void Watchdog_IdleTask(WATCHDOG_TASK *pstTask_);
//---------------------------------------------------------------------------
void Watchdog_Tick(void);

#endif
