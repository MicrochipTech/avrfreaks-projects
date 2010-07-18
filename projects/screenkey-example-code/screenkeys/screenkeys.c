/*
 * This file establishes basic communication with a ScreenKeys configurable
 * LCD display/switch.  See <http://www.screenkeys.com>.  The routines here
 * should work with both the LC and RGB series as well as both size displays.
 *
 * An 8-bit timer on the AVR is used to contorl the clock generator.  If we
 * look at the extreme limits of the frequencies involved, an 8-bit counter
 * should be enough to handle the task.
 *   1Mhz AVR + 50KHz ScreenKey Clock = 1,000,000 / 50,000 / 2 = 10 counts
 *   20MHz AVR + 4MHz ScreenKey = 20/4/2 = 3 counts
 *   20MHz AVR + 50KHz Screenkey = 20,000,000 / 50,000 /2 = 200 counts
 * In reality, it looks like 80KHz ScreenKey clock is about the best that can
 * be done with 8MHz CPU, 190KHz with 20MHz, and the slowest CPU clock is about
 * 5MHz to get the minimum 50KHz ScreenKey clock.
 *
 * This code is not maintained and is free to use with no rights reserved, and
 * no guarantees made.
 *
 * Code is based on examples provided by Sparkfun:
 *  <http://www.sparkfun.com/datasheets/Components/screenkeys_example.zip>
 *  <http://forum.sparkfun.com/viewtopic.php?t=8312&start=30>
 *
 * Uses - Timer0, 2 outputs, 1 input.
 *
 * Tom Blough 7/20/2008
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "screenkeys.h"
#include "utility.h"

volatile uint16_t bitData;  // bit buffer for 12-bit synchronous data packet


/////////////////////////////////////////////////
//                PUBLIC INTERFACE             //
/////////////////////////////////////////////////

/*
 * Setup the physical connections and establish communications with
 * the ScreenKey configurable pushbutton.
 */
void sk_init()
{
	SK_CLK( DDR) = OUT;  // set SK clock pin to output
	SK_DATA( DDR) = OUT;  // set SK data pin to output
	SK_DATA( PORT) = HIGH;	// set data pin high

	// setup 8-bit timer for ScreenKey clock generator
	TCCR0A |= (1<<WGM01);  // CTC mode
	TCCR0B |= (1<<CS00);  // internal clock on, no prescale
	OCR0A = (F_CPU / SK_CLK_FREQ / 2);  // set matchA count
	TIMSK0 |= (1<<OCIE0A);  // enable CTC interrupt on matchA
	sei();  // enable global interrupts

	// setup frequency divider and multiplex values
	send_cmd( SK_SET_FREQ_REG, SK_FREQ_REG);
	send_cmd( SK_SET_MUX, SK_MUX_DATA);
}

/*
 * Write the button image data (or a portion of it) to the ScreenKey
 */
void sk_set_pixels( uint8_t data[], uint8_t count, uint8_t offset)
{
	if( offset + count > SK_IMAGE_BYTES) return;

	write_byte( SK_START_BYTE, SK_START_PARITY);
	write_byte( SK_SET_PIXEL + offset, SK_CMD_PARITY);

	for( uint8_t i = 0; i < count; i++)
		write_byte( data[i], SK_DATA_PARITY);

	write_byte( SK_END_BYTE, SK_END_PARITY);
}

/*
 * Set the ScreenKey background color
 */
void sk_set_color( uint8_t color)
{
	send_cmd( SK_SET_COLOR, color);
}

#ifdef SK_TEXT_SUPPORT
/*
 * Clears a line of text
 */
void sk_clear_text( uint8_t line)
{
	uint8_t clear[SK_IMAGE_WIDTH] = {0x00};

	sk_set_pixels( clear, SK_IMAGE_WIDTH, line);
}

/*
 * Convert the text characters into a bitmap array and send to the ScreenKey
 * display area.  The bitmap array will need manipulation because the LC16
 * display starts at the upper left, while the LC24 display starts at the
 * upper right.
 */
