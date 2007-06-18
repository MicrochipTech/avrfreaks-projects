// lcd.h
//
// Tom Benedict

// Only include this code if we have an Orangutan device that has an
// LCD attached (set in "device.h").

#ifdef _ENABLE_LCD_

#ifndef _LCD_
#define _LCD_

// These commands are all straight out of the data sheet for the LCD
// installed on the Orangutan.  

// Commands

#define	LCD_CLEAR		0x01
#define	LCD_LINE1		0x02
#define LCD_LINE2		0xC0
#define LCD_SHOW_BLINK	0x0F
#define LCD_SHOW_SOLID	0x0E		
#define LCD_HIDE		0x0C
#define LCD_CURSOR_L	0x10
#define LCD_CURSOR_R	0x14
#define LCD_SHIFT_L		0x18
#define LCD_SHIFT_R		0x1C

// Inlines to determine the setting of the RS line for command
// and data writes to the LCD

#define lcd_cmd(cmd)	(lcd_send(cmd, 0))
#define lcd_data(data)	(lcd_send(data, 1))

void lcd_send(uint8_t data, uint8_t rs);

// lcd_string sends a string to the LCD.  You can send a string
// longer than 8 characters, but only eight characters show up.
// The string is printed from wherever the cursor is, and will
// not span lines.  (This lets you concatenate print statements.)

void lcd_string(const char *str);

// lcd_string_P copies a string from FLASH to RAM and calls
// lcd_string.

void lcd_string_P(const char *str);

// lcd_int prints an integer.  Again, this prints from wherever
// the cursor is, and will not span lines.  (This lets you 
// concatenate print statements.)

void lcd_int(uint16_t n);

// lcd_init initializes the LCD and MUST be called prior to
// every other LCD command.

void lcd_init(void);

// Go to an (X,Y) location on the LCD.  The top line is Y=0, the 
// leftmost character is X=0.

void lcd_gotoxy(uint8_t x, uint8_t y);

// The lcd_moveto() command is deprecated, and will be removed
// as of Orangutan-lib 0.4

#define lcd_moveto(line, x)	(lcd_gotoxy(x, (line-1)))

// lcd_moverel shifts the cursor left or right the given number of
// positions.
//
// dir = 0	Shift left
//       1	Shift right

void lcd_moverel(uint8_t dir, uint8_t num);

// lcd_shift shifts the display left or right the given number of
// positions.  This is what you'd use for a scrolling display.
//
// dir = 0	Shift left
//       1	Shift right

void lcd_shift(uint8_t dir, uint8_t num);

// The rest of these are one-liners.  Send the appropriate command
// to the LCD, and it takes care of the rest:

// Clear the LCD

#define lcd_clear()	lcd_cmd(LCD_CLEAR)

// This command is deprecated, and will be removed as of Orangutan-lib
// 0.4
// Move the cursor to the beginning of line 1

#define lcd_line1() (lcd_gotoxy(0,0))

// This command is deprecated, and will be removed as of Orangutan-lib
// 0.4
// Move the cursor to the beginning of line 2

#define lcd_line2() (lcd_gotoxy(0,1))

// Show the cursor as a blinking block.  (A non-blinking cursor is
// also available.)

#define lcd_show() lcd_cmd(LCD_SHOW_BLINK)

// Hide the cursor.

#define lcd_hide() lcd_cmd(LCD_HIDE)


// That's really it for the LCD stuff (and it's plenty!).  

#endif //_LCD_

#endif //_ENABLE_LCD_
