#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 10000000UL //define crystal frequency

#define DISABLE_ONE 0x02 //define disable_one for player 1 00000010
#define DISABLE_TWO 0x04 //define disable_two for player 2 00000100  
#define DISABLE_THR 0x10 //define disable_thr for player 3 00010000
#define DISABLE_FOU 0x20 //define disable_fou for player 4 00100000

#define BUZZTIME 100     //define buzz time frequency for the buzzer

volatile uint8_t timercount = 0; //define & set 0 value for timercount variable 
volatile uint8_t Buzz_on = 0;    //define & set 0 value for buzz_on variable
volatile uint8_t Play_game = 0;  //define & set 0 value for play_game variable

ISR(TIMER1_COMP1_vect)           
{
	if (Buzz_on == 1)
	{
		PORTB = PORTB & 0xF7; 
		Buzz_on = 0;
		timercount++;
	}
	else
	{
		PORTB = PORTB | 0x08; 
		Buzz_on = 1;
	}

	if (timercount == BUZZTIME)
	{
		Buzz_on = 0;
		PORTB = PORTB | 0x08;
		TCCR1B = 8+0+0+0;
		Play_game = 1; 
	}
}

int main(void)
{
	// Initialization
	DDRD = 0x00; // all ports are input
	PORTD = 0xFF; // pullup all internal resistors
	
	DDRB = 0x3F; // 00111111 only PB6 and PB7 is input
	PORTB = 0xFF; // 11111111 pullups resistor active

	PIND = 0xFF;

	TCCR1B = 8+0+0+0; // generate clock for timer1 = 0
	OCR1A = 900; // compare register: interrupt every 6 ms 
	TIMSK = 64; // enable Timer1 compare match interrupt, bit 6 

	uint8_t pins = 0;
	uint8_t count = 0;
	uint8_t disable_one = 0;
	uint8_t disable_two = 0;
	uint8_t disable_thr = 0;
	uint8_t disable_fou = 0;

	uint8_t win_one = 0;
	uint8_t win_two = 0;
	uint8_t win_thr = 0;
	uint8_t win_fou = 0;

	uint8_t pressed_one = 0;
	uint8_t pressed_two = 0;
	uint8_t pressed_thr = 0;
	uint8_t pressed_fou = 0;

	uint8_t numberPlayer = 0;

	sei(); //interrupt begins

	numberPlayer = 4;
	win_one = 1; 
	win_two = 1;
	win_thr = 1;
	win_fou = 1;

	
	while (1)
	{
		pins = PIND | 0xFE;
		while (pins == 0xFF)
		{
			pins = PIND | 0xFE; //
		}
		
		PORTB = 0xFE;        // switch start LED on
		TCCR1B = 8+0+2+1;
		timercount = 0;
		Buzz_on = 1;
		Play_game = 0;

		while (Play_game == 0);

		disable_one = 0;		
		disable_two = 0;
		disable_thr = 0;
		disable_fou = 0;

		count = 0;
		pressed_one = 0;
		pressed_two = 0;
		pressed_thr = 0;
		pressed_fou = 0;

		while (count < numberPlayer - 1)
		{
			do
			{
				pins = PIND | 0xC9; // disable unused ports PD
				if ((disable_one == 1) | (win_one == 0)) pins = pins | DISABLE_ONE;
				if ((disable_two == 1) | (win_two == 0)) pins = pins | DISABLE_TWO;
				if ((disable_thr == 1) | (win_thr == 0)) pins = pins | DISABLE_THR;
				if ((disable_fou == 1) | (win_fou == 0)) pins = pins | DISABLE_FOU; 
			}
			while (pins == 0xFF);

			// disable winner button
			if (pins == 0xFD) 
			{
				disable_one = 1;
				PORTB = PORTB & 0xFD;
				pressed_one = 1;
			}

			if (pins == 0xFB) 
			{
				disable_two = 1;
				PORTB = PORTB & 0xFB;
				pressed_two = 1;
			}

			if (pins == 0xEF) 
			{
				disable_thr = 1;
				PORTB = PORTB & 0xEF;
				pressed_thr = 1;
			}

			if (pins == 0xDF) 
			{
				disable_fou = 1;
				PORTB = PORTB & 0xDF;
				pressed_fou = 1;
			}

			count++;
		}

		if (pressed_one == 0) win_one = 0;
		if (pressed_two == 0) win_two = 0;
		if (pressed_thr == 0) win_thr = 0;
		if (pressed_fou == 0) win_fou = 0;

		numberPlayer--;
		if (numberPlayer == 1) 
		{
			numberPlayer = 4;
			win_one = 1;
			win_two = 1;
			win_thr = 1;
			win_fou = 1;
		}
	}
	return 0;
}
