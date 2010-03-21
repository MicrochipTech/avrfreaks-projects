//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvHD44780.h
//
// Description:
//	Driver specification for HD44780 LCD controller.  Extends
// the driver struct through the init/start/stop/control functions
// and  HD44780_CONFIG_STRUCT.
//--------------------------------------------------------------------------- 

#ifndef __DRV_HD44780_H_
#define __DRV_HD44780_H_

#include "types.h"
#include "driver.h"

//---------------------------------------------------------------------------
// Register configuration - 
#define HD44780_DATA_DIR 		DDRB
#define HD44780_DATA_OUT		PORTB
#define HD44780_DATA_SHIFT		(0)
#define HD44780_RS_DIR 			DDRB
#define HD44780_RS_BIT			(4)
#define HD44780_RS_OUT			PORTB
#define HD44780_RW_BIT			(5)
#define HD44780_RW_OUT			PORTB
#define HD44780_RW_DIR			DDRB
#define HD44780_ENABLE_DIR		DDRB
#define HD44780_ENABLE_BIT		(6)
#define HD44780_ENABLE_OUT		PORTB
#define HD44780_PIN				PINB

//---------------------------------------------------------------------------
typedef enum
{
	HD44780_EVENT_RESET = DRIVER_EVENTS,
	HD44780_EVENT_HOME,
	HD44780_EVENT_MODE,
	HD44780_EVENT_DISPLAY,
	HD44780_EVENT_CURSOR,
	HD44780_EVENT_FUNCTION,
	HD44780_EVENT_SETCG,
	HD44780_EVENT_SETDD,
	HD44780_EVENT_BUSY,
	HD44780_EVENT_WRITE,
	HD44780_EVENT_READ,
	HD44780_EVENT_SETROW,
	HD44780_EVENT_SETCOL,
	HD44780_EVENTS
} HD44780_EVENT;

//---------------------------------------------------------------------------
// Command set defines
//---------------------------------------------------------------------------
// Clear command
#define	HD44780_CMD_CLEAR			((UCHAR) 0x01)

// Cursor home 
#define HD44780_CMD_HOME			((UCHAR) 0x02)

// Entry Mode Set
#define HD44780_CMD_MODE			((UCHAR) 0x04)
//-- Bit Defines
#define HD44780_MODE_BIT_DECREMENT	((UCHAR) 0x00)
#define HD44780_MODE_BIT_INCREMENT	((UCHAR) 0x01)
#define HD44780_MODE_BIT_NOSHIFT	((UCHAR) 0x00)
#define HD44780_MODE_BIT_SHIFT		((UCHAR) 0x02)

// Display On/Off Controls
#define HD44780_CMD_DISPLAY			((UCHAR) 0x08)
//-- Bitfield defines
#define HD44780_DISPLAY_BIT_ON		((UCHAR) 0x04)
#define HD44780_DISPLAY_BIT_OFF		((UCHAR) 0x00)
#define HD44780_DISPLAY_BIT_CURSOR	((UCHAR) 0x02)
#define HD44780_DISPLAY_BIT_BLINK	((UCHAR) 0x01)

// Display/Cursor Shift
#define HD44780_CMD_CURSOR			((UCHAR) 0x10)
//-- Bitfield defines
#define HD44780_CURSOR_MOVE			((UCHAR) 0x00)
#define HD44780_CURSOR_SHIFT		((UCHAR) 0x08)
#define HD44780_CURSOR_LEFT			((UCHAR) 0x00)
#define HD44780_CURSOR_RIGHT		((UCHAR) 0x04)

// Function Set
#define HD44780_CMD_FUNCTION		((UCHAR) 0x20)
//-- Bitfields
#define HD44780_FUNCTION_4BIT		((UCHAR) 0x00)
#define HD44780_FUNCTION_8BIT		((UCHAR) 0x10)
#define HD44780_FUNCTION_1LINE		((UCHAR) 0x00)
#define HD44780_FUNCTION_2LINE		((UCHAR) 0x08)
#define HD44780_FUNCTION_5X7		((UCHAR) 0x00)
#define HD44780_FUNCTION_5X10		((UCHAR) 0x04)

// Function, set CGRAM Address
#define HD44780_CMD_SETCGRAM		((UCHAR) 0x40)
#define HD44780_CMD_SETDDRAM		((UCHAR) 0x80)

//---------------------------------------------------------------------------
typedef struct
{
	//--[Explicit Inheritence from driver parent class - must be first]--
	DRIVER_STRUCT stDriver;
	//-------------------------------------------------------------------

	USHORT usNumCols;		// Number of columns
	USHORT usNumRows;		// Number of Rows
	BOOL b4Bit;				// 4 bit or 8 bit
	BOOL bInit;				
	UCHAR ucRow;
	UCHAR ucCol;
} HD44780_DRIVER_STRUCT;

//---------------------------------------------------------------------------
void HD44780_Init(DRIVER_STRUCT *pstThis_);
void HD44780_Start(DRIVER_STRUCT *pstThis_);
void HD44780_Stop(DRIVER_STRUCT *pstThis_);
void HD44780_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_);


#endif //__DRV_HD44780_H_

