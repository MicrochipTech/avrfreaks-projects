//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvNESjoy_in.c
//
// Description:
//	Input driver for NES compatible joysticks.  
//--------------------------------------------------------------------------- 

#include "types.h"
#include "driver.h"
#include "drvNESjoy_in.h"
#include "joystick.h"
//---------------------------------------------------------------------------
static void NESi_Read(NES_IN_STRUCT *pstJoy_);

//---------------------------------------------------------------------------
void NESi_Init(DRIVER_STRUCT *pstThis_)
{
	NES_IN_STRUCT *pstJoy = (NES_IN_STRUCT*)pstThis_;
	
	// Set up the direction registers for the joystick pins
	NES_DATA_DIR &= ~NES_DATA_BIT;	// in
	NES_LATCH_DIR |= NES_LATCH_BIT;	// out
	NES_CLOCK_DIR |= NES_CLOCK_BIT;	// out
	
	// Set up pullup on data line
	NES_DATA_PORT |= NES_DATA_BIT;
	
	// Set latch and clock low
	NES_LATCH_PORT &= ~NES_LATCH_BIT;
	NES_CLOCK_PORT &= ~NES_CLOCK_BIT;
	
	// Clear the data in the joystick report struct
	Joystick_Clear(&pstJoy->stReport);
}
//---------------------------------------------------------------------------
void NESi_Start(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to start the driver...
}
//---------------------------------------------------------------------------
void NESi_Stop(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to stop the driver...
}
//---------------------------------------------------------------------------
void NESi_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_)
{	
	// Cast the driver to the specified type
	NES_IN_STRUCT *pstJoy = (NES_IN_STRUCT*)pstThis_;
	
	// Switch I/O control operation by ID
	switch(usID_)
	{
		// Read the joystick buttons
		case JOY_IN_READ:
		{
			// Read the buttons...
			NESi_Read(pstJoy);
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
static void NESi_Read(NES_IN_STRUCT *pstJoy_)
{
	NES_BUTTON eButton;
	UCHAR ucButton;
	
	// Latch the controller data into the shift register
	NES_LATCH_PORT |= NES_LATCH_BIT;
	NES_CLOCK_PORT |= NES_CLOCK_BIT;
	Joystick_Delay(12);
	NES_LATCH_PORT &= ~NES_LATCH_BIT;
	Joystick_Delay(12);
	
	// Read each button in loop form...
	for (eButton = (NES_BUTTON)0; eButton < NES_BUTTON_COUNT; eButton++)
	{
		// Read the data bit from the shift register - active low.
		ucButton = ((NES_DATA_IN & NES_DATA_BIT) == 0);
		
		// Clock the next bit into the shift register
		NES_CLOCK_PORT &= ~NES_CLOCK_BIT;
		Joystick_Delay(12);
		NES_CLOCK_PORT |= NES_CLOCK_BIT;
		Joystick_Delay(12);
		
		// Read the button data in...
		switch (eButton)
		{
			case NES_A:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_NES.bA = ucButton;
			}
				break;
			case NES_B:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_NES.bB = ucButton;
			}
				break;
			case NES_SELECT:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_NES.bSelect = ucButton;
			}
				break;
			case NES_START:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_NES.bStart = ucButton;
			}
				break;
			case NES_UP:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_NES.bUp = ucButton;
			}
				break;
			case NES_DOWN:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_NES.bDown = ucButton;
			}
				break;
			case NES_LEFT:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_NES.bLeft = ucButton;
			}
				break;
			case NES_RIGHT:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_NES.bRight = ucButton;
			}
				break;
			default:
				break;
		}
	}
}
