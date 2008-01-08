/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef LCDDRIVER_H
#define LCDDRIVER_H

	// INCLUDES:
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	#include <avr/interrupt.h>
	
	#include "GlobalMacros.h"
	
	// EXTERNAL VARIABLES:
	extern volatile uint8_t ScrollFlags;
	
	// DEFINES:
	#define LCD_LCDREGS_START          ((uint8_t*)&LCDDR0)
	#define LCD_SPACE_OR_INVALID_CHAR  0xFF
	
	#define LCD_CONTRAST_LEVEL(level)  MACROS{ LCDCCR = (0x0F & level); }MACROE
	#define LCD_WAIT_FOR_SCROLL_DONE() MACROS{ while (!(ScrollFlags & LCD_FLAG_SCROLL_DONE)) {} }MACROE	
	
	#define LCD_SCROLLCOUNT_DEFAULT    6
	#define LCD_DELAYCOUNT_DEFAULT     20

	#define LCD_TEXTBUFFER_SIZE        20
	#define LCD_DISPLAY_SIZE           6

	#define LCD_FLAG_SCROLL            (1 << 0)
	#define LCD_FLAG_SCROLL_DONE       (1 << 1)	

	#define LCD_BARGRAPH_SIZE          5

	// PROTOTYPES:
	void LCD_Init(void);
	void LCD_Bargraph(const uint8_t Segments);
	void LCD_PutStr_f(const char *FlashData) ATTR_NON_NULL_PTR_ARGS(1);
	void LCD_PutStr(const char *Data) ATTR_NON_NULL_PTR_ARGS(1);
	void LCD_Bargraph(const uint8_t Segments);
	
	#if defined(INC_FROM_DRIVER)
	  static inline void LCD_WriteChar(const uint8_t Byte, const uint8_t Digit);
	#endif

#endif
