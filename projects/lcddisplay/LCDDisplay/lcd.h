#include "io.h"

void LCD_init (void);
void LCD_sendcommand (const char command);
void LCD_sendstring (char *str);
int  LCD_busy (void);
void LCD_clrscr (void);
void LCD_home (void);
void LCD_setpos (char x,char y);
void LCD_sendconststring (char *str);


// EOF