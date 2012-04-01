/* 
 * main.c
 * LCD test for libavrutil
 * 
 * Created by Árpád Goretity on 31/03/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <avrutil/avrutil.h>

int main()
{
	avr_lcd_init();
	avr_lcd_config(AVR_LCD_DISPLAY);
	
	avr_lcd_puts("Hello World!");
	avr_lcd_set_cursor_pos(AVR_LCD_LINE1);
	avr_lcd_puts("Cool, huh?");
	
	while (1)
	{
	}
}

