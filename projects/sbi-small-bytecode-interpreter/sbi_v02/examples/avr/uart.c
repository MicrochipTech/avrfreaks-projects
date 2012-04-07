// AVR306: Using the AVR UART in C
// Routines for polled UART
// Last modified: 02-06-21
// Modified by: AR

/* Includes */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "uart.h"

/* Initialize UART */
void InitUART(int baudrate)
{
	//UBRR = baudrate;                  /* Set the baud rate */
	UBRRH = (UART_CALC_BAUDRATE(baudrate)>>8) & 0xFF;
	UBRRL = (UART_CALC_BAUDRATE(baudrate) & 0xFF);
	UCR = ( (1<<RXEN) | (1<<TXEN) );  /* Enable UART receiver and transmitter */
}

/* Read and write functions */
unsigned char ReceiveByte(void)
{
	while ( !(USR & (1<<RXC)) ) 	/* Wait for incoming data */
		;			                /* Return the data */
	return UDR;
}

void TransmitByte(unsigned char data)
{
	while ( !(USR & (1<<UDRE)) )
		; 			                /* Wait for empty transmit buffer */
	UDR = data; 			        /* Start transmission */
}

void SPrint(char *data)
{
	int i;
	for (i=0; i<strlen(data); i++)
	{
		TransmitByte(data[i]);
	}
}

void SPrintN(long n)
{
	unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
	unsigned long i = 0;
	uint8_t base = 10;

	if (n == 0) {
		TransmitByte('0');
		return;
	} 

	while (n > 0) {
		buf[i++] = n % base;
		n /= base;
	}

	for (; i > 0; i--)
		TransmitByte((char) (buf[i - 1] < 10 ?
		'0' + buf[i - 1] :
		'A' + buf[i - 1] - 10));
}

/*int SAvailable(void)
{
	if (USR & (1<<RXC)) return 1; else return 0;
}

unsigned char SRead(void)
{
	return UDR;
}
*/
