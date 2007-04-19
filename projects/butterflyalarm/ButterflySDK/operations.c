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

/** @file operations.c Collection of user tasks. The file contains
	#UserTaskFnType functions to be included in the #userTasksTable of the
	Scheduler module.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include "c_ext.h"
#include "usart.h"
#include "button.h"
#include "timer0.h"
#include "lcd.h"
#include "rtc.h"
#include "clock.h"
#include "sensors.h"
#include "dataflash.h"
#include "spi.h"
#include "bcd.h"
#include "sound.h"
#include "songs.h"
#include "pmSongs.h"
#include "dfSongs.h"


/*******************************************************************************
* Static variables
*******************************************************************************/

/** Id of an rtc listener */
static uint8_t rtcListener;

/*******************************************************************************
* Functions
*******************************************************************************/

/** Clock main routine. It is a #UserTaskFnType function and must be
	included in the #userTasksTable of the Scheduler module. It displays the
	clock and allows the user to edit the time by using the joystick.
	@param key The key pressed by the user
	@param init If is set to TRUE, it indicates that the editing module
				must be initialized
	@return 1 or -1 if the user selected to leave this task. A value of
			1 requests a move to the next menu item and a value of -1
			a move to the previous one. A value of 0 indicates that this
			task is still running. */
int8_t ClockMain(char key, BOOL init)
{
	if (init)
	{
		ClockTimeUpdate();
		rtcListener = RtcRegisterListener();
		return 0;
	}

	if (ClockGetTimeState() == CLOCK_SHOW)
	{
		if (key == KEY_DOWN || key == KEY_UP)
		{
			RtcUnregisterListener(rtcListener);
			LcdColon(FALSE);
			return (key == KEY_DOWN) ? 1 : -1;
		}
		else if (key == KEY_ENTER)
		{
			ClockTimeEditInit();
		}
		else if (RtcNewEvent(rtcListener))
		{
			ClockTimeUpdate();
		}
	}
	else
	{
		ClockTimeEdit(key);
	}
	return 0;
}

/** Date main routine. It is a #UserTaskFnType function and must be
	included in the #userTasksTable of the Scheduler module. It displays the
	date and allows the user to edit it by using the joystick.
	@param key The key pressed by the user
	@param init If is set to TRUE, it indicates that the editing module
				must be initialized
	@return 1 or -1 if the user selected to leave this task. A value of
			1 requests a move to the next menu item and a value of -1
			a move to the previous one. A value of 0 indicates that this
			task is still running. */
int8_t DateMain(char key, BOOL init)
{
	if (init)
	{
		ClockDateUpdate();
		rtcListener = RtcRegisterListener();
		return 0;
	}

	if (ClockGetDateState() == DATE_SHOW)
	{
		if (key == KEY_DOWN || key == KEY_UP)
		{
			RtcUnregisterListener(rtcListener);
			LcdColon(FALSE);
			return (key == KEY_DOWN) ? 1 : -1;
		}
		else if (key == KEY_ENTER)
		{
			ClockDateEditInit();
		}
		else if (RtcNewEvent(rtcListener))
		{
			ClockDateUpdate();
		}
	}
	else
	{
		ClockDateEdit(key);
	}
	return 0;
}

/** Sensor measurements routine. It is a #UserTaskFnType function and must be
	included in the #userTasksTable of the Scheduler module. It allows the user
	to select one of the different sensor measurements (temperature, light,
	voltage)
	@param key The key pressed by the user
	@param init If is set to TRUE, it indicates that the editing module
				must be initialized
	@return 1 or -1 if the user selected to leave this task. A value of
			1 requests a move to the next menu item and a value of -1
			a move to the previous one. A value of 0 indicates that this
			task is still running. */
int8_t MeasurementMain(char key, BOOL init)
{
	static uint8_t sensor;
	BOOL changeSensor = FALSE;

	if (init)
	{
		sensor = 0;
		changeSensor = TRUE;
		rtcListener = RtcRegisterListener();
		SensorsSelect(0);
		SensorsDisplayMeasurement();
		return 0;
	}

	if (key == KEY_DOWN)
	{
		RtcUnregisterListener(rtcListener);
		return 1;
	}
	if (key == KEY_UP)
	{
		RtcUnregisterListener(rtcListener);
		return -1;
	}

	if (key == KEY_LEFT)
	{
		if (sensor == 0)
			sensor = 2;
		else
			sensor--;
		changeSensor = TRUE;
	}
	else if (key == KEY_RIGHT)
	{
		if (sensor == 2)
			sensor = 0;
		else
			sensor++;
		changeSensor = TRUE;
	}

	if (changeSensor)
	{
		SensorsSelect((SensorType) sensor);
		SensorsDisplayMeasurement();
	}
	else if (RtcNewEvent(rtcListener))
	{
		SensorsDisplayMeasurement();
	}

	return 0;
}

