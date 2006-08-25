#include <avr/pgmspace.h>

#include "screens.h"
#include "lcd.h"
#include "clock.h"
#include "event.h"
#include "db.h"
#include "utils.h"

uint8_t screenClockAdjust;
uint8_t screen;
uint8_t screenMenuLine;
uint8_t screenGraphPeriod;

void screenDateAndTimeRedraw(void);

#define SCREEN_HOME_TIME_START (LCD_WIDTH-LCD_FONT_SMALL_WIDTH*21)/2
void screenTimeRefresh(void)
{		
	char temp[10];
	
	if (screen == SCREEN_HOME) 
	{
		clockGetTimeAsString(temp, CLOCK_WITH_SECONDS);
		lcdDrawShiftedStringAt(SCREEN_HOME_TIME_START, 3, temp, 8, LCD_FONT_SMALL, 3);
		lcdDrawShiftedStringAt(SCREEN_HOME_TIME_START+8*LCD_FONT_SMALL_WIDTH, 3, " - ", 3, LCD_FONT_SMALL, 3);
		clockGetDateAsString(temp, CLOCK_WITH_LONG_YEARS);
		lcdDrawShiftedStringAt(SCREEN_HOME_TIME_START+11*LCD_FONT_SMALL_WIDTH, 3, temp, 10, LCD_FONT_SMALL, 3);
		
		lcdDrawHorizontalPattern(3, 0x01, 0, LCD_WIDTH);
	}
	else if (screen == SCREEN_TIME_AND_DATE) screenDateAndTimeRedraw();
}


// ----------------- SCREEN_HOME ---------------------------------

void screenSensorsRefresh(void)
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
	screenSensorsRefresh();
	
	screenTimeRefresh();
}

// ----------------- SCREEN_MENU ---------------------------------

void screenMenuHighlight(uint8_t line)
{
	lcdDrawCharAt(0, screenMenuLine, ' ', LCD_FONT_MEDIUM);
	lcdDrawCharAt(94, screenMenuLine, ' ', LCD_FONT_MEDIUM);
	if (line == 255) screenMenuLine = SCREEN_MENU_LINES-1;
	if (line == SCREEN_MENU_LINES) screenMenuLine = 0;
	else screenMenuLine = line;
	lcdDrawCharAt(0, screenMenuLine, LCDFONTNX7_ARROW_RIGHT, LCD_FONT_SPECIAL);
	lcdDrawCharAt(94, screenMenuLine, LCDFONTNX7_ARROW_LEFT, LCD_FONT_SPECIAL);
}

