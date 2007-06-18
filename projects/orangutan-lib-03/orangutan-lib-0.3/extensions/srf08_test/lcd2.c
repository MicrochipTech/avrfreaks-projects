//
// Orangutan / Baby Orangutan
//
// Utility Library
//
// Version 0.23 - 27 Dec 2006
//
// Revision History:
//
// 0.24	04 Feb 2007 (sj) Make all delays CPU-independent using F_CPU and util/delay.h
//					Tighten up delays, remove excess wait states per LCD data sheet
//					Add lcd_string_P for displaying strings from program FLASH memory.
//
//
// Commands:
//
////// LCD Routines (Orangutan-Only) //////
//
// lcd_init()			Initialize the LCD routines
// lcd_string(str)		Send a string to the LCD
// lcd_int(int)			Send an integer to the LCD
// lcd_moveto(line, pos)	Move cursor to line, position
// lcd_moverel(dir, num)	Shift the cursor left or right by N characters
// lcd_shift(dir, num)		Scroll the display left or right by N characters
// lcd_line1()			Move cursor to line 1
// lcd_line2()			Move cursor to line 2
// lcd_clear()			Clear the LCD
// lcd_show()			Show the LCD cursor
// lcd_hide()			Hide the LCD cursor
//
// Credit where credit is due:
//
// LCD routines came from a whole range of sources including the device
// datasheet, my own original attempt at coding, Cathy Saxton's code,
// some code posted to the Pololu online forums, and the ultimate show-
// saver: some code Jan Malasec was kind enough to send me (that worked!)
//
// Tom Benedict
//

// Baby Orangutans now use a 20MHz clock.  If you're compiling this for
// a different device, this needs to be changed for delays to work
// properly.  (The newer version of the Orangutan supports a 20MHz
// clock but runs at 8 on the internal oscillator.)

#ifndef F_CPU
#define F_CPU        8000000UL     
#endif
// This one uses the AVR I/O as well as the delay header files.
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "lcd.h"
 
/////////////////////////////////////////////////////////////////////
//
// ORANGUTAN LCD ROUTINES
//
/////////////////////////////////////////////////////////////////////

// This is code to drive the 8x2 LCD display on
// the Orangutan controller.  It is driven in
// 4-bit mode with E, R/W, and RS control lines
//
// This is a combination of some stuff I wrote (that
// didn't work) based off of the device data sheet, and
// some code Jan Malasec sent to me (which did work!)
//
// Pin Assignments
// 
// LCD	AVR	Notes     Direction
// ---	---	--------- ----------------
// E	PD4	Dedicated Output
// RS	PD2	Dedicated Output
// R/W	PD3	Dedicated Output
// DB7	PD7	Dedicated Input and Output
// DB6	PB5	Also SW5  Output
// DB5	PB4	Also SW4  Output
// DB4	PB3	Also SW3  Output
// DB3	N/C
// DB2	N/C
// DB1	N/C
// DB0	N/C
//
// As you can see, three of the four data lines are also
// tasked to the three push buttons on the Orangutan.
// What this means is that we need to keep track of this
// and that we can't use the buttons when the LCD is
// being written to.  We have no gurantee what state
// those lines are in, so we need to return them to
// whatever state they were in once we're done using
// them.

// Port Masks

#define PORTB_MASK		0x38	// PB3, PB4, PB5
#define PORTD_MASK		0x80	// PD7

// Control Pins

#define LCD_RW			PD3
#define LCD_RS			PD2
#define LCD_E			PD4

// Commands (from the LCD's datasheet)

#define	LCD_CLEAR		0x01
#define	LCD_LINE1		0x02
#define LCD_LINE2		0xC0
#define LCD_SHOW_BLINK	0x0F
#define LCD_SHOW_SOLID	0x0E		
#define LCD_HIDE		0x0C
#define LCD_CURSOR_L	0x10
#define LCD_CURSOR_R	0x14
#define LCD_SHIFT_L		0x18
#define LCD_SHIFT_R		0x1C

