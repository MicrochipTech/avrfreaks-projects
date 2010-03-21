#include "types.h"
#include "kernelcfg.h"

#include <iom328p.h>

//---------------------------------------------------------------------------
void KernelTimer_Config(void)
{		
	TCCR1B_WGM12 = 1; // CTC Mode Bit
	TCCR1B_CS11 = 1; // Prescale timer is /64
	OCR1A = (USHORT)((SYSTEM_FREQ / TICK_FREQ) / 64) - 1;	// Set the trigger time 
}

//---------------------------------------------------------------------------
void KernelTimer_Start(void)
{
	TIFR1_OCF1A = 0;  // Clear pending interrupts
	TIMSK1_OCIE1A = 1;	// Enable interrupt
	TCCR1B_CS10 = 1;
}

//---------------------------------------------------------------------------
 void KernelTimer_Stop(void)
{
	TIFR1_OCF1A = 0;  // Clear interrupt flags
	TIMSK1_OCIE1A = 0;	// Disable interrupt
	TCCR1B_CS10 = 0;
}

//---------------------------------------------------------------------------
void KernelTimer_DI(void)
{
	TIFR1_OCF1A = 0;  // Clear pending interrupts
	TIMSK1_OCIE1A = 0;	// Disable interrupt
}

//---------------------------------------------------------------------------
void KernelTimer_EI(void)
{
	TIFR1_OCF1A = 0;  // Clear pending interrupts
	TIMSK1_OCIE1A = 1;	// Enable interrupt
}