#ifdef __USE_DATAFLASH_TEST__
extern const uint8_t Mozart[];

/** Dataflash test routine. It is a #UserTaskFnType function and must be
	included in the #userTasksTable of the Scheduler module. It allows the
	user to perform a test of the dataflash routines. The test writes a song
	in the dataflash and verifies that it can be read back correctly.
	@param key The key pressed by the user
	@param init If is set to TRUE, it indicates that the editing module
				must be initialized
	@return 1 or -1 if the user selected to leave this task. A value of
			1 requests a move to the next menu item and a value of -1
			a move to the previous one. A value of 0 indicates that this
			task is still running. */
int8_t DataFlashTestMain(char key, BOOL init)
{
	if (init)
	{
		LcdWriteString_F(PSTR("DataFlash Test"));
		return 0;
	}

	if (key == KEY_DOWN)
		return 1;
	if (key == KEY_UP)
		return -1;
	if (key == KEY_ENTER)
	{
		uint8_t j, byte;

		//UsartTxChar(DataFlashReadStatus());

		DATAFLASH_CHIP_SELECT_INACTIVE();
		DATAFLASH_CHIP_SELECT_ACTIVE();

		// Initialize write, write ID
		DataFlashBufferWriteByte(0, 0, 1);

		for(j=0;j<102;j++)
		{
			byte = pgm_read_byte(&Mozart[j]);
			SpiReadWrite(byte);
		}
		DataFlashBufferToPage(0, 1);

		DATAFLASH_CHIP_SELECT_INACTIVE();
		DATAFLASH_CHIP_SELECT_ACTIVE();

		DataFlashContinuousReadEnable(1, 0);

		for(j=0;j<103;j++)
		{
			byte = SpiReadWrite(0);

			if (j==0)
			{
				if (byte != 1)
					break;
			}
			else if( byte != pgm_read_byte(&Mozart[j-1]))
			{
				break;
			}
		}
		DATAFLASH_CHIP_SELECT_INACTIVE();

		if (j == 103)
		{
			LcdWriteString_F(PSTR("OK"));
		}
		else
		{
			LcdWriteString_F(PSTR("ERROR!"));
		}
	}
	return 0;
}
#endif // __USE_DATAFLASH_TEST__

/*******************************************************************************
* Play song
*******************************************************************************/

const char songsLabel_F[] PROGMEM = "Songs1";
const char stopLabel_F[] PROGMEM = "Stop";

/** Play flash song routine. It is a #UserTaskFnType function and must be
	included in the #userTasksTable of the Scheduler module. It allows the
	user to select one of the songs in flash memory and start/stop playing it.
	@param key The key pressed by the user
	@param init If is set to TRUE, it indicates that the editing module
				must be initialized
	@return 1 or -1 if the user selected to leave this task. A value of
			1 requests a move to the next menu item and a value of -1
			a move to the previous one. A value of 0 indicates that this
			task is still running. */
int8_t PlaySongMain(char key, BOOL init)
{
	static int8_t songIndex;

	if (init)
	{
		songIndex = -1;
		LcdWriteString_F(songsLabel_F);
		return 0;
	}

	if (key == KEY_NULL)
		return 0;

	if (songIndex == -1)
	{
		if (key == KEY_DOWN)
			return 1;
		if (key == KEY_UP)
			return -1;
		if (key == KEY_RIGHT)
		{
			songIndex = 0;
			if (SoundIsSongPlaying())
			{
				LcdWriteString_F(stopLabel_F);
				return 0;
			}
		}
	}
	else if (key == KEY_UP)
	{
		if (SoundIsSongPlaying())
		{
			SoundVolumeUp();
		}
		else
		{
			if (songIndex > 0)
				songIndex--;
			else
				songIndex = SongGetCount() - 1;
		}
	}
	else if (key == KEY_DOWN)
	{
		if (SoundIsSongPlaying())
		{
			SoundVolumeDown();
		}
		else
		{
			songIndex++;
			if (songIndex == SongGetCount())
				songIndex= 0;
		}
	}
	else if (key == KEY_ENTER)
	{
		if (!SoundIsSongPlaying())
		{
			if (SongSelect(songIndex, &currentSong))
			{
				SoundSongStart();
			}
		}
		else
		{
			SoundSongStop();
			songIndex = -1;
			LcdWriteString_F(songsLabel_F);
		}
		return 0;
	}
	else if (key == KEY_LEFT)
	{
		songIndex = -1;
		LcdWriteString_F(songsLabel_F);
		return 0;
	}

	// Show song name
	if (!SoundIsSongPlaying())
	{
		if (SongSelect(songIndex, &currentSong))
		{
			LcdWriteString(currentSong.name);
		}
	}

	return 0;
}

