// lcd_bargraph
//
// Adds an additional function to the LCD library:  The ability to
// display a one-character vertical bargraph for an unsigned char
// (0-255).  The idea for this came from a post Jim Remington made
// to the Pololu Forum demonstrating how to do a horizontal bar
// graph that used individual columns of pixels independently,
// resulting in a bar graph with pretty darned good resolution:
//
// http://www.uoxray.uoregon.edu/orangutan/lcd_bargraph.c
//
// This code turns things 90 degrees, and ditches Jim's nice 
// resolution in favor of fitting the entire bar graph into a 
// single character on a small LCD.

// Include our device settings
#include "device.h"

// We're storing stuff in EEPROM, so we need this:
#include <avr/pgmspace.h>

// This set of routines piggy-backs on top of lcd.c and lcd.h,
// so we need to include this, too:
#include "lcd.h"

// Location of the LCD's CG-RAM.  (It might behoove to move
// this to lcd.h and the first routine in here to lcd.c,
// making it more widely available.)
#define LCD_CGRAM	0x06

// Bitmaps of the bar graph characters.  Note that the LCD
// only has room for eight characters, but there are eight
// rows of pixels per character.  So we don't actually 
// define an empty block, using character 0x20 (space)
// instead:
const int vertical_bargraph_chars[] PROGMEM =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, // 1
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, // 2
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, // 3
	0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, // 4
	0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, // 5
	0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, // 6
	0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, // 7
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, // 8
};

// Routine to load a custom character into the LCD's CG-RAM.  This
// assumes you've got a block of characters defined, similar to what
// we've got up above.  So there are two indices passed to the routine.
// The first is the index inside the char_array[].  The second is the
// index for the character number you want to create inside the LCD.
// In this example the two indices are used in lock-step, but you
// don't have to do things this way.  In fact, this would be a neat
// way to do some graphics animation.  But I digress...

void lcd_load_custom_char(int* char_array, unsigned char rom_char_num, unsigned char lcd_char_num)
{
	register unsigned char i;

	// Multiply the character index by 8
	lcd_char_num = (lcd_char_num<<3);	// each character occupies 8 bytes
	rom_char_num = (rom_char_num<<3);	// each character occupies 8 bytes

	// Copy the 8 bytes into CG (character generator) RAM
	for(i=0; i<8; i++)
	{
		// Set CG RAM address
		lcd_cmd((1<<LCD_CGRAM) | (lcd_char_num + i));
		// write character data
		lcd_data( pgm_read_byte(char_array + rom_char_num + i) );
	}
}

// Initialize the bar code routine.  All this does is load the custom
// characters into the LCD:

void lcd_bar_init()
{
	// Load our vertical bar graph characters
	lcd_load_custom_char((int *)vertical_bargraph_chars, 0, 0);
	lcd_load_custom_char((int *)vertical_bargraph_chars, 1, 1);
	lcd_load_custom_char((int *)vertical_bargraph_chars, 2, 2);
	lcd_load_custom_char((int *)vertical_bargraph_chars, 3, 3);
	lcd_load_custom_char((int *)vertical_bargraph_chars, 4, 4);
	lcd_load_custom_char((int *)vertical_bargraph_chars, 5, 5);
	lcd_load_custom_char((int *)vertical_bargraph_chars, 6, 6);
	lcd_load_custom_char((int *)vertical_bargraph_chars, 7, 7);
}

// Print the bar code character for an unsigned 8-bit number:

void lcd_bar(unsigned char value)
{
	// Knock the value down into the 0-7 range
	value = value >> 5;

	// Print the appropriate character.  Only caution is we don't
	// have an empty bar (use a blank)

	if(value > 0)
	{
		// Print our custom character
		lcd_data(value - 1);
	} else {
		// Print a blank
		lcd_data(0x20);
	}
}

