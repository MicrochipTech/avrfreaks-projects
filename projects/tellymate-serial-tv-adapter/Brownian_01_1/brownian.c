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

Brownian Motion 'Simulator'
===========================

26 characters are wiggled around the screen.
It outputs to a TellyMate display terminal via a TTL serial signal.
If pin 3 is shorted to ground, the trails are not wiped.

Hardware:
	Tiny13 at default fuse settings (1.2Mhz)
	PB3 (pin 2) outputs a TTL serial signal
	PB4 (pin 3) is an input (pulled high)

Software:
	A simple bit-bang serial data sender.
	Not fancy. Not interrupt driven.
	It drives a TellyMate display terminal.

Notes:
======
Developed using WinAVR20080512.
*/

#include <avr/io.h>
#include <util/delay.h>

// note that the baud rate is (very) approximate.
// It's more of a 'ball park' target.
#define BAUD_RATE 57600
#define CYCLES_PER_US (F_CPU / 1000000)
#define BIT_DELAY_US (1000000L / BAUD_RATE)

#define SERIAL_PORT PORTB
#define SERIAL_DDR DDRB
#define SERIAL_PIN_TX 3
#define PIN_TRAIL 4

#define CHAR_ESC 27
#define ROWCOUNT 25
#define COLCOUNT 38

// serial functions
static void serial_init( void ) ;
static void serial_putc( char c ) ; // writes the character to the serial out pin

// terminal helper functions
static inline void term_init( void ) ;
static inline void term_line_overflow_off( void ) ;
static inline void term_clear_screen( void ) ;
static inline void term_cursor_off( void ) ;
static inline void term_move_cursor( uint8_t , uint8_t ) ;
static inline void term_place_char( uint8_t , uint8_t, uint8_t ) ;

// random functions/variables
static uint16_t	g_randomSeed = 0x8765; // random seed
static inline uint8_t random_getbyte( void ); // returns a random byte

int __attribute__((OS_main)) main (void)
{
#define PARTICLE_COUNT 26
uint8_t particle_x[ PARTICLE_COUNT ] ;
uint8_t particle_y[ PARTICLE_COUNT ] ;

	serial_init();
	PORTB |= (1 << PIN_TRAIL) ; // pull the input pin high
	term_init();
	
	// initialise the particles' position.
	for( uint8_t i = 0 ; i < PARTICLE_COUNT ; i++ )
	{
		particle_x[ i ] = i ;
		particle_y[ i ] = ROWCOUNT >> 1 ;
	}

	// start the simulation
	while(1)
	{
		for( uint8_t i = 0 ; i < PARTICLE_COUNT ; i++ )
		{
			uint8_t x = particle_x[ i ] ;
			uint8_t y = particle_y[ i ] ;
			
			if ( PINB & (1 << PIN_TRAIL) )
			{
				// we're clearing any trail, so overwrite the old character.
				term_place_char( x , y , ' ' ) ;
			}
			
			// my random number generator isn't very random, so
			// I'm selecting a couple of bits in the middle of the byte
			// to work out which way to move the particle.
			switch(random_getbyte() & 0b11000 )
			{
				case 0b00000 :
					if (x < (COLCOUNT - 1)) x += 1 ;
					break ;
				case 0b01000 :
					if (x > 0) x -= 1 ;
					break ;
				case 0b10000 :
					if (y < (ROWCOUNT - 1)) y += 1 ;
					break ;
				case 0b11000 :
					if (y > 0) y -= 1 ;
					break ;
			}
			particle_x[ i ] = x ;
			particle_y[ i ] = y ;
			term_place_char( x , y , 'A' + i ) ;
		}
	}
	
}

//=================
// Serial functions
//=================
static void serial_init( void )
{
	SERIAL_DDR = (1<<SERIAL_PIN_TX) ; // the serial tx pin is an output.
	SERIAL_PORT |= (1<< SERIAL_PIN_TX) ; // set the serial line high (the 'nothing happening' state).
}

static void serial_putc( char c )
{
	// output the start bit
	SERIAL_PORT &= ~(1<<SERIAL_PIN_TX) ;
	_delay_us( BIT_DELAY_US - 2) ;
	
	for( uint8_t bitpos = 8 ; bitpos > 0 ; bitpos-- )
	{
		// This mildly verbose way of setting the output pin ensures
		// that the path for 'set' and 'clear' is exactly the same length
		// and that the pin state is set at the same point in the loop.
		//
		// This becomes quite important at higher speeds.
		uint8_t out = SERIAL_PORT ;
		out &= ~(1<<SERIAL_PIN_TX) ;
		if (c & 0b1) out |= (1<<SERIAL_PIN_TX) ;
		SERIAL_PORT = out ;
		
		_delay_us( BIT_DELAY_US - (8 * CYCLES_PER_US)) ; // shorten the delay slightly to allow for the loop, setting the pin etc.
		c = c >> 1 ;
	}
	// output the stop bit
	SERIAL_PORT |= (1<<SERIAL_PIN_TX);
	_delay_us( BIT_DELAY_US ) ;

}

//===================
// Terminal functions
//===================
static inline void term_init( void )
{
	// wait for the terminal to initialise.
	// (half a second is probably too long)
	_delay_ms(500);
	
	// send some characters for the autobauding to catch
	for( uint8_t i = 10; i ; i-- )
	{
		serial_putc('U');
	}
	
	// wait to let the terminal take a deep breath.
	_delay_ms(5);
	
	// set up any options
	term_line_overflow_off() ;
	term_clear_screen() ;
	term_cursor_off() ;
}

static inline void term_move_cursor( uint8_t x , uint8_t y )
{
	serial_putc( CHAR_ESC ) ;
	serial_putc( 'Y' ) ;
	serial_putc( ' ' + y ) ;
	serial_putc( ' ' + x ) ;	
}

static inline void term_place_char( uint8_t x , uint8_t y , uint8_t c )
{
	term_move_cursor( x , y ) ;
	serial_putc( c ) ;
}

static inline void term_clear_screen( void )
{
	serial_putc( CHAR_ESC ) ;
	serial_putc( 'E' ) ; // clear screen
}

static inline void term_cursor_off( void )
{
	serial_putc( CHAR_ESC ) ;
	serial_putc( 'f' ) ; // cursor off
}

static inline void term_line_overflow_off( void )
{
	serial_putc( CHAR_ESC ) ;
	serial_putc( 'w' ) ; // line overflow off
}

//===================
// Random functions
//===================
static inline uint8_t random_getbyte(void)
{
   if((g_randomSeed&0x8000)==0)
      g_randomSeed=g_randomSeed<<1;
   else {
      g_randomSeed=g_randomSeed<<1;
      g_randomSeed = g_randomSeed ^ 0x1D87;
      }
   return((g_randomSeed >> 8) & 0xff);
} 