#include "types.h"
#include "task.h"
#include "kernelswi.h"

#include "iomacro.h"

#include <iom328p.h>

//---------------------------------------------------------------------------
void KernelSWI_Config(void)
{
	PORTD &= ~0x04; // Clear INT0
	DDRD |= 0x04;	// Set PortD, bit 2 (INT0) As Output
	EICRA_ISC00 = 1;
        EICRA_ISC01 = 1;	// Rising edge on INT0
}

//---------------------------------------------------------------------------
void KernelSWI_Start(void)
{		
	EIFR_INTF0 = 0;	// Clear any pending interrupts on INT0
	EIMSK_INT0 = 1;	// Enable INT0 interrupt (as long as I-bit is set)
}

//---------------------------------------------------------------------------
void KernelSWI_Stop(void)
{
	EIMSK_INT0 = 0; // Disable INT0 interrupts
}

//---------------------------------------------------------------------------
void KernelSWI_Clear(void)
{
	EIFR_INTF0 = 0; // Clear the interrupt flag for INT0
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
