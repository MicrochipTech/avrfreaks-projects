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
 *                           UART HEADER FILE                              *
 *                                                                         *
 ***************************************************************************/
#ifndef UART_H
#define UART_H

//Headers
#include "config/global.h"		//GLOBAL DEFINITIONS
#include <avr/io.h>			//I/O
#include <inttypes.h>			//
#include <avr/pgmspace.h>		//PGM space read
#include "config/uart.h"		//CONFIG UART
//UART class
class UART {
	public:
		UART();						//INIT
		void put(char *text);				//PUT TEXT ON LCD
		#ifdef UART_PGM
		void put_p(const char *text);			//PUT TEXT FROM PGM SPACE
		#endif
		void put(int16_t number, uint8_t base);		//PUT NUMBER ON LCD
		#ifdef UART_RECIVE
		uint8_t get(void);				//Get single byte
		#endif
	private:
		void send(uint8_t data);			//Send single byte	
};
//
#ifdef UART_STREAMS
UART &operator<<(UART &rs232, uint16_t val);
UART &operator<<(UART &rs232, char* val);
#ifdef UART_PGM
UART &operator<<(UART &rs232, void* val);
#endif
#endif

#endif

