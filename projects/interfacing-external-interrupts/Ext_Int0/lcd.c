//*****************************************************************************
//
// File Name	: 'lcd_lib.c'
// Title		: 4 bit LCd interface
// Author		: Scienceprog.com - Copyright (C) 2007
// Created		: 2007-06-18
// Revised		: 2007-06-18
// Version		: 1.0
// Target MCU	: Atmel AVR series
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
#include "lcd.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

void LCDsendChar(uint8_t ch)		//Sends Char to LCD
{
	LDP=(ch&0b11110000);
	LCP|=1<<LCD_RS;
	LCP|=1<<LCD_E;		
	_delay_ms(1);
	LCP&=~(1<<LCD_E);	
	LCP&=~(1<<LCD_RS);
	_delay_ms(1);
	LDP=((ch&0b00001111)<<4);
	LCP|=1<<LCD_RS;
	LCP|=1<<LCD_E;		
	_delay_ms(1);
	LCP&=~(1<<LCD_E);	
	LCP&=~(1<<LCD_RS);
	_delay_ms(1);
}
void LCDsendCommand(uint8_t cmd)	//Sends Command to LCD
{
	LDP=(cmd&0b11110000);
	LCP|=1<<LCD_E;		
	_delay_ms(1);
	LCP&=~(1<<LCD_E);
	_delay_ms(1);
	LDP=((cmd&0b00001111)<<4);	
	LCP|=1<<LCD_E;		
	_delay_ms(1);
	LCP&=~(1<<LCD_E);
	_delay_ms(1);
}
void LCDinit(void)//Initializes LCD
{
	_delay_ms(15);
	LDP=0x00;
	LCP=0x00;
	LDDR|=1<<LCD_D7|1<<LCD_D6|1<<LCD_D5|1<<LCD_D4;
	LCDR|=1<<LCD_E|1<<LCD_RS;
   //---------one------
	LDP=0<<LCD_D7|0<<LCD_D6|1<<LCD_D5|1<<LCD_D4; //4 bit mode
	LCP|=1<<LCD_E|0<<LCD_RS;		
	_delay_ms(1);
	LCP&=~(1<<LCD_E);
	_delay_ms(1);
	//-----------two-----------
	LDP=0<<LCD_D7|0<<LCD_D6|1<<LCD_D5|1<<LCD_D4; //4 bit mode
	LCP|=1<<LCD_E|0<<LCD_RS;		
	_delay_ms(1);
	LCP&=~(1<<LCD_E);
	_delay_ms(1);
	//-------three-------------
	LDP=0<<LCD_D7|0<<LCD_D6|1<<LCD_D5|0<<LCD_D4; //4 bit mode
	LCP|=1<<LCD_E|0<<LCD_RS;		
	_delay_ms(1);
	LCP&=~(1<<LCD_E);
	_delay_ms(1);
	//--------4 bit--dual line---------------
	LCDsendCommand(0b00101000);
   //-----increment address, cursor shift------
	LCDsendCommand(0b00001110);


}			
void LCDclr(void)				//Clears LCD
{
	LCDsendCommand(1<<LCD_CLR);
}
void LCDhome(void)			//LCD cursor home
{
	LCDsendCommand(1<<LCD_HOME);
}
void LCDstring(uint8_t* data, uint8_t nBytes)	//Outputs string to LCD
{
register uint8_t i;

	// check to make sure we have a good pointer
	if (!data) return;

	// print data
	for(i=0; i<nBytes; i++)
	{
		LCDsendChar(data[i]);
	}
}
void LCDGotoXY(uint8_t x, uint8_t y)	//Cursor to X Y position
{
	register uint8_t DDRAMAddr;
	// remap lines into proper order
	switch(y)
	{
	case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
	case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;
	case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
	case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
	default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
	}
	// set data address
	LCDsendCommand(1<<LCD_DDRAM | DDRAMAddr);
	
}	
void CopyStringtoLCD(const uint8_t *FlashLoc, uint8_t x, uint8_t y)
{
	uint8_t i;
	LCDGotoXY(x,y);
	for(i=0;(uint8_t)pgm_read_byte(&FlashLoc[i]);i++)
	{
		LCDsendChar((uint8_t)pgm_read_byte(&FlashLoc[i]));
	}
}
void LCDdefinechar(const uint8_t *pc,uint8_t char_code){
	uint8_t a, pcc;
	uint16_t i;
	a=(char_code<<3)|0x40;
	for (i=0; i<8; i++){
		pcc=pgm_read_byte(&pc[i]);
		LCDsendCommand(a++);
		LCDsendChar(pcc);
		}
}

void LCDcursorOn(void) //displays LCD cursor
{
	LCDsendCommand(0x0E);
}
void LCDcursorOnBlink(void)	//displays LCD blinking cursor
{
	LCDsendCommand(0x0F);
}
void LCDcursorOFF(void)	//turns OFF cursor
{
	LCDsendCommand(0x0C);
}
void LCDblank(void)		//blanks LCD
{
	LCDsendCommand(0x08);
}
void LCDvisible(void)		//Shows LCD
{
	LCDsendCommand(0x0C);
}
