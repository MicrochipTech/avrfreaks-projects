// lcd.h ///

#ifndef LCD_H
#define LCD_H


#define LCDCOLS 16
#define LCDROWS 2

// a few handy lcd instructions for display mode setting and

// clear screen
#define LCD_CLS 0x01

// home screen  or curser
#define LCD_HOME 0x02

// data entry mode set
#define LCD_MODE_INC 0x06
#define LCD_MODE_DEC 0x04
#define LCD_MODE_INCS 0x07
#define LCD_MODE_DECS 0x05
#define LCD_TEXT 0x06

// curser visible setting
#define LCD_CURS_ON 0x0f
#define LCD_CURS_OFF 0x0e

// display / curser shift
#define LCD_SHIFT_CL 0x10
#define LCD_SHIFT_CR 0x14
#define LCD_SHIFT_SL 0x18
#define LCD_SHIFT_SR 0x1c

void lcd_write_string(char x,char y, char *text,char width);
void lcd_init (void);
void lcd_cls (void);

#endif


