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

/** @file comm.c Communication module. Provides functions for editing the
		electronic card name and updating the dataflash song book though the
		serial port. It defines a non ISR task that must be iteratively
		called by the main routine. This task serves the characters received
		through the serial port.
*/

#include <avr/pgmspace.h>
#include <inttypes.h>

#include "c_ext.h"
#include "usart.h"
#include "card.h"
#include "sound.h"
#include "songs.h"
#include "dfSongs.h"
#include "bcd.h"

/*******************************************************************************
* Firmware Version
*******************************************************************************/

/** A flash string with the current version of the firmware. */
const char versionString_F[] PROGMEM = "BF SDK 0.9";

/*******************************************************************************
* Definitions
*******************************************************************************/

/** Enumeration of Communication module's states. */
typedef enum
{
	COMM_STATE_IDLE = 0,
	COMM_STATE_CARD_NAME,
	COMM_STATE_DF_SONG,

} CommState;

/** Enumeration of states for song book's operations. */
typedef enum
{
	COMM_STATE_DF_SONG_START = 0,
	COMM_STATE_DF_SONG_WRITE_NAME,
	COMM_STATE_DF_SONG_WRITE_SETTINGS,
	COMM_STATE_DF_SONG_WRITE_SONG,
	COMM_STATE_DF_SONG_READ,
	COMM_STATE_DF_SONG_DELETE,
} DfSongState;

// First byte
#define CARD_NAME_EDIT_START_ID		'N'
#define DATAFLASH_SONG_OPERATION_ID	'S'
#define FIRMWARE_VERSION_ID			'V'

// Second byte
#define DATAFLASH_SONG_READ_ID		'R'
#define DATAFLASH_SONG_WRITE_ID		'W'
#define DATAFLASH_SONG_DELETE_ID	'D'
#define DATAFLASH_SONG_MAINTAIN_ID	'M'

// Third byte
#define DATAFLASH_SONG_READ_UP_ID	'U'
#define DATAFLASH_SONG_READ_DOWN_ID	'D'

/*******************************************************************************
* Flash Strings
*******************************************************************************/

/** Reply from the module in case of an error */
const char errorString_F[] PROGMEM = "Error!";

/** Reply from the module in case of a successful operation */
const char okString_F[] PROGMEM = "OK";

/*******************************************************************************
* Static Variables
*******************************************************************************/

/** Communication module's state. */
static CommState state;

/** Dataflash song book operation state. */
static DfSongState dfSongState;

/** Index of character received from the UART. Starts from 0 and increments
	every time a new character is received. It is cleared back to 0, when
	an operation is completed. */
static uint16_t rxIndex;

/** The expected size of the song being downloaded. */
static uint8_t expectedSongSize;

/*******************************************************************************
* Functions
*******************************************************************************/

/** Initializes the Communications Module. */
void CommInit(void)
{
	state = COMM_STATE_IDLE;
}

/** Edits the electronic card's name. For every byte received, the corresponding
	byte in the card's name buffer in RAM is modified. If the maximum length of
	the name is exceeded or a null character is receiver, the name is saved in
	EEPROM.
	@param byte The character received.
*/
void CommCardNameEdit(uint8_t byte)
{
	CardSetNameChar(rxIndex, byte);
	rxIndex++;

	if (rxIndex == CARD_NAME_MAX_SIZE || byte == '\0')
	{
		if (rxIndex < CARD_NAME_MAX_SIZE - 1)
			CardSetNameChar(rxIndex, byte);
		CardSaveName();
		UsartTxString_F(okString_F);
		state = COMM_STATE_IDLE;
	}
}

