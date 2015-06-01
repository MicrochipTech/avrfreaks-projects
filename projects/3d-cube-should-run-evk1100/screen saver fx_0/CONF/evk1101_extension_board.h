/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief AT32UC3B EVK1101 extension board for EVK1101 header file.
 *
 * This file contains definitions and services related to the features of the
 * EXT1102 board.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 AT32UC3B devices can be used.
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


#ifndef _EVK1101_EXTENSION_BOARD_H_
#define _EVK1101_EXTENSION_BOARD_H_

#include "compiler.h"

/*! \name GPIO and SPI Connections of the Nokia LCD
 */
//! @{
#define LCD_NOKIA_RESET_PIN         AVR32_PIN_PA09
#define LCD_NOKIA_SPI               (&AVR32_SPI0)
#define LCD_NOKIA_SPI_NPCS          0
#define LCD_NOKIA_SPI_SCK_PIN       AVR32_SPI0_SCK_0_0_PIN
#define LCD_NOKIA_SPI_SCK_FUNCTION  AVR32_SPI0_SCK_0_0_FUNCTION
#define LCD_NOKIA_SPI_MISO_PIN      AVR32_SPI0_MISO_0_0_PIN
#define LCD_NOKIA_SPI_MISO_FUNCTION AVR32_SPI0_MISO_0_0_FUNCTION
#define LCD_NOKIA_SPI_MOSI_PIN      AVR32_SPI0_MOSI_0_0_PIN
#define LCD_NOKIA_SPI_MOSI_FUNCTION AVR32_SPI0_MOSI_0_0_FUNCTION
#define LCD_NOKIA_SPI_NPCS_PIN      AVR32_SPI0_NPCS_0_0_PIN
#define LCD_NOKIA_SPI_NPCS_FUNCTION AVR32_SPI0_NPCS_0_0_FUNCTION
//! @}


#endif  // _EVK1101_EXTENSION_BOARD_H_
