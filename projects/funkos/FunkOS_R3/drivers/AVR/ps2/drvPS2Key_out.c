//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvPS2Key_out.c
//
// Description:
//	Output driver for PS2 Keyboard
//--------------------------------------------------------------------------- 

#include "types.h"
#include "driver.h"
#include "drvPS2Key_out.h"

#define PHASE_TIME			(15)

//---------------------------------------------------------------------------
static BOOL PS2KeyOut_WriteByte(PS2KEY_OUT_STRUCT *pstThis_, UCHAR ucByte_);
static void PS2KeyOut_WriteBit(PS2_OUT_STRUCT *pstKeyboard_, BOOL bBit_);
static void PS2KeyOut_Delay(USHORT usMicroSeconds_);

//---------------------------------------------------------------------------
void PS2KeyOut_Init(DRIVER_STRUCT *pstThis_)
{
	PS2_OUT_STRUCT *pstKeyboard = (PS2_OUT_STRUCT*)pstThis_;
	
	// Set lines as inputs...
	*(pstKeyboard->pucClkDir)	&= ~(pstKeyboard->ucClkBit);
	*(pstKeyboard->pucDataDir)	&= ~(pstKeyboard->ucDataBit);
	
	// Set pullups on inputs
	*(pstKeyboard->pucClkPort)	|= (pstKeyboard->ucClkBit);
	*(pstKeyboard->pucDataPort)	|= (pstKeyboard->ucDataBit);	
}

//---------------------------------------------------------------------------
void PS2KeyOut_Start(PS2KEY_OUT_STRUCT *pstThis_)
{
	// Stub
}

//---------------------------------------------------------------------------
void PS2KeyOut_Stop(PS2KEY_OUT_STRUCT *pstThis_)
{
	// Stub
}

//---------------------------------------------------------------------------
void PS2KeyOut_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_)
{	
	PS2KEY_OUT_STRUCT *pstKeyboard = (PS2KEY_OUT_STRUCT*)pstThis_;
	
	switch(usID_)
	{
		// Switch command IDs - group together by function
		case PS2KEY_OUT_EVENT_WRITE: // Write to host...
		{			
			// Write the byte - what to do if it aborts?
			while (!PS2KeyOut_WriteByte(pstKeyboard, ucByte))
			{
				// Read a command...
				
				// Send a response...
			}
		}
			break;
		case PS2KEY_OUT_EVENT_READ:	
		{
			// Read an event from the host
		}
			break;
		default:
			break;
	}
}

//---------------------------------------------------------------------------
static BOOL PS2KeyOut_WriteByte(PS2KEY_OUT_STRUCT *pstThis_, UCHAR ucByte_)
{
	UCHAR i;
	UCHAR ucShift = 1;
	UCHAR ucParity = 0;
	BOOL bBit;
	BOOL bAbort = TRUE;
	PS2_OUT_BIT_STATE eState;

	for (eState = (PS2_OUT_BIT_STATE)0; eState < PS2_OUT_BIT_COUNT; eState++)
	{
		// If the clock is pulled low, the host is trying to do something...
		if ((*(pstKeyboard_->pucClkIn) & pstKeyboard_->ucClkBit) == 0)
		{
			break;
		}
		
		// Check to see if we're being pre-empted by a host read.
		switch(eState)
		{
			case PS2_OUT_BIT_START:
			{
				// Write a zero as a start bit
				PS2KeyOut_WriteBit(pstThis_, 0);
			}
				break;
			case PS2_OUT_BIT_0:
			case PS2_OUT_BIT_1:
			case PS2_OUT_BIT_2:
			case PS2_OUT_BIT_3:
			case PS2_OUT_BIT_4:
			case PS2_OUT_BIT_5:
			case PS2_OUT_BIT_6:
			case PS2_OUT_BIT_7:
			{
				// Check if the bit is set or cleared
				bBit = ((ucByte_ & ucShift) != 0);
				
				// If the bit is set add to the parity count
				if (bBit)
				{
					ucParity++;
				}
				
				// Write the bit value out the bus
				PS2KeyOut_WriteBit(pstThis_, bBit);
				ucShift <<= 1;
			}
				break;
			case PS2_OUT_BIT_PARITY:
			{				
				// Odd parity bit check
				if (ucParity & 0x01)
				{	
					bBit = FALSE;
				}
				else
				{
					bBit = TRUE;
				}
				
				// Write the parity bit
				PS2KeyOut_WriteBit(pstThis_, bBit);
				
				// We made it without having been pre-empted by a host!!
				bAbort = FALSE;
			}
				break;
			case PS2_OUT_BIT_STOP:
			{
				// Write a 1 as a stop bit
				PS2KeyOut_WriteBit(pstThis_, 1);		
			}
				break;
			case default:
				break;
		}
	}
	
	return bAbort;
}

//---------------------------------------------------------------------------
static void PS2KeyOut_WriteBit(PS2_OUT_STRUCT *pstKeyboard_, BOOL bBit_)
{
	// Clock high..
	*(pstKeyboard_->pucClkDir) 	&= ~pstKeyboard_->ucClkBit;
	*(pstKeyboard_->pucClkPort)	|= pstKeyboard_->ucClkBit;
	
	PS2KeyOut_Delay(PHASE_TIME);
	
	// Modify data half-way through the clock-high period
	if (bBit_)
	{
		// High - input w/pullup enabled
		*(pstKeyboard_->pucDataDir) 	&= ~pstKeyboard_->ucDataBit;
		*(pstKeyboard_->pucDataPort)	|= pstKeyboard_->ucDataBit;
	}
	else
	{
		// Low - output low
		*(pstKeyboard_->pucDataPort)	&= ~pstKeyboard_->ucDataBit;
		*(pstKeyboard_->pucDataDir) 	|= pstKeyboard_->ucDataBit;
	}
	
	// The rest of the high-period
	PS2KeyOut_Delay(PHASE_TIME);
	
	// Data is sampled on the falling edge
	*(pstKeyboard_->pucClkPort)	&= ~pstKeyboard_->ucClkBit;
	*(pstKeyboard_->pucClkDir) 	|= pstKeyboard_->ucClkBit;
	
	PS2KeyOut_Delay(2*PHASE_TIME);	
	
	// Clock high
	*(pstKeyboard_->pucClkDir) 	&= ~pstKeyboard_->ucClkBit;
	*(pstKeyboard_->pucClkPort)	|= pstKeyboard_->ucClkBit;
}

//---------------------------------------------------------------------------
static void PS2KeyOut_Delay(USHORT usMicroSeconds_)
{
	volatile UCHAR i;
	
	// Approximate loop - 1us per loop @ 16MHz (w/loop overhead)
	for (i = 0; i < usMicroSeconds_; i++)
	{
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}
}
