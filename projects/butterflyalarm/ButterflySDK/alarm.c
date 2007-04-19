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

/** @file alarm.c Alarm Module. Provides functions for using an alarm clock.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include "c_ext.h"
#include "button.h"
#include "lcd.h"
//#include "clock.h"
#include "bcd.h"
#include "rtc.h"
#include "sound.h"
#include "pmSongs.h"
#include "dfSongs.h"

/*******************************************************************************
* Type Definitions
*******************************************************************************/

/** Alarm structure */
typedef struct
{
	/** Alarm's hour setting */
	uint8_t hour;
	/** Alarm's minute setting */
	uint8_t minute;
	/** Id of song associated with the alarm */
	uint8_t song;

	/** TRUE if the alarm is enabled */
	BOOL enabled;
} Alarm;

/** Alarm states enumeration */
typedef enum
{
	ALARM_DISPLAY_LABEL = 0,
	ALARM_DISPLAY_TIME,
	ALARM_EDIT_HOUR,
	ALARM_EDIT_MINUTE,
	ALARM_SELECT_SONG
} AlarmState;

/*******************************************************************************
* Static Variables
*******************************************************************************/

/** @def ALARM_COUNT Number of alarms */
#define ALARM_COUNT	2

/** Table of alarm objects. Currently the ButterflySDK supports two alarm clocks */
static Alarm alarm[ALARM_COUNT];

/** The number of the selected alarm */
static uint8_t alarmNumber;

/** The current state of the alarm module */
static AlarmState alarmState;

/** Id of an rtc listener */
static uint8_t rtcListener;

static BOOL isAlarmOn;

/*******************************************************************************
* Forward declarations
*******************************************************************************/

int8_t AlarmMain(char key, BOOL init, uint8_t number);

/*******************************************************************************
* External Variables
*******************************************************************************/

// table used when displaying 12H clock, taken from RTC module
//extern uint8_t clock12HTable[];

/*******************************************************************************
* Functions
*******************************************************************************/

/** Initializes the alarm module. */
void AlarmInit(void)
{
	for(uint8_t i=0; i<ALARM_COUNT; i++)
	{
		alarm[i].hour = 12;
		alarm[i].minute = 0;
		alarm[i].enabled = FALSE;
		alarm[i].song = DF_SONG_ID_VOID;
	}
	alarmState = ALARM_DISPLAY_LABEL;
	rtcListener = RtcRegisterListener();
	isAlarmOn = FALSE;
}

/** Tests whether an alarm is on.
	@result TRUE if an alarm is currently on
*/
BOOL AlarmIsOn(void)
{
	return isAlarmOn;
}

/** Displays the label of the selected alarm at the LCD screen.
	This is "Alarm1" for the first alarm and "Alarm2" for the
	second.*/
void AlarmDisplayLabel(void)
{
	LcdWriteString_F(PSTR("Alarm"));
	LcdWriteChar(5, '1' + alarmNumber);
	LcdWriteChar(6, '\0');
}

/** Displays the time setting of the selected alarm. */
void AlarmDisplayTime(void)
{
	uint8_t hour, minute;


	hour = alarm[alarmNumber].hour;
	//if (ClockGetTimeFormat() == CLOCK_FORMAT_12H)
	//	hour = clock12HTable[hour];

	hour = CharToBCD2(hour);
	minute = CharToBCD2(alarm[alarmNumber].minute);

	if (alarmState == ALARM_EDIT_HOUR)
	{
		LcdWriteChar(0, BCD_HIGH_NIBBLE_CHAR(hour) | 0x80);
		LcdWriteChar(1, BCD_LOW_NIBBLE_CHAR(hour) | 0x80);
	}
	else
	{
		LcdWriteChar(0, BCD_HIGH_NIBBLE_CHAR(hour));
		LcdWriteChar(1, BCD_LOW_NIBBLE_CHAR(hour));
	}

	LcdWriteChar(2, ' ');

	if (alarmState == ALARM_EDIT_MINUTE)
	{
		LcdWriteChar(3, BCD_HIGH_NIBBLE_CHAR(minute) | 0x80);
		LcdWriteChar(4, BCD_LOW_NIBBLE_CHAR(minute) | 0x80);
	}
	else
	{
		LcdWriteChar(3, BCD_HIGH_NIBBLE_CHAR(minute));
		LcdWriteChar(4, BCD_LOW_NIBBLE_CHAR(minute));
	}
	LcdWriteChar(5, ' ');

	LcdWriteChar(6, '\0');
	LcdShowSymbol(0, alarm[0].enabled);
	LcdShowSymbol(1, alarm[1].enabled);
}

