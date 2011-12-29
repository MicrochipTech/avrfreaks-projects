/*
 * usart.c
 *
 * Created: 12/23/2011 9:37:03 AM
 *  Author: Kat
 */

#include <avr/io.h>
#include "usart.h"

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

void usart_init(void)
{
   UCSR0B |= (1 << RXEN0)  | (1 << TXEN0);  // Turn on the transmission and reception circuitry
   UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Use 8-bit character sizes

   UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
   UBRR0L = BAUD_PRESCALE;        // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
}