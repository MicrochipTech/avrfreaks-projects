//-------------------------------------------------------------------------------------------------
// Wyúwietlacz alfanumeryczny ze sterownikiem HD44780
// Sterowanie w trybie 4-bitowym bez odczytu flagi zajÍtoúci
// z dowolnym przypisaniem sygna≥Ûw sterujπcych
// Plik : HD44780.h	
// Mikrokontroler : Atmel AVR XMEGA
// Kompilator : avr-gcc
// Autor : Rados≥aw KwiecieÒ
// èrÛd≥o : http://radzio.dxp.pl/hd44780/
// Data : 24.03.2007
//
// Dostosowanie do procesorÛw XMEGA
// Dominik Leon BieczyÒski
// Leon Instruments
// http://leon-instruments.blogspot.com
//-------------------------------------------------------------------------------------------------

#ifndef HD44780_H_
#define HD44780_H_


#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "Globals_Defs.h"

//-------------------------------------------------------------------------------------------------
//
// Konfiguracja sygna≥Ûw sterujπcych wyúwietlaczem.
// Moøna zmieniÊ stosownie do potrzeb.
//
//-------------------------------------------------------------------------------------------------

#define LCD_PORT		PORTK
#define LCD_RS_bm		PIN5_bm
#define LCD_E_bm		PIN4_bm
#define LCD_D4_bm		PIN3_bm
#define LCD_D5_bm		PIN2_bm
#define LCD_D6_bm		PIN1_bm
#define LCD_D7_bm		PIN0_bm

// definicje ulatwiajace pisanie tekstow
#define Lcd(tekst) 	LcdWriteProgmem(PSTR(tekst))
#define Lcd1		LcdGoto(0,0)					// skok do pierwszej linii
#define Lcd2		LcdGoto(0,1)					// skok do drugiej linii

//-------------------------------------------------------------------------------------------------
//
// Instrukcje kontrolera Hitachi HD44780
//
//-------------------------------------------------------------------------------------------------

#define HD44780_CLEAR					0x01

#define HD44780_HOME					0x02

#define HD44780_ENTRY_MODE				0x04
	#define HD44780_EM_SHIFT_CURSOR		0
	#define HD44780_EM_SHIFT_DISPLAY	1
	#define HD44780_EM_DECREMENT		0
	#define HD44780_EM_INCREMENT		2

#define HD44780_DISPLAY_ONOFF			0x08
	#define HD44780_DISPLAY_OFF			0
	#define HD44780_DISPLAY_ON			4
	#define HD44780_CURSOR_OFF			0
	#define HD44780_CURSOR_ON			2
	#define HD44780_CURSOR_NOBLINK		0
	#define HD44780_CURSOR_BLINK		1

#define HD44780_DISPLAY_CURSOR_SHIFT	0x10
	#define HD44780_SHIFT_CURSOR		0
	#define HD44780_SHIFT_DISPLAY		8
	#define HD44780_SHIFT_LEFT			0
	#define HD44780_SHIFT_RIGHT			4

#define HD44780_FUNCTION_SET			0x20
	#define HD44780_FONT5x7				0
	#define HD44780_FONT5x10			4
	#define HD44780_ONE_LINE			0
	#define HD44780_TWO_LINE			8
	#define HD44780_4_BIT				0
	#define HD44780_8_BIT				16

#define HD44780_CGRAM_SET				0x40

#define HD44780_DDRAM_SET				0x80

//-------------------------------------------------------------------------------------------------
//
// Deklaracje funkcji
//
//-------------------------------------------------------------------------------------------------

void LcdCommand(unsigned char commandToWrite);
void LcdData(unsigned char data);
void LcdWrite(char * text);
void LcdWriteProgmem(const char * data);
void LcdGoto(unsigned char x, unsigned char y);
void LcdClear(void);
void LcdInit(void);
void LcdDec(uint32_t liczba);




#endif /* HD44780_H_ */
