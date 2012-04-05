/*
 * LCD.cpp
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include "LCD.h"

avrutil::LCD::LCD()
{
	avr_lcd_init();
}

void avrutil::LCD::clearScreen()
{
	avr_lcd_clear();
}

void avrutil::LCD::home()
{
	avr_lcd_home();
}

void avrutil::LCD::setCursorPosition(uint8_t pos)
{
	avr_lcd_set_cursor_pos(pos);
}

void gotoXY(uint8_t yLine, uint8_t xColumn)
{
	switch (yLine)
	{
		case 0:
			avr_lcd_set_cursor_pos(AVR_LCD_LINE0 + xColumn);
			break;
		case 1:
			avr_lcd_set_cursor_pos(AVR_LCD_LINE1 + xColumn);
			break;
		default:
			/* Invalid X position */
			break;
	}
}

uint8_t avrutil::LCD::getCursorPosition()
{
	return avr_lcd_get_cursor_pos();
}

void avrutil::LCD::putc(char c)
{
	avr_lcd_putc(c);
}

void avrutil::LCD::puts(const char *s)
{
	avr_lcd_puts(s);
}

void avrutil::LCD::puts(const char *s, size_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		avr_lcd_putc(s[i]);
	}
}

