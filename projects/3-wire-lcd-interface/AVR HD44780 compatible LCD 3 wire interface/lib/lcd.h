// HD44780 & compatible LCD driver.
// © Portia CyberLogic.
// www.portia-cyberlogic.com
// mail@portia-cyberlogic.com
//
// LCD.h
// LCD main include.
// By Mahmoud Attaher.

#include <avr/io.h>

#ifndef F_CPU
	#define F_CPU 10000000UL
#endif

#include <util/delay.h>

#ifndef _LCD_H
#define _LCD_H

// ################################################################ //
// # Pin configuration
// ################################################################ //

// LCD serial interface pin configuration..
#ifndef LCD_DAT_PORT
	#define LCD_DAT_PORT PORTB // LCD Serial data pin port.
#endif
#ifndef LCD_DAT_DDR
	#define LCD_DAT_DDR DDRB // LCD Serial data pin port DDR.
#endif
#ifndef LCD_DAT_PIN
	#define LCD_DAT_PIN PB0 // LCD Serial data pin.
#endif

#ifndef LCD_CLK_PORT
	#define LCD_CLK_PORT PORTB // LCD Serial clock pin port.
#endif
#ifndef LCD_CLK_DDR
	#define LCD_CLK_DDR DDRB // LCD Serial clock pin port DDR.
#endif
#ifndef LCD_CLK_PIN
	#define LCD_CLK_PIN PB1 // LCD Serial clock pin.
#endif

#ifndef LCD_E_PORT
	#define LCD_E_PORT PORTB // LCD enable pin port.
#endif
#ifndef LCD_E_DDR
	#define LCD_E_DDR DDRB // LCD enable pin port DDR.
#endif
#ifndef LCD_E_PIN
	#define LCD_E_PIN PB2 // LCD enable pin.
#endif

// ################################################################ //
// # Definitions
// ################################################################ //

// Cursor styles..
#define LS_CUR_ON 0B00000100 // Cursor ON.
#define LS_CUR_BLINK 0B00000001 // Cursor blinking.
#define LS_CUR_ULINE 0B00000010 // Cursor underline, otherwise block.

// ################################################################ //
// # API
// ################################################################ //

// Functions..
void LCDInit(void);
void LCDPrintString(const char *_Str);
void LCDPrintInt(int _Int, unsigned int _nDgts);
void LCDGotoXY(uint8_t _x,uint8_t _y);

// Macros..
#define LCDClear() LCDCmd(0b00000001);
#define LCDClearLine(__y) LCDPrintStringXY(0, __y, "                ");
#define LCDHome() LCDCmd(0b00000010);
#define LCDCursor(__CStyle) LCDCmd(0b00001100|__CStyle);
#define LCDDisplayOFF() LCDCmd(0b00001000);
#define LCDDisplayON() LCDCmd(0b00001100);
#define LCDPrintStringXY(__x, __y, __Str) {LCDGotoXY(__x, __y);LCDPrintString(__Str);}
#define LCDPrintIntXY(__x, __y, __Int, __nDgts) {LCDGotoXY(__x, __y); LCDPrintInt(__Int, __nDgts);}

// ################################################################ //
// # Internal
// ################################################################ //

// Functions..
void LCDSend(uint8_t _Cmd, uint8_t _Byte);

// Macros..
#define LCD_DAT_SET() (LCD_DAT_PORT |= (1<<LCD_DAT_PIN))
#define LCD_CLK_SET() (LCD_CLK_PORT |= (1<<LCD_CLK_PIN))
#define LCD_E_SET() (LCD_E_PORT |= (1<<LCD_E_PIN))

#define LCD_DAT_CLR() (LCD_DAT_PORT &= (~(1<<LCD_DAT_PIN)))
#define LCD_CLK_CLR() (LCD_CLK_PORT &= (~(1<<LCD_CLK_PIN)))
#define LCD_E_CLR() (LCD_E_PORT &= (~(1<<LCD_E_PIN)))

#define LCDCmd(__c) (LCDSend(1, __c))
#define LCDData(__d) (LCDSend(0, __d))

// ################################################################ //
// # EOF
// ################################################################ //

#endif






