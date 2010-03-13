/*
 * This file contains basic setup for clock speed and a simple demonstration of the use
 * 3 of the e3 routines to control the E3 LCD-Key Part SB6432 configurable
 * LCD display/switches.  See <http://www.e3-keys.com/>.
 * This code was developed on the Atmel mega168.
 *
 * This code is not maintained and is free to use with no rights reserved, and
 * no guarantees made.
 *
 * Code is based on that developed by Tom Blough at AVR Freaks <http://www.avrfreaks.net>
 *
 * Uses - SPI
 *
 * Jim Minihane 3/7/2010
 */


#define F_CPU 8000000L	// 8 mhz cpu frequency for delay timing

#define DEMO_DELAY 1000
#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "e3_lcdkey.h"
#include "utility.h"


void init()
{

/*
 * Initialize clock settings
 */
	CLKPR = _BV(CLKPCE);
	  	// Sets CLKPCE Bit to 1 to enable the CLKPS bits

	CLKPR = 0;
	  	// A value = 0 sets CLKPS0 Bit to 0 to set the clock divider to 1.
		// This uses the calibrated 8MHz clock with no divider for 8MHz.

/* Button - Set as input - You can use whatever port you want as your button.
 * A button input isn't used in the below demo code, but I've left it here for your reference.
 */

	DDRD &= ~_BV(DDD7);		// Define Port D7 as an input.
	PORTD |= _BV(PORTD7);	// Enable Pullup on Button input

//----------------- End INIT section --------------------------

}

int main(void)
{
	init(); 	// Initialize routine
	e3_init();	// E3 Initialise routine

	_delay_ms(5);				// Delay to allow the E3 to self initialize if it's powered on at the same time as the AVR
								// This may not be required or may not need to be as long as 5 mS. So experiment.
	e3_set_scr(0x00); // Clear the entire screen
	_delay_ms(1);
	e3_set_color(E3_BRIGHT_GREEN); // Set RGB color to Bright green
	_delay_ms(1);
	e3_set_text_line("DEMO START",E3_TEXT_CENTER,E3_LINE_1); //Display text
	_delay_ms(DEMO_DELAY);

	for (;;)
	{
		e3_set_scr(0x00); // Clear the entire screen
		_delay_ms(1);
		e3_set_color(E3_BRIGHT_BLUE); // Set RGB color to Bright Blue
		_delay_ms(DEMO_DELAY);
		e3_set_color(E3_BRIGHT_MAGENTA); // Set RGB color to Bright magenta
		_delay_ms(DEMO_DELAY);
		e3_set_color(E3_BRIGHT_RED); // Set RGB color to Bright red
		_delay_ms(DEMO_DELAY);
		e3_set_color(E3_BRIGHT_GREEN); // Set RGB color to Bright green
		_delay_ms(DEMO_DELAY);

		e3_set_text_line("This is a",E3_TEXT_LEFT,E3_LINE_1);	// Send text string "This is a", Left Justified, to Line 1
		_delay_ms(DEMO_DELAY);
		e3_set_text_line("demo of",E3_TEXT_CENTER,E3_LINE_2);	// Send text string "demo of", Center Justified, to Line 2
		_delay_ms(DEMO_DELAY);
		e3_set_text_line("LCD-Key",E3_TEXT_RIGHT,E3_LINE_3);	// Send text string "of LCD-Key", Right Justified, to Line 3
		_delay_ms(DEMO_DELAY);
		e3_set_text_line("Functions",E3_TEXT_LEFT,E3_LINE_4);	// Send text string "Functions", Left Justified, to Line 4
		_delay_ms(DEMO_DELAY);


		e3_clear_line(E3_LINE_1);	// Clear Line 1 of all data.
		_delay_ms(DEMO_DELAY);
		e3_clear_line(E3_LINE_2);	// Clear Line 2 of all data.
		_delay_ms(DEMO_DELAY);
		e3_clear_line(E3_LINE_3);	// Clear Line 3 of all data.
		_delay_ms(DEMO_DELAY);
		e3_clear_line(E3_LINE_4);	// Clear Line 4 of all data.
		_delay_ms(DEMO_DELAY);

		e3_bar_graph(10,E3_LINE_1); // Build a bar graph on line 1, Value = 10
		_delay_ms(DEMO_DELAY);

		e3_bar_graph(30,E3_LINE_2); // Build a bar graph on line 2, Value = 30
		_delay_ms(DEMO_DELAY);

		e3_bar_graph(50,E3_LINE_3); // Build a bar graph on line 3, Value = 50
		_delay_ms(DEMO_DELAY);

		e3_bar_graph(60,E3_LINE_4); // Build a bar graph on line 4, Value = 60
		_delay_ms(DEMO_DELAY);

		e3_set_image(1); // Display image in position 0
		_delay_ms(DEMO_DELAY);

		e3_set_image(0); // Display image in position 1
		_delay_ms(DEMO_DELAY);

		e3_set_scr(0xFF);
		_delay_ms(DEMO_DELAY);

		e3_set_text_loc("txt", E3_TEXT_EXACT, 6, E3_TEXT_LEFT, E3_LINE_1, 4);
		_delay_ms(DEMO_DELAY);

		e3_set_text_loc("txt", E3_TEXT_EXACT, 6, E3_TEXT_CENTER, E3_LINE_1, 4);
		_delay_ms(DEMO_DELAY);

		e3_set_text_loc("txt", E3_TEXT_EXACT, 6, E3_TEXT_RIGHT, E3_LINE_1, 4);
		_delay_ms(DEMO_DELAY);

		e3_set_text_loc("text", E3_TEXT_EXACT, 4, E3_TEXT_LEFT, E3_LINE_2, 10);
		_delay_ms(DEMO_DELAY);

		e3_set_text_loc("text", E3_TEXT_EXACT, 4, E3_TEXT_CENTER, E3_LINE_3, 20);
		_delay_ms(DEMO_DELAY);

		e3_set_text_loc("text", E3_TEXT_EXACT, 4, E3_TEXT_RIGHT, E3_LINE_4, 30);
		_delay_ms(DEMO_DELAY);

		e3_set_scr(0x00); // Clear the entire screen
		_delay_ms(1);
		e3_set_text_line("DEMO",E3_TEXT_CENTER,E3_LINE_1);
		e3_set_text_line("RESTARTING",E3_TEXT_CENTER,E3_LINE_2);
		_delay_ms(1000);

	}
}