extern const char emptySongBookLabel_F[];

/** Displays the name of the selected dataflash song. If the dataflash songbook
	is empty, it displays an appropriate message.
*/
void AlarmDisplaySong(void)
{
	if (alarm[alarmNumber].song == DF_SONG_ID_VOID)
	{
		LcdWriteString_F(emptySongBookLabel_F);
	}
	else
	{
		LcdWriteString(currentSong.name);
	}
}


/** Alarm 1 editing main routine. It is a #UserTaskFnType function and must be
	included in the #userTasksTable of the Scheduler module. It displays the
	current setting of the alarm clocks and allows the user to modify the
	settings by using the joystick.
	@param key The key pressed by the user
	@param init If is set to TRUE, it indicates that the editing module
				must be initialized
	@return 1 or -1 if the user selected to leave this task. A value of
			1 requests a move to the next menu item and a value of -1
			a move to the previous one. A value of 0 indicates that this
			task is still running. */
int8_t Alarm1Main(char key, BOOL init)
{
	return AlarmMain(key, init, 0);
}

/** Alarm 2 editing main routine. It is a #UserTaskFnType function and must be
	included in the #userTasksTable of the Scheduler module. It displays the
	current setting of the alarm clocks and allows the user to modify the
	settings by using the joystick.
	@param key The key pressed by the user
	@param init If is set to TRUE, it indicates that the editing module
				must be initialized
	@return 1 or -1 if the user selected to leave this task. A value of
			1 requests a move to the next menu item and a value of -1
			a move to the previous one. A value of 0 indicates that this
			task is still running. */
int8_t Alarm2Main(char key, BOOL init)
{
	return AlarmMain(key, init, 1);
}


/** Common Alarm editing routine. It is called by both #Alarm1Main and
	#Alarm2Main.
	@param key The key pressed by the user
	@param init If is set to TRUE, it indicates that the editing module
				must be initialized
	@param number Number of alarm to edit
	@return 1 or -1 if the user selected to leave this task. A value of
			1 requests a move to the next menu item and a value of -1
			a move to the previous one. A value of 0 indicates that this
			task is still running. */
