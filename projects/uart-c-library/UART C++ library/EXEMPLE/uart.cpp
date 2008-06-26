/***************************************************************************
 *   Copyright (C) 2008 by Damian Kmiecik                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *                             UART FILE                                   *
 *                                                                         *
 ***************************************************************************/
//Headers
#include "config/global.h"		//GLOBAL DEFINITIONS
#include <avr/io.h>			//I/O
#include <inttypes.h>			//
#include <util/delay.h>			//SIMPLE DELAYS
#include "config/uart.h"		//UART CONFIG
#include "uart.h"			//UART
#include <avr/pgmspace.h>		//PGM space read
//Definitions

//Constructor UART()
UART::UART() {
	//Setting bauderate and enabling interfaces
	uint16_t baud;
	#if F_CPU < 20000000L && defined(U2X)
	 #define UART_BAUDRATE_ ((F_CPU / (8L * UART_BAUDRATE)) - 1L)
	 baud = static_cast<uint16_t>(UART_BAUDRATE_);
	 UCSRA = _BV(U2X);
	 UBRRH = static_cast<uint8_t>(baud>>8);
  	 UBRRH = static_cast<uint8_t>(baud);
	#else
	 #define UART_BAUDRATE_ ((F_CPU / (16L * UART_BAUDRATE)) - 1L)
	 baud = static_cast<uint16_t>(UART_BAUDRATE_);
	 UBRRH = static_cast<uint8_t>(baud>>8);
	 UBRRL = static_cast<uint8_t>(baud);
	#endif
	#ifdef UART_RECIVE
	UCRB = _BV(TXEN) | _BV(RXEN);
	#else
	UCRB = _BV(TXEN);
	#endif
}
//Put text on UART
void UART::put(char* text) {
	while(*text){
		this->send(*text);
		text++;
	}
}
#ifdef UART_PGM
//Put text from PGM
void UART::put_p(const char *text) {
	while(pgm_read_byte(text)){
		this->send(pgm_read_byte(text));
		*text++;
	}
}
#endif
//Put number on UART
void UART::put(int16_t number, uint8_t base) {
	char buf[16] = {0};
	register char i = 14;
	char m = 0;
	if(number < 0) {
		number = number * (-1);
		m = 1;
	}
	for(; number && i ; --i, number /= base)
		buf[i] = "0123456789abcdef"[number % base];
	if(m) {
		buf[i] = '-';
		this->put(&buf[i]);
	} else {
		this->put(&buf[i+1]);
	}
}
//Transmit one byte
void UART::send(uint8_t data) {
	loop_until_bit_is_set(UCRA, UDRE);
	UDR = data;	
}
#ifdef UART_RECIVE
//Recive one byte
uint8_t UART::get(void) {
	loop_until_bit_is_set(UCRA, RXC);
	return UDR;
}
#endif
#ifdef UART_STREAMS
//
UART &operator<<(UART &rs232, uint16_t val) {
	rs232.put(val, 10);
	return rs232;
}
//
UART &operator<<(UART &rs232, char* val) {
	rs232.put(val);
	return rs232;
}
#ifdef UART_PGM
//Compilator not recognize type const char*, i have to throwing this to void* before streaming
//plz use (void*)PSTR(text) if you want put text from PGM
UART &operator<<(UART &rs232, void* val) {
	rs232.put_p((const char*)val);
	return rs232;
}
#endif
#endif

