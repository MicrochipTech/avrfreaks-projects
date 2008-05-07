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
 *                            LCD CONFIG FILE                              *
 *                                                                         *
 ***************************************************************************/
#ifndef LCD_H_CONFIG			//
#define LCD_H_CONFIG			//

#define LCD_STREAM			//comment this for disable streams
#define LCD_STREAM_PGM			//comment this for disable streams program memmory

#define LCD_PORT        PORTA		//LCD PORT
#define LCD_DDR		DDRA		//LCD DDR register
#define LCD_DATA0_PORT	LCD_PORT	//
#define LCD_DATA1_PORT	LCD_PORT	//
#define LCD_DATA2_PORT	LCD_PORT	//
#define LCD_DATA3_PORT	LCD_PORT	//
#define LCD_DATA0_DDR	LCD_DDR		//
#define LCD_DATA1_DDR	LCD_DDR		//
#define LCD_DATA2_DDR	LCD_DDR		//
#define LCD_DATA3_DDR	LCD_DDR		//
#define LCD_DATA0_PIN	3		//pin for 4 bits interface, data bit 0
#define LCD_DATA1_PIN	2		//pin for 4 bits interface, data bit 1
#define LCD_DATA2_PIN	1		//pin for 4 bits interface, data bit 2
#define LCD_DATA3_PIN	0		//pin for 4 bits interface, data bit 3
#define LCD_RS_PORT	LCD_PORT	//
#define LCD_RS_DDR	LCD_DDR		//
#define LCD_RS_PIN	5		//pin for RS line
#define LCD_E_PORT	LCD_PORT	//
#define LCD_E_DDR	LCD_DDR		//
#define LCD_E_PIN	4		//pin for ENABLE line

//#define BACKLIGHT			//uncomment this if you connected backlight
#ifdef BACKLIGHT			//
 #define BACKLIGHT_PIN	6		//backlight pin
 #define BACKLIGHT_PORT	LCD_PORT	//
 #define BACKLIGHT_DDR	LCD_DDR		//
#endif					//

#define LCD_LINES	1		//size of the lcd module
#define LCD_DISP_LENGTH	16		//

#endif					//

