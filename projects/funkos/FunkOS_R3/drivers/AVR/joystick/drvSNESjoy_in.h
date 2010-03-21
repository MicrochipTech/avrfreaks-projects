//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvSNESjoy_in.h
//
// Description:
//	Driver definition for Nintendo SNES joystick input.
//--------------------------------------------------------------------------- 

#ifndef __DRV_SNESJOY_IN_H_
#define __DRV_SNESJOY_IN_H_

#include "types.h"
#include "driver.h"
#include "joystick.h"

//---------------------------------------------------------------------------
#define SNES_DATA_DIR		DDRC
#define SNES_LATCH_DIR		DDRC
#define SNES_CLOCK_DIR		DDRC

#define SNES_DATA_PORT		PORTC
#define SNES_LATCH_PORT		PORTC
#define SNES_CLOCK_PORT		PORTC

#define SNES_DATA_IN		PINC

#define SNES_DATA_BIT		(0x01)
#define SNES_LATCH_BIT		(0x02)
#define SNES_CLOCK_BIT		(0x04)

//---------------------------------------------------------------------------
typedef enum
{
	SNES_B = 0,
	SNES_Y,
	SNES_SELECT,
	SNES_START,
	SNES_UP,
	SNES_DOWN,
	SNES_LEFT,
	SNES_RIGHT,
	SNES_A,
	SNES_X,
	SNES_L,
	SNES_R,
	SNES_UNUSED1,
	SNES_UNUSED2,
	SNES_UNUSED3,
	SNES_UNUSED4,
	SNES_BUTTON_COUNT
} SNES_BUTTON;

//---------------------------------------------------------------------------
// Command set defines
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
typedef struct
{
	//--[Explicit Inheritence from driver parent class - must be first]--
	DRIVER_STRUCT stDriver;
	
	JOYSTICK_REPORT_STRUCT	stReport;
} SNES_IN_STRUCT;

//---------------------------------------------------------------------------
void SNESi_Init(DRIVER_STRUCT *pstThis_);
void SNESi_Start(DRIVER_STRUCT *pstThis_);
void SNESi_Stop(DRIVER_STRUCT *pstThis_);
void SNESi_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_);

#endif