int8_t AlarmMain(char key, BOOL init, uint8_t number)
{
	if (init)
	{
		alarmNumber = number;
		alarmState = ALARM_DISPLAY_LABEL;
		AlarmDisplayLabel();
		return 0;
	}

	if (alarmState == ALARM_DISPLAY_LABEL)
	{
		switch(key)
		{
			case KEY_DOWN:
				alarmState = ALARM_DISPLAY_LABEL;
				return 1;
				break;
			case KEY_UP:
				alarmState = ALARM_DISPLAY_LABEL;
				return -1;
				break;
			case KEY_RIGHT:
				alarmState = ALARM_DISPLAY_TIME;
				AlarmDisplayTime();
				break;
		}
	}
	else if (alarmState == ALARM_DISPLAY_TIME)
	{
		switch(key)
		{
			case KEY_DOWN:
			case KEY_UP:
				// Toggle enable
				alarm[alarmNumber].enabled ^= 1;
				AlarmDisplayTime();
				break;
			case KEY_LEFT:
				alarmState = ALARM_DISPLAY_LABEL;
				AlarmDisplayLabel();
				break;
			case KEY_RIGHT:
				if (!SoundIsSongPlaying())
				{
					BOOL OK;

					alarmState = ALARM_SELECT_SONG;
					OK = DfSongsSelectSong(alarm[alarmNumber].song, &currentSong);
					if (!OK)
					{
						// Try to load another Dataflash song
						if (DfSongsSelectNextSong(&currentSong, TRUE))
						{
							alarm[alarmNumber].song = DfSongsGetSelectedSongId();
						}
						else
						{
							alarm[alarmNumber].song = DF_SONG_ID_VOID;
						}
					}
					 AlarmDisplaySong();
				}
				break;
			case KEY_ENTER:
				alarmState = ALARM_EDIT_HOUR;
				AlarmDisplayTime();
				break;
		}
	}
	else if (alarmState == ALARM_EDIT_HOUR)
	{
		switch(key)
		{
			case KEY_UP:
				alarm[alarmNumber].hour++;
				if (alarm[alarmNumber].hour == 24)
					alarm[alarmNumber].hour = 0;
				break;
			case KEY_DOWN:
				if (alarm[alarmNumber].hour > 0)
					alarm[alarmNumber].hour--;
				else
					alarm[alarmNumber].hour = 23;
				break;
			case KEY_LEFT:
			case KEY_RIGHT:
				alarmState = ALARM_EDIT_MINUTE;
				break;
			case KEY_ENTER:
				alarmState = ALARM_DISPLAY_TIME;
				break;
		}
		if (key != KEY_NULL)
			AlarmDisplayTime();
	}
	else if (alarmState == ALARM_EDIT_MINUTE)
	{
		switch(key)
		{
			case KEY_UP:
				alarm[alarmNumber].minute++;
				if (alarm[alarmNumber].minute == 60)
					alarm[alarmNumber].minute = 0;
				break;
			case KEY_DOWN:
				if (alarm[alarmNumber].minute > 0)
					alarm[alarmNumber].minute--;
				else
					alarm[alarmNumber].minute = 59;
				break;
			case KEY_LEFT:
			case KEY_RIGHT:
				alarmState = ALARM_EDIT_HOUR;
				break;
			case KEY_ENTER:
				alarmState = ALARM_DISPLAY_TIME;
				break;
		}
		if (key != KEY_NULL)
			AlarmDisplayTime();
	}
	else if (alarmState == ALARM_SELECT_SONG)
	{
		switch(key)
		{
			case KEY_DOWN:
			case KEY_UP:
				if (DfSongsSelectNextSong(&currentSong, key != KEY_UP))
				{
					alarm[alarmNumber].song = DfSongsGetSelectedSongId();
				}
				else
				{
					alarm[alarmNumber].song = DF_SONG_ID_VOID;
				}
				AlarmDisplaySong();
				break;
			case KEY_LEFT:
				alarmState = ALARM_DISPLAY_TIME;
				AlarmDisplayTime();
				break;
		}
	}
	return 0;
}

/** Stops the alarm song. */
void AlarmStop(void)
{
	SoundSongStop();
	isAlarmOn = FALSE;
}

/** Compares current time with alarm settings. If current time matches an alarm
	time and the alarm is enabled, it starts playing the selected sound.
*/
void AlarmCompareTask(void)
{
	if (!isAlarmOn && RtcNewEvent(rtcListener))
	{
		for(uint8_t i=0; i<ALARM_COUNT; i++)
		{
			if(alarm[i].enabled && alarm[i].hour == RtcGetHour() &&
			   alarm[i].minute == RtcGetMinute() && RtcGetSecond() == 0)
			{
				if (!SoundIsOn() && alarmState == ALARM_DISPLAY_LABEL)
				{
					BOOL OK = DfSongsSelectSong(alarm[i].song, &currentSong);

					if (!OK)
					{
						OK = SongSelect(i, &currentSong);
					}

					if (OK)
					{
						isAlarmOn = TRUE;
						SoundSongStart();
						break;
					}
				}
			}
		}
	}
}
