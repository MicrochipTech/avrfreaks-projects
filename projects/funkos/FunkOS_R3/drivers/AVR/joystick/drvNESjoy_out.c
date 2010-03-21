//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvNESjoy_out.c
//
// Description:
//	Driver implementation for Nintendo NES joystick output.
//--------------------------------------------------------------------------- 
#include "types.h"
#include "driver.h"
#include "joystick.h"
#include "drvNESjoy_out.h"

//---------------------------------------------------------------------------
static UCHAR ucOutBuffer;
static UCHAR ucOut;

//---------------------------------------------------------------------------
void NESo_Init(DRIVER_STRUCT *pstThis_)
{
	ucOutBuffer = 0xFF;
	ucOut = 0xFF;
}
//---------------------------------------------------------------------------
void NESo_Start(DRIVER_STRUCT *pstThis_)
{	
	PCICR &= ~PCICR;				// Disable interrupts	
	PCMSK2 |= NES_O_LATCH_BIT;		// Latch is an interrupt source
	PCMSK2 |= NES_O_CLOCK_BIT;		// Clock is an interrupt source	
	
	NES_O_DATA_PORT &= ~NES_O_DATA_BIT;	// Set the data line low.
	
	PCIFR |= (1 << PCIF2);			// Clear any pending interrupts on this port
	PCICR |= (1 << PCIE2);			// Enable interrupts on this port
}
//---------------------------------------------------------------------------
void NESo_Stop(DRIVER_STRUCT *pstThis_)
{
	UCHAR ucICRState;
	
	ucICRState = PCICR;				// Disable interrupts while changing the mask.
	PCICR &= ~(1 << PCIE2);
	
	PCMSK2 &= ~NES_O_LATCH_BIT;		// Latch is an interrupt source
	PCMSK2 &= ~NES_O_CLOCK_BIT;		// Clock is an interrupt source
	
	PCICR = ucICRState;				// Restore the interrupt mask.
}
//---------------------------------------------------------------------------
void NESo_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_)
{	
	// Cast the driver to the specified type
	NES_OUT_STRUCT *pstJoy = (NES_OUT_STRUCT*)pstThis_;
	JOYSTICK_REPORT_STRUCT *pstReport = (JOYSTICK_REPORT_STRUCT *)pvData_;
	
	// Switch I/O control operation by ID
	switch(usID_)
	{
		// Copy the source report into the joystick buffer.
		case JOY_OUT_COPY:
		{
			Joystick_Copy((&pstJoy->stReport),(pvData_));
		}
			break;
		case JOY_OUT_WRITE:
		{
			UCHAR ucTemp = 0xFF;
			
			// Copy the buttons from the input report.
			// A,B,Sel,Start,Up,Down,Left,Right...
			if ((pstReport_->JOY_DIGITAL.JOY_NES.bA) != 0)
			{
				ucTemp &= ~0x80;
			}
			if ((pstReport_->JOY_DIGITAL.JOY_NES.bB) != 0)
			{
				ucTemp &= ~0x40;
			}
			if ((pstReport_->JOY_DIGITAL.JOY_NES.bSelect) != 0)
			{
				ucTemp &= ~0x20;
			}
			if ((pstReport_->JOY_DIGITAL.JOY_NES.bStart) != 0)
			{
				ucTemp &= ~0x10;
			}
			if ((pstReport_->JOY_DIGITAL.JOY_NES.bUp) != 0)
			{
				ucTemp &= ~0x08;
			}
			if ((pstReport_->JOY_DIGITAL.JOY_NES.bDown) != 0)
			{
				ucTemp &= ~0x04;
			}
			if ((pstReport_->JOY_DIGITAL.JOY_NES.bLeft) != 0)
			{
				ucTemp &= ~0x02;
			}
			if ((pstReport_->JOY_DIGITAL.JOY_NES.bRight) != 0)
			{
				ucTemp &= ~0x01;
			}
						
			// Write the contents of the joystick - under interrupts disabled...
			CS_ENTER();
			ucOutBuffer = ucTemp;
			CS_EXIT();			
		}	
			break;
		default:
			break;
	}
}
//---------------------------------------------------------------------------
// Use pin-change interrupts (PCI..)
ISR(PCINT2_vect)
{
	BOOL bTrigger = FALSE;
	
	// If the latch is high...
	if ((NES_O_LATCH_IN & NES_O_LATCH_BIT) != 0)
	{		
		// Copy in the data from the joystick report...
		ucOut = ucOutBuffer;
		
		// Level out the next bit.
		bTrigger = TRUE;
	}
	// Clock out the next bit from the buffer on the next falling edge...
	if ((NES_O_CLOCK_IN & NES_O_CLOCK_BIT) != 0)
	{
		// Level out the next bit.
		bTrigger = TRUE;
	}
	
	if (bTrigger == TRUE)
	{
		// Bash the data out.
		if (ucOut & 0x80)
		{
			NES_O_DATA_PORT |= NES_O_DATA_BIT;
		}
		else
		{
			NES_O_DATA_PORT &= ~NES_O_DATA_BIT;
		}
		// Shift out the next bit.
		ucOut <<= 1;
	}
}