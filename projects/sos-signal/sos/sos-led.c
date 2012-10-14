/*
 * sos_led.c
 *
 * Created: 2012-10-07 14:16:52
 *  Author: Laptop
 */ 


#include <avr/io.h>
#define F_CPU 4000000 
#include <util/delay.h>

int main(void)
{
	DDRB = 0x03; 
	
    while(1)
    {
		// 3 short
        PORTB = 0x01;
		_delay_ms(50);
		PORTB = 0x00;
		_delay_ms(50);
		PORTB = 0x01;
		_delay_ms(50);
		PORTB = 0x00;
		_delay_ms(50);
		PORTB = 0x01;
		_delay_ms(50);
		PORTB = 0x00;
		_delay_ms(50);
		
		// 3 long
		PORTB = 0x01;
		_delay_ms(200);
		PORTB = 0x00;
		_delay_ms(50);
		PORTB = 0x01;
		_delay_ms(200);
		PORTB = 0x00;
		_delay_ms(50);	
		PORTB = 0x01;
		_delay_ms(200);
		PORTB = 0x00;
		_delay_ms(50);
		
		// 3 short
        PORTB = 0x01;
        _delay_ms(50);
        PORTB = 0x00;
        _delay_ms(50);
        PORTB = 0x01;
        _delay_ms(50);
        PORTB = 0x00;
        _delay_ms(50);
        PORTB = 0x01;
        _delay_ms(50);
        PORTB = 0x00;
        _delay_ms(50);
		
		// pause 1s
		_delay_ms(250);
		_delay_ms(250);
		_delay_ms(250);
		_delay_ms(250);
    }
}