/*

	 Usart functions (usart.h) for Atmega16 for IAR

	 Version 0.1ß		2007-06-21

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

#ifndef ENABLE_BIT_DEFINITIONS
#define ENABLE_BIT_DEFINITIONS
// Enable the bit definitions in the iom128.h file
#endif

#ifndef _USART_H_
#define _USART_H_ 1

#include <ioavr.h>
#include <intrinsics.h>

#define true 1
#define false 0

unsigned char USART_Receive( void );
int USART_Receive_time_out( void );

#define INTERRUPT_USART_RXC()	 _Pragma("vector=USART_RXC_vect")   __interrupt void signal_func_USART1_RXC(void)
#define INTERRUPT_USART_DATA() _Pragma("vector=USART_UDRE_vect")  __interrupt void signal_func_USART1_UDRE_vect(void)


void USART_Init(double baud);
void USART_Transmit( unsigned char data );
void USART_Transmit_Str(const char *str );
void USART_Flush( void );

#endif			/* _USARTUSB_H_ */
