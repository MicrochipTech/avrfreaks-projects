#include <avr/io.h>
#include <util/delay.h>
#include "usart.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
extern unsigned char i;

#ifdef _STDIO_H_
static FILE mystdout = FDEV_SETUP_STREAM(usart_transmit, NULL,
										 _FDEV_SETUP_WRITE);
void usart_transmit(char c, FILE *stream)
{
	if (c == '\n')
		usart_transmit('\r', stream);
	while (!( UCSRA & (1<<UDRE)));
	UDR = c;
}
#else
void usart_transmit(char c )
{
	/* Wait for empty transmit buffer */
#if ! _USE_USART_INTERRUPT
	while (!( UCSRA & (1<<UDRE)));
#endif
	/* Put data into buffer, sends the data */
	UDR = c;
}
/*---------------------------------------------------------------*/
void puts(char *string )
{
	while ( *string )
		usart_transmit( *string++ );
}
/*---------------------------------------------------------------*/
void puts_h( unsigned char count, unsigned long data )
{
	unsigned char i, temp;
	char dataString[] = "0x        ";

	for(i=count; i>0; i--)
	{
		temp = data % 16;
		if((temp>=0) && (temp<10)) dataString [i+1] = temp + 0x30;
		else dataString [i+1] = (temp - 10) + 0x41;

		data = data/16;
	}
	puts (dataString);
}
/*---------------------------------------------------------------*/
#endif

#if _USE_USART_INTERRUPT
char *usart_buffer;
unsigned char buf_cnt,buf_len;
#endif

/*---------------------------------------------------------------*/
void usart_init( unsigned int ubrr)
{
#ifdef _STDIO_H_
	stdout = &mystdout;
#endif

	//stdout = &mystdout;
	/* Set baud rate */
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)(ubrr);
	/* Enable receiver and transmitter */
	//UCSRA|=(1<<U2X);
	UCSRB = (1<<RXEN)|(1<<TXEN)
#if _USE_USART_INTERRUPT
		|(1<<UDRIE)|(1<<RXCIE);
#else
		|(1<<RXCIE);
#endif
	/* Set frame format: 8data, 1 stop bit */
	UCSRC = (1<<URSEL)|(3<<UCSZ0);

	/*Set PORTD.RXD As input ,enable pull-up*/
	PORTD &=~(1<<0);
	DDRD |=(1<<1); //TXD is output
}
/*---------------------------------------------------------------*/
unsigned char usart_receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) )
		;
	/* Get and return received data from buffer */
	return UDR;
}
/*---------------------------------------------------------------*/

#if  _USE_USART_INTERRUPT
ISR(SIG_USART_DATA)
{
	UDR = *usart_buffer++;
	if((buf_cnt++ ==USART_BUFF_SIZE)|(buf_cnt==buf_len))
	{
		UCSRB &=~(1<<UDRIE);
	}
}
#endif
unsigned char r,j;
extern unsigned char i;
extern unsigned int _TCNT[60];
ISR(SIG_USART_RECV)
{
	//usart_transmit (UDR);
	//i=0;
	//r=UDR;
	//if(r=='r')
	//{
	//	for(j=0;j<60;j++)
	//	{
	//		printf_P(PSTR("_TCNT[%d]=%x\n"),j,_TCNT[j]);
	//	}
	//	i=0;
	//}
}
