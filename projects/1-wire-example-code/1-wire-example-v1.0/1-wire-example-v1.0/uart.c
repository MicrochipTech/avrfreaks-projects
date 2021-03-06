
#include <util/delay.h>
#include <avr/io.h>

#include "uart.h"

/*****************************************************************************\
|* Definitions
\*****************************************************************************/
#define BAUDRATE					(19200)
#define UBRR_VAL					(F_CPU / 16 / BAUDRATE - 1)
#define RX_BUFSIZE  				80

/*****************************************************************************\
|* Set up the serial interface
\*****************************************************************************/
void initUart(void)
	{
	/*************************************************************************\
	|* Zero the control registers first, so the configuration isn't rejected 
	\*************************************************************************/
	UCSR0A	= 0;
	UCSR0B	= 0;
	UCSR0C	= 0;
	
	/*************************************************************************\
	|* Set the baudrate @ 20MHz, this equates to 115200
	\*************************************************************************/
	UBRR0H	= 0;
	UBRR0L	= 10;
	
	/*************************************************************************\
	|* Enable the receiver and transmitter 
	\*************************************************************************/
	UCSR0B  = (1<< RXEN0) | (1<<TXEN0);
	
	/*************************************************************************\
	|* 8-N-1 
	\*************************************************************************/
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
	}
	
/*****************************************************************************\
|* Serial output routine
\*****************************************************************************/
int uart_putchar(char c, FILE *stream)
	{
  	if (c == '\n')
    	uart_putchar('\r', stream);
  	
	loop_until_bit_is_set(UCSR0A, UDRE0);
  	UDR0 = c;

  	return 0;
	}
	
/*****************************************************************************\
| Serial input routine 
\*****************************************************************************/
int uart_getchar(FILE *stream)
	{
	loop_until_bit_is_set(UCSR0A, RXC0);
	
	if (UCSR0A & (1<<FE0))
	  	return _FDEV_EOF;
	
	if (UCSR0A & (1<<DOR0))
  		return _FDEV_ERR;
	return (int) UDR0;
	}
	
/*****************************************************************************\
| Read a word from stdin - terminates with ' ' or \n'. "Alternative" to scanf
\*****************************************************************************/
int uart_getword(char *buf, int max)
	{
	uint8_t num = 0;
	char c = (char) uart_getchar(NULL);
	while ((num < max) && (c != ' ') && (c != '\r') && (c != '\n'))
		{
		buf[num++] = c;
		c = (char) uart_getchar(NULL);
		}
	buf[num] = '\0';
	return 1;		
	}
	
