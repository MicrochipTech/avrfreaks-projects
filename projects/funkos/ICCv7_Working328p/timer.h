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
typedef struct
{
	USHORT usTicks;	 	//!< Ticks between timer calls
	USHORT usOffset; 	//!< Timer Phase Offset (for timer load-balancing)
	USHORT usTickCount;	//!< Timer's own tick count
	TIMER_STATE eState;	//!< Timer state
	
	TIMER_CALLBACK pfCallback;	//!< Callback Function (Lightweight Thread)
} TIMER_STRUCT;

//---------------------------------------------------------------------------
void Timer_Init(void);
TIMER_STRUCT* Timer_Add(USHORT usTicks_, USHORT usOffset_, TIMER_CALLBACK pfCallback_);
void Timer_Start(TIMER_STRUCT *pstTimer_);
void Timer_Stop(TIMER_STRUCT *pstTimer_);
void Timer_Tick(void);

//---------------------------------------------------------------------------
#endif //__TIMER_H_


