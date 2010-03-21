//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
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
#define NUM_WDT_ENTRIES					((USHORT) 20)
#define TIME_PER_KICK_MS				((USHORT) 200)

//---------------------------------------------------------------------------
typedef enum
{
	WDT_TASK_UNINIT = 0,
	WDT_TASK_IDLE,
	WDT_TASK_ACTIVE
} WATCHDOG_STATE;

//---------------------------------------------------------------------------
class FunkOS_Watchdog
{

public:		
	void Init(void);
	BOOL AddTask(USHORT usTime_);
	void RemoveTask(void);
	void Start(void);
	void Idle(void);

	USHORT GetTime(void);
	USHORT GetTimeout(void);
	void AddTime(USHORT usTime_);
	WATCHDOG_STATE GetState(void);

	static void Tick(void);
private:
	USHORT m_usTimeout;
	USHORT m_usCurrentTime;
	WATCHDOG_STATE m_eState;
};


#endif
