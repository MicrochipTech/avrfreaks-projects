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
//_____  I N C L U D E S ___________________________________________________
#include "spi.h"
#include "delay.h"
#include "gpio.h"
#include "lcd_nokia.h"
#include "compiler.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________

//_____ D E C L A R A T I O N S ____________________________________________

extern avr32_spi_t *spi ;                 //!< SPI Instance dedicated for LCD Display

void lcd_nokia_select(void);
void lcd_nokia_select(void)
{
  spi_selectChip(LCD_NOKIA_SPI, LCD_NOKIA_SPI_NPCS);
}

void lcd_nokia_unselect(void);
void lcd_nokia_unselect(void)
{
  spi_unselectChip(LCD_NOKIA_SPI, LCD_NOKIA_SPI_NPCS);
}

void lcd_nokia_reset_low(void);
void lcd_nokia_reset_low(void)
{
  gpio_enable_gpio_pin(LCD_NOKIA_RESET_PIN);
  gpio_clr_gpio_pin(LCD_NOKIA_RESET_PIN);
}

void lcd_nokia_reset_high(void);
void lcd_nokia_reset_high(void)
{
  gpio_enable_gpio_pin(LCD_NOKIA_RESET_PIN);
  gpio_set_gpio_pin(LCD_NOKIA_RESET_PIN);
}

void lcd_nokia_send(unsigned char type, unsigned short data);
void lcd_nokia_send(unsigned char type, unsigned short data)
{
    if(type)
    {
        data |=0x0100;                                      
    }
    spi_write( LCD_NOKIA_SPI, (unsigned short) data );
}

void lcd_nokia_gotoXY(unsigned char x, unsigned char y);
void lcd_nokia_gotoXY(unsigned char x, unsigned char y)
{
  x += 2;                  // for some reason starts at 2
  lcd_nokia_send(LCDCommand,PASETP);
  lcd_nokia_send(LCDData,x);
  lcd_nokia_send(LCDData,SCR_WIDE);

  lcd_nokia_send(LCDCommand,CASETP);
  lcd_nokia_send(LCDData,y);
  lcd_nokia_send(LCDData,SCR_HEIGHT-1);                               // End Column to display to
}

void lcd_nokia_putpixel(unsigned char color, unsigned char x, unsigned char y)
{
  x += 2;                  // for some reason starts at 2
  lcd_nokia_send(LCDCommand,PASETP);
  lcd_nokia_send(LCDData,x);
  lcd_nokia_send(LCDData,SCR_WIDE);

  lcd_nokia_send(LCDCommand,CASETP);
  lcd_nokia_send(LCDData,y);
  lcd_nokia_send(LCDData,SCR_HEIGHT-1);

  lcd_nokia_send(LCDCommand,RAMWRP);
  lcd_nokia_send(LCDData,color);
}

void lcd_nokia_putbuffer(unsigned char *buffer)
{
  
  lcd_nokia_send(LCDCommand,PASETP);
  lcd_nokia_send(LCDData,2);
  lcd_nokia_send(LCDData,SCR_WIDE);

  lcd_nokia_send(LCDCommand,CASETP);
  lcd_nokia_send(LCDData,0);
  lcd_nokia_send(LCDData,SCR_HEIGHT-1);

  lcd_nokia_send(LCDCommand,RAMWRP);
  for(int i = 0; i < (SCR_WIDE * SCR_HEIGHT); i++) {
	  lcd_nokia_send(LCDData, ((buffer[i] & 0b11100000) >> 5) * 2);
	  lcd_nokia_send(LCDData, ((buffer[i] & 0b00011100) >> 2) * 2);
	  lcd_nokia_send(LCDData, (buffer[i] & 0b00000011) * 5);
  }
}

void lcd_nokia_init(void)
{
    lcd_nokia_select();

    lcd_nokia_reset_low();
    delay_ms(50);   //fake! 50ms needed
    lcd_nokia_reset_high();
    delay_ms(50);   //fake! 50ms needed
    
    lcd_nokia_send(LCDCommand,SLEEPOUT); // Sleep out
    delay_ms(10);
	 
    lcd_nokia_send(LCDCommand,DISPON);		// Display ON
}

void lcd_nokia_clear_screen(unsigned char color)
{
 
  lcd_nokia_send(LCDCommand,PASETP);
  lcd_nokia_send(LCDData,2);
  lcd_nokia_send(LCDData,SCR_WIDE-2);

  lcd_nokia_send(LCDCommand,CASETP);
  lcd_nokia_send(LCDData,0);
  lcd_nokia_send(LCDData,SCR_HEIGHT);

  lcd_nokia_send(LCDCommand,RAMWRP);
  for(int i = 0; i < SCR_WIDE * SCR_HEIGHT; i++) {
	  lcd_nokia_send(LCDData, ((color & 0b00011100) >> 2) * 2);
	  lcd_nokia_send(LCDData, ((color & 0b11100000) >> 5) * 2);
	  lcd_nokia_send(LCDData, (color & 0b00000011) * 5);  }
}

