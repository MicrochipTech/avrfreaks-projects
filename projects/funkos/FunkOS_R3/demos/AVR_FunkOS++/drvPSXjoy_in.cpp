//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, All Rights Reserved
//--------------------------------------------------------------------------- 
// File:  drvPSXjoy_in.c
//
// Description:
//	Input driver for PSX controllers
//
//--------------------------------------------------------------------------- 

#include "types.h"
#include "driver.h"
#include "drvPSXjoy_in.h"
#include "joystick.h"

//---------------------------------------------------------------------------
static UCHAR PSXi_CmdByte(PSX_IN_STRUCT *pstThis_, UCHAR ucCmd_, BOOL bWaitAck_);
static void PSXi_Read(PSX_IN_STRUCT *pstJoy_);
static void PSXi_Decode(PSX_IN_STRUCT *pstJoy_, UCHAR ucJoyMode_, UCHAR *pucData_);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// PSX joypad input driver.
//---------------------------------------------------------------------------
void PSXi::DeviceInit(void)
{
	PSX_DDR  &= ~PSX_DATA_BIT;	// We receive data (act like PSX)	
	PSX_PORT |= PSX_DATA_BIT;	// Set internal pullup

	PSX_PORT |= PSX_CLK_BIT;	// Clock output (we control?)
	PSX_DDR  |= PSX_CLK_BIT;	// Clock output high
	
	PSX_DDR  |= PSX_CMD_BIT;	// We send commands (act like PSX)
	PSX_PORT |= PSX_CMD_BIT;	// Hold line high
		
	PSX_DDR  |= PSX_ATT_BIT;	// Attention bit (master ctrld)
	PSX_PORT |= PSX_ATT_BIT;	// Attention bit high until CS

	PSX_PORT |= PSX_ACK_BIT;	// Set internal pullup	
	PSX_DDR  &= ~PSX_ACK_BIT;	// Ack controlled by ctrlr	
}
//---------------------------------------------------------------------------
BOOL PSXi::DeviceStart(void)
{
	// Stub
	return TRUE;
}
//---------------------------------------------------------------------------
void PSXi::DeviceStop(void)
{
	// Stub
	return TRUE;
}
//---------------------------------------------------------------------------
USHORT PSXi::DeviceControl(USHORT usID_, void *pvData_)
{
	switch(usID_)
	{
		// Main event - poll joypad.
		case JOY_IN_READ:
		{
			PSXi_Read(pstJoy);
			Joystick_Copy(((JOYSTICK_REPORT_STRUCT*)pvData_), (&pstJoy->stReport));	
		}
			break;
		case JOY_IN_COPY:
		{
			// Check to make sure the output ptr isn't null first...
			if (pvData_ == NULL)
			{
				break;
			}
			
			// Copy the joystick data out
			Joystick_Copy(((JOYSTICK_REPORT_STRUCT*)pvData_), (&pstJoy->stReport));	
		}
			break;
		default:
			break;
	}
	return 0;
}
//---------------------------------------------------------------------------
static void PSXi_Read(PSX_IN_STRUCT *pstJoy_)
{
	UCHAR ucBytesLeft;
	UCHAR ucJoyMode;
	UCHAR ucJoyIndex;
	
	UCHAR aucData[MAX_PSX_DATA_BYTES];
	
	// Signal attention - time to read out data
	PSX_PORT |= PSX_CMD_BIT;
	PSX_PORT |= PSX_CLK_BIT;
	PSX_PORT &= ~PSX_ATT_BIT;	
	
	Joystick_Delay(2);	
	
	// Send request for data
	PSXi_CmdByte(pstJoy_, PSX_CMD_START, TRUE); 
		
	// Get the joypad mode
	ucJoyMode = PSXi_CmdByte(pstJoy_, PSX_CMD_POLL, FALSE);
	pstJoy_->aucRaw[0] = ucJoyMode;

	// Idle - Joypad ready to talk...
	pstJoy_->aucRaw[1] = PSXi_CmdByte(pstJoy_, PSX_CMD_IDLE, FALSE);

	// # bytes remaining based on Joypad mode:
	switch (ucJoyMode)
	{
		case PSX_TYPE_DIGITAL:	// Digital Mode
		{
			ucBytesLeft = PSX_SIZE_DIGITAL;
		}
			break;
		case PSX_TYPE_ANALOG_GREEN:	// Analog Green Mode
		{
			ucBytesLeft = PSX_SIZE_ANALOG;
		}
			break;
		case PSX_TYPE_ANALOG_RED:	// Analog Red Mode
		{
			ucBytesLeft = PSX_SIZE_ANALOG;
		}
			break;
		default:
		{
			// Disconnect.
			ucBytesLeft = 0;
			pstJoy_->bConnected = FALSE;
		}
	}
	
	// Array index for the raw PSX joypad input
	ucJoyIndex = 0;
	
	// Read the rest of the bytes.
	while (ucBytesLeft && pstJoy_->bConnected)
	{
		if (ucBytesLeft != 1)
		{
			aucData[ucJoyIndex] = PSXi_CmdByte(pstJoy_, PSX_CMD_IDLE, FALSE);
			pstJoy_->aucRaw[3] = aucData[ucJoyIndex];
		}
		else
		{
			aucData[ucJoyIndex] = PSXi_CmdByte(pstJoy_, PSX_CMD_IDLE, TRUE);
			pstJoy_->aucRaw[4] = aucData[ucJoyIndex];
		}
		
		// Update indexes.
		ucBytesLeft--;
		ucJoyIndex++;
	}
	
	// Set attention high.
	PSX_PORT |= PSX_ATT_BIT;
	if (pstJoy_->bConnected)
	{
		// Decode the joypad data if it's connected.
		PSXi_Decode(pstJoy_, ucJoyMode, aucData);
	}
}

