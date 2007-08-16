	/*

	 TWI functions (twi.h) for Atmega16 and WINAVR

	 Version 0.1ß		2005-10-07

	 Copyright (C) Andreas Kingbäck 2005 (andki@itn.liu.se)

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


#ifndef _TWI_H_
#define _TWI_H_

#include <ioavr.h>
//#include <compat/twi.h>
//#include <avr/signal.h>
//#include <avr/interrupt.h>

#define BUSY 7
#define ULTRACONTROLER_TWI_ADDRESS 0x02			// TWI address

unsigned char twi_data_buffer[5];

volatile unsigned char twi_address;
volatile unsigned char twi_data;
volatile unsigned char twi_ddr;
volatile unsigned char twi_bytes;
volatile unsigned char twi_stop;
volatile unsigned char status;
volatile unsigned char retry_cnt;

void TWI_Init( void );
void TWI_SendStopCondition( void );
void TWI_SendStartCondition( void );
void TWI_SendAddress(unsigned char address);
void TWI_SendData(unsigned char data);


int TWI_write(unsigned char address, unsigned char *data, unsigned char bytes);
int TWI_read(unsigned char address, unsigned char *data, unsigned char bytes);

void TWI_Test( void );

#endif		/* _TWI_H_ */
