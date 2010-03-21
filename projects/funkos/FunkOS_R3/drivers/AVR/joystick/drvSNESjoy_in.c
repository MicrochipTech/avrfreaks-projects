//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvSNESjoy_in.c
//
// Description:
//	Input driver for SNES compatible joysticks.  
//--------------------------------------------------------------------------- 

#include "types.h"
#include "driver.h"
#include "drvSNESjoy_in.h"
#include "joystick.h"

//---------------------------------------------------------------------------
static void SNESi_Read(SNES_IN_STRUCT *pstJoy_);

//---------------------------------------------------------------------------
void SNESi_Init(DRIVER_STRUCT *pstThis_)
{
	SNES_IN_STRUCT *pstJoy = (SNES_IN_STRUCT*)pstThis_;
	
	// Set up the direction registers for the joystick pins
	SNES_DATA_DIR &= ~SNES_DATA_BIT;	// in
	SNES_LATCH_DIR |= SNES_LATCH_BIT;	// out
	SNES_CLOCK_DIR |= SNES_CLOCK_BIT;	// out
	
	// Set up pullup on data line
	SNES_DATA_PORT |= SNES_DATA_BIT;
	
	// Set latch and clock low
	SNES_LATCH_PORT &= ~SNES_LATCH_BIT;
	SNES_CLOCK_PORT &= ~SNES_CLOCK_BIT;
	
	// Clear the data in the joystick report struct
	Joystick_Clear(&pstJoy->stReport);
}
//---------------------------------------------------------------------------
void SNESi_Start(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to start the driver...
}
//---------------------------------------------------------------------------
void SNESi_Stop(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to stop the driver...
}
//---------------------------------------------------------------------------
void SNESi_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_)
{	
	// Cast the driver to the specified type
	SNES_IN_STRUCT *pstJoy = (SNES_IN_STRUCT*)pstThis_;
	
	// Switch I/O control operation by ID
	switch(usID_)
	{
		// Read the joystick buttons
		case JOY_IN_READ:
		{
			// Read the buttons...
			SNESi_Read(pstJoy);
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
			Joystick_Copy((pvData_), (&pstJoy->stReport));
		}
			break;
		default:
			break;
	}
}

//---------------------------------------------------------------------------
static void SNESi_Read(SNES_IN_STRUCT *pstJoy_)
{
	SNES_BUTTON eButton;
	UCHAR ucButton;
	
	// Latch the controller data into the shift register
	SNES_LATCH_PORT |= SNES_LATCH_BIT;
	Joystick_Delay(12);
	SNES_LATCH_PORT &= ~SNES_LATCH_BIT;
	Joystick_Delay(12);
	
	// Read each button in loop form...
	for (eButton = (SNES_BUTTON)0; eButton < SNES_BUTTON_COUNT; eButton++)
	{
		ucButton = ((SNES_DATA_IN & SNES_DATA_BIT) == 0);
		
		// Clock the next bit into the shift register
		SNES_CLOCK_PORT &= ~SNES_CLOCK_BIT;
		Joystick_Delay(12);
		SNES_CLOCK_PORT |= SNES_CLOCK_BIT;
		Joystick_Delay(12);
		
		// Read the button data in...
		switch (eButton)
		{
			case SNES_A:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bA = ucButton;
			}
				break;
			case SNES_B:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bB = ucButton;
			}
				break;
			case SNES_X:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bX = ucButton;
			}
				break;
			case SNES_Y:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bY = ucButton;
			}
				break;
			case SNES_L:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bL = ucButton;
			}
				break;
			case SNES_R:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bR = ucButton;
			}
				break;
			case SNES_SELECT:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bSelect = ucButton;
			}
				break;
			case SNES_START:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bStart = ucButton;
			}
				break;
			case SNES_UP:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bUp = ucButton;
			}
				break;
			case SNES_DOWN:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bDown = ucButton;
			}
				break;
			case SNES_LEFT:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bLeft = ucButton;
			}
				break;
			case SNES_RIGHT:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_SNES.bRight = ucButton;
			}
				break;
			default:
				break;
		}
	}
}
