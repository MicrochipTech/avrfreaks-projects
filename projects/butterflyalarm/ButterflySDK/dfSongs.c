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

/** @file dfSongs.c Dataflash Song Book module. Provides functionality for
		downloading songs to the dataflash and playing them back from the
		Sound module. The songs are stored in the dataflash and have the
		same format as the songs in flash (defined in Songs module). The
		organisation of the dataflash is as follows:
		<br><br><table border="1" align="middle">
		<tr align="middle"><th>Page number</th><th>Byte 0</th><th>Byte 1-12</th>
			<th>Byte 13-14</th><th>Byte 15-254</th></tr>
		<tr align="middle"><td>1</td><td>Song ID</td><td>Song Name</td>
			<td>Song Settings</td><td>Song Body</td></tr>
		<tr align="middle"><td>2</td><td>Song ID</td><td>Song Name</td>
			<td>Song Settings</td><td>Song Body</td></tr>
		<tr><td> </td><td> </td><td> </td><td> </td><td> </td></tr>
		<tr align="middle"><td>250</td><td>Song ID</td><td>Song Name</td>
		<td>Song Settings</td><td>Song Body</td></tr>
		</table><br>
		The songs are in Sector 0 and Sector 1 of the dataflash, in pages 1 to
		250. Each page can contain exactly one song. Therefore up to 250 songs
		can be stored. The first byte in the page is the song ID. The song ID
		is always the same with the number of the page the song is stored (1
		for song in page 1, 2 for song in page 2 and so on). A value of 0xFE
		denotes a deleted song and a value of 0xFF an empty page (a page never
		written with a song). The module read songs starting from page 1 and
		stops if all 250 pages are read or if a page with 0xFF in the first
		byte is encountered. The next 12 bytes is the song's name. After the
		name, two bytes with the song's settings follows (tempo, repeat,
		size). The last segment is the song's body, which can contain up to
		120 notes. For each note two bytes are required.
*/

#include <avr/io.h>
#include <inttypes.h>

#include "c_ext.h"
#include "spi.h"
#include "dataflash.h"
#include "songs.h"
#include "dfSongs.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/** @def DF_SONGS_DATAFLASH_BUFFER Dataflash buffer used for Song Book
	operations */
#define DF_SONGS_DATAFLASH_BUFFER	0

#define DF_SONGS_PAGE_FIRST		1
#define DF_SONGS_PAGE_COUNT		250
#define DF_SONGS_PAGE_LAST		(DF_SONGS_PAGE_FIRST + DF_SONGS_PAGE_COUNT - 1)

#define DF_SONG_HEADER_SIZE		15

/*******************************************************************************
* Static variables
*******************************************************************************/

/** Number of dataflash page the module is currently using. */
static uint16_t dfSongsPageIndex;

/** The first note of the song loaded by the module. It is stored in RAM for
	the songs that start from the beginning, when all the notes are played*/
static Note firstNote;

/** The note that it would be played at the next Timer0 interrupt. */
static Note nextNote;

/** State of the module. */
static enum {
	DF_SONGS_PLAY_BACK_IDLE = 0,
	DF_SONGS_PLAY_BACK_GET_NEXT_NOTE,
	DF_SONGS_PLAY_BACK_REPEAT
	} dfSongsPlayBackState;

/** TRUE if a song is currently being written. Playback is disabled during
	writing of songs. */
static BOOL writingSong;

/** Byte address in the currently selected page. */
static uint16_t byteAddress;

/*******************************************************************************
* Functions
*******************************************************************************/

/** Initializes the Dataflash Song Book module.
*/
void DfSongsInit(void)
{
	dfSongsPageIndex = DF_SONGS_PAGE_FIRST - 1;

	firstNote.duration = firstNote.tone = 0;
	nextNote.duration = nextNote.tone = 0;
	dfSongsPlayBackState = DF_SONGS_PLAY_BACK_IDLE;
	writingSong = FALSE;
}

