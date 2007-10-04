/*
 * For the STK500. Connect PORTB to leds and PORTD to switches.
 */
//#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 100000UL 
#include <util/delay.h>

// Globals
volatile uint8_t playing;
volatile uint8_t score;
int leds[9] = {255,254,253,251,247,239,223,191,127};
int i = 0;

// Prototypes
int attract(int);
int speed(void);
int target(int);
int tug(void);
int bounce(void);
void display(int);
void countdown(void);

// Interrupt Handler
SIGNAL(SIG_OUTPUT_COMPARE1A) {
	if ( playing == 1 ) {
		playing = 0;
		for(i=0;i<=10;i++){
			display(score);
			_delay_ms(3000);
			PORTB=0xFF;
			_delay_ms(3000);
		}
	}
	return;
}

int main(void) {
	sei();
	DDRD = 0;    /* set for input. */
	DDRB = 0xff; /* enable as output */
	int j = 0;

	while(1) {
		int rc = attract(j++);
		if ( rc == 1 || rc == 5 ) {
			speed();
		}
		else if ( rc == 2 || rc == 6 ) {
			target(500);
		}
		else if ( rc == 3 || rc == 7 ) {
			tug();
		}
		else if ( rc == 4 ) {
			bounce();
		}
	}
	return 1;
}

int attract(int j) {
	int l_rc = 0;
	PORTB = leds[j%9];
	for(i=0;i<900;i++) {
		if ( ( ( PIND & ( 1 << PD0 ) ) == 0 ) || ( ( PIND & ( 1 << PD1 ) ) == 0 ) ) {
			l_rc=j%9;
		}
		_delay_ms(5);
	}
	return l_rc;
}

int bounce(void) {
	_delay_ms(1000);
	int ball = 5;
	int miss = 0;
	int forward = 0;
	int count = 500;
	score = -1;
	while ( miss != 1 ) {
		if ( forward == 1 ) {
			ball++;
		}
		else {
			ball--;
		}
		PORTB=leds[ball];
		if ( ball == 0 || ball == 8 ) {
			miss=1;
		}
		else {
			for(i=0;i<count;i++) {
				if ( ( ball == 1 ) && ( ( PIND & ( 1 << PD0 ) ) == 0 ) ) {
						forward=1;
				}
				else if ( ( ball == 7 ) && ( ( PIND & ( 1 << PD1 ) ) == 0 ) ) {
						forward=0;
				}
				else {
					if ( ( PIND & ( 1 << PD0 ) ) == 0 ) {
						miss=1;
					}
					if ( ( PIND & ( 1 << PD1 ) ) == 0 ) {
						miss=1;
					}
				}
				_delay_ms(10);
			}
		}
		if ( ball == 7 || ball == 1 ) {
			if ( count > 61 ) {
				count=count-20;
			}
			else if ( count > 20 && ball == 7 ) {
				count=count-1;
			}
			score++;
		}
	}
	// Display Score
	for(i=0;i<=10;i++){
		PORTB=0xFF;
		_delay_ms(3000);
		display(score);
		_delay_ms(3000);
	}
	return 1;
}

int target(int l_time) {
	// Flash Target
	//int bullseye = rand()%6+2;
	int bullseye = 4;
	for(i=0;i<7;i++) {
		PORTB = leds[bullseye];
		_delay_ms(5000);
		PORTB = 0xff;
		_delay_ms(5000);
	}

	// Run lights
	score = 1;
	i = 0;
	while ( ( ( PIND & ( 1 << PD1 ) ) != 0 ) && ( ( PIND & ( 1 << PD0 ) ) != 0 ) && (i++<(25*9)+1) ) {
		PORTB = leds[i%9];
		_delay_ms(l_time);
		if ( (i%9) == 0 ) {
			score++;
		}
	}
	if ( (i%9) != bullseye ) {
		score=255;
	}

	// Display Score
	for(i=0;i<=10;i++){
		PORTB=0xFF;
		_delay_ms(3000);
		display(score);
		_delay_ms(3000);
	}

	PORTB = 0xff;
     return 1;
}

int speed(void) {
	countdown();
	// Timer setup
	TCNT1 = 0;
	TIMSK = _BV(OCIE1A);
	TCCR1B = _BV(CS12) | _BV(WGM12);
	OCR1A = 31250;

	score = 0;
	playing = 1;
	while (playing==1) {
		if ( ( PIND & ( 1 << PD1 ) ) == 0  ) {
			_delay_ms(75);
			while ( ( PIND & ( 1 << PD1 ) ) == 0  ) {}
			score++;
		}
		display(score);
	}
	return 1;
}

int tug(void) {
	countdown();
	
	score = 0;
	i = 4;
	int down0 = 0;
	int down1 = 0;

	// Timer setup
	TCNT1 = 0;
	TIMSK = _BV(OCIE1A);
	TCCR1B = _BV(CS12) | _BV(WGM12);
	OCR1A = 31250;
	playing = 1;
	while (playing==1) {
		PORTB=leds[i];
		if ( i>4 ) {
			score=0xF8;
		}
		else if (i<4) {
			score=0x0F;
		}
		else {
			score=0xFF;
		}
		if ( ( PIND & ( 1 << PD1 ) ) == 0  ) {
			_delay_ms(50);
			down1=1;
		}
		if ( ( PIND & ( 1 << PD0 ) ) == 0  ) {
			_delay_ms(50);
			down0=1;
		}
		if ( ( ( PIND & ( 1 << PD1 ) ) != 0 ) && ( down1 == 1 ) ) {
			down1=0;
			if ( i<7 ) {
				i++;
			}
		}
		if ( ( ( PIND & ( 1 << PD0 ) ) != 0 ) && ( down0 == 1 ) ) {
			down0=0;
			if ( i>1 ) {
				i--;
			}
		}
	}
	return 1;
}

void countdown(void) {
	for(i=5;i>0;i--){
		PORTB=leds[i];
		_delay_ms(3000);
	}
	return;
}

void display(int l_score) {
	PORTB=255-l_score;
}




