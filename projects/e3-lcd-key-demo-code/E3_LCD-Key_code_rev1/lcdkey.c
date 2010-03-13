/*
 * This file contains parameters to set up SPI and routines for formatting and sending
 * commands and data to the E3 LCD-Key Part SB6432 configurable
 * LCD display/switches.  See <http://www.e3-keys.com/>.
 * This code was developed on the Atmel mega168.
 *
 * This code is not maintained and is free to use with no rights reserved, and
 * no guarantees made.
 *
 * Code is based on ScreenKey code developed by Tom Blough at AVR Freaks <http://www.avrfreaks.net>
 *
 * Uses - SPI
 *
 * Jim Minihane 3/7/2010
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include "e3_lcdkey.h"

#define F_CPU 8000000L // CPU clock

/////////////////////////////////////////////////
//                PUBLIC INTERFACE             //
/////////////////////////////////////////////////

/*
 * Setup the physical connections and establish communications with
 * the LCDKey configurable pushbutton.
 */
void e3_init()
{

/*
 * SPI I/O Pin Definitions
 */
	// SS Pin Configure as input, enable pullup
	DDRB &= ~_BV(DDB2);
	PORTB |= _BV(PORTB2); // However, if this pin is pulled LOW, the SPI will shift to Slave mode.

	//SCK - Set as output
	DDRB |= _BV(DDB5);

	//MOSI - Set as output
	DDRB |= _BV(DDB3);

	//MISO - Not required for just sending data to the LCD-Key.

	SPCR = _BV(SPE)+_BV(MSTR)+_BV(CPOL)+_BV(SPR0);
		// SPE Enables SPI
		// MSTR sets Master Mode
		// CPOL sets Clock Polarity such that the clock line is high during inactivity
		// SPR0 select clock divider = 16. 8MHz clock / 16 = 500 kHz (See SPI2X below)

	SPSR = _BV(SPI2X);
		// Doubles SPI Clock rate --> 500kHz * 2 = 1 MHz

	// The LCD-Key SB6432 is capable up to 2 MHz via SPI, however, my development hardware
	// was incapable of speed in excess of 1 MHz. Drawing a full line of text requires 5 bytes to be sent per vertical
	// line over the SPI connection.
	// This ends up equating to 320 total SPI bytes sent per line. At 1 MHz, this takes 2.56 mS.

}

/*
 * Write the image/pixel data (or a portion of it) to the LCD-Key
 */
void e3_set_pixels( uint8_t data[], uint8_t array_bytes, uint16_t line_address)
{
	//if( offset + count > E3_IMAGE_BYTES) return;

	for( uint8_t i = 0; i < array_bytes; i++)
	{
		write_byte( E3_SET_ADDR );
		write_address( line_address - (i * 8UL) );
		write_data( data[i] );
	}

}

/*
 * Set the LCD-Key background color
 */
void e3_set_color( uint8_t red_byte, uint8_t green_byte, uint8_t blue_byte)
{
	write_byte( E3_SET_RGB_COLOR );	//initiates extended color command
	write_byte( red_byte ); 		//sets red LED value
	write_byte( green_byte ); 		//sets green LED value
	write_byte( blue_byte ); 		//sets blue LED value
}

#ifdef E3_GRAPH_SUPPORT
/*
 * Create a bitmap array for a horizontal bar graph 1 line high
 * and send to the LCD-Key display area.
 */
void e3_bar_graph( uint8_t bar_value, uint16_t line)
{
	uint8_t barBuffer[E3_IMAGE_WIDTH] = {0x00};
	uint8_t *ptr;

	if ( bar_value > 60 ) return; // 60 is the maximum number of vertical elements in the bar value display

	ptr = barBuffer;

	// now fill buffer with bar elements starting from the left and
	// moving towards the right.

	for( uint8_t col = 0; col < E3_IMAGE_WIDTH; col++)
	{
			if(( col == 0 ) || ( col == ( E3_IMAGE_WIDTH - 1 )))
			{
				*ptr++ = ( E3_BG_VERT ); // First and last vertical line
			}
			else if (( col == 1 ) || (col == ( E3_IMAGE_WIDTH - 2 )))
			{
				*ptr++ = ( E3_BG_GAP ); // Leading and trailing bar gaps
			}
			else
			{
				if( col < bar_value )
				{
					*ptr++ = ( E3_BG_BAR ); // Bar graph element
				}
				else
				{
					*ptr++ = ( E3_BG_GAP ); // trailing blank bar
				}
			}
	}

	// finally, send it to the LCD-Key for display

	e3_set_pixels( barBuffer, E3_IMAGE_WIDTH, line);

}
#endif

