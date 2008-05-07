/***************************************************************************
 *   Copyright (C) 2008 by Damian Kmiecik                                  *
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
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *                              LCD FILE                                   *
 *                                                                         *
 ***************************************************************************/
//Headers
#include "config/global.h"		//GLOBAL DEFINITIONS
#include <avr/io.h>			//I/O
#include <inttypes.h>			//
#include <util/delay.h>			//SIMPLE DELAYS
#include "lcd.h"			//LCD
#include "config/lcd.h"			//LCD CONFIG
#include <avr/pgmspace.h>		//PGM space read
//Delays configure
#define ENABLE_PULSE_DELAY	1
//Definitions
#define ENABLE_SET	LCD_E_PORT |= _BV(LCD_E_PIN);
#define ENABLE_CLR	LCD_E_PORT &= ~_BV(LCD_E_PIN);
#define RS_SET		LCD_RS_PORT |= _BV(LCD_RS_PIN)
#define RS_CLR		LCD_RS_PORT &= ~_BV(LCD_RS_PIN)
#define ENABLE_PULSE	ENABLE_SET; /*_delay_us(ENABLE_PULSE_DELAY);*/ ENABLE_CLR
#define DB0_SET		LCD_DATA0_PORT |= _BV(LCD_DATA0_PIN)
#define DB0_CLR		LCD_DATA0_PORT &= ~_BV(LCD_DATA0_PIN)
#define DB1_SET		LCD_DATA1_PORT |= _BV(LCD_DATA1_PIN)
#define DB1_CLR		LCD_DATA1_PORT &= ~_BV(LCD_DATA1_PIN)
#define DB2_SET		LCD_DATA2_PORT |= _BV(LCD_DATA2_PIN)
#define DB2_CLR		LCD_DATA2_PORT &= ~_BV(LCD_DATA2_PIN)
#define DB3_SET		LCD_DATA3_PORT |= _BV(LCD_DATA3_PIN)
#define DB3_CLR		LCD_DATA3_PORT &= ~_BV(LCD_DATA3_PIN)
#ifdef BACKLIGHT
  #define BACKLIGHT_ON	BACKLIGHT_PORT |= _BV(BACKLIGHT_PIN)
  #define BACKLIGHT_OFF	BACKLIGHT_PORT &= ~_BV(BACKLIGHT_PIN)
#endif
//HD44780 instructions, take a look to the datasheet for more informations
#if LCD_LINES==1
  #if LCD_DISP_LENGTH==16
	//Commands
	#define FUNCTION_SET			0x28
	#define ENTRY_MODE_SET			0x06
	#define DISPLAY_SET			0x0C
	#define CLEAR				0x01
	#define HOME				0x02
	#define CURSOR_ON			0x0C
	#define CURSOR_OFF			0x0E
	//Hardware defines
	#define LINE1_ADDR			0x00
	#define LINE2_ADDR			0x40
	#define CGRAM_ADDR			0x40
	#define DDRAM_ADDR			0x80
	static uint8_t position = 0;
  #else
	//Commands
	#define FUNCTION_SET			0x20
	#define ENTRY_MODE_SET			0x06
	#define DISPLAY_SET			0x0C
	#define CLEAR				0x01
	#define HOME				0x02
	#define CURSOR_ON			0x0C
	#define CURSOR_OFF			0x0E
	//Hardware defines
	#define LINE1_ADDR			0x00
	#define CGRAM_ADDR			0x40
	#define DDRAM_ADDR			0x80
  #endif
#endif
#if LCD_LINES==2
	//Commands
	#define FUNCTION_SET			0x28
	#define ENTRY_MODE_SET			0x06
	#define DISPLAY_SET			0x0C
	#define CLEAR				0x01
	#define HOME				0x02
	#define CURSOR_ON			0x0C
	#define CURSOR_OFF			0x0E
	//Hardware defines
	#define LINE1_ADDR			0x00
	#define LINE2_ADDR			0x40
	#define CGRAM_ADDR			0x40
	#define DDRAM_ADDR			0x80
#endif
#if LCD_LINES==4
	//Commands
	#define FUNCTION_SET			0x28
	#define ENTRY_MODE_SET			0x06
	#define DISPLAY_SET			0x0C
	#define CLEAR				0x01
	#define HOME				0x02
	#define CURSOR_ON			0x0C
	#define CURSOR_OFF			0x0E
	//Hardware defines
	#define LINE1_ADDR			0x00
	#define LINE2_ADDR			0x40
	#define LINE3_ADDR			0x14
	#define LINE4_ADDR			0x54
	#define CGRAM_ADDR			0x40
	#define DDRAM_ADDR			0x80
