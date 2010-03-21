//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvArcadejoy_in.c
//
// Description:
//	Input driver for Arcade joysticks.  
//--------------------------------------------------------------------------- 

#include "types.h"
#include "driver.h"
#include "drvArcadejoy_in.h"

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
static void Arcade_i_Read(ARCADE_IN_STRUCT *pstJoy_);

//---------------------------------------------------------------------------
void Arcade_i_Init(DRIVER_STRUCT *pstThis_)
{
	ARCADE_IN_STRUCT *pstJoy = (ARCADE_IN_STRUCT*)pstThis_;
	
	// Set the joypad port bits to inputs.

	ARCADE_UP_DIR		&= ~ARCADE_UP_BIT;
	ARCADE_DOWN_DIR		&= ~ARCADE_DOWN_BIT;
	ARCADE_LEFT_DIR		&= ~ARCADE_LEFT_BIT;
	ARCADE_RIGHT_DIR	&= ~ARCADE_RIGHT_BIT;
	ARCADE_BUTTON1_DIR	&= ~ARCADE_BUTTON1_BIT;

	ARCADE_UP_PORT		&= ~ARCADE_UP_BIT;
	ARCADE_DOWN_PORT	&= ~ARCADE_DOWN_BIT;
	ARCADE_LEFT_PORT	&= ~ARCADE_LEFT_BIT;
	ARCADE_RIGHT_PORT	&= ~ARCADE_RIGHT_BIT;
	ARCADE_BUTTON1_PORT	&= ~ARCADE_BUTTON1_BIT;
	
	// Clear the data in the joystick report struct
	Joystick_Clear(&pstJoy->stReport);
}
//---------------------------------------------------------------------------
void Arcade_i_Start(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to start the driver...
}
//---------------------------------------------------------------------------
void Arcade_i_Stop(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to stop the driver...
}
//---------------------------------------------------------------------------
void Arcade_i_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_)
{	
	// Cast the driver to the specified type
	ARCADE_IN_STRUCT *pstJoy = (ARCADE_IN_STRUCT*)pstThis_;
	
	// Switch I/O control operation by ID
	switch(usID_)
	{
		// Read the joystick buttons
		case JOY_IN_READ:
		{
			// Read the buttons...
			Arcade_i_Read(pstJoy);
			
			// Copy the joystick data out
			Joystick_Copy(pvData_, &pstJoy->stReport);
		}
			break;
			
		// Copy the value of the buttons from another type
		case JOY_IN_COPY:
		{
			// Check to make sure the output ptr isn't null first...
			if (pvData_ == NULL)
			{
				break;
			}
			
			// Copy the joystick data out
			Joystick_Copy(pvData_, &pstJoy->stReport);
		}
			break;
		default:
			break;
	}
}

//---------------------------------------------------------------------------
static void Arcade_i_Read(ARCADE_IN_STRUCT *pstJoy_)
{
	USHORT usDebounceCount;
	USHORT usTimeoutCount;
	BOOL bButtonState;
	unsigned int uiButtonVal;
		
	pstJoy_->stReport.JOY_DIGITAL.ulRawData = 0;

	if ((ARCADE_UP_IN & ARCADE_UP_BIT) == 0)
	{
		pstJoy_->stReport.JOY_DIGITAL.JOY_ARCADE.bUp = 1;
	}

	if ((ARCADE_DOWN_IN & ARCADE_DOWN_BIT) == 0)
	{
		pstJoy_->stReport.JOY_DIGITAL.JOY_ARCADE.bDown = 1;
	}

	if ((ARCADE_LEFT_IN & ARCADE_LEFT_BIT) == 0)
	{
		pstJoy_->stReport.JOY_DIGITAL.JOY_ARCADE.bLeft = 1;
	}

	if ((ARCADE_RIGHT_IN & ARCADE_RIGHT_BIT) == 0)
	{
		pstJoy_->stReport.JOY_DIGITAL.JOY_ARCADE.bRight = 1;
	}

	if ((ARCADE_BUTTON1_IN & ARCADE_BUTTON1_BIT) == 0)
	{
		pstJoy_->stReport.JOY_DIGITAL.JOY_ARCADE.bButton1 = 1;
	}
}
