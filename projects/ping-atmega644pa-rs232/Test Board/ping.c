/*
 * ping.c
 *
 * Created: 12/23/2011 5:21:36 PM
 *  Author: Kat
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "ping.h"

void ping_init()
{
   DDRD  |=  (1 << DDD6);   // Set PD6 as output
   PORTD &= ~(1 << PORTD6); // Drive PD6 low (0v)
}

void send_ping()
{
   TIMSK1 &= ~(1 << ICIE1);  // Disable Timer/Counter1 interrupt
   DDRD   |=  (1 << DDD6);   // Set PD6 as output
   PORTD  |=  (1 << PORTD6); // Drive PD6 high (5v)
   _delay_us(5); // Delay for 5 microseconds
   PORTD  &= ~(1 << PORTD6); // Drive PD6 low (0v)
   DDRD	  &= ~(1 << DDD6);   // Set PD6 as input
   PORTD  &= ~(1 << PORTD6); // Pull-up resistor off
   TIMSK1 |=  (1 << ICIE1);  // Enable Timer/Counter1 interrupt
}