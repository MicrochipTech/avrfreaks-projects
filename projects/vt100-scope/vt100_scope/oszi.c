/*-----------------------------------------------------------------------
	Compiler: avr-gcc
	Micro   : tested with 90at2333
	Version : 1	(1.12.2001)
	Author  : Günther Klement (guenther.klement@t-online.de)

	Try to simulate a digital scope using a vt100 terminal emulation
	as display.

	Note: As input is echoed to the terminal you kann type eg. ESC [ J
	      to clear the screen or any other ESC sequenz.
-----------------------------------------------------------------------*/
#define XTAL			8000*1000	// 8 MHz
// Don't use for high baud rates
#define UART_UBRR(baudrate)	(XTAL/16/baudrate-1)
// set to 1 to use LTC1196 instead of internal ADC
#define EXT_ADC			0

#define _EEPROM_H_ 1
//#include <eeprom.h>	saves some FLASH space

#include <inttypes.h>
#include <timer.h>
#include <progmem.h>
#include <sig-avr.h>
#include <interrupt.h>

enum { // time bases
	TB_MIN,
	TB_50micro,
	TB_100micro,
	TB_500micro,
	TB_1ms,
	TB_5ms,
	TB_10ms,
	TB_50ms,
	TB_MAX
};


// timer ticks
static unsigned int	tick_0;		// timer0 tick
static uint8_t		cmd;
static uint8_t		refresh;
static uint8_t		time_base;
static uint8_t		pos_x, pos_y, mark_x, mark_y;
static uint8_t 		flag;
uint8_t			channel;

#define	TEST_GRID	(flag & 1)
#define SET_GRID 	flag |= 1
#define CLR_GRID 	flag &= ~1
#define TEST_FILL	(flag & 2)
#define SET_FILL	flag |= 2
#define CLR_FILL	flag &= ~2
#define TEST_DUMP	(flag & 4)
#define SET_DUMP	flag |= 4
#define CLR_DUMP	flag &= ~4
#define TEST_4BIT_RES	(flag & 8)
#define SET_4BIT_RES	flag |= 8
#define CLR_4BIT_RES	flag &= ~8

#define OVERFLOW		255
#define X_PLOT_OFFSET		10	// must be > 0
#define Y_PLOT_OFFSET		5	// only if 4 bit resolution
#define MAX_RECV_BUF		3	// must be power of 2 + 1
uint8_t buf_tail, buf_head;
uint8_t receiv_buf[ MAX_RECV_BUF+1 ];

#define MAX_ADC_BUF		57
uint8_t adc_buf_ptr;
uint8_t adc_buf[ MAX_ADC_BUF ];


/* Funktion prototypes */
uint8_t receiv_char( void );
void send_char( uint8_t c );
void send_string( const prog_char* p );
void send_digi8s( int8_t i );
void send_digi8 ( uint8_t i );
void show_help( void );
void show_sample( void );
void show_status( void );
void show_pos( uint8_t cmd );
uint8_t grid_char( uint8_t x, uint8_t y );
void start_sampling( void );
void home( void );
void plot( uint8_t x, uint8_t y, uint8_t ch );
void gotoxy( uint8_t x, uint8_t y );
uint8_t normalize( uint8_t x );
void dump( void );


SIGNAL( SIG_OVERFLOW0 )
{
	++tick_0;
	// toggle test pin for frequency control
	outp( inp( PORTB ) ^ BV(PB2), PORTB );
}

SIGNAL( SIG_UART_RECV )
{
	receiv_buf[ buf_head ] = inp( UDR );
	buf_head = ++buf_head & MAX_RECV_BUF;
}


