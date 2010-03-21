//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvPSXjoy_in.h
//
// Description:
//	Driver definition for Sony Playstation joystick input.
//--------------------------------------------------------------------------- 

#ifndef __DRV_PSXJOY_IN_H_
#define __DRV_PSXJOY_IN_H_

#include "types.h"
#include "driver.h"
#include "joystick.h"

//---------------------------------------------------------------------------
// Module configuration
//---------------------------------------------------------------------------
#define PSX_ACK_BIT		(0x20)		// Bit 5
#define PSX_CLK_BIT		(0x10)		// Bit 4
#define PSX_ATT_BIT		(0x08)		// Bit 3
#define PSX_CMD_BIT		(0x04)		// Bit 2
#define PSX_DATA_BIT	(0x02)		// Bit 1

#define PSX_PORT		PORTC
#define PSX_PIN			PINC
#define PSX_DDR			DDRC

//---------------------------------------------------------------------------
#define MAX_PSX_DATA_BYTES		(6)
#define POLL_SPIN_COUNT			(100)

//---------------------------------------------------------------------------
// Commands sent from PSX to Joypad
#define PSX_CMD_START			(0x01)
#define PSX_CMD_POLL			(0x42)
#define PSX_CMD_IDLE			(0xFF)

// Possible TYPE command responses
#define PSX_TYPE_DIGITAL		(0x41)
#define PSX_TYPE_ANALOG_RED		(0x73)
#define PSX_TYPE_ANALOG_GREEN	(0x53)
#define PSX_TYPE_NEGCON			(0x23)

// Joypad report sizes
#define PSX_SIZE_DIGITAL		(2)
#define PSX_SIZE_ANALOG			(6)

//---------------------------------------------------------------------------
// Command set defines
//---------------------------------------------------------------------------
typedef struct
{
	//-- Must Be First ------------------------------------------------------
	DRIVER_STRUCT stDriver;

	UCHAR ucType;
	BOOL bConnected;
	
	JOYSTICK_REPORT_STRUCT stReport;
	
	UCHAR aucRaw[6];
} PSX_IN_STRUCT;
//---------------------------------------------------------------------------
void PSXi_Init(DRIVER_STRUCT *pstThis_);
void PSXi_Start(DRIVER_STRUCT *pstThis_);
void PSXi_Stop(DRIVER_STRUCT *pstThis_);
void PSXi_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_);

#endif 

