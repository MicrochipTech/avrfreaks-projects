#define F_CPU 4000000UL
#include <avr/io.h>
#include <avr/signal.h>
#include <stdio.h>
#include <util/delay.h>

int main()
{
	DDRD = 0xff; //ouput
	DDRB = 0x00; //input
	
	while(1)
	{
		PORTD = PINB;
	}

	return 0;
	

}