SIGNAL( SIG_OUTPUT_COMPARE1A )
{
	if (adc_buf_ptr < MAX_ADC_BUF*2)
	{
		uint8_t* p_buf = &adc_buf[ adc_buf_ptr >> 1 ];

#define DIN	PD7

#if EXT_ADC == 1	// routine is for a LTC1196

#define CS	PB0
#define CLK	PD6

		uint8_t		k, temp_i;

		__cbi( PORTD, CLK );	// pre condition
		__cbi( PORTB, CS );	// chip select
		// clock out tree null bits
		__sbi( PORTD, CLK );
		__cbi( PORTD, CLK );
		__sbi( PORTD, CLK );
		__cbi( PORTD, CLK );
		__sbi( PORTD, CLK );
		// clock in 8 bits; MSB first
		for (k=8; k > 0; --k) {
			__cbi( PORTD, CLK );
			temp_i <<= 1;
			asm ( "sbic %0, %1" :: "I" (PIND), "I" (DIN) );
			asm ( "inc %0" :: "r" (temp_i) );
			__sbi( PORTD, CLK );
		}
		__sbi( PORTB, CS );

#else // intern ADC

		uint16_t temp_i;

		temp_i = __inw( ADC ); // sampling is done in free run mode
		temp_i >>= 2;	// 8 bit resolution

#endif // EXT_ADC

		if (TEST_4BIT_RES) {
			if (adc_buf_ptr & 1) { // odd adr.
				*p_buf |= (uint8_t)temp_i & 0xf0;
			} else {
				*p_buf = (uint8_t)temp_i >> 4;
			}
		} else {
			*p_buf = (uint8_t)temp_i;
			++adc_buf_ptr; // step by 2
		}
		++adc_buf_ptr;
	} else {
		// stop timer_1
		outp( 0, TCCR1B );
		adc_buf_ptr = 0;
	}
}

int main(void)
{
	/* setup Port B */
	outp( BV(DDB2)|BV(DDB1)|BV(DDB0), DDRB );
	/* setup Port D */
	outp( BV(DDD6), DDRD );
	__sbi( PORTD, DIN );	// Pullup for data line
	/* setup UART Baudrate
	   57600 = 8; 38400 = 12; 19200 = 25; 14400 = 34; 9600 = 51; */
	outp( 25, UBRR );
	/* enable UART */
	outp( BV(RXCIE) | BV(RXEN) | BV(TXEN), UCR );
	/* enable ADC in free run mode */
	outp( BV(MUX2), ADMUX );
	outp( BV(ADEN)|BV(ADSC)|BV(ADFR)| BV(ADPS1)|BV(ADPS0), ADCSR );
	/* preset sample timer */
	time_base = TB_10ms;
	/* install a time base. Must be adjusted to XTAL*/
	outp( BV(CS02)|BV(CS00), TCCR0 ); // we will get 7812 ticks per seconds
	outp( BV(TOIE0), TIMSK );
	/* enable global int too */
	sei();

	/* idle loop */
	refresh = 70;
	cmd = 'r'; // free run
	for(;;)
	{	asm("wdr"); // feed watchdog

		if (buf_head != buf_tail)
			cmd = receiv_char();
		if (tick_0 > refresh) {
			tick_0 = 0;
			// blink to show activity
			outp( inp( PORTB ) ^ BV(PB1), PORTB );
			if (cmd == 'd')
				dump();
			if (cmd == 'r') {
				start_sampling();
				show_status();
				show_sample();
			}
			if (cmd == 's') {
				show_sample();
				cmd = ' '; // do nothing
			}
		}
	}
} // end main



uint8_t receiv_char( void )
{
	static uint8_t	state;
	uint8_t		cmd;

	cmd = receiv_buf[ buf_tail ];
	buf_tail = ++buf_tail & MAX_RECV_BUF;

	send_char( cmd ); // echo it

	if (cmd == '\033') {state=1;}
	if (cmd == '[') {if (state==1) state=2;}
	if (cmd >= 'A' && cmd <= 'D' && state==2){show_pos(cmd);}
	if (cmd == 'm') {mark_x = pos_x; mark_y = pos_y;}
	if (cmd == 'd') ;
	if (cmd == 'h') {show_help();}
	if (cmd == 's') ;
	// following commands change into run mode!
	if (cmd == 'r') ;
	if (cmd == 'c') {++channel; channel &= 7; 		goto run;}
	if (cmd == 'f') {if(TEST_FILL) CLR_FILL; else SET_FILL; goto run;}
	if (cmd == 'g') {if(TEST_GRID) CLR_GRID; else SET_GRID; goto run;}
	if (cmd == 't') {if (time_base > TB_MIN+1) --time_base; goto run;}
	if (cmd == 'T') {if (time_base < TB_MAX-1) ++time_base; goto run;}
	if (cmd == 'z') {if(refresh>10) refresh-=10; 		goto run;}
	if (cmd == 'Z') {refresh+=20;				goto run;}
	if (cmd == 'a') { if(TEST_4BIT_RES) {
				CLR_4BIT_RES;
				send_string( PSTR("\033[?3l") );
			} else {
				SET_4BIT_RES;
				send_string( PSTR("\033[?3h") );
			} 					goto run;}
	return cmd;
run:
	tick_0 = ~0; // don't wait
	return 'r';
}

