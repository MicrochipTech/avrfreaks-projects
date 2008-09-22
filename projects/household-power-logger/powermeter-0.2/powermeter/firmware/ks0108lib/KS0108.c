//-------------------------------------------------------------------------------------------------
// Universal KS0108 driver library
// (c) Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#include "KS0108.h"
#include "font5x8.h"
//-------------------------------------------------------------------------------------------------
unsigned char char_mode = CHAR_NORMAL;
unsigned char screen_x = 0, screen_y = 0;
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_Initalize(void)
{
unsigned char i;
for(i = 0; i < 3; i++)
	GLCD_WriteCommand((DISPLAY_ON_CMD | ON), i);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_GoTo(unsigned char x, unsigned char y)
{
unsigned char i;
screen_x = x;
screen_y = y;

for(i = 0; i < KS0108_SCREEN_WIDTH/64; i++)
	{
	GLCD_WriteCommand(DISPLAY_SET_Y | 0,i);
	GLCD_WriteCommand(DISPLAY_SET_X | y,i);
	GLCD_WriteCommand(DISPLAY_START_LINE | 0,i);
	}
GLCD_WriteCommand(DISPLAY_SET_Y | (x % 64), (x / 64));
GLCD_WriteCommand(DISPLAY_SET_X | y, (x / 64));
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_ClearScreen(void)
{
unsigned char i, j;
for(j = 0; j < KS0108_SCREEN_HEIGHT/8; j++)
	{
	GLCD_GoTo(0,j);
	for(i = 0; i < KS0108_SCREEN_WIDTH; i++)
		GLCD_WriteData(0x00);
	}
}

void GLCD_ClearToRowEnd() {
	while(screen_x < KS0108_SCREEN_WIDTH)
		GLCD_WriteData(0x00);
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_WriteChar(char charToWrite)
{
	if(
		charToWrite == CHAR_NORMAL ||
		charToWrite == CHAR_INVERSE ||
		charToWrite == CHAR_UNDERLINE
	) {
		char_mode = charToWrite;
		return;
	}
	int i;
	charToWrite -= 32; 
	for(i = 0; i < 6; i++) {
		unsigned char dat = 0;
		if(i < 5) {
			dat = GLCD_ReadByteFromROMMemory((char *)((int)font5x8 + (5 * charToWrite) + i)); 
		}
		if(char_mode == CHAR_INVERSE) dat = ~dat;
		if(char_mode == CHAR_UNDERLINE) dat |= 0x80;
		GLCD_WriteData(dat);
	}
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_WriteString(char * stringToWrite)
{
while(*stringToWrite)
	GLCD_WriteChar(*stringToWrite++);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_SetPixel(unsigned char x, unsigned char y, unsigned char color)
{
unsigned char tmp;
GLCD_GoTo(x, (y / 8));
tmp = GLCD_ReadData();
GLCD_GoTo(x, (y / 8));
tmp = GLCD_ReadData();
GLCD_GoTo(x, (y / 8));
tmp |= (1 << (y % 8));
GLCD_WriteData(tmp);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_Bitmap(char * bmp, unsigned char x, unsigned char y, unsigned char dx, unsigned char dy)
{
unsigned char i, j;
for(j = 0; j < dy / 8; j++)
	{
	GLCD_GoTo(x,y + j);
	for(i = 0; i < dx; i++) 
		GLCD_WriteData(GLCD_ReadByteFromROMMemory(bmp++));
	}
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------





