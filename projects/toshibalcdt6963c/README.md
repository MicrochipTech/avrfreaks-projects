# TOSHIBA_LCD_T6963C

Uploaded by marekw on 2001-11-22 13:34:00 (rating 0 out of 5)

## Summary

Version V1.0  

This small library gives you the base control over a port controlled  

Toshiba LCD display (T6963C)  

It is ment as a starter for using "cheap" grafics displays.  

I got mine from a surplus dealer in germany.


Port connections are defined in header file.


Furthermore the following basic routines are provided:  

void lcd\_clrscr(void); void lcd\_gotoxy(u\_char x,u\_char y);  

void lcd\_printchar(u\_char ch); void lcd\_printstring(u\_char *str);  

void lcd\_printstringxy(u\_char x,u\_char y, u\_char *str);


Have fun.  

Marc Wetzel

## Compilers

- AVR32 GNU Toolchain

## Tags

- Part-specific Lib.functions
- AVR32 GNU Toolchain
