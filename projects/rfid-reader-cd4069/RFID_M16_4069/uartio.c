/*
    Copyright (C) 2004 by Michal Vanka. All rights reserved.

    This file is part of ERFID FIRMWARE ONE, the firmware for ERFID1 board.
    For more information see http://www.vanka.net

    ERFID FIRMWARE ONE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ERFID FIRMWARE ONE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ERFID FIRMWARE ONE; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    uartio.c
    
*/

#define 	outb(addr, data)   addr = (data)
#define 	inb(addr)   (addr)
#define 	outw(addr, data)   addr = (data)
#define 	inw(addr)   (addr)
#define 	outp(addr, data)   addr = (data)
#define 	inp(addr)   (addr)
#define 	BV(bit)   (1<<(bit))
#define 	cbi(reg, bit)   reg &= ~(BV(bit))
#define 	sbi(reg, bit)   reg |= (BV(bit))
#define 	cli()   __asm__ __volatile__ ("cli" ::)
#define 	sei()   __asm__ __volatile__ ("sei" ::)
#define 	DDR(x)   ((x)-1)
#define 	PIN(x)   ((x)-2)

#include <avr/io.h>
#include "uartio.h"

void uart_init(uint8_t sp)
{
	outb(UBRRL,sp);  ///23 for 9600 bd, 5 for 38k4, 3=57k6 and crystal 3.686 MHz
	outb(UCSRC,(1<<URSEL)|(1<<USBS)|(1<<UCSZ1)|(1<<UCSZ0)); // ;2stopbits, 8bits
	outb(UCSRB,(1<<TXEN)|(1<<RXEN)|(1<<RXCIE)); 	//; enable tx enable rx enable int
}

void uart_send_string(char *s)
{
	while(*s) {
		while(!bit_is_set(UCSRA,UDRE));  //wait until tx register is empty
		outb(UDR,*s++);
	}
}

void uart_send_char(uint8_t c)
{
	while(!bit_is_set(UCSRA,UDRE));  //wait until tx register is empty
	outb(UDR,c);
}

