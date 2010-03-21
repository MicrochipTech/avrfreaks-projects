//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvNESjoy_in.h
//
// Description:
//	Driver definition for Nintendo NES joystick input.
//--------------------------------------------------------------------------- 

#ifndef __DRV_NESJOY_IN_H_
#define __DRV_NESJOY_IN_H_

#include "types.h"
#include "driver.h"
#include "joystick.h"
//---------------------------------------------------------------------------
#define NES_DATA_DIR		DDRC
#define NES_LATCH_DIR		DDRC
#define NES_CLOCK_DIR		DDRC

#define NES_DATA_PORT		PORTC
#define NES_LATCH_PORT		PORTC
#define NES_CLOCK_PORT		PORTC

#define NES_DATA_IN			PINC

#define NES_DATA_BIT		(0x01)
#define NES_LATCH_BIT		(0x02)
#define NES_CLOCK_BIT		(0x04)

//---------------------------------------------------------------------------
typedef enum
{
	NES_A = 0,
	NES_B,
	NES_SELECT,
	NES_START,
	NES_UP,
	NES_DOWN,
	NES_LEFT,
	NES_RIGHT,
	NES_BUTTON_COUNT
} NES_BUTTON;

//---------------------------------------------------------------------------
// Command set defines
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
typedef struct
{
	//--[Explicit Inheritence from driver parent class - must be first]--
	DRIVER_STRUCT stDriver;
	JOYSTICK_REPORT_STRUCT	stReport;
} NES_IN_STRUCT;

//---------------------------------------------------------------------------
void NESi_Init(DRIVER_STRUCT *pstThis_);
void NESi_Start(DRIVER_STRUCT *pstThis_);
void NESi_Stop(DRIVER_STRUCT *pstThis_);
void NESi_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_);

#endif