void start_sampling( void )
{
	uint8_t		TCCR1B_val = BV(CTC1) | BV(CS10);
	//------ CLK 8 MHz / 1 ==> one tick is 125 ns

	outp( channel, ADMUX );	// select input (see also errata data sheet)

	// use compare match interrupt for getting sample data
	// sampling is done in free run mode
	// one div = 8 samples; one tick is 0.125 µs
#define TICKS(val)	__outw( val, OCR1 );

	if (time_base == TB_50micro) {
		TICKS( 160 ) // 20 µs
		// Note that the signal routine lasts more than 160 ticks
	}
	if (time_base == TB_100micro) {
		TICKS( 800 ) // 0.1 ms
	}
	if (time_base == TB_500micro) {
		TICKS( 4000 ) // 0.5 ms
	}
	if (time_base == TB_1ms) {
		TICKS( 8000 ) // 1 ms
	}
	if (time_base == TB_5ms) {
		TICKS( 40000 )
	}
	//------ CLK 8 MHz / 256 ==> one tick is 32 µs
	if (time_base >= TB_10ms)
		TCCR1B_val = BV(CTC1) | BV(CS12) ;

	if (time_base == TB_10ms) {
		TICKS( 312 )
	}
	if (time_base == TB_50ms) {
		TICKS( 1562 )
	}

	outp( 0, TCCR1A );
	// CTC1 = clear counter on compare match
	outp( TCCR1B_val, TCCR1B );

	// run
	adc_buf_ptr = 0;
	outp( BV(OCIE1)|BV(TOIE0), TIMSK );
}

void show_status( void )
{
	send_string( PSTR("\033[H\t\t\033[K") );
	if (time_base == TB_50micro)
		send_string( PSTR(".02") );
	if (time_base == TB_100micro)
		send_string( PSTR("0.1") );
	if (time_base == TB_500micro)
		send_string( PSTR("0.5") );
	if (time_base == TB_1ms)
		send_string( PSTR("  1") );
	if (time_base == TB_5ms)
		send_string( PSTR("  5") );
	if (time_base == TB_10ms)
		send_string( PSTR(" 10") );
	if (time_base == TB_50ms)
		send_string( PSTR(" 50") );
	send_string( PSTR(" ms/div  0.6 V/div\tCh \0") );
	send_digi8( channel );
	if (TEST_4BIT_RES)
		send_string( PSTR("\t4 Bit") );
}

void dump( void )
{
	uint8_t x;
	for(x=0; x < MAX_ADC_BUF; x++) {
		send_digi8( adc_buf[x] );
		send_char(' ');
	}
	send_char('\r');
	send_char('\n');
}

void show_sample( void )
{
	static uint8_t	loop_count;
	uint8_t		x, y, y0, y00;

	if (TEST_4BIT_RES)
		y = 8;
	else
		y = 1;

	while ( ++y < 25 )
	{
		if ( TEST_4BIT_RES )
			gotoxy( X_PLOT_OFFSET, y - Y_PLOT_OFFSET );
		else
			gotoxy( X_PLOT_OFFSET, y );
		send_string( PSTR("\033[K") ); // clear line

		for (x=0; x < MAX_ADC_BUF*2; x++) {
			if ( !TEST_4BIT_RES && (x&1) )
				continue; // skip odd addresse
			y0 = y00 = normalize( x );
			if (x>1) {
				if (TEST_4BIT_RES)
					y00 = normalize( x-1 );
				else
					y00 = normalize( x-2 );
			}
			if (y == 2 && y0 == OVERFLOW) {
				plot( x, y, '^' ); // overflow character
				continue;
			}
			if ( x && TEST_FILL ) {
				if (y > y0 && y < y00) {
					plot( x, y, '/' );
					continue;
				}
				if (y < y0 && y > y00) {
					plot( x, y, '\\' );
					continue;
				}
			}
			if (y0 == y) {
				plot( x, y, '*' );
				continue;
			}
			if ( TEST_GRID ) {	// turned on?
				y0 = grid_char( x, y );
				if (y0)	plot( x, y, y0 );
			}
		} // for all x positions
		if (buf_head != buf_tail) // break on any input
			break;
	} // for all lines

	gotoxy( 1, 1 );
	send_digi8( ++loop_count );
	send_char(' ');
	send_char('>');
}

