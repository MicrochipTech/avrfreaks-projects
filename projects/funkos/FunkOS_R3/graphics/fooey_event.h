//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  fooey_event.h

	Description:
    Define the event types used for manipulating a UI built with Fooey GUI.
*/
//---------------------------------------------------------------------------
#ifndef __FOOEY_EVENT_H_
#define __FOOEY_EVENT_H_

#include "types.h"
#include "keycodes.h"
//---------------------------------------------------------------------------
//! Event types
typedef enum
{
    FOOEY_EVENT_TICK,
	FOOEY_EVENT_TYPE_KEYBOARD,
	FOOEY_EVENT_TYPE_MOUSE,
	//!! Other event type IDs go here.
	FOOEY_EVENT_TYPES
} FOOEY_EVENT_TYPE;

//---------------------------------------------------------------------------
typedef struct
{
	USHORT usEventID;					//!< Event type (used for subclassification)
	USHORT usEventTime;					//!< Number of event ticks elapsed
    USHORT usDest;                      //!< Destination control

	union
	{
		ULONG ulRawData;				//!< Raw 32-bit data field, all other structs are specializations that are < 32-bits wide
		
		struct
		{
			ULONG ulTickIntervalMS;			//!< Time since last tick in MS
		} TICK_EVENT_DATA;		
		struct
		{
			unsigned int usX:12;			//!< X Position of the mouse event
			unsigned int bLeftState:1;		//!< Left button state
			unsigned int bRightState:1;		//!< Right button state
			unsigned int bMiddleState:1;	//!< Middle button state
			unsigned int bSideState1:1;		//!< Side button state
			unsigned int usY:12;			//!< Y Position of the mouse event
			unsigned int bXScrollState:1;	//!< Left scroll state
			unsigned int bYScrollState:1;	//!< Right scroll state
			unsigned int bSideState2:1;		//!< Second side button state
		} MOUSE_EVENT_DATA;
		struct
		{
			KEYCODE eKeyCode;				//!< 8-bit keycode (as an enumeration)
			unsigned int bKeyDown:1;		//!< 1 if key is down, 0 if key is up
			unsigned int bShift:1;			//!< 1 if shift is held
			unsigned int bAlt:1;			//!< 1 if ALT is held
			unsigned int bCtrl:1;			//!< 1 if CTRL is held
			unsigned int bSpec1:1;			//!< Special shift 1
			unsigned int bSpec2:1;			//!< Special shift 2
			unsigned int bSpec3:1;			//!< Special shift 3
			unsigned int bSpec4:1;			//!< Special shift 4
		} KEYBOARD_EVENT_DATA;
		struct
		{
			unsigned int bUp:1;				//!< Up button
			unsigned int bDown:1;			//!< Down button
			unsigned int bLeft:1;			//!< Left button
			unsigned int bRight:1;			//!< Right button
			unsigned int uiButtons:12;		//!< 12 Digital IOs
			CHAR cAnalogX;					//!< 8-bit signed integer for X axis
			CHAR cAnalogY;					//!< 8-bit signed integer for Y axis
		} JOYSTICK_EVENT_DATA;
	}EVENT_DATA;
} FOOEY_EVENT_STRUCT;

#endif //__FOOEY_EVENT_H_
