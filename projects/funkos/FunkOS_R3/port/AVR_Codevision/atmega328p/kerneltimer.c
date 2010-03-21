#include "types.h"
#include "kernelcfg.h"
#include "taskport.h"

//---------------------------------------------------------------------------
#define WGM12 	(3)
#define CS11 	(1)
#define CS10 	(0)

//---------------------------------------------------------------------------
void KernelTimer_Config(void)
{		
    TCCR1B |= (1 << WGM12); 	// CTC Mode Bit
	TCCR1B |= (1 << CS11);		// CPU Time / 64 prescaler		
	OCR1A = (USHORT)((SYSTEM_FREQ / TICK_FREQ) / 64) - 1;	// Set the trigger time 	
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
