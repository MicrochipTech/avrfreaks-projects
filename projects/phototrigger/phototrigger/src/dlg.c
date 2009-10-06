/*
 * dlg.c
 *
 *  Created on: 2009-09-02
 *      Author: liku
 *
 *      This file contains, mostly, so called dialogs to interact with user.
 *      Mind that many functions are not generic and need to be reviewed when
 *      reused.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <util/delay.h>

#include <avr/pgmspace.h>

#include "enc.h"
#include "lcd.h"
#include "dlg.h"
#include "rtc.h"


/*
 * Creates "GUI" for entering time (hh:mm:ss) and sets "t" accordingly
 * returns true on OK, false on cancel ("Back").
 * limit24h - limit hours to 24 h
 * limit1s  - set at least 1 second
 * Actually contains implementation for limits, but they're not in argument
 * list to ease use of this function - just add mint and maxt.
 */
bool dlgSetTime(tm* t, bool limit24h, bool atleast1s)
{
#define SETTIME_CANCEL 0 /* mode must reflect selection index */
#define SETTIME_SETH   1
#define SETTIME_SETM   2
#define SETTIME_SETS   3
#define SETTIME_OK     4
#define SETTIME_SEL    10

	lcd_clr();

	int8_t   mode   = SETTIME_SEL;       // current mode (what the user currently does)
	int8_t   cursel = 0;                 // current selection (what will happen if user presses switch)
	tm       tmptm  = *t;                // temporary holder for new time
	int32_t  ti     = rtcTm2Int(&tmptm); // convert to int and operate on int value
	tm       mint   = {.tm_hour = 0,  .tm_min = 0,  .tm_sec = 0  };
	tm       maxt   = {.tm_hour = 99, .tm_min = 59, .tm_sec = 59 };
	int16_t  diff   = 0;                 // difference value to add/substract
	char     sel;                        // selection character
	key_t    k;

	ti = rtcTm2Int(t);

	// do we have some limitations?
	if(limit24h)
		maxt.tm_hour = 23;
	if(atleast1s)
		mint.tm_sec  = 1;

	int32_t minti = rtcTm2Int(&mint);
	int32_t maxti = rtcTm2Int(&maxt);

	while(1)
	{
		rtcInt2Tm(ti, &tmptm); // convert to tm struct to display variables
		lcd_setxy(0, 0);
		printf_P(PSTR("Back %02d:%02d:%02d OK"), tmptm.tm_hour, tmptm.tm_min, tmptm.tm_sec);
		if(mode != SETTIME_SEL)
			sel = LCDCHARARRVERTEMPTY;
		else
			sel = LCDCHARARRVERT;
		dlgPrintSel("XXXX XX XX XX XX", cursel, sel);

		switch(mode)
		{
		case SETTIME_SEL:
			k = encGetKey(KEYWAITFORKEY);
			if(k == KEYLEFT)
				cursel = dlgIncVar(cursel, -1, 0, 4, false);
			else if(k == KEYRIGHT)
				cursel = dlgIncVar(cursel, 1, 0, 4, false);
			else if(k == KEYSWITCH)
				mode = cursel; // mode code must reflect selection
			break;

		case SETTIME_CANCEL:
			return false;
			break;

		case SETTIME_SETH:
			diff = 3600l;
		case SETTIME_SETM:
			if(!diff) // not yet set
				diff = 60l;
		case SETTIME_SETS:
			if(!diff) // not yet set
				diff = 1l;
			k = encGetKey(KEYWAITFORKEY);
			if(k == KEYLEFT)
			{
				diff = -diff;
				ti = dlgIncVar(ti, diff, minti, maxti, true);
			}
			else if(k == KEYRIGHT)
				ti = dlgIncVar(ti, diff, minti, maxti, true);
			else if(k == KEYSWITCH)
				mode = SETTIME_SEL; // mode code must reflect selection

			diff = 0l;
			break;

		case SETTIME_OK:
			*t = tmptm;
			return true;
			break;
		}
	} // <-- while loop
}

/*
 * Helper function - prints "mark" character under words
 * This function is not universal, assumes the string will begin at 0, 0
 * Space is not considered a group
 * Non-blocking
 */
void dlgPrintSel(char *mask, uint8_t idx, char mark)
{
	// 1. Each group is a same character group ("XXAAACC" - 3 groups)
	uint8_t len    = strlen(mask);
	char    lastch = *mask;
	uint8_t gidx   = 0;

	lcd_setxy(0, 1);
	for(uint8_t i = 0; i<len; i++)
	{
		if(*(mask+i) == ' ')
			putchar(' ');
		else
		{
			if(*(mask+i) != lastch)
				gidx++;
			if(idx == gidx)
				putchar(mark);
			else
				putchar(' ');
		}

		lastch = *(mask+i);
	}
}

/*
 * Display yes/no dialog (yes - true, no - false)
 */
bool dlgYesNo(char* label)
{
	uint8_t i = 0;

	lcd_clr();
	while(1)
	{
		printf_P(PSTR("No %-9s Yes"), label);
		dlgPrintSel("XX           XXX", i, '^');

		key_t k = encGetKey(KEYWAITFORKEY);
		if(k == KEYLEFT)
			i = 0;
		else if(k == KEYRIGHT)
			i = 1;
		else if(k == KEYSWITCH)
		{
			if(i)
				return true;
			else
				return false;
		}
	}
}