/** Serves a received byte, when the module is in dataflash song book updating
	state. Switches between song book operation states.
	@param byte The character received.
*/
void CommDfSongOperation(uint8_t byte)
{
	BOOL error = FALSE;

	switch (dfSongState)
	{
	case COMM_STATE_DF_SONG_START:
		switch (byte)
		{
			case DATAFLASH_SONG_READ_ID:
				dfSongState = COMM_STATE_DF_SONG_READ;
				break;
			case DATAFLASH_SONG_WRITE_ID:
				if (DfSongsWriteSongStart())
				{
					rxIndex = 0;
					expectedSongSize = 0;
					dfSongState = COMM_STATE_DF_SONG_WRITE_NAME;
				}
				else
					error = TRUE;
				break;
			case DATAFLASH_SONG_DELETE_ID:
				dfSongState = COMM_STATE_DF_SONG_DELETE;
				break;
			case DATAFLASH_SONG_MAINTAIN_ID:
				if (DfSongsMaintain())
				{
					UsartTxString_F(okString_F);
					state = COMM_STATE_IDLE;
				}
				else
				{
					error = TRUE;
				}
				break;
			default:
				error = TRUE;
		}
		break;
	case COMM_STATE_DF_SONG_READ:
		if (byte == DATAFLASH_SONG_READ_UP_ID ||
			byte == DATAFLASH_SONG_READ_DOWN_ID)
		{
			if (DfSongsSelectNextSong(&currentSong,
									  byte == DATAFLASH_SONG_READ_UP_ID))
			{
				uint8_t i, temp;

				temp = CharToBCD2(DfSongsGetSelectedSongId());
				UsartTxChar(BCD_HIGH_NIBBLE_CHAR(temp));
				UsartTxChar(BCD_LOW_NIBBLE_CHAR(temp));
				UsartTxChar(',');
				for(i=0; i<SONG_NAME_SIZE_MAX; i++)
				{
					if (!currentSong.name[i])
						break;
					UsartTxChar(currentSong.name[i]);
				}
				UsartTxChar(',');
				UsartTxChar('T');
				UsartTxChar('0' + currentSong.tempo);
				if (currentSong.repeat)
				{
					UsartTxChar(',');
					UsartTxChar('R');
				}
				UsartTxChar(',');
				temp = CharToBCD2(currentSong.size);
				UsartTxChar(BCD_HIGH_NIBBLE_CHAR(temp));
				UsartTxChar(BCD_LOW_NIBBLE_CHAR(temp));
				state = COMM_STATE_IDLE;
			}
			else
				error = TRUE;
		}
		else
		{
			error = TRUE;
		}
		break;
	case COMM_STATE_DF_SONG_WRITE_NAME:
		DfSongsWriteSongInsertByte(rxIndex, byte);
		rxIndex++;
		if (rxIndex == SONG_NAME_SIZE_MAX)
			dfSongState = COMM_STATE_DF_SONG_WRITE_SETTINGS;
		break;
	case COMM_STATE_DF_SONG_WRITE_SETTINGS:
		DfSongsWriteSongInsertByte(rxIndex, byte);
		rxIndex++;
		if (rxIndex == SONG_NAME_SIZE_MAX + 2)
		{
			dfSongState = COMM_STATE_DF_SONG_WRITE_SONG;
			expectedSongSize = byte;
			if (expectedSongSize > DF_SONG_MAX_SIZE)
				expectedSongSize = DF_SONG_MAX_SIZE;
		}
		break;
	case COMM_STATE_DF_SONG_WRITE_SONG:
		{
			uint16_t size;

			DfSongsWriteSongInsertByte(rxIndex, byte);
			rxIndex++;

			size = (((uint16_t) expectedSongSize) << 1) +
					SONG_NAME_SIZE_MAX + 2;

			if (size == rxIndex)
			{
				DfSongsWriteSongFinish();
				UsartTxString_F(okString_F);
				state = COMM_STATE_IDLE;
			}
		}
		break;
	case COMM_STATE_DF_SONG_DELETE:
		if (DfSongDelete(byte))
		{
			UsartTxString_F(okString_F);
			state = COMM_STATE_IDLE;
		}
		else
			error = TRUE;
		break;
	}

	if (error)
	{
		state = COMM_STATE_IDLE;
		UsartTxString_F(errorString_F);
	}
}

/** Communication task. It must be called by the main routine iteratively.
	It checks for new received bytes and calls the appropriate function based
	on the module's state.
	Precise timing isn't necessary. However it must be called often enough,
	for characters not to be lost (The maximum allowed time between two
	successive calls is UART_BUFFER_SIZE*10/9600 sec).*/
void CommTask(void)
{
	uint8_t byteIn;

	while(UsartNewDataIn())
	{
		byteIn = UsartRxChar();

		switch(state)
		{
		case COMM_STATE_IDLE:
			switch (byteIn)
			{
				case CARD_NAME_EDIT_START_ID:
					if (CardEditInProgress())
						UsartTxString_F(errorString_F);
					else
					{
						rxIndex = 0;
						state = COMM_STATE_CARD_NAME;
					}
					break;
				case DATAFLASH_SONG_OPERATION_ID:
					if (SoundIsSongPlaying())
						UsartTxString_F(errorString_F);
					else
					{
						state = COMM_STATE_DF_SONG;
						dfSongState = COMM_STATE_DF_SONG_START;
					}
					break;
				case FIRMWARE_VERSION_ID:
					UsartTxString_F(versionString_F);
					break;
			}
			break;
		case COMM_STATE_CARD_NAME:
			CommCardNameEdit(byteIn);
			break;
		case COMM_STATE_DF_SONG:
			CommDfSongOperation(byteIn);
			break;
		}
	}
}
