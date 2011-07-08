/*
lfuse=0x7a
hfuse=0xff
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <util/delay.h>
#include <util/parity.h>

#define CLK PB4
#define CLKINT PCINT4

#define DATA PB3
#define DATAINT PCINT3

#define LED PB0

#define start_timer(TM) do{\
	cli(); \
	OCR0A=TM; \
	TCNT0=0; \
	timeout=0; \
	TCCR0A=2; \
	TIMSK0=_BV(TOIE0); \
	TCCR0B=3; \
	sei(); \
}while(0)

#define stop_timer() TCCR0B=0;

#define sleep() do{ \
	cli(); \
	set_sleep_mode( SLEEP_MODE_IDLE ); \
	sleep_enable(); \
	sei(); \
	sleep_cpu(); \
	sleep_disable(); \
}while(0)


volatile uint8_t timeout, state;


ISR( PCINT0_vect ) {
	_delay_us(3);
	state=PINB;
}

ISR( TIM0_OVF_vect ) {
	if( timeout<255 )
		timeout++;
}


int main() {
	uint8_t i, byte, prev, sum, set_rate=0, cmd=0;
	uint16_t word;

	GIMSK = _BV(PCIE);
	PCMSK = _BV(CLKINT) | _BV(DATAINT);

	DDRB = _BV(LED);
	PORTB = 0;

	while( 1 ) {
		stop_timer();
		prev=state=PINB;
		word=0;
		while( 1 ) {
			prev=state;
			sleep();
			if( (prev^state)&_BV(DATA) )
				if( !(state&(_BV(CLK)|_BV(DATA))) )
					break;
		}

		start_timer( 150 );
		while( !timeout ) {
			prev=state;
			sleep();
			if( (prev^state)&_BV(CLK) ) {
				if( state&_BV(CLK) && !(state&_BV(DATA)) )
					break;
				else
					timeout++;
			}
		}

		cli();
		if( timeout )
			continue;

		for( i=9; i; --i ) {
			start_timer(10);
			while( !timeout ) {
				prev=state;
				sleep();
				if( state&_BV(CLK) ) {
					word <<= 1;
					if( state&_BV(DATA) ) {
						word |= 1;
					}
					break;
				}
			}

			if( timeout )
				break;
		}

		cli();
		if( timeout )
			continue;



		sum = word&0x01;
		word>>=1;
		byte=0;
		for( i=8; i; --i ) {
			byte<<=1;
			byte |= (word&0x01);
			word>>=1;
		}

		sum ^= parity_even_bit(byte);

		if( !sum )
			continue;

		switch( byte ) {
			case 0xF3:
				set_rate=1;
				break;

			case 10:
				if( set_rate )
					cmd<<=1;
				set_rate=0;
				break;

			case 20:
				if( set_rate ) {
					cmd<<=1;
					cmd |= 1;
				}
				set_rate=0;
				break;

			case 40: case 60: case 80: case 100: case 200:
				cmd=0;
			default:
				set_rate=0;
				break;
		}

		if( cmd & 0x80 )
			cmd &= 0x7f;
		else
			continue;

		switch( cmd ) {
			case 0x00:
				break;

			case 0x01:
				PORTB &= ~_BV(LED);
				break;

			case 0x02:
				PORTB |= _BV(LED);
				break;

			case 0x03:
				PINB = _BV(LED);
				break;
		}
		cmd=0;
	}
}
