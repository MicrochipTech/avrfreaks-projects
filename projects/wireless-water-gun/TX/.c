#include "TX.h"

void USART_init()
{
	//Set baud rate
	UBRRL=(uint8_t)UBRRVAL;  //low byte
	UBRRH=(UBRRVAL>>8);	//high byte

	//UCSRC select to write, asynchronous mode, no parity, 1 stop bit, 8 bit size
	UCSRC=(1<<URSEL)|(3<<UCSZ0);
	
	//Enable USART Transmitter
	UCSRB=(1<<TXEN);
}

void send_tx_byte(uint8_t data)
{
    // Wait if a byte is being transmitted
	while(!(UCSRA & (1<<UDRE)));
    //while((UCSRA&(1<<UDRE)) == 0);
    // Transmit data
    UDR = data; 
}

void send_packet(uint8_t addr, uint8_t cmd)
{
	send_tx_byte(SYNC);//send synchro byte	
	send_tx_byte(addr);//send receiver address
	send_tx_byte(cmd);//send increment command
	send_tx_byte((addr+cmd));//send checksum
}


