/*
 * a.lp_mp3 - Open Source Atmel AVR based MP3 Player
 * Copyright (c) 2003-2004 K. John '2B|!2B' Crispin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
 *
 * Feedback, Bugs.... mail john{AT}phrozen.org
 *
 */ 
 
#include <avr/io.h>
#include <avr/interrupt.h>

#include "types.h"

void spi_init(void){
/*	UCSR0C = 0xc0;
	UCSR0B = 0x1e;
	UBRR0 = 1;*/
	sbi(DDRB,PB2);
	sbi(DDRB,PB3);
	cbi(PORTB,PB5);
	sbi(DDRB,PB5);

	outp(((1<<MSTR)|(1<<SPE) ), SPCR );	// enable SPI interface (8MHz/4=2MHz)
};

inline u08 spi_io(u08 data){
/*	UDR0 = data;
	while (!(UCSR0A&(1<<RXC0)));
	return UDR0;*/
	outp(data, SPDR);
	while((inp(SPSR)&(1<<SPIF)) == 0x00){};
	return inp(SPDR);
};
