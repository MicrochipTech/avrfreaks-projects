//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drv2600joy_in.h
//
// Description:
//	Driver definition for Atari 2600 joystick input.
//--------------------------------------------------------------------------- 

#ifndef __DRV_2600JOY_IN_H_
#define __DRV_2600JOY_IN_H_

#include "types.h"
#include "driver.h"
#include "joystick.h"

//---------------------------------------------------------------------------
// joypad configuration
#define ATARI_DIR_UP		DDRC
#define ATARI_DIR_DOWN		DDRC
#define ATARI_DIR_LEFT		DDRC
#define ATARI_DIR_RIGHT		DDRC
#define ATARI_DIR_FIRE		DDRC

#define ATARI_PORT_UP		PORTC
#define ATARI_PORT_DOWN		PORTC
#define ATARI_PORT_LEFT		PORTC
#define ATARI_PORT_RIGHT	PORTC
#define ATARI_PORT_FIRE		PORTC

#define ATARI_PIN_UP		PINC
#define ATARI_PIN_DOWN		PINC
#define ATARI_PIN_LEFT		PINC
#define ATARI_PIN_RIGHT		PINC
#define ATARI_PIN_FIRE		PINC

#define ATARI_BIT_UP		(0x20)
#define ATARI_BIT_DOWN		(0x10)
#define ATARI_BIT_LEFT		(0x04)
#define ATARI_BIT_RIGHT		(0x02)
#define ATARI_BIT_FIRE		(0x01)

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
#define	ATARI2600_IN_CMD_READ			((UCHAR) 0x01)

// Debounce logic defines...
#define ATARI2600_IN_INITIAL			(5)
#define ATARI2600_IN_HIGHCOUNT			(10)
#define ATARI2600_IN_LOWCOUNT			(0)
#define ATARI2600_IN_TIMEOUT			(50)

//---------------------------------------------------------------------------
typedef struct
{
	//--[Explicit Inheritence from driver parent class - must be first]--
	DRIVER_STRUCT stDriver;
	
	JOYSTICK_REPORT_STRUCT	stReport;
} ATARI2600_IN_STRUCT;

//---------------------------------------------------------------------------
void Atari2600i_Init(DRIVER_STRUCT *pstThis_);
void Atari2600i_Start(DRIVER_STRUCT *pstThis_);
void Atari2600i_Stop(DRIVER_STRUCT *pstThis_);
void Atari2600i_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_);


#endif //__DRV_HD44780_H_

