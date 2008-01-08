/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#include "OSCCal.h"

// ======================================================================================

/*
 NAME:      | OSCCAL_Calibrate
 PURPOSE:   | Recalibrates the internal RC oscillator to 7.372800MHz for 115200 baud serial communications
 ARGUMENTS: | None
 RETURNS:   | None
*/
void OSCCAL_Calibrate(void)
{
	uint8_t LoopCount = (0x7F / 2); // Maximum range is 128, and starts from the middle, so 64 is the max number of iterations required

	// Make sure all clock division is turned off (8MHz RC clock)
	CLKPR  = (1 << CLKPCE);
	CLKPR  = 0x00;
	
	// Inital OSCCAL of half its maximum
	OSCCAL = (0x7F / 2);
	
	// Disable timer interrupts
	TIMSK1 = 0;
	TIMSK2 = 0;
			
	// Set timer 2 to asyncronous mode (32.768KHz crystal)
	ASSR   = (1 << AS2);
	
	// Ensure timer 1 control register A is cleared
	TCCR1A = 0;
	
	// Start both counters with no prescaling
	TCCR1B = (1 << CS10);
	TCCR2A = (1 << CS20);
			 
	// Wait until timer 2's external 32.768KHz crystal is stable
	while (ASSR & ((1 << TCN2UB) | (1 << TCR2UB) | (1 << OCR2UB)));
		
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{	
		while (LoopCount--)
		{
			// Clear the timer values
			TCNT1  = 0;
			TCNT2  = 0;

			// Wait until timer 2 overflows
			while (!(TIFR2 & (1 << TOV2)));
		
			// Stop timer 1 so it can be read
			TCCR1B = 0x00;
			
			if (TCNT1 > (OSCCAL_TARGETCOUNT + OSCCAL_TOLLERANCE))      // Clock is running too fast
			  OSCCAL--;
			else if (TCNT1 < (OSCCAL_TARGETCOUNT - OSCCAL_TOLLERANCE)) // Clock is running too slow
			  OSCCAL++;
			else                                                       // Clock within tollerance
			  LoopCount = 0;
			
			// Clear timer 2 overflow flag
			TIFR2 |= (1 << TOV2);
	
			// Restart timer 1
			TCCR1B = (1 << CS10);	
		}

		// Stop the timers
		TCCR1B = 0x00;
		TCCR2A = 0x00;
	}	
	
	// Turn off timer 2 asynchronous mode
	ASSR  &= ~(1 << AS2);
	
	return;
}