//---------------------------------------------------------------------------
// returns data byte.
//---------------------------------------------------------------------------
static UCHAR PSXi_CmdByte(PSX_IN_STRUCT *pstJoy_, UCHAR ucCmd_, BOOL bWaitAck_)
{
	UCHAR ucReturnVal = 0;
	UCHAR ucBitMask = 0x01;
	UCHAR ucSpin = 0;
	
	// Wait for ack to go high...
	while ((PSX_PORT & PSX_ACK_BIT) == 0)
	{
		ucSpin++;
		Joystick_Delay(1);
		if (ucSpin > POLL_SPIN_COUNT)
		{
			pstJoy_->bConnected = FALSE;
			return 0;
		}		
	}

	pstJoy_->bConnected = TRUE;

	Joystick_Delay(2);
	
	while (ucBitMask)
	{

		if ((ucCmd_ & ucBitMask) != 0)
		{
			// Clock command out.
			PSX_PORT |= PSX_CMD_BIT;
		}
		else
		{
			PSX_PORT &= ~PSX_CMD_BIT;
		}

		// data clocked in on falling edge
		PSX_PORT &= ~PSX_CLK_BIT;	
		Joystick_Delay(5);

		// Read the input data pin.
		if ((PSX_PIN & PSX_DATA_BIT) != 0)
		{
			ucReturnVal	|= ucBitMask;
		}
		
		// data high phase
		PSX_PORT |= PSX_CLK_BIT;	
		Joystick_Delay(5);
		
		// Next bit
		ucBitMask <<= 1;
	}

	// If this current byte requires acknowledgement
	if (bWaitAck_)
	{
		ULONG ulCounter = 0;
		
		// Retry loop for waiting on ACK
		while(ulCounter < POLL_SPIN_COUNT)
		{
			// Poll for ACK...
			if ((PSX_PIN & PSX_ACK_BIT) == 0)
			{				
				break;
			}			
			ulCounter++;
			Joystick_Delay(1);
		}	
	}

	return ucReturnVal;
}
//---------------------------------------------------------------------------
static void PSXi_Decode(PSX_IN_STRUCT *pstJoy_, UCHAR ucJoyMode_, UCHAR *pucData_)
{
	// Decode the message data into the appropriate joypad report format
	switch (ucJoyMode_)
	{
		case PSX_TYPE_DIGITAL:	// Digital Mode
		{
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bSelect = ((pucData_[0] & 0x01) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bL3 = ((pucData_[0] & 0x02) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bR3 = ((pucData_[0] & 0x04) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bStart = 	((pucData_[0] & 0x08) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bUp = ((pucData_[0] & 0x10) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bRight = 	((pucData_[0] & 0x20) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bDown = ((pucData_[0] & 0x40) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bLeft = 	((pucData_[0] & 0x80) == 0);
			
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bL1 = ((pucData_[1] & 0x01) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bR1 = ((pucData_[1] & 0x02) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bL2 = ((pucData_[1] & 0x04) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bR2 = ((pucData_[1] & 0x08) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bTriangle = ((pucData_[1] & 0x10) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bO = 	((pucData_[1] & 0x20) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bX = 	((pucData_[1] & 0x40) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bSquare = ((pucData_[1] & 0x80) == 0);			
		}
			break;

		case PSX_TYPE_ANALOG_GREEN:	// Analog Green Mode
		{
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bStart = 
				((pucData_[0] & 0x08) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bUp = 
				((pucData_[0] & 0x10) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bRight = 
				((pucData_[0] & 0x20) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bDown = 
				((pucData_[0] & 0x40) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bLeft = 
				((pucData_[0] & 0x80) == 0);
			
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bL2 = 
				((pucData_[1] & 0x01) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bL1 = 
				((pucData_[1] & 0x02) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bSquare = 
				((pucData_[1] & 0x04) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bTriangle = 
				((pucData_[1] & 0x08) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bR1 = 
				((pucData_[1] & 0x10) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bO = 
				((pucData_[1] & 0x20) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bX = 
				((pucData_[1] & 0x40) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bR2 = 
				((pucData_[1] & 0x80) == 0);

			pstJoy_->stReport.usXAxis2 = pucData_[2];
			pstJoy_->stReport.usYAxis2 = pucData_[3];
			pstJoy_->stReport.usXAxis1 = pucData_[4];
			pstJoy_->stReport.usYAxis1 = pucData_[5];
		}
			break;
			
		case PSX_TYPE_ANALOG_RED:	// Analog Red Mode
		{
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bSelect = 
				((pucData_[0] & 0x01) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bL3 = 
				((pucData_[0] & 0x02) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bR3 = 
				((pucData_[0] & 0x04) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bStart = 
				((pucData_[0] & 0x08) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bUp = 
				((pucData_[0] & 0x10) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bRight = 
				((pucData_[0] & 0x20) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bDown = 
				((pucData_[0] & 0x40) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bLeft = 
				((pucData_[0] & 0x80) == 0);
			
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bL1 = 
				((pucData_[1] & 0x01) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bR1 = 
				((pucData_[1] & 0x02) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bL2 = 
				((pucData_[1] & 0x04) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bR2 = 
				((pucData_[1] & 0x08) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bTriangle = 
				((pucData_[1] & 0x10) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bO = 
				((pucData_[1] & 0x20) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bX = 
				((pucData_[1] & 0x40) == 0);
			pstJoy_->stReport.JOY_DIGITAL.JOY_PSX.bSquare = 
				((pucData_[1] & 0x80) == 0);
		
			pstJoy_->stReport.usXAxis2 = pucData_[2];
			pstJoy_->stReport.usYAxis2 = pucData_[3];
			pstJoy_->stReport.usXAxis1 = pucData_[4];
			pstJoy_->stReport.usYAxis1 = pucData_[5];
		}
			break;
		default:
			break;
	}
}

