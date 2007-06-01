/********************************************************
**Project: 	Signalprocessing and Embedded Systems laboratory work
**
**Filename:	lcd.h
**
**Author:	Niklas Wiik, N85745
**
**Description: Functions for Seiko L1672 16x2 Lcd display.
********************************************************/

#ifndef LCD_H
#define LCD_H

void lcd_init();
void delay2();
void lcd_command(char);
void lcd_print(char);
void lcd_busy();
void lcd_printf(char *data);
void lcd_goto(int, int);
void lcd_update_weather(int);



#endif



