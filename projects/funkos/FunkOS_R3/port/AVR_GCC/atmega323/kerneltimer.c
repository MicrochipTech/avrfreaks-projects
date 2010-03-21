#include "types.h"
#include "kernelcfg.h"

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
void KernelTimer_Config(void)
{		
	TCCR1B |= (1 << CTC11); // CTC Mode Bit
	OCR1A = (USHORT)((SYSTEM_FREQ / TICK_FREQ) / 64) - 1;	// Set the trigger time / 64 prescaler
	TCCR1B |= (1 << CS11);		// CPU Time / 64 prescaler		
}

//---------------------------------------------------------------------------
void KernelTimer_Start(void)
{
	TIFR &= ~0x10;  // Clear pending interrupts
	TIMSK |= 0x10;	// Enable interrupt
	TCCR1B |= (1 << CS10);
}

//---------------------------------------------------------------------------
 void KernelTimer_Stop(void)
{
	TIFR &= ~0x10;  // Clear interrupt flags
	TIMSK &= ~0x10;	// Disable interrupt
	TCCR1B &= ~(1 << CS10);
}

//---------------------------------------------------------------------------
void KernelTimer_DI(void)
{
	TIFR &= ~0x10;  // Clear pending interrupts
	TIMSK &= ~0x10;	// Disable interrupt
}

//---------------------------------------------------------------------------
void KernelTimer_EI(void)
{
	TIFR &= ~0x10;  // Clear pending interrupts
	TIMSK |= 0x10;	// Enable interrupt
}
