/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief LCD Nokia Driver
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (C) 2006-2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _LCD_NOKIA_H_
#define _LCD_NOKIA_H_

#include <avr32/io.h>
#include "CONF/evk1101_extension_board.h"


/*! \name 9th Bit Value for Serial Communication
 */
//! @{
#define LCDCommand      0
#define LCDData         1
//! @}

/*! \name Display Resolution
 */
#define SCR_WIDE        132
#define SCR_HEIGHT      132


// Color Panel
#define DarkBlue    0x02
#define LightBlue   0x03
#define DarkGreen   0x14
#define LightGreen  0x1c
#define DarkRed     0x60
#define LightRed    0xe0
#define White       0xff
#define Black       0x00
#define Yellow      0xdd
#define Purple      0x62


// Command List for PHILIPPS Controller
#define	NOPP	   0x00
#define SWRESET    0x01 
#define	BSTRON	   0x03
#define SLEEPIN    0x10
#define	SLEEPOUT   0x11
#define	NORON	   0x13
#define	INVOFF	   0x20
#define INVON      0x21
#define	SETCON	   0x25
#define DISPOFF    0x28
#define DISPON     0x29
#define CASETP     0x2A
#define PASETP     0x2B
#define RAMWRP     0x2C
#define RGBSET	   0x2D
#define	MADCTL	   0x36
#define	COLMOD	   0x3A
#define DISCTR     0xB9
#define	EC		   0xC0
//! @}
/*! \brief Low Level Send Command/Data Function
 *  \param type Command/Data
 *  \param data Data to transmit
 */
extern void lcd_nokia_send(unsigned char type, unsigned short data);

/*! \brief Driver Initialization
 */
extern void lcd_nokia_init(void);

/*! \brief Put Pixel Function
 *  \param color
 *  \param x coordinate (x value)
 *  \param y coordinate (y value)
 */
extern void lcd_nokia_putpixel(unsigned char color, unsigned char x, unsigned char y);

/*! \brief Put Buffer Function
 *  \param *buffer
 */
extern void lcd_nokia_putbuffer(unsigned char *buffer);

/*! \brief Goto  Function
 *  \param x coordinate (x value)
 *  \param y coordinate (y value)
 */
extern void lcd_nokia_gotoXY(unsigned char x, unsigned char y);

/*! \brief Clear Screen  Function
 *  \param color
 */
extern void lcd_nokia_clear_screen(unsigned char color);

#endif  // _LCD_NOKIA_H_
