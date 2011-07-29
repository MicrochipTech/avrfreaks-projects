#include "larslib.h"

volatile uint8_t t=0, r=100, g=50, b=200;
volatile int8_t dg=1, dr=1, db=1;


ISR(TIM0_COMPA_vect)
{
    PORTB &= ~((1<<PB0)|(1<<PB1)|(1<<PB2));
    if(t<b) PORTB |= (1<<PB0);
    if(t<g) PORTB |= (1<<PB1);
    if(++t<r) PORTB |= (1<<PB2);
    TCNT0 = 0;  
}


int main()
{
   // Use normal timer mode, no PWM or fancy stuff enabled
    TCCR0A = 0;

    // Timer0 no prescaler
    TCCR0B = (1<<CS00);

	// Enable timer comparator interrupt A
	TIMSK |= (1<<OCIE0A);

    // Set comparator time
    OCR0A = 30;

    DDRB = 0xff;

    sei();

    while(1)
    {
        // Do nothing. 
        _delay_ms(5);
;
		
        r+=dr;
        g+=dg;
        b+=db;

        if(r>254) dr=-1;
        else if(r<110) dr = 1;

        if(g>250) dg=-1;
        else if(g<100) dg = 1;

        if(b>250) db=-1;
        else if(b<100) db = 1;
		
    }

    return 0;
}
