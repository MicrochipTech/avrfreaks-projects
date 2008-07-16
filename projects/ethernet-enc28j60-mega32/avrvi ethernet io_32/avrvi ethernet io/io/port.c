/*
,-----------------------------------------------------------------------------------------.
| io/port
|-----------------------------------------------------------------------------------------
| this file implements a very basic port io lib
|
| Author   : {{removed according to contest rules}}
|            -> circuitcellar.com avr design contest 2006
|            -> Entry #AT2616
|
|-----------------------------------------------------------------------------------------
| License:
| This program is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free Software
| Foundation; either version 2 of the License, or (at your option) any later
| version.
| This program is distributed in the hope that it will be useful, but
|
| WITHOUT ANY WARRANTY;
|
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with
| this program; if not, write to the Free Software Foundation, Inc., 51
| Franklin St, Fifth Floor, Boston, MA 02110, USA
|
| http://www.gnu.de/gpl-ger.html
`-----------------------------------------------------------------------------------------*/
#include "port.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

void port_set_portbit(unsigned char portchar, unsigned char pin, unsigned char val){
	//volatile char *port;
	unsigned char data;

	//only allow bit0-bit7
	pin = (pin-'0') & 0x07;

	if (val)
		data = (1<<pin);
	else
		data = ~(1<<pin);

	//WARNING: make sure not to set/clear used IO pins !!!
	switch (portchar){
		case('A'):
			if (val)
				PORTA |= data;
			else
				PORTA &= data;
			break;
		case('C'):
		//	if (pin != 3)
		//		return;

			if (val)
			//	PORTC |= data;
			//	PORTC |=(1<<6);
				PORTC |=(1<<pin);
			else
			//	PORTC &= data;
			//	PORTC &= ~(1<<6);
				PORTC &=~(1<<pin);
			break;
		case('D'):
			if (pin != 7)
				return;
			if (val)
				PORTD |= data;
			else
				PORTD &= data;
			break;
		//case('B'):	port = &PORTB; break;		
		default:    return;
	}
	return;

/*
	//strange, this does not work (set=1 is ok, clear does not work...)
	if (val)
		val = (1<<pin);

	if (val){
		*port |= (val);
		softuart_putc('S');
	}else{
		(*port) = (*port) & (~(val));
		softuart_putc('C');
	}
*/
}

unsigned char port_get_portbit(unsigned char portchar, unsigned char pin){
	volatile char *port;
	switch (portchar){
		case('A'):	port = &PORTA; break;
		case('B'):	port = &PORTB; break;
		case('C'):	port = &PORTC; break;
		case('D'):	port = &PORTD; break;
		default:    return 0;
	}

	pin = (pin-'0') & 0x07;

	if (*port & (1<<pin))
		return 1;
	//else 
	return 0;
}