#ifdef E3_CLEAR_PIXELS
/*
 * Clears a line of text
 */
void e3_clear_line( uint16_t line)
{
	uint8_t clear[E3_IMAGE_WIDTH] = {0x00};

	e3_set_pixels( clear, E3_IMAGE_WIDTH, line);

}

/*
 * Sets the entire screen. 0x00 = clear, 0xFF = black, This is faster than clearing 4 separate lines.
 * If speed isn't really important, you can comment out this routine.
 */
void e3_set_scr( uint8_t data_byte)
{

	write_byte( E3_SET_ADDR ); 
	write_address(0x0000); 
	for( uint16_t row = 0; row < E3_IMAGE_BYTES; row++)
	{
		write_data( data_byte ); // write 256 bytes to the LCD-Key
	}
	//write_byte( E3_END_BYTE );
	

}

#endif // E3_CLEAR_PIXELS

#ifdef E3_TEXT_SUPPORT
/*
 * Convert the text characters into a bitmap array and send to the LCD-Key
 * display area on a specific line. It replaces the entire line.
 */
void e3_set_text_line( char* text, uint8_t justify, uint16_t line)
{
	uint8_t textBuffer[E3_IMAGE_WIDTH] = {0x00};
	uint8_t count, *ptr;

	// adjust string length if necessary. count is number of characters in
	// string or E3_MAX_TEXT characters - whichever is fewer.
	for( count = 0, ptr = (uint8_t*)text; *ptr && (count < E3_MAX_TEXT); count++, ptr++);



	// justify text by moving down appropiate number of columns in our text buffer
	if( justify == E3_TEXT_CENTER)
		ptr = textBuffer + ((E3_IMAGE_WIDTH - (count * 6)) >> 1); // Use right shift of 1 bit to divide by 2 quickly.
	else if( justify == E3_TEXT_RIGHT)
		ptr = textBuffer + (E3_IMAGE_WIDTH - (count * 6));
	else
		ptr = textBuffer;

	// now fill buffer with characters starting from the start of the line
	// moving towards the end.

	for( uint8_t chr = 0; chr < count; chr++)
	{
		// each character is 5 columns
		for( uint8_t col = 0; col < 5; col++)
		{
			// for each column, index into the character array (offsetting for the
			// 32 ( or 46) control characters we've excluded to save space), and grab
			// the corresponding 8-pixel row bitmap.

		  #if E3_TEXT_CHAR == 96
			*ptr++ = pgm_read_byte(&( charMap[(uint8_t)( text[chr] ) - 0x20][col] )); // Full 96 charcter array
		  #else
		    *ptr++ = pgm_read_byte(&( charMap[(uint8_t)( text[chr] ) - 0x2E][col] )); // 45 charcter array
		  #endif // E3_TEXT_CHAR

		}
		ptr++;  // add a blank column between characters
	}

	// finally, send it to the LCD-Key for display

	e3_set_pixels( textBuffer, E3_IMAGE_WIDTH, line);

}

/*
 * Convert the text characters into a bitmap array and send to the LCD-Key
 * display area in a specific line beginning at a specific column in the line
 * for only as many characters as required to display the text.
 */

