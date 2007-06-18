// analog-test
//
// Tom Benedict

// This example demonstrates some of the functions in the
// analog subsystem.  It uses the onboard potentiometer
// as an analog input, so no additional hardware is
// required.

// Include our I/O definitions
#include <avr/io.h>

// Include information about the device we're building for
#include "device.h"

// Include for the analog subsystem
#include "analog.h"

// If we have an LCD, might as well use it:
#include "lcd.h"

// And now for our main routine:
int main(void)
{

	// The Orangutan and Baby-Orangutan both have an onboard
	// potentiometer hooked up to the AD7 line.  They also have
	// a red LED hooked up to PORTD, pin 1.  We can use
	// them to test the ADC routines.

	// A variable to store our converted analog value in:
	int pot;

// Only include this next part if we're using an Orangutan (not a
// Baby-O)

#ifdef _ENABLE_LCD_

	// Start up the LCD
	lcd_init();
	
#endif //_ENABLE_LCD_

	// Initialize the ADC subsystem:
	analog_init();

	// Set our LED as an output
	LED_DDR |= (1 << LED0);

	// Endless loop
	for(;;)
	{
		// Store the value of our analog input:
		pot = analog8(7);

		// A 10-bit number can give us 1024 counts of resolution.
		// Half that is 512.  If the value in pot is less than
		// 512, turn off the LED.  If it's higher, turn it on:

		if(pot > 128)
		{
			LED_PORT |=  (1 << LED0);
		} else {
			LED_PORT &= ~(1 << LED0);
		}

		// If we have an LCD, use that as well
#ifdef _ENABLE_LCD_
		lcd_gotoxy(0,0);
		lcd_string("ADC: ");
		lcd_int(pot);
#endif // _ENABLE_LCD_
	}

	// We never get here, but return a zero if we do:
	return(0);
}

