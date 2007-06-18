// analog-test
//
// Tom Benedict

// The examples that use the library are significantly shorter than
// the stand-alone single-file examples.  This is the advantage of
// using a library of routines rather than throwing everything into
// one file.
//
// In this example we replicate the behavior of the bitfield-only.c
// example, but using Orangutan-lib instead.

// Include our I/O definitions
#include <avr/io.h>

// Include information about the device we're building for
#include "device.h"

// Include for using delay routines
#include <util/delay.h>

// Include the bitfield information
#include "bitfield.h"

// Delay for N seconds
void delay(unsigned char sec)
{
	unsigned int cycles;

	// Delay 25ms at a time (38.4ms is the most we can delay with a
	// 20MHz processor, unfortunately.  See the delay.h include file
	// for more info.)

	for(cycles = 0; cycles < (sec * 40); cycles ++)
	{
		_delay_ms(25);
	}
}

// And now for our main routine:
int main(void)
{

	// Since we don't actually have any subroutines or persistent 
	// "things" hanging around in memory, and since the function 
	// of the bitfields doesn't depend on any particular bits being 
	// set in memory, there's no initialization routines.  We just 
	// get to use them!
	//
	// We've got an LED on board that we can play with.  It lives on 
	// PORTD, on I/O bit 1.  So we can use our bitfield definitions 
	// to refer to it as "_PORTD.B1".  But first we need to tell the 
	// AVR it's an output!

	_DDRD.B1 = 1;

	// Now we can start assigning it a value.  Turn it off for now:

	_PORTD.B1 = 0;

	// Endless loop

	for(;;)
	{
		// Toggle the LED's value:
		_PORTD.B1 ^= 1;
		// Delay by one second
		delay(1);
	}

	// We never get here, but return a zero if we ever do.
	return(0);
}
