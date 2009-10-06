/*
 * tinymenu_hw.h -- LCD interface definitions for tinymenu functions
 *
 *  (c) 2004 Tymm Twillman (tymm@booyaka.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __TINYMENU_HW_H__
#define __TINYMENU_HW_H__

/*
 * What you need to do:
 *  - change #includes to use your LCD/display include files
 *  - change the macros to match your LCD/display functions
 *
 * #definable options:
 *
 *   CONFIG_TINYMENU_USE_CLEAR -- clear display each time a menu is displayed
 *    (alternative fills up any menu area without entries with spaces)
 *
 *   CONFIG_TINYMENU_HAS_INVERSE -- if your display routines include inverse
 *    display functions, set this and set menu_set_inverse and menu_set_normal
 *    to call your LCD's inverse/normal functions.  Otherwise entries are
 *    all displayed in normal characters, with an asterisk before the
 *    currently "highlighted" entry.
 */


/*
 * Definitions for LCD interface for tinymenu;
 * Change these to fit your LCD interface libs
 */

#include "lcd.h"

//Set if your LCD has a routine to set inverse printing
//#define CONFIG_TINYMENU_HAS_INVERSE

#ifdef CONFIG_TINYMENU_HAS_INVERSE
# define menu_set_inverse()    lcd_set_mode(LCD_MODE_COPY_INVERSE)
# define menu_set_normal()     lcd_set_mode(0)
#endif

#ifdef CONFIG_TINYMENU_USE_CLEAR
# define menu_clear()          lcd_clr()
#endif

// Display routine to go to an (x,y) position

#define menu_set_pos(x,y)      lcd_setxy(x, y)

// Display routine to output a character

#define menu_putchar(x)       lcd_putchar(x, NULL)

#endif // #ifndef __TINYMENU_HW_H___
