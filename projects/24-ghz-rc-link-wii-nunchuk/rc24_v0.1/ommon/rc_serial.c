/*================================================================================
	2.4 Ghz R/C Control
	rc_serial.c
	The code in this file is based on wiring_serial.c which is part of Arduini 
	software distribution. Accordingly, the copyright notice from that file is
	included below.
================================================================================*/

/*
  wiring_serial.c - serial functions.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.c 248 2007-02-03 15:36:30Z mellis $
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdio.h>
#include <stdarg.h>
#include "rc_global.h"
#include "rc_serial.h"

uint8_t rxBuffer[RC_SERIAL_BUF_SIZE];
uint8_t rxBufferHead = 0;
uint8_t rxBufferTail = 0;

void Serial_Init(long baud)
{
	UBRR0H = ((F_CPU / 16 + baud / 2) / baud - 1) >> 8;
	UBRR0L = ((F_CPU / 16 + baud / 2) / baud - 1);
	
	// enable rx and tx
	UCSR0B |= _BV(RXEN0);
	UCSR0B |= _BV(TXEN0);
	
	// enable interrupt on complete reception of a byte
	UCSR0B |= _BV(RXCIE0);
}

void Serial_Write(uint8_t c)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;

	UDR0 = c;
}

uint8_t Serial_Read(void)
{
	// if the head isn't ahead of the tail, we don't have any characters
	if (rxBufferHead == rxBufferTail) {
		return -1;
	} else {
		uint8_t c = rxBuffer[rxBufferTail];
		rxBufferTail = (rxBufferTail + 1) % RC_SERIAL_BUF_SIZE;
		return c;
	}
}

ISR (SIG_USART_RECV)
{
	uint8_t c = UDR0;

	uint8_t i = (rxBufferHead + 1) % RC_SERIAL_BUF_SIZE;

	// if we should be storing the received character into the location
	// just before the tail (meaning that the head would advance to the
	// current location of the tail), we're about to overflow the buffer
	// and so we don't write the character or advance the head.
	if (i != rxBufferTail) {
		rxBuffer[rxBufferHead] = c;
		rxBufferHead = i;
	}
}

void Serial_PrintIntegerInBase(unsigned long n, unsigned long base)
{ 
	unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
	unsigned long i = 0;

	if (n == 0) {
		Serial_Write('0');
		return;
	} 

	while (n > 0) {
		buf[i++] = n % base;
		n /= base;
	}

	for (; i > 0; i--) {
		Serial_Write(buf[i - 1] < 10 ? '0' + buf[i - 1] : 'A' + buf[i - 1] - 10);
	}
}

void Serial_PrintDec(long n)
{
	Serial_PrintIntegerInBase(n, 10);
}

void Serial_PrintHex(long n)
{
	Serial_PrintIntegerInBase(n, 16);
}

void Serial_PrintStr(const char *s)
{
	while ( *s ) {
		Serial_Write(*s++);
	}
}
