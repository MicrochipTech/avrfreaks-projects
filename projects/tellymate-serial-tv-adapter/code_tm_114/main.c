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

TellyMate Serial to Video adapter
=================================
	38x25 character display
	PAL or NTSC composite video output
	Up to 57k6 serial data.
	Autobauding option.
	VT52 escape codes.
	H19 escape codes.
	Double-height / Double-width text.
	Code Page 437 graphics
	White text on black background
	Reprogrammable Fonts


Hardware
========
	An AVR Mega8 / Mega88 / Mega168 / Mega328p running at 16Mhz

	Input
	-----
	TTL Serial data on RXD and ICP1 pins
	
	Output
	------
	!Sync_signal on PB1
	Pixel_signal on MOSI (PB3)
	
	A couple of diodes and a voltage divider is
	required to get the 1v peak-to-peak video signal.

	J6 determines wether a PAL or NTSC signal is output.
	No jumper = NTSC, jumpered = PAL.


Software
========
	Timing in the software is very tight.
	It is suggested that the supplied makefile is used.
	The make file has various optimisation settings that are needed for correct output.
	It also supplies the location of the 256-byte-aligned ".font" memory section to the linker.

	Note: As from v1.0.6, the meaning of J6 changes.
	Before v1.0.6, J6 was "Automatic Carriage Return when a Line Feed is received"
	v1.0.6 and later, J6 is 'PAL'. No jumper = NTSC, Jumpered = PAL.

Notes
=====
	Hans-Juergen Heinrichs' delay_x header file is used to provide cycle-accurate delays.
	It was developed using WinAVR-20080512.
	It now works with WinAVR-20090313.

	See http://www.batsocks.co.uk/products/Other/TellyMate.htm for further details.
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "delay_x.h"
#include "video.h"

static void init( void ) ;
static void display_clear( void ) ;
static void init_options( void ) ;
static uint16_t	uart_Autobaud( void );
static uint16_t uart_GetUBBR(void);

int __attribute__((OS_main)) main( void )
{
#if defined(  _AVR_IOMX8_H_  ) || defined( _AVR_IOM328P_H_ )
// M88, M168 and M328p need the watchdog timeout flag clearing
	MCUSR &= ~(1<<WDRF);
#endif
	wdt_disable();
	
	init();
	display_clear();

	g_current_scanline = 0 ;
	g_char = 0 ;
	g_current_row = 0 ;
	g_current_col = 0 ;
	g_row_scroll_offset = 0;
	
	// start the timers from zero...
	TCNT1 = 0;
	
	sei();

	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable(); // only needs doing once (against advice).
	while(1){
		sleep_cpu(); // now go back to sleep until the next scanline starts
		SYNC_ON;
		g_current_scanline++ ;
		g_sync_handler();
		g_render_handler();
	}
}

static inline void init_options()
{
	// read the (non baud-rate) options from the jumpers...
	// (baud-rate settings are read in the main init function)
	uint8_t opt = 0;
	if (!(PINC & (1<<3))) opt |= (1<<OPT_LINEOVERFLOW) ;
	if (!(PINC & (1<<4))) opt |= (1<<OPT_AUTO_LF) ;
	if (!(PINC & (1<<5))) opt |= (1<<OPT_PAL) ;
	g_options = opt ;
}

static inline void display_clear( void )
{
	for ( uint8_t row = 0 ; row < ROW_COUNT ; row++ )
	{
		g_rowinfo[row] = 0;
	}
}


#if defined(  _AVR_IOMX8_H_  ) || defined( _AVR_IOM328P_H_ )
// USART initialisation for M88, M168 and M328p
#define INIT_USART()\
do {\
	UCSR0B = (1<< RXEN0);\
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);\
} while(0) /* Enable receive, 8-bit, no parity, one stop bit */
#elif defined( _AVR_IOM8_H_ )
// USART initialisation for M8
#define INIT_USART()\
do {\
	UCSRB = (1 << RXEN);\
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);\
} while(0) /* Enable receive, 8-bit, no parity, one stop bit */
#endif


