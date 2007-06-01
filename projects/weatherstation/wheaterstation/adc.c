
#include "ADC.h"
#include "lcd.h"
#include <util/delay.h>
#include <avr/io.h>

void setupADC()
{   
	ACSR = 0x00; //Enable analog comparator
	DDRD |= 0x22; //OC1A(PD5) and PD1 as output

	TCCR1A = 0x83; //Clear OC1A on compare match, phase correct PWM 10bit
	TCCR1B = 0x01; //Use system clock (no prescaling)
		
    OCR1AH = 0x02;
	OCR1AL = 0x00; //pulse width set to 0x0200

	TIMSK = 0x80; //Timer1 overflow interrupt enabled
	PORTD = 0x02; //Switch to temperature measurement
	sei();
}


void setupHumidity()
{   
	ACSR = 0x00; //Enable analog comparator
	DDRD |= 0x03; //(PD0 and PD1) as output
	DDRD |= 0x20; //OC1A(PD5) as output
	TCCR1A = 0x83; //Clear OC1A on compare match, phase correct PWM 10bit
	TCCR1B = 0x01; //Use system clock (no prescaling)

    OCR1AH = 0x02;
	OCR1AL = 0x00; //pulse width set to 0x0200

	TIMSK = 0x40; //Timer1 compare interrupt enabled
	
	PORTD = 0x01; //Enable humidity sensor
	sei(); //Enable interrupts

}

