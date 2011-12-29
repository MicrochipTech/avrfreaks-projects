/*
 * Test_Board.c
 *
 * Created: 12/23/2011 9:37:03 AM
 *  Author: Kat
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include "usart.h"
#include "usart_stdout.h"
#include "timer1.h"
#include "ping.h"

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main(void)
{
	stdout = &mystdout;
	usart_init();
	ping_init();
	timer1_init();
	sei();
	
	for (;;) // Loop forever
	{
		_delay_ms(10); // 13.107 max
		_delay_ms(10); // 13.107 max
		send_ping();
	}   
}