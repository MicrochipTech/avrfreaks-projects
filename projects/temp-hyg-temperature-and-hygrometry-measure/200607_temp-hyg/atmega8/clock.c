#include <inttypes.h>

#include "clock.h"
#include "event.h"
#include "eeprom.h"

clock_t clock;

void clockUpdate(void)
{
	clock.second ++;
	if (clock.second < 60) return;
	clock.second = 0;
	clock.minute ++;
	eventAddEvent(EVENT_TIME_MIN);
	if (clock.minute < 60) return;
	clock.minute = 0;
	clock.hour ++;
	eventAddEvent(EVENT_TIME_HOUR);
	if (clock.hour < 24) return;
	clock.hour = 0;
	clock.day ++;
	eventAddEvent(EVENT_TIME_DAY);
	if (clock.day < 29) return;
	if (clock.month == 2)			// in February
	{
		// allow 29 days on bissextile years
		if ((clock.day == 29) && (clock.year % 4 == 0)) return;
	}
	else if ((clock.month == 4) || (clock.month == 6) || (clock.month == 9) || (clock.month == 11))
	{
		// allow 30 days
		if (clock.day < 31) return;
	}
	else
	{
		// allow 31 days
		if (clock.day < 32) return;
	}
	clock.day = 1;
	clock.month ++;
	eventAddEvent(EVENT_TIME_MONTH);
	if (clock.month < 13) return;
	clock.month = 1;
	clock.year ++; 
	eventAddEvent(EVENT_TIME_YEAR);
}

void clockStore(void)
{
	switch (eventGetCurrentEvent())
	{
		case EVENT_TIME_DAY:
			eepromWriteByte(CLOCK_RECORD+2, clock.day);
			break;
		case EVENT_TIME_MONTH:
			eepromWriteByte(CLOCK_RECORD+1, clock.month);
			break;
		case EVENT_TIME_YEAR:
			eepromWriteByte(CLOCK_RECORD, clock.year);
			break;
		default:
			break;
	}
}

void clockInit(void)
{
	eventRegisterHandler(clockUpdate, EVENT_TIME_SEC);
	eventRegisterHandler(clockStore, EVENT_TIME);
	eepromReadBuffer(CLOCK_RECORD, (uint8_t *)&clock, 3);
	// if no year is stored, store a default year
	if (clock.year == 0xFF) clockSet(CLOCK_FIRST_YEAR, 1, 1, 0, 0, 0);
}

void clockDateValidate(uint8_t *year, uint8_t *month, uint8_t *day)
{
	if (*year > 99) *year = 99;
	if (*month < 1) *month = 1;
	if (*month > 12) *month = 12;
	if (*day < 1) *day = 1;
	if (*day < 29) return;
	if (*month == 2)			// in February
	{
		// allow 29 days on bissextile years
		if ((*day == 29) && (*year % 4 == 0)) return;
		*day = 28;
	}
	else if ((*month == 4) || (*month == 6) || (*month == 9) || (*month == 11)) 
	{
		// allow 30 days
		if (*day < 31) return;
		*day = 30;
	}
	else
	{
		// allow 31 days
		if (*day < 32) return;
		*day = 31;
	}
}
 
void clockSet(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
	clock.year = year;
	clock.month = month;
	clock.day = day;
	clock.hour = hour;
	clock.minute = min;
	clock.second = sec;
	if (clock.hour > 23) clock.hour = 0;
	if (clock.minute > 59) clock.minute = 0;
	if (clock.second > 59) clock.second = 0;
	if (clock.year == 0xFF) clock.year = 0;
	clockDateValidate(&(clock.year), &(clock.month), &(clock.day));
	eepromWriteBuffer(CLOCK_RECORD, (uint8_t *)&clock, 4);
}

clock_t *clockGet(void)
{
	return &clock;
}

void clockGetTimeAsString(char *s, uint8_t withSeconds)
{
	uint8_t temp;
	
	temp = clock.hour/10;
	if (temp) s[0] = temp + '0';
	else s[0] = ' ';
	s[1] = clock.hour%10 + '0';
	s[2] = ':';
	s[3] = clock.minute/10 + '0';
	s[4] = clock.minute%10 + '0';
	if (withSeconds)
	{
		s[5] = ':';
		s[6] = clock.second/10 + '0';
		s[7] = clock.second%10 + '0';
	}	
}

void clockGetDateAsString(char *s)
{
	uint8_t temp;
	
	temp = clock.day/10;
	if (temp) s[0] = temp + '0';
	else s[0] = ' ';
	s[1] = clock.day%10 + '0';
	s[2] = '/';
	s[3] = clock.month/10 + '0';
	s[4] = clock.month%10 + '0';
	s[5] = '/';
	s[6] = '2';
	s[7] = '0';
	s[8] = clock.year/10 + '0';
	s[9] = clock.year%10 + '0';
}
