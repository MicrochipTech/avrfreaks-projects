/*  pwm.c

Atmel AVR Design Contest 2006 Registration Number AT3337

*/

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "ATmega16MIDI.h"
#include "midilookuptable.h"

unsigned char milliseconds = 0;




void pwmInit(void)
{
	cli();
	setOCR1A(0x0006); //50us
	// Initialize Timer1.
    // Enable timer1 compare interrupt
	TIMSK = (1<<OCIE1A);
	// Set PWM Phase Correct mode, CLK/8 prescaler
	TCCR1A = (1<<COM1A1)|(1<<WGM11);
	TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10);
    sei();
}


void setOCR1A(int count)
{  
    unsigned char sreg;
	sreg = SREG;
	cli();
	// Sets the compare value
	OCR1A = count;
    SREG = sreg;
}
void setICR1(int count)
{    
    unsigned char sreg;
	sreg = SREG;
	cli();
	// Sets the compare value
	ICR1 = count;
	TCNT1 = 0;
    SREG = sreg;
}

void setICR1_P(int code)
{
    unsigned char sreg;
	sreg = SREG;
	cli();
	// Sets the compare value
	ICR1H = pgm_read_byte(&midi_table_P[code][0]);
	ICR1L = pgm_read_byte(&midi_table_P[code][1]);
	TCNT1 = 0;
    SREG = sreg;
}

SIGNAL(SIG_INPUT_CAPTURE1)
{
     if (TIFR & ~(1<<ICF1)) //if Flag is set
       TIFR |= (1<<ICF1); //clear it by writing a one to ICF1

}

SIGNAL(SIG_OUTPUT_COMPARE1A)
{
     if (TIFR & ~(1<<OCF1A)) //if output compare flag is set
       TIFR |= (1<<OCF1A); //clear it by writing a one to OCF1A
}
SIGNAL(SIG_OUTPUT_COMPARE1B)
{
     if (TIFR & ~(1<<OCF1B)) //if output compare flag is set
       TIFR |= (1<<OCF1B); //clear it by writing a one to OCF1B
}
