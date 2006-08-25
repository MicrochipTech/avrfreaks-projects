#ifndef LCD_H
#define LCD_H

#include "lcdFont3x5.h"
#include "lcdFont10x14.h"
#include "lcdFont5x7.h"
#include "lcdFontNx7.h"

#define LCD_FONT_3X5 1
#define LCD_FONT_5X7 2
#define LCD_FONT_10X14 3
#define LCD_FONT_NX7 4

#define LCD_FONT_SMALL LCD_FONT_3X5
#define LCD_FONT_MEDIUM LCD_FONT_5X7
#define LCD_FONT_BIG LCD_FONT_10X14
#define LCD_FONT_SPECIAL LCD_FONT_NX7

#define LCD_FONT_SMALL_WIDTH LCDFONT3X5_WIDTH
#define LCD_FONT_MEDIUM_WIDTH LCDFONT5X7_WIDTH
#define LCD_FONT_BIG_WIDTH LCDFONT10X14_WIDTH

#define LCD_LINE_DASHES	0xFC
#define LCD_LINE_PLAIN	0xFF
#define LCD_LINE_DOTS	0xAA

#define LCD_IS_DEC 1

void lcdInit(void);
void lcdClearLine(uint8_t);
void lcdClearScreen(void);
void lcdDrawShiftedStringAt(uint8_t, uint8_t, char *, uint8_t, uint8_t, int8_t);
#define lcdDrawStringAt(column, line, string, slength, font)  lcdDrawShiftedStringAt(column, line, string, slength, font, 0)
void lcdDrawShiftedCharAt(uint8_t, uint8_t, char, uint8_t, uint8_t);
#define lcdDrawCharAt(column, line, charCode, font)  lcdDrawShiftedCharAt(column, line, charCode, font, 0)
void lcdDrawChar(char, uint8_t);
void lcdDrawShiftedDecNumberAt(uint8_t, uint8_t, int16_t, uint8_t, uint8_t, uint8_t, int8_t);
#define lcdDrawDecNumberAt(column, line, number, isDec, occupy, font)  lcdDrawShiftedDecNumberAt(column, line, number, isDec, occupy, font, 0)
#define lcdDrawNumberAt(column, line, number, occupy, font)  lcdDrawShiftedDecNumberAt(column, line, number, 0, occupy, font, 0)
#define lcdDrawShiftedNumberAt(column, line, number, occupy, font, shift)  lcdDrawShiftedDecNumberAt(column, line, number, 0, occupy, font, shift)
void lcdDrawBitmapAt(uint8_t, uint8_t, uint8_t *, uint8_t);
void lcdDrawHorizontalPattern(uint8_t, uint8_t, uint8_t, uint8_t);
void lcdDrawVerticalPattern(uint8_t, uint8_t, uint8_t, uint8_t);
void lcdDrawDot(uint8_t, uint8_t, uint8_t);

#endif
