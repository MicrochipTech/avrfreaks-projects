//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
 File:  Driver.c

 Description:
	This module implements the driver-level abstraction for FunkOS.  
 All managed driver access is handled through this module.  This is the 
 version used with the cooperative task scheduler.

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

//---------------------------------------------------------------------------
/*!
	Calls the driver's initialization function and sets the	driver state.
	\fn void Driver_Init(DRIVER_STRUCT *pstDriver_)
	\param pstDriver_ - Pointer to the driver
*/
//---------------------------------------------------------------------------
void Driver_Init(DRIVER_STRUCT *pstDriver_)
{
	if (pstDriver_ != NULL)
	{
		if (pstDriver_->eState == DRIVER_UNINIT)
		{
			pstDriver_->pfConstructor(pstDriver_);			
			pstDriver_->eState = DRIVER_CLOSE;
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Calls the driver's start function and sets the driver to the active state
	\fn void Driver_Start(DRIVER_STRUCT *pstDriver_)
	\param pstDriver_ - pointer to the driver to start
*/
void Driver_Start(DRIVER_STRUCT *pstDriver_)
{
	if (pstDriver_ != NULL)
	{
		if (pstDriver_->eState == DRIVER_CLOSE)
		{
			pstDriver_->pfDriverStart(pstDriver_);
			pstDriver_->eState = DRIVER_IDLE;
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Stop the driver and set to the idle state
	\fn void Driver_Stop(DRIVER_STRUCT *pstDriver_)
	\param pstDriver_ - driver to stop
*/
void Driver_Stop(DRIVER_STRUCT *pstDriver_)
{
	if (pstDriver_ != NULL)
	{
		while (pstDriver_->eState == DRIVER_IO) {}; // wait for IO to complete		
		if (pstDriver_->eState == DRIVER_IDLE)
		{
			pstDriver_->pfDriverStop(pstDriver_);
			pstDriver_->eState = DRIVER_CLOSE;
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Remove the control function for the driver
	\fn void Driver_RemoveControlFunc(DRIVER_STRUCT *pstDriver_)
	\param pstDriver_ - driver to remove the event handler from
*/
void Driver_RemoveControlFunc(DRIVER_STRUCT *pstDriver_)
{
	if (pstDriver_ != NULL)
	{		
		if (pstDriver_->eState >= DRIVER_CLOSE)
		{
			pstDriver_->pfControl = NULL;
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Set the control/event handler for the driver in question.
	\fn void Driver_AddControlFunc(DRIVER_STRUCT *pstDriver_, void *pfDriverControl_)
	\param pstDriver_ - pointer to the driver
	\param pfDriverControl_ - pointer to the control function
*/
void Driver_AddControlFunc(DRIVER_STRUCT *pstDriver_, void *pfDriverControl_)
{
	// only allow the driver to have a reassinged control function when
	// the driver is closed.
	if (pstDriver_ != NULL)
	{
		if (pstDriver_->eState >= DRIVER_CLOSE)
		{
			if (pstDriver_->pfControl == NULL)
			{
				// initialization
				pstDriver_->pfControl = (DRIVER_CONTROL)pfDriverControl_;
			}
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Calls the driver event handler from a given task with event type specified in 
	the event ID field, and data passed in through the void * parameter field.
	\fn DRIVER_RETURN Driver_Control(DRIVER_STRUCT *pstDriver_, USHORT usID_, void *pvData_)
	\param pstDriver_ - Pointer to the driver struct
	\param usID_ - Event ID
	\param pvData_ - Pointer to the event data.
*/
DRIVER_RETURN Driver_Control(DRIVER_STRUCT *pstDriver_,USHORT usID_, void *pvData_)
{

	DRIVER_RETURN eReturn;

	if (pstDriver_ != NULL)
	{
		// Run the control function
		if ((pstDriver_->pfControl != NULL) && (pstDriver_->eState > DRIVER_CLOSE))
		{
			// Explicit resource locking with priority inversion protection 
			// When mutex module included
			eReturn = pstDriver_->pfControl(pstDriver_, usID_, pvData_);
			
			return eReturn;
		}	
		return DRIVER_OK;
	}
	return DRIVER_ERR;
}
//---------------------------------------------------------------------------
/*!
	Use the driver's dedicated read function to read a specified amount of
	data from the driver's stream.
	
	\fn void  Driver_Read(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_)
	\param pstDriver_ - pointer to the task's driver
	\param pucData_ - pointer to the input data stream
	\param usLen_ - number of bytes to read from the stream.
	\return USHORT - the number of bytes actually read from the stream
*/
USHORT Driver_Read(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_)
{
	USHORT usReturn = 0;
	if (pstDriver_ != NULL)
	{		
		if ((pstDriver_->pfDriverRead != NULL) && (pstDriver_->eState > DRIVER_CLOSE))
		{
			usReturn = pstDriver_->pfDriverRead(pstDriver_, pucData_, usLen_);
		}
	}
	return usReturn;
}

//---------------------------------------------------------------------------
/*!
	Use the driver's dedicated read function to write a specified amount of
	data to the driver's stream.
	
	\fn void  Driver_Write(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_)
	\param pstDriver_ - pointer to the task's driver
	\param pucData_ - pointer to data to write
	\param usLen_ - number of bytes to write to the stream.
	\return USHORT - the number of bytes actually written to the stream
*/
USHORT Driver_Write(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_)
{
	USHORT usReturn = 0;
	if (pstDriver_ != NULL)
	{		
		if ((pstDriver_->pfDriverWrite != NULL) && (pstDriver_->eState > DRIVER_CLOSE))
		{
			usReturn = pstDriver_->pfDriverWrite(pstDriver_, pucData_, usLen_);
		}
	}
	return usReturn;
}
