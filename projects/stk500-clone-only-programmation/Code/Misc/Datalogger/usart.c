/*****************************************************************************
*Author: 	Jidan Al-Eryani,	jidan@gmx.net 
*Date: 	3.4.2007
*
* Functions for using USART on ATmega128(L). To support other ATMEL MCU's, just change the name of registers.
*
****************************************************************************/

#include <avr/io.h>

#include "usart.h"


// intiliaze the USART
void USART_init(void){
	// set baud rate. Caution: when baud rate gets high, take the values from the table in the MCU datasheet
	UBRR0H = 0; //(uint8_t) (BAUD_UBRR>>8);
	UBRR0L = BAUD_UBRR; //(uint8_t) BAUD_UBRR;

	#ifdef ENABLE_U2X
		// Enable double speed mode (U2X) 
		UCSR0A |= (1<<U2X);
	#endif

	// Enable receiver and transmitter //
	UCSR0B |= (1<<RXEN) | (1<<TXEN);

	// Default frame format: 8 data bits, no parity, 1 stop bit
	UCSR0C |= (1<<UCSZ1)|(1<<UCSZ0); //0b00000110
}

void USART_Transmit( uint8_t data ){
	while ( !( UCSR0A & (1<<UDRE)) ); // Wait for empty transmit buffer
	UDR0 = data; // Put data into buffer, sends the data
}

uint8_t USART_Receive( void )
{
	// Wait for data to be received 
	while ( !(UCSR0A & (1<<RXC)) );

	/* Get and return received data from buffer */
	return UDR0;
}

