// qti-test.c
//
// This demonstrates the use of the qti.c/h code, which reads
// a Parallax QTI edge sensor on one of the Orangutan's
// ADC inputs.
//
// The QTI is actually a pretty strange sensor, but its small
// size makes it handy for a lot of applications.
//
// Tom Benedict

// Include our device definitions

#include "device.h"

// Include our I/O definitions for our processor

#include <avr/io.h>

// For this we'll need the analog subsystem and the QTI-specific
// code

#include "analog.h"
#include "qti.h"

// If we have an LCD, might as well use it:

#include "lcd.h"


// Now for the main routine

int main(void)
{
	uint8_t qti0;

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
		qti0 = qti8(0);

		// Set the LED if the QTI is higher than 128
		if(qti0 > 128)
		{
			LED_PORT |= (1 << LED0);
		} else {
			LED_PORT &= ~(1 << LED0);
		}

		// If we have an LCD, use that as well
#ifdef _ENABLE_LCD_
		lcd_gotoxy(0,0);
		lcd_string("QTI: ");
		lcd_int(qti0);
#endif // _ENABLE_LCD_
	}
		
	// We never get here
	return 0;
}
