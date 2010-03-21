//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
 File:  Driver.cpp

 Description:
	This module implements the driver-level abstraction for FunkOS.  
 All managed driver access is handled through this module.

 Usage:
	Drivers are initialized with Driver_Init, started using
 Driver_Start, and stopped using Driver_Stop.  These functions abstract the
 calls to the Init/Start/Stop functions specified in the DRIVER_STRUCT 
 pointer corresponding to the driver.  When a control event is triggered by 
 an interrupt or by application code, the event is passed through the event 
 handler in the driver for processing.  In this way, all drivers are
 accessed consistently.
*/
//---------------------------------------------------------------------------
#include "types.h"
#include "driver.h"
#include "taskport.h"
#include "mutex.h"
#include "kernelcfg.h"

//---------------------------------------------------------------------------
/*!
	Calls the driver's initialization function and sets the	driver state.
    \fn FunkOS_Driver::FunkOS_Driver()
*/
//---------------------------------------------------------------------------
FunkOS_Driver::FunkOS_Driver()
{
	DeviceInit();
	m_eState = DRIVER_CLOSE;
#if KERNEL_USE_MUTEX
	m_clMutex.Init();
#endif
}
//---------------------------------------------------------------------------
/*!
	Calls the driver's start function and sets the driver to the active state
	\fn BOOL FunkOS_Driver::Start(void)
    \return TRUE on success, FALSE on failure
*/
BOOL FunkOS_Driver::Start(void)
{	
	BOOL bReturn = FALSE;
	if (m_eState == DRIVER_CLOSE)
	{
		bReturn = DeviceStart();
		m_eState = DRIVER_IDLE;
#if KERNEL_USE_MUTEX		
		m_bUseMutex = TRUE;
#endif		
	}
	return bReturn;
}

//---------------------------------------------------------------------------
/*!
	Enable or disable the use of resource protection for this driver
	\fn BOOL FunkOS_Driver::SetProtection(BOOL bEnable_)
    \param bEnable_ - TRUE to enable, FALSE to disable
*/
void FunkOS_Driver::SetProtection(BOOL bEnable_)
{
	#if KERNEL_USE_MUTEX
		m_bUseMutex = bEnable_;
	#endif
}

//---------------------------------------------------------------------------
/*!
	Stop the driver and set to the idle state
	\fn BOOL FunkOS_Driver::Stop(void)
    \return TRUE on success, FALSE on failure
*/
BOOL FunkOS_Driver::Stop(void)
{	
	BOOL bReturn = DeviceStop();
	m_eState = DRIVER_CLOSE;
	return bReturn;
}

//---------------------------------------------------------------------------
/*!
	Calls the driver event handler from a given task with event type specified in 
	the event ID field, and data passed in through the void * parameter field.
    \fn USHORT FunkOS_Driver::Control(USHORT usID_, void *pvData_)
	\param usID_ - Event ID
	\param pvData_ - Pointer to the event data.
*/
USHORT FunkOS_Driver::Control(USHORT usID_, void *pvData_)
{
	USHORT usReturn;
	
	// Run the control function
	if (m_eState > DRIVER_CLOSE)
	{
		// Explicit resource locking with priority inversion protection 
		// When mutex module included
#if KERNEL_USE_MUTEX
		{
			BOOL bCacheMutex = m_bUseMutex;
			if (bCacheMutex)
			{
				m_clMutex.Claim(TIME_FOREVER);
			}
#endif
			usReturn = DeviceControl(usID_, pvData_);
			
#if KERNEL_USE_MUTEX
			if (bCacheMutex)
			{
				// Release the resource 
				m_clMutex.Release();
			}
		}
#endif
		return usReturn;
	}	
	return (UCHAR)DRIVER_OK;

}
//---------------------------------------------------------------------------
/*!
	Use the driver's dedicated read function to read a specified amount of
	data from the driver's stream.
	
	\fn USHORT FunkOS_Driver::Read(UCHAR *pucData_, USHORT usLen_)
	\param pucData_ - pointer to the input data stream
	\param usLen_ - number of bytes to read from the stream.
	\return USHORT - the number of bytes actually read from the stream
*/
USHORT FunkOS_Driver::Read(UCHAR *pucData_, USHORT usLen_)
{
	USHORT usReturn = 0;
	if (m_eState > DRIVER_CLOSE)
	{
#if KERNEL_USE_MUTEX
		{
			BOOL bCacheMutex = m_bUseMutex;
			if (bCacheMutex)
			{
				m_clMutex.Claim(TIME_FOREVER);
			}
#endif
			usReturn = DeviceRead(pucData_, usLen_);
#if KERNEL_USE_MUTEX
			if (bCacheMutex)
			{
				// Release the resource 
				m_clMutex.Release();
			}
		}
#endif
	}

	return usReturn;
}

//---------------------------------------------------------------------------
/*!
	Use the driver's dedicated read function to write a specified amount of
	data to the driver's stream.
	
	\fn USHORT FunkOS_Driver::Write(UCHAR *pucData_, USHORT usLen_)
	\param pucData_ - pointer to data to write
	\param usLen_ - number of bytes to write to the stream.
	\return USHORT - the number of bytes actually written to the stream
*/
USHORT FunkOS_Driver::Write(UCHAR *pucData_, USHORT usLen_)
{
	USHORT usReturn = 0;
	if (m_eState > DRIVER_CLOSE)
	{
#if KERNEL_USE_MUTEX
		{
			BOOL bCacheMutex = m_bUseMutex;
			if (bCacheMutex)
			{
				m_clMutex.Claim(TIME_FOREVER);
			}
#endif
			usReturn = DeviceWrite(pucData_, usLen_);
#if KERNEL_USE_MUTEX
			if (bCacheMutex)
			{
				// Release the resource 
				m_clMutex.Release();
			}
		}
#endif
	}
	return usReturn;
}
