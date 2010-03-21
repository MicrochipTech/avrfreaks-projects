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
class NESi : public FunkOS_Driver
{
public:
	virtual BOOL DeviceInit(void);
	virtual BOOL DeviceStart(void);
	virtual BOOL DeviceStop(void);
	virtual USHORT DeviceControl(USHORT usID_, void *pvData_);

private:
	JOYSTICK_REPORT_STRUCT	m_stReport;
	void Read(void);
};

//---------------------------------------------------------------------------

#endif

