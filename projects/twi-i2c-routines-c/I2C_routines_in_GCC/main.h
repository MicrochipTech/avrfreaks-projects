/***************************************************************************
 *   Copyright (C) 2005 by Dimitar Dimitrov                                *
 *   dinuxbg at gmail.com                                                  *
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
 ***************************************************************************/

/** Macroses, constants, etc*/

#ifndef _MAIN_H_
#define _MAIN_H_

/** CPU clock frequency (in Hz).*/
#define CPU_CLOCK	11059200L


#define FALSE 	0
#define TRUE	(!FALSE)


/* Return codes. Generally zero means success, and non-zero means failure. */
#define RC_SUCCESS	0
#define RC_FAIL		1
#define RC_FAIL1	2
#define RC_FAIL2	3

/* GCC should optimizie them to single asm instructions.*/
#define clrbit(PORT,BIT)	((PORT)&=~(1<<(BIT)))
#define setbit(PORT,BIT)	((PORT)|=(1<<(BIT)))
#define toggle_bit(PINX,BIT)	{if(bit_is_set(PINX,BIT)) clrbit(PINX,BIT); else setbit(PINX,BIT);}

/** Memory section: EEPROM.*/
#define __eeprom	__attribute__ ((section (".eeprom")))
/** Memory section: FLASH (program memory).*/
#define __flash		__attribute__ ((__progmem__))

/** Delay routine for a specific number of microseconds based on the CPU clk.*/
#define __delay_us(DELAY)	_delay_loop_2((((long)DELAY)*(CPU_CLOCK))/(4000000L))

/*
 * Macros for extracting the ASCII value of a number. hex_N -> N is which
 * half-byte we want. Typical usage (8 bit integer value):
 *    putchar(hex_1(val)); putchar(hex0(val));
 */
#define hex_0(byte)     ( (((byte)&0x0f)>9) ? (0x37+((byte)&0x0f)) : ('0'+((byte)&0x0f)) )
#define hex_1(byte)     ( ((((byte)&0xf0)>>4)>9) ? (0x37+(((byte)&0xf0)>>4)) : ('0'+(((byte)&0xf0)>>4)) )
#define hex_2(byte)     ( ((((byte)&0xf00)>>8)>9) ? (0x37+(((byte)&0xf00)>>8)) : ('0'+(((byte)&0xf00)>>8)) )
#define hex_3(byte)     ( ((((byte)&0xf000)>>12)>9) ? (0x37+(((byte)&0xf000)>>12)) : ('0'+(((byte)&0xf000)>>12)) )

/** Structure for storing flags and boolean types */
typedef struct {
	unsigned char b0:1;
	unsigned char b1:1;
	unsigned char b2:1;
	unsigned char b3:1;
	unsigned char b4:1;
	unsigned char b5:1;
	unsigned char b6:1;
	unsigned char b7:1;
} bitfield_t;

/** Global bit field variable for the whole project. */
extern volatile bitfield_t bits_store1;

/** Example for a global bit variable (global flag). */
#define some_global_flag		bits_store1.b0

#endif //_MAIN_H_
