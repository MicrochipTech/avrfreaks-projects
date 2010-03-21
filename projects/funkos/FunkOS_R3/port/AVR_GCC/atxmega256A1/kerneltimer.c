//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  kerneltimer.c
	
	Description:
		AVR XMEGA implementation of the kernel timer
*/
//--------------------------------------------------------------------------- 
#include "types.h"
#include "kernelcfg.h"

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
void KernelTimer_Config(void)
{		
	TCC1.CTRLA = 0;	
	TCC1.CTRLB = 0; 	// Disable individual capture/compares on this timer since all we care about is overflow.
	TCC1.CTRLC = 0; 	
	TCC1.CTRLD = 0x20;
			
	TCC1.CTRLE = 0x00;	// using word mode (not byte mode)

	TCC1.CTRLFCLR |= 0x0F;	// Reset the controller for upcount, no controller update
	TCC1.INTFLAGS |= 0x01;	// Clear interrupt on overflow

	TCC1.PER = (USHORT)((SYSTEM_FREQ / TICK_FREQ) / 64) - 1;	// Set the trigger time
	
	TCC1.INTCTRLB = 0; // Disable ABCD Capture compare interrupts.
}

//---------------------------------------------------------------------------
void KernelTimer_Start(void)
{
	TCC1.CTRLA = 0x05;	// Clock / 64
	TCC1.INTFLAGS |= 0x01;	// Clear interrupt on overflow
	TCC1.INTCTRLA |= 0x01;
}

//---------------------------------------------------------------------------
 void KernelTimer_Stop(void)
{
	TCC1.INTFLAGS |= 0x01;	// Clear interrupt on overflow
	TCC1.INTCTRLA &= ~(0x01); // Disable the interrupt
	TCC1.CTRLA = 0;	// Clock source disable.
}

//---------------------------------------------------------------------------
void KernelTimer_DI(void)
{
	TCC1.INTCTRLA &= ~(0x01); // Disable the interrupt
}

//---------------------------------------------------------------------------
void KernelTimer_EI(void)
{
	TCC1.INTCTRLA |= 0x01; // Enable the interrupt
}
