//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drv7800joy_in.h
//
// Description:
//	Driver definition for Atari 7800 joystick input.
//--------------------------------------------------------------------------- 

#ifndef __DRV_7800JOY_IN_H_
#define __DRV_7800JOY_IN_H_

#include "types.h"
#include "driver.h"
#include "joystick.h"

//---------------------------------------------------------------------------
typedef enum
{
	ATARI7800_IN_BUTTON_UP = 0,
	ATARI7800_IN_BUTTON_DOWN,
	ATARI7800_IN_BUTTON_LEFT,
	ATARI7800_IN_BUTTON_RIGHT,
	ATARI7800_IN_BUTTON_BUTTONL,
	ATARI7800_IN_BUTTON_BUTTONR,
	ATARI7800_IN_BUTTON_COUNT
} ATARI7800_IN_BUTTONS;

//---------------------------------------------------------------------------
// Command set defines
//---------------------------------------------------------------------------
// Clear command
#define	ATARI7800_IN_CMD_READ			((UCHAR) 0x01)

// Debounce logic defines...
#define ATARI7800_IN_INITIAL			(50)
#define ATARI7800_IN_HIGHCOUNT			(100)
#define ATARI7800_IN_LOWCOUNT			(0)
#define ATARI7800_IN_TIMEOUT			(500)

//---------------------------------------------------------------------------
typedef struct
{
	//--[Explicit Inheritence from driver parent class - must be first]--
	DRIVER_STRUCT stDriver;
	
	//-------------------------------------------------------------------	
	// Driver-specific data
	
	volatile UCHAR *pucPortInUp;		
	volatile UCHAR *pucPortInDown;
	volatile UCHAR *pucPortInLeft;
	volatile UCHAR *pucPortInRight;
	volatile UCHAR *pucPortInButtonL;
	volatile UCHAR *pucPortInButtonR;

	volatile UCHAR *pucPortUp;		
	volatile UCHAR *pucPortDown;
	volatile UCHAR *pucPortLeft;
	volatile UCHAR *pucPortRight;
	volatile UCHAR *pucPortButtonL;
	volatile UCHAR *pucPortButtonR;
	
	volatile UCHAR *pucPortDirUp;		
	volatile UCHAR *pucPortDirDown;
	volatile UCHAR *pucPortDirLeft;
	volatile UCHAR *pucPortDirRight;
	volatile UCHAR *pucPortDirButtonL;
	volatile UCHAR *pucPortDirButtonR;
	
	UCHAR ucPortBitUp;		
	UCHAR ucPortBitDown;
	UCHAR ucPortBitLeft;
	UCHAR ucPortBitRight;
	UCHAR ucPortBitButtonL;
	UCHAR ucPortBitButtonR;
	UCHAR ucPortBitButtonC;
	UCHAR ucPortBitGround;

	JOYSTICK_REPORT_STRUCT	stReport;
} ATARI7800_IN_STRUCT;

//---------------------------------------------------------------------------
void Atari7800i_Init(DRIVER_STRUCT *pstThis_);
void Atari7800i_Start(DRIVER_STRUCT *pstThis_);
void Atari7800i_Stop(DRIVER_STRUCT *pstThis_);
void Atari7800i_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_);


#endif 