void sk_set_text( char* text, uint8_t justify, uint8_t line)
{
	uint8_t textBuffer[SK_IMAGE_WIDTH] = {0x00}, dataBuffer[SK_IMAGE_WIDTH];
	uint8_t data, count, *ptr;

	// adjust string length if necessary. count is number of characters in
	// string or SK_MAX_TEXT characters - whichever is fewer.
	for( count = 0, ptr = (uint8_t*)text; *ptr && (count < SK_MAX_TEXT); count++, ptr++);

	// justify text by moving down appropiate number of columns in our text buffer
	if( justify == SK_TEXT_CENTER)
		ptr = textBuffer + ((SK_IMAGE_WIDTH - (count * 6)) / 2);
	else if( justify == SK_TEXT_LEFT)
		ptr = textBuffer + (SK_IMAGE_WIDTH - (count * 6));
	else
		ptr = textBuffer;

	// now fill buffer with characters starting from end of line
	// moving towards beginnning
	for( uint8_t chr = count; chr > 0; chr--)
	{
		// each character is 5 columns
		for( uint8_t col = 0; col < 5; col++)
		{
			// for each column, index into the character array (offsetting for the
			// 32 control characters we've excluded to save space), and grab
			// the corresponding 8-pixel row bitmap.
			*ptr++ = charMap[(uint8_t)(text[chr - 1]) - 0x20][col];
		}
		ptr++;  // add a blank column between characters
	}
	
	// convert the line of text into image bytes in another buffer. we'll then
	// send the buffer to the ScreenKey to improve the refresh speed over
	// creating and then sending one byte at a time. the tradeoff is that
	// this uses an additional 24/36 bytes for the buffer. some guru
	// might be able to figure out how to do this conversion in-place in the
	// text buffer without needing an additional buffer.
	count = 0;  // current bit/row position
	data = 0x00;
	ptr = dataBuffer;

	// for each of the 8 rows of pixels in the line of text
	for( uint8_t row = 8; row > 0; row--)
	{
	#ifdef SK_LCD24  // LCD24 display (36*24) with 0,0 in the upper right
		// for each of the 24 or 36 columns in the line of text
		for( uint8_t col = 0; col < SK_IMAGE_WIDTH; col++, count++)
		{
			if( count == 8)  // finished building a byte?
			{
				*ptr++ = data;  // queue it up
				data = 0x00;  // and reset to build another
				count = 0;
			}
			// set the next bit in our data byte
			data |= (((textBuffer[col] & (1<<(row - 1)))>>(row - 1))<<count);
		}
	#else  // LCD16 display (32*16) with 0,0 in the upper left 
		for( uint8_t col = SK_IMAGE_WIDTH; col > 0 ; col--, count++)
		{
			if( count == 8)
			{
				*ptr++ = data;
				data = 0x00;
				count = 0;
			}
			data |= (((textBuffer[col - 1] & (1<<(row - 1)))>>(row - 1))<<count);
		}
	#endif
	}
	*ptr = data;  // queue up the last byte

	// finally, send it to the ScreenKey for display
	sk_set_pixels( dataBuffer, SK_IMAGE_WIDTH, line);
}
#endif // SK_TEXT_SUPPORT


/////////////////////////////////////////////////
//         PRIVATE HELPER FUNCTIONS            //
/////////////////////////////////////////////////

/*
 * Timer ISR to generate clock signal and sychronize data transmission.  All
 * data transitions occur on the rising edge of the clock, and the data line
 * is to remain high when not transmitting.  Since last two bits (stop bits)
 * of every data packet transmitted are high, we do not have to actively do
 * anything to make the data line high after transmitting data.
 *
 * We want to keep this as short as possible.  At 8MHz uP clock and 50KHz
 * ScreenKey clock, we only have 80 clock ticks between interrupts.
 */
ISR( TIMER0_COMPA_vect)
{
	// start the next half of the clock waveform
	SK_CLK( PORT) = ~SK_CLK( PORT);
	
	if( SK_CLK( PORT)) // are we on rising edge?
	{
		if( bitData) // do we have data?
		{
			// set the data output pin to correspond
			// to the current bit0 of the data buffer
			SK_DATA( PORT) = (bitData & 0x01);
			// advance to the next bit - unsigned shift will fill with 0s
			// bitData will be == 0 when stop bits have finished transmitting
			bitData >>= 1;
		}
	}
}

/*
 * Prepares a byte to be sent to the ScreenKey
 * Parity is 1 for odd parity, 0 for even
 */
