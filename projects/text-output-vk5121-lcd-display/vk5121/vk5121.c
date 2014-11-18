#include <avr/io.h>

// the character set starts at the space - 0x20 - to save space

const unsigned char font[480] PROGMEM = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5f,0x00,0x00,	//  !
	0x00,0x03,0x00,0x03,0x00,0x14,0x7f,0x14,0x7f,0x14,	// "#
	0x24,0x2a,0x7f,0x2a,0x12,0x23,0x13,0x08,0x64,0x62,	// $%
	0x36,0x49,0x55,0x22,0x50,0x00,0x05,0x03,0x00,0x00,	// &'
	0x00,0x1c,0x22,0x41,0x00,0x00,0x41,0x22,0x1c,0x00,	// ()
	0x14,0x08,0x3e,0x08,0x14,0x08,0x08,0x3e,0x08,0x08,	// *+
	0x00,0x50,0x30,0x00,0x00,0x08,0x08,0x08,0x08,0x08,	// ,-
	0x00,0x60,0x60,0x00,0x00,0x20,0x10,0x08,0x04,0x02,	// ./
	0x3e,0x51,0x49,0x45,0x3e,0x00,0x42,0x7f,0x40,0x00,	// 01
	0x42,0x61,0x51,0x49,0x46,0x21,0x41,0x45,0x4b,0x31,	// 23
	0x18,0x14,0x12,0x7f,0x10,0x27,0x45,0x45,0x45,0x39,	// 45
	0x3c,0x4a,0x49,0x49,0x30,0x01,0x71,0x09,0x05,0x03,	// 67
	0x36,0x49,0x49,0x49,0x36,0x06,0x49,0x49,0x29,0x1e,	// 89
	0x00,0x36,0x36,0x00,0x00,0x00,0x56,0x36,0x00,0x00,	// :;
	0x08,0x14,0x22,0x41,0x00,0x14,0x14,0x14,0x14,0x14,	// <=
	0x00,0x41,0x22,0x14,0x08,0x02,0x01,0x51,0x09,0x06,	// >?
	0x32,0x49,0x79,0x41,0x3e,0x7e,0x11,0x11,0x11,0x7e,	//  @A
	0x7f,0x49,0x49,0x49,0x36,0x3e,0x41,0x41,0x41,0x22,	// BC
	0x7f,0x41,0x41,0x22,0x1c,0x7f,0x49,0x49,0x49,0x41,	// DE
	0x7f,0x09,0x09,0x09,0x01,0x3e,0x41,0x49,0x49,0x7a,	// FG
	0x7f,0x08,0x08,0x08,0x7f,0x00,0x41,0x7f,0x41,0x00,	// HI
	0x20,0x40,0x41,0x3f,0x01,0x7f,0x08,0x14,0x22,0x41,	// JK
	0x7f,0x40,0x40,0x40,0x40,0x7f,0x02,0x0c,0x02,0x7f,	// LM
	0x7f,0x04,0x08,0x10,0x7f,0x3e,0x41,0x41,0x41,0x3e,	// NO
	0x7f,0x09,0x09,0x09,0x06,0x3e,0x41,0x51,0x21,0x5e,	// PQ
	0x7f,0x09,0x19,0x29,0x46,0x46,0x49,0x49,0x49,0x31,	// RS
	0x01,0x01,0x7f,0x01,0x01,0x3f,0x40,0x40,0x40,0x3f,	// TU
	0x1f,0x20,0x40,0x20,0x1f,0x3f,0x40,0x38,0x40,0x3f,	// VW
	0x63,0x14,0x08,0x14,0x63,0x07,0x08,0x70,0x08,0x07,	// XY
	0x61,0x51,0x49,0x45,0x43,0x00,0x7f,0x41,0x41,0x00,	// Z[
	0x02,0x04,0x08,0x10,0x20,0x00,0x41,0x41,0x7f,0x00,	// \]
	0x04,0x02,0x01,0x02,0x04,0x40,0x40,0x40,0x40,0x40,	// ^_
	0x00,0x01,0x02,0x04,0x00,0x20,0x54,0x54,0x54,0x78,	// `a
	0x7f,0x48,0x44,0x44,0x38,0x38,0x44,0x44,0x44,0x20,	// bc
	0x38,0x44,0x44,0x48,0x7f,0x38,0x54,0x54,0x54,0x18,	// de
	0x08,0x7e,0x09,0x01,0x02,0x0c,0x52,0x52,0x52,0x3e,	// fg
	0x7f,0x08,0x04,0x04,0x78,0x00,0x44,0x7d,0x40,0x00,	// hi
	0x20,0x40,0x44,0x3d,0x00,0x7f,0x10,0x28,0x44,0x00,	// jk
	0x00,0x41,0x7f,0x40,0x00,0x7c,0x04,0x18,0x04,0x78,	// lm
	0x7c,0x08,0x04,0x04,0x78,0x38,0x44,0x44,0x44,0x38,	// no
	0x7c,0x14,0x14,0x14,0x08,0x08,0x14,0x14,0x18,0x7c,	// pq
	0x7c,0x08,0x04,0x04,0x08,0x48,0x54,0x54,0x54,0x20,	// rs
	0x04,0x3f,0x44,0x40,0x20,0x3c,0x40,0x40,0x20,0x7c,	// tu
	0x1c,0x20,0x40,0x20,0x1c,0x3c,0x40,0x30,0x40,0x3c,	// vw
	0x44,0x28,0x10,0x28,0x44,0x0c,0x50,0x50,0x50,0x3c,	// xy
	0x44,0x64,0x54,0x4c,0x44,0x00,0x08,0x36,0x41,0x00,	// z{
	0x00,0x00,0x7f,0x00,0x00,0x00,0x41,0x36,0x08,0x00,	// |}
	0x10,0x08,0x08,0x10,0x08,0x00,0x00,0x02,0x05,0x02};	// ^degree
	
	///////////////////////////////////////////////////////////////////////////////////////////
