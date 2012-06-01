/********************************************** 

	LCD.C
	
	16x2 LCD  4-bit interfacing 
	
	Author: Visakhan C
	
	
	
***********************************************/

#include "lcd.h"

static uint8_t lcdX,lcdY;
static void IncrY(void);
static void DecrY(void);


static void IncrY()
{
	if(++lcdY > 39) {
		lcdY = 0;
		lcdX ^= 1;
	}
}

static void DecrY()
{
	if(lcdY-- == 0) {
	
		lcdY = 39;
		lcdX ^= 1;
	}

}


//////////////////////////	PUBLIC FUNCTIONS   //////////////////////


void lcdInit(void)
//initialise LCD module - make it ready for display
{

	DDR(DATA_PORT) |= 0x0F<<DATA_POS;
	DDR(E_PORT) |= 1<<E_POS;
	DDR(RW_PORT) |= 1<<RW_POS;
	DDR(RS_PORT) |= 1<<RS_POS;

	clearRW();
	clearRS();
	PORT(DATA_PORT) &= ~(0X0F<<DATA_POS); // Clear DATA PORT
	
	_delay_ms(25); // INITIAL DELAY
	
	setE();	
	PORT(DATA_PORT) |= (0x03<<DATA_POS);
	_delay_us(10);
	clearE();
	
	_delay_ms(5); // SECOND DELAY
	
	setE();	
	_delay_us(10); // SEND 0X03 AGAIN
	clearE();
	
	_delay_us(200); // THIRD DELAY
	
	setE();	
	PORT(DATA_PORT) &= ~(0X0F<<DATA_POS);
	PORT(DATA_PORT) |= (0X02<<DATA_POS);	//set 4-bit mode by tranfering (0010)  
	_delay_us(10);
	clearE();
	_delay_us(50);
	
	lcdCommand( LCD_FUNC|(1<<FUNC_2LINES)|(0<<FUNC_10DOTS)); // 2 line, 5x7 dot
	lcdOff(); // display ON! 
	lcdClear(); // display clear
	lcdEntryLeft(); // increment, no shift 
	lcdOn();
	
	lcdX=lcdY=0;
}

void lcdByte(unsigned char b, unsigned char isdata)
// send a byte to LCD
{
	
	unsigned char hn = b>>4;	// HIGH NIBBLE
	unsigned char ln = b&0x0F;	//LOW NIBBLE
	
	
	clearRW();  //Write mode
	if(isdata == 0) {
	
		clearRS();
	}
	else {
		IncrY();	
		setRS();
	}

	
	setE();	//send high nibble first
	
	PORT(DATA_PORT) &= ~(0x0F<<DATA_POS); 
	PORT(DATA_PORT) |= (hn<<DATA_POS);
	_delay_us(10);
	clearE();
	
	_delay_us(50);
	
	setE();  //send low nibble
	
	PORT(DATA_PORT) &= ~(0x0F<<DATA_POS);
	PORT(DATA_PORT) |= (ln<<DATA_POS);
	_delay_us(10);
	clearE();
	_delay_us(150);
	
}

void lcdString(uint8_t *str)
//write a string to LCD
{
	while(*str) {
	
		lcdChar(*(str++));
	}
}


void lcdString_P(const uint8_t *FlashLoc)
//Write string stored in Flash to LCD
{
	uint8_t ch;
	while(( ch=(uint8_t)pgm_read_byte(FlashLoc++) )) 
		lcdChar(ch);
}



/* 	Display 8-bit Numbers	*/
void lcdNumberSmall(uint8_t N)
{
	uint8_t d=0;
	d = N/100;
	if(d)
		lcdChar(d+48);
	N -= (d*100);
	d = N/10;
	lcdChar(d+48);
	lcdChar( (N%10)+48 );

}

//////////////////
//				//
// LCD COMMANDS	//
// 				//
//////////////////

void lcdClear(void)
//clear LCD
{
	lcdCommand(1<<LCD_CLEAR);
	_delay_ms(3);
	lcdX = lcdY = 0;
}

void lcdHome(void)
//clear LCD
{
	lcdCommand(1<<LCD_HOME);
	_delay_ms(3);
	lcdX = lcdY = 0;
}

void lcdCursorLeft(uint8_t n)	
//Moves cursor by n poisitions left
{
	for (uint8_t i=0;i<n;i++)
	{
		DecrY();
		lcdCursorLeftOnce();
	}
}

void lcdCursorRight(uint8_t n)	
//Moves cursor by n poisitions right
{
	for (uint8_t i=0;i<n;i++)
	{
		IncrY();
		lcdCursorRightOnce();
	}
}

void lcdShiftLeft(unsigned char n)
{
	for (uint8_t i=0;i<n;i++)
	{
		lcdShiftLeftOnce();
	}
}


void lcdShiftRight(unsigned char n)
{
	
	for(uint8_t i=0;i<n;i++)
	{
		lcdShiftRightOnce();
	}

}


void lcdGotoXY(uint8_t x, uint8_t y)	//Cursor to Row:X Col:Y position
{
	lcdX = x;
	lcdY = y;
	if(x==0) 
		lcdCommand(0x80+y);  //first line
	else
		lcdCommand(0xC0+y);	//second line
	
}


uint8_t lcdGetX(void)
{
	return lcdX;
}

uint8_t lcdGetY(void)
{
	return lcdY;
}
