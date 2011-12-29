/*
 * timer1.c
 *
 * Created: 12/23/2011 5:17:17 PM
 *  Author: Kat
 */
 
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "timer1.h"

#define ME_PS 340 // Speed of sound in meters per second
#define CM_PS 34029 // Speed of sound in centimeters per second
#define FT_PS 1126 // Speed of sound in feet per second
#define IN_PS 13512 // Speed of sound in inches per second

// Formula for speed of sound for this circuit is (time/2 * rate/clock)
// time = duration lapsed from signal sent to received
//        we divide by two because we are only measuring one way
// rate = speed of sound in X per second
// clock = avr clock in Hz

// The clock is divided by the prescaler and multiplied by 2 
// simple arithmetic changes the formula to (time * rate) / (2 * clock)
// this way the we can define the big number constant
// by waiting to divide we don't loose the decimals till the end.
#define PING_MATH ((F_CPU / 8) * 2)

unsigned long time;

void timer1_init(void)
{
   TCCR1B |= (1 << ICES1); // Set input capture edge to rising (positive), stopped clock
   ACSR   |= (1 << ADC);   // Turn Off Analog Comparator on Timer1
}

ISR(TIMER1_CAPT_vect)
{
   static int x;
   if(x == 0)
   {
       TCCR1B |=  (1 << CS11);  // Start Timer 1 clk/8 from prescaler (F_CPU / 8)
       TCCR1B &= ~(1 << ICES1); // Set input capture edge to falling (negative)
       x = 1;
   }
   else if(x == 1)
   {
       time = TCNT1;
       TCCR1B &= ~(1 << CS11);   // Disable Timer 1 clock
       TCNT1   =   0x00;         // Clear Timer 1 High and Low registers
       TIMSK1 &= ~(1 << ICIE1);  // Disable Timer/Counter1 Input Capture
       DDRD   |=  (1 << DDD6);   // Set PD6 as output
       TCCR1B |=  (1 << PORTD6); // Set input capture edge to rising (positive)
       x = 2;
	   printf("%lu.%lucm\n",((time * CM_PS) / PING_MATH ), ((time * CM_PS) % PING_MATH ));
   }
   else
   {
       x = 0;
   }
}