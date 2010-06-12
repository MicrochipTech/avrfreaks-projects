#include "RX.h"

void USART_init()
{
		//Set baud rate
	UBRRL=(uint8_t)UBRRVAL;  //low byte
	UBRRH=(UBRRVAL>>8);	//high byte

	//UCSRC select to write, asynchronous mode, no parity, 1 stop bit, 8 bit size
	UCSRC=(1<<URSEL)|(3<<UCSZ0);

	//Enable USART Receiver 
	UCSRB=(1<<RXEN);
}

uint8_t get_rx_byte()
{
    // Wait until a byte has been received
	while(!(UCSRA & (1<<RXC)));
    //while((UCSRA&(1<<RXC)) == 0);
    // Return received data
	char data = UDR;
    return data;
}

signed int receive_packet()
{
	char addr = get_rx_byte();
	char cmd = get_rx_byte();
	char cksum = get_rx_byte();

	if (cksum == (addr + cmd))
	{
		return cmd;
	}
	else
	{
		return -1;
	}
}
