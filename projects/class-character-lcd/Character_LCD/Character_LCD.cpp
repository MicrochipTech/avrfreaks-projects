//*****************************************************************************
//
// File Name	: 'Character_LCD.cpp'
// Title		: 4 bit LCd interface
// Author		: Hamid Davoodi Shandiz - Copyright (C) 2014
// Created		: 2007-03-29
// Revised		: 2012-06-16
// Version		: 4.5.2.6
// Target MCU	: Atmel AVR series
//
// This code is distributed under the GNU Public License
//		which can be found at www. RoboticNGO .com  -  Hamid.davoodi.sh@gmail.com
//
//*****************************************************************************

#ifndef CHARACTER_LCD_LIB_CPP
	#define CHARACTER_LCD_LIB_CPP

	#ifndef F_CPU
	/* prevent compiler error by supplying a default */
		# warning "F_CPU not defined for <util/delay.h>"
		# define F_CPU 1000000UL
	#endif
	
	#include <util/delay.h>
	#include <inttypes.h>
	#include <avr/pgmspace.h>

	const uint8_t LcdCustomChar[] PROGMEM=//define 8 custom LCD chars
	{
		0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, // 0. 0/5 full progress block
		0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, // 1. 1/5 full progress block
		0x00, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00, // 2. 2/5 full progress block
		0x00, 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x00, // 3. 3/5 full progress block
		0x00, 0x1F, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x00, // 4. 4/5 full progress block
		0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, // 5. 5/5 full progress block
		0x03, 0x07, 0x0F, 0x1F, 0x0F, 0x07, 0x03, 0x00, // 6. rewind arrow
		0x18, 0x1C, 0xFE, 0x7F, 0xFE, 0x1C, 0x18, 0x00  // 7. fast-forward arrow
	};
	

#define LCD_CLR             0	//DB0: clear display
#define LCD_HOME            1	//DB1: return to home position
#define LCD_ENTRY_MODE      2	//DB2: set entry mode
#define LCD_ENTRY_INC       1	//DB1: increment
#define LCD_ENTRY_SHIFT     0	//DB2: shift
#define LCD_ON_CTRL         3	//DB3: turn lcd/cursor on
#define LCD_ON_DISPLAY      2	//DB2: turn display on
#define LCD_ON_CURSOR       1	//DB1: turn cursor on
#define LCD_ON_BLINK        0	//DB0: blinking cursor
#define LCD_MOVE            4	//DB4: move cursor/display
#define LCD_MOVE_DISP       3	//DB3: move display (0-> move cursor)
#define LCD_MOVE_RIGHT      2	//DB2: move right (0-> left)
#define LCD_FUNCTION        5	//DB5: function set
#define LCD_FUNCTION_8BIT   4	//DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES 3	//DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS 2	//DB2: 5x10 font (0->5x7 font)
#define LCD_CGRAM           6	//DB6: set CG RAM address
#define LCD_DDRAM           7	//DB7: set DD RAM address
// reading:
#define LCD_BUSY            7	//DB7: LCD is busy
#define LCD_LINES			2	//visible lines
#define LCD_LINE_LENGTH		16	//line length (in characters)
// cursor position to DDRAM mapping
#define LCD_LINE0_DDRAMADDR		0x00
#define LCD_LINE1_DDRAMADDR		0x40
#define LCD_LINE2_DDRAMADDR		0x14
#define LCD_LINE3_DDRAMADDR		0x54
// progress bar defines
#define PROGRESSPIXELS_PER_CHAR	6

class Character_LCD
{
	uint8_t RS_PORT , RS_DDR , RS_Bit ;
	uint8_t RW_PORT , RW_DDR , RW_Bit ;
	uint8_t E_PORT  , E_DDR  , E_Bit  ;

	uint8_t D4_PORT , D4_DDR , D4_Bit ;
	uint8_t D5_PORT , D5_DDR , D5_Bit ;
	uint8_t D6_PORT , D6_DDR , D6_Bit ;
	uint8_t D7_PORT , D7_DDR , D7_Bit ;
	



	public:
		Character_LCD ();
		
