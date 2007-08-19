//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  mt - used for debugging only - may not work

#include <avr/pgmspace.h>
//Functions
// mt void LCD_puts_f(char __flash *pFlashStr, char scrollmode);
void LCD_puts_f(const char *pFlashStr, char scrollmode);
void LCD_puts(char *pStr, char scrollmode);
void LCD_UpdateRequired(char update, char scrollmode);
//void LCD_putc(char digit, char character);
void LCD_putc(uint8_t digit, char character);
void LCD_Clear(void);
void LCD_Colon(char show);
void LCD_FlashReset(void);
void SetContrast(char input);