void write_byte( uint8_t data, uint8_t parity)
{
	uint16_t tmp;
	uint8_t pCalc;

	// calculate parity
	// lsb of pd keeps track of unmatched 1 bits which is then XORed with
	// desired parity type.  Needed parity bit is left in lsb
	/*pCalc = (data ^ (data >> 1) ^ (data >> 2) ^ (data >> 3)
				^ (data >> 4) ^ (data >> 5) ^ (data >> 6) ^ (data >> 7)
				^ parity) & 0x01;*/

	// this parity algorithm saves a few bytes over the one above.
	// from "compute parity in parallel" bit twiddling hacks by Sean Anderson
	// http://graphics.stanford.edu/~seander/bithacks.html
	// needed parity bit is left in lsb.
	pCalc = data;
	pCalc ^= pCalc >> 4;
	pCalc &= 0x0f;
	pCalc = ((0x6996 >> pCalc) ^ parity) & 0x01;


	// build our data packet
	//      not used
	//      |   stop bits
	//      |   | parity bit
	//      |   | |data (msb-lsb)
	//      |   | ||       start bit
	//      <--><>|<------>|
	//      xxxx11pdddddddd0      parity          data
	tmp = 0b0000110000000000 | (pCalc << 9) | (data << 1);

	// queue our data for transmission
	// since no transmit buffer, we have to wait until previous data is done
	// and since data is two bytes, we have to make sure no clock pulse happens
	// between the bytes loading.
	while( bitData);
	ATOMIC_BLOCK( ATOMIC_RESTORESTATE)
	{
		bitData = tmp;
	}
}

/*
 * Send a command to the ScreenKey
 */
void send_cmd( uint8_t cmd, uint16_t data)
{
	write_byte( SK_START_BYTE, SK_START_PARITY);
	write_byte( cmd, SK_CMD_PARITY);

	if( cmd == SK_SET_MUX)  // we only have one command requiring 2 bytes of data
		write_byte( (uint8_t)(data>>8), SK_DATA_PARITY);

	write_byte( (uint8_t)data, SK_DATA_PARITY);
	write_byte( SK_END_BYTE, SK_END_PARITY);
}

/********************************************
This turned out to be an acadaemic exercise.  Since we need to specify the frequency
to generate our clock, it's just as easy to also look up the value for the divider
and define it at the same time.  This ends up saving 354 bytes.  This function might
be useful if one wished to implement a contrast adjustment for the screenkey, but I
doubt such an adjustment would have a practical benefit.

uint8_t freq_reg_value( uint32_t frequency)
{
	// This approximates the lookup table on page 15 in the RGB24 datasheet.
	// The table is function of log10( frequency), but the lookup procedure
	// below actually has a smaller footprint and is faster than a curve fit.
	//
	// Datasheet table is split at 499.2KHz.  If table frequencies of 499.2KHz
	// and above are divided by 10 they almost correspond 1:1 with the first
	// half of the table.  We us this characteristic to reduce the lookup
	// table size by half.  Further, we can divide the frequencies by 1/2
	// and still retain sufficient precision, but we reduce the size of the
	// values so they fit within 8-bits. 
	uint8_t freqLookup[] = { 25, 27, 28, 30, 31, 33, 34, 36, 37, 39, 41, 42, 44, 45,
							47, 48, 50, 53, 56, 59, 62, 66, 69, 72, 75, 78, 81, 84,
							87, 91, 94, 97, 100, 106, 112, 119, 125, 131, 137, 144,
							150, 156, 162, 169, 175, 181, 187, 193, 200, 212, 225, 237 };

	uint16_t lookup = frequency / 1000 / 2;  // convert to KHz and divide by 2
	uint8_t value = 0;  // the resulting frequency value from the table

	for( uint8_t i = 0; ; i++, value += 2)
	{
		// if we reach the end of the lookup list, fold back table to beginning
		if( i >= sizeof( freqLookup) / sizeof( freqLookup[0]))
		{
			i = 0;
			lookup /= 10;
		}
		if( lookup <= freqLookup[i]) return value;
	}
}
*************************************************/

