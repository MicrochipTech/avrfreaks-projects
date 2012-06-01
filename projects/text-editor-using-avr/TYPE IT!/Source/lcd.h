/*********************************************

	LCD.H
	
	16x2 LCD   4-bit interfacing -
	
	Author: Visakhan C
	
**********************************************/
#ifndef LCD_H
#define LCD_H

#include<avr/io.h>
#include<util/delay.h>
#include<avr/pgmspace.h>




////////  CONFIGURE LCD PIN POSITIONS  /////////////

#define DATA_PORT 	C	//Port connected to MSB 4-bit data(D4-D7) of LCD module
#define DATA_POS	2	//Location of D4

#define E_PORT 		C	//Port and 
#define E_POS 		1	//position of E pin of LCD 

#define RS_PORT 	D	//Port and  
#define RS_POS 		7	//position of RS pin of LCD

#define RW_PORT 	C	//Port and position of	
#define RW_POS 		0	//R/W pin of LCD module (if R/W is not necessary connect it to GROUND)

////////////////////////////////////////////////



/////////  SOME PORTABILITY MACROS   /////////////

#define _COMBINE(a,b) a##b
#define PORT(x) _COMBINE(PORT,x)
#define DDR(x) _COMBINE(DDR,x)
#define PIN(x) _COMBINE(PIN,x)



#define LCD_CLEAR           0		//DB0: clear display

#define LCD_HOME            1		//DB1: return to home position

#define LCD_ENTRY	        (1<<2)	//DB2: set entry mode
#define ENTRY_INC	        1		//DB1: increment
#define ENTRY_SHIFT 	    0		//DB2: shift

#define LCD_ON	            (1<<3)	//DB3: turn lcd/cursor on
#define ON_DISPLAY 	        2		//DB2: turn display on
#define ON_CURSOR	        1		//DB1: turn cursor on
#define ON_BLINK            0		//DB0: blinking cursor

#define LCD_MOVE            (1<<4)	//DB4: move cursor/display
#define MOVE_DISP 	        3		//DB3: move display (0-> move cursor)
#define MOVE_RIGHT          2		//DB2: move right (0-> left)

#define LCD_FUNC        	(1<<5)	//DB5: function set
#define FUNC_8BIT 	    	4		//DB4: set 8BIT mode (0->4BIT mode)
#define FUNC_2LINES     	3		//DB3: two lines (0->one line)
#define FUNC_10DOTS     	2		//DB2: 5x10 font (0->5x7 font)

#define LCD_CGRAM           (1<<6)	//DB6: set CG RAM address
#define LCD_DDRAM           (1<<7)	//DB7: set DD RAM address

// reading:
#define LCD_BUSY            7	//DB7: LCD is busy

#define LCD_LINES			2	//visible lines
#define LCD_LINE_LENGTH		16	//line length (in characters)

// cursor position to DDRAM mapping
#define LCD_LINE0_ADDR		0x00
#define LCD_LINE1_ADDR		0x40
#define LCD_LINE2_ADDR		0x14
#define LCD_LINE3_ADDR		0x54


///////////////		MACROS		///////////////////////

#define setE()		( PORT(E_PORT) |= (1<<E_POS) )
#define clearE() 	( PORT(E_PORT) &= ~(1<<E_POS) )
#define setRW() 	( PORT(RW_PORT) |= (1<<RW_POS) )
#define clearRW() 	( PORT(RW_PORT) &= ~(1<<RW_POS) )
#define setRS() 	( PORT(RS_PORT) |= (1<<RS_POS) )
#define clearRS() 	( PORT(RS_PORT) &= ~(1<<RS_POS) )

#define lcdChar(c) 		( lcdByte(c,1) )
#define lcdCommand(d) 	( lcdByte(d,0) )


#define lcdEntryLeft() 			( lcdCommand(LCD_ENTRY|(1<<ENTRY_INC)) )
#define lcdEntryRight()			( lcdCommand(LCD_ENTRY|(0<<ENTRY_INC)) )
#define lcdEntryLeftShift() 	( lcdCommand(LCD_ENTRY|(1<<ENTRY_INC)|(1<<ENTRY_SHIFT)) )
#define lcdEntryRightShift() 	( lcdCommand(LCD_ENTRY|(0<<ENTRY_INC)|(1<<ENTRY_SHIFT)) )

#define lcdOff() 				( lcdCommand(LCD_ON |(0<<ON_DISPLAY)) )
#define lcdOn()  				( lcdCommand(LCD_ON |(1<<ON_DISPLAY)) )
#define lcdCursorOn()			( lcdCommand(LCD_ON |(1<<ON_DISPLAY)|(1<<ON_CURSOR)) )
#define lcdCursorOff() 			( lcdCommand(LCD_ON |(1<<ON_DISPLAY)|(0<<ON_CURSOR)) )
#define lcdCursorBlink() 		( lcdCommand(LCD_ON |(1<<ON_DISPLAY)|(1<<ON_CURSOR)|(1<<ON_BLINK)) )
#define lcdCursorNoBlink() 		( lcdCommand(LCD_ON |(1<<ON_DISPLAY)|(1<<ON_CURSOR)|(0<<ON_BLINK)) )

#define lcdCursorRightOnce() 	( lcdCommand(LCD_MOVE |(0<<MOVE_DISP)|(1<<MOVE_RIGHT)) )
#define lcdCursorLeftOnce()  	( lcdCommand(LCD_MOVE |(0<<MOVE_DISP)|(0<<MOVE_RIGHT)) )
#define lcdShiftRightOnce()  	( lcdCommand(LCD_MOVE |(1<<MOVE_DISP)|(1<<MOVE_RIGHT)) )
#define lcdShiftLeftOnce()   	( lcdCommand(LCD_MOVE |(1<<MOVE_DISP)|(0<<MOVE_RIGHT)) )



////////////////////////		FUNCTONS		/////////////////////////////


extern void lcdInit(void);
extern void lcdByte(uint8_t byte, uint8_t is_data);
extern void lcdString(uint8_t *str);
extern void lcdString_P(const uint8_t *FlashLoc);
extern void lcdDefineChar(const uint8_t *pc,uint8_t char_code);
extern void lcdNumber(uint16_t N,uint8_t dotPos);
extern void lcdNumberSmall(uint8_t N);
extern void lcdNumberHex(uint8_t num);


extern void lcdClear(void);
extern void lcdHome(void);
extern void lcdCursorLeft(unsigned char n);
extern void lcdCursorRight(unsigned char n);
extern void lcdShiftLeft(unsigned char n);
extern void lcdShiftRight(unsigned char n);
extern void lcdGotoXY(unsigned char x,unsigned char y);
uint8_t lcdGetX(void);
uint8_t lcdGetY(void);




#endif