//
// LCD control functions
//
// we're controlling a Vikay VK5121ELYG as noted above driven by two SED1520 controller
// chips
// we use astatic variable 'column' to define which column we're writing and therefore
// select the chip and odd offsets where necessary
// each chip controls 61 columns but the display has only 120; the 61st column is never used
//
// void lcd_control(unsigned char)		- write to both display control chips
// void lcd_set_column(unsigned char)	- set the start address for the display
// void lcd_set_cursor(unsigned char x, unsigned char y)	
//										- position the text cursor
// void lcd_write(unsigned char)		- write to the display based on 'column'
// lcd_read								- return the data from the display (not yet implemented)
// void lcd_init(void)					- reset the display and clear it
// void lcd_cls(void)					- clear the display
// void lcd_scroll(void)				- move the cursor to the next line
// void lcd_char(char)					- write a character to the display
// void lcd_string(* char)				- write a zero-terminated string to the display
///////////////////////////////////////////////////////////////////////////////////////////

static uint8_t column;
static uint8_t cursorx;
static uint8_t cursory;

//***************************************************************************
//* Port Pin Assignments
//***************************************************************************

// pc0-7	d0-7
// pa7		elcnt	(backlight control)	
// pa6		e2
// pa5		a0
// pa4		e1
// pa3		r/~w

#define	rw PA3
#define	e1 PA4
#define	a0 PA5
#define	e2 PA6
#define	elcnt PA7
	


void lcd_control(unsigned char control)
{
	// write a control value to *both* controllers simultaneously
	// this makes sense because setup etc is required for both
	
	// make port c all outputs
	DDRC = 0xff;
	
	// make all control outputs except elcnt, e1, and e2 low
	PORTA = _BV(elcnt) | _BV(e1) | _BV(e2);
	
	// write the data to port c
	PORTC = control;
	
	// and clear e1 and e2 again
	PORTA = _BV(elcnt);				// we want the light to stay on :)
}
	
	
void lcd_set_cursor(unsigned char cx, unsigned char cy)
{
	// put the cursor on a specified cell, and set the column value at the same time
	
	cursorx = cx;
	cursory = cy;
	
	lcd_set_column(cx*6, cy);
}