void e3_set_text_loc( char* text, uint8_t mode_option, uint8_t exact_length_char, uint8_t justify, uint16_t line, uint8_t text_loc)
{
	
	uint8_t array_bytes;

	if(mode_option == E3_TEXT_FULL)
	{
		array_bytes = E3_IMAGE_WIDTH;
	}
	else
	{
		array_bytes = (exact_length_char * 6);
		line = (line - ((text_loc-1) * 8));
	}
	
		uint8_t textBuffer[E3_IMAGE_WIDTH] = {0x00};
		uint8_t count, *ptr;

		// adjust string length if necessary. count is number of characters in
		// string or E3_MAX_TEXT characters - whichever is fewer.
		for( count = 0, ptr = (uint8_t*)text; *ptr && (count < E3_MAX_TEXT); count++, ptr++);



		// justify text by moving down appropiate number of columns in our text buffer
		if( justify == E3_TEXT_CENTER)
			ptr = textBuffer + ((array_bytes - (count * 6)) >> 1); // Use right shift of 1 bit to divide by 2 quickly.
		else if( justify == E3_TEXT_RIGHT)
			ptr = textBuffer + (array_bytes - (count * 6));
		else
			ptr = textBuffer;

		// now fill buffer with characters starting from the start of the line
		// moving towards the end.

		for( uint8_t chr = 0; chr < count; chr++)
		{
			// each character is 5 columns
			for( uint8_t col = 0; col < 5; col++)
			{
				// for each column, index into the character array (offsetting for the
				// 32 ( or 46) control characters we've excluded to save space), and grab
				// the corresponding 8-pixel row bitmap.

			  #if E3_TEXT_CHAR == 96
				*ptr++ = pgm_read_byte(&( charMap[(uint8_t)( text[chr] ) - 0x20][col] )); // Full 96 charcter array
			  #else
			    *ptr++ = pgm_read_byte(&( charMap[(uint8_t)( text[chr] ) - 0x2E][col] )); // 45 charcter array
			  #endif // E3_TEXT_CHAR

			}
			ptr++;  // add a blank column between characters
		}

		// finally, send it to the LCD-Key for display

	e3_set_pixels( textBuffer, array_bytes, line );
}

#endif // E3_TEXT_SUPPORT


#ifdef E3_IMAGE_SUPPORT
void e3_set_image( uint8_t image_index)
{
	write_byte( E3_SET_ADDR ); 
	write_address(0x0000); 
	for( uint16_t row = 0; row < E3_IMAGE_BYTES; row++)
	{
		write_data(pgm_read_byte(&( imageMap[row][image_index] ))); // index through bytes in the image array
	}

}
#endif

/////////////////////////////////////////////////
//         PRIVATE HELPER FUNCTIONS            //
/////////////////////////////////////////////////


/*
 * Writes a byte to the SPI Data Register be sent to the LCD-Key.
 */
void write_byte( uint8_t data) //
{
	SPDR = data; // Write byte to SPI Data Register
	while(!( SPSR & _BV(SPIF) )); // wait for SPI to finish sending.
}

/*
 * Writes a byte to the LCD-Key as 2 nibbles
 */
void write_data( uint8_t data) //
{
	write_byte( data & 0b00001111 ); // Use bit mask to only send the LSB nibble first
	write_byte( data >> 4 ); // Right shit 4 places to write the MSB nibble last.
}

/*
 * Writes an address given as a uint16_t value.
 * The LCD-Key only needs the 3 least significant nibbles
 */
void write_address( uint16_t addr_data)
{
	write_byte(( addr_data >> 8 ) & 0b00001111 ); // Right shift given value 8 places and use a bit mask to send the MSB nibble first.
	write_byte(( addr_data >> 4 ) & 0b00001111 ); // Right shift given value 4 places and use a bit mask to send the middle nibble second.
	write_byte(  addr_data & 0b00001111 ); // Use a bit mask to send the LSB nibble last.
}


#ifdef E3_TEXT_SUPPORT
/* Each character is encoded into 5 bytes. These 40 bits create
 * a 5x8 character cell rotated 90 degrees counterclockwise with
 * one bit (pixel) of vertical space between rows of characters.
 * For example:
 *
 *    'B' { 0xFE,0x92,0x92,0x92,0x6C }
 *
 *       MSB      LSB
 *    B[4] --**-**- 0b00110110 = 0x6C
 *    B[3] -*--*--* 0b01001001 = 0x92
 *    B[2] -*--*--* 0b01001001 = 0x92
 *    B[1] -*--*--* 0b01001001 = 0x92
 *    B[0] -******* 0b01111111 = 0xFE
 *
 *    'g' { 0x10,0xA8,0xA8,0xA8,0x78 }
 *
 *       MSB      LSB
 *    B[4] ---****- 0b00011110 = 0x78
 *    B[3] ---*-*-* 0b00010101 = 0xA8
 *    B[2] ---*-*-* 0b00010101 = 0xA8
 *    B[1] ---*-*-* 0b00010101 = 0xA8
 *    B[0] ----*--- 0b00001000 = 0x10
 *
 * The first 32 ASCII control characters are not included in this table
 * since they have no use on the LCD-Key, so be sure to offset by
 * 0x20 when indexing into the table.
 *
 * This table can be further reduced to save data space if, for example,
 * only numbers and upper case letters were needed. If so use the SubSet
 * table and offset by 0x2E.
 */


