/*--------------------------------------------------------------*/
#include "iocompat.h"

void
Timer1_prescaler_init(void)
{
    switch (Timer1_prescale) {
	case 0	:
	    TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
	    break;

        case 1	: 
	    TCCR1B |= _BV(CS10);
	    TCCR1B &= ~(_BV(CS11) | _BV(CS12));
	    break;

        case 8	: 
	    TCCR1B |= _BV(CS11);
	    TCCR1B &= ~(_BV(CS10) | _BV(CS12));
	    break;

        case 64 : 
	    TCCR1B |= _BV(CS10) | _BV(CS11);
	    TCCR1B &= ~_BV(CS12);
	    break;

        case 256 : 
	    TCCR1B |= _BV(CS12);
	    TCCR1B &= ~(_BV(CS10) | _BV(CS11));
	    break;
	
        case 1024 : 
	    TCCR1B |= _BV(CS12) | _BV(CS10);
	    TCCR1B &= ~_BV(CS11);
	    break;
	}
}

/* -------------------------------------------- */
void
ioinit (void)
{
/* outputs setup */
/* disable analog comparator, needed?, seems not */
/*    ACSR = _BV(ACD); */

/*  set LED bits as outputs */
    LED_1_DDR |= _BV(LED_1_BIT);
#if !defined PRODUCTION_BOARD
    LED_2_DDR |= _BV(LED_2_BIT);
#endif
/*  pullups for switches */
    KEY_1_PORT |= _BV(KEY_1_BIT);
    KEY_2_PORT |= _BV(KEY_2_BIT);
    KEY_3_PORT |= _BV(KEY_3_BIT);
#if defined PRODUCTION_BOARD
    KEY_4_PORT |= _BV(KEY_4_BIT);
    KEY_5_PORT |= _BV(KEY_5_BIT);
    KEY_6_PORT |= _BV(KEY_6_BIT);
#endif
    /*DDRC = _BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3); *//* LCD data */
    /* let the library initialize it */

/*--------------------------------------------------------------*/
/* Timer 1 is 16-bit 						*/
/* normal mode, "4", CTC, period in OCR1A, prescaler/64,	*/
/* start timer, this will be 10ms period			*/
/*--------------------------------------------------------------*/
     TCCR1B &= ~(_BV(WGM13) | _BV(WGM11) | _BV(WGM10));
     TCCR1B |= _BV(WGM12);

     OCR1A = half_period;

/*     OCR1B = power_level; */

     TIMSK |= _BV(OCIE1A); /* | _BV(OCIE1B); */

     Timer1_prescaler_init();

/*------------------------------------------------------*/
/* 	serial port baudrate setup using setbaud.h	*/
/*------------------------------------------------------*/
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif

/*------------------------------------------------------*/
/* 8 data bit, 1 stop (default), no parity (default)    */
    UCSR0C = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
    UCSR0B |= _BV(TXEN0); /* | _BV(RXEN0);	 */ /* rx/tx enable	*/
/*    UCSR0B |= _BV(RXCIE0); */ /* receiver interrupt enable	*/

/*------------------------------------------------------*/
/* setup INT0 as RISING edge, enable int0		*/
/*    MCUCR |= _BV(ISC00) | _BV(ISC01);
    GICR |= _BV(INT0);*/
/*------------------------------------------------------*/
    sei();		/* enable interrupts	 	*/
/*    wdt_enable(WDTO_1S); */
}
