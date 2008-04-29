/*****************************************************************************
*Author: 	Jidan Al-Eryani,	jidan@gmx.net 
*Date: 	3.4.2007
*
* Functions for using USART.
*Tested with ATmega128L and  FT232R(UART<>USB chip). To support other ATMEL MCU's, just change the name of registers.
*
****************************************************************************/

#include <avr/io.h>
#include <stdlib.h>
#include "usart.h"


// intiliaze the USART
void usart_init(unsigned char option){
	#ifdef ENABLE_U2X
		// Enable double speed mode (U2X) 
		UCSR0A |= (1<<U2X);
	#endif
	
	// set baud rate. Caution: when baud rate gets high, take the values from the table in the MCU datasheet
	UBRR0H = 0; 
	UBRR0L = BAUD_UBRR;
	
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); //RX Complete Interrupt Enable; Receiver Enable; Transmitter Enable
	
	if(option==1)
		UCSR0B|=(1<<RXCIE0); //RX Complete Interrupt Enable

	// Default frame format: 8 data bits, no parity, 1 stop bit
	UCSR0C = (1<<UCSZ1)|(1<<UCSZ0); //0b00000110
}

void usart_transmit(unsigned char data ){
	while ( !( UCSR0A & (1<<UDRE)) ); // Wait for empty transmit buffer
	UDR0 = data; // Put data into buffer, sends the data
}

//transmit strings
void usart_transmits(char *data){
    while(*data != 0){
        usart_transmit(*data);
        data++;
	}
}

void usart_transmitd(unsigned char *data, unsigned int count){
    while(count != 0){
        usart_transmit(*data);
        data++;
		count--;
	}
}

//transmit integers up to 32-bit long
void usart_transmiti(unsigned long uint32){
	char s_uint32[11];
	ultoa(uint32, s_uint32, 10);
	usart_transmits(s_uint32);
}

unsigned char usart_receive( void )
{
	// Wait for data to be received 
	while ( !(UCSR0A & (1<<RXC)) );

	/* Get and return received data from buffer */
	return UDR0;
}
