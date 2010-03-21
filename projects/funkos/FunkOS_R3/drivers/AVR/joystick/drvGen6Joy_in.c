//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvGen6joy_in.c
//
// Description:
//	Input driver for Sega Genesis 6-button joysticks.  
//--------------------------------------------------------------------------- 

#include "types.h"
#include "driver.h"
#include "drvGen6joy_in.h"

//---------------------------------------------------------------------------
static void Gen6i_Read(GEN6_IN_STRUCT *pstJoy_);
static void Gen6i_Delay(USHORT usMicroSeconds_);

//---------------------------------------------------------------------------
void Gen6i_Init(DRIVER_STRUCT *pstThis_)
{
	GEN6_IN_STRUCT *pstJoy = (GEN6_IN_STRUCT*)pstThis_;
	
	// Set up the select line as an output
	*(pstJoy->pucPortDirSelect)	|= pstJoy->ucBitSelect;		
	// Output high on the line
	*(pstJoy->pucPortData1) 	|= pstJoy->ucBitSelect;
	
	// Set the rest of the pins as inputs
	*(pstJoy->pucPortDirData1)	&= ~pstJoy->ucBitData1;
	*(pstJoy->pucPortDirData2)	&= ~pstJoy->ucBitData2;
	*(pstJoy->pucPortDirData3)	&= ~pstJoy->ucBitData3;
	*(pstJoy->pucPortDirData4)	&= ~pstJoy->ucBitData4;
	*(pstJoy->pucPortDirData6)	&= ~pstJoy->ucBitData6;
	*(pstJoy->pucPortDirData9)	&= ~pstJoy->ucBitData9;
	
	// Enabled pullups on the rest of the port pins
	*(pstJoy->pucPortData1)		&= ~pstJoy->ucBitData1;
	*(pstJoy->pucPortData2)		&= ~pstJoy->ucBitData2;
	*(pstJoy->pucPortData3)		&= ~pstJoy->ucBitData3;
	*(pstJoy->pucPortData4)		&= ~pstJoy->ucBitData4;
	*(pstJoy->pucPortData6)		&= ~pstJoy->ucBitData6;
	*(pstJoy->pucPortData9)		&= ~pstJoy->ucBitData9;
	
	// Clear the data in the joystick report struct
	Joystick_Clear(&pstJoy->stReport);
}
//---------------------------------------------------------------------------
void Gen6i_Start(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to start the driver...
}
//---------------------------------------------------------------------------
void Gen6i_Stop(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to stop the driver...
}
//---------------------------------------------------------------------------
void Gen6i_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_)
{	
	// Cast the driver to the specified type
	GEN6_IN_STRUCT *pstJoy = (GEN6_IN_STRUCT*)pstThis_;
	
	// Switch I/O control operation by ID
	switch(usID_)
	{
		// Read the joystick buttons
		case JOY_IN_READ:
		{
			// Read the buttons...
			Gen6i_Read(pstJoy);
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
static void Gen6i_Read(GEN6_IN_STRUCT *pstJoy_)
{
	UCHAR ucButton;
	
	// Ensure that we're starting with the select line high...
	*(pstJoy_->pucPortSelect) |= pstJoy_->ucBitSelect;
	Gen6i_Delay(10);
	
	//-----------------------------------------------------------------------
	// 4 phases used to read a 6-button genesis joypad input as follows.
	// Pulse select line to read inputs.
	//-----------------------------------------------------------------------
	
	//					1,    2,   3,   4, 6, 9
	// Pulse 1 - Low:  Up, Down, Low, Low, A, Start
	*(pstJoy_->pucPortSelect) &= ~pstJoy_->ucBitSelect;
	Gen6i_Delay(10);
	
	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bUp	= 
		(*(pstJoy_->pucPortInData1) & (pstJoy_->ucBitData1));
	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bDown	=
		(*(pstJoy_->pucPortInData2) & (pstJoy_->ucBitData2));
	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bA		=
		(*(pstJoy_->pucPortInData3) & (pstJoy_->ucBitData3));
	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bStart	=
		(*(pstJoy_->pucPortInData4) & (pstJoy_->ucBitData4));
	
	// Pulse 1 - High: Up, Down, Left, Right, B, C
	*(pstJoy_->pucPortSelect) |= pstJoy_->ucBitSelect;
	Gen6i_Delay(10);

	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bLeft	= 
		(*(pstJoy_->pucPortInData3) & (pstJoy_->ucBitData3));
	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bRight	=
		(*(pstJoy_->pucPortInData4) & (pstJoy_->ucBitData4));
	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bB		=
		(*(pstJoy_->pucPortInData6) & (pstJoy_->ucBitData6));
	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bC	=
		(*(pstJoy_->pucPortInData9) & (pstJoy_->ucBitData9));
	
	// Pulse 2 - Low:  Up, Down, Low, Low, A, Start
	// Pulse 2 - High: Up, Down, Left, Right, B, C
	*(pstJoy_->pucPortSelect) &= ~pstJoy_->ucBitSelect;
	Gen6i_Delay(10);
	*(pstJoy_->pucPortSelect) |= pstJoy_->ucBitSelect;
	Gen6i_Delay(10);

	// Pulse 3 - Low:  N/A
	*(pstJoy_->pucPortSelect) &= ~pstJoy_->ucBitSelect;
	Gen6i_Delay(10);

	// Pulse 3 - High: Z, Y, X, Mode, N/A, N/A
	*(pstJoy_->pucPortSelect) |= pstJoy_->ucBitSelect;
	Gen6i_Delay(10);
	
	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bZ	= 
		(*(pstJoy_->pucPortInData1) & (pstJoy_->ucBitData1));
	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bY	=
		(*(pstJoy_->pucPortInData2) & (pstJoy_->ucBitData2));
	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bX		=
		(*(pstJoy_->pucPortInData3) & (pstJoy_->ucBitData3));
	pstJoy_->stReport.JOY_DIGITAL.JOY_GEN.bMode =
		(*(pstJoy_->pucPortInData4) & (pstJoy_->ucBitData4));
	
	// Pulse 4 - Resets the pattern
	*(pstJoy_->pucPortSelect) &= ~pstJoy_->ucBitSelect;
	Gen6i_Delay(10);
	*(pstJoy_->pucPortSelect) |= pstJoy_->ucBitSelect;
	Gen6i_Delay(10);
}
//---------------------------------------------------------------------------
static void Gen6i_Delay(USHORT usMicroSeconds_)
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