#endif
//Send data or command
void LCD::send(uint8_t data, bool mode) {
	//Set mode
	if(mode) {
		RS_SET;
	} else {
		RS_CLR;
	}
	//Send first half of data
	DB0_CLR;
	DB1_CLR;
	DB2_CLR;
	DB3_CLR;
	if(data & 0x80) DB3_SET;
	if(data & 0x40) DB2_SET;
	if(data & 0x20) DB1_SET;
	if(data & 0x10) DB0_SET;
	ENABLE_PULSE;
	//Send second half of data
	DB0_CLR;
	DB1_CLR;
	DB2_CLR;
	DB3_CLR;
	if(data & 0x08) DB3_SET;
	if(data & 0x04) DB2_SET;
	if(data & 0x02) DB1_SET;
	if(data & 0x01) DB0_SET;
	ENABLE_PULSE;
	//Wait some time
	if(!mode) {
		if(data == HOME or data == CLEAR) {
			_delay_ms(2);
		} else {
			_delay_us(40);
		}	
	} else {
		_delay_us(40);
	}
}
//Constructor LCD()
LCD::LCD() {
#ifdef LCD_STREAM
	this->base_l = 10;
#endif
	//Waiting for internal LCD reset
	_delay_ms(10);
	_delay_ms(10);
	//Configure I/O
	LCD_E_DDR |= _BV(LCD_E_PIN);
	LCD_RS_DDR |= _BV(LCD_RS_PIN);
	LCD_DATA0_DDR |= _BV(LCD_DATA0_PIN);
	LCD_DATA1_DDR |= _BV(LCD_DATA1_PIN);
	LCD_DATA2_DDR |= _BV(LCD_DATA2_PIN);
	LCD_DATA3_DDR |= _BV(LCD_DATA3_PIN);
#ifdef BACKLIGHT
	BACKLIGHT_DDR |= _BV(BACKLIGHT_PIN);
#endif
	//Wait some time
	_delay_ms(10);
	//Configure as 4bit interface
	LCD_DATA1_PORT |= _BV(LCD_DATA1_PIN);
	LCD_DATA0_PORT |= _BV(LCD_DATA0_PIN);
	ENABLE_PULSE;
	_delay_ms(5);
	ENABLE_PULSE;
	_delay_us(64);
	ENABLE_PULSE;
	_delay_us(64);
	LCD_DATA0_PORT &= ~_BV(LCD_DATA0_PIN);
	ENABLE_PULSE;
	_delay_us(64);
	//Configure LCD
	this->send(FUNCTION_SET, 0);
	//Default settings
	this->send(ENTRY_MODE_SET, 0);
	//Display ON, cursor OFF
	this->send(DISPLAY_SET, 0);
	//LCD clear
	this->cls();
	//Home position
	this->send(HOME, 0);
}
//Goto xy
void LCD::gotoxy(uint8_t x, uint8_t y) {
#if LCD_LINES==1
  #if LCD_DISP_LENGTH==16
	if(y < 8) {
		this->send(LINE1_ADDR + DDRAM_ADDR + y, 0);
	}
	if(y > 7) {
		this->send(LINE2_ADDR + DDRAM_ADDR + (y - 8), 0);
	}
	position = y;
  #else
	this->send(LINE1_ADDR + DDRAM_ADDR + y, 0);
  #endif
#endif
#if LCD_LINES==2
	if(x == 0) {
		this->send(LINE1_ADDR + DDRAM_ADDR + y, 0);
	}
	if(x == 1) {
		this->send(LINE2_ADDR + DDRAM_ADDR + y, 0);
	}
#endif
#if LCD_LINES==4
	if(x == 0) {
		this->send(LINE1_ADDR + DDRAM_ADDR + y, 0);
	}
	if(x == 1) {
		this->send(LINE2_ADDR + DDRAM_ADDR + y, 0);
	}
	if(x == 2) {
		this->send(LINE3_ADDR + DDRAM_ADDR + y, 0);
	}
	if(x == 3) {
		this->send(LINE4_ADDR + DDRAM_ADDR + y, 0);
	}
#endif
}
//Clear LCD
void LCD::cls(void) {
	this->send(CLEAR, 0);
}
//Put number on LCD
void LCD::put(int16_t number, uint8_t base) {
	char buf[16] = {0};
	register char i = 14;
	char m = 0;
	if(number < 0) {
		number = number * (-1);
		m = 1;
	}
	for(; number && i ; --i, number /= base)
		buf[i] = "0123456789abcdef"[number % base];
	if(m) {
		buf[i] = '-';
		this->put(&buf[i]);
	} else {
		this->put(&buf[i+1]);
	}
}
//Put text on LCD
void LCD::put(char *text) {
	while(*text){
		if(*text=='/') {
			switch(*(text+1)) {
			  case '1':	{this->cls();
					 this->send(LINE1_ADDR + DDRAM_ADDR, 0);
					 *text++;
					 #if LCD_LINES==1
					  #if LCD_DISP_LENGTH==16
					    position = 0;
					  #endif
					 #endif
					 break;
					}
#if LCD_LINES==2
			  case '2':	this->send(LINE2_ADDR + DDRAM_ADDR, 0); *text++; break;
#endif
#if LCD_LINES==3
			  case '3':	this->send(LINE3_ADDR + DDRAM_ADDR, 0); *text++; break;
#endif
#if LCD_LINES==4
			  case '4':	this->send(LINE4_ADDR + DDRAM_ADDR, 0); *text++; break;
#endif
#ifdef LCD_STREAM
			  case 'c':	{this->cls();
					 this->send(LINE1_ADDR + DDRAM_ADDR, 0);
					 *text++;
					 #if LCD_LINES==1
					  #if LCD_DISP_LENGTH==16
					    position = 0;
					  #endif
					 #endif
					 break;
					}
#endif
#ifdef LCD_STREAM
#ifdef BACKLIGHT
			  case 'o':	this->backlight(1); *text++; break;
			  case 'f':	this->backlight(0); *text++; break;
#endif
#endif
#ifdef LCD_STREAM
			  case 'h':	this->base_l = 16; *text++; break;
			  case 'b':	this->base_l = 2; *text++; break;
			  case 'd':	this->base_l = 10; *text++; break;
#endif
			  default:	{this->send(*text, 1);
					 #if LCD_LINES==1
					  #if LCD_DISP_LENGTH==16
					    position++;
					    if(position == 8) {
					      this->send(LINE2_ADDR + DDRAM_ADDR, 0);
					    }
					  #endif
					 #endif
					}
			}
		} else {
			this->send(*text, 1);
			#if LCD_LINES==1
			  #if LCD_DISP_LENGTH==16
			    position++;
			    if(position == 8) {
			      this->send(LINE2_ADDR + DDRAM_ADDR, 0);
			    }
			  #endif
			#endif
		}
		*text++;
	}
}
//Put text from PGM
void LCD::put_p(const char *text) {
	while(pgm_read_byte(text)){
		this->send(pgm_read_byte(text), 1);
		*text++ ;
		#if LCD_LINES==1
		  #if LCD_DISP_LENGTH==16
		    position++;
		    if(position == 8) {
		      this->send(LINE2_ADDR + DDRAM_ADDR, 0);
		    }
		  #endif
		#endif
	}
}
//Define user-defined chars
void LCD::define_char(uint8_t number, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, uint8_t p5, uint8_t p6, uint8_t p7, uint8_t p8) {
	uint8_t temp = CGRAM_ADDR;
	temp += (8 * number);
	//Set position in CGRAM
	this->send(temp, 0);
	//Send pixels image
	this->send(p1, 1);
	this->send(p2, 1);
	this->send(p3, 1);
	this->send(p4, 1);
	this->send(p5, 1);
	this->send(p6, 1);
	this->send(p7, 1);
	this->send(p8, 1);
	//End of CGRAM write
	this->cls();
}
#ifdef BACKLIGHT
//Turn on/Turn off backlight
void LCD::backlight(bool status) {
	if(status) {
		BACKLIGHT_ON;
	} else {
		BACKLIGHT_OFF;
	}
}
#endif
#ifdef LCD_STREAM
//
LCD &operator<<(LCD &lcd, uint16_t val) {
	lcd.put(val, lcd.base_l);
	return lcd;
}
//
LCD &operator<<(LCD &lcd, char *val) {
	lcd.put(val);
	return lcd;
}
//
#ifdef LCD_STREAM_PGM
//Compilator not recognize type const char*, i have to throwing this to void* before streaming
//plz use (void*)PSTR(text) if you want put text from PGM
LCD &operator<<(LCD &lcd, void* val) {
	lcd.put_p((const char*)val);
	return lcd;
}
#endif
#endif

