//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
#include "types.h"
#include "kernelcfg.h"

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
void KernelTimer_Config(void)
{		
	TCCR1B |= (1 << WGM12); // CTC Mode Bit
	OCR1A = (USHORT)((SYSTEM_FREQ / TICK_FREQ) / 64) - 1;	// Set the trigger time / 64 prescaler
	TCCR1B |= (1 << CS11);		// CPU Time / 64 prescaler		
}

//---------------------------------------------------------------------------
void KernelTimer_Start(void)
{
	TIFR1 &= ~0x02;  // Clear pending interrupts
	TIMSK1 |= 0x02;	// Enable interrupt
	TCCR1B |= (1 << CS10);
}

//---------------------------------------------------------------------------
 void KernelTimer_Stop(void)
{
	TIFR1 &= ~0x02;  // Clear interrupt flags
	TIMSK1 &= ~0x02;	// Disable interrupt
	TCCR1B &= ~(1 << CS10);
}

//---------------------------------------------------------------------------
void KernelTimer_DI(void)
{
	TIFR1 &= ~0x02;  // Clear pending interrupts
	TIMSK1 &= ~0x02;	// Disable interrupt
}

//---------------------------------------------------------------------------
void KernelTimer_EI(void)
{
	TIFR1 &= ~0x02;  // Clear pending interrupts
	TIMSK1 |= 0x02;	// Enable interrupt
}
