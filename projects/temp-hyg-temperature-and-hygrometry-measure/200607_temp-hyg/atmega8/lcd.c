#include <avr/pgmspace.h>
#include <stdlib.h>

#include "hardware.h"
#include "lcd.h"
#include "utils.h"
#include "dmx973b.h"

#define LCD_FORCE_WRITE	1
#define LCD_DO_NOT_FORCE_WRITE 0
 
uint8_t lcdColumn;
uint8_t lcdLine;
uint8_t lcdScreenData[LCD_HEIGHT/8][LCD_WIDTH];

void lcdSetLocation(uint8_t column, uint8_t line)
{
	if (column >= LCD_WIDTH) return;
	if (line >= LCD_HEIGHT/8) return;
	lcdColumn = column;
	lcdLine = line;
	lcdHardwareSetLocation(lcdColumn, lcdLine);
}

void lcdInit(void)
{
	lcdColumn = 0;
	lcdLine = 0;	
	lcdHardwareInit();
	lcdSetLocation(0, 0);
}

void lcdWriteData(uint8_t data, uint8_t force)
{
	// Do not go beyond the end of screen
	if (lcdColumn >= LCD_WIDTH) return;
	
	// to avoid flickering, check that we are attempting to write something different before writing
	if ((force == LCD_FORCE_WRITE) || (data != lcdScreenData[lcdLine][lcdColumn]))
	{
		lcdHardwareWriteData(data);
		lcdScreenData[lcdLine][lcdColumn] = data;
		lcdColumn ++;
	}
	else lcdSetLocation(lcdColumn + 1, lcdLine);
}

void lcdDraw10x14CharLine(char c, uint8_t romOffset)
{
	uint8_t i;
	uint16_t charAddress;

	// if character not defined
	if ((c < LCDFONT10X14_START_CHAR) || (c > LCDFONT10X14_END_CHAR))
	{
		// write a space
		for (i=0; i<LCDFONT10X14_WIDTH; i++) lcdWriteData(0, LCD_DO_NOT_FORCE_WRITE);
		return;
	}
	// write the pattern of the line
	charAddress = (uint16_t)lcdFont10x14 + romOffset + (c - LCDFONT10X14_START_CHAR) * LCDFONT10X14_ROM_SIZE * 2;
	for (i=0; i<LCDFONT10X14_ROM_SIZE; i++) lcdWriteData(pgm_read_byte_near(charAddress + i), LCD_DO_NOT_FORCE_WRITE);
	// prepare for writing next character
	for (i=0; i<LCDFONT10X14_SPACE; i++) lcdWriteData(0, LCD_DO_NOT_FORCE_WRITE);
}

void lcdDraw10x14Char(char c)
{
	uint8_t initialColumn;
	
	initialColumn = lcdColumn;
	lcdDraw10x14CharLine(c, 0);
	lcdSetLocation(initialColumn, lcdLine + 1);
	lcdDraw10x14CharLine(c, LCDFONT10X14_ROM_SIZE);
	lcdSetLocation(lcdColumn, lcdLine - 1);
}

void lcdDraw5x7Char(char c)
{
	volatile uint8_t i;
	uint16_t charAddress;
	
	// if character not defined
	if ((c < LCDFONT5X7_START_CHAR) || (c > LCDFONT5X7_END_CHAR)) 
	{
		// write a space
		for (i=0; i<LCDFONT5X7_WIDTH; i++) lcdWriteData(0, LCD_DO_NOT_FORCE_WRITE);
		return;
	}
	// write the pattern
	charAddress = (uint16_t)lcdFont5x7 + (c - LCDFONT5X7_START_CHAR) * LCDFONT5X7_ROM_SIZE;
	for (i=0; i<LCDFONT5X7_ROM_SIZE; i++) lcdWriteData(pgm_read_byte_near(charAddress + i), LCD_DO_NOT_FORCE_WRITE);
	// prepare for writing next character
	for (i=0; i<LCDFONT5X7_SPACE; i++) lcdWriteData(0, LCD_DO_NOT_FORCE_WRITE);
}

void lcdDrawNx7Char(char c)
{
	uint8_t i, width;
	uint16_t charAddress;
	
	if ((c < LCDFONTNX7_START_CHAR) || (c > LCDFONTNX7_END_CHAR)) return;
	charAddress = (uint16_t)lcdFontNx7;
	for (i=0; i<c-LCDFONTNX7_START_CHAR; i++) charAddress += pgm_read_byte_near(charAddress) + 1;
	width = pgm_read_byte_near(charAddress);
	charAddress ++;
	for (i=0; i<width; i++) lcdWriteData(pgm_read_byte_near(charAddress + i), LCD_DO_NOT_FORCE_WRITE);
	for (i=0; i<LCDFONTNX7_SPACE; i++) lcdWriteData(0, LCD_DO_NOT_FORCE_WRITE);
}

void lcdDraw3x5Char(char c, int8_t shift)
{
	uint8_t i;
	char uC;
	uint16_t charAddress;

	// set c uppercase
	uC = c;
	if ((uC >= 'a') && (uC <= 'z')) uC -= 'a'-'A';
	// if character not defined
	if ((uC < LCDFONT3X5_START_CHAR) || (uC > LCDFONT3X5_END_CHAR)) 
	{	
		// write a space
		for (i=0; i<LCDFONT3X5_WIDTH; i++) lcdWriteData(0, LCD_DO_NOT_FORCE_WRITE);
		return;
	}
	charAddress = (uint16_t)lcdFont3x5 + (uC - LCDFONT3X5_START_CHAR) * LCDFONT3X5_ROM_SIZE;
	for (i=0; i<LCDFONT3X5_ROM_SIZE; i++) 
	{
		uint8_t dataToWrite = pgm_read_byte_near(charAddress + i);
		lcdWriteData(shift < 0 ? dataToWrite >> -shift : dataToWrite << shift, LCD_DO_NOT_FORCE_WRITE);
	}
	// write the spacer
	for (i=0; i<LCDFONT3X5_SPACE; i++) lcdWriteData(0, LCD_DO_NOT_FORCE_WRITE);
}

void lcdClearLine(uint8_t line)
{
	uint8_t i;
	
	lcdSetLocation(0, line);
	for (i=0; i<LCD_WIDTH; i++) lcdWriteData(0, LCD_FORCE_WRITE);
}

void lcdClearScreen(void)
{
	uint8_t i;

	for (i=0; i<LCD_HEIGHT/8; i++) lcdClearLine(i);
}

void lcdDrawShiftedStringAt(uint8_t column, uint8_t line, char *s, uint8_t len, uint8_t font, int8_t shift)
{
	uint8_t i;
	
	lcdSetLocation(column, line);
	for (i=0; i<len; i++) 
	{
		if (s[i] == 0) return;
		if (font == LCD_FONT_10X14) 
		{
			if (s[i] == '.')
			{
				lcdSetLocation(lcdColumn, lcdLine + 1);
				lcdDrawNx7Char(LCDFONTNX7_BIGDOT /*, 0 */);	
				lcdSetLocation(lcdColumn + 2, lcdLine - 1);
			}
			else if ((s[i] == '-') || (s[i] == '/')) lcdDrawNx7Char(LCDFONTNX7_BIGMINUS /*, 0 */);
			else lcdDraw10x14Char(s[i]);
		}
		else if (font == LCD_FONT_5X7) lcdDraw5x7Char(s[i] /*, shift */);
		else if (font == LCD_FONT_NX7) lcdDrawNx7Char(s[i] /*, shift */);
		else if (font == LCD_FONT_3X5) lcdDraw3x5Char(s[i], shift);
		
		if (s[i] == '.') lcdSetLocation(lcdColumn-2, lcdLine);
	}
}

void lcdDrawShiftedCharAt(uint8_t column, uint8_t line, char c, uint8_t font, uint8_t shift)
{
	lcdDrawShiftedStringAt(column, line, &c, 1, font, shift);
}

uint8_t lcdGetFontOccupancy(uint8_t font)
{
	uint8_t width = LCDFONT5X7_WIDTH;
	
	if (font == LCD_FONT_10X14) width = LCDFONT10X14_WIDTH;
	else if (font == LCD_FONT_3X5) width = LCDFONT3X5_WIDTH;
	return width;
}

/*
 *	Draws a number, with one decimal value if required, positioning the unit character at unitCharColumn:
 *		lcdDrawShiftedDecNumberAt(x, y, 100, 0, n, n, n) will show '100'
 *		lcdDrawShiftedDecNumberAt(x, y, 10, LCD_IS_DEC, n, n, n) will show '10.1' 
 *  The function will make the number and front-padding spaces take 'occupy' font_width
 */
void lcdDrawShiftedDecNumberAt(uint8_t unitCharColumn, uint8_t line, int16_t number, uint8_t isDec, uint8_t occupy, uint8_t font, int8_t shift)
{
	char temp[10];
	uint8_t i, length, x;
	
	length = int16ToString((isDec ? number / 10 : number), temp);
	x = unitCharColumn-lcdGetFontOccupancy(font)*(length-1);
	if (isDec)
	{
		temp[length] = '.';
		temp[length+1] = '0' + number % 10;
	}
	lcdDrawShiftedStringAt(x, line, temp, length + (isDec ? 2 : 0), font, shift);
	// Erase any leftover character from last display
	for (i=0; i<=occupy; i++) temp[i] = 0; 
	x = unitCharColumn-lcdGetFontOccupancy(font)*(occupy-1);
	lcdDrawShiftedStringAt(x, line, temp, occupy-length, font, shift);	
}


void lcdDrawBitmapAt(uint8_t column, uint8_t line, uint8_t *bitmap, uint8_t bitmapSize)
{
	uint8_t i;
	
	lcdSetLocation(column, line);
	for (i=0; i<bitmapSize; i++) 
	{
		if (lcdColumn == LCD_WIDTH) lcdSetLocation(0, lcdLine + 1);
		lcdWriteData(bitmap[i], LCD_DO_NOT_FORCE_WRITE);
	}
}

void lcdDrawHorizontalPattern(uint8_t line, uint8_t pattern, uint8_t start, uint8_t end)
{
	uint8_t i;
	uint8_t bitmap[LCD_WIDTH];

	for (i=0; i<end-start; i++) bitmap[i] = lcdScreenData[line][i + start] | pattern;
	lcdDrawBitmapAt(start, line, bitmap, end-start);
}	

void lcdDrawVerticalPattern(uint8_t column, uint8_t pattern, uint8_t start, uint8_t end)
{
	uint8_t i;

	for (i=start; i<=end; i++) 
	{
		lcdSetLocation(column, i);
		lcdWriteData(pattern, LCD_DO_NOT_FORCE_WRITE);
	}
}	

void lcdDrawDot(uint8_t x, uint8_t y, uint8_t on)
{
	uint8_t mask, line;
	
	line = y / 8;
	lcdSetLocation(x, line);
	mask = 0x01 << (y - (line * 8));
	if (on) lcdWriteData(lcdScreenData[line][x] | mask, LCD_DO_NOT_FORCE_WRITE);
	else lcdWriteData(lcdScreenData[line][x] & ~mask, LCD_DO_NOT_FORCE_WRITE);
}	