void screenMenuDraw(void)
{
	PGM_P screenMenuLineText;
	uint8_t i;
	char temp[20];

	screen = SCREEN_MENU;
	lcdClearScreen();
	for (i=0; i<SCREEN_MENU_LINES; i++) 
	{
	    memcpy_P(&screenMenuLineText, &(screenMenu[i]), sizeof(PGM_P));
		strcpy_P(temp, screenMenuLineText);
		lcdDrawStringAt(2*LCD_FONT_MEDIUM_WIDTH, i, temp, 16, LCD_FONT_MEDIUM);
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
			else if (screen == SCREEN_TIME_AND_DATE) screenDateAndTimeDraw();
			else screenGraphDraw(screen);
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
	uint8_t line = 0, column, bitmap;
	char temp[10];

	lcdClearScreen();
	clockGetTimeAsString(temp, CLOCK_WITH_SECONDS);
	lcdDrawStringAt(SCREEN_TIME_START, 1, temp, 8, LCD_FONT_MEDIUM);
	clockGetDateAsString(temp, CLOCK_WITH_LONG_YEARS);
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

// ----------------- SCREEN_GRAPH --------------------------------

#define SCREEN_H_LABEL_X 10
#define SCREEN_T_LABEL_X 4

void screenGraphDrawVAxis(uint8_t x, int16_t min, int16_t max)
{
	uint8_t i; 
	int16_t middle;

	// vertical indications
	lcdDrawShiftedDecNumberAt(x, 0, max, x == SCREEN_T_LABEL_X, 2, LCD_FONT_SMALL, 0);
	middle = (min + max)/2;
	lcdDrawShiftedDecNumberAt(x, 1, middle, x == SCREEN_T_LABEL_X, 2, LCD_FONT_SMALL, 2);
	lcdDrawShiftedDecNumberAt(x, 2, min, x == SCREEN_T_LABEL_X, 2, LCD_FONT_SMALL, 4);
	lcdDrawShiftedDecNumberAt(x, 3, min, x == SCREEN_T_LABEL_X, 2, LCD_FONT_SMALL, -4);
	// vertical line
	lcdDrawVerticalPattern(15, 0xFF, 0, 2);	
	for (i=1; i<24; i+=10) lcdDrawDot(14, i, 1);
}

void screenGraphDrawHLabel(uint8_t x, uint8_t value, uint8_t symbol)
{
	if (symbol == 0xFE) lcdDrawShiftedNumberAt(x + 12, 3, value + CLOCK_BASE_YEAR, 2, LCD_FONT_SMALL, 3);
	else lcdDrawShiftedNumberAt(x+4, 3, value, 2, LCD_FONT_SMALL, 3);
	if (symbol < 0x7F) lcdDrawShiftedCharAt(x+8, 3, symbol, LCD_FONT_SMALL, 3); 
	lcdDrawDot(x+3, 25, 1);
}

void screenGraphDrawHAxis(uint8_t start, uint8_t end, uint8_t middle, uint8_t symbol, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t step)
{
	uint8_t i; 

	screenGraphDrawHLabel(x1, start, symbol);
	screenGraphDrawHLabel(x2, middle, symbol);
	screenGraphDrawHLabel(x3, end, symbol);
	lcdDrawHorizontalPattern(3, 0x01, x1, x3+4);
	for (i=x1+3; i<=x3+3; i+=step) lcdDrawDot(i, 23, 1);
}

uint8_t screenGraphCalculateY(uint8_t screen, uint8_t value, int16_t min, int16_t max)
{
	int16_t rValue = dbGetRealValue(screen == SCREEN_TEMPERATURE ? DB_TEMPERATURE : DB_HUMIDITY, value);
	return 21 - (20 * (rValue - min) / (max - min));
}

void screenGraphDrawGraph(uint8_t size, uint8_t step, dbValue_t *data, int16_t min, int16_t max)
{
	uint8_t x, y, y1, y2, i, j;
	
	for (i=0; i<size; i++)
	{
		if (data[i].value != 0xFF)
		{
			x = 18 + i*step;
			y = screenGraphCalculateY(screen, data[i].value, min, max);
			if (data[i].min != 0xFF)
			{
				y1 = screenGraphCalculateY(screen, data[i].max, min, max);
				y2 = screenGraphCalculateY(screen, data[i].min, min, max);
				for (j=y1; j<=y2; j++) lcdDrawDot(x, j, 1);
				lcdDrawDot(x+1, y, 1);
			}
			else 
			{
				for (j=y-1; j<=y+1; j++) lcdDrawDot(x, j, 1);
				lcdDrawDot(x-1, y, 1);
				lcdDrawDot(x+1, y, 1);
			}
		}
	}
}

dbValue_t screenGraphData[31];		// we use global variables to detect stack overflow
void screenGraphRedraw(void)
{
	uint8_t start, end, eMin, eMax;
	int16_t min, max;
	char temp[10];

	lcdClearScreen();
	dbGetSensorData(screen == SCREEN_TEMPERATURE ? DB_TEMPERATURE : DB_HUMIDITY, screenGraphPeriod, screenGraphData, &eMin, &eMax, &start, &end);
	min = dbGetRealValue(screen == SCREEN_TEMPERATURE ? DB_TEMPERATURE : DB_HUMIDITY, eMin);
	max = dbGetRealValue(screen == SCREEN_TEMPERATURE ? DB_TEMPERATURE : DB_HUMIDITY, eMax);
	if (screen == SCREEN_TEMPERATURE)
	{
		lcdDrawShiftedCharAt(89, 0, LCDFONT3X5_T, LCD_FONT_SMALL, 1); 
		lcdDrawCharAt(93, 0, LCDFONTNX7_SMALL_CELSIUS, LCD_FONT_SPECIAL);
	}
	else
	{
		lcdDrawShiftedCharAt(88, 0, LCDFONT3X5_UPPERCASE_H, LCD_FONT_SMALL, 1); 
		lcdDrawCharAt(92, 0, LCDFONTNX7_SMALL_PERCENT, LCD_FONT_SPECIAL);
	}
	screenGraphDrawVAxis(screen == SCREEN_TEMPERATURE ? SCREEN_T_LABEL_X : SCREEN_H_LABEL_X, min, max);
	
	switch (screenGraphPeriod)
	{
		case DB_HOUR:
			screenGraphDrawHAxis(start, end, (start+12)%24, LCDFONT3X5_LOWERCASE_H, 15, 51, 84, 3);
			screenGraphDrawGraph(24, 3, screenGraphData, min, max);
			break;
		case DB_DAY:
			screenGraphDrawHAxis(start, end, (start+13)%31+1, 0xFF, 15, 45, 75, 2);
			clockGetDateAsString(temp, CLOCK_WITH_SHORT_YEARS);
			lcdDrawShiftedStringAt(83, 3, &(temp[2]), 3, LCD_FONT_SMALL, 3);
			screenGraphDrawGraph(31, 2, screenGraphData, min, max);
			break;
		case DB_MONTH:
			screenGraphDrawHAxis(start, end, (start+4)%12+1, 0xFF, 15, 45, 81, 6);
			clockGetDateAsString(temp, CLOCK_WITH_SHORT_YEARS);
			lcdDrawShiftedStringAt(89, 3, &(temp[5]), 3, LCD_FONT_SMALL, 3);
			screenGraphDrawGraph(12, 6, screenGraphData, min, max);
			break;
		case DB_YEAR:
			screenGraphDrawHAxis(start, end, (start+end)/2, 0xFE, 15, 43, 78, 7);
			screenGraphDrawGraph(DB_NB_YEARS, 7, screenGraphData, min, max);
			break;
	}
}


void screenGraphRefresh(void)
{
	// refresh only if the screen is graph
	if ((screen != SCREEN_TEMPERATURE) && (screen != SCREEN_HUMIDITY)) return;
	// only refresh every hour
	if (eventGetCurrentEvent() < EVENT_TIME_HOUR) return;
	// only refresh the screen if the graphPeriod is corresponding to the clock
	if (eventGetCurrentEvent()-EVENT_TIME_HOUR != screenGraphPeriod-DB_HOUR) return;
	// redraw
	screenGraphRedraw();
}

void screenGraphDraw(uint8_t sensorScreen)
{
	screen = sensorScreen;
	screenGraphPeriod = DB_HOUR;
	screenGraphRedraw();
}

void screenGraphHandleButtons()
{
	switch (eventGetCurrentEvent())
	{
		case EVENT_BUTTON_ESC:
			screenHomeDraw();
			break;
		case EVENT_BUTTON_UP:
			screenGraphPeriod ++;
			if (screenGraphPeriod > DB_YEAR) screenGraphPeriod = DB_HOUR;
			screenGraphRedraw();
			break;
		case EVENT_BUTTON_DOWN:
			screenGraphPeriod --;
			if (screenGraphPeriod < DB_HOUR) screenGraphPeriod = DB_YEAR;
			screenGraphRedraw();
			break;
	}
}