/** Initiates the writing of a song in the dataflash. It searches for a free
	page in the dataflash, writes the song id and switches the module to the
	writing song state. #DfSongsWriteSongInsertByte and #DfSongsWriteSongFinish
	must be subsequently called to complete the writing of a song.
	@result TRUE if an empty page was found.
*/
BOOL DfSongsWriteSongStart(void)
{
	uint16_t page, id;

	SpiInit();
	for(page = DF_SONGS_PAGE_FIRST; page != (DF_SONGS_PAGE_LAST + 1); page++)
	{
		DataFlashContinuousReadEnable(page, 0);

		id = SpiReadWrite(0);

		if (id == DF_SONG_ID_VOID || id == DF_SONG_ID_DELETED)
		{
			break;
		}
	}

	if (page == DF_SONGS_PAGE_LAST + 1)
	{
		// Song book is full
		SpiDisable();
		DATAFLASH_CHIP_SELECT_INACTIVE();
		return FALSE;
	}

	writingSong = TRUE;
	dfSongsPageIndex = page;

	DataFlashBufferWriteEnable(DF_SONGS_DATAFLASH_BUFFER, 0);
	// Write ID = page number
	SpiReadWrite((uint8_t) page);

	SpiDisable();
	DATAFLASH_CHIP_SELECT_INACTIVE();

	return TRUE;
}

/** Writes a byte of a song. The byte is written temporarily in a dataflash
	buffer.
	@param pos Position of the new byte in the song memory
	@param byte The byte to write
*/
void DfSongsWriteSongInsertByte(uint16_t pos, uint8_t byte)
{
	SpiInit();
	DataFlashBufferWriteEnable(DF_SONGS_DATAFLASH_BUFFER, pos+1);
	SpiReadWrite(byte);
	SpiDisable();
	DATAFLASH_CHIP_SELECT_INACTIVE();
}

/** Finalizes the writing of a song. It moves the song data from the dataflash
	buffer to a page. It would block for 20ms until the writing is completed.
*/
void DfSongsWriteSongFinish(void)
{
	// Will block for 20ms
	SpiInit();
	DataFlashBufferToPage(DF_SONGS_DATAFLASH_BUFFER, dfSongsPageIndex);
	writingSong = FALSE;
	DATAFLASH_CHIP_SELECT_INACTIVE();
	SpiDisable();
}

/** Gets the selected song's ID
	@result The song's ID
*/
uint8_t DfSongsGetSelectedSongId(void)
{
	return (uint8_t) dfSongsPageIndex;
}

/** Deletes a song from the song book. It would block for 20ms.
	@param id The id of the song to delete
	@result TRUE if an id of a song that can be deleted is given
*/
BOOL DfSongDelete(uint8_t id)
{
	uint8_t songState;
	dfSongsPageIndex = id;

	SpiInit();
	DataFlashContinuousReadEnable(dfSongsPageIndex, 0);

	songState = SpiReadWrite(0);

	if (songState == DF_SONG_ID_VOID || songState == DF_SONG_ID_DELETED)
	{
		SpiDisable();
		DATAFLASH_CHIP_SELECT_INACTIVE();
		return FALSE;
	}

	DataFlashBufferWriteByte(DF_SONGS_DATAFLASH_BUFFER, 0, DF_SONG_ID_DELETED);

	// Page write, takes 20ms
	DataFlashBufferToPage(DF_SONGS_DATAFLASH_BUFFER, dfSongsPageIndex);

	SpiDisable();
	DATAFLASH_CHIP_SELECT_INACTIVE();
	return TRUE;
}

