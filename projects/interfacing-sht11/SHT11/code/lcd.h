/*********************************************************************/
/*************** Genreal File Used for Function Declarations***********/
/*********************************************************************/
#include <avr/io.h>

// Define LCD PORT
#define LCDDATAPORT 				PORTC
#define LCDCONTROLPORT 				PORTA
#define LCD_RS						PINA7
#define LCD_EN 						PINA6
#define lcd_cls						0x01
   
extern void lcd_init(void );
extern void lcdCmdWrite(char cmddata);
extern void lcdDataWrite(char lcddata);
extern void delay(unsigned int x);
extern void lcd_puts(const char *s); 
extern void lcd_putc(char c);
extern  int printf(const char *format, ...);
extern void lcd_gotoxy(char x, char y);
extern void cursorOff(void );
extern void cursorOn(void );
extern void lcdcls(void);
extern void cursorBlinkOn(void ); 
extern void cursorBlinkOff(void );
extern void cursorBack(void);
extern void cursorForward(void );
