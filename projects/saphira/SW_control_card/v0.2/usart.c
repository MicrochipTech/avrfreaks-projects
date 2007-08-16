/*

	 USART functions (usart.c) for Atmega16 for WINAVR

	 Version 0.1ß		2007-06-18

	 Copyright (C) Andreas Kingbäck  (andki@itn.liu.se)

   This program is free software; you can redistribute it and/or modify it 
	 under the terms of the GNU General Public License as published by the 
	 Free Software Foundation; either version 2 of the License, or 
	 (at your option) any later version.

   This program is distributed in the hope that it will be useful, 
	 but WITHOUT ANY WARRANTY; without even the implied warranty of 
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	 See the GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License along with this program; 
	 if not, write to the Free Software Foundation, Inc., 
	 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#pragma language=extended

#ifndef ENABLE_BIT_DEFINITIONS
#define ENABLE_BIT_DEFINITIONS
// Enable the bit definitions in the iom128.h file
#endif

#include <ioavr.h>
#include <stdio.h>
#include <intrinsics.h>
#include "usart.h"

void USART_Init(double baud)
{
	/* Set baud rate */
	double bbaud;
	bbaud = (8000000/(8*baud))-1;
	UBRRH = (unsigned char)((unsigned int)bbaud >> 8);
	UBRRL = (unsigned char)bbaud & 0xFF;
	UCSRB = (1<<RXEN)|(1<<TXEN); 
	UCSRA |= 2; 													// U2X
}

void USART_Transmit_Str(const char *str)
{
	/* Wait for empty transmit buffer */
	int i=0;
	while (str[i] != 0)
	{
		while ( !( UCSRA & (1<<UDRE)) );
		UDR = str[i];										// Put data into buffer, sends the data 
		i++;
	}
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
	UDR = data;
}

int USART_Receive_time_out( void )
{
	int timeout = 10000;										// time out value
	/* Wait for data to be received */
	int data;
	while (((UCSRA & 0x80) == 0) && (timeout > 0)) {
		__delay_cycles(8000);      
		timeout --;
	}
	if ((UCSRA & 0x80) == 0)
		data = -1;											// No data availible
	else
		data=UDR;
	return data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	unsigned char data;
	while ((UCSRA & 0x80) == 0);
	data=UDR;
	/* Get and return received data from buffer */
	return data;
}

void USART_Flush( void )
{
	unsigned char dummy;
	while ( UCSRA & (1<<RXC) ) dummy = UDR;
}

unsigned char USART_ReadUCSRC( void )
{
	unsigned char ucsrc;
	/* Read UCSRC */
	ucsrc = UBRRH;
	ucsrc = UCSRC;
	return ucsrc;
}

INTERRUPT_USART_RXC()// Interrupt driven USART Receive
{
	unsigned char slask;
	__disable_interrupt();
	slask = UCSRA;		        // Must read UCSRA with dummy read :-/
	slask = UDR;					
	__enable_interrupt();
}

