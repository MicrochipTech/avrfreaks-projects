#include "types.h"
#include "kernelcfg.h"
#include "kerneltimer.h"

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
void KernelTimer::Config(void)
{		
	TCCR1B |= (1 << WGM12); // CTC Mode Bit
	TCCR1B |= (1 << CS11); // Prescale timer is /64
	OCR1A = (USHORT)((SYSTEM_FREQ / TICK_FREQ) / 64) - 1;	// Set the trigger time 
}

//---------------------------------------------------------------------------
void KernelTimer::Start(void)
{
	TIFR1 &= ~(1 << OCF1A);  // Clear pending interrupts
	TIMSK1 |= (1 << OCIE1A);	// Enable interrupt
	TCCR1B |= (1 << CS10);
}

//---------------------------------------------------------------------------
void KernelTimer::Stop(void)
{
	TIFR1 &= ~(1 << OCF1A);  // Clear interrupt flags
	TIMSK1 &= ~(1 << OCIE1A);	// Disable interrupt
	TCCR1B &= ~(1 << CS10);
}

//---------------------------------------------------------------------------
void KernelTimer::DI(void)
{
	TIFR1 &= ~(1 << OCF1A);  // Clear pending interrupts
	TIMSK1 &= ~(1 << OCIE1A);	// Disable interrupt
}

//---------------------------------------------------------------------------
void KernelTimer::EI(void)
{
	TIFR1 &= ~(1 << OCF1A);  // Clear pending interrupts
	TIMSK1 |= (1 << OCIE1A);	// Enable interrupt
}
