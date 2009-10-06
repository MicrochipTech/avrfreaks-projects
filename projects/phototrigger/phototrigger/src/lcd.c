/*
 * lcd.c
 *
 *  Created on: 2009-08-29
 *
 *  Heavily modified original file.
 */

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
 * $Id: lcd.c,v 1.1 2005/12/28 21:38:59 joerg_wunsch Exp $
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include <util/delay.h>
#include <util/atomic.h>

#include "hw.h"
#include "hd44780.h"
#include "lcd.h"
#include "dlg.h"
#include "eeprom.h"


// globals
FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
static volatile uint16_t newblval; // new backlight value

static char custchar[8][8] PROGMEM =
{
		{0x06, 0x0F, 0x09, 0x09, 0x09, 0x09, 0x0F, 0x00}, // battery
		{0x00, 0x04, 0x1E, 0x1F, 0x1E, 0x04, 0x00, 0x00}, // horizontal arrow
		{0x04, 0x0E, 0x1F, 0x0E, 0x0E, 0x0E, 0x0E, 0x00}, // vertical arrow
		{0x04, 0x0E, 0x1B, 0x0A, 0x0A, 0x0A, 0x0E, 0x00}, // vertical arrow empty
		{0x0A, 0x15, 0x00, 0x0F, 0x1E, 0x01, 0x1E, 0x00}, // mili second sign
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

// special routine for fluent backlight dimming
ISR(TIMER1_OVF_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(OCR1A == newblval)
		{
			hwT1OvfIntOff(); // turn ourself off (no interrupts)
		}
		else
		{
			if(OCR1A < newblval)
				OCR1A = dlgIncVar(OCR1A, LCDBLSTEP, 0, 0x3ff, false);
			else
				OCR1A = dlgIncVar(OCR1A, -LCDBLSTEP, 0, 0x3ff, false);
		}
	}
}

/*
 * Setup the LCD controller.  First, call the hardware initialisation
 * function, then adjust the display attributes we want.
 */
void
lcd_init(void)
{

  hd44780_init();

  /*
   * Clear the display.
   */
  hd44780_outcmd(HD44780_CLR);
  hd44780_wait_ready();

  /*
   * Entry mode: auto-increment address counter, no display shift in
   * effect.
   */
  hd44780_outcmd(HD44780_ENTMODE(1, 0));
  hd44780_wait_ready();

  /*
   * Enable display, activate non-blinking cursor.
   */
  hd44780_outcmd(HD44780_DISPCTL(1, 0, 0));
  hd44780_wait_ready();

  lcd_setcontrast(eeprom_read_byte(&eecontr)); // we need this to see something
  stdout = stderr = &lcd_str; // setup streams

  /*
   * Set custom characters
   */
  for(uint8_t i=0; i<8; i++)
	  lcd_setfont(i, custchar[i]);
}

/*
 * Send character c to the LCD display.  After a '\n' has been seen,
 * the next character will first clear the display.
 */
int
lcd_putchar(char c, FILE *unused)
{
  if (c == '\n' || c == '\r')
  {
	  uint8_t addr;

	  hd44780_wait_ready();
	  addr = hd44780_incmd() & HD44780_ADDRCOUNT;

	  if (addr <= 0x27)
		  addr = (c == '\n' ? 0x40 : 0x00); // go to second line
	  else
		  addr = (c == '\n' ? 0x00 : 0x40); // go to first line

	  hd44780_wait_ready();
	  hd44780_outcmd(HD44780_DDADDR(addr));
  }
  else
  {
      hd44780_wait_ready();
      hd44780_outdata(c);
  }

  return 0;
}

/*
 * Changes backlight of LCD
 */
void lcd_setbacklight(uint16_t newbl)
{
	static uint16_t lastblval;

	if(lastblval != newbl)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			newblval = newbl;
		}
		hwT1OvfIntOn(); // turn on dimming in interrupt
	}

	lastblval = newbl;
}

/*
 * Clears screen
 */
void lcd_clr(void)
{
	hd44780_wait_ready();
	hd44780_outcmd(HD44780_CLR);
}

/*
 * Sets x, y position
 */
void lcd_setxy(uint8_t x, uint8_t y)
{
	hd44780_wait_ready();
	hd44780_outcmd(HD44780_DDADDR(x + y * 0x40));
}

/*
 * Send a character to CGRAM
 */
void lcd_setfont(uint8_t idx, PGM_P cdata)
{
	hd44780_wait_ready();
	hd44780_outcmd(HD44780_CGADDR(idx*8));

	for(uint8_t i=0; i<8; i++)
	{
		hd44780_wait_ready();
		hd44780_outdata(pgm_read_byte(cdata+i));
	}
}