#if E3_TEXT_CHAR == 96
// Character array for E3 LCDKey
 static const uint8_t charMap[96][5] PROGMEM = {
	{ 0x00,0x00,0x00,0x00,0x00 },  // 032  Space
	{ 0x00,0x00,0x9E,0x00,0x00 },  // 033  !
	{ 0x00,0x0E,0x00,0x0E,0x00 },  // 034  "
	{ 0x28,0xFE,0x28,0xFE,0x28 },  // 035  Hash mark
	{ 0x48,0x54,0xFE,0x54,0x24 },  // 036  $
	{ 0x46,0x26,0x10,0xC8,0xC4 },  // 037  %
	{ 0x6C,0x92,0xAA,0x44,0xA0 },  // 038  &
	{ 0x00,0x0A,0x06,0x00,0x00 },  // 039  '
	{ 0x00,0x38,0x44,0x82,0x00 },  // 040  (
	{ 0x00,0x82,0x44,0x38,0x00 },  // 041  )
	{ 0x28,0x10,0x7C,0x10,0x28 },  // 042  *
	{ 0x10,0x10,0x7C,0x10,0x10 },  // 043  +
	{ 0x00,0xA0,0x60,0x00,0x00 },  // 044  ,
	{ 0x10,0x10,0x10,0x10,0x10 },  // 045  -
	{ 0x00,0xC0,0xC0,0x00,0x00 },  // 046  .
	{ 0x40,0x20,0x10,0x08,0x04 },  // 047  forward slash
	{ 0x7C,0xA2,0x92,0x8A,0x7C },  // 048  0
	{ 0x00,0x84,0xFE,0x80,0x00 },  // 049  1
	{ 0x84,0xC2,0xA2,0x92,0x8C },  // 050  2
	{ 0x42,0x82,0x8A,0x96,0x62 },  // 051  3
	{ 0x30,0x28,0x24,0xFE,0x20 },  // 052  4
	{ 0x4E,0x8A,0x8A,0x8A,0x72 },  // 053  5
	{ 0x78,0x94,0x92,0x92,0x60 },  // 054  6
	{ 0x02,0xE2,0x12,0x0A,0x06 },  // 055  7
	{ 0x6C,0x92,0x92,0x92,0x6C },  // 056  8
	{ 0x0C,0x92,0x92,0x52,0x3C },  // 057  9
	{ 0x00,0x6C,0x6C,0x00,0x00 },  // 058  :
	{ 0x00,0xAC,0x6C,0x00,0x00 },  // 059  ;
	{ 0x10,0x28,0x44,0x82,0x00 },  // 060  <
	{ 0x28,0x28,0x28,0x28,0x28 },  // 061  =
	{ 0x00,0x82,0x44,0x28,0x10 },  // 062  >
	{ 0x04,0x02,0xA2,0x12,0x0C },  // 063  ?
	{ 0x7C,0x82,0xBA,0xAA,0x3C },  // 064  @
	{ 0xFC,0x12,0x12,0x12,0xFC },  // 065  A
	{ 0xFE,0x92,0x92,0x92,0x6C },  // 066  B
	{ 0x7C,0x82,0x82,0x82,0x44 },  // 067  C
	{ 0xFE,0x82,0x82,0x44,0x38 },  // 068  D
	{ 0xFE,0x92,0x92,0x92,0x82 },  // 069  E
	{ 0xFE,0x12,0x12,0x12,0x02 },  // 070  F
	{ 0x7C,0x82,0x92,0x92,0xF4 },  // 071  G
	{ 0xFE,0x10,0x10,0x10,0xFE },  // 072  H
	{ 0x00,0x82,0xFE,0x82,0x00 },  // 073  I
	{ 0x40,0x80,0x82,0x7E,0x02 },  // 074  J
	{ 0xFE,0x10,0x28,0x44,0x82 },  // 075  K
	{ 0xFE,0x80,0x80,0x80,0x80 },  // 076  L
	{ 0xFE,0x04,0x18,0x04,0xFE },  // 077  M
	{ 0xFE,0x08,0x10,0x20,0xFE },  // 078  N
	{ 0x7C,0x82,0x82,0x82,0x7C },  // 079  O
	{ 0xFE,0x12,0x12,0x12,0x0C },  // 080  P
	{ 0x7C,0x82,0xA2,0x42,0xBC },  // 081  Q
	{ 0xFE,0x12,0x32,0x52,0x8C },  // 082  R
	{ 0x4C,0x92,0x92,0x92,0x64 },  // 083  S
	{ 0x02,0x02,0xFE,0x02,0x02 },  // 084  T
	{ 0xFE,0x80,0x80,0x80,0xFE },  // 085  U
	{ 0x3E,0x40,0x80,0x40,0x3E },  // 086  V
	{ 0x7E,0x80,0x70,0x80,0x7E },  // 087  W
	{ 0xC6,0x28,0x10,0x28,0xC6 },  // 088  X
	{ 0x0E,0x10,0xE0,0x10,0x0E },  // 089  Y
	{ 0xC2,0xA2,0x92,0x8A,0x86 },  // 090  Z
	{ 0x00,0xFE,0x82,0x22,0x00 },  // 091  [
	{ 0x04,0x08,0x10,0x20,0x40 },  // 092  back slash
	{ 0x00,0x82,0x82,0xFE,0x00 },  // 093  ]
	{ 0x08,0x04,0x02,0x04,0x08 },  // 094  ^
	{ 0x80,0x80,0x80,0x80,0x80 },  // 095  _
	{ 0x00,0x00,0x06,0x0A,0x00 },  // 096  `
	{ 0x40,0xA8,0xA8,0xA8,0xF0 },  // 097  a
	{ 0xFE,0x88,0x88,0x88,0x70 },  // 098  b
	{ 0x70,0x88,0x88,0x88,0x88 },  // 099  c
	{ 0x70,0x88,0x88,0x88,0xFE },  // 100  d
	{ 0x70,0xA8,0xA8,0xA8,0x30 },  // 101  e
	{ 0x08,0x08,0xFC,0x0A,0x0A },  // 102  f
	{ 0x10,0xA8,0xA8,0xA8,0x78 },  // 103  g
	{ 0xFE,0x10,0x08,0x08,0xF0 },  // 104  h
	{ 0x00,0x88,0xFA,0x80,0x00 },  // 105  i
	{ 0x40,0x80,0x88,0x7A,0x00 },  // 106  j
	{ 0xFE,0x20,0x50,0x88,0x00 },  // 107  k
	{ 0x00,0x82,0xFE,0x80,0x00 },  // 108  l
	{ 0xF8,0x08,0xF8,0x08,0xF0 },  // 109  m
	{ 0xF8,0x10,0x08,0x08,0xF0 },  // 110  n
	{ 0x70,0x88,0x88,0x88,0x70 },  // 111  o
	{ 0xF8,0x28,0x28,0x28,0x10 },  // 112  p
	{ 0x10,0x28,0x28,0x28,0xF8 },  // 113  q
	{ 0xF8,0x10,0x08,0x08,0x00 },  // 114  r
	{ 0x90,0xA8,0xA8,0xA8,0x48 },  // 115  s
	{ 0x08,0x08,0x7E,0x88,0x88 },  // 116  t
	{ 0x78,0x80,0x80,0x40,0xF8 },  // 117  u
	{ 0x38,0x40,0x80,0x40,0x38 },  // 118  v
	{ 0x78,0x80,0x60,0x80,0x78 },  // 119  w
	{ 0x88,0x50,0x20,0x50,0x88 },  // 120  x
	{ 0x18,0xA0,0xA0,0xA0,0x78 },  // 121  y
	{ 0x88,0xC8,0xA8,0x98,0x88 },  // 122  z
	{ 0x10,0x6C,0x82,0x82,0x00 },  // 123  {
	{ 0x00,0x00,0xEE,0x00,0x00 },  // 124  |
	{ 0x00,0x82,0x82,0x6C,0x10 },  // 125  }
	{ 0x10,0x10,0x54,0x38,0x10 },  // 126  ~
	{ 0x10,0x38,0x54,0x10,0x10 }   // 127  left arrow
 };

 #else
