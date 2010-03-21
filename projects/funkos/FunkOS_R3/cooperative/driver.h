//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file driver.h

	Description:
		Implements a virtual base "class" for any driver type, and generic
		accessor functions for controlling a driver from its handle.
*/
//---------------------------------------------------------------------------
#ifndef __DRIVER_H_
#define __DRIVER_H_

#include "types.h"

//---------------------------------------------------------------------------
typedef BOOL (*DRIVER_CONSTRUCTOR)(void *pstThis_);			//!< Constructor function pointer type 
typedef BOOL (*DRIVER_START)(void *pstThis_);				//!< Driver start function pointer type 
typedef BOOL (*DRIVER_STOP)(void *pstThis_);				//!< Driver stop function pointer type 
typedef BOOL (*DRIVER_CONTROL)(void *pstThis_, USHORT usID_, void *pvData_);	//!< Driver event function pointer type
typedef USHORT (*DRIVER_READ)(void *pstThis_, UCHAR *pucData_, USHORT usLen_);	//!< Driver stream read function type
typedef USHORT (*DRIVER_WRITE)(void *pstThis_, UCHAR *pucData_, USHORT usLen_);	//!< Driver stream write function type


//---------------------------------------------------------------------------
typedef enum
{
	DRIVER_RX_BYTE = 0,
	DRIVER_TX_BYTE,
	DRIVER_TX_STRING,
	DRIVER_EVENTS
} DRIVER_EVENT;

//---------------------------------------------------------------------------
typedef enum
{
	DRIVER_UNINIT = 0,	
	DRIVER_CLOSE,
	DRIVER_IDLE,
	DRIVER_IO
} DRIVER_STATE;

//---------------------------------------------------------------------------
typedef enum
{
	DRIVER_OK = 0,
	DRIVER_BUSY,
	DRIVER_ERR,	
} DRIVER_RETURN;

//---------------------------------------------------------------------------
typedef enum
{
	DRIVER_TYPE_UART = 0,
	DRIVER_TYPE_TIMER,
	DRIVER_TYPE_ATOD,
	DRIVER_TYPE_I2C,
	DRIVER_TYPE_SPI,
	DRIVER_TYPE_KEYPAD,
	DRIVER_TYPE_BUTTONS,
	DRIVER_TYPE_LEDS,
	DRIVER_TYPE_HD44780,
	DRIVER_TYPE_JOY_PSX_IN,
	DRIVER_TYPE_JOY_NES_IN,
	DRIVER_TYPE_JOY_SNES_IN,
	DRIVER_TYPE_JOY_ATARI2600_IN,
	DRIVER_TYPE_JOY_ARCADE_IN,
	DRIVER_TYPE_JOY_ATARI7800_IN,
	DRIVER_TYPE_JOY_TG16_IN,
	DRIVER_TYPE_JOY_RAW_IN,
	DRIVER_TYPE_JOY_GENESIS_IN,
	DRIVER_TYPE_UOLED,
	DRIVER_TYPE_COUNT
} DRIVER_TYPE;
//-----------------------------------------------------------------------------
// Driver "struct", really a virtual base class since we have member function
//-----------------------------------------------------------------------------
typedef struct
{
	//--[Driver info]--------------------------------------------------------
	UCHAR *szName;						//!< Name for the driver (optional)
	DRIVER_STATE		eState;			//!< Driver state (i.e. active/uninit/open)
	DRIVER_TYPE			eType;			//!< Driver type info (used for specialization)
	//--[Driver functions]---------------------------------------------------
	DRIVER_CONSTRUCTOR	pfConstructor;	//!< Pointer to constructor function
	DRIVER_START		pfDriverStart;	//!< Pointer to driver start function
	DRIVER_STOP			pfDriverStop;	//!< Pointer to driver stop function
	DRIVER_CONTROL		pfControl;		//!< Pointer to driver event function
	DRIVER_READ			pfDriverRead;	//!< Pointer to driver "read" function
	DRIVER_WRITE		pfDriverWrite;	//!< Pointer to driver "write" function

} DRIVER_STRUCT;

//---------------------------------------------------------------------------
void Driver_Init(DRIVER_STRUCT *pstDriver_);
void Driver_Start(DRIVER_STRUCT *pstDriver_);
void Driver_Stop(DRIVER_STRUCT *pstDriver_);
USHORT Driver_Read(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_);
USHORT Driver_Write(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_);

DRIVER_RETURN Driver_Control(DRIVER_STRUCT *pstDriver_, USHORT usID_, void *pvData_);

void Driver_SetControlFunc(DRIVER_STRUCT *pstDriver_, void *pfDriverControl_);
void Driver_RemoveControlFunc(DRIVER_STRUCT *pstDriver_);
void Driver_AddControlFunc(DRIVER_STRUCT *pstDriver_, void *pfDriverControl_);

//---------------------------------------------------------------------------

#endif //__DRIVER_H_
