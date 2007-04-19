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

/** @file rtc.c RTC driver. Counts time using Timer2. It keeps both time and
	date information.
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "c_ext.h"
#include "rtc.h"

/*******************************************************************************
* Static Variables
********************************************************************************/

/** Real Time Clock second (0..59) */
static volatile uint8_t rtcSecond;

/** Real Time Clock minute (0..59) */
static volatile uint8_t rtcMinute;

/** Real Time Clock hour (0.23) */
static volatile uint8_t rtcHour;

/** Real Time Clock day. It starts from 0, which corresponds to the first day of
	the month */
static volatile uint8_t rtcDay;

/** Real Time Clock month. It takes values from the range 0..11 (January to
	December) */
static volatile uint8_t rtcMonth;

/** Real Time Clock year. It starts counting from 6, which corresponds to
	2006 */
static volatile uint8_t rtcYear;

/** Flag that indicates that the values of the RTC have changed. It is set to
	0xFF when an Rtc interrupt occurs. #RtcNewEvent function process this
	variable in order to inform listeners. */
static volatile uint8_t rtcUpdateRequired;

/** Bit mask of the available Rtc listeners. If bit n is set to 1, then a
	listener with Id n is registered for receiving Rtc events. */
static uint8_t rtcListenersBitMask;

/** Lookup table holding the length of each month. */
const char monthLength[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*******************************************************************************
* MACROS
********************************************************************************/

#define RTC_IS_LEAP_YEAR()	(rtcYear & 0x03) == 0

/*******************************************************************************
* Functions
********************************************************************************/

/** Initializes the RTC driver. It starts the Timer/Counter2 in asynchronous
operation using the 32.768kHz crystal and initializes the RTC variables.
*/
void RtcInit(void)
{
	// Initial time and date setting
	rtcSecond = 0;
	rtcMinute = 0;
	rtcHour = 12;

	rtcDay = 0;		// Days from 0 to monthMax - 1
	rtcMonth = 8;	// Months from 0 to 11
	rtcYear = 6;

	rtcUpdateRequired = 0;
	rtcListenersBitMask = 0;

	// Disable OCIE2A and TOIE2
	cbiBF(TIMSK2, TOIE2);

	// Select asynchronous operation of Timer2
	ASSR = (1<<AS2);

	TCNT2 = 0;

	// Select precaler: 32.768 kHz / 128 = 1 sec between each overflow
	TCCR2A |= (1<<CS22) | (1<<CS20);

	// Wait for TCN2UB and TCR2UB to be cleared
	while((ASSR & 0x01) | (ASSR & 0x04));

	// Clear interrupt-flags
	TIFR2 = 0xFF;

	// Enable Timer2 overflow interrupt
	sbiBF(TIMSK2, TOIE2);
}

/** Starts the RTC timer */
void RtcStart(void)
{
	sbiBF(TIMSK2, TOIE2);
}

/** Stops the RTC timer */
void RtcStop(void)
{
	cbiBF(TIMSK2, TOIE2);
}

/** Setter of the #rtcSecond variable.
	@param second Second of the real time clock
*/
void RtcSetSecond(uint8_t second)
{
	if (rtcSecond < 60)
		rtcSecond = second;
}

/** Getter of the #rtcSecond variable.
	@result The second of the real time clock
*/
uint8_t RtcGetSecond(void)
{
	return rtcSecond;
}

/** Setter of the #rtcMinute variable.
	@param minute Minute of the real time clock
*/
void RtcSetMinute(uint8_t minute)
{
	if (rtcMinute < 60)
		rtcMinute = minute;
}

/** Getter of the #rtcMinute variable.
	@result The minute of the real time clock
*/
uint8_t RtcGetMinute(void)
{
	return rtcMinute;
}

/** Setter of the #rtcHour variable.
	@param hour Hour of the real time clock
*/
void RtcSetHour(uint8_t hour)
{
	if (rtcHour < 60)
		rtcHour = hour;
}

/** Getter of the #rtcHour variable.
	@result The hour of the real time clock
*/
uint8_t RtcGetHour(void)
{
	return rtcHour;
}

/** Setter of the #rtcDay variable.
	@param day Day of the real time clock
*/
void RtcSetDay(uint8_t day)
{
	if (rtcDay < 31)
		rtcDay = day;
}

/** Getter of the #rtcDay variable.
	@result The day of the real time clock
*/
uint8_t RtcGetDay(void)
{
	return rtcDay;
}

/** Setter of the #rtcMonth variable.
	@param month Month of the real time clock
*/
void RtcSetMonth(uint8_t month)
{
	if (rtcMonth < 12)
		rtcMonth = month;
}

/** Getter of the #rtcMonth variable.
	@result The month of the real time clock
*/
uint8_t RtcGetMonth(void)
{
	return rtcMonth;
}

/** Setter of the #rtcYear variable.
	@param year Year of the real time clock
*/
void RtcSetYear(uint8_t year)
{
	rtcYear = year;
}

/** Getter of the #rtcYear variable.
	@result The year of the real time clock
*/
uint8_t RtcGetYear(void)
{
	return rtcYear;
}

/** Checks whether current year is a leap year.
	@result True if current year is a leap year.
*/
BOOL RtcIsLeapYear(void)
{
	return RTC_IS_LEAP_YEAR();
}

/** @def RTC_INVALID_LISTENER_ID Invalid id for an RTC listener
*/
#define RTC_INVALID_LISTENER_ID		0xFF

/** Registers a listener to Rtc events. A listener can be informed about
	Rtc events (Rtc interrupts) by calling the #RtcNewEvent function with
	its id as a parameter. Up to 8 listeners are allowed. A listener can
	unregister itself with the #RtcUnregisterListener function.
	@result The id of the registered listener. It will return
			#RTC_INVALID_LISTENER_ID if a listener can't be registered.
*/
uint8_t RtcRegisterListener(void)
{
	uint8_t index;

	for (index = 0; index < 8; index++)
	{
		if ((rtcListenersBitMask & (1<<index)) == 0)
		{
			rtcListenersBitMask |= (1<<index);

			// Disable Timer2 overflow interrupt
			cbiBF(TIMSK2, TOIE2);

			rtcUpdateRequired &= ~(1<<index);

			// Enable Timer2 overflow interrupt
			sbiBF(TIMSK2, TOIE2);

			return index;
		}
	}

	return RTC_INVALID_LISTENER_ID;
}

/** Returns TRUE if a new Rtc interrupt has occured. In case of a new event
	the function will return TRUE only once. The internal state will be
	immediately cleared after returning TRUE.
	@param listenerId The id of the listener querying for new events
	@result TRUE if a new event has occured.
*/
BOOL RtcNewEvent(uint8_t listenerId)
{
	if (listenerId >= 8)
		return FALSE;

	if (rtcUpdateRequired & (1<<listenerId))
	{
		rtcUpdateRequired &= ~(1<<listenerId);
		return TRUE;
	}
	return FALSE;
}

/** Unregisters an Rtc listener.
	@param listenerId The id of the listener
*/
void RtcUnregisterListener(uint8_t listenerId)
{
	if (listenerId < 8)
	{
		rtcListenersBitMask &= ~(1<<listenerId);
	}
}

/** @fn ISR(TIMER2_OVF_vect)
	RTC Interrupt Service Routine. It is called everu
	one second. It will be called even if the controller is in power-save mode.
*/
ISR(TIMER2_OVF_vect)
{
	char leapMonth;

	rtcSecond++;
	rtcUpdateRequired = 0xFF;

	if (rtcSecond == 60)
	{
		rtcSecond = 0;
		rtcMinute++;

		if (rtcMinute > 59)
		{
			rtcMinute = 0;
			rtcHour++;

			if (rtcHour > 23)
			{
				rtcHour = 0;
				rtcDay++;

				// Check for leap year if month == February
				leapMonth = 0;
				if (rtcMonth == 2)
				{
					leapMonth = RTC_IS_LEAP_YEAR() ? 1 : 0;
				}

				// Now, we can check for month length
				if (rtcDay >= (monthLength[rtcMonth] + leapMonth))
				{
					rtcDay = 0;
					rtcMonth++;

					if (rtcMonth > 11)
					{
						rtcMonth = 0;
						rtcYear++;
					}
				}
			}
		}
	}
}
