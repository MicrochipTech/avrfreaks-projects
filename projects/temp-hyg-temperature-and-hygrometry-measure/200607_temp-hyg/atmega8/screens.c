#include <avr/pgmspace.h>

#include "screens.h"
#include "lcd.h"
#include "clock.h"
#include "event.h"
#include "db.h"

uint8_t screenClockAdjust;
uint8_t screen;
uint8_t screenMenuLine;

void screenDateAndTimeRedraw(void);

#define SCREEN_HOME_TIME_START (LCD_WIDTH-LCD_FONT_SMALL_WIDTH*21)/2
void screenTimeDisplay(void)
{		
	if (screen == SCREEN_HOME) 
	{
		char temp[10];

		clockGetTimeAsString(temp, CLOCK_WITH_SECONDS);
		lcdDrawShiftedStringAt(SCREEN_HOME_TIME_START, 3, temp, 8, LCD_FONT_SMALL, 3);
		lcdDrawShiftedStringAt(SCREEN_HOME_TIME_START+8*LCD_FONT_SMALL_WIDTH, 3, " - ", 3, LCD_FONT_SMALL, 3);
		clockGetDateAsString(temp);
		lcdDrawShiftedStringAt(SCREEN_HOME_TIME_START+11*LCD_FONT_SMALL_WIDTH, 3, temp, 10, LCD_FONT_SMALL, 3);
		
		lcdDrawHorizontalPattern(3, 0x01, 0, LCD_WIDTH);
	}
	else if (screen == SCREEN_TIME_AND_DATE) screenDateAndTimeRedraw();
}


// ----------------- SCREEN_HOME ---------------------------------

void screenSensorsDisplay(void)
{
	dbSensor_t *s;
	
	if (screen != SCREEN_HOME) return;

	s = dbGetSensor(DB_TEMPERATURE);
	if (s->max < s->min) return;	// No measure recorded
	
	lcdDrawDecNumberAt(LCD_FONT_BIG_WIDTH, 0, s->value, LCD_IS_DEC, 2, LCD_FONT_BIG);
	lcdDrawShiftedDecNumberAt(4, 2, s->min, LCD_IS_DEC, 2, LCD_FONT_SMALL, 1);
	lcdDrawShiftedDecNumberAt(29, 2, s->max, LCD_IS_DEC, 2, LCD_FONT_SMALL, 1);
	
	s = dbGetSensor(DB_HUMIDITY);
	lcdDrawNumberAt(83, 0, s->value, 3, LCD_FONT_BIG);
	lcdDrawShiftedNumberAt(66, 2, s->min, 3, LCD_FONT_SMALL, 1);
	lcdDrawShiftedNumberAt(89, 2, s->max, 3, LCD_FONT_SMALL, 1);	
}

void screenHomeDraw(void)
{
	screen = SCREEN_HOME;
	lcdClearScreen();	
	// Draw the permanent elements of the sensor screen
	// separator
	lcdDrawVerticalPattern(55, 0xFF, 0, 2);
	// temperature elements
	lcdDrawCharAt(LCD_FONT_BIG_WIDTH*3+6, 0, LCDFONTNX7_CELSIUS, LCD_FONT_SPECIAL);
	lcdDrawCharAt(15, 2, LCDFONTNX7_ARROW, LCD_FONT_SPECIAL);
	lcdDrawCharAt(39, 2, LCDFONTNX7_SMALL_CELSIUS, LCD_FONT_SPECIAL);
	// humidity elements
	lcdDrawCharAt(95, 0, '%', LCD_FONT_MEDIUM);
	lcdDrawCharAt(71, 2, LCDFONTNX7_ARROW, LCD_FONT_SPECIAL);
	lcdDrawCharAt(93, 2, LCDFONTNX7_SMALL_PERCENT, LCD_FONT_SPECIAL);
	screenSensorsDisplay();
	
	screenTimeDisplay();
}

// ----------------- SCREEN_MENU ---------------------------------