		void SetRSPin(uint8_t ADD_PORT , uint8_t ADD_DDR , uint8_t Bit) { RS_PORT = ADD_PORT ; RS_DDR = ADD_DDR ; RS_Bit = Bit ; }
		void SetRWPin(uint8_t ADD_PORT , uint8_t ADD_DDR , uint8_t Bit) { RW_PORT = ADD_PORT ; RW_DDR = ADD_DDR ; RW_Bit = Bit ; }
		void SetEPin(uint8_t ADD_PORT , uint8_t ADD_DDR , uint8_t Bit) { E_PORT = ADD_PORT ; E_DDR = ADD_DDR ; E_Bit = Bit ; }
		void SetD4Pin(uint8_t ADD_PORT , uint8_t ADD_DDR , uint8_t Bit) { D4_PORT = ADD_PORT ; D4_DDR = ADD_DDR ; D4_Bit = Bit ; }
		void SetD5Pin(uint8_t ADD_PORT , uint8_t ADD_DDR , uint8_t Bit) { D5_PORT = ADD_PORT ; D5_DDR = ADD_DDR ; D5_Bit = Bit ; }
		void SetD6Pin(uint8_t ADD_PORT , uint8_t ADD_DDR , uint8_t Bit) { D6_PORT = ADD_PORT ; D6_DDR = ADD_DDR ; D6_Bit = Bit ; }
		void SetD7Pin(uint8_t ADD_PORT , uint8_t ADD_DDR , uint8_t Bit) { D7_PORT = ADD_PORT ; D7_DDR = ADD_DDR ; D7_Bit = Bit ; }
						
		void SendChar(uint8_t);								//forms data ready to send to 74HC164
		void SendCommand(uint8_t);							//forms data ready to send to 74HC164
		void Init(void);										//Initializes LCD
		void Clr(void);										//Clears LCD
		void Home(void);										//LCD cursor home
		void String(uint8_t*, uint8_t);						//Outputs string to LCD
		void GotoXY(uint8_t, uint8_t);						//Cursor to X Y position
		void CopyStringtoLCD(const uint8_t*, uint8_t, uint8_t);	//copies flash string to LCD at x,y
		void Definechar(const uint8_t *,uint8_t);			//write char to LCD CGRAM
		void ShiftRight(uint8_t);							//shift by n characters Right
		void ShiftLeft(uint8_t);								//shift by n characters Left
		void CursorOn(void);									//Underline cursor ON
		void CursorOnBlink(void);							//Underline blinking cursor ON
		void CursorOFF(void);								//Cursor OFF
		void Blank(void);									//LCD blank but not cleared
		void Visible(void);									//LCD visible
		void CursorLeft(uint8_t);							//Shift cursor left by n
		void CursorRight(uint8_t);							//shift cursor right by n
		void ProgressBar(uint8_t progress, uint8_t maxprogress, uint8_t length,uint8_t x, uint8_t y);//Create Progress Bar
		// توابعی که افزوده شده است برای کاربرد راحتر کاربران
		void CopyVariableDecToLCD(uint32_t Namber, uint8_t x, uint8_t y);
		void CopyVariableBinToLCD(uint32_t Namber, uint8_t x, uint8_t y);
		void CopyVariableHexToLCD(uint32_t Namber, uint8_t x, uint8_t y);
		void CopyVariableOctToLCD(uint32_t Namber, uint8_t x, uint8_t y);
		void CopyVariableDecToLCD_Length(uint32_t Namber, uint8_t length, uint8_t x, uint8_t y);
		void CopyVariableBinToLCD_Length(uint32_t Namber, uint8_t length, uint8_t x, uint8_t y);
		void CopyVariableOctToLCD_Length(uint32_t Namber, uint8_t length, uint8_t x, uint8_t y);
		void CopyVariableHexToLCD_Length(uint32_t Namber, uint8_t length, uint8_t x, uint8_t y);
		
