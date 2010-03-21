//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvGen6joy_in.h
//
// Description:
//	Driver definition for Sega Genesis 6-button joystick input.
//--------------------------------------------------------------------------- 

#ifndef __DRV_GEN6JOY_IN_H_
#define __DRV_Gen6JOY_IN_H_

#include "types.h"
#include "driver.h"
#include "joystick.h"

//---------------------------------------------------------------------------
// Command set defines
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
typedef struct
{
	//--[Explicit Inheritence from driver parent class - must be first]--
	DRIVER_STRUCT stDriver;
	
	//-------------------------------------------------------------------	
	// Driver-specific data
	
	volatile UCHAR *pucPortSelect;		
	volatile UCHAR *pucPortData1;
	volatile UCHAR *pucPortData2;
	volatile UCHAR *pucPortData3;
	volatile UCHAR *pucPortData4;
	volatile UCHAR *pucPortData6;
	volatile UCHAR *pucPortData9;

	volatile UCHAR *pucPortDirSelect;		
	volatile UCHAR *pucPortDirData1;
	volatile UCHAR *pucPortDirData2;
	volatile UCHAR *pucPortDirData3;
	volatile UCHAR *pucPortDirData4;
	volatile UCHAR *pucPortDirData6;
	volatile UCHAR *pucPortDirData9;
	
	volatile UCHAR *pucPortInData1;
	volatile UCHAR *pucPortInData2;
	volatile UCHAR *pucPortInData3;
	volatile UCHAR *pucPortInData4;
	volatile UCHAR *pucPortInData6;
	volatile UCHAR *pucPortInData9;
	
	UCHAR ucBitSelect;		
	UCHAR ucBitData1;
	UCHAR ucBitData2;
	UCHAR ucBitData3;
	UCHAR ucBitData4;
	UCHAR ucBitData6;
	UCHAR ucBitData9;
	
	JOYSTICK_REPORT_STRUCT	stReport;
} GEN6_IN_STRUCT;

//---------------------------------------------------------------------------
void Gen6i_Init(DRIVER_STRUCT *pstThis_);
void Gen6i_Start(DRIVER_STRUCT *pstThis_);
void Gen6i_Stop(DRIVER_STRUCT *pstThis_);
void Gen6i_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_);

#endif

