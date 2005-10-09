
/*
Line diagram :
mega32            LCD Module
PD0        ->     D4
PD1        ->     D5
PD2        ->     D6
PD3        ->     D7
PD4        ->     RS
PD5        ->     R/W
PD6        ->     EN

DDRAM Address :
0x80 to 0x8F  =  1st Line
0xC0 to 0xCF  =  2nd Line

*/
#include <stdio.h>
#include <string.h>
#include <avr\io.h>
#include <avr\delay.h>

#define Plcd PORTD
#define RS 0x10
#define RW 0x20
#define EN 0x40
/* Function Prototype*/
void Toggle_Enable(void);
void Set_RW(void);
void Clear_RW(void);
void Set_RS(void);
void Clear_RS(void);
void LCD_init (void);
void LCD_Send(char c, unsigned char _DC); //_DC : 1 for Data, else for Command
void LCD_Send_String(unsigned char *s);
