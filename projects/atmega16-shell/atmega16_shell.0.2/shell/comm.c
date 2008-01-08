#include "comm.h"

#include <avr/io.h>

void USART_Init(unsigned int ubrr) {
	// Baud rate registers (hi/lo)
	UBRRH = (unsigned char) (ubrr >> 8);
	UBRRL = (unsigned char) ubrr;
	
	// Enable transmitting and receiving
	UCSRB = (1<<RXEN)|(1<<TXEN);
	
	// 
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
}

unsigned char USART_Receive( void )
{
  /* Wait for data to be received */
  while ( !(UCSRA & (1<<RXC)) )
        ;
  /* Get and return received data from buffer */
  return UDR;
}


void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	
	/* Put data into buffer, sends the data */
	UDR = data;
}

void send_string(const unsigned char *s) {
	register char i = 0;
	
	while (s[i]) {
		USART_Transmit(s[i]);
		i++;
	}
}

void send_char(unsigned char c) {
	USART_Transmit(c);
}

unsigned int receive_string	(unsigned char *s, unsigned int buffer_length, unsigned char echo) {
	char buffering = 1, c;
	unsigned int length = 0;
	
	if (buffer_length > 0) {
		while (buffering) {
			/* fetch a character */
			c = USART_Receive();
			if (c == 13) {
				/* we received [enter], finish, we do not add it to the buffer, 
				 * we do not add it to the length 
				 */
				buffering = 0;
				s[length] = '\0';		 /* null terminate the buffer */
			} else {
				/* read a character, add to the buffer */
				s[length] = c;
				length++;
				
				if (echo == 1) {
					USART_Transmit(c);
				}
				
				if (length == buffer_length - 1) {
					/* buffer full, end */
					s[length] = '\0';
					buffering = 0;
				}
			}
		}
	}

	return length;
}

void receive_char(unsigned char *c) {
}