/*******************************************************************************
* Play dataflash song
*******************************************************************************/

const char songs2Label_F[] PROGMEM = "Songs2";
const char emptySongBookLabel_F[] PROGMEM = "Empty";

/** Play dataflash song routine. It is a #UserTaskFnType function and must be
	included in the #userTasksTable of the Scheduler module. It allows the
	user to select one of the songs in dataflash song book and start/stop
	playing it.
	@param key The key pressed by the user
	@param init If is set to TRUE, it indicates that the editing module
				must be initialized
	@return 1 or -1 if the user selected to leave this task. A value of
			1 requests a move to the next menu item and a value of -1
			a move to the previous one. A value of 0 indicates that this
			task is still running. */
int8_t PlayDataflashSongMain(char key, BOOL init)
{
	static enum {PLAY_DF_SONG_SHOW_LABEL = 0,
				 PLAY_DF_SONG_SELECT_ERROR,
				 PLAY_DF_SONG_SELECT_OK} playDfSongState;

	if (init)
	{
		DfSongsInit();
		playDfSongState = PLAY_DF_SONG_SHOW_LABEL;
		LcdWriteString_F(songs2Label_F);
		return 0;
	}

	if (key == KEY_NULL)
		return 0;

	if (playDfSongState == PLAY_DF_SONG_SHOW_LABEL)
	{
		if (key == KEY_DOWN)
			return 1;
		if (key == KEY_UP)
			return -1;
		if (key == KEY_RIGHT)
		{
			if (SoundIsSongPlaying())
			{
				playDfSongState = PLAY_DF_SONG_SELECT_OK;
				LcdWriteString_F(stopLabel_F);
				return 0;
			}
			else if (DfSongsSelectNextSong(&currentSong, TRUE))
			{
				playDfSongState = PLAY_DF_SONG_SELECT_OK;
			}
			else
			{
				playDfSongState = PLAY_DF_SONG_SELECT_ERROR;
			}
		}
	}
	else if (playDfSongState == PLAY_DF_SONG_SELECT_ERROR)
	{
		if (key == KEY_LEFT)
		{
			playDfSongState = PLAY_DF_SONG_SHOW_LABEL;
			LcdWriteString_F(songs2Label_F);
			return 0;
		}
	}
	else if (key == KEY_UP || key == KEY_DOWN)
	{
		if (SoundIsSongPlaying())
		{
			if (key == KEY_UP)
				SoundVolumeUp();
			else
				SoundVolumeDown();
		}
		else
		{
			DfSongsSelectNextSong(&currentSong, key != KEY_UP);
		}
	}
	else if (key == KEY_ENTER)
	{
		if (!SoundIsSongPlaying())
		{
			if (playDfSongState == PLAY_DF_SONG_SELECT_OK)
				SoundSongStart();
		}
		else
		{
			SoundSongStop();
			playDfSongState = PLAY_DF_SONG_SHOW_LABEL;
			LcdWriteString_F(songs2Label_F);
		}
		return 0;
	}
	else if (key == KEY_LEFT)
	{
		playDfSongState = PLAY_DF_SONG_SHOW_LABEL;
		LcdWriteString_F(songs2Label_F);
		return 0;
	}

	// Show song name
	if (!SoundIsSongPlaying())
	{
		if (playDfSongState == PLAY_DF_SONG_SELECT_OK)
		{
			LcdWriteString(currentSong.name);
		}
		else if (playDfSongState == PLAY_DF_SONG_SELECT_ERROR)
		{
			LcdWriteString_F(emptySongBookLabel_F);
		}
	}

	return 0;
}