#ifdef SK_TEXT_SUPPORT
/* Each character is encoded into 5 bytes. These 40 bits create
 * a 5x8 character cell rotated 90 degrees counterclockwise with
 * one bit (pixel) of vertical space between rows of characters.
 * For example:
 *
 *    'B' {0x36,0x49,0x49,0x49,0x7F}
 *  
 *       MSB      LSB
 *    B[0] --**-**-
 *    B[1] -*--*--*
 *    B[2] -*--*--*
 *    B[3] -*--*--*
 *    B[4] -*******
 *
 *    'g' {0x1E,0x15,0x15,0x15,0x08}
 *  
 *       MSB      LSB
 *    B[0] ---****-
 *    B[1] ---*-*-*
 *    B[2] ---*-*-*
 *    B[3] ---*-*-*
 *    B[4] ----*---
 *
 * Therefore, an LC24 with a 36*24 display should be able to display
 * 3 rows of text each consisting of 6 characters with one column of 
 * pixels between characters.  A LC16 display (32x16) will display
 * 2 rows of 5 character text with one column of pixels between chars.
 *
 * The first 32 ASCII control characters are not included in this table
 * since they have no use on the ScreenKey, so be sure to offset by
 * 0x20 when indexing into the table.
 *
 * This table can be further reduced to save data space if, for example,
 * only numbers and upper case letters were needed.
 */
