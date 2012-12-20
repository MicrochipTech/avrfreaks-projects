// Modified by Ozhan KD (www.knowledgeplus.ir)

#ifndef __XMEGA_LCD_V2__
#define __XMEGA_LCD_V2__

#include"avr_compiler.h"
#include "XMEGA_timer_v2.h"

//#define LCD_BUSY_CHECK 

#define LCD_SIZE  6

/*
0:  8x1 
1:  8x2 
2:  16x1
3:  16x2 
4:  16x4 
5:  20x2 
6:  20x4
7:  40x1 
8:  40x2 
*/

#define LCD_D4_DIR	PORTE_DIR
#define LCD_D4_DATA	PORTE_OUT
#define LCD_D4_BIT	4

#define LCD_D5_DIR	PORTE_DIR
#define LCD_D5_DATA	PORTE_OUT
#define LCD_D5_BIT	5

#define LCD_D6_DIR	PORTE_DIR
#define LCD_D6_DATA	PORTE_OUT
#define LCD_D6_BIT	6

#define LCD_D7_DIR	PORTE_DIR
#define LCD_D7_DATA	PORTE_OUT
#define LCD_D7_IN	PORTE_IN
#define LCD_D7_BIT	7

#define LCD_E_DIR	PORTF_DIR
#define LCD_E_PORT  	PORTF_OUT
#define LCD_E_BIT 	0

#define LCD_RW_DIR	PORTF_DIR
#define LCD_RW_PORT  	PORTF_OUT
#define LCD_RW_BIT 	1

#define LCD_RS_DIR	PORTF_DIR
#define LCD_RS_PORT  	PORTF_OUT
#define LCD_RS_BIT 	2

#define E_High()	LCD_E_PORT|=(1<<LCD_E_BIT)
#define E_Low()		LCD_E_PORT&=~(1<<LCD_E_BIT)

#define RW_High()	LCD_RW_PORT|=(1<<LCD_RW_BIT)
#define RW_Low()	LCD_RW_PORT&=~(1<<LCD_RW_BIT)

#define RS_High()	LCD_RS_PORT|=(1<<LCD_RS_BIT)
#define RS_Low()	LCD_RS_PORT&=~(1<<LCD_RS_BIT)

#define pgm_read_byte(x)    (*((uint8_t __flash *)(x)))

#define LCD_CLR             1	// clear display
#define LCD_HOME            2	// return to home position
#define LCD_DDRAM           7	//DB7: set DD RAM address

// cursor position to DDRAM mapping 

#define LCD_LINE0_DDRAMADDR		0x00

#if (LCD_SIZE==1) || (LCD_SIZE==3) || (LCD_SIZE==4) || (LCD_SIZE==5) || (LCD_SIZE==6)|| (LCD_SIZE==8)
 #define LCD_LINE1_DDRAMADDR		0x40
#endif

#if (LCD_SIZE==4)
 #define LCD_LINE2_DDRAMADDR		0x10
 #define LCD_LINE3_DDRAMADDR		0x50
#elif (LCD_SIZE==6)
 #define LCD_LINE2_DDRAMADDR		0x14
 #define LCD_LINE3_DDRAMADDR		0x54
#endif

void LCDDataLines(unsigned char);
void LCDDirOut(void);
void LCDDirIn(void);
void LCDSendData(unsigned char);
void LCDSendCommand(unsigned char);
void LCDClr(void);
void LCDHome(void);
void LCDCursorOn(void);
void LCDCursorOnBlink(void);
void LCDCursorOffBlink(void);
void LCDCursorOFF(void);
void LCDVisible(void);
void LCDCursorLeft(uint8_t n);
void LCDCursorRight(uint8_t n);
void LCDInit(void);
void LCDGotoXY(unsigned char, unsigned char);
void LCDBusyWait(void);
void LCDStringRam(char *);
void LCDStringFlash(char __flash *);
void LCDShiftRight(uint8_t);
void LCDShiftLeft(uint8_t);
void LCDdefinechar(uint8_t __flash *,uint8_t);

#endif