static inline void init( void )
{

	DDRC = 0x00; // all inputs
	PORTC = 0xff ; //all pulled-up

	// setup the two signal pins (sync and pixel) as outputs, and the input capture pin (PB0) as an input.
	DDRB = (1 << SIG_SYNC_PIN) | (1 << SIG_PIXEL_PIN) | (1 << SPI_SS) | (1 << SPI_SCK) | (0 << PB0);
	PORTB |= SPI_MISO;
	PORTB |= SPI_SS;
	PORTB |= (1<<PB0);
	
	CYCLES(10);
	
	// read the jumper settings.
	// This is now called from here, as J6 (PAL/NTSC) needs to be known.
	init_options();
	
	
	// setup the serial module...
	uint16_t ubbr = uart_GetUBBR();
	
	USART0_BAUD_L = ubbr & 0xff ;
	USART0_BAUD_H = (ubbr >> 8) & 0xff ;
	INIT_USART();
	
	// setup the 16-bit timer to give a scanline interrupt...
	TCCR1A = ((0<<WGM11)|(0<<WGM10));
	TCCR1B = ((0<<WGM13)|(1<<WGM12)|(1<<CS10)); // CTC, with TOP of OCR1A... (using 1:1 clock)
	if (g_options & (1 << OPT_PAL))
	{
		OCR1A = CLOCK_SCANLINE_PAL ;
	}
	else
	{
		OCR1A = CLOCK_SCANLINE_NTSC ;
	}
	
	// the second interrupt is set to around 12.5us in...
	if (g_options & (1<< OPT_PAL))
	{
		OCR1B = CLOCK_DISPLAYSTART_PAL ;
	}
	else
	{
		OCR1B = CLOCK_DISPLAYSTART_NTSC ;
	}
	
	TCNT1 = 0;

	// and initialise the scanline interrupts...
	TIMER1INTERRUPTS = (1<<OCIE1A);
	TIMER1INTERRUPTS |= (1<<OCIE1B);

	// set the SPI to be as fast as possible...
	SPCR = 0 ;
	SPCR = (1 << SPE)|(1 << MSTR); // master mode, enabled
	SPSR = (1 << SPI2X); // as fast as possible - F_CPU / 2.
	// as soon as interrupts are enabled the scanline interrupt will be firing...
	
}

// interrupt for the "12.5us through a scanline" compare match
// This could only be made quicker by inserting a reti directly into the interrupt vector table.
ISR( TIMER1_COMPB_vect, ISR_NAKED)
{
	asm("reti");
}

// interrupt for the start/end of scanline compare match.
// Again, this could only be made quicker (by 2c) by inserting a reti directly into the interrupt vector table.
ISR( TIMER1_COMPA_vect, ISR_NAKED)
{
	asm("reti");
}


uint16_t uart_GetUBBR(void)
{
uint16_t UBBRValue = 0;

/*
	Port C (pull-ups are on - jumpers pull to ground)
	3 2 1
	====
	0 0 0 57k6
	0 0 1 38k4
	0 1 0 19k2
	0 1 1 9600
	1 0 0 4800
	1 0 1 1200
	1 1 0 300
	1 1 1 Autobaud
*/

#define BAUD_TO_UBBR( n ) ((F_CPU/(n<<4))-1) 
#define BAUD_UBBR_300	BAUD_TO_UBBR( 300UL )
#define BAUD_UBBR_1200	BAUD_TO_UBBR( 1200UL )
#define BAUD_UBBR_2400	BAUD_TO_UBBR( 2400UL )
#define BAUD_UBBR_4800	BAUD_TO_UBBR( 4800UL )
#define BAUD_UBBR_9600	BAUD_TO_UBBR( 9600UL )
#define BAUD_UBBR_14400	BAUD_TO_UBBR( 14400UL )
#define BAUD_UBBR_19200	BAUD_TO_UBBR( 19200UL )
#define BAUD_UBBR_28800	BAUD_TO_UBBR( 28800UL )
#define BAUD_UBBR_38400	BAUD_TO_UBBR( 38400UL )
#define BAUD_UBBR_57600	BAUD_TO_UBBR( 57600UL )
#define BAUD_UBBR_76800	BAUD_TO_UBBR( 76800UL )
#define BAUD_UBBR_115200 BAUD_TO_UBBR( 115200UL )
#define BAUD_AUTO   0

	static uint16_t PROGMEM baud_ubbr_lookup[] =
		{	BAUD_UBBR_57600,
			BAUD_UBBR_38400,
			BAUD_UBBR_19200,
			BAUD_UBBR_9600,
			BAUD_UBBR_4800,
			BAUD_UBBR_1200,
			BAUD_UBBR_300,
			BAUD_AUTO
		};
	// Get BAUD rate jumper settings
	UBBRValue = pgm_read_word( &(baud_ubbr_lookup[ PINC & 0b111 ] )) ;
	
	if (UBBRValue == BAUD_AUTO)
	{
		UBBRValue = uart_Autobaud();
	}
	return UBBRValue;
}		 


/*
To save stack space, I'm 'overloading' some existing variables for use during autobauding.
It's not pretty, but it does save enough stack bytes to prevent overflowing.

The following global variables are 're-used'. They are assumed to be volatile.

g_char
g_current_scanline
g_current_row

*/

