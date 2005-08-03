#ifndef __TINYMENU_HW_H__
#define __TINYMENU_HW_H__

/* Definitions for LCD interface for tinymenu; 
 * Change these to fit your LCD interface libs
 */

#include "lcd_pcd8544.h"
#include "lcd_pcd8544_text.h"

#define menu_set_inverse()    lcd_setmode(LCD_MODE_COPY_INVERSE)
#define menu_set_normal()     lcd_setmode(0)
#define menu_set_xy(x,y)      lcd_setpos((x) * 6, (y) * 6)
#define menu_putchar(x)       lcd_putchar(x)
#define menu_clear()          lcd_clear()

#endif // #ifndef __TINYMENU_HW_H__