void screenMenuHighlight(uint8_t line)
{
	lcdDrawCharAt(0, screenMenuLine+1, ' ', LCD_FONT_MEDIUM);
	lcdDrawCharAt(94, screenMenuLine+1, ' ', LCD_FONT_MEDIUM);
	if (line == 255) screenMenuLine = SCREEN_MENU_LINES-1;
	if (line == SCREEN_MENU_LINES) screenMenuLine = 0;
	else screenMenuLine = line;
	lcdDrawCharAt(0, screenMenuLine+1, LCDFONTNX7_ARROW_RIGHT, LCD_FONT_SPECIAL);
	lcdDrawCharAt(94, screenMenuLine+1, LCDFONTNX7_ARROW_LEFT, LCD_FONT_SPECIAL);
}

void screenMenuDraw(void)
{
	PGM_P screenMenuLineText;
	char temp[16];
	uint8_t i;

	screen = SCREEN_MENU;
	lcdClearScreen();
	for (i=0; i<SCREEN_MENU_LINES; i++) 
	{
	    memcpy_P(&screenMenuLineText, &(screenMenu[i]), sizeof(PGM_P));
		strcpy_P(temp, screenMenuLineText);
		lcdDrawStringAt(2*LCD_FONT_MEDIUM_WIDTH, i+1, temp, 16, LCD_FONT_MEDIUM);
	}
	screenMenuHighlight(0);
}

void screenMenuHandleButtons()
{
	switch (eventGetCurrentEvent())
	{
		case EVENT_BUTTON_ESC:
			screenHomeDraw();
			break;
		case EVENT_BUTTON_UP:
			screenMenuHighlight(screenMenuLine + 1);
			break;
		case EVENT_BUTTON_DOWN:
			screenMenuHighlight(screenMenuLine - 1);
			break;
		case EVENT_BUTTON_ENTER:
			screen = screenMenuLine;
			if (screen == SCREEN_HOME) screenHomeDraw();
			else screenDateAndTimeDraw();
			break;
		default:
			break;
	}
}

// ----------------- SCREEN_DATE_AND_TIME ------------------------

#define SCREEN_TIME_START (LCD_WIDTH-LCD_FONT_MEDIUM_WIDTH*8)/2
#define SCREEN_DATE_START (LCD_WIDTH-LCD_FONT_MEDIUM_WIDTH*10)/2
void screenDateAndTimeRedraw()
{
	char temp[10];
	uint8_t line = 0, column, bitmap;

	lcdClearScreen();
	clockGetTimeAsString(temp, CLOCK_WITH_SECONDS);
	lcdDrawStringAt(SCREEN_TIME_START, 1, temp, 8, LCD_FONT_MEDIUM);
	clockGetDateAsString(temp);
	lcdDrawStringAt(SCREEN_DATE_START, 2, temp, 10, LCD_FONT_MEDIUM);
	bitmap = LCDFONTNX7_HIGHLIGHT;
	if (screenClockAdjust >= 3) 
	{
		bitmap ++;
		column = SCREEN_TIME_START + (screenClockAdjust-3)*3*LCD_FONT_MEDIUM_WIDTH;
	}
	else
	{
		line = 3;
		column = SCREEN_DATE_START;											// days
		if (screenClockAdjust == 0) column += 8*LCD_FONT_MEDIUM_WIDTH; 		// years
		else if (screenClockAdjust == 1) column += 3*LCD_FONT_MEDIUM_WIDTH;	// months
	}
	lcdDrawCharAt(column, line, bitmap, LCD_FONT_SPECIAL);
}

void screenDateAndTimeDraw(void)
{
	screen = SCREEN_TIME_AND_DATE;
	screenClockAdjust = 0;			// highlight the year
	screenDateAndTimeRedraw();
}

void screenDateAndTimeHandleButtons(void)
{
	uint8_t *clock;
	
	clock = (uint8_t *)clockGet();
	switch (eventGetCurrentEvent())
	{
		case EVENT_BUTTON_ESC:
			eventAddEvent(EVENT_TIME_ADJUST);
			screenHomeDraw();
			break;
		case EVENT_BUTTON_UP:
			clock[screenClockAdjust] += 2;
		case EVENT_BUTTON_DOWN:
			clock[screenClockAdjust] --;
			clockSet(clock[0], clock[1], clock[2], clock[3], clock[4], 0);
			screenDateAndTimeRedraw();
			break;
		case EVENT_BUTTON_ENTER:
			screenClockAdjust ++;
			if (screenClockAdjust > 4) screenClockAdjust = 0;
			screenDateAndTimeRedraw();
			break;
		default:
			break;
	}
}