	private:
	
};
Character_LCD :: Character_LCD ()
{
		RS_PORT = 0x18 ; RS_DDR = 0x17 ; RS_Bit = 1 ;
		RW_PORT = 0x18 ; RW_DDR = 0x17 ; RW_Bit = 2 ;
		E_PORT = 0x18 ; E_DDR = 0x17 ; E_Bit  = 3 ;

		D4_PORT = 0x18 ; D4_DDR = 0x17 ; D4_Bit = 4 ;
		D5_PORT = 0x18 ; D5_DDR = 0x17 ; D5_Bit = 5 ;
		D6_PORT = 0x18 ; D6_DDR = 0x17 ; D6_Bit = 6 ;
		D7_PORT = 0x18 ; D7_DDR = 0x17 ; D7_Bit = 7 ;
}
void Character_LCD :: Clr(void)				//Clears LCD
{
	SendCommand(1<<LCD_CLR);
}
void Character_LCD :: Home(void)			//LCD cursor home
{
	SendCommand(1<<LCD_HOME);
}
void Character_LCD :: String(uint8_t* data, uint8_t nBytes)	//Outputs string to LCD
{
	register uint8_t i;
	if (!data) return;
	for(i=0; i<nBytes; i++)
	{
		SendChar(data[i]);
	}
}
void Character_LCD :: GotoXY(uint8_t x, uint8_t y)	//Cursor to X Y position
{
	register uint8_t DDRAMAddr;
	switch(y)
	{
		case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
		case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;
		case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
		case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
		default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
	}
	SendCommand(1<<LCD_DDRAM | DDRAMAddr);
	
}
void Character_LCD :: CopyVariableDecToLCD(uint32_t Namber, uint8_t x, uint8_t y)
{
	uint8_t i=0;
	uint8_t b;
	int8_t k[15];
	GotoXY(x,y);
	do
	{
		b=Namber%10;
		k[i] = 48 +b;
		Namber /= 10;
		i++;
	}
	while (Namber != 0);
	while (i != 0 )
	{
		i--;
		SendChar(k[i]);
	}
}
void Character_LCD :: CopyVariableBinToLCD(uint32_t Namber, uint8_t x, uint8_t y)
{
	uint8_t i=0;
	uint8_t b;
	int8_t k[30];
	GotoXY(x,y);
	
	do
	{
		b=Namber%2;
		k[i] = 48 +b;
		Namber /= 2;
		i++;
	}
	while (Namber != 0);
	while (i != 0 )
	{
		i--;
		SendChar(k[i]);
	}
}
void Character_LCD :: CopyVariableOctToLCD(uint32_t Namber, uint8_t x, uint8_t y)
{
	uint8_t i=0;
	uint8_t b;
	int8_t k[30];
	GotoXY(x,y);
	do
	{
		b=Namber%8;
		k[i] = 48 + b;
		Namber /= 8;
		i++;
	}
	while (Namber != 0);
	while (i != 0 )
	{
		i--;
		SendChar(k[i]);
	}
}
void Character_LCD :: CopyVariableHexToLCD(uint32_t Namber, uint8_t x, uint8_t y)
{
	uint8_t i=0;
	uint8_t b;
	int8_t k[30];
	GotoXY(x,y);
	do
	{
		b=Namber%16;
		if (b<10)k[i] = 48 + b;
		else k[i] = 87 + b;
		Namber /= 16;
		i++;
	}
	while (Namber != 0);
	while (i != 0 )
	{
		i--;
		SendChar(k[i]);
	}
}
void Character_LCD :: CopyVariableDecToLCD_Length(uint32_t Namber, uint8_t length, uint8_t x, uint8_t y)
{
	uint8_t i=0;
	uint8_t b;
	int8_t k[16];
	while(i<16){k[i]=48;i++;}
	i=0;
	GotoXY(x,y);
	do
	{
		b=Namber%10;
		k[i] = 48 +b;
		Namber /= 10;
		i++;
	}
	while (Namber != 0);
	i=length;
	while (i != 0 )
	{
		i--;
		SendChar(k[i]);
	}
}
void Character_LCD :: CopyVariableBinToLCD_Length(uint32_t Namber, uint8_t length, uint8_t x, uint8_t y)
{
	uint8_t i=0;
	uint8_t b;
	int8_t k[30];
	while(i<16){k[i]=48;i++;}
	i=0;
	GotoXY(x,y);
	
	do
	{
		b=Namber%2;
		k[i] = 48 +b;
		Namber /= 2;
		i++;
	}
	while (Namber != 0);
	i=length;
	while (i != 0 )
	{
		i--;
		SendChar(k[i]);
	}
}
void Character_LCD :: CopyVariableOctToLCD_Length(uint32_t Namber, uint8_t length, uint8_t x, uint8_t y)
{
	uint8_t i=0;
	uint8_t b;
	int8_t k[30];
	while(i<16){k[i]=48;i++;}
	i=0;
	GotoXY(x,y);
	do
	{
		b=Namber%8;
		k[i] = 48 + b;
		Namber /= 8;
		i++;
	}
	while (Namber != 0);
	i=length;
	while (i != 0 )
	{
		i--;
		SendChar(k[i]);
	}
}
void Character_LCD :: CopyVariableHexToLCD_Length(uint32_t Namber, uint8_t length, uint8_t x, uint8_t y)
{
	uint8_t i=0;
	uint8_t b;
	int8_t k[30];
	while(i<16){k[i]=48;i++;}
	i=0;
	GotoXY(x,y);
	do
	{
		b=Namber%16;
		if (b<10)k[i] = 48 + b;
		else k[i] = 87 + b;
		Namber /= 16;
		i++;
	}
	while (Namber != 0);
	i=length;
	while (i != 0 )
	{
		i--;
		SendChar(k[i]);
	}
}
void Character_LCD :: ShiftLeft(uint8_t n)	//Scrol n of characters Right
{
	for (uint8_t i=0;i<n;i++)
	{
		SendCommand(0x1E);
	}
}
void Character_LCD :: ShiftRight(uint8_t n)	//Scrol n of characters Left
{
	for (uint8_t i=0;i<n;i++)
	{
		SendCommand(0x18);
	}
}
void Character_LCD :: CursorOn(void) //displays LCD cursor
{
	SendCommand(0x0E);
}
void Character_LCD :: CursorOnBlink(void)	//displays LCD blinking cursor
{
	SendCommand(0x0F);
}
void Character_LCD :: CursorOFF(void)	//turns OFF cursor
{
	SendCommand(0x0C);
}
void Character_LCD :: Blank(void)		//blanks LCD
{
	SendCommand(0x08);
}
void Character_LCD :: Visible(void)		//Shows LCD
{
	SendCommand(0x0C);
}
void Character_LCD :: CursorLeft(uint8_t n)	//Moves cursor by n poisitions left
{
	for (uint8_t i=0;i<n;i++)
	{
		SendCommand(0x10);
	}
}
void Character_LCD :: CursorRight(uint8_t n)	//Moves cursor by n poisitions left
{
	for (uint8_t i=0;i<n;i++)
	{
		SendCommand(0x14);
	}
}
void Character_LCD :: ProgressBar(uint8_t progress, uint8_t maxprogress, uint8_t length,uint8_t x, uint8_t y)
{
	uint8_t i;
	uint16_t pixelprogress;
	uint8_t c;
	GotoXY(x,y);
	pixelprogress = ((progress*(length*PROGRESSPIXELS_PER_CHAR))/maxprogress);
	for(i=0; i<length; i++)
	{
		if( ((i*(uint16_t)PROGRESSPIXELS_PER_CHAR)+5) > pixelprogress )
		if( ((i*(uint16_t)PROGRESSPIXELS_PER_CHAR)) > pixelprogress )
		c = 0;
		else
		c = pixelprogress % PROGRESSPIXELS_PER_CHAR;
		else
		c = 5;
		
		
		SendChar(c);
	}

}
void Character_LCD :: SendChar(uint8_t ch)		//Sends Char to LCD
{
	if ((ch&0b00010000)==0)
	_SFR_IO8(D4_PORT) &= ~(1<<D4_Bit) ;
	else
	_SFR_IO8(D4_PORT) |= 1<<D4_Bit ;
	if ((ch&0b00100000)==0)
	_SFR_IO8(D5_PORT) &= ~(1<<D5_Bit) ;
	else
	_SFR_IO8(D5_PORT) |= 1<<D5_Bit ;
	if ((ch&0b01000000)==0)
	_SFR_IO8(D6_PORT) &= ~(1<<D6_Bit) ;
	else
	_SFR_IO8(D6_PORT) |= 1<<D6_Bit ;
	if ((ch&0b10000000)==0)
	_SFR_IO8(D7_PORT) &= ~(1<<D7_Bit) ;
	else
	_SFR_IO8(D7_PORT) |= 1<<D7_Bit ;
	
	_SFR_IO8(RS_PORT)|=1<<RS_Bit;
	_SFR_IO8(E_PORT) |= 1<<E_Bit;
	_delay_ms(1);
	_SFR_IO8(E_PORT)&=~(1<<E_Bit);
	_SFR_IO8(RS_PORT)&=~(1<<RS_Bit);
	_delay_ms(1);
	
	if ((ch&0b00000001)==0)
	_SFR_IO8(D4_PORT) &= ~(1<<D4_Bit) ;
	else
	_SFR_IO8(D4_PORT) |= 1<<D4_Bit ;
	if ((ch&0b00000010)==0)
	_SFR_IO8(D5_PORT) &= ~(1<<D5_Bit) ;
	else
	_SFR_IO8(D5_PORT) |= 1<<D5_Bit ;
	if ((ch&0b00000100)==0)
	_SFR_IO8(D6_PORT) &= ~(1<<D6_Bit) ;
	else
	_SFR_IO8(D6_PORT) |= 1<<D6_Bit ;
	if ((ch&0b00001000)==0)
	_SFR_IO8(D7_PORT) &= ~(1<<D7_Bit) ;
	else
	_SFR_IO8(D7_PORT) |= 1<<D7_Bit ;
	
	_SFR_IO8(RS_PORT)|=1<<RS_Bit;
	_SFR_IO8(E_PORT)|=1<<E_Bit;
	_delay_ms(1);
	_SFR_IO8(E_PORT)&=~(1<<E_Bit);
	_SFR_IO8(RS_PORT)&=~(1<<RS_Bit);
	_delay_ms(1);
}
void Character_LCD :: SendCommand(uint8_t cmd)	//Sends Command to LCD
{
	if ((cmd&0b00010000)==0)_SFR_IO8(D4_PORT) &= ~(1<<D4_Bit) ;
	else _SFR_IO8(D4_PORT) |= 1<<D4_Bit ;
	if ((cmd&0b00100000)==0)_SFR_IO8(D5_PORT) &= ~(1<<D5_Bit) ;
	else _SFR_IO8(D5_PORT) |= 1<<D5_Bit ;
	if ((cmd&0b01000000)==0)_SFR_IO8(D6_PORT) &= ~(1<<D6_Bit) ;
	else _SFR_IO8(D6_PORT) |= 1<<D6_Bit ;
	if ((cmd&0b10000000)==0)_SFR_IO8(D7_PORT) &= ~(1<<D7_Bit) ;
	else _SFR_IO8(D7_PORT) |= 1<<D7_Bit ;
	
	_SFR_IO8(E_PORT)|=1<<E_Bit;
	_delay_ms(1);
	_SFR_IO8(E_PORT)&=~(1<<E_Bit);
	_delay_ms(1);

	if ((cmd&0b00000001)==0)_SFR_IO8(D4_PORT) &= ~(1<<D4_Bit) ;
	else _SFR_IO8(D4_PORT) |= 1<<D4_Bit ;
	if ((cmd&0b00000010)==0)_SFR_IO8(D5_PORT) &= ~(1<<D5_Bit) ;
	else _SFR_IO8(D5_PORT) |= 1<<D5_Bit ;
	if ((cmd&0b00000100)==0)_SFR_IO8(D6_PORT) &= ~(1<<D6_Bit) ;
	else _SFR_IO8(D6_PORT) |= 1<<D6_Bit ;
	if ((cmd&0b00001000)==0)_SFR_IO8(D7_PORT) &= ~(1<<D7_Bit) ;
	else _SFR_IO8(D7_PORT) |= 1<<D7_Bit ;
	
	_SFR_IO8(E_PORT)|=1<<E_Bit;
	_delay_ms(1);
	_SFR_IO8(E_PORT)&=~(1<<E_Bit);
	_delay_ms(1);
}
void Character_LCD :: Definechar(const uint8_t *pc,uint8_t char_code){
	uint8_t a = 0 , pcc = 0 ;
	uint16_t i;
	//a=(char_code<<3)|0x40;
	a=64+(char_code*8);
	for (i=0; i<9; i++){
		pcc=pgm_read_byte(&pc[i]);
		
		SendCommand(a);a++;
		SendChar(pcc);
	}
}
void Character_LCD :: Init(void)//Initializes LCD
{
	uint8_t chn ;
	_delay_ms(15);

	_SFR_IO8(D4_PORT) &= ~(1<<D4_Bit) ;_SFR_IO8(D5_PORT) &= ~(1<<D5_Bit) ;_SFR_IO8(D6_PORT) &= ~(1<<D6_Bit) ;_SFR_IO8(D7_PORT) &= ~(1<<D7_Bit) ;
	_SFR_IO8(E_PORT) &= ~(1<<E_Bit);
	_SFR_IO8(D4_DDR) |= 1<<D4_Bit ;_SFR_IO8(D5_DDR) |= 1<<D5_Bit ;_SFR_IO8(D6_DDR) |= 1<<D6_Bit ;_SFR_IO8(D7_DDR) |= 1<<D7_Bit ;
	_SFR_IO8(E_DDR)|=(1<<E_Bit);
	_SFR_IO8(RS_DDR)|=(1<<RS_Bit);_SFR_IO8(RW_DDR)|=(1<<RW_Bit);
	_SFR_IO8(D4_PORT) |= 1<<D4_Bit ;_SFR_IO8(D5_PORT) |= 1<<D5_Bit ;_SFR_IO8(D6_PORT) &= ~(1<<D6_Bit) ;_SFR_IO8(D7_PORT) &= ~(1<<D7_Bit) ;
	_SFR_IO8(E_PORT)|=(1<<E_Bit);
	_SFR_IO8(RS_PORT) &= ~(1<<RS_Bit);_SFR_IO8(RW_PORT) &= ~(1<<RW_Bit);
	
	_delay_ms(1);
	_SFR_IO8(E_PORT)&=~(1<<E_Bit);
	
	_delay_ms(1);
	_SFR_IO8(D4_PORT) |= 1<<D4_Bit ;_SFR_IO8(D5_PORT) |= 1<<D5_Bit ;_SFR_IO8(D6_PORT) &= ~(1<<D6_Bit) ;_SFR_IO8(D7_PORT) &= ~(1<<D7_Bit) ;
	_SFR_IO8(E_PORT)|=(1<<E_Bit);
	_SFR_IO8(RS_PORT) &= ~(1<<RS_Bit);_SFR_IO8(RW_PORT) &= ~(1<<RW_Bit);
	
	_delay_ms(1);
	_SFR_IO8(E_PORT)&=~(1<<E_Bit);
	
	_delay_ms(1);
	_SFR_IO8(D4_PORT) &= ~(1<<D4_Bit) ;_SFR_IO8(D5_PORT) |= 1<<D5_Bit ;_SFR_IO8(D6_PORT) &= ~(1<<D6_Bit) ;_SFR_IO8(D7_PORT) &= ~(1<<D7_Bit) ;
	
	_SFR_IO8(E_PORT)|=(1<<E_Bit);
	_SFR_IO8(RS_PORT) &= ~(1<<RS_Bit);_SFR_IO8(RW_PORT) &= ~(1<<RW_Bit);


	_delay_ms(1);
	_SFR_IO8(E_PORT)&=~(1<<E_Bit);
	_delay_ms(1);
	SendCommand(40);
	SendCommand(12);
	chn=0;
	while(chn<8)
	{
		
		Definechar((LcdCustomChar+(chn*8)),chn);
		chn++;
	}
}
void Character_LCD :: CopyStringtoLCD(const uint8_t *FlashLoc, uint8_t x, uint8_t y)
{
	uint8_t i;
	GotoXY(x,y);
	for(i=0;(uint8_t)pgm_read_byte(&FlashLoc[i]);i++)
	{
		SendChar((uint8_t)pgm_read_byte(&FlashLoc[i]));
	}
}

#endif