#define AUTOBAUD_INVALID_BIT_DURATION 0xffff

#define g_autobaud_StateChangeCount g_char // uint8_t. How many state changes have occurred since we started looking?
#define g_autobaud_MinDuration g_current_scanline // uint16_t. what's the minimum duration state that we've seen so far?
#define g_autobaud_OverflowCount g_current_row // uint8_t. How many overflows have occurred since we started timing this state?
volatile uint16_t g_autobaud_LastChangeTime ; // what was the timer count on the last state change.

uint16_t uart_Autobaud( void )
{
/*

This routine determines the correct UBBR value to use.

Basically look at the times between 8 state changes on the 'input capture' pin.
The smallest time duration between state changes is found.
This will be (with a small amount of luck) a single bit-width.

This can then be simply turned into a UBBR value.

In reality, this mechanism works between 300 baud and (at least) 115200 baud.
300 is the lower limit. Any slower and we overflow the 16-bit counter.
115200 is the upper limit that's been tested. It's the limit of my PC's serial port.
*/

//uint8_t spin_count = 0;


	g_autobaud_MinDuration = AUTOBAUD_INVALID_BIT_DURATION ; // invalid value
	g_autobaud_OverflowCount= 255 ; // ensure that the first state change doesn't result in a duration.
	

	while( g_autobaud_MinDuration == AUTOBAUD_INVALID_BIT_DURATION )
	{
		// start the timer...
		
		TCCR1B = (1<<ICNC1)|(1<<ICES1)|(1<<CS10); // 1:1 clock source. Noise canceller enabled. Looking for a rising edge. (e.g. start bit)
		
		g_autobaud_StateChangeCount = 0;
		
		// now enable the overflow and input capture interrupts...
#if defined(  _AVR_IOMX8_H_  ) || defined( _AVR_IOM328P_H_ )
		TIMER1INTERRUPTS |= ((1<<TOIE1)|(1<<ICIE1));
#elif defined( _AVR_IOM8_H_ )
		TIMER1INTERRUPTS |= ((1<<TOIE1)|(1<<TICIE1));
#endif
		sei();


		// The interrupt routines will now do their thing.
		// we'll just go into a loop waiting for them to finish.
		
		while( g_autobaud_StateChangeCount < 8){}
		
		// we've now had (at least) 10 state changes...
		// stop the timer that's looking for them...
#if defined(  _AVR_IOMX8_H_  ) || defined( _AVR_IOM328P_H_ )
		TIMER1INTERRUPTS &= ~((1<<TOIE1)|(1<<ICIE1));
#elif defined( _AVR_IOM8_H_ )
		TIMER1INTERRUPTS &= ~((1<<TOIE1)|(1<<TICIE1));
#endif
		
		TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));// stop the timer. No point leaving it running.
		
		if (g_autobaud_MinDuration == AUTOBAUD_INVALID_BIT_DURATION)
		{
			// things didn't work out.
			// all the durations found were too long.
			g_display[0][0] = 'b';
			g_display[0][1] = 'a';
			g_display[0][2] = 'u';
			g_display[0][3] = 'd';
			g_display[0][5] = 't';
			g_display[0][6] = 'o';
			g_display[0][7] = 'o';
			g_display[0][9] = 's';
			g_display[0][10] = 'l';
			g_display[0][11] = 'o';
			g_display[0][12] = 'w';
		}
	}
	// we've found a valid minimum duration for state changes.
	// the UBBR value to return is simply 1/16th of that bit-time in clock ticks.
	return (g_autobaud_MinDuration >> 4);
}

ISR( TIMER1_OVF_vect )
{
	// bump up the overflow count.
	// It'll max-out at 255
	if (g_autobaud_OverflowCount != 255) g_autobaud_OverflowCount++ ;
}

ISR( TIMER1_CAPT_vect )
{
	uint16_t time = ICR1 ; // capture the input time as early in the routine as possible.
	TCCR1B ^= (1<<ICES1) ; // toggle the input capture mode so that we're looking for to be the opposite sort (rising/falling edge).

	if (g_autobaud_OverflowCount < 2)
	{
		if ((time<=g_autobaud_LastChangeTime) || (g_autobaud_OverflowCount==0))
		{
			uint16_t duration = time - g_autobaud_LastChangeTime ;
			if (duration < g_autobaud_MinDuration) g_autobaud_MinDuration = duration ;
		}
	}
	g_autobaud_StateChangeCount++ ;
	g_autobaud_LastChangeTime = time ;
	g_autobaud_OverflowCount = 0;
}

