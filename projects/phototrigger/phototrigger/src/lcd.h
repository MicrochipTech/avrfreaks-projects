/*
 * lcd.h
 *
 *  Created on: 2009-08-29
  */

#ifndef LCD_H_
#define LCD_H_

#include <stdio.h>

#include <avr/pgmspace.h>

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Stdio demo, upper layer of LCD driver.
 *
 * $Id: lcd.h,v 1.1 2005/12/28 21:38:59 joerg_wunsch Exp $
 */

#define LCDBLSTEP           20    // steps in which to inc or dec backlight
#define LCDLOWBACKLIGHT     0     // PWM duty when inactive

/*
 * Custom characters (see also custchar variable)
 */
#define LCDCHARBATT         0     // battery
#define LCDCUSTCHAR0        0
#define LCDCHARARRHORIZ     1     // horizontal arrow
#define LCDCUSTCHAR1        1
#define LCDCHARARRVERT      2     // vertical arrow
#define LCDCUSTCHAR2        2
#define LCDCHARARRVERTEMPTY 3     // vertical arrow empty inside
#define LCDCUSTCHAR3        3
#define LCDCHARMS           4     // "ms" sign in one char
#define LCDCUSTCHAR4        4
#define LCDCUSTCHAR5        5
#define LCDCUSTCHAR6        6
#define LCDCUSTCHAR7        7

/*
 * Initialise LCD controller.  Performs a software reset.
 */
void lcd_init(void);

/*
 * Send one character to the LCD.
 */
int	 lcd_putchar(char c, FILE *stream);

/*
 * Set LCD contrast (PWM) /pseudofunction/
 */
#define lcd_setcontrast(x) OCR0A = (x);

/*
 * Set LCD backlight (PWM)
 */
void lcd_setbacklight(uint16_t newbl);

/*
 * Clear display
 */
void lcd_clr(void);

/*
 * Sets x, y position
 */
void lcd_setxy(uint8_t x, uint8_t y);

/*
 * Sends custom font data to CGRAM
 */
void lcd_setfont(uint8_t idx, PGM_P cdata);

#endif /* LCD_H_ */
