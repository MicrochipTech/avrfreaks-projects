/*  This file is a part of ButterflySDK. ButterflySDK is a set of software
	modules used to program the Butterfly Evaluation Board for ATMEL.
	Emphasis is given in code re-use and in maintainability. ButterflySDK
	is compiled with GCC AVR compiler. Supporting tools are written in Python
	programming language.
	Copyright (C) 2007, Kostas Giannakakis

    ButterflySDK is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ButterflySDK is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ButterflySDK; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** @file clock.c Clock module. Used for displaying and editing the time and
	date. Time and date values are supplied from the Rtc module. The module
	settings (time format, date format) are edited from the Options module.
*/

#include <inttypes.h>
#include <avr/pgmspace.h>

#include "rtc.h"
#include "clock.h"
#include "lcd.h"
#include "button.h"
#include "bcd.h"

/*******************************************************************************
* Look-up Tables
*******************************************************************************/

/** Table used when displaying time in 12H format */
uint8_t clock12HTable[] =
{12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

// From Rtc module
extern char monthLength[12];

/** Table in flash, holding the positions of the day, month and year for all
	the date formats. The positions are stored sequentially. The first three
	bytes correspond to the European, the next three to the American and the
	last three to the Canadian date format. The first byte in each triplet is
	the day, the second one the month and the third the year position.
*/
const uint8_t dateFormatPosTable_F[] PROGMEM = {0, 1, 2,	// EUROPEAN
												1, 0, 2,	// AMERICAN
												2, 1, 0};	// CANADIAN

/** @def GET_DATE_FORMAT_START_POS Gets the starting position of the supplied
		date format in the #dateFormatPosTable_F table.*/
#define GET_DATE_FORMAT_START_POS(format) ((format) + (format) + (format))

/*******************************************************************************
* Static Variables
*******************************************************************************/

/** Clock date format. Could be one of DATE_EUROPE_FORMAT,
	DATE_AMERICAN_FORMAT or DATE_CANADIAN_FORMAT */
uint8_t clockDateFormat;

/** Clock time format. Could be either CLOCK_FORMAT_12H or CLOCK_FORMAT_24H */
uint8_t clockTimeFormat;

/** Current time editing  state. It only has meaning when time is
	displayed/edited */
ClockTimeState clockTimeState;

/** Current date editing state. It only has meaning when date is
	displayed/edited */
ClockDateState clockDateState;

/*******************************************************************************
* Functions
*******************************************************************************/

/** Initializes the Clock module. */
void ClockInit(void)
{
	clockDateFormat = DATE_EUROPE_FORMAT;
	clockTimeFormat = CLOCK_FORMAT_24H;
	clockTimeState = CLOCK_SHOW;
	clockDateState = DATE_SHOW;
}

/*******************************************************************************
* Time Functions
*******************************************************************************/

/** Displays the time. If the time isn't being edited, the time is taken from
	the Rtc module. If the time is being edited by the user, the appropriate
	characters are flashing. */
void ClockTimeUpdate(void)
{
	uint8_t hour, minute, second;

	hour = RtcGetHour();
	if (clockTimeFormat == CLOCK_FORMAT_12H)
		hour = clock12HTable[hour];

	hour = CharToBCD2(hour);
	minute = CharToBCD2(RtcGetMinute());
	second = CharToBCD2(RtcGetSecond());

	if (clockTimeState == CLOCK_EDIT_HOUR)
	{
		LcdWriteChar(0, BCD_HIGH_NIBBLE_CHAR(hour) | 0x80);
		LcdWriteChar(1, BCD_LOW_NIBBLE_CHAR(hour) | 0x80);
	}
	else
	{
		LcdWriteChar(0, BCD_HIGH_NIBBLE_CHAR(hour));
		LcdWriteChar(1, BCD_LOW_NIBBLE_CHAR(hour));
	}
	if (clockTimeState == CLOCK_EDIT_MINUTE)
	{
		LcdWriteChar(2, BCD_HIGH_NIBBLE_CHAR(minute) | 0x80);
		LcdWriteChar(3, BCD_LOW_NIBBLE_CHAR(minute) | 0x80);
	}
	else
	{
		LcdWriteChar(2, BCD_HIGH_NIBBLE_CHAR(minute));
		LcdWriteChar(3, BCD_LOW_NIBBLE_CHAR(minute));
	}
	if (clockTimeState == CLOCK_EDIT_SECOND)
	{
		LcdWriteChar(4, BCD_HIGH_NIBBLE_CHAR(second) | 0x80);
		LcdWriteChar(5, BCD_LOW_NIBBLE_CHAR(second) | 0x80);
	}
	else
	{
		LcdWriteChar(4, BCD_HIGH_NIBBLE_CHAR(second));
		LcdWriteChar(5, BCD_LOW_NIBBLE_CHAR(second));
	}
	LcdWriteChar(6, '\0');
	LcdColon(TRUE);
}

/** Sets the clock module in time editing state. */
void ClockTimeEditInit(void)
{
	clockTimeState = CLOCK_EDIT_HOUR;
	RtcStop();
	ClockTimeUpdate();
}

/** Edits the time. Switches between editing states and informs the Rtc module
	for the user's modifications to the time.
	@param key The code of the key pressed by the user.
*/
void ClockTimeEdit(char key)
{
	if (key == KEY_NULL)
	{
		return;
	}
	else if (key == KEY_ENTER)
	{
		clockTimeState = CLOCK_SHOW;
		RtcStart();
	}
	else if (clockTimeState == CLOCK_EDIT_HOUR)
	{
		uint8_t hour = RtcGetHour();

		switch(key)
		{
			case KEY_UP:
				hour++;
				if (hour == 24)
					hour = 0;
				RtcSetHour(hour);
				break;
			case KEY_DOWN:
				if (hour > 0)
					hour--;
				else
					hour = 23;
				RtcSetHour(hour);
				break;
			case KEY_LEFT:
				clockTimeState = CLOCK_EDIT_SECOND;
				break;
			case KEY_RIGHT:
				clockTimeState = CLOCK_EDIT_MINUTE;
				break;
		}
	}
	else if (clockTimeState == CLOCK_EDIT_MINUTE)
	{
		uint8_t minute = RtcGetMinute();

		switch(key)
		{
			case KEY_UP:
				minute++;
				if (minute == 60)
					minute = 0;
				RtcSetMinute(minute);
				break;
			case KEY_DOWN:
				if (minute > 0)
					minute--;
				else
					minute = 59;
				RtcSetMinute(minute);
				break;
			case KEY_LEFT:
				clockTimeState = CLOCK_EDIT_HOUR;
				break;
			case KEY_RIGHT:
				clockTimeState = CLOCK_EDIT_SECOND;
				break;
		}
	}
	else if (clockTimeState == CLOCK_EDIT_SECOND)
	{
		uint8_t second = RtcGetSecond();

		switch(key)
		{
			case KEY_UP:
				second++;
				if (second == 60)
					second = 0;
				RtcSetSecond(second);
				break;
			case KEY_DOWN:
				if (second > 0)
					second--;
				else
					second = 59;
				RtcSetSecond(second);
				break;
			case KEY_LEFT:
				clockTimeState = CLOCK_EDIT_MINUTE;
				break;
			case KEY_RIGHT:
				clockTimeState = CLOCK_EDIT_HOUR;
				break;
		}
	}
	ClockTimeUpdate();
}

/** Gets the time editing state.
	@result The time editing state */
ClockTimeState ClockGetTimeState(void)
{
	return clockTimeState;
}

/** Displays in the LCD screen the current time format (12H or 24H).
*/
void ClockShowTimeFormat(void)
{
	if (clockTimeFormat == CLOCK_FORMAT_12H)
		LcdWriteString_F(PSTR("  12H"));
	else
		LcdWriteString_F(PSTR("  24H"));
}

/** Gets the current time format.
	@result One of CLOCK_FORMAT_12H and CLOCK_FORMAT_24H
*/
uint8_t ClockGetTimeFormat(void)
{
	return clockTimeFormat;
}

/** Toggles between the two available time formats (12H or 24H).*/
void ClockToggleTimeFormat(void)
{
	if (clockTimeFormat == CLOCK_FORMAT_12H)
		clockTimeFormat = CLOCK_FORMAT_24H;
	else
		clockTimeFormat = CLOCK_FORMAT_12H;
}

/*******************************************************************************
* Date Functions
*******************************************************************************/


/** Displays the date. If the date isn't being edited, the date is taken from
	the Rtc module. If the date is being edited by the user, the appropriate
	characters are flashing. */
void ClockDateUpdate(void)
{
	uint8_t month, day, year;
	uint8_t monthPos, dayPos, yearPos, startPos;

	month = CharToBCD2(RtcGetMonth() + 1);
	day = CharToBCD2(RtcGetDay() + 1);
	year = CharToBCD2(RtcGetYear());

	startPos = GET_DATE_FORMAT_START_POS(clockDateFormat);

	dayPos = pgm_read_byte(dateFormatPosTable_F + startPos);
	monthPos = pgm_read_byte(dateFormatPosTable_F + startPos + 1);
	yearPos = pgm_read_byte(dateFormatPosTable_F + startPos + 2);

	dayPos = dayPos << 1; 		// *2
	monthPos = monthPos << 1; 	// *2
	yearPos = yearPos << 1; 	// *2

	if (clockDateState == DATE_EDIT_DAY)
	{
		LcdWriteChar(dayPos, BCD_HIGH_NIBBLE_CHAR(day) | 0x80);
		LcdWriteChar(dayPos + 1, BCD_LOW_NIBBLE_CHAR(day) | 0x80);
	}
	else
	{
		LcdWriteChar(dayPos, BCD_HIGH_NIBBLE_CHAR(day));
		LcdWriteChar(dayPos + 1, BCD_LOW_NIBBLE_CHAR(day));
	}
	if (clockDateState == DATE_EDIT_MONTH)
	{
		LcdWriteChar(monthPos, BCD_HIGH_NIBBLE_CHAR(month) | 0x80);
		LcdWriteChar(monthPos + 1, BCD_LOW_NIBBLE_CHAR(month) | 0x80);
	}
	else
	{
		LcdWriteChar(monthPos, BCD_HIGH_NIBBLE_CHAR(month));
		LcdWriteChar(monthPos + 1, BCD_LOW_NIBBLE_CHAR(month));
	}
	if (clockDateState == DATE_EDIT_YEAR)
	{
		LcdWriteChar(yearPos, BCD_HIGH_NIBBLE_CHAR(year) | 0x80);
		LcdWriteChar(yearPos + 1, BCD_LOW_NIBBLE_CHAR(year) | 0x80);
	}
	else
	{
		LcdWriteChar(yearPos, BCD_HIGH_NIBBLE_CHAR(year));
		LcdWriteChar(yearPos + 1, BCD_LOW_NIBBLE_CHAR(year));
	}
	LcdWriteChar(6, '\0');
	LcdColon(TRUE);
}

/** Sets the clock module in date editing state. */
void ClockDateEditInit(void)
{
	uint8_t	startPos;

	startPos = GET_DATE_FORMAT_START_POS(clockDateFormat);
	clockDateState = pgm_read_byte(dateFormatPosTable_F + startPos);

	RtcStop();
	ClockDateUpdate();
}

/** Edits the date. Switches between editing states and informs the Rtc module
	for the user's modifications to the date.
	@param key The code of the key pressed by the user.
*/
void ClockDateEdit(char key)
{
	if (key == KEY_NULL)
	{
		return;
	}
	else if (key == KEY_ENTER)
	{
		clockDateState = DATE_SHOW;
		RtcStart();
	}
	else if (key == KEY_LEFT || key == KEY_RIGHT)
	{
		uint8_t	startPos, pos;
		uint8_t state = (uint8_t) clockDateState;

		startPos = GET_DATE_FORMAT_START_POS(clockDateFormat);

		if (pgm_read_byte(dateFormatPosTable_F + startPos) == state)
			pos = 0;
		else if (pgm_read_byte(dateFormatPosTable_F + startPos + 1) == state)
			pos = 1;
		else
			pos = 2;

		if (key == KEY_LEFT)
		{
			if (pos == 0)
				pos = 2;
			else
				pos--;
		}
		else
		{
			if (pos == 2)
				pos = 0;
			else
				pos++;
		}

		startPos = GET_DATE_FORMAT_START_POS(clockDateFormat);
		clockDateState = pgm_read_byte(dateFormatPosTable_F + startPos + pos);
	}
	else if (clockDateState == DATE_EDIT_DAY)
	{
		uint8_t day = RtcGetDay();
		uint8_t dayMax = monthLength[RtcGetMonth()];

		if ((RtcGetMonth() == 1) && RtcIsLeapYear())
			dayMax = 29;

		if (key == KEY_UP)
		{
			if (day < dayMax - 1)
				day++;
			else
				day = 0;
		}
		else if (key == KEY_DOWN)
		{
			if (day > 0)
				day--;
			else
				day = dayMax-1;
		}
		RtcSetDay(day);
	}
	else if (clockDateState == DATE_EDIT_MONTH)
	{
		uint8_t month = RtcGetMonth();
		uint8_t dayMax;

		if (key == KEY_UP)
		{
			month++;
			if (month == 12)
				month = 0;
		}
		else if (key == KEY_DOWN)
		{
			if (month > 0)
				month--;
			else
				month = 11;
		}
		RtcSetMonth(month);
		dayMax = monthLength[month];
		if ((month == 1) && RtcIsLeapYear())
			dayMax = 29;
		if (RtcGetDay() >= dayMax)
			RtcSetDay(0);
	}
	else if (clockDateState == DATE_EDIT_YEAR)
	{
		uint8_t year = RtcGetYear();

		if (key == KEY_UP)
		{
			year++;
			if (year == 99)
				year = 0;
			RtcSetYear(year);
		}
		else if (key == KEY_DOWN)
		{
			if (year > 0)
				year--;
			else
				year = 99;
		}
		RtcSetYear(year);
	}
	ClockDateUpdate();
}

/** Gets the date editing state.
	@result The date editing state */
ClockDateState ClockGetDateState(void)
{
	return clockDateState;
}

/** Displays in the LCD screen the current date format (European, American
	or Canadian).
*/
void ClockShowDateFormat(void)
{
	if (clockDateFormat == DATE_EUROPE_FORMAT)
		LcdWriteString_F(PSTR("DDMMYY"));
	else if (clockDateFormat == DATE_AMERICAN_FORMAT)
		LcdWriteString_F(PSTR("MMDDYY"));
	else
		LcdWriteString_F(PSTR("YYMMDD"));
}

/** Changes the date format, based on the next parameter and the previous
	value of the #clockDateFormat.
	@param next If TRUE, moves to the next date format, otherwise to the
		previous one.
*/
void ClockChangeDateFormat(BOOL next)
{
	if (next && clockDateFormat < DATE_CANADIAN_FORMAT)
		clockDateFormat++;
	else if (next)
		clockDateFormat = DATE_EUROPE_FORMAT;
	else if (clockDateFormat > DATE_EUROPE_FORMAT)
		clockDateFormat--;
	else
		clockDateFormat = DATE_CANADIAN_FORMAT;
}
