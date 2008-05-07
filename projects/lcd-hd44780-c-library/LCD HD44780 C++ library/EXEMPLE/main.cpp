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
 *                               MAIN FILE                                 *
 *                                                                         *
 ***************************************************************************/
//Headers
#include "config/global.h"		//GLOBAL DEFINITIONS
#include <avr/io.h>			//I/O
#include <inttypes.h>			//
#include <util/delay.h>			//SIMPLE DELAYS
#include "lcd.h"			//LCD
#include <avr/pgmspace.h>		//PGM space read
//Main objects
LCD lcd;				//make LCD object
//Main function
int main(void) {
	
	lcd << "/c/1" << (void*)PSTR("Hello World!!!");

	//lcd is streaming device...
	//you can put this(if you enabled this in conf/lcd.h): lcd << 22 << "TEXT IN RAM" << (void*)PSTR("TEXT IN PGM");
	// "/c" is modeficator - clear lcd
	// "/1" first line
	// "/2" second line
	// "/3" third line
	// "/4" third line
	// "/o" backlight on
	// "/f" backlight off

	// if would you put string in PGM you must throw PSTR to (void*) like this: (void*)PSTR("TEXT in PGM")
	//in text from pgm you can't put /c /1 /o /f .....

	//every questions send to d0zoenator@gmail.com

	for(;;) {
	
	}
	return 0;
}
