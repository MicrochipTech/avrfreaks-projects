/* 
 * lcd.h
 * libavrutil
 *
 * Created by Árpád Goretity on 30/03/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

/* 
 * These function permit the easy cooperation with the de facto standard
 * Hitachi HD44780 LCD display controller. The library uses a 4-bit mode,
 * that is, the LCD needs to be connected to the AVR using 6 lines
 * (compared to 10 wires when using 8-bit mode):
 * port B, pin 0: LCD data bit 4
 * port B, pin 1: LCD data bit 5
 * port B, pin 2: LCD data bit 6
 * port B, pin 3: LCD data bit 7
 * port B, pin 4: Register Select (RS)
 * port B, pin 5: Clock/Enable (E)
 * The Read/~Write pin of the LCD should be grounded.
 */

#ifndef __AVRUTIL_LCD_H__
#define __AVRUTIL_LCD_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "base.h"

#define AVR_LCD_DISPLAY _BV(2)
#define AVR_LCD_CURSOR _BV(1)
#define AVR_LCD_BLINK _BV(0)

#define AVR_LCD_LINE0 0x00
#define AVR_LCD_LINE1 0x40

/* 
 * Initialize the LCD.
 * Should be called before any other avr_lcd_*() function.
 */
void avr_lcd_init();

/* 
 * Configures the LCD parameters as follows (should be OR-ed):
 * AVR_LCD_DISPLAY: switch on the display (at all)
 * AVR_LCD_CURSOR: set the underscore-style cursor
 * AVR_LCD_BLINK: make the cursor blink
 */
void avr_lcd_config(uint8_t flags);

/* 
 * Clear the screen and make the cursor go home
 */
void avr_lcd_clear();

/* 
 * Cursor to home (row 0, column 0), without clearing the display contents
 */
void avr_lcd_home();

/* 
 * Position the cursor explicitly using the constants
 * AVR_LCD_LINE0, AVR_LCD_LINE1
 * Add the column index (indexed from 0) to shift it to the right
 */
void avr_lcd_set_cursor_pos(uint8_t addr);

/* 
 * Returns the current position (line + char, indexed from 0)
 * of the LCD cursor.
 */
uint8_t avr_lcd_get_cursor_pos();

/* 
 * Display a character at the current cursor position
 * and advance the cursor by one.
 */
void avr_lcd_putc(char c);

/* 
 * Display a null-terminated C string at the current cursor position
 */
void avr_lcd_puts(const char *s);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AVRUTIL_LCD_H__ */

