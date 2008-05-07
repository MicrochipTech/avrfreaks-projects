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
 *                            LCD HEADER FILE                              *
 *                                                                         *
 ***************************************************************************/
#ifndef LCD_H
#define LCD_H

//Headers
#include "config/global.h"		//GLOBAL DEFINITIONS
#include <avr/io.h>			//I/O
#include <inttypes.h>			//
#include <avr/pgmspace.h>		//PGM space read
#include "config/lcd.h"			//CONFIG
//LCD class
class LCD {
	public:
		LCD();							//INIT
		void put(char *text);					//PUT TEXT ON LCD
		#ifdef BACKLIGHT
		void backlight(bool status);				//TURN OFF/ON BACKLIGHT
		#endif
		void put_p(const char *text);				//PUT TEXT FROM PGM SPACE
		void put(int16_t number, uint8_t base);			//PUT NUMBER ON LCD
		void gotoxy(uint8_t x, uint8_t y);			//Set cursor position
		void cls(void);						//CLEAR LCD
		void define_char(uint8_t number, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, uint8_t p5, uint8_t p6, uint8_t p7, uint8_t p8);	//Define user-defined chars
		#ifdef LCD_STREAM
			uint8_t base_l;					//BASE OF DISPLAYED DIGITS
		#endif
	private:
		void send(uint8_t data, bool mode);			//SEND DATA OR COMMAND
};
#ifdef LCD_STREAM
//
LCD &operator<<(LCD &lcd, uint16_t val);
LCD &operator<<(LCD &lcd, char *val);
#ifdef LCD_STREAM_PGM
LCD &operator<<(LCD &lcd, void* val);
#endif
#endif

#endif

