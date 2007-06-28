/*
avr_uart.h

Provide functions to set up uart.

Copyright 2007, 2012
Henrik Björkman www.eit.se/habl

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

If you do modify it please make a note about it in the history 
section below.

History

2005-02-16
Modified for atmega8 and renamed. 
Henrik Björkman

2007-03-15
Modified for atmega88.
Henrik Björkman

2012-05-14
Corrected baudrate on ATMEGA88
Henrik Björkman

2012-05-22
Small fix to compile on atmega168
Henrik Björkman

*/


/* -------------------------------------------------------------------------
 * BUGS
 * ------------------------------------------------------------------------- */
/*
// port_or is not defined in iomacro.h (which gets included in io.h) while
// __port_or has a definition. This seems to be a bug in iomacro.h
#define port_or __port_or
// ditto
#define port_and __port_and
*/

/* -------------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------------- */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "avr_cfg.h"
#include "avr_uart.h"

/* compatibilty macros for old style */
/* for new design instead of "sbi(DDRC,PC5);" use "DDRC|= _BV(PC5);" */
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef outp
#define outp(value,sfr) (_SFR_BYTE(sfr) = value)
#endif

#ifndef inp
#define inp(sfr) (_SFR_BYTE(sfr))
#endif



/* -------------------------------------------------------------------------
 * local variables and defines
 * ------------------------------------------------------------------------- */




#ifdef __AVR_ATmega8__
#define UART_OUTBUF_EMPTY() (!( UCSRA & (1<<UDRE)))
#define UART_INBUF_EMPTY() (!(UCSRA & (1<<RXC)))
#elif (defined __AVR_ATmega88__) || (defined __AVR_ATmega168__)
#define UART_OUTBUF_EMPTY() (!( UCSR0A & (1<<UDRE0)))
#define UART_INBUF_EMPTY() (!(UCSR0A & (1<<RXC0)))
#else
#error
#endif



void uart_init(void)
{
  //ubrr_value = 103;
  unsigned int ubrr_value = (AVR_FOSC/(16L*BAUDRATE))-1; // AVR_FOSC is larger than a 16 bit int so be carefull here;

#ifdef __AVR_ATmega8__

  /* Set baud rate */
  UBRRH = (unsigned char)(ubrr_value>>8);
  UBRRL = (unsigned char)ubrr_value;

  //Setting this bit UCSRA bit 1 (U2Xn) the baudrate is doubled, remember to adjust calculation of ubrr_value.
  //UCSRA |= _BV(1);


  /* Enable receiver and transmitter */
  UCSRB = (1<<RXEN)|(1<<TXEN);

  /* Set frame format: 8data, 2stop bit */
  UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);

  /* enable RX interrupt */
  //UCSRB |= (1<<RXCIE);

  // Perhaps activate internal pull up for serial data input? Could reduce noice if unconnected.
  // PORTD |= _BV(PD0);

#elif (defined __AVR_ATmega88__ || __AVR_ATmega168__)
  // The code below is tested on ATmega88. ATmega168 is probably same as ATmega88 but I have not tested it.

  /* Set baud rate */
  UBRR0H = (unsigned char)(ubrr_value>>8);
  UBRR0L = (unsigned char)ubrr_value;

  // Setting this bit UCSR0A bit 1 (U2Xn) the baudrate is doubled,
  // remember to adjust calculation of ubrr_value.
  //UCSR0A |= _BV(1);

  /* Enable receiver and transmitter */
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);

  /* Set frame format: 8data, 2stop bit */
  UCSR0C = (1<<USBS0)|(3<<UCSZ00);

  // URSEL not needed in atmega88?

  /* enable RX interrup */
  //UCSR0B |= (1<<RXCIE0);

  // activate internal pull up on PD0, used for serial data input (active low signal).
  PORTD |= _BV(PD0);


#else
#error MCU not yet supported. If you implement one please let me know (www.eit.se/habl).
#endif
}



/* send if not using interrupt */
void uart_putchar( unsigned char data )
{
  /* Wait for empty transmit buffer */
  while ( UART_OUTBUF_EMPTY() )
  {
#ifdef ENABLE_WDT
    wdt_reset();
#endif
  }

  /* Put data into buffer, sends the data */
#ifdef __AVR_ATmega8__
  UDR = data;
#elif (defined __AVR_ATmega88__ || __AVR_ATmega168__)
  UDR0 = data;
#else
#error
#endif
}



/* receive if not using interrupt */
/* wait until a character is available */
unsigned char uart_waitchar( void )
{
  /* Wait for data to be received */
  while ( UART_INBUF_EMPTY() )
  {
#ifdef ENABLE_WDT
    wdt_reset();
#endif
  }

  /* Get and return received data from buffer */
#ifdef __AVR_ATmega8__
  return UDR;
#elif (defined __AVR_ATmega88__ || __AVR_ATmega168__)
  return UDR0;
#else
#error
#endif
}

/* receive if not using interrupt */
/* return -1 if no character was available */
int uart_getchar(void)
{
  /* Wait for data to be received */
  if (UART_INBUF_EMPTY())
  {
#ifdef ENABLE_WDT
    wdt_reset();
#endif
	return -1;
  }

  /* Get and return received data from buffer */
#ifdef __AVR_ATmega8__
  return UDR;
#elif (defined __AVR_ATmega88__ || __AVR_ATmega168__)
  return UDR0;
#else
#error
#endif
}


#ifdef WELCOME_STRING
void uart_print_P(const char *addr)
{
  char c; 
  while ((c = pgm_read_byte(addr++))) 
  {
    uart_putchar(c);
  }
}
#endif

