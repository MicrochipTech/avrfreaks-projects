/*

Copyright (c) 2009, Keith Batten, <firstname>.<lastname>@lsc.gov.uk
Copyright (c) 2009, Nigel Batten, <firstname>.<lastname>@mail.com

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
===================
T13 skiing
 
Hardware:
	Tiny13 at default fuse settings (1.2Mhz)
	PB4 (pin 3) outputs a TTL serial signal
	PB1 (pin 6) button (NO) to ground - right
	PB2 (pin 7) button (NO) to ground - left
	RST (pin 1) button (NO) to ground - reset
	
Software:
	A simple bit-bang serial data sender.
	Driving out to a TellyMate display terminal.
	Using ASCII code page 437 characters.
	1 interrupt for simple switch debouncing.
	Divided down to a slower flag for game steps.

Play:
	Use the left and right buttons to guide
	the skier down the course through the forest.
	The course gets narrower and faster.
	To start a new game, just press reset.

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//===================
// Serial stuff
//===================
static void serial_init( void ) ;
static void serial_putc( char c ) ; // writes the character to the serial out pin

// note that the baud rate is (very) approximate.
// With the RC internal clock, it's more of a 'ball park' figure.
#define BAUD_RATE 57600
#define CYCLES_PER_US (F_CPU / 1000000)
#define BIT_DELAY_US (1000000L / BAUD_RATE)

#define SERIAL_PORT PORTB
#define SERIAL_DDR DDRB
#define SERIAL_PIN_TX 4

//===================
// Terminal stuff
//===================
static inline void term_init( void ) ;
static inline void term_line_overflow_off( void ) ;
static inline void term_clear_screen( void ) ;
static inline void term_cursor_off( void ) ;
static inline void term_move_cursor( uint8_t , uint8_t ) ;
static inline void term_place_char( uint8_t , uint8_t, uint8_t ) ;
static inline void term_put_spaces( uint8_t n ) ;

#define ROWCOUNT 25
#define COLCOUNT 38
#define CHAR_ESC 27
#define CHAR_DLE 0x10
#define CHAR_BACKSPACE 0x08
#define CHAR_LF 0x0A

//===================
// Random stuff
//===================
static uint16_t	g_randomSeed = 0x8765; // random seed
static inline uint8_t random_getbyte( void ); // returns a random byte

//===================
// Interrupt stuff
//===================

static void interrupt_init( void );

// Instead of using a 'volatile uint8_t' for the 'timer tick' flag
// We can use the register of timerb that we're not running.
#define         g_shorttick    OCR0B 

#define TIMER_ISR_NAME TIM0_COMPA_vect
#define TIMER_OCRA OCR0A
#define TIMER_COUNT TCNT0

//===================
// Button stuff
//===================
static void	button_init( void );
static void debounce_buttons( void );

#define BUTTON_DDR DDRB
#define BUTTON_PIN PINB
#define BUTTON_PORT PORTB
#define BUTTON_L 2
#define BUTTON_R 1
#define BUTTON_INIT 0b010101010
#define BUTTON_PRESSED 0x00
#define BUTTON_RELEASED 0xFF

static	uint8_t g_button_l = BUTTON_INIT ; 
static	uint8_t g_button_r = BUTTON_INIT ; 

//===================
// Game stuff
//===================
static void game_init( void ) ;
static void adjust_wobble(void); // varies the distance to the left side of the skiing track.
static void print_scenery(void); 
static void print_track(uint8_t x); // prints x track chars
static void print_trees(uint8_t x); // prints x chars, some of which look like trees.
static void store_track(uint8_t wob, uint8_t wid, uint8_t line_ref); // store this line of the ski track.
static void print_skier(void); // also checks for a collision.
static void print_score(void); // uint8_t converted to 3 decimal digits.

#define WOBBLE_MAX 15
#define WIDTH_MAX 15
#define WIDTH_EXT 3
#define GAMETICK_INTERVAL 180
#define SKIER_START ((ROWCOUNT)-(5)) 
#define CHAR_EDGE 0xf9
#define CHAR_TRACK ' '
#define CHAR_SKIER 0xba
#define CHAR_SHRUB 0x1e
#define CHAR_TREE 24
#define CHAR_NOTREE ' '

static	uint8_t g_line_ref ;
static	uint8_t g_width = 15 ; //width of the skiing track
static	uint8_t g_wobble = 8 ; //distance between the left side and the skiing track
static	uint8_t g_track[SKIER_START]; //stores the wobble and width at each line
static	uint8_t g_player_x = 17; //the skier
static	uint8_t g_distance = 0; //how many screenfuls have you skied.


//===================
// main
//===================

int __attribute__((OS_main)) main (void)
{
	uint8_t speed=GAMETICK_INTERVAL;
	uint8_t GameTick=speed;

	interrupt_init();
	button_init();
	serial_init();
	term_init(); // turns interrupts on as a by-product
	game_init();


	// Announce the start of the game.
	term_move_cursor(g_player_x,5);
	for (uint8_t i=3;i;i--)
	{
		serial_putc('0'+i);
		// simple delays are fine here
		_delay_ms(1000);
		serial_putc(CHAR_BACKSPACE);
	}
	serial_putc(CHAR_TRACK);
	
	// Start the game.
	while(1)
	{
		while (g_shorttick==0) {}
		g_shorttick=0; // consume the tick
		
		debounce_buttons();
		
		if (--GameTick == 0){
			GameTick = speed;
			
			if (g_line_ref==0){
				g_line_ref=SKIER_START;
				g_distance++;
				if ((g_distance&0b00000111)==0b00000111){
					g_width--;
					speed-=10;
				}
			} else {
				g_line_ref--;
			}
			
			print_scenery();
			print_score();
			
			if (g_button_l==BUTTON_PRESSED) {
				g_player_x--;
				random_getbyte(); //a touch of 'real' randomness.
			}
			if (g_button_r==BUTTON_PRESSED) {
				g_player_x++;
			}

			adjust_wobble();
			store_track(g_wobble, g_width, g_line_ref);
			print_skier(); // collision detection too.
		}
	}
}

//===================
// Game functions
//===================

static void game_init()
{
	g_line_ref = 0 ;
	for (uint8_t i=ROWCOUNT-1;i;i--){
		g_line_ref=(g_line_ref==0)?SKIER_START:g_line_ref-1;
		store_track(g_wobble, g_width, g_line_ref);
		print_scenery();
	}
}

static void store_track(uint8_t wob, uint8_t wid, uint8_t line_ref)
{
	// both wob and wid must be stored in 4 bits.
	// width never needs to be less than 3, so by taking 3 off the width stored, 
	// we can effectively use a width of 3 to 18.
	g_track[line_ref]=(wob<<4)|(wid-WIDTH_EXT);
}

static void print_scenery()
{
	// wipe over the skiier character.
	term_move_cursor(g_player_x,ROWCOUNT-SKIER_START);
	serial_putc(CHAR_TRACK);

	// scroll display up one line
	term_move_cursor(0,ROWCOUNT-1);
	serial_putc(CHAR_LF); 

	// draw a line of scenery
	print_trees(g_wobble);
	serial_putc(CHAR_EDGE);//left track edge.
	print_track(g_width);
	serial_putc(CHAR_EDGE);//right track edge.
	print_trees(COLCOUNT-(2+g_width+g_wobble));
}

static void print_skier()
{
	uint8_t skier_line;
	uint8_t left_edge;
	uint8_t right_edge;

	// Find the edges
	skier_line=(g_line_ref==0)?SKIER_START:g_line_ref-1;
	left_edge=g_track[skier_line]>>4;
	right_edge=left_edge + (g_track[skier_line]&WIDTH_MAX)+WIDTH_EXT + 1;

	// print the skier
	term_move_cursor(g_player_x,ROWCOUNT-SKIER_START);
	serial_putc(CHAR_SKIER);	

	// Has the skier gone off the edge of the track
	if ((g_player_x <= left_edge) | (g_player_x >= right_edge)){
		term_move_cursor(g_player_x,ROWCOUNT-SKIER_START+2);
		serial_putc('O');
		serial_putc('u');
		serial_putc('c');
		serial_putc('h');
		serial_putc('!');
		cli(); // Stop. End of game.
	}
}

static void print_score()
{
	// print the g_distance out in human readable numbers.
	// maximum g_distance is 255. Three characters: hundreds, tens and units.
	term_move_cursor(0,0);
	serial_putc('S');
	serial_putc('c');
	serial_putc('o');
	serial_putc('r');
	serial_putc('e');
	serial_putc(':');

	uint8_t x=g_distance;
	uint8_t i=0;
	uint8_t j=100;

	for(uint8_t a=3;a;a--)
	{
		while(x>=j){
		// Count how many times you can subtract the hundreds, tens or units.
		x-=j;
		i++;
		}
		serial_putc('0'+i);
		i=0;
		j=(j==100)?10:1;	
	}
}

static void adjust_wobble()
{
	// adjust how far from the left the ski track is.

	switch(random_getbyte()&0b00011000) {
		case 0b00011000:{ //go left if there's room
			if (g_wobble>1) g_wobble-=1;
			break;
		}
		case 0b00001000:{ //go right if there's room
			if ((g_wobble+1)<WOBBLE_MAX) g_wobble+=1;
			break;
		}
		default:{ //stay straight.
			break;
		}
	}
}

static void print_track(uint8_t x)
{
	for(uint8_t i=x;i;i--) {
		serial_putc(CHAR_TRACK);
	}
}

static void print_trees(uint8_t x)
{
	for(uint8_t i=x;i;i--){
		uint8_t mytree=CHAR_NOTREE;
		if (random_getbyte() > 235){
			mytree=CHAR_SHRUB;
		}
		if (random_getbyte() > 220){
			mytree=CHAR_TREE;
		}
		if (mytree<32){
			serial_putc(CHAR_DLE); //need to escape some chars
		}
		serial_putc(mytree);
	}
}

//=================
// Interrupt functions
//=================

ISR( TIMER_ISR_NAME )
{
	g_shorttick=1;
}

static void interrupt_init( void )
{
	// Set up the timer and interrupt
	// Set the timer to clk/8 ticking
	// Set the timer to CTC mode
	// Set the compare match interrupt
	TCCR0A |= ((1<<WGM01)|(0<<WGM00)) ;
	TCCR0B |= ((0<<CS02)|(1<<CS01)|(0<<CS00)) ;
	TIMSK0 |= (1 << OCIE0A) ;

	// About 100 should debounce nicely.
	TIMER_OCRA = 100; 
}

//===================
// Random functions
//===================
static uint8_t random_getbyte(void)
{
   if((g_randomSeed&0x8000)==0)
      g_randomSeed=g_randomSeed<<1;
   else {
      g_randomSeed=g_randomSeed<<1;
      g_randomSeed = g_randomSeed ^ 0x1D87;
      }
   return((g_randomSeed >> 8) & 0xff);
} 

//===================
// Button functions
//===================
static void button_init( void )
{
	// Set the buttons to be inputs.
	// Turn on the pull-up resistors
	BUTTON_DDR &= ~((1<<BUTTON_L)|(1<<BUTTON_R));
	BUTTON_PORT |= ((1<<BUTTON_L)|(1<<BUTTON_R));
}

static void debounce_buttons( void )
{
	// Simple debouncing.

	// Each time this function is called, the current state of each button is
	// shifted onto the end of the variable.  The variable represents the state of
	// the button over the last 8 calls to this function.

	// If all bits are 0, then the button has stopped bouncing and is closed.
	// If all bits are 1, then the button has stopped bouncing and is open.
	// Anywhere in between is still bouncing.

	g_button_l = (g_button_l << 1) | ((PINB >> BUTTON_L) & 0b00000001);
	g_button_r = (g_button_r << 1) | ((PINB >> BUTTON_R) & 0b00000001);
}


/*
The code below this point was taken from tbe Brownian motion Tiny13 demo
available from http://www.batsocks.co.uk/downloads/index_downloads.htm
*/
//=================
// Serial functions
//=================
static void serial_init( void )
{
	SERIAL_DDR |= (1<<SERIAL_PIN_TX) ; // the serial tx pin is an output.
	SERIAL_PORT |= (1<< SERIAL_PIN_TX) ; // set the serial line high (the 'nothing happening' state).
}

static void serial_putc( char c )
{
	// switch off interrupts. Timing is important here.
	cli();

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
		
		// shorten the delay slightly to allow for the loop, setting the pin etc.
		_delay_us( BIT_DELAY_US - (8 * CYCLES_PER_US)) ; 
		c = c >> 1 ;
	}
	// output the stop bit
	SERIAL_PORT |= (1<<SERIAL_PIN_TX);
	_delay_us( BIT_DELAY_US ) ;
	
	// switch the interrupts back on.
	sei();
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
	
void term_put_spaces( uint8_t n )
{
	for(uint8_t i=n;i;i--)
	{
		serial_putc(' ') ;
	}
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