// PD7 serves both as input and output, so each routine that references
// it needs to make sure of its state.
//
// On the Orangutan, the three pushbuttons share duty with the LCD as 
// well, so the routines that reference them must assume they're in a
// state other than the one the LCD wants them to be, and return them
// to that state when they're done.

// lcd_nibble loads the data lines with four bits of data and
// strobe the enable pin to let the LCD know it should latch
// them.
//
// This assumes port B and D already have their DDR set correctly
// and assumes PD7 is an output.

//******************************************************************
// Global variables
//******************************************************************
	static unsigned char ucLcd_done;

// End Global vatiables
//------------------------------------------------------------------

void lcd_nibble(unsigned char nibble)
{

	// Make sure our nibble really is a nibble
	nibble &= 0x0F;
	// Since the data line pattern is so Orangutan-specific, I'm not 
	// doing this using #define bitmasks.  Besides, this is the one
	// place this happens in the code:
	nibble <<= 3;
	PORTB &= ~PORTB_MASK;
	PORTB |= (nibble & PORTB_MASK);
	nibble <<= 1;
	PORTD &= ~PORTD_MASK;
	PORTD |= (nibble & PORTD_MASK);
//	_delay_ms(1);
	_delay_us(1);
	// At this point the four data lines are set, so the Enable pin is
	// strobed to let the LCD latch them.
	//
	// The LCD device is highly dependent on delaying for a proper length
	// of time.  For a 20MHz device, 5 clock cycles is enough of a delay
	// for the LCD to latch its inputs. The delays provided should work on any
	// CPU 1 MHz or higher.
	//
	// Bring E high
	PORTD |= (1 << LCD_E);

	_delay_us(2);

	// Bring E low
	PORTD &= ~(1 << LCD_E);

	_delay_us(2);

}

// lcd_send writes one byte to the LCD (or rather, it sends two nibbles)
//
// This is the command where all the registers get saved, loaded, and
// restored at the end.  It assumes nothing about the state of ports
// B or D, and is the routine that actually sets them correctly, 
// restoring them when it's done.

void lcd_send(unsigned char data)
{
	unsigned char 	temp_ddrb, temp_portb,
					temp_ddrd, temp_portd;

	// Store our port settings
	temp_ddrb = DDRB;
	temp_portb = PORTB;
	temp_ddrd = DDRD;
	temp_portd = PORTD;

	// Set up port I/O to match what the LCD needs
	DDRB |= PORTB_MASK;
	DDRD |= PORTD_MASK;

	// Send the data
	lcd_nibble(data >> 4);	// High nibble first
	lcd_nibble(data);		// Low nibble second
	// Restore our port settings
	DDRD = temp_ddrd;
	PORTD = temp_portd;
	DDRB = temp_ddrb;
	PORTB = temp_portb;
}

// lcd_cmd sends a command to the LCD.  Hold the RS line low and send
// the byte of data.  Different commands require different delays;  ERASE
// and HOME are significantly slower than the rest, so the delays are 
// provided in the routines and macros that call lcd_cmd

void lcd_cmd(unsigned char cmd)
{

	PORTD &= !((1 << LCD_RW) | (1 << LCD_RS));
	lcd_send(cmd);

}


// lcd_data sends data to the LCD.  Hold the RS line high and send
// the byte of data.

void lcd_data(unsigned char data)
{
	PORTD &= ~(1 << LCD_RW);
	PORTD |= (1 << LCD_RS);
	lcd_send(data);
	_delay_us(100);
}

// lcd_string sends a string to the LCD.  You can send a string
// longer than 8 characters, but only eight characters show up.
// The string is printed from wherever the cursor is, and will
// not span lines.  (This lets you concatenate print statements.)

void lcd_string(const unsigned char *str)
{
	while (*str != 0)
		lcd_data(*str++);
}

