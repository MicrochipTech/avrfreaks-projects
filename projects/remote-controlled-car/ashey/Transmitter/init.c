void initDDRs(void)
{
	DDRB = _BV(RED_LED) | _BV(GREEN_LED) | _BV(IR);
	DDRC = 0;
	DDRD = 0;
	PORTB = PORTC = PORTD = 0;
	/* All LEDs off , TSOP pins in High Z mode */
}

void setupTimer1(void)
{
   //8 Bit timer counter 2 
   TIMSK = TIMSK | _BV(OCIE2) ; //output Compare Interrupt
   TCCR2 = TCCR2 | _BV(WGM21) | _BV(CS20) ; // for CTC mode
   
}
void setupTimer2(void)
{   // setting the Timer 16 Bit .....
    TCCR1B = TCCR1B | _BV(WGM12)  | _BV(CS10); // CTC mode and no prescaler
	TIMSK  = TIMSK  | _BV(OCIE1A) ;        // compare match interrupt 
}

