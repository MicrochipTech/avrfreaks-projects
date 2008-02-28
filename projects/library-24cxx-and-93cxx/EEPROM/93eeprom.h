/*! \file spi.h \brief SPI interface driver. */
//*****************************************************************************
//
// File Name	: 'spi.h'
// Title		: SPI interface driver
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 11/22/2000
// Revised		: 06/06/2002
// Version		: 0.6
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
///	\ingroup driver_avr
/// \defgroup spi SPI (Serial Peripheral Interface) Function Library (spi.c)
/// \code #include "spi.h" \endcode
/// \par Overview
///		Provides basic byte and word transmitting and receiving via the AVR
///	SPI interface.  Due to the nature of SPI, every SPI communication operation
/// is both a transmit and simultaneous receive.
///
///	\note Currently, only MASTER mode is supported.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef _93EEPROM_H
#define _93EEPROM_H

#include "global.h"


#define		ee93_port		PORTB
#define		ee93_pin		PINB
#define		ee93_ddr		DDRB

#define		ee93_sck		1
#define		ee93_miso		3
#define		ee93_mosi		2
#define		ee93_cs			0


#define		ee93_cs_h		sbi(ee93_port,ee93_cs)	,	delay
#define		ee93_cs_l		cbi(ee93_port,ee93_cs)
#define		ee93_mosi_h		sbi(ee93_port,ee93_mosi)
#define		ee93_mosi_l		cbi(ee93_port,ee93_mosi)
#define		ee93_sck_h		sbi(ee93_port,ee93_sck)
#define		ee93_sck_l		cbi(ee93_port,ee93_sck)


#define		chip46			7			//	7 address
#define		chip56			8			//	8 address
#define		chip66			9			//	9 address
#define		chip86			11			//	11 address



volatile unsigned char ee93_chip;
volatile unsigned char ee93_error;

void eeprom_93_init(void);

// spiTransferByte(u08 data) waits until the SPI interface is ready
// and then sends a single byte over the SPI port.  The function also
// returns the byte that was received during transmission.
u08 eeprom_93_byte(u08 data);

void	ee93_we(void);						//	enable write
void	ee93_wd(void);						//	disable write
u08		ee93_erase(void);
u08		ee93_read(u16 address);
u08 	ee93_write(u16 address,u08 data);
u08 	read_chip();
u08		ee93_r_byte(void);
void	ee93_w_byte(u08 data);
#endif