/** Reads the header of the dataflash song (name, settings, first note) pointed
	to by the #dfSongsPageIndex variable.
	@param currentSong A pointer to a SongInfo structure that the name and the
			settings of the song will be copied to
	@result The id of the song read.
*/
uint8_t DfSongsReadSongHeader(SongInfo *currentSong)
{
	uint8_t i, id;
	uint8_t byte1, byte2;

	SpiInit();
	DataFlashContinuousReadEnable(dfSongsPageIndex, 0);

	id = SpiReadWrite(0);

	if (id == DF_SONG_ID_VOID || id == DF_SONG_ID_DELETED)
	{
		SpiDisable();
		DATAFLASH_CHIP_SELECT_INACTIVE();
		return id;
	}

	for(i=0; i<SONG_NAME_SIZE_MAX; i++)
	{
		currentSong->name[i] = SpiReadWrite(0);
	}
	currentSong->name[SONG_NAME_SIZE_MAX] = '\0';
	byte1 = SpiReadWrite(0);
	byte2 = SpiReadWrite(0);
	currentSong->tempo = SETTINGS_B1_GET_TEMPO(byte1);
	currentSong->repeat = SETTINGS_B1_GET_REPEAT(byte1);
	currentSong->size = SETTINGS_B0_GET_SIZE(byte2);
	currentSong->type = SONG_DATAFLASH_TYPE;

	byte1 = SpiReadWrite(0);
	byte2 = SpiReadWrite(0);

	firstNote.duration = NOTE_BYTE1_GET_DURATION(byte1);
	firstNote.tone = NOTE_BYTES_GET_TONE(byte1, byte2);

	nextNote.duration = firstNote.duration;
	nextNote.tone = firstNote.duration;

	dfSongsPlayBackState = DF_SONGS_PLAY_BACK_IDLE;

	// Move page to buffer - takes 250 us to complete
	DataFlashPageToBuffer(dfSongsPageIndex, DF_SONGS_DATAFLASH_BUFFER);

	SpiDisable();
	DATAFLASH_CHIP_SELECT_INACTIVE();

	return id;
}

/** Selects the next or previous available song in the dataflash. It is used to
	build a list of the songs in the song book. It traverses the song book
	either forwards (ascending page numbers) or backwards (descending
	page numbers), based on the moveForward parameter. When the routine is
	called for the very first time, it will start from page 1, regardless
	of the moveForward parameter. In subsequently calls it will use the
	#dfSongsPageIndex as a starting point. If a deleted song is found, the
	routine will try the immediate next (or previous one). If moving forwards
	and the last page is reached or an empty page is found, the routine will
	continue from the beginning. When moving backwards and the first page is
	reached, the routine will stop and report failure. The routine will update
	the #dfSongsPageIndex variable with the page number of the found song and
	it will call #DfSongsReadSongHeader to load its header.
	@param currentSong  A pointer to a SongInfo structure that the name and the
			settings of the song found will be copied to
	@param moveForward If TRUE the song book will be traversed forwards
	@result TRUE if a song is found
*/
BOOL DfSongsSelectNextSong(SongInfo *currentSong, BOOL moveForward)
{
	uint8_t id = dfSongsPageIndex;
	uint16_t startPage = 0;
	BOOL firstTime = TRUE;

	if (writingSong)
		return FALSE;

	if (dfSongsPageIndex < DF_SONGS_PAGE_FIRST)
		moveForward = TRUE;

	do
	{
		if (moveForward)
		{
			dfSongsPageIndex++;
			if (dfSongsPageIndex > DF_SONGS_PAGE_LAST)
				dfSongsPageIndex = DF_SONGS_PAGE_FIRST;
		}
		else
		{
			dfSongsPageIndex--;
			if (dfSongsPageIndex < DF_SONGS_PAGE_FIRST)
				dfSongsPageIndex = DF_SONGS_PAGE_FIRST;
		}

		if (firstTime)
		{
			startPage = dfSongsPageIndex;
			firstTime = FALSE;
		}
		else if (dfSongsPageIndex == startPage)
			return FALSE;

		id = DfSongsReadSongHeader(currentSong);

		if (id == DF_SONG_ID_VOID)
			dfSongsPageIndex = DF_SONGS_PAGE_FIRST - 1;
	} while(!DF_SONG_ID_VALID(id));

	byteAddress = 0;
	return TRUE;
}
/** Selects a dataflash song based on its id. This routine allows us to store
	the id of a song and be able to load it at a later time.
	@param id The id of the song to select
	@param currentSong  A pointer to a SongInfo structure that the name and the
			settings of the song found will be copied to
*/
BOOL DfSongsSelectSong(uint8_t id, SongInfo *currentSong)
{
	uint8_t readId;

	if (id < DF_SONGS_PAGE_FIRST || id > DF_SONGS_PAGE_LAST)
		return FALSE;

	dfSongsPageIndex = id;
	readId = DfSongsReadSongHeader(currentSong);

	if (readId == DF_SONG_ID_VOID || readId == DF_SONG_ID_DELETED)
	{
		dfSongsPageIndex = DF_SONGS_PAGE_FIRST - 1;
		return FALSE;
	}
	return TRUE;
}

