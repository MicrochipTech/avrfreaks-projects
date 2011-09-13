///////////////////////////////////////////////////////////////////////////////
//	Low pin count seven-segment display. Source file.
//	Version 1.0
//		(c) 2011 sashman@gmail.com
///////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lpc7seg.h"
#include "misc.h"
#define S7_OR			_BV(S7LATCH_PIN)
///////////////////////////////////////////////////////////////////////////////
uint8_t s7fbuf[S7N];	//frame buffer. bits equivalent to display segments
uint8_t s7cbuf[S7N]="123456789AB";	//character buffer. contains ASCII characters
uint8_t s7brightness = S7_DELAY_US;	// controls brightness
///////////////////////////////////////////////////////////////////////////////
#define S7FONT_FIRST '0'	// first character of s7font
PROGMEM uint8_t s7font[] = 
{ 
	0x3F, 0x06, 0x5B, // 012
	0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,	//3456789
	0x09, 0x09, 0x61, 0x41, 0x43, 0x4B, 0x5F,	//:;<=>?@
	0x77, 0x7C, 0x58, 0x5E, 0x79, 0x71, 0x3D, 	//AbcdeFG
	0x74, 0x10, 0x0E, 0x70, 0x38, 0x49, 0x54,	//hiJkLmn
	0x5C, 0x73, 0x67, 0x50, 0x6D, 0x78, 0x3e,	//oPqrStU
	0x62, 0x6A, 0x76, 0x6E, 0x5B  				//vwXYZ
};
///////////////////////////////////////////////////////////////////////////////
void sevenseg_refresh()
{
	uint8_t c,s,d; //characters, segments, delay

	for(s=0; s<S7SEGS; s++)
	{
		S7PORT = S7_MASK & (S7_OR | s);
		clear(S7LATCH);
		set(S7LATCH);

		for(c=0; c<S7N; c++)
		{
			S7PORT = S7_MASK & (S7_OR|c);
			if(s7fbuf[c] & _BV(s) && s7brightness) 
				set(S7GLOW);

			//note that for consistent, data-independent performance, 
			//the delay is still incurred, even if no segments are to be lit

			for(d=0; d<s7brightness; d++)
				_delay_us(1); // may as well call some application code here
			clear(S7GLOW);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
void sevenseg_update_fbuf()
{
	uint8_t c, i;
	for(i = 0; i<S7N; i++)
	{
		c = s7cbuf[i] - S7FONT_FIRST;
		s7fbuf[i] = (c>sizeof(s7font))?0: pgm_read_byte(s7font+c);
	}
}
///////////////////////////////////////////////////////////////////////////////
void sevenseg_init()
{
	DDRB=0xff;
	out(S7LATCH);
	out(S7GLOW);
}
///////////////////////////////////////////////////////////////////////////////
void sevenseg_puthex(uint32_t value)
{
	uint8_t i, x;
	for(i=0; i<S7N; i++)
	{
		x=0x0f & value; 
		value>>=4;
		if(x>10) x+=10;
		s7fbuf[i] = s7font[x];
	}
}
///////////////////////////////////////////////////////////////////////////////
