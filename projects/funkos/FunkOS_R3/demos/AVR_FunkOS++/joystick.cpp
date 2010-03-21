//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File: joystick.c
//
// Description:
//  Common joystick routines... do not optimize.
//--------------------------------------------------------------------------- 

#include "types.h"
#include "joystick.h"

void Joystick_Delay(USHORT usDelayUS_)
{
	volatile USHORT i;
	
	// Approximate loop - 1us per loop @ 16MHz (w/loop overhead)
	for (i = 0; i < usDelayUS_; i++)
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
//---------------------------------------------------------------------------
// Macro to copy the joystick data...
void Joystick_Copy(JOYSTICK_REPORT_STRUCT *pstDst_, JOYSTICK_REPORT_STRUCT *pstSrc_)
{
	pstDst_->usXAxis1 = pstSrc_->usXAxis1;
	pstDst_->usXAxis2 = pstSrc_->usXAxis2;
	pstDst_->usYAxis1 = pstSrc_->usYAxis1;
	pstDst_->usYAxis2 = pstSrc_->usYAxis2;
	pstDst_->JOY_DIGITAL.ulRawData = pstSrc_->JOY_DIGITAL.ulRawData; 
} 
//--------------------------------------------------------------------------- 
// Macro to clear the joystick data
void Joystick_Clear(JOYSTICK_REPORT_STRUCT *pstJoy_)
{
	pstJoy_->usXAxis1 = 0;
	pstJoy_->usXAxis2 = 0;
	pstJoy_->usYAxis1 = 0;
	pstJoy_->usYAxis2 = 0;
	pstJoy_->JOY_DIGITAL.ulRawData = 0;
}