void lcd_set_column(unsigned char cx, unsigned char cy)
{
	// set the controller chip data to match the required column and row
	column = cx;
	if (cx>60)
		cx -= 60;
	lcd_control(cx);					// set the column in both controllers
	lcd_control((cy & 3) | 0xb8);
	return;
}

void lcd_write(char data)
{
//	- writes arg0 to the currently selected column of the display
//	- decides which display to write to by looking at 'column' but
//	- does not preset the chips' internal column counters - these
//	- are set by the routine lcd_set_column before writing starts.
//	- the chips update their internal column counter after each
//	- write so sequential writes will be to sequential columns but
//	- this routine does not adjust 'column'

	// set the data pins for output
	DDRC = 0xff;
	
	if (column == 60)
		// select column 1
		lcd_control(0);
		
	// else set e1, e2, rw low, a0 and elcnt high
	PORTA = _BV(elcnt) | _BV(a0);
	
	if (column < 60)
		// we're in controller 1
		PORTA += _BV(e1);
	else
		// we're in controller 2
		PORTA += _BV(e2);
	// for some reason the above does not properly compile with |= instead of +=
	// latch the data
	PORTC = data;
	
	// and clear the enables again
	PORTA &= ~(_BV(e1) | _BV(e2));
	return;
}

void lcd_init(void)
{
	// reset the display and clear it, for both controllers
	
	// first, set the control port and turn on the backlight
	DDRA = 0xff;
	PORTA = _BV(elcnt);			
	
	// cursor to top left of window
	lcd_set_cursor(0,0);
	
	// display off
	lcd_control(0xae);
	
	// startup delay of 100ms
	for (int q=0; q<4; q++)
		_delay_loop_2(50000);
	
	// start line = 0
	lcd_control(0xc0);
	
	// static driving off
	lcd_control(0xa4);
	
	// column = 0
	lcd_control(0);
	
	// page = 0
	lcd_control(0xb8);
	
	// 1/32 duty
	lcd_control(0xa9);
	
	// output bits forwards
	lcd_control(0xa0);
	
	// ensure not in read-modify-write mode
	lcd_control(0xee);
	
	// clear screen
	lcd_cls();
	
	lcd_control(0xaf);
	
	return;
}

void lcd_cls (void)
{
	for (int r=0; r<4; r++)
	{
		lcd_set_cursor(0,r);
		for (int q=0; q<80; q++)
			lcd_char(' ');
	}
	lcd_set_cursor(0,0);
}

void lcd_char (char ch)
{

uint16_t ptr;

	// write ch to the screen at the current cursor position
	// update the cursor position and scroll to the next line
	// if required
	// to avoid scrolling the screen if we need to use 19,3 the 
	// cursor is moved to the top of the screen again
	// when we arrive, the cursor is already set
	
	// we set the cursor to where we already are, which sets up the column
	lcd_set_cursor(cursorx, cursory);		

	// now we work out where in the font array the character lives, get the five bytes
	// that define the character, and stuff them to the display, followed by a zero for space
	// the characters start with 0x20
	ptr = (uint16_t)(ch - 0x20)*5;
	for (int q = 0; q < 5; q++)
	{
		lcd_write(pgm_read_byte(&font[ptr++]));
		column++;
	}
	lcd_write(0);
	
	// increment the cursor to the next place
	cursorx++;
	if (cursorx == 20)
	{
		cursorx = 0;
		cursory++;
	}
	if (cursory == 4)
		cursory = 0;
		
}

void lcd_print (char * text)
{
int q;

	// print a string to the lcd
	for (q=0; q<strlen(text); q++)
	{
		lcd_char(text[q]);
	}
}