// Character array Subset for E3 LCDKey
 static const uint8_t charMap[45][5] PROGMEM = {
	{ 0x00,0xC0,0xC0,0x00,0x00 },  // 046  .
	{ 0x40,0x20,0x10,0x08,0x04 },  // 047  forward slash
	{ 0x7C,0xA2,0x92,0x8A,0x7C },  // 048  0
	{ 0x00,0x84,0xFE,0x80,0x00 },  // 049  1
	{ 0x84,0xC2,0xA2,0x92,0x8C },  // 050  2
	{ 0x42,0x82,0x8A,0x96,0x62 },  // 051  3
	{ 0x30,0x28,0x24,0xFE,0x20 },  // 052  4
	{ 0x4E,0x8A,0x8A,0x8A,0x72 },  // 053  5
	{ 0x78,0x94,0x92,0x92,0x60 },  // 054  6
	{ 0x02,0xE2,0x12,0x0A,0x06 },  // 055  7
	{ 0x6C,0x92,0x92,0x92,0x6C },  // 056  8
	{ 0x0C,0x92,0x92,0x52,0x3C },  // 057  9
	{ 0x00,0x6C,0x6C,0x00,0x00 },  // 058  :
	{ 0x00,0xAC,0x6C,0x00,0x00 },  // 059  ;
	{ 0x10,0x28,0x44,0x82,0x00 },  // 060  <
	{ 0x28,0x28,0x28,0x28,0x28 },  // 061  =
	{ 0x00,0x82,0x44,0x28,0x10 },  // 062  >
	{ 0x04,0x02,0xA2,0x12,0x0C },  // 063  ?
	{ 0x7C,0x82,0xBA,0xAA,0x3C },  // 064  @
	{ 0xFC,0x12,0x12,0x12,0xFC },  // 065  A
	{ 0xFE,0x92,0x92,0x92,0x6C },  // 066  B
	{ 0x7C,0x82,0x82,0x82,0x44 },  // 067  C
	{ 0xFE,0x82,0x82,0x44,0x38 },  // 068  D
	{ 0xFE,0x92,0x92,0x92,0x82 },  // 069  E
	{ 0xFE,0x12,0x12,0x12,0x02 },  // 070  F
	{ 0x7C,0x82,0x92,0x92,0xF4 },  // 071  G
	{ 0xFE,0x10,0x10,0x10,0xFE },  // 072  H
	{ 0x00,0x82,0xFE,0x82,0x00 },  // 073  I
	{ 0x40,0x80,0x82,0x7E,0x02 },  // 074  J
	{ 0xFE,0x10,0x28,0x44,0x82 },  // 075  K
	{ 0xFE,0x80,0x80,0x80,0x80 },  // 076  L
	{ 0xFE,0x04,0x18,0x04,0xFE },  // 077  M
	{ 0xFE,0x08,0x10,0x20,0xFE },  // 078  N
	{ 0x7C,0x82,0x82,0x82,0x7C },  // 079  O
	{ 0xFE,0x12,0x12,0x12,0x0C },  // 080  P
	{ 0x7C,0x82,0xA2,0x42,0xBC },  // 081  Q
	{ 0xFE,0x12,0x32,0x52,0x8C },  // 082  R
	{ 0x4C,0x92,0x92,0x92,0x64 },  // 083  S
	{ 0x02,0x02,0xFE,0x02,0x02 },  // 084  T
	{ 0xFE,0x80,0x80,0x80,0xFE },  // 085  U
	{ 0x3E,0x40,0x80,0x40,0x3E },  // 086  V
	{ 0x7E,0x80,0x70,0x80,0x7E },  // 087  W
	{ 0xC6,0x28,0x10,0x28,0xC6 },  // 088  X
	{ 0x0E,0x10,0xE0,0x10,0x0E },  // 089  Y
	{ 0xC2,0xA2,0x92,0x8A,0x86 }   // 090  Z
 };
 #endif // E3_TEXT_CHAR

