#include "types.h"
#include "taskport.h"
#include "kernelswi.h"

//#include <avr\interrupt.h>

//---------------------------------------------------------------------------
void KernelSWI_Config(void)
{
	PORTD &= ~0x04; // Clear INT0
	DDRD |= 0x04;	// Set PortD, bit 2 (INT0) As Output
	MCUCR |= 0x03;	// Rising edge
}

//---------------------------------------------------------------------------
void KernelSWI_Start(void)
{
	GICR |= 0x40;	// Enable INT0 interrupts
}

//---------------------------------------------------------------------------
void KernelSWI_Stop(void)
{
	GICR &= ~0x40;	// Disable INT0 interrupts
}

//---------------------------------------------------------------------------
void KernelSWI_Clear(void)
{
	GIFR |= 0x40; 	// Clear the interrupt flag for INT0
}

//---------------------------------------------------------------------------
void KernelSWI_Trigger(void)
{
	if (Task_IsSchedulerEnabled())
	{
		PORTD &= ~0x04;
		PORTD |= 0x04;
	}
}
