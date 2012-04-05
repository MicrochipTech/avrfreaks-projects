/*
 * LCD.h
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTILPP_LCD_H__
#define __AVRUTILPP_LCD_H__

#include <avrutil/avrutil.h>
#include "base.h"

namespace avrutil {

class LCD {
	public:
	LCD();

	void clearScreen();
	void home();
	void setCursorPosition(uint8_t pos);
	void gotoXY(uint8_t yLine, uint8_t xColumn);
	uint8_t getCursorPosition();

	void putc(char c);
	void puts(const char *s);
	void puts(const char *s, size_t count);
};

};

#endif /* __AVRUTILPP_LCD_H__ */

