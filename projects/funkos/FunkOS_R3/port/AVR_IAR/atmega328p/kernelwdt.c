#include "types.h"
#include "kernelcfg.h"
#include "taskport.h"

#include <iom328p.h>

#define WDT_PERIOD		(0x05)		// ~0.5 seconds

//---------------------------------------------------------------------------
void KernelWDT_Config(void)
{
	//WDTSCR |= WDT_PERIOD;
}

//---------------------------------------------------------------------------
void KernelWDT_Start(void)
{
	//WDTSCR |= (1 << WDE);
}

//---------------------------------------------------------------------------
void KernelWDT_Stop(void)
{
	//!! Must do this in critical seciton as the WDT clear enable only
	//!! exists for 4 clock cycles !!
	//CS_ENTER();
	
	// Write WDTE and WDTOE in the same instruction
	//WDTSCR |= (1 << WDE) | (1 << WDTOE);
	// Disable WDTE within 4 clock cycles
	//WDTSCR &= ~(1 << WDE);
	
	//CS_EXIT();
}

//---------------------------------------------------------------------------
void KernelWDT_Kick(void)
{
	// Write WDTE and WDTOE in the same instruction
	//WDTSCR |= (1 << WDE) | (1 << WDTOE);
}
