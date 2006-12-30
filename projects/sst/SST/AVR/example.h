#ifndef ARM_EXAMPLE_H_
#define ARM_EXAMPLE_H_

/**************************************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *************************************************************************************************/


#include <avr/io.h>


#define LED1_PORT   PORTA
#define LED1_PORTD  DDRA
#define LED1_BIT    PA0

#define LED2_PORT   PORTA
#define LED2_PORTD  DDRA
#define LED2_BIT    PA1

#define SW1_PORT    PORTC
#define SW1_PIN     PINC
#define SW1_PORTD   DDRC
#define SW1_BIT     PC0

#define LED0        LED1_BIT
#define LED1        LED2_BIT


#define IS_BTN_PRESS()      (0 == (SW1_PIN & _BV(SW1_BIT)))

#define LED_OFF(_led_)      do { LED2_PORT &= ~_BV(_led_); } while (0)
#define LED_ON(_led_)      do { LED2_PORT |= _BV(_led_); } while (0)

#define LED_TOGGLE(_led_) \
    do { \
        if (LED2_PORT & _BV(_led_)) \
        { \
            LED_OFF(_led_); \
        } \
        else \
        { \
            LED_ON(_led_); \
        } \
    } while (0)

#define TIMER100MS_ON()    do { TIFR |= _BV(OCF1A); TIMSK |= _BV(OCIE1A); TCCR1B |= _BV(CS11) | _BV(CS10); } while (0)
#define TIMER100MS_OFF()   do { TIMSK &= ~_BV(OCIE1A); TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10)); } while (0)

void ioinit(void);


#endif /*ARM_EXAMPLE_H_*/
