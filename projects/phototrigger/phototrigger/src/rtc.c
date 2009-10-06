/*
 * rtc.c
 *
 *  Created on: 2009-08-30
 *      Author: liku
 *
 *      This file contains implementations of RTC and timer routines.
 *      Also RTC ISR (Interrupt Service Routine) contains code for
 *      detecting power-on event and dimming LCD backlight when
 *      nothing is done with the device.
 */

#include <stdbool.h>

#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include <util/atomic.h>
#include <util/delay.h>

#include "rtc.h"
#include "lcd.h"
#include "hw.h"
#include "eeprom.h"

static volatile uint32_t stimers[RTCSTATTIMERCOUNT];     // static timers
static volatile tm       rtc = {12, 0, 0};               // system clock

static volatile uint8_t inacttimer  = RTCINACTIVITYTIME; // inactivity timer (used to dim the backlight)
static volatile uint16_t oldblval   = 0;                 // backlight value before dimming
static volatile bool    lcdbldimmed = false;             // backlight already set

static volatile uint8_t pwrontimer  = RTCPWRONTIME;      // power on timer (used to power on)
static volatile bool    pwronflag   = false;             // we should power on!

/*
 * RTC Interrupt Service Routine
 */
ISR(TIMER2_OVF_vect)
{
	// handle time
	if(++rtc.tm_sec == 60)
	{
		rtc.tm_sec = 0;
		if(++rtc.tm_min == 60)
		{
			rtc.tm_min = 0;
			if(++rtc.tm_hour == 24)
				rtc.tm_hour = 0;
		}
	}

	// handle timers (just decrease until they're equal to zero)
	for(uint8_t i = 0; i < RTCSTATTIMERCOUNT; i++)
	{
		if(stimers[i])
			stimers[i]--;
	}

	// power on timer (works only in power off state)
	if(!hwIsPwrOn())
	{
		if(ENCPIN & _BV(ENCSW))
		{
			pwrontimer = RTCPWRONTIME;
			pwronflag = false;
		}
		else
		{
			if(!(--pwrontimer))
				pwronflag = true;
		}
	}

	// inactivity timer - dim light when no keys are pressed (and power is on)
	if(hwIsPwrOn())
	{
		inacttimer--;
		if((!inacttimer) && (!lcdbldimmed))
		{
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				oldblval = OCR1A;
			}
			lcd_setbacklight(LCDLOWBACKLIGHT);
			lcdbldimmed = true;
		}
	}
}

/*
 * Get current system time
 */
void rtcGetSysTime(tm* t)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		*t = rtc;
	}
}

/*
 * Set current system time (note: no boundaries check)
 */
void rtcSetSysTime(tm* nt)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		rtc = *nt;
	}

	TCNT2 = 0; // reset current second
}

/*
 * Convert time structure to integer (uint32_t)
 */
uint32_t rtcTm2Int(tm* t)
{
	return
		(uint32_t) t->tm_hour * 3600UL +
		(uint32_t) t->tm_min  * 60UL +
		(uint32_t) t->tm_sec;
}

/*
 * Convert integer (uint32_t) to time structure
 */
void rtcInt2Tm(uint32_t ti, tm* t)
{
	t->tm_hour = ti / 3600UL;
	t->tm_min  = (ti - (t->tm_hour * 3600UL)) / 60UL;
	t->tm_sec  = ti % 60UL;
}

/*
 * Get static timer (warning: returns zero if timer idx out of bounds)
 */
uint32_t rtcGetStatTimer(uint8_t i)
{
	uint32_t retval = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(i < RTCSTATTIMERCOUNT)
			retval = stimers[i];
	}

	return retval;
}

/*
 * Set static timer (does nothing when timer idx is out of bounds)
 */
void rtcSetStatTimer(uint8_t i, uint32_t val)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(i < RTCSTATTIMERCOUNT)
			stimers[i] = val;
	}
}

/*
 * Returns at full second (or not at all if RTC if off)
 */
inline void rtcSyncToSec(void)
{
	while(TCNT2);
}

/*
 * Returns flag indicating that user want to power up the device
 */
inline bool rtcGetPwrOnFlag(void)
{
	return pwronflag;
}

/*
 * After some time inacttimer = 0 and lcd backlight is dimmed, to
 * prevent that this function must be called while user interacts
 * with device
 */
inline void rtcUpdInactTimer(void)
{
	inacttimer = RTCINACTIVITYTIME;

	if(lcdbldimmed) // bring back old backlight value
		lcd_setbacklight(oldblval);

	lcdbldimmed = false;
}

/*
 * Wait for t time in ms (this function is not 100% accurate)
 */
void rtcWaitMs(uint16_t t)
{
	while(t--)
		_delay_ms(1);
}

/*
 * Decrease 1s from t
 */
void rtcDec1sFromTm(tm* t)
{
	uint32_t ti = rtcTm2Int(t);
	ti--;
	rtcInt2Tm(ti, t);
}
