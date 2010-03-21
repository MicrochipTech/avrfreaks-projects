//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  timer.h

	Description:
		Fixed-frequency event API
*/
#ifndef __TIMER_H_
#define __TIMER_H_

#include "types.h"

//---------------------------------------------------------------------------
// Configuration parameters
//---------------------------------------------------------------------------
#define NUM_TIMERS				4		//!< This defines the size of the event table

//---------------------------------------------------------------------------
// Timer interface module for kernel
//---------------------------------------------------------------------------

typedef void (*TIMER_CALLBACK)(void);

typedef enum
{
	TIMER_STATE_UNINIT,					//!< Table entry has not been defined
	TIMER_STATE_CLOSED,					//!< Table entry is disabled
	TIMER_STATE_OPEN					//!< Table entry is enabled
} TIMER_STATE;

//---------------------------------------------------------------------------
class FunkOS_Timer
{
public:
	FunkOS_Timer();
	
	void Create(USHORT usTicks_, USHORT usOffset_, TIMER_CALLBACK pfCallback_);	
	
	TIMER_STATE GetState(void);
	void Start(void);
	void Stop(void);		
	
	void Tick(void);	
private:
	USHORT m_usTicks;	 			//!< Ticks between timer calls
	USHORT m_usOffset; 				//!< Timer Phase Offset (for timer load-balancing)
	USHORT m_usTickCount;			//!< Timer's own tick count
	TIMER_STATE m_eState;			//!< Timer state
	TIMER_CALLBACK m_pfCallback;	//!< Callback Function (Lightweight Thread)
};

//---------------------------------------------------------------------------
class FunkOS_TimerList
{
public:
	static FunkOS_Timer *Add(USHORT usTicks_, USHORT usOffset_, TIMER_CALLBACK pfCallback_);	
	static void Tick(void);
};

//---------------------------------------------------------------------------
#endif //__TIMER_H_


