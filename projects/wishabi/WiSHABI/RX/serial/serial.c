#include "serial.h"

extern unsigned char RECEIVED;

void USART_Flush (void)
{
	unsigned char dummy;
	while (UCSRA & (1<<RXC) )
	{
		dummy = UDR;
	}
}

/* Initialize USART */
void USART_Init( unsigned int baudrate )
{
	/* Set the baud rate */
	UBRRH = (unsigned char) (baudrate>>8);                  
	UBRRL = (unsigned char) baudrate;
	
	/* Enable UART receiver and transmitter */
	UCSRB |= ( ( 1 << RXCIE ) | ( 1 << RXEN ) | ( 1 << TXEN ) ); 
	
	/* Set frame format: 8 data, no parity, 1 stop (for devices without Extended IO) */
	UCSRC |= (1<<URSEL)|(0<<USBS)|(1<<UCSZ1)|(1<<UCSZ0);

	USART_Flush();
}
 

void USART_Transmit (unsigned char to_send)
{
	/* Wait until the transmit buffer is empty */
	/* UCSRA - USART Control And Status Register A */
	/* UDRE - USART Data Register Empty Flag */
	while ( !(UCSRA & (1<<UDRE)) );

	/*Send the data to the UDR - USART Data Register */
	UDR = to_send;
}

unsigned char USART_Receive (void)
{
	TIMER=0;
	/* Wait for data to be received or a timeout*/
	while ((!(UCSRA & (1<<RXC))) & (TIMER<TEN_MILLISECONDS)) ;
	
	/* Get and Return received data from the buffer */
	return (UDR);

}


void SendInitConfirmation (void)
{
	USART_Transmit(0x0D);
	USART_Transmit(0x0D);
	USART_Transmit(0x0A);
	USART_Transmit('U');
	USART_Transmit('S');
	USART_Transmit('A');
	USART_Transmit('R');
	USART_Transmit('T');
	USART_Transmit('_');
	USART_Transmit('T');
	USART_Transmit('X');
	USART_Transmit(' ');
	USART_Transmit('O');
	USART_Transmit('K');
	USART_Transmit('!');
	USART_Transmit(0x0D);
	USART_Transmit(0x0D);
	USART_Transmit(0x0A);
}

