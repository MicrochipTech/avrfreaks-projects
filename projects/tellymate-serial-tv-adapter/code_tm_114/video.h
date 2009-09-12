/*
Copyright (c) 2009, Nigel Batten.
Contactable at <firstname>.<lastname>@mail.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

	1.	Redistributions of source code must retain the above copyright
		notice, this list of conditions and the following disclaimer.
	2.	Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.

===============================================
*/

#ifndef _VIDEO_H__
#define _VIDEO_H__

#include "font.h"
#include "video_sync.h"
#include "video_render.h"
#include "video_serial.h"
#include "delay_x.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_REVISION 14
#define _QUOTEME(x) #x
#define QUOTEME(x) _QUOTEME(x)
#define VERSION QUOTEME(VERSION_MAJOR) "." QUOTEME(VERSION_MINOR) "." QUOTEME(VERSION_REVISION)

#define ROW_COUNT 25
#define COL_COUNT 38

// Scanline definitions:
// SCANLINE_FIRST_DISPLAY
// 	determines which scanline we're on when the V-Sync routine has finished.
// SCANLINE_LAST_DISPLAY
//	determines when the V-Sync routine should start
// SCANLINE_FIRST_TEXTSAFE
//	determines where the first line of text should start being drawn
//  This may need to be adjusted to ensure that the text area appears
//  vertically centred. It will depend on your TV to some degree.

// PAL has the following scanlines:
// 1-5		broad v-sync pulses and short-pulse post-equalisation
// 6-23		'blanked' scanlines (with normal HSync)
// 24		first displayable line
// 52		top line of text
// 281		last line of text (TODO: CHECK THIS)
// 309		last visible scanline
// 310-312	short-pulse pre-equalisation

// NTSC:
// Has the whole Vertical blanking period at the start
// of the field/frame:
// scanline
// 1-9    v-sync pulses
// 10-20  'blanked' scanlines (with normal HSync)
// 21     first displayable line
// 262    last displayable line


#define SCANLINE_FIRST_DISPLAY_PAL 6
#define SCANLINE_LAST_DISPLAY_PAL 309
#define SCANLINE_FIRST_TEXTSAFE_PAL 52
#define SCANLINE_FIRST_DISPLAY_NTSC 10
#define SCANLINE_LAST_DISPLAY_NTSC 262
#define SCANLINE_FIRST_TEXTSAFE_NTSC 30

// SCANLINE_FIRST_TEXTSAFE_NTSC: Technically, it should be 29, but on
// my monitor, that's the very first line that appears (and is
// actually chopped off when used Picture-in-picture).



// times, cycles and clock settings for various points in a scanline...
#define CYCLES_PER_US		(F_CPU / 1000000)

// Note that the NTSC timing is not quite 'correct', but it produces
// a nicely divisible-by-two figure of 1016 cycles per scanline.
#define TIME_SCANLINE_PAL	64
#define TIME_SCANLINE_NTSC	63.5

#define TIME_HALFSCANLINE_PAL	(TIME_SCANLINE_PAL / 2)
#define TIME_HALFSCANLINE_NTSC	(TIME_SCANLINE_NTSC / 2)

#define TIME_HSYNC			4
#define TIME_SHORTSYNC		2

// NTSC has a smaller back-porch than PAL
// It's perfectly centred (on my monitor) with 11.5, but that doesn't quite leave enough time
// for processing, so I'm resigned to it being a fraction too far to the right.
#define TIME_DISPLAYSTART_PAL 12.5
#define TIME_DISPLAYSTART_NTSC 11.75


#define CYCLES_HSYNC		(TIME_HSYNC * CYCLES_PER_US)
#define CYCLES_SHORTSYNC	(TIME_SHORTSYNC * CYCLES_PER_US)
#define CYCLES_SCANLINE_PAL			(TIME_SCANLINE_PAL * CYCLES_PER_US)
#define CYCLES_HALFSCANLINE_PAL 	(TIME_HALFSCANLINE_PAL * CYCLES_PER_US)
#define CYCLES_DISPLAYSTART_PAL		(TIME_DISPLAYSTART_PAL * CYCLES_PER_US)
#define CYCLES_SCANLINE_NTSC		(TIME_SCANLINE_NTSC * CYCLES_PER_US)
#define CYCLES_HALFSCANLINE_NTSC	(TIME_HALFSCANLINE_NTSC * CYCLES_PER_US)
#define CYCLES_DISPLAYSTART_NTSC	(TIME_DISPLAYSTART_NTSC * CYCLES_PER_US)

#define CLOCK_HSYNC			(CYCLES_HSYNC - 1 )
#define CLOCK_SHORTSYNC		(TIME_SHORTSYNC - 1 )
#define CLOCK_SCANLINE_PAL		(CYCLES_SCANLINE_PAL - 1 )
#define CLOCK_HALFSCANLINE_PAL	(CYCLES_HALFSCANLINE_PAL - 1 )
#define CLOCK_DISPLAYSTART_PAL	(CYCLES_DISPLAYSTART_PAL - 1 )
#define CLOCK_LONGSYNC_PAL		((CYCLES_HALFSCANLINE_PAL - CYCLES_HSYNC) - 1 )
#define CLOCK_SCANLINE_NTSC		(CYCLES_SCANLINE_NTSC - 1 )
#define CLOCK_HALFSCANLINE_NTSC	(CYCLES_HALFSCANLINE_NTSC - 1 )
#define CLOCK_DISPLAYSTART_NTSC	(CYCLES_DISPLAYSTART_NTSC - 1 )
#define CLOCK_LONGSYNC_NTSC		((CYCLES_HALFSCANLINE_NTSC - CYCLES_HSYNC) - 1 )


