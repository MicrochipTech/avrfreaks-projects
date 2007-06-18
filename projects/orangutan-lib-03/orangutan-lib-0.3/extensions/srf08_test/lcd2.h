//
// Orangutan / Baby Orangutan
//
// Utility Library
//
// Version 0.23 - 27 Dec 2006
//
// Revision History:
//
// Commands:
//
////// LCD Routines (Orangutan-Only) //////
//
// lcd_init()			Initialize the LCD routines
// lcd_string(str)		Send a string to the LCD
// lcd_int(int)			Send an integer to the LCD
// lcd_moveto(line, pos)	Move cursor to line, position
// lcd_moverel(dir, num)	Shift the cursor left or right by N characters
// lcd_shift(dir, num)		Scroll the display left or right by N characters
// lcd_line1()			Move cursor to line 1
// lcd_line2()			Move cursor to line 2
// lcd_clear()			Clear the LCD
// lcd_show()			Show the LCD cursor
// lcd_hide()			Hide the LCD cursor
//
// Credit where credit is due:
//
// LCD routines came from a whole range of sources including the device
// datasheet, my own original attempt at coding, Cathy Saxton's code,
// some code posted to the Pololu online forums, and the ultimate show-
// saver: some code Jan Malasec was kind enough to send me (that worked!)
//
// Tom Benedict
//

/////////////////////////////////////////////////////////////////////
//
// ORANGUTAN LCD ROUTINES
//
/////////////////////////////////////////////////////////////////////

#ifndef __LCD_
#define __LCD_

// Commands used by the LCD routines

#define LCD_CLEAR               0x01
#define LCD_LINE1               0x02
#define LCD_LINE2               0xC0
#define LCD_SHOW_BLINK  0x0F
#define LCD_SHOW_SOLID  0x0E
#define LCD_HIDE                0x0C
#define LCD_CURSOR_L    0x10
#define LCD_CURSOR_R    0x14
#define LCD_SHIFT_L             0x18
#define LCD_SHIFT_R             0x1C

// lcd_cmd
//
// Sends a command to the LCD.

void lcd_cmd(unsigned char cmd);

// lcd_data
//
// Sends data to the LCD.

void lcd_data(unsigned char data);

// lcd_string
//
// Sends a string to the LCD

void lcd_string(const unsigned char *str);

// lcd_string_P
//
// Sends a string from program FLASH to the LCD

void lcd_string_P(const unsigned char *str);

// lcd_int 
//
// Sends an integer to the LCD

void lcd_int(unsigned char n);

// lcd_init
//
// Initializes the LCD and MUST be called prior to
// every other LCD command.

void lcd_init(void);

// LCD Commands

// lcd_moveto 
//
// Moves the cursor to the given position
//
// Character	0  1  2  3  4  5  6  7  
//           	------------------------
// Line 1		80 81 82 83 84 85 86 87
// Line 2		C0 C1 C2 C3 C4 C5 C6 C7

void lcd_moveto(unsigned char line, unsigned char pos);

// lcd_moverel
//
// Shifts the cursor left or right the given number of
// positions.
//
// dir = 0	Shift left
//       1	Shift right
//

void lcd_moverel(unsigned char dir, unsigned char num);

// lcd_shift
//
// Shifts the display left or right the given number of
// positions.
//
// dir = 0	Shift left
//       1	Shift right
//

void lcd_shift(unsigned char dir, unsigned char num);

// lcd_clear clears the LCD display

#define lcd_clear()	lcd_cmd(LCD_CLEAR); _delay_ms(2)

// lcd_line1 moves the cursor to line 1

#define lcd_line1() lcd_cmd(LCD_LINE1); _delay_ms(2)

// lcd_line2 moves the cursor to line 2

#define lcd_line2() lcd_cmd(LCD_LINE2); _delay_us(100)

// lcd_show shows the LCD cursor

#define lcd_show() lcd_cmd(LCD_SHOW_BLINK); _delay_us(50)

// lcd_hide hides the LCD cursor

#define lcd_hide() lcd_cmd(LCD_HIDE); _delay_us(50)

#endif
