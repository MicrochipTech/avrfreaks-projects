//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
#include "types.h"
#include "kernelcfg.h"
#include "task.h"

#include <avr\io.h>
#include <avr\interrupt.h>

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
	//GICR |= 0x40;	// Enable INT0 interrupts
	EIMSK &= ~0x01;	// Disable INT0

	EICRA |= 0x03;	// Rising edge interrupts on INT0
	EIFR |= 0x01;	// Clear any pending interrupts on INT0
	EIMSK |= 0x01;	// Re-enable interrupts on INT0

}

//---------------------------------------------------------------------------
void KernelSWI_Stop(void)
{
	//GICR &= ~0x40;	// Disable INT0 interrupts
	EIMSK &= ~0x01;
	EIFR |= 0x01;
}

//---------------------------------------------------------------------------
void KernelSWI_Clear(void)
{
	//GIFR |= 0x40; 	// Clear the interrupt flag for INT0
	EIFR |= 0x01;
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
