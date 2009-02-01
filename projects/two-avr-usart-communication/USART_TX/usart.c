//C file to handle USART initialization, transmit, and receive
//USART function is for 8 data bits, no parity, and 1 stop bit
//Coded by Adam Kadolph, 7-23-08

#include <avr/io.h>

void USART_init(unsigned int ubrr)
{
	//set baud rate
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	
	//Enable receiver and transmitter
	UCSRB = (1<<RXEN)|(1<<TXEN);
	
	//set frame format: 8 data, 1 stop bit
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}

void USART_Transmit(unsigned char data)
{
	//wait for empty transmit buffer
	while(!(UCSRA & (1<<UDRE)));
	
	//put data into buffer, sends the data
	UDR = data;
}
unsigned char USART_Receive(void)
{
	//wait for data to be received
	while(!(UCSRA & (1<<RXC)));
	
	//get and return received data from buffer
	return UDR;
}
