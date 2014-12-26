/************************************************************/
/**********File Related to usage of LCD *********************/
/************************************************************/
// Resources
#include <compat/deprecated.h>
#include <stdarg.h>
#include "lcd.h"
#include <util/delay.h>
 
// LCD initialization
void lcd_init(void)
{

	_delay_ms(100);
	// set up the LCD ; first the command mode
	lcdCmdWrite(0x30);		
	_delay_us(200);
	lcdCmdWrite(0x30);	
	_delay_us(200);
	lcdCmdWrite(0x30);		
    _delay_us(200);
	lcdCmdWrite(0x38);		// Interface length- 8 bit, 2line 5*7 dots
    _delay_us(200);
	lcdCmdWrite(0x0f);
	_delay_us(200);
	lcdCmdWrite(0x01);
	_delay_us(200);
	lcdCmdWrite(0x06);		// Display Address increment
	_delay_us(200);
	lcdCmdWrite(0x10);		// Display Address increment
	_delay_us(100);
	lcdcls();
	cursorOff();
	cursorBlinkOff();
}

// clear LCD 
void lcdcls(void)
{
	lcdCmdWrite(0x01);
}
// write the LCD Command setting write  - Mode 
void lcdCmdWrite(char cmdData)
{ 
	outp(0xFF, DDRC);		// Port C o/p
	
	_delay_us(80);   
	
	cbi(LCDCONTROLPORT,LCD_RS); 			// Port A  - RS Control
	_delay_us(80);
	sbi(LCDCONTROLPORT, LCD_EN);			// set the enable
	_delay_us(90);
	outp(cmdData, LCDDATAPORT);		// send the LCD command data out on DATA port- C
	_delay_us(50);
	
	cbi(LCDCONTROLPORT, LCD_EN);			// disable the enable line
	_delay_us(50);
	sbi(LCDCONTROLPORT,LCD_RS);
}

// Write the Display Data  on the LCD
void lcdDataWrite(char lcddata)
{
	outp(0xFF, DDRC);		// Port C O/p
	_delay_us(30);
	sbi(LCDCONTROLPORT,LCD_RS); 			// Port A7  - RS Control
	_delay_us(80);
	sbi(LCDCONTROLPORT, LCD_EN);			// set the enable
	_delay_us(90);
	outp(lcddata, LCDDATAPORT);		// send the  LCD data out on DATA port- C
	_delay_us(50);
	cbi(LCDCONTROLPORT, LCD_EN);			// disable the enable line
	_delay_us(60);
	outp(0, LCDDATAPORT);
	cbi(LCDCONTROLPORT,LCD_RS);
		_delay_us(20);
}
// move the cursor to particular location
// y is the line number
// x is the location at the point 
void lcd_gotoxy(char x, char y)
{
	// char i;
	if ( y == 1)				// first line 
		lcdCmdWrite(0x80 |  x);	
	
	if ( y == 2)				// second line 
		lcdCmdWrite((1<<7) + 0x40+ x);
}

// Set  LCD cusrsor off
void cursorOff()
{
	lcdCmdWrite (0x0c);
}
// Set LCD cursor ON
void cursorOn()
{
	lcdCmdWrite (0x0e);
}

// Make cursor BLINK
void cursorBlinkOn()
{
	lcdCmdWrite (0x0d);
	_delay_us(20);
	lcdCmdWrite (0x0d);
}
//Switch of Cursor BLINK
void cursorBlinkOff()
{
	lcdCmdWrite (0x0c);
}

//Move curcor back by one place
void cursorBack()
{
	lcdCmdWrite(0x04);
}

// Move cursor forward by one place
void cursorForward()
{
	lcdCmdWrite(0x06);
}	
// Put a  string  on LCD
void lcd_puts(const char *s)
/* print string on lcd (no auto linefeed) */
{
    register char c;
    while ( (c = *s++) ) 
    {
        lcdDataWrite(c);
		_delay_us(10);
    }
}

// write a charachter on LCD
void lcd_putc(char c)
{
        lcdDataWrite(c); 
}

//printf  function, with selectyed arguments 
int printf(const char *format, ...)
{
  char format_flag;
  unsigned int u_val, div_val, base;
  static const char hex[] = "0123456789ABCDEF";
  //char *ptr;
  va_list ap;

  va_start (ap, format);
  for (;;) {
     while ((format_flag = *format++) != '%')
    {      
       if (!format_flag) {
         va_end (ap);
         return (0);
       }
       lcd_putc (format_flag);   // put as a character
     }
// format list selection 
     switch (format_flag = *format++) {
       case 'c': format_flag = va_arg(ap,int);  // character forund
       default:  lcd_putc(format_flag); 
       		 continue;
       case 'd': base = 10; div_val = 10000; goto CONVERT;	//   int  found
       case 'x': base = 16; div_val = 0x10;				// hexadecimal  value

       CONVERT:				//  format  conversion 
       u_val = va_arg(ap,int);
       if (format_flag == 'd') {
         if (((int)u_val) < 0) {
           u_val = - u_val;
           lcd_putc ('-');
         }
         while (div_val > 1 && div_val > u_val) div_val /= 10;
       }
       do {
         lcd_putc (hex[u_val / div_val]);		// puts as character on the LCD
         u_val %= div_val;
         div_val /= base;
       } while (div_val);
    }
  }
}

