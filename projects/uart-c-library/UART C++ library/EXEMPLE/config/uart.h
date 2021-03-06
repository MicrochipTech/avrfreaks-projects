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
 *                           UART CONFIG FILE                              *
 *                                                                         *
 ***************************************************************************/
#ifndef UART_H_CONFIG			//
#define UART_H_CONFIG			//

#define UART_HARD			//Hardware UART
//TODO://#define UART_SOFT		//Software UART

#define UART_STREAMS			//define if use streams
#define UART_PGM			//define to enable PGM read
#define UART_RECIVE			//define to enable read from UART

#define UART_BAUDRATE	57600		//UART baudrate

#ifdef UART_SOFT
 #define UART_PORT	PORTD		//UART port
 #define UART_DDR	DDRD		//UART ddr register
 #define UART_PINR	PIND		//UART pin register
 #define UART_RXD_PIN	5		//RXD pin number
 #define UART_TXD_PIN	4		//TXD pin number
 #define UART_RXD_DDR	UART_DDR	//
 #define UART_RXD_PORT	UART_PORT	//
 #define UART_RXD_PINR	UART_PINR	//
 #define UART_TXD_PORT	UART_PORT	//
 #define UART_TXD_DDR	UART_DDR	//
#else
 #if defined (__AVR_ATmega128__)
	 #define USR UCSR0A
	 #define UCR UCSR0B
	 #define UDR UDR0
	 #define UBRR UBRR0L
	 #define EICR EICRB
	 #define UCRA	UCSRA
 	 #define UCRB	UCSRB
 	 #define UCRC	UCSRC
 #endif
 #if defined (__AVR_ATmega32__)
 	#define UCRA	UCSRA
 	#define UCRB	UCSRB
 	#define UCRC	UCSRC
 #endif
 #if defined (__AVR_ATmega16__)
 	#define UCRA	UCSRA
 	#define UCRB	UCSRB
 	#define UCRC	UCSRC
 #endif
#endif

#endif					//

