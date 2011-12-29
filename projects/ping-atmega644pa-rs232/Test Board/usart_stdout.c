/*
 * usart_stdout.c
 *
 * Created: 12/23/2011 9:37:03 AM
 *  Author: Kat
 */

#include <avr/io.h>
#include <stdio.h>
#include "usart_stdout.h"

void uart_putchar(char c, FILE *stream)
{
	if (c == '\n')
	  uart_putchar('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}