/*
 * Print nice dialog and set a variable
 * cancel - returns false, OK - returns true
 * callback - call this function evrytime value is changed
 * void callback(int32_t v);
 */
bool dlgSetVar(int32_t* val, int32_t min, int32_t max, void (*callback)(int32_t val), char postfix)
{
#define SETVAR_CANCEL   0 /* mode must reflect selection index */
#define SETVAR_SETHHALF 1
#define SETVAR_SETLHALF 2
#define SETVAR_OK       3
#define SETVAR_SEL      10

	lcd_clr();

	uint8_t mode = SETVAR_SEL; // current mode (what the user currently does)
	uint8_t cursel = 0;        // current selection (what will happen if user presses switch)
	int16_t h, l;              // low and high value to set
	int32_t tmpval = *val;     // temporary container for our value
	int32_t diff = 0;          // difference value
	char    sel;               // selection character
	key_t   k;

	while(1)
	{
		l = tmpval % 100l;
		l = l < 0 ? -l : l; // this value must always be (+)
		h = tmpval / 100l;

		lcd_setxy(0, 0);
		if(h)
			printf_P(PSTR("Back   %3d%02d%c OK"), h, l, postfix);
		else
			printf_P(PSTR("Back      %2d%c OK"), l, postfix);

		// print selection indicator
		if(mode != SETVAR_SEL)
			sel = LCDCHARARRVERTEMPTY;
		else
			sel = LCDCHARARRVERT;
		dlgPrintSel("XXXX   XXXYY  XX", cursel, sel);

		switch(mode)
		{
		case SETVAR_SEL:
			k = encGetKey(KEYWAITFORKEY);
			if(k == KEYLEFT)
				cursel = dlgIncVar(cursel, -1, 0, 3, false);
			else if(k == KEYRIGHT)
				cursel = dlgIncVar(cursel, 1, 0, 3, false);
			else if(k == KEYSWITCH)
				mode = cursel;
			break;

		case SETVAR_CANCEL:
			if(callback)
				callback(*val);
			return false;
			break;

		case SETVAR_SETHHALF:
			diff = 100l;
		case SETVAR_SETLHALF:
			if(!diff)
				diff = 1l;
			k = encGetKey(KEYWAITFORKEY);
			if(k == KEYLEFT)
			{
				diff = -diff;
				tmpval = dlgIncVar(tmpval, diff, min, max, true);
			}
			else if(k == KEYRIGHT)
				tmpval = dlgIncVar(tmpval, diff, min, max, true);
			else if(k == KEYSWITCH)
				mode = SETVAR_SEL;

			diff = 0;

			if(callback) // call callback function if one is set
				callback(tmpval);
			break;

		case SETVAR_OK:
			*val = tmpval;
			return true;
			break;
		} // <-- the while loop
	}
}

/*
 * Shows info dialog
 */
void dlgInfo_P(PGM_P info)
{
	lcd_clr();
	printf_P(info);
	_delay_ms(1000);
	encClrKeyBuf(); // clear key buffer
}

/*
 * increase variable with limits (and round robin option)
 */
int32_t dlgIncVar(int32_t val, int32_t incval, int32_t min, int32_t max, bool roundrobin)
{
	if((val + incval) > max)
	{
		if(roundrobin)
		{
			if(val < max)
				val = max;
			else
				val = min;
		}
		else
			val = max;
	}
	else if ((val + incval) < min)
	{
		if(roundrobin)
		{
			if(val > min)
				val = min;
			else
				val = max;
		}
		else
			val = min;
	}
	else
		val += incval;

	return val;
}

/*
 * Displays how much time has left (non blocking)
 * t - start time
 */
void dlgPrintTimeLeft(tm* t)
{
	tm currtm;
	rtcGetSysTime(&currtm);

	int32_t t0 = rtcTm2Int(t); // will hold also timediff
	int32_t t1 = rtcTm2Int(&currtm);
	if(t0 < t1)
		t0 = 24l * 3600l + t0 - t1; // time difference (add one day /in seconds/)
	else
		t0 = t0 - t1; // time difference

	rtcInt2Tm(t0, &currtm); // reuse currtm

	lcd_setxy(0, 0);
	printf_P(PSTR("Start:  %02d:%02d:%02d\nLeft:   %02d:%02d:%02d"),
			t->tm_hour,     t->tm_min,     t->tm_sec,
			currtm.tm_hour, currtm.tm_min, currtm.tm_sec);
}

/*
 * print information about bulb mode (non blocking)
 */
void dlgPrintBulb(tm* t)
{
	lcd_setxy(0, 0);
	printf_P(PSTR("Bulb in progress\nLeft:   %02d:%02d:%02d"), t->tm_hour, t->tm_min, t->tm_sec);
}

/*
 * print information about photo count and time left for next photo (non blocking)
 */
void dlgPrintCountAndTime(uint16_t count, uint16_t total, tm* t)
{
	lcd_setxy(0, 0);
	printf_P(PSTR("No:  %05d/%05d\nNext:   %02d:%02d:%02d"), count, total, t->tm_hour, t->tm_min, t->tm_sec);
}
