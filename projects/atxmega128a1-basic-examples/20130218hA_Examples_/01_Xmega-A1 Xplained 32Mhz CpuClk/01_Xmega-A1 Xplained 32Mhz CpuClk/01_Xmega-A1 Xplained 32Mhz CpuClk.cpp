/*
 * 01_Xmega-A1 Xplained 32Mhz CpuClk.cpp
 *
 * Version: 1.00
 *  Device: ATXmega128A1
 *	   Kit: Xmega-A1 Xplained
 *
 * Created: 18. 2. 2013 10:15:55
 * Updated: -
 *
 * Description:
 *    - switch cpu to 32 Mhz
 *		- led blink	in 1 second interval
 *
 * Note.1: I'm not professional programer. Sorry, i use own, non-standard programming style and techniques. :-)
 *			 	 
 *
 */ 

#define F_CPU 32000000UL											// this (F_CPU) inform _delay_ms function about cpuclk frequency we are used

#include <avr/io.h>
#include <util/delay.h>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "Drivers/eXmega_clock_driver.hpp"		// include unit with object eSystemClockDef

eSystemClockDef eSystemClock;									// System Clock Object

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main(void)
{
	// here MCU works on 2 Mhz internal oscilator
	
	// switch to 32 Mhz Internal oscilator
	eSystemClock.SetClock( ectInternal_Oscilator_32_Mhz );
	
	// here MCU works on 32 Mhz internal oscilator	

	// XMegaA1-Explained have leds on PORTE
	
	
	PORTE.DIR = 0xFF;						// Set PORTE as OUTPUT
	PORTE.OUT = 0xFF;						// All leds OFF	(XmegaA1-Explained rev. 7: 0 = ON, 1 = OFF :-) )
  
	while(1)
  {
    
		PORTE.OUTCLR = 0x01;			// Led ON
		_delay_ms( 500 );					// delay 500 ms
		
		PORTE.OUTSET = 0x01;			// Led OFF
		_delay_ms( 500 );					// delay 500 ms
		
  }
}
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
