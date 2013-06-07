/*
 * Usart.c
 *
 * Created: 02/03/2013 11:02:56
 *  Author: Calogiuri Enzo Antonio
 */ 
#include "CX80.h"

/*Enale and configure avr usart module, used for control video chip*/
void Seput_CX80_Usart(void)
{
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	
	/*
	UCSRB = (1 << TXEN);					//Only transmitter
	UCSRC = _BV(UCSZ1) | _BV(UCSZ0);		//8bit, no parity e 1 stop bit
	*/
	UCSRA = (0 << RXC) | (0 << TXC) | (0 << UDRE) | (0 << FE) | (0 << DOR) | (0 << UPE) | (0 << U2X) | (0 << MPCM);
	UCSRB = (0 << RXCIE) | (0 << TXCIE) | (0 << UDRIE) | (0 << RXEN) | (1 << TXEN) | (0 << UCSZ2) | (0 << RXB8) | (0 << TXB8);
	UCSRC = (1 << URSEL) | (0 << UMSEL) | (0 << UPM1) | (0 << UPM0) | (0 << USBS) | (1 << UCSZ1) | (1 << UCSZ0) | (0 << UCPOL);
}

/*Send a char via usart*/
void CX80_Usart_SendChar(char c)
{
	loop_until_bit_is_set(UCSRA, UDRE);
	
	UDR = c;
}