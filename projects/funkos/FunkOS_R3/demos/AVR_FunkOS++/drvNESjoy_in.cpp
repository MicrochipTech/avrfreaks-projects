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
BOOL NESi::DeviceInit(void)
{
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
	Joystick_Clear(&m_stReport);

	return TRUE;
}
//---------------------------------------------------------------------------
BOOL NESi::DeviceStart(void)
{
	// Nothing to do to start the driver...
	return TRUE;
}
//---------------------------------------------------------------------------
BOOL NESi::DeviceStop(void)
{
	// Nothing to do to stop the driver...
	return TRUE;
}
//---------------------------------------------------------------------------
USHORT NESi::DeviceControl(USHORT usID_, void *pvData_)
{		
	// Switch I/O control operation by ID
	switch(usID_)
	{
		// Read the joystick buttons
		case JOY_IN_READ:
		{
			// Read the buttons...
			Read();
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
			Joystick_Copy((JOYSTICK_REPORT_STRUCT*)pvData_, (&m_stReport));
		}
			break;
		default:
			break;
	}
	return 0;
}

//---------------------------------------------------------------------------
void NESi::Read(void)
{
	UCHAR ucButtonIdx;
	UCHAR ucButton;
	
	// Latch the controller data into the shift register
	NES_LATCH_PORT |= NES_LATCH_BIT;
	NES_CLOCK_PORT |= NES_CLOCK_BIT;
	Joystick_Delay(12);
	NES_LATCH_PORT &= ~NES_LATCH_BIT;
	Joystick_Delay(12);
	
	// Read each button in loop form...
	for (ucButtonIdx = 0; ucButtonIdx < (UCHAR)NES_BUTTON_COUNT; ucButtonIdx++)
	{
		// Read the data bit from the shift register - active low.
		ucButton = ((NES_DATA_IN & NES_DATA_BIT) == 0);
		
		// Clock the next bit into the shift register
		NES_CLOCK_PORT &= ~NES_CLOCK_BIT;
		Joystick_Delay(12);
		NES_CLOCK_PORT |= NES_CLOCK_BIT;
		Joystick_Delay(12);
		
		// Read the button data in...
		switch ((NES_BUTTON)ucButtonIdx)
		{
			case NES_A:
			{
				m_stReport.JOY_DIGITAL.JOY_NES.bA = ucButton;
			}
				break;
			case NES_B:
			{
				m_stReport.JOY_DIGITAL.JOY_NES.bB = ucButton;
			}
				break;
			case NES_SELECT:
			{
				m_stReport.JOY_DIGITAL.JOY_NES.bSelect = ucButton;
			}
				break;
			case NES_START:
			{
				m_stReport.JOY_DIGITAL.JOY_NES.bStart = ucButton;
			}
				break;
			case NES_UP:
			{
				m_stReport.JOY_DIGITAL.JOY_NES.bUp = ucButton;
			}
				break;
			case NES_DOWN:
			{
				m_stReport.JOY_DIGITAL.JOY_NES.bDown = ucButton;
			}
				break;
			case NES_LEFT:
			{
				m_stReport.JOY_DIGITAL.JOY_NES.bLeft = ucButton;
			}
				break;
			case NES_RIGHT:
			{
				m_stReport.JOY_DIGITAL.JOY_NES.bRight = ucButton;
			}
				break;
			default:
				break;
		}
	}
}
