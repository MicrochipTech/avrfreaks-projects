/*  This file is a part of ButterflySDK. ButterflySDK is a set of software
	modules used to program the Butterfly Evaluation Board for ATMEL.
	Emphasis is given in code re-use and in maintainability. ButterflySDK
	is compiled with GCC AVR compiler. Supporting tools are written in Python
	programming language.
	Copyright (C) 2007, Kostas Giannakakis

    ButterflySDK is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ButterflySDK is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ButterflySDK; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** @file lcd.h LCD driver module header file.
*/

#ifndef __LCD_HEADER__
#define __LCD_HEADER__

/*******************************************************************************
* Definitions
*******************************************************************************/

#define LCD_INITIAL_CONTRAST	0x0F
#define LCD_MINIMUM_CONTRAST	0x00
#define LCD_MAXIMUM_CONTRAST	0x0F

/*******************************************************************************
* Macros
*******************************************************************************/

// DEVICE SPECIFIC!!! (ATmega169) First LCD segment register
#define LCD_SET_CONTRAST_LEVEL(level) 	LCDCCR=(0x0F & level)
#define LCD_GET_CONTRAST_LEVEL()	(LCDCCR & 0x0F)

/*******************************************************************************
* Exported global variables
*******************************************************************************/

extern volatile BOOL lcdInterruptFlag;

/*******************************************************************************
* Exported Functions
*******************************************************************************/

extern void LcdInit(void);
extern void LcdAllSegments(BOOL show);
extern void LcdWriteChar(uint8_t digit, char character);
extern void LcdWriteString(const char *str);
extern void LcdWriteString_F(const char *str_F);
extern void LcdColon(BOOL show);
extern void LcdShowSymbol(uint8_t symbol, BOOL show);

extern void LcdTask(void);

#endif // __LCD_HEADER__
