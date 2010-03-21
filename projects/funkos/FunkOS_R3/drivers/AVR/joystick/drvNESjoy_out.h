//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvNESjoy_out.h
//
// Description:
//	Driver definition for Nintendo NES joystick output.
//--------------------------------------------------------------------------- 

#ifndef __DRV_NESJOY_OUT_H_
#define __DRV_NESJOY_OUT_H_

#include "types.h"
#include "driver.h"
#include "joystick.h"
//---------------------------------------------------------------------------
#define NES_O_LATCH_DIR		PORTD
#define NES_O_CLOCK_DIR		PORTD
#define NES_O_DATA_DIR		PORTD

#define NES_O_LATCH_BIT		(0x80)		// Port D - Bit 7 (PCINT23)
#define NES_O_CLOCK_BIT		(0x40)		// Port D - Bit 6 (PCINT22)
#define NES_O_DATA_BIT		(0x20)		// Port D - Bit 5

#define NES_O_LATCH_IN		PIND
#define NES_O_CLOCK_IN		PIND
#define NES_O_DATA_PORT		PORTD

#define NES_O_LATCH_PORT	PIND
#define NES_O_CLOCK_PORT	PIND

//---------------------------------------------------------------------------
typedef struct
{
	//--[Explicit Inheritence from driver parent class - must be first]--
	DRIVER_STRUCT stDriver;
	JOYSTICK_REPORT_STRUCT	stReport;
} NES_OUT_STRUCT;

//---------------------------------------------------------------------------
void NESo_Init(DRIVER_STRUCT *pstThis_);
void NESo_Start(DRIVER_STRUCT *pstThis_);
void NESo_Stop(DRIVER_STRUCT *pstThis_);
void NESo_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_);

#endif 
