#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

#define F_CPU 4000000UL

void displayNumber(int number);

volatile int num;
volatile int ones;
volatile int tens;
volatile int hundreds;
volatile int thousands;

int main(void)
{
	DDRB = 0xff;
	PORTB = 0x00;
	DDRD = 0xff;
	PORTD = 0x01;



	TCCR0 |= (1<< CS02);	//256 PRESCALER
	TCCR0 |= (1<< WGM01);	//CTC MODE
	OCR0 = 65;	//4ms //setting how fast to refresh the display


	TIMSK |= (1<<OCIE0);	//enable the timer0 compare interrupt
	sei();					//enable global interrupts

	int temp = 0;
	num = 0;	//initializng the "counter"

	while(1)
	{
			num++; //increment the counter variable

			//splitting the number into separate numbers
			//to display on the 4 digit 7-segment display
			thousands = floor(num / 1000);
			temp = num % 1000;
			hundreds = floor(temp/100);
			temp = num % 100;
			tens = floor(temp/10);
			ones = temp % 10;

			temp = 0;

		//setting how fast to increment the counter value
		_delay_ms(500);
	}
	return 0;
}

//displaying the number on the 7 segment display
/*
	How i wired the display

	A - 0
	B - 1
	C - 2
	D - 3
	E - 4
	F - 5
	G - 6
	DP - 7
*/
void displayNumber(int number)
{
	switch(number)
		{
			case 0:
			PORTB = 0x3f;
			break;
			case 1:
			PORTB = 0x06;
			break;
			case 2:
			PORTB = 0x5b;
			break;
			case 3:
			PORTB = 0x4f;
			break;
			case 4:
			PORTB = 0x66;
			break;
			case 5:
			PORTB = 0x6d;
			break;
			case 6:
			PORTB = 0x7d;
			break;
			case 7:
			PORTB = 0x07;
			break;
			case 8:
			PORTB = 0x7f;
			break;
			case 9:
			PORTB = 0x67;
			break;
			default:
			PORTB = 0x80;
			break;
		}
}

ISR(TIMER0_COMP_vect)
{

	//selecting which digit to display
	//timing controlled by Timer0
	switch(PORTD)
	{
		case 0x08:
			displayNumber(hundreds);
			PORTD = 0x04;
			break;
		case 0x04:
			displayNumber(tens);
			PORTD = 0x02;
			break;
		case 0x02:
			displayNumber(ones);
			PORTD = 0x01;
			break;
		case 0x01:
			displayNumber(thousands);
			PORTD = 0x08;
			break;
		default:
			break;

	}
}
