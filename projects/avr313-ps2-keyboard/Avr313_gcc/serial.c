#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "serial.h"

#define ESC 0x1b
#define BUFF_SIZE 64

// const char CLR[] PROGMEM = {ESC, '[','H', ESC, '[', '2', 'J',0};

uint8_t UART_buffer[BUFF_SIZE];
volatile uint8_t *ub_inptr;
volatile uint8_t *ub_outptr;
volatile uint8_t ub_buffcnt;

#define UBR  (unsigned int)( F_CPU / (16.0 * BAUD) - 0.5 )

void init_uart(void)
{
	ub_inptr  = UART_buffer;
	ub_outptr = UART_buffer;
	ub_buffcnt = 0;

    UBRRL  = UBR;				// 19200bps @ 4 MHz
    UCSRB  = (1 << TXEN);		// TX enable
}



/*
// Send a 'clear screen' to a VT100 terminal

void clr(void)
{
puts_P(CLR);
}
*/

int putchar(int c)
{
	if (ub_buffcnt < BUFF_SIZE)
	{
		uint8_t tmp = SREG;
		cli();

		// Put character into buffer
		// Increment pointer and buffer count

		*ub_inptr++ = c;
		ub_buffcnt++;

		// Pointer wrapping

		if (ub_inptr >= UART_buffer + BUFF_SIZE)
			ub_inptr = UART_buffer;

		// Enable UART Data register empty interrupt

		UCSRB |= (1 << UDRIE);
		SREG = tmp;
		return 1;
	}
	else
	{
		// Buffer is full
		return 0;
	}
}



// ----------------------------------------------
// Interrupt driven transmitter
// ----------------------------------------------
ISR (USART_UDRE_vect)
{
	// Send next byte and increment pointer
	UDR = *ub_outptr++;

	// Pointer wrapping
	if (ub_outptr >= UART_buffer + BUFF_SIZE)
		ub_outptr = UART_buffer;

	// If buffer is empty: disable interrupt
	if(--ub_buffcnt == 0)
		UCSRB &= ~(1 << UDRIE);
}
