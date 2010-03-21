//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drv2600joy_in.h
//
// Description:
//	Driver definition for Atari 2600 joystick input.
//--------------------------------------------------------------------------- 

#ifndef __DRV_ARCADE_IN_H_
#define __DRV_ARCADE_IN_H_

#include "types.h"
#include "driver.h"
#include "joystick.h"

//---------------------------------------------------------------------------
typedef enum
{
	ATARI2600_IN_BUTTON_UP = 0,
	ATARI2600_IN_BUTTON_DOWN,
	ATARI2600_IN_BUTTON_LEFT,
	ATARI2600_IN_BUTTON_RIGHT,
	ATARI2600_IN_BUTTON_BUTTON,
	ATARI2600_IN_BUTTON_COUNT
} ATARI2600_IN_BUTTONS;

//---------------------------------------------------------------------------
// Command set defines
//---------------------------------------------------------------------------
// Clear command

#define ARCADE_UP_DIR					DDRC
#define ARCADE_DOWN_DIR					DDRC
#define ARCADE_LEFT_DIR					DDRC
#define ARCADE_RIGHT_DIR				DDRC
#define ARCADE_BUTTON1_DIR				DDRC

#define ARCADE_UP_PORT					PORTC
#define ARCADE_DOWN_PORT				PORTC
#define ARCADE_LEFT_PORT				PORTC
#define ARCADE_RIGHT_PORT				PORTC
#define ARCADE_BUTTON1_PORT				PORTC

#define	ARCADE_UP_BIT					(0x20)
#define ARCADE_DOWN_BIT					(0x10)
#define ARCADE_LEFT_BIT					(0x08)
#define ARCADE_RIGHT_BIT				(0x04)
#define ARCADE_BUTTON1_BIT				(0x02)

#define ARCADE_UP_IN					PINC
#define ARCADE_DOWN_IN					PINC
#define ARCADE_LEFT_IN					PINC
#define ARCADE_RIGHT_IN					PINC
#define ARCADE_BUTTON1_IN				PINC


//---------------------------------------------------------------------------
typedef struct
{
	//--[Explicit Inheritence from driver parent class - must be first]--
	DRIVER_STRUCT stDriver;
	
	//-------------------------------------------------------------------	
	// Driver-specific data	
	JOYSTICK_REPORT_STRUCT	stReport;
} ARCADE_IN_STRUCT;

//---------------------------------------------------------------------------
void Arcade_i_Init(DRIVER_STRUCT *pstThis_);
void Arcade_i_Start(DRIVER_STRUCT *pstThis_);
void Arcade_i_Stop(DRIVER_STRUCT *pstThis_);
void Arcade_i_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_);


#endif //__DRV_HD44780_H_

