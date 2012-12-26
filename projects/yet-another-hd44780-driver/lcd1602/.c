/*
 * lcd.c
 *
 * Example file to show operation of LCD display
 *
 * Created: 2012-12-24 18:46:59
 *  Author: George
 */ 


#include <avr/io.h> // every file should have one of these
#include <util/delay.h> // for the delay in the counting loop (see EXAMPLE 3)
#include "lcd1602.h" // how to use the lcd1602 code: copy the files into your directory, and include/configure the h file

/* A custom character - carefully arranged here as it will display on screen
**		- 8 bytes, each one is a row of bits
**		- A 1 is a dark dot, a 0 is a light dot
**		- For 5x8 displays (also referred to as 5x7, as the bottom row is usually reserved for the cursor underline)
**			(most common), the 5 LSBs are data, the others are useless 
**		- For 5x10 displays (uncommon), you only get 4 characters instead of 8, as a whole character address is used for those bottom two lines!
**		- Refer to a chip datasheet for more info
*/
uint8_t customchar[8] = {	0b00011100,
							0b00010000,
							0b00010100,
							0b00010111,
							0b00011101,
							0b00000111,
							0b00000110,
							0b00000101,
						};

void main() // I prefer to not have an int return - who's gonna use it??
{

	/* initialise LCD - check lcd1602.h for all the config options!
	** initialisation should leave you with a blank, switched-on display, ready for
	** auto-incrementing writing to the first character of line (DD ram address 0x00)
	*/
	lcdInit();
	
	// *** EXAMPLE 1: WRITING TEXT TO THE SCREEN ***
	lcdFunc(LCD_FUNC_ONOFF); // switch off (so we don't see it flicker as the test is written - try commenting this out and see how there's really no difference!
	lcdWrite("Hello", 5); // writes "Hello" to the first five characters of line 1 (we start at this position, of course)
	lcdSetPos(LCD_LINE2 + 5); // move to line 2 - typically DD address 0x40, then 5 characters to the right
	lcdWrite("World!", 6); // writes "World!" - note the length must be the same as the string. No biggie if you get it wrong, just expect garbage!
	lcdFunc(LCD_FUNC_ONOFF | LCD_VAR_DISPON); // switch the display on to see some text. try or'ing VAR_CURSON or VAR_BLINKON here to see how these work
	
	// *** EXAMPLE 2: MAKING AND DISPLAYING A CUSTOM CHARACTER ***
	lcdFunc(LCD_FUNC_SETCGADDR | 0x00); // set address to CG ram (first character, first byte) - same as lcdSetPos, but didn't feel it was necessary to wrap this one
	lcdWrite(customchar, 8); // write character data to CG ram (with benefit of auto-increment setting, works just like writing text!)
	lcdSetPos(LCD_LINE2 + 15); // move back to a DD ram address at the end of line 2
	lcdData(0x00); // write custom character to the screen (note lcdWrite just calls lcdData multiple times for each character of a string, here we just do one directly)
	
	// ** EXAMPLE 3: CONTINUOUSLY CHANGING CONTENT AND STRINGIFICATION OF A NUMBER ***
	uint8_t i = 0; // set up a number to count to 255 over and over again
	uint8_t s[3]; // set up a string buffer to turn a number into a string representation
	while(1) // do this forever
    {
		lcdSetPos(LCD_LINE1 + 12); // move to line 1, 12 characters over
		
		/* Turning a number into a string - some LCD code from the net will give you a handy function for this, but I prefer to write the code myself.
		** I have a number <--> decimal/hex string include file set for these kind of things, but you may not want all that
		**		Fortunately, single quotes in C gives you the ascii value of the character contained: '0' = 48, or 0x30
		**		This is handy for decimal string conversion
		*/
		s[0] = i/100 + '0'; // i/100 gives you the 100's place - i.e. 154 / 100 = 1
		s[1] = (i%100)/10 + '0'; // i%100 gives you the remainder after dividing by 100 - i.e. 154 % 100 = 54, then /10 gives you the tens place - i.e. 54/10 = 5
		s[2] = (i%10) + '0'; // i%10 gives you the remainder after dividing by 10 - i.e. 154 % 10 = 4
		
		lcdWrite(s, 3); // write the stringified number to the screen
		i++; // increment counter
		_delay_ms(1000); // wait for one second
    }
}