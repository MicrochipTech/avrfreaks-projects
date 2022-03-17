/*! \file i2ceeprom.h \brief Interface for standard I2C EEPROM memories. */
//*****************************************************************************
//
// File Name	: 'i2ceeprom.h'
// Title		: Interface for standard I2C EEPROM memories
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.04.23
// Revised		: 2003.04.23
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
///	\ingroup driver_hw
/// \defgroup i2ceeprom Interface for standard I2C EEPROM memories (i2ceeprom.c)
/// \code #include "i2ceeprom.h" \endcode
/// \par Overview
///		This library provides functions for reading and writing standard
///	24Cxxx/24LCxxx I2C EEPROM memories.  Memory sizes up to 64Kbytes are
///	supported.  Future revisions may include page-write support.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef I2CEEPROM_H
#define I2CEEPROM_H

#include "global.h"

// functions

//! Initialize I2C EEPROM interface
void i2ceepromInit(void);

//! In the I2C EEPROM at [i2cAddr], read a byte from memory location [memAddr]
u08 i2ceepromReadByte(u08 i2cAddr, u32 memAddr);

//! In the I2C EEPROM at [i2cAddr], write a byte [data] to the memory location [memAddr]
void i2ceepromWriteByte(u08 i2cAddr, u32 memAddr, u08 data);

void i2ceepromReadPage(u08 i2cAddr, u32 memAddr, u08 length, u08 *data);

void i2ceepromWritePage(u08 i2cAddr, u16 memAddr, u08 length, u08 *data);



#endif