/** Gets a note of the selected song. The noteNumber parameter is normally
	ignored as the routine will return the note stored in #nextNote variable.
	If the noteNumber parameter is 0 however, then instead of the #nextNote,
	the note in #firstNote will be returned instead.
	@param noteNumber The number of the note requested
	@result The note selected
*/
Note DfSongsGetNote(uint8_t noteNumber)
{
	if (noteNumber == 0)
	{
		dfSongsPlayBackState = DF_SONGS_PLAY_BACK_REPEAT;
		return firstNote;
	}

	dfSongsPlayBackState = DF_SONGS_PLAY_BACK_GET_NEXT_NOTE;
	return nextNote;
}

/** Auto-rewrites all songs in the song book. The Dataflash Song Book module
	doesn't implement an automatic mechanism to re-write a page every 10,000
	page write-erase operations in the same sector. Therefore in case of
	extensive use of the song book, the user must call this routine to maintain
	the integrity of the data. However such a heave use is not anticipated.
	@result FALSE if a song is currently being written. In this case the
		maintanance operation will not be carried out.
*/
BOOL DfSongsMaintain(void)
{
	uint8_t id;
	uint16_t page;

	if (writingSong)
		return FALSE;

	SpiInit();
	for(page = DF_SONGS_PAGE_FIRST; page <= DF_SONGS_PAGE_LAST; page++)
	{
		DataFlashContinuousReadEnable(page, 0);
		id = SpiReadWrite(0);
		if (id == DF_SONG_ID_VOID)
			break;
		DataFlashAutoPageRewrite(page, DF_SONGS_DATAFLASH_BUFFER);
	}

	SpiDisable();
	DATAFLASH_CHIP_SELECT_INACTIVE();

	dfSongsPageIndex = DF_SONGS_PAGE_FIRST - 1;
	return TRUE;
}

/** Dataflash song book task. It must be called by the main routine iteratively.
	It must be called at least once every 10ms. During a song playback, it
	access the dataflash to read the next note to be played.
*/
void DfSongsTask(void)
{
	if (!writingSong)
	{
		uint8_t byte1, byte2;

		if (dfSongsPlayBackState == DF_SONGS_PLAY_BACK_REPEAT)
		{
			byteAddress = DF_SONG_HEADER_SIZE + 2;
		}

		if (dfSongsPlayBackState != DF_SONGS_PLAY_BACK_IDLE)
		{
			SpiInit();
			byte1 = DataFlashBufferReadByte(DF_SONGS_DATAFLASH_BUFFER, byteAddress);
			byte2 = SpiReadWrite(0);
			DATAFLASH_CHIP_SELECT_INACTIVE();

			nextNote.duration = NOTE_BYTE1_GET_DURATION(byte1);
			nextNote.tone = NOTE_BYTES_GET_TONE(byte1, byte2);

			dfSongsPlayBackState = DF_SONGS_PLAY_BACK_IDLE;
			byteAddress+=2;
		}
	}
}