// normalize y-value (0..255) to line (2..24) for 8 bit resolution
// for 4 bit resolution return is (0..15)
uint8_t normalize( uint8_t x )
{
	uint8_t		y;

	y = adc_buf[x>>1];

	if (TEST_4BIT_RES) {
		if (x & 1) // odd adr.
			y >>= 4;
		else
			y &= 0x0f;
	} else { //8 Bit Resolution
		y >>= 3;	// div 8 => 0..31
		if (y>22)	// keep out top line
			return OVERFLOW;
	}
	return 24 - y; // range 2..24
}

void plot( uint8_t x, uint8_t y, uint8_t ch )
{
	if (TEST_4BIT_RES)
		y -= Y_PLOT_OFFSET;
	else			// x = 0..MAX_ADC_BUF-1
		x >>= 1;	// else x = 0..132 max
	x += X_PLOT_OFFSET;	// x starts at 0!
	if (pos_x != x)
		gotoxy( x, y );
	else
		++pos_x; // advance one char
	send_char( ch );
}

// returns grid character or null
uint8_t grid_char( uint8_t x, uint8_t y )
{
	uint8_t ch = 0;

	if (!TEST_4BIT_RES)
		x >>= 1;
	if ((x & 7) == 0) // horizontal 1, 9, 17...
		ch = '.';
	if ((y & 3) == 0) { // vertical 24, 20, 16,
		if(ch) // then it's a cross point
			ch = '.'; // try '+'
		else if ((x&1)==0)
			ch = '.';
	}
	return ch;
}

void send_string( const prog_char* p )
{
	uint8_t		c, i=0;

	while( (c = PRG_RDB(p+i)) )
	{
		send_char( c );
		++i;
	}
}

void send_char( uint8_t c )
{
	while( !(inp(USR) & BV(UDRE)) )
		;
	outp( c, UDR );
}

void send_digi8s( int8_t i )
{
	if (i < 0) {
		send_char('-');
		i = -i;
	}
	send_digi8( i );
}

void send_digi8( uint8_t i ) // hex to decimal without div
{
	uint8_t	s = '0';

	while(i>=100) {	i-=100;	++s; }
	if (s>'0') {
		send_char(s);
		s = '0';
		if (i<10)
			send_char(s); // eg. 101
	}
	while(i>=10) { i-=10; ++s; }
	if (s>'0') send_char(s);
	s = '0';
	while(i>0) { --i; ++s; }
	send_char(s);
}

uint8_t help_str[] __attribute__((section(".eeprom"))) =
	"\033[2J\n\t"
	"r\trun\r\n\t"
	"f\tfill\r\n\t"
	"s\tsingle shot\r\n\t"
	"t/T\ttime base\r\n\t"
	"z/Z\trefresh rate\r\n\t"
	"g\tgrid\r\n\t"
	"c\tclear\r\n\t"
	"m\tmark\r\n\t"
	"d\tdump\r\n\t"
	"c\tChannel"
	"\033[H\0";

uint8_t eeprom_rb( int ptr )
{
	loop_until_bit_is_clear(EECR, EEWE);
	outp( (uint8_t)ptr, EEAR );
	sbi( EECR, EERE );
	return inp( EEDR );
}

void show_help( void )
{
	uint8_t		c, i;
	i = 0;
	do {
		c = eeprom_rb( (int)help_str+i );
		send_char( c );
		++i;
	} while(c);
}

void show_pos( uint8_t cmd )
{
	uint8_t 	x = pos_x;
	uint8_t		y = pos_y;

	if (cmd == 'A' && y> 1)	--y;
	if (cmd == 'B' && y<24)	++y;
	if (cmd == 'D' && x> 1)	--x;
	if (cmd == 'C') {
		if (x<80)	++x;
		else if ( TEST_4BIT_RES && (x < 132) )
					++x;
	}

	gotoxy( 60, 1 );
	send_char( 'x' );
	send_digi8s( x - mark_x );
	send_char('y');
	send_digi8s( y - mark_y );
	send_char( ' ' );
	gotoxy( x, y );
}

void gotoxy( uint8_t x, uint8_t y )
{
	send_char( '\033' );
	send_char( '[' );
	send_digi8( y );
	send_char( ';' );
	send_digi8( x );
	send_char( 'H' );
	pos_x = x;
	pos_y = y;
}

/*------------------------------------ eof --------------------------------*/