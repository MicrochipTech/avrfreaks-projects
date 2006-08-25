#include <avr/pgmspace.h>
#include <stdlib.h>

#include "hardware.h"
#include "lcd.h"
#include "utils.h"

#define lcdSetData() LCD_PORT |= _BV(LCD_A0)
#define lcdSetCmd() LCD_PORT &= ~_BV(LCD_A0)
#define lcdSetCLK() LCD_PORT |= _BV(LCD_CLK)
#define lcdClearCLK() LCD_PORT &= ~_BV(LCD_CLK)
#define lcdSetD() LCD_PORT |= _BV(LCD_D)
#define lcdClearD() LCD_PORT &= ~_BV(LCD_D)
#define lcdSetRST() LCD_PORT |= _BV(LCD_RST)
#define lcdClearRST() LCD_PORT &= ~_BV(LCD_RST)
#define lcdDelay()	asm volatile ("nop\n nop\n nop\n nop\n")

uint8_t lcdColumn;
uint8_t lcdLine;

void lcdWrite(uint8_t data)
{
	uint8_t shifter=0x80;
	
	while (shifter > 0)
	{
		if (shifter & data) lcdSetD();
		else lcdClearD();
		lcdDelay();
		lcdSetCLK();
		lcdDelay();
		lcdClearCLK();
		lcdDelay();
		shifter = shifter >> 1;
	}
}


void lcdWriteCommand(uint8_t command)
{
	lcdSetCmd();
	lcdWrite(command);
}

void lcdWriteData(uint8_t data)
{
	lcdSetData();
	lcdWrite(data);
	lcdColumn ++;
}

void lcdSetLocation(uint8_t column, uint8_t line)
{
	lcdColumn = column;
	lcdLine = line;
	lcdWriteCommand(LCD_SET_LINE + lcdLine);
	lcdWriteCommand(LCD_SET_COL_H + (lcdColumn >> 4));
	lcdWriteCommand(LCD_SET_COL_L + (lcdColumn & 0x0F));
}

void lcdDrawWxHChar(char c, int8_t shift, uint8_t fontFirstChar, uint8_t fontLastChar, uint8_t fontSpacing,
	uint8_t fontROMSize, uint8_t fontHeightAsLines, uint16_t fontBaseROMAddress) 
{
	uint8_t i, j, initialColumn, initialLine, fontROMSizePerLine;
	uint16_t charAddress = 0;	// by default assume that the character is not defined in the font
	uint8_t dataToWrite = 0;	// by default assume that the character is not defined in the font

	initialColumn = lcdColumn;
	initialLine = lcdLine;
	fontROMSizePerLine = fontROMSize / fontHeightAsLines;
	if ((c >= fontFirstChar) && (c <= fontLastChar)) 
		charAddress = fontBaseROMAddress + (c - fontFirstChar) * fontROMSize;
	for (j=0; j<fontHeightAsLines; j++)
	{
		if (charAddress) dataToWrite = pgm_read_byte_near(charAddress ++);
		for (i=0; i<fontROMSizePerLine; i++) 
			lcdWriteData(shift < 0 ? dataToWrite << -shift : dataToWrite >> shift);
		// prepare for writing next character
		for (i=0; i<fontSpacing; i++) lcdWriteData(0);
		if (fontHeightAsLines > 1) lcdSetLocation(initialColumn, lcdLine + 1);
	}
	// restore correct position if required
	if (fontHeightAsLines > 1) lcdSetLocation(lcdColumn, initialLine);
}

#define lcdDraw5x7Char(c, shift) lcdDrawWxHChar(c, shift, LCDFONT5X7_START_CHAR, LCDFONT5X7_END_CHAR, LCDFONT5X7_SPACE, LCDFONT5X7_ROM_SIZE, 1, (uint16_t)lcdFont5x7)
#define lcdDraw3x5Char(c, shift) lcdDrawWxHChar(c, shift, LCDFONT3X5_START_CHAR, LCDFONT3X5_END_CHAR, LCDFONT3X5_SPACE, LCDFONT3X5_ROM_SIZE, 1, (uint16_t)lcdFont3x5)
#define lcdDraw10x14Char(c) lcdDrawWxHChar(c, 0, LCDFONT10X14_START_CHAR, LCDFONT10X14_END_CHAR, LCDFONT10X14_SPACE, LCDFONT10X14_ROM_SIZE, 2, (uint16_t)lcdFont10x14);

void lcdDrawNx7Char(char c, int8_t shift)
{
	uint8_t i, width, dataToWrite;
	uint16_t charAddress;
	
	if ((c < LCDFONTNX7_START_CHAR) || (c > LCDFONTNX7_END_CHAR)) return;
	charAddress = (uint16_t)lcdFontNx7;
	for (i=0; i<c-LCDFONTNX7_START_CHAR; i++) charAddress += pgm_read_byte_near(charAddress) + 1;
	width = pgm_read_byte_near(charAddress ++);
	for (i=0; i<width; i++) 
	{
		dataToWrite = pgm_read_byte_near(charAddress ++);
		lcdWriteData(shift < 0 ? dataToWrite << -shift : dataToWrite >> shift);
	}
	for (i=0; i<LCDFONTNX7_SPACE; i++) lcdWriteData(0);
}

void lcdInit(void)
{
	uint8_t i;
	
	lcdColumn = 0;
	lcdLine = 0;
	
	LCD_DDR |= LCD_MASK;
	lcdClearRST();
	for (i=0; i<0xFF; i++) lcdDelay();
	lcdSetRST();
	lcdWriteCommand(LCD_SET_BIAS);
	lcdWriteCommand(LCD_SET_ADC);
	lcdWriteCommand(LCD_SET_CONTRAST);
	lcdWriteCommand(LCD_SET_POWER_CTRL);
	lcdWriteCommand(LCD_DISPLAY_ON);
	lcdSetLocation(0, 0);
}

void lcdClearLine(uint8_t line)
{
	uint8_t i;
	
	lcdSetLocation(0, line);
	for (i=0; i<LCD_WIDTH; i++) lcdWriteData(0);
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
		if (!s[i]) return; 
		if (font == LCD_FONT_10X14) 
		{
			if (s[i] == '.')
			{
				lcdSetLocation(lcdColumn, lcdLine + 1);
				lcdDrawNx7Char(LCDFONTNX7_BIGDOT, 0);	
				lcdSetLocation(lcdColumn, lcdLine - 1);
			}
			else if ((s[i] == '-') || (s[i] == '/')) lcdDrawNx7Char(LCDFONTNX7_BIGMINUS, 0);
			else lcdDraw10x14Char(s[i]);
		}
		else if (font == LCD_FONT_5X7) lcdDraw5x7Char(s[i], shift);
		else if (font == LCD_FONT_NX7) lcdDrawNx7Char(s[i], shift);
		else if (font == LCD_FONT_3X5) 
		{
			lcdDraw3x5Char(s[i], shift);
			if (s[i] == '.') lcdSetLocation(lcdColumn-2, lcdLine);
		}
	}
}

void lcdDrawCharAt(uint8_t column, uint8_t line, char c, uint8_t font)
{
	lcdDrawShiftedStringAt(column, line, &c, 1, font, 0);
}

/*
void lcdDrawChar(char c, uint8_t font)
{
	lcdDrawShiftedStringAt(lcdColumn, lcdLine, &c, 1, font, 0);
}
*/

uint8_t lcdGetFontOccupancy(uint8_t font)
{
	uint8_t width = LCDFONT5X7_WIDTH;
	
	if (font == LCD_FONT_10X14) width = LCDFONT10X14_WIDTH;
	else if (font == LCD_FONT_3X5) width = LCDFONT3X5_WIDTH;
	return width;
}

void lcdDrawShiftedDecNumberAt(uint8_t rightCharColumn, uint8_t line, int16_t number, uint8_t dec, uint8_t font, int8_t shift)
{
	char temp[10];
	uint8_t length;
	
	length = int16ToString(number, temp);
	lcdDrawShiftedStringAt(rightCharColumn-lcdGetFontOccupancy(font)*(length-1), line, temp, length, font, shift);
	if (dec > 10) return;
	temp[0] = '.';
	temp[1] = '0' + dec;
	lcdDrawShiftedStringAt(rightCharColumn+lcdGetFontOccupancy(font), line, temp, 2, font, shift);
}

void lcdDrawBitmapAt(uint8_t column, uint8_t line, uint8_t *bitmap, uint8_t bitmapSize)
{
	uint8_t i;
	
	lcdSetLocation(column, line);
	for (i=0; i<bitmapSize; i++) 
	{
		if (lcdColumn == LCD_WIDTH) lcdSetLocation(0, lcdLine + 1);
		lcdWriteData(bitmap[i]);
	}
}


