//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file driver.h

	Description:
		Implements a virtual base class for any driver type, and generic
		accessor functions for controlling a driver.
*/
//---------------------------------------------------------------------------
#ifndef __DRIVER_H_
#define __DRIVER_H_

#include "types.h"
#include "task.h"
#include "mutex.h"

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
// Driver base class definition
//-----------------------------------------------------------------------------

class FunkOS_Driver
{
public:
	FunkOS_Driver();                                
	
	BOOL Start(void);   //!< These are the public interface to the device driver handler functions
	BOOL Stop(void);
	
	void SetProtection(BOOL bEnable_);
	USHORT Control(USHORT usID_, void* pvData_);
	USHORT Read(UCHAR *pucData_, USHORT usLen_);
	USHORT Write(UCHAR *pucData_, USHORT usLen_);
	
private:
	virtual BOOL DeviceInit(void){return FALSE;}    //!< These are the device driver handler functions which are implemented by the inheriting class
	
	virtual BOOL DeviceStart(void){return FALSE;}
	virtual BOOL DeviceStop(void){return FALSE;}
	
	virtual USHORT DeviceControl(USHORT usID_, void* pvData_){return 0;}
	virtual USHORT DeviceRead(UCHAR *pucData_, USHORT usLen_){return 0;}
	virtual USHORT DeviceWrite(UCHAR *pucData_, USHORT usLen_){return 0;}
	
	UCHAR *m_szName;					//!< Name for the driver (optional)
	DRIVER_STATE		m_eState;		//!< Driver state (i.e. active/uninit/open)
	DRIVER_TYPE			m_eType;		//!< Driver type info (used for specialization)

#if KERNEL_USE_MUTEX	
	//--[Resource protection]------------------------------------------------
	FunkOS_Mutex		m_clMutex;		//!< Resource protection mutex
	BOOL				m_bUseMutex;	//!< Whether or not resource protection is enabled;
#endif	
};
//---------------------------------------------------------------------------

#endif //__DRIVER_H_
