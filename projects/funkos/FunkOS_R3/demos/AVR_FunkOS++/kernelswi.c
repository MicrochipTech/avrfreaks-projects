#include "types.h"
#include "task.h"
#include "kernelswi.h"
#include "scheduler.h"

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
void KernelSWI_Config(void)
{
	PORTD &= ~0x04; // Clear INT0
	DDRD |= 0x04;	// Set PortD, bit 2 (INT0) As Output
	EICRA |= (1 << ISC00) | (1 << ISC01);	// Rising edge on INT0
}

//---------------------------------------------------------------------------
void KernelSWI_Start(void)
{		
	EIFR &= ~(1 << INTF0);	// Clear any pending interrupts on INT0
	EIMSK |= (1 << INT0);	// Enable INT0 interrupt (as long as I-bit is set)
}

//---------------------------------------------------------------------------
void KernelSWI_Stop(void)
{
	EIMSK &= ~(1 << INT0);	// Disable INT0 interrupts
}

//---------------------------------------------------------------------------
void KernelSWI_Clear(void)
{
	EIFR &= ~(1 << INTF0); 	// Clear the interrupt flag for INT0
}

//---------------------------------------------------------------------------
void KernelSWI_Trigger(void)
{
	if (FunkOS_Scheduler::IsEnabled())
	{
		PORTD &= ~0x04;
		PORTD |= 0x04;
	}
}
