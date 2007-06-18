// lcd-bargraph-test
//
// lcd-bargraph-test displays the ADC value of the on-board 
// potentiometer as a one-character vertical bar graph on the 
// Orangutan's LCD.
//
// This example, in and of itself, isn't all that useful.  The LCD
// has enough space on it to read out the ADC value of the on-board
// potentiometer without resorting to bar graphs.  If you're careful
// with your use of LCD space, you can display four ADC channels
// without breaking a sweat.  Past that things start to get more
// interesting, though, or you start resorting to hex.
//
// I recently took on a project where I wanted to be able to see
// the value of six ADC channels simultaneously in order to verify
// the system was working.  At the same time I wanted at least one
// whole line of the LCD left free so I could display the processed
// number that resulted from all six ADC numbers.  (If you haven't
// figured out I'm doing an analog line sensor for a line follower,
// I'll go ahead and spill the beans:  I'm doing an analog line
// sensor for a line follower.)
//
// All of a sudden the ability to display multiple bar graphs, each
// using no more than one LCD character space, took on new value.
// And so the lcd-bargraph-test test code was written.
//
// This example requires you to have an LCD.  But since it is an
// extension for the LCD subsystem, no #ifdefs have been used in
// the code.  If you're trying to use this on a Baby-Orangutan,
// it simply won't work.
//
// Much of the code is mine, but the entire credit for the idea goes
// to Jim Remington, who posted a horizontal bargraph program to
// the Pololu forums.  His is the first case I know of where someone
// has given the Orangutan a GUI, and is the first really useful
// example I've seen that makes use of the custom character space
// on HD44780 controlled LCDs.  For a look at Jim's code, see:
//
// http://www.uoxray.uoregon.edu/orangutan/lcd_bargraph.c
//
// It is a horizontal bargraph that takes up an entire line of the
// LCD display, and uses each column of pixels separately, giving
// really good resolution.  If you're looking to read out two sensors
// simultaneously (IR rangefinders for mini-sumo, for example),
// his bar graph code would be the way to go.
//
// Have fun!  Enjoy!
//
// Tom

// Of course we start by including device.h to pick up all our settings
#include "device.h"

// Since this example uses the ADC, include that next
#include "analog.h"

// Finally include the lcd.h file and the lcd_bargraph.h file (which
// can be found in the same directory where this file is found.)
#include "lcd.h"
#include "lcd-bargraph.h"

// Main routine

int main()
{
	// To store our ADC value in
	unsigned char adc;

	// Turn on the LCD
	lcd_init();

	// Start up the ADC
	analog_init();

	// Initialize the LCD bargraph
	lcd_bar_init();

	// Print our header line
	lcd_line1();
	lcd_string("Bargraph");
	
	// The endless loop
	for(;;)
	{
		// Read the onboard potentiometer
		adc = analog8(7);

		// Print the actual ADC value
		lcd_moveto(2,0);
		lcd_int(adc);

		// Display the vertical bargraph
		lcd_moveto(2,5);
		lcd_bar(adc);
	}

	// We never get here
	return 0;
}

// Not much to it, really.  And tweaking things to read out more than
// one ADC channel wouldn't take a lot of effort.
