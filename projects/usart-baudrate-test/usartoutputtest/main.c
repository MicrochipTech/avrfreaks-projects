/*
 * usartoutputtest.c
 * Sends the letter U continuously to test baudrate and prove usart tx works
 * Coded for Mega16 AVR, works with all classic AVR8's 
 * Created: 9/28/2020 2:32:23 PM
 * Author : jim.cordill KI0BK
 */ 

#include <avr/io.h>
#include <stdlib.h>

#define F_CPU 8000000UL      //set this value to actual cpu frequency (fuse settings)
#define UART_BAUD_RATE 4800  //this is a low error baud rate with 1,2,4,or 8MHz xtal
#define UART_BAUD_SELECT(baudRate,xtalCpu)  (((xtalCpu) + 8UL * (baudRate)) / (16UL * (baudRate)) -1UL)

/* Defines for Mega16, change as needed */
 #define UART0_STATUS      UCSRA
 #define UART0_CONTROL     UCSRB
 #define UART0_CONTROLC    UCSRC
 #define UART0_DATA        UDR
 #define UART0_DATA        UDR
 #define UART0_UDRIE       UDRIE
 #define UART0_UBRRL       UBRRL
 #define UART0_UBRRH       UBRRH
 #define UART0_BIT_U2X     U2X
 #define UART0_BIT_RXCIE   RXCIE
 #define UART0_BIT_RXEN    RXEN
 #define UART0_BIT_TXEN    TXEN
 #define UART0_BIT_UCSZ0   UCSZ0
 #define UART0_BIT_UCSZ1   UCSZ1
 #define UART0_BIT_URSEL   URSEL

void uart_init( uint16_t baudrate )
{
	/* Enable USART receiver and transmitter and receive complete interrupt */
	UART0_CONTROL = (1<<UART0_BIT_RXEN)|(1<<UART0_BIT_TXEN);
	UART0_CONTROLC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0); //8N1 is default format
	#if defined(UART0_UBRRH)
	UART0_UBRRH = (unsigned char)((baudrate>>8)&0x80) ;
	#endif
	UART0_UBRRL = (unsigned char) (baudrate&0x00FF);
}

int uart_putc( char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UART0_STATUS & (1<<UDRE)) )
	;
	/* Put data into buffer, sends the data */
	UDR = data;
	return 0;
}

int main(void)
{
	/* init TXD and RXD port pins as output and input */
	DDRA = 0x00;
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	/* enabling USART overrides DDR settings for TXD/RXD pins */
	
    /*
     *  Initialize UART library, pass baudrate and AVR cpu clock
     *  with the macro 
     *  UART_BAUD_SELECT() (normal speed mode )
     *  or 
     *  UART_BAUD_SELECT_DOUBLE_SPEED() ( double speed mode)
     */
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	
	
    while (1) //forever
    {
		uart_putc( 'U' );
    }
	
	return 0;
}

