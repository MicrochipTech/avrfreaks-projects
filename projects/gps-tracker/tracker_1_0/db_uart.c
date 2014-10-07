/*
 * db_uart.c
 *
 * Created: 14.3.2014 12:06:54
 *  Author: PjV
 */ 

#include <avr/io.h>
#include "macros.h"
#include "db_uart.h"

/**************************************************************//**
 *
 *****************************************************************/
void db_uart_init(void)
{
    /* set debug baud rate */
	UBRR0L = DB_BAUDRATE;		//low byte
	UBRR0H = (DB_BAUDRATE>>8); //high byte	

    /* use Control&Status -register's default values 
     * UCSR0A == default values
	 * default frame format: 8 bit, no parity, 1 bit
	 * UCSR0C == default values, 
	*/
    
	/* enable serial transmitter */
    UCSR0B = (1 << TXEN0);
}

/**************************************************************//**
 *
 *****************************************************************/

void db_uart_putc(unsigned char c)
{
#if DEBUG
    if(c == '\n')
        db_uart_putc('\r');

    /* wait until transmit buffer is empty */
    while(!(UCSR0A & (1 << UDRE0)));

    /* send next byte */
    UDR0 = c;
#endif
}
	
/**************************************************************//**
 *
 *****************************************************************/
void db_uart_puts(const char* str)
{
#if DEBUG	
	while(*str)
	db_uart_putc(*str++);
#endif	
}

/**************************************************************//**
 *
 *****************************************************************/
void db_uart_puts_p(PGM_P str)
{
#if DEBUG	
	while(1)
	{
		uint8_t b = pgm_read_byte_near(str++);
		if(!b)
		break;

		db_uart_putc(b);
	}
#endif
}


