/*
 * This file contains hardware constants for ScreenKeys configurable
 * LCD display/switches.  See <http://www.screenkeys.com>.
 *
 * This code is not maintained and is free to use with no rights reserved, and
 * no guarantees made.
 *
 * Uses - Timer0, 2 outputs, 1 input.
 *
 * Tom Blough 7/20/2008
 */

#ifndef SCREENKEYS_H
#define SCREENKEYS_H

#include "utility.h"


/******** USER CONFIGURABLE OPTIONS ************************/

// ScreenKey type
#define SK_LCD24 1  // 0 = LC16/RGB16, 1 = LC24/RGB24

// Physical connection between ScreenKey and uP
#define SK_CLK(reg) BIT(D,0,reg)  // Screenkey clock output pin
#define SK_DATA(reg) BIT(D,1,reg)  // Screenkey data output pin
#define SK_SWITCH(reg) BIT(D,7,reg)  // Screenkey switch input pin

// Screenkey clock data - probably don't need to change this.  A SK clock
// of 50KHz should work fine for CPU clocks from 1MHz to 20MHz
#define SK_CLK_FREQ 50000UL  // ScreenKey clock frequency in Hz.  Range 50KHz - 4.0MHz
#define SK_FREQ_REG 0x00  // from Frequency Value Table in datasheet; must match SK_CLK_FREQ value

// uncomment the following line to include support for dynamic text.
// this adds approximately 1K program and 480 data bytes to the code footprint.
#define SK_TEXT_SUPPORT

/******** END OF CONFIGURATION OPTIONS ************************/


// RGB Background colors (3 colors [rgb] each with 3 intensities [off, on, bright])
#define SK_BLACK 0b00000000
#define SK_GRAY 0b00000111
#define SK_WHITE 0b01110111

#define SK_DARK_BLUE 0b00000001
#define SK_BRIGHT_BLUE 0b00010001
#define SK_PALE_BLUE 0b00010111

#define SK_DARK_RED 0b00000010
#define SK_BRIGHT_RED 0b00100010
#define SK_PALE_RED 0b00100111

#define SK_DARK_MAGENTA 0b00000011
#define SK_BRIGHT_MAGENTA 0b00110011
#define SK_PALE_MAGENTA 0b00110111

#define SK_DARK_GREEN 0b00000100
#define SK_BRIGHT_GREEN 0b01000100
#define SK_PALE_GREEN 0b01000111

#define SK_DARK_CYAN 0b00000101
#define SK_BRIGHT_CYAN 0b01010101
#define SK_PALE_CYAN 0b01010111

#define SK_DARK_YELLOW 0b00000110
#define SK_BRIGHT_YELLOW 0b01100110
#define SK_PALE_YELLOW 0b01100111

#define SK_PURPLE 0b00010011
#define SK_COBALT 0b00010101
#define SK_LAVENDER 0b00100011
#define SK_ORANGE 0b00100110
#define SK_TURQUOISE 0b01000101
#define SK_CHARTREUSE 0b01000110

// Commands
#define SK_SET_PIXEL 0x80
#define SK_SET_COLOR 0xED
#define SK_SET_FREQ_REG 0xEE
#define SK_SET_MUX 0xEF
#define SK_START_BYTE 0x00
#define SK_END_BYTE 0xAA

// LCD size specific image bytes and mux contrast data
#if SK_LCD24
#define SK_IMAGE_BYTES 108  // maximum bytes in pixel image
#define SK_MUX_DATA 0x0700  // mux divisor values for LCD contrast
#define SK_IMAGE_WIDTH 36  // display width in pixels  
#define SK_MAX_TEXT 6  // maximum of 6x8 characters across LCD
#define SK_LINE_1 0  // byte offset in display pixel map for start of 8-pixel line
#define SK_LINE_2 36
#define SK_LINE_3 72
#else
#define SK_IMAGE_BYTES 64
#define SK_MUX_DATA 0x0205
#define SK_IMAGE_WIDTH 32
#define SK_MAX_TEXT 5
#define SK_LINE_1 0
#define SK_LINE_2 32
#endif

// text alignment
#define SK_TEXT_CENTER 0
#define SK_TEXT_RIGHT 1
#define SK_TEXT_LEFT 2

// Parity values
#define SK_START_PARITY 0  // 0 = even parity
#define SK_CMD_PARITY 1  // 1 = odd parity
#define SK_DATA_PARITY 1
#define SK_END_PARITY 0

// public functions
extern void sk_init( void);
extern void sk_set_pixels( uint8_t data[], uint8_t offset, uint8_t count);
extern void sk_set_color( uint8_t color);

#ifdef SK_TEXT_SUPPORT
static const uint8_t charMap[96][5];  // 5x8 bitmap font
extern void sk_set_text( char* text, uint8_t justify, uint8_t line);
extern void sk_clear_text( uint8_t line);
#endif


// private helper functions
// uint8_t freq_reg_value( uint32_t frequency); // not needed see function implementation
void send_cmd( uint8_t cmd, uint16_t data);
void write_byte( uint8_t data, uint8_t parity);

#endif // SCREENKEYS_H