#define SIG_SYNC_PIN 1
#define SIG_PIXEL_PIN 3
#define SPI_SS 2
#define SPI_SCK 5
#define SPI_MISO 4


#define SYNC_ON (PORTB &= (~(1<<SIG_SYNC_PIN)))
#define SYNC_OFF (PORTB |= (1<<SIG_SYNC_PIN))
#define PIXEL_ENABLE  DDRB = ((1<<SIG_PIXEL_PIN)|(1<<SIG_SYNC_PIN))
#define PIXEL_DISABLE DDRB = ((1<<SIG_SYNC_PIN))


// Note: The OPT flags don't correspond directly to the option jumpers.
// The option jumpers are read at start-up, and some may be mapped to bits
// in the options byte.

#define OPT_LINEOVERFLOW 0
#define OPT_HIDECURSOR 1
#define OPT_BLOCKCURSOR 2
#define OPT_AUTO_LF 3
#define OPT_AUTO_CR 4
#define OPT_SUPPRESSOUTPUT 5
#define OPT_HIDEDISPLAY 6
#define OPT_PAL 7

// The following OPT flags are in the top 3 bits of the framecount byte.
#define OPT_ALTCURSORADVANCE 5
#define OPT_RAMFONTENABLE 6
#define OPT_UNUSED 7

#define ROWINFO_DISPLAY 0
#define ROWINFO_DOUBLE_HEIGHT 1
#define ROWINFO_DOUBLE_HEIGHT_BOTTOM 2
#define ROWINFO_DOUBLE_WIDTH 3
// bits 4,5,6 and 7 are used to define which fontbank is in use for the row.


// definition of the CYCLES macro.
// This is to wrap the _delay_cycles() macro in some code that will cause the
// compiler/linker to fail if a negative parameter is supplied.
// The optimiser removes the call to the (non-existent) ERROR_NEGATIVE_DELAY function if the delay is >= 0
void ERROR_NEGATIVE_DELAY( void ) ;
#define CYCLES(n) do { if (n < 0) ERROR_NEGATIVE_DELAY(); else _delay_cycles(n); }while(0)

// sticky plaster to define PB0 to PB7 for the M328P
// (I'm assuming that the fact they're missing from iom328p.h is an error, rather than a quiet way of renaming the aeons old naming conventions)
#if defined( _AVR_IOM328P_H_ ) && (!defined(PB0))
#define PB7     7
#define PB6     6
#define PB5     5
#define PB4     4
#define PB3     3
#define PB2     2
#define PB1     1
#define PB0     0
#endif

#if defined( _AVR_IOMX8_H_ ) || defined( _AVR_IOM328P_H_ )
// This is a Mega88,168 or 328p
#define TIMER1FLAGS TIFR1
#define TIMER1INTERRUPTS TIMSK1
#define USART0_BAUD_H UBRR0H
#define USART0_BAUD_L UBRR0L
#define USART0_REG_A UCSR0A
#define USART0_REG_B UCSR0B
#define USART0_BIT_RXC RXC0
#define USART0_BIT_TXEN  TXEN0
#define USART0_DATA  UDR0

#elif defined( _AVR_IOM8_H_ )
// This is a Mega8
#define TIMER1FLAGS TIFR
#define TIMER1INTERRUPTS TIMSK
#define USART0_BAUD_H UBRRH
#define USART0_BAUD_L UBRRL
#define USART0_REG_A UCSRA
#define USART0_REG_B UCSRB
#define USART0_BIT_RXC RXC
#define USART0_BIT_TXEN TXEN
#define USART0_DATA UDR

#else
#error Only Mega8, Mega88, Mega168 and Mega328p processors are supported
#endif

// Global variables that have been shunted into GPIO (or otherwise unused registers).
//extern uint8_t g_char;				// Most recently received character
//extern uint8_t g_current_row;			// Current cursor row position
//extern uint8_t g_current_col;			// Current cursor column position
//extern uint16_t g_current_scanline;	// Current scanline. NOTE: 16-bit!


#if defined( _AVR_IOMX8_H_ ) || defined( _AVR_IOM328P_H_ )
#define g_current_scanline	_SFR_IO16(0x2A) // This consumes GPIOR1 AND GPIOR2
#define g_char				GPIOR0
#define g_current_row		TCNT0
#define	g_current_col		EEDR
#else
#define g_current_scanline	_SFR_IO16(0x23) // This consumes OCR2 and TCNT2
#define g_char				TWBR
#define g_current_row		TCNT0
#define g_current_col		EEDR
#endif

extern uint8_t g_row_scroll_offset;   // Which row should be displayed at the top. This is to enable simple scrolling.
extern uint8_t g_options;			// various options (note: not directly mapped to jumpers)
extern uint8_t g_framecount;		// used to blink the cursor. Simply wraps around after 256 frames

extern uint8_t g_display[ROW_COUNT][COL_COUNT] ;
extern uint8_t g_rowinfo[ROW_COUNT];


#endif