#endif // E3_TEXT_SUPPORT

#ifdef E3_IMAGE_SUPPORT
/*
 * Array of bytes to map a complete image to the display
 * Each "column" in the array contains the bytes for a single image
 * In the below example, column 0 contains the byes for a full screen "prohibited" symbol.
 * You can add columns to this array if you want, just increase the width of the array accordingly.
 * Index 0 = Prohibited Symbol
 * Index 1 = Flashlight
 */
static const uint8_t imageMap[E3_IMAGE_BYTES][E3_IMAGE_COUNT] PROGMEM = {
{ 0b00000000, 0b01000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b10000001},
{ 0b00000000, 0b01000000},
{ 0b00000000, 0b00010000},
{ 0b00000000, 0b00000100},
{ 0b00000000, 0b10000001},
{ 0b00000000, 0b01000000},
{ 0b00000000, 0b00010000},
{ 0b00000000, 0b00000100},
{ 0b00000000, 0b10000001},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00100000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b10000010},
{ 0b00000000, 0b00100000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b10000010},
{ 0b00000000, 0b00100000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b10000010},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000100},
{ 0b00000000, 0b00010000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000100},
{ 0b00000000, 0b00010000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000011, 0b00010000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b11000000, 0b10000100},
{ 0b00011111, 0b00010000},
{ 0b00000000, 0b00000010},
{ 0b00000000, 0b00100000},
{ 0b11111000, 0b10000100},
{ 0b01111111, 0b00010000},
{ 0b00000000, 0b00000010},
{ 0b00000000, 0b00100000},
{ 0b11111110, 0b10000100},
{ 0b11111111, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b11111111, 0b00000000},
{ 0b11111111, 0b00000000},
{ 0b00000001, 0b00000000},
{ 0b10000000, 0b00000000},
{ 0b11111111, 0b00000000},
{ 0b11111111, 0b00000000},
{ 0b00000011, 0b00000001},
{ 0b11000000, 0b01000000},
{ 0b11111111, 0b00000000},
{ 0b11111111, 0b00001000},
{ 0b00000111, 0b00000001},
{ 0b11100000, 0b01000000},
{ 0b11111111, 0b10001000},
{ 0b11111111, 0b00001000},
{ 0b00001111, 0b00000000},
{ 0b11110000, 0b00000000},
{ 0b11111111, 0b10001000},
{ 0b11111111, 0b00001000},
{ 0b00001111, 0b00000000},
{ 0b11110000, 0b00000000},
{ 0b11111111, 0b10001000},
{ 0b11111100, 0b10000000},
{ 0b00011111, 0b00000000},
{ 0b11111000, 0b10000000},
{ 0b00111111, 0b00000000},
{ 0b11110000, 0b10000000},
{ 0b00111111, 0b00000000},
{ 0b11111100, 0b10000000},
{ 0b00001111, 0b00000000},
{ 0b11100000, 0b00000000},
{ 0b00111111, 0b00000000},
{ 0b11111100, 0b00000000},
{ 0b00000111, 0b00000000},
{ 0b11110000, 0b00000100},
{ 0b00111111, 0b00000000},
{ 0b11111100, 0b00000000},
{ 0b00000011, 0b10010000},
{ 0b11111000, 0b01000100},
{ 0b01111111, 0b00000000},
{ 0b11111110, 0b00000000},
{ 0b00000001, 0b10010001},
{ 0b11111100, 0b01000100},
{ 0b01111111, 0b00000000},
{ 0b11111110, 0b00000000},
{ 0b00000000, 0b10010001},
{ 0b11111100, 0b00000000},
{ 0b01111111, 0b00000000},
{ 0b11111110, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b11111110, 0b11111111},
{ 0b01111111, 0b00000001},
{ 0b01111110, 0b11000000},
{ 0b00000000, 0b11111111},
{ 0b11111111, 0b00000000},
{ 0b11111111, 0b00000001},
{ 0b01111111, 0b01000000},
{ 0b00000000, 0b00000000},
{ 0b11111111, 0b00000000},
{ 0b11111110, 0b00000001},
{ 0b01111111, 0b01000000},
{ 0b10000000, 0b00000000},
{ 0b11111111, 0b00000000},
{ 0b11111110, 0b00000001},
{ 0b01111111, 0b01000000},
{ 0b11000000, 0b00000000},
{ 0b01111111, 0b10000000},
{ 0b11111100, 0b00000000},
{ 0b00111111, 0b10000000},
{ 0b11000000, 0b00000000},
{ 0b00111111, 0b01000000},
{ 0b11111100, 0b00000000},
{ 0b00111111, 0b00000000},
{ 0b11100000, 0b00000001},
{ 0b00011111, 0b00100000},
{ 0b11111100, 0b00000000},
{ 0b00111111, 0b00000000},
{ 0b11110000, 0b00000010},
{ 0b00011111, 0b00010000},
{ 0b11111100, 0b00000000},
{ 0b00111111, 0b00000000},
{ 0b11111000, 0b00000100},
{ 0b00001111, 0b00001111},
{ 0b11111100, 0b00000000},
{ 0b00111111, 0b00000000},
{ 0b11111100, 0b11111000},
{ 0b00000111, 0b00001000},
{ 0b11111110, 0b00000000},
{ 0b01111111, 0b00000000},
{ 0b11111100, 0b00001000},
{ 0b00000011, 0b00001000},
{ 0b11111110, 0b00000000},
{ 0b01111111, 0b00000000},
{ 0b11111110, 0b00001000},
{ 0b00000001, 0b00001000},
{ 0b11111110, 0b00000000},
{ 0b01111111, 0b00000000},
{ 0b11111111, 0b00001000},
{ 0b00000001, 0b00001000},
{ 0b01111111, 0b00000000},
{ 0b11111110, 0b00000000},
{ 0b11111111, 0b00001000},
{ 0b00000000, 0b00001000},
{ 0b01111111, 0b00000000},
{ 0b11111110, 0b00000000},
{ 0b11111111, 0b00001000},
{ 0b00000000, 0b00001000},
{ 0b01111111, 0b00000000},
{ 0b11111110, 0b00000000},
{ 0b01111111, 0b00001000},
{ 0b10000000, 0b00001000},
{ 0b01111111, 0b00000000},
{ 0b11111110, 0b00000000},
{ 0b00111111, 0b00001000},
{ 0b11000000, 0b00001000},
{ 0b00111111, 0b00000000},
{ 0b11111100, 0b00000000},
{ 0b00011111, 0b00001000},
{ 0b11100000, 0b00001000},
{ 0b00111111, 0b00000000},
{ 0b11111100, 0b00000000},
{ 0b00011111, 0b00001000},
{ 0b11110000, 0b00001000},
{ 0b00111111, 0b00000000},
{ 0b11111100, 0b00000000},
{ 0b00001111, 0b00001000},
{ 0b11111100, 0b00001000},
{ 0b00011111, 0b00000000},
{ 0b11111000, 0b00000000},
{ 0b00111111, 0b00001000},
{ 0b11111111, 0b00001000},
{ 0b00001111, 0b00000000},
{ 0b11110000, 0b00000000},
{ 0b11111111, 0b00001000},
{ 0b11111111, 0b00001000},
{ 0b00001111, 0b00000000},
{ 0b11110000, 0b00000000},
{ 0b11111111, 0b00001000},
{ 0b11111111, 0b00001000},
{ 0b00000111, 0b00000000},
{ 0b11100000, 0b00000000},
{ 0b11111111, 0b00001000},
{ 0b11111111, 0b00001000},
{ 0b00000011, 0b00000000},
{ 0b11000000, 0b00000000},
{ 0b11111111, 0b00001000},
{ 0b11111111, 0b00001000},
{ 0b00000001, 0b00000000},
{ 0b10000000, 0b00000000},
{ 0b11111111, 0b00001000},
{ 0b11111111, 0b00001000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b11111111, 0b00001000},
{ 0b01111111, 0b00001000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b11111110, 0b00001000},
{ 0b00011111, 0b00001000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b11111000, 0b00001000},
{ 0b00000011, 0b00001000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b11000000, 0b00001000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00001000},
{ 0b00000000, 0b00000111},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b11110000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000},
{ 0b00000000, 0b00000000}
};

#endif // E3_IMAGE_SUPPORT