static const uint8_t charMap[96][5] = {
	{ 0x00,0x00,0x00,0x00,0x00 },  // 032  Space
	{ 0x00,0x00,0x79,0x00,0x00 },  // 033  !
	{ 0x00,0x70,0x00,0x70,0x00 },  // 034  "
	{ 0x14,0x7F,0x14,0x7F,0x14 },  // 035  Hash mark
	{ 0x24,0x2A,0x7F,0x2A,0x12 },  // 036  $
	{ 0x23,0x13,0x08,0x64,0x62 },  // 037  %
	{ 0x05,0x22,0x55,0x49,0x36 },  // 038  &
	{ 0x00,0x00,0x60,0x50,0x00 },  // 039  '
	{ 0x00,0x41,0x22,0x1c,0x00 },  // 040  (
	{ 0x00,0x1c,0x22,0x41,0x00 },  // 041  )
	{ 0x14,0x08,0x3E,0x08,0x14 },  // 042  *
	{ 0x08,0x08,0x3E,0x08,0x08 },  // 043  +
	{ 0x00,0x00,0x06,0x05,0x00 },  // 044  ,
	{ 0x08,0x08,0x08,0x08,0x08 },  // 045  -
	{ 0x00,0x00,0x03,0x03,0x00 },  // 046  .
	{ 0x20,0x10,0x08,0x04,0x02 },  // 047  forward slash
	{ 0x3E,0x51,0x49,0x45,0x3E },  // 048  0
	{ 0x00,0x01,0x7f,0x21,0x00 },  // 049  1
	{ 0x31,0x49,0x45,0x43,0x21 },  // 050  2
	{ 0x46,0x69,0x51,0x41,0x42 },  // 051  3
	{ 0x04,0x7F,0x24,0x14,0x0C },  // 052  4
	{ 0x4E,0x51,0x51,0x51,0x72 },  // 053  5
	{ 0x06,0x49,0x49,0x29,0x1E },  // 054  6
	{ 0x60,0x50,0x48,0x47,0x40 },  // 055  7
	{ 0x36,0x49,0x49,0x49,0x36 },  // 056  8
	{ 0x3C,0x4A,0x49,0x49,0x30 },  // 057  9
	{ 0x00,0x00,0x36,0x36,0x00 },  // 058  :
	{ 0x00,0x00,0x36,0x35,0x00 },  // 059  ;
	{ 0x00,0x41,0x22,0x14,0x08 },  // 060  <
	{ 0x14,0x14,0x14,0x14,0x14 },  // 061  =
	{ 0x08,0x14,0x22,0x41,0x00 },  // 062  >
	{ 0x30,0x48,0x45,0x40,0x20 },  // 063  ?
	{ 0x3C,0x55,0x5D,0x41,0x3E },  // 064  @
	{ 0x3F,0x48,0x48,0x48,0x3F },  // 065  A
	{ 0x36,0x49,0x49,0x49,0x7F },  // 066  B
	{ 0x22,0x41,0x41,0x41,0x3E },  // 067  C
	{ 0x1C,0x22,0x41,0x41,0x7F },  // 068  D
	{ 0x41,0x49,0x49,0x49,0x7F },  // 069  E
	{ 0x40,0x48,0x48,0x48,0x7F },  // 070  F
	{ 0x2F,0x49,0x49,0x41,0x3E },  // 071  G
	{ 0x7F,0x08,0x08,0x08,0x7F },  // 072  H
	{ 0x00,0x41,0x7f,0x41,0x00 },  // 073  I
	{ 0x40,0x7E,0x41,0x01,0x02 },  // 074  J
	{ 0x41,0x22,0x14,0x08,0x7F },  // 075  K
	{ 0x01,0x01,0x01,0x01,0x7F },  // 076  L
	{ 0x7F,0x20,0x18,0x20,0x7F },  // 077  M
	{ 0x7F,0x04,0x08,0x10,0x7F },  // 078  N
	{ 0x3E,0x41,0x41,0x41,0x3E },  // 079  O
	{ 0x30,0x48,0x48,0x48,0x7F },  // 080  P
	{ 0x3D,0x42,0x45,0x41,0x3E },  // 081  Q
	{ 0x31,0x4A,0x4C,0x48,0x7F },  // 082  R
	{ 0x26,0x49,0x49,0x49,0x32 },  // 083  S
	{ 0x40,0x40,0x7F,0x40,0x40 },  // 084  T
	{ 0x7F,0x01,0x01,0x01,0x7F },  // 085  U
	{ 0x7C,0x02,0x01,0x02,0x7C },  // 086  V
	{ 0x7E,0x01,0x0E,0x01,0x7E },  // 087  W
	{ 0x63,0x14,0x08,0x14,0x63 },  // 088  X
	{ 0x70,0x08,0x07,0x08,0x70 },  // 089  Y
	{ 0x61,0x51,0x49,0x45,0x43 },  // 090  Z
	{ 0x00,0x44,0x41,0x7f,0x00 },  // 091  [
	{ 0x02,0x04,0x08,0x10,0x20 },  // 092  back slash
	{ 0x00,0x7f,0x41,0x41,0x00 },  // 093  ]
	{ 0x10,0x20,0x40,0x20,0x10 },  // 094  ^
	{ 0x01,0x01,0x01,0x01,0x01 },  // 095  _
	{ 0x00,0x50,0x60,0x00,0x00 },  // 096  `
	{ 0x0F,0x15,0x15,0x15,0x02 },  // 097  a
	{ 0x0E,0x11,0x11,0x11,0x7F },  // 098  b
	{ 0x11,0x11,0x11,0x11,0x0E },  // 099  c
	{ 0x7F,0x11,0x11,0x11,0x0E },  // 100  d
	{ 0x0C,0x15,0x15,0x15,0x0E },  // 101  e
	{ 0x50,0x50,0x3F,0x10,0x10 },  // 102  f
	{ 0x1E,0x15,0x15,0x15,0x08 },  // 103  g
	{ 0x0F,0x10,0x10,0x08,0x7F },  // 104  h
	{ 0x00,0x01,0x5f,0x11,0x00 },  // 105  i
	{ 0x00,0x5e,0x11,0x01,0x02 },  // 106  j
	{ 0x00,0x11,0x0a,0x04,0x7f },  // 107  k
	{ 0x00,0x01,0x7f,0x41,0x00 },  // 108  l
	{ 0x0F,0x10,0x1F,0x10,0x1F },  // 109  m
	{ 0x0F,0x10,0x10,0x08,0x1F },  // 110  n
	{ 0x0E,0x11,0x11,0x11,0x0E },  // 111  o
	{ 0x08,0x14,0x14,0x14,0x1F },  // 112  p
	{ 0x1F,0x14,0x14,0x14,0x08 },  // 113  q
	{ 0x00,0x10,0x10,0x08,0x1f },  // 114  r
	{ 0x12,0x15,0x15,0x15,0x09 },  // 115  s
	{ 0x11,0x11,0x7E,0x10,0x10 },  // 116  t
	{ 0x1F,0x02,0x01,0x01,0x1E },  // 117  u
	{ 0x1C,0x02,0x01,0x02,0x1C },  // 118  v
	{ 0x1E,0x01,0x06,0x01,0x1E },  // 119  w
	{ 0x11,0x0A,0x04,0x0A,0x11 },  // 120  x
	{ 0x1E,0x05,0x05,0x05,0x18 },  // 121  y
	{ 0x11,0x19,0x15,0x13,0x11 },  // 122  z
	{ 0x00,0x41,0x41,0x36,0x08 },  // 123  {
	{ 0x00,0x00,0x77,0x00,0x00 },  // 124  |
	{ 0x08,0x36,0x41,0x41,0x00 },  // 125  }
	{ 0x08,0x1C,0x2A,0x08,0x08 },  // 126  ~
	{ 0x08,0x08,0x2A,0x1C,0x08 }   // 127  left arrow	 
};
#endif // SK_TEXT_SUPPORT

