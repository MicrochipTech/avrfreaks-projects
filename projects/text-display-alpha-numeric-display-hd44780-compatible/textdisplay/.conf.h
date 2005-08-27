/* ********************************************************************************
 *  textdisplay module for use with AVR series microcontrollers from Atmel        *
 ******************************************************************************** */
 
/*
    Copyright (C) 2005  Erik Häggström <xpress@xpress.mine.nu>

    Copyright notice:

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _TEXTDISPLAY_CONF_H_
#define _TEXTDISPLAY_CONF_H_ 1

/* ********************************************************************************
 *  SETTINGS                                                                      *
 ******************************************************************************** */

/* Select memory signal (RS) */
#define TEXTDISPLAY_RS_DDR			DDRC
#define TEXTDISPLAY_RS_PORT			PORTC
#define TEXTDISPLAY_RS_P				PC3

/* Read/write signal (RW) */
#define TEXTDISPLAY_RW_DDR			DDRC
#define TEXTDISPLAY_RW_PORT			PORTC
#define TEXTDISPLAY_RW_P				PC4

/* Enable signal (E) */
#define TEXTDISPLAY_E_DDR				DDRC
#define TEXTDISPLAY_E_PORT			PORTC
#define TEXTDISPLAY_E_P					PC5


/* Set I/O mode for textdisplay bus */
/* #define TEXTDISPLAY_IO_PORT 1 */
#define TEXTDISPLAY_IO_PINS 1

#define TEXTDISPLAY_BUS_WIDTH 4
/* #define TEXTDISPLAY_BUS_WIDTH 8 */

/* I/O mode port */
#if defined(TEXTDISPLAY_IO_PORT)
	/* Set data port */ 
	#define TEXTDISPLAY_DATA_DDR	DDRB
	#define TEXTDISPLAY_DATA_PORT	PORTB
	#define TEXTDISPLAY_DATA_PIN	PINB

/* I/O mode pins */
#elif defined(TEXTDISPLAY_IO_PINS)
	/* Set data pins */
	#define TEXTDISPLAY_DATA_DDR0		DDRB
	#define TEXTDISPLAY_DATA_PORT0	PORTB
	#define TEXTDISPLAY_DATA_PIN0		PINB
	#define TEXTDISPLAY_DATA_P0			PB0

	#define TEXTDISPLAY_DATA_DDR1		DDRB
	#define TEXTDISPLAY_DATA_PORT1	PORTB
	#define TEXTDISPLAY_DATA_PIN1		PINB
	#define TEXTDISPLAY_DATA_P1			PB1

	#define TEXTDISPLAY_DATA_DDR2		DDRB
	#define TEXTDISPLAY_DATA_PORT2	PORTB
	#define TEXTDISPLAY_DATA_PIN2		PINB
	#define TEXTDISPLAY_DATA_P2			PB2

	#define TEXTDISPLAY_DATA_DDR3		DDRB
	#define TEXTDISPLAY_DATA_PORT3	PORTB
	#define TEXTDISPLAY_DATA_PIN3		PINB
	#define TEXTDISPLAY_DATA_P3			PB3

	#define TEXTDISPLAY_DATA_DDR4		DDRB
	#define TEXTDISPLAY_DATA_PORT4	PORTB
	#define TEXTDISPLAY_DATA_PIN4		PINB
	#define TEXTDISPLAY_DATA_P4			PB4
	
	#define TEXTDISPLAY_DATA_DDR5		DDRB
	#define TEXTDISPLAY_DATA_PORT5	PORTB
	#define TEXTDISPLAY_DATA_PIN5		PINB
	#define TEXTDISPLAY_DATA_P5			PB5
	
	#define TEXTDISPLAY_DATA_DDR6		DDRD
	#define TEXTDISPLAY_DATA_PORT6	PORTD
	#define TEXTDISPLAY_DATA_PIN6		PIND
	#define TEXTDISPLAY_DATA_P6			PD6
	
	#define TEXTDISPLAY_DATA_DDR7		DDRD
	#define TEXTDISPLAY_DATA_PORT7	PORTD
	#define TEXTDISPLAY_DATA_PIN7		PIND
	#define TEXTDISPLAY_DATA_P7			PD7
#endif

#endif
