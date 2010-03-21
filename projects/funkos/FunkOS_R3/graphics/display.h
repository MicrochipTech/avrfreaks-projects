//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file display.h

	Description:
		Display driver type - used as a bridge between the display hardware
		itself and the hardware abstraction layer.
*/
//---------------------------------------------------------------------------

#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include "types.h"
#include "driver.h"

//---------------------------------------------------------------------------
// acceleration flags, use in the driver declaration to provide access to 
// hardware accelerated implementations.
#define ACCELERATED_CLEAR			(0x0001)
#define ACCELERATED_POINT			(0x0002)
#define ACCELERATED_LINE			(0x0004)
#define ACCELERATED_CIRCLE			(0x0008)
#define ACCELERATED_ELLPISE			(0x0010)
#define ACCELERATED_RECTANGLE		(0x0020)
#define ACCELERATED_TEXT			(0x0040)
#define ACCELERATED_CONSOLE_TEXT	(0x0080)
#define ACCELERATED_STAMP			(0x0100)
#define ACCELERATED_BITMAP			(0x0200)

//---------------------------------------------------------------------------
typedef void (*DRAW_PIXEL_FUNC)(void *pstThis_, void *pstPixel_);

//---------------------------------------------------------------------------
typedef struct
{
	// Must be first!!!
	DRIVER_STRUCT *pstDriver;

	// These are for all display drivers
	USHORT usResX;
	USHORT usResY;
	UCHAR ucBPP;
	USHORT usAccelerationFlags;
	
	// Must be implemented for software rendering.
	DRAW_PIXEL_FUNC	pfDrawPixel;
	
} DISPLAY_DRIVER_STRUCT;

//---------------------------------------------------------------------------
// Event definitions for 2D hardware accelerated graphics functions
typedef enum
{
	//--[Mandatory for a display driver]-------------------------------------
	DISPLAY_EVENT_PIXEL = DRIVER_EVENTS,	
	
	//--[Optional if supported in hardware]----------------------------------
	DISPLAY_EVENT_CLEAR,
	DISPLAY_EVENT_LINE,
	DISPLAY_EVENT_RECTANGLE,
	DISPLAY_EVENT_CIRCLE,
	DISPLAY_EVENT_ELLIPSE,
	DISPLAY_EVENT_BITMAP,
	DISPLAY_EVENT_STAMP,
	DISPLAY_EVENT_TEXT,
	DISPLAY_EVENT_CONSOLE_TEXT
} DISPLAY_EVENT;

#endif __DISPLAY_H_
