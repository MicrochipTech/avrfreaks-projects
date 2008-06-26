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
 *                               MAIN FILE                                 *
 *                                                                         *
 ***************************************************************************/
//Headers
#include "config/global.h"		//GLOBAL DEFINITIONS
#include <avr/io.h>			//I/O
#include <inttypes.h>			//
#include <util/delay.h>			//SIMPLE DELAYS
#include <avr/pgmspace.h>		//PGM space read
#include "uart.h"			//UART transmission
//Main objects
UART uart;				//make UART object
//Main function
int main(void) {
	
	uart << 232 << "HELLO WORLD" << (void*)PSTR("HELLO WORLD");

	uint8_t get;

	get = uart.get();

	//uart is streaming device...
	//you can type this:
	// uart << 232 << "HELLO WORLD" << (void*)PSTR("HELLO WORLD");


	// if would you put string in PGM you must throw PSTR to (void*) like this: (void*)PSTR("TEXT in PGM")

	//every questions send to d0zoenator@gmail.com

	//tested on ATMEGA32 and ATMEGA16... i think on ATMEGA128 this library doesn't work... you can repair this... you must define registers in uart.h

	for(;;) {
	
	}
	return 0;
}