// lcd_string_P copies a string from FLASH to RAM and calls
// lcd_string.  In theory this routine SHOULD be able to use
// the "pgm_read_byte" macro to fetch each byte from FLASH and
// pass it to lcd_data, but for some reason that does not work.
//
void lcd_string_P(const unsigned char *str)
{
	unsigned char ucStr[16];
	strncpy_P(ucStr,str,15);
	lcd_string(ucStr);
}

// lcd_int prints an integer (similar to my old baby-o "blink it out"
// routine).  Again, this prints from wherever the cursor is, and
// will not span lines.  (This lets you concatenate print
// statements.)

void lcd_int(unsigned char n)
{
	unsigned char st[4] = {0,0,0,0};

	// The 0x30 addition shifts the decimal number up
	// to the ASCII location of "0"

	// Hundreds place
	st[0] = (n / 100) + 0x30;
	n = n % 100;
	// Tens place
	st[1] = (n / 10) + 0x30;
	n = n % 10;
	// Ones place
	st[2] = n + 0x30;

	// Print it as a string
	lcd_string(st);
}

// lcd_init initializes the LCD and MUST be called prior to
// every other LCD command.

void lcd_init(void)
{
	// This is done once, and is never checked or set afterward
	// These are dedicated lines, so this shouldn't be a problem
	// provided the rest of the program doesn't dink with it.
	//
	DDRD |= (1 << LCD_RW) | (1 << LCD_RS) | (1 << LCD_E);

	ucLcd_done = 1;		// First time in, mark LCD free

	_delay_ms(30);		// Wait for the LCD to power up
	lcd_cmd(0x30);		// 8-bit mode (wake up!)
	_delay_ms(5);		// Wait 5 ms
	lcd_cmd(0x30);		// 8-bit mode (wake up!)
	_delay_ms(1);		// Wait 1 ms
	lcd_cmd(0x30);		// 8-bit mode (wake up!)
	_delay_ms(1);		// Wait 1 ms
	lcd_cmd(0x32);		// 4-bit mode
	_delay_us(100);		// Wait 1 ms

	lcd_cmd(0x20);
	_delay_us(100);		// Wait 1 ms
	lcd_cmd(0x28);		// 4-bit mode, 2-line, 5x8 dots/char
	_delay_us(100);		// Wait 1 ms
	lcd_cmd(0x08);		// display off, cursor off, blink off
	_delay_us(100);		// Wait 1 ms
	lcd_cmd(0x01);		// clear display
	_delay_us(100);		// Wait 1 ms
	lcd_cmd(0x0F);		// display on, cursor on, blink on
	_delay_us(100);		// Wait 1 ms
	lcd_cmd(0x02);		// return home
	_delay_us(100);		// Wait 1 ms
	lcd_cmd(0x01);		// Clear Display
	_delay_us(100);		// Wait 1 ms

}

// LCD Commands

// lcd_moveto moves the cursor to the given position
//
// Character	0  1  2  3  4  5  6  7  
//           	------------------------
// Line 1		80 81 82 83 84 85 86 87
// Line 2		C0 C1 C2 C3 C4 C5 C6 C7

void lcd_moveto(unsigned char line, unsigned char pos)
{
  lcd_cmd((line == 1 ? 0x80 : 0xC0) + pos);
  _delay_us(50);
}

// lcd_moverel shifts the cursor left or right the given number of
// positions.
//
// dir = 0	Shift left
//       1	Shift right

void lcd_moverel(unsigned char dir, unsigned char num)
{
	unsigned char cmd;

	cmd = dir ? LCD_CURSOR_R : LCD_CURSOR_L;
	while(num-- > 0) {
		lcd_cmd(cmd);
		_delay_us(100);
	}
}

// lcd_shift shifts the display left or right the given number of
// positions.
//
// dir = 0	Shift left
//       1	Shift right

void lcd_shift(unsigned char dir, unsigned char num)
{
	unsigned char cmd;

	cmd = dir ? LCD_SHIFT_R : LCD_SHIFT_L;
	while(num-- > 0) {
		lcd_cmd(cmd);
		_delay_us(100);
		};
}

// The rest of the routines are inlines, and live in the .h file

