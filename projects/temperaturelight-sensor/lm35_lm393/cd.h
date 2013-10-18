#include <avr/io.h>

#ifndef F_CPU
	#define F_CPU 12000000UL
#endif

#include <util/delay.h>

#ifndef _LCD_H
#define _LCD_H
/*_________________________________________________________________________________________*/

/************************************************
	LCD CONNECTIONS
*************************************************/

#define LCD_DATA C	//Port PC0-PC3 are connected to D4-D7

#define LCD_E C			//Enable OR strobe signal
#define LCD_E_POS	PC7	//Position of enable in above port

#define LCD_RS C
#define LCD_RS_POS 	PC6

#define LCD_RW C
#define LCD_RW_POS 	PC5


//************************************************

#define LS_BLINK 0B00000001
#define LS_ULINE 0B00000010

#define _CONCAT(a,b) a##b
#define PORT(x) _CONCAT(PORT,x)
#define PIN(x) _CONCAT(PIN,x)
#define DDR(x) _CONCAT(DDR,x)


/***************************************************
			F U N C T I O N S
****************************************************/

void LCDscroll(const char *scroling_text, uint8_t line_scroll);

void InitLCD(uint8_t style);
void LCDWriteString(const char *msg);
void LCDWriteInt(int val,unsigned int field_length);
void LCDGotoXY(uint8_t x,uint8_t y);

//this function returns the value with 2 decimals
void LCDWriteDecimals(int x, int y,float val , int field_length);



//Low level
void LCDByte(uint8_t,uint8_t);
#define LCDCmd(c) (LCDByte(c,0))
#define LCDData(d) (LCDByte(d,1))

void LCDBusyLoop();


/***************************************************
			F U N C T I O N S     E N D
****************************************************/


/***************************************************
	M A C R O S
***************************************************/
#define LCDClear() LCDCmd(0b00000001)
#define LCDHome() LCDCmd(0b00000010);

#define LCDWriteStringXY(x,y,msg) {\
 LCDGotoXY(x,y);\
 LCDWriteString(msg);\
}

#define LCDWriteIntXY(x,y,val,fl) {\
 LCDGotoXY(x,y);\
 LCDWriteInt(val,fl);\
}
/***************************************************/




/*_________________________________________________________________________________________*/
#endif






