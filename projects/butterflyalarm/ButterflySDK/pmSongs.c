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

/** @file pmSongs.c Program Memory (Flash) Songs Module. It contains the
		definitions of program memory songs and routines for playing them
		back.
*/

#include <inttypes.h>
#include <avr/pgmspace.h>

#include "c_ext.h"
#include "sound.h"
#include "songs.h"

/*******************************************************************************
* Songs
*******************************************************************************/

/** Fur Elise song */
const uint8_t FurElise[] PROGMEM =
{
	'F', 'u', 'r' , ' ', 'E', 'l', 'i' ,'s', 'e', '\0', '\0', '\0',
	SONG_SETTINGS(3, 0, 40),

	NOTE(D_8,e2 ), NOTE(D_8,xd2), NOTE(D_8,e2 ), NOTE(D_8,xd2), NOTE(D_8,e2 ),
	NOTE(D_8,b1 ), NOTE(D_8,d2 ), NOTE(D_8,c2 ), NOTE(D_4,a1 ), NOTE(D_8,p  ),
	NOTE(D_8,c1 ), NOTE(D_8,e1 ), NOTE(D_8,a1 ), NOTE(D_4,b1 ), NOTE(D_8,p  ),
	NOTE(D_8,e1 ), NOTE(D_8,xg1), NOTE(D_8,b1 ), NOTE(D_4,c2 ), NOTE(D_8,p  ),
	NOTE(D_8,e1 ), NOTE(D_8,e2 ), NOTE(D_8,xd2), NOTE(D_8,e2 ), NOTE(D_8,xd2),
	NOTE(D_8,e2 ), NOTE(D_8,b1 ), NOTE(D_8,d2 ), NOTE(D_8,c2 ), NOTE(D_4,a1 ),
	NOTE(D_8,p  ), NOTE(D_8,c1 ), NOTE(D_8,e1 ), NOTE(D_8,a1 ), NOTE(D_4,b1 ),
	NOTE(D_8,p  ), NOTE(D_8,e1 ), NOTE(D_8,c2 ), NOTE(D_8,b1 ), NOTE(D_4,a1 ),
};

/** Turkey March song */
const uint8_t Mozart[] PROGMEM =
{
	'T', 'u', 'r', 'k', 'e', 'y', ' ', 'm', 'a', 'r', 'c', 'h',
	SONG_SETTINGS(3, 1, 44),

	NOTE(D_16,xf1), NOTE(D_16,e1),  NOTE(D_16,xd1), NOTE(D_16,e1),  NOTE(D_4,g1),
	NOTE(D_16,a1),  NOTE(D_16,g1),  NOTE(D_16,xf1), NOTE(D_16,g1),  NOTE(D_4,b1),
	NOTE(D_16,c2),  NOTE(D_16,b1),  NOTE(D_16,xa1), NOTE(D_16,b1),  NOTE(D_16,xf2),
	NOTE(D_16,e2),  NOTE(D_16,xd2), NOTE(D_16,e2),  NOTE(D_16,xf2), NOTE(D_16,e2),
	NOTE(D_16,xd2), NOTE(D_16,e2),  NOTE(D_4,g2),   NOTE(D_8,e2),   NOTE(D_8,g2),
	NOTE(D_32,d2),  NOTE(D_32,e2),  NOTE(D_16,xf2), NOTE(D_8,e2),   NOTE(D_8,d2),
	NOTE(D_8,e2),   NOTE(D_32,d2),  NOTE(D_32,e2),  NOTE(D_16,xf2), NOTE(D_8,e2),
	NOTE(D_8,d2),   NOTE(D_8,e2),   NOTE(D_32,d2),  NOTE(D_32,e2),  NOTE(D_16,xf2),
	NOTE(D_8,e2),   NOTE(D_8,d2),   NOTE(D_8,xc2),  NOTE(D_4,b1)
};

#ifdef __USE_ALL_PROGRAM_MEMORY_SONGS__

/** Minuet song */
const uint8_t Minuet[] PROGMEM =
{
	'M', 'i', 'n', 'u', 'e', 't', '\0', '\0', '\0', '\0', '\0', '\0',
	SONG_SETTINGS(2, 1, 33),

	NOTE(D_4,d2), NOTE(D_8,g1),  NOTE(D_8,a1),  NOTE(D_8,b1), NOTE(D_8,c2),
	NOTE(D_4,d2), NOTE(D_4,g1),  NOTE(D_4,g1),  NOTE(D_4,e2), NOTE(D_8,c2),
	NOTE(D_8,d2), NOTE(D_8,e2),  NOTE(D_8,xf2), NOTE(D_4,g2), NOTE(D_4,g1),
	NOTE(D_4,g1), NOTE(D_4,c2),  NOTE(D_8,d2),  NOTE(D_8,c2), NOTE(D_8,b1),
	NOTE(D_8,a1), NOTE(D_4,b1),  NOTE(D_8,c2),  NOTE(D_8,b1), NOTE(D_8,a1),
	NOTE(D_8,g1), NOTE(D_4,xf1), NOTE(D_8,g1),  NOTE(D_8,a1), NOTE(D_8,b1),
	NOTE(D_8,g1), NOTE(D_4,b1),  NOTE(D_2,a1)
};

/** Auld Lang Syne song */
const uint8_t AuldLangSyne[] PROGMEM =
{
	'A', 'u', 'l', 'd', 'L', 'a', 'n', 'g', 'S' ,'y', 'n', 'e',
	SONG_SETTINGS(3, 1, 32),

	NOTE(D_4,g2), NOTE(D_2,c3), NOTE(D_8,c3), NOTE(D_4,c3), NOTE(D_4,e3),
	NOTE(D_2,d3), NOTE(D_8,c3), NOTE(D_4,d3), NOTE(D_8,e3), NOTE(D_8,d3),
	NOTE(D_2,c3), NOTE(D_8,c3), NOTE(D_4,e3), NOTE(D_4,g3), NOTE(D_2,a3),
	NOTE(D_8,p),  NOTE(D_4,a3), NOTE(D_2,g3), NOTE(D_8,e3), NOTE(D_4,e3),
	NOTE(D_4,c3), NOTE(D_2,d3), NOTE(D_8,c3), NOTE(D_4,d3), NOTE(D_8,e3),
	NOTE(D_8,d3), NOTE(D_2,c3), NOTE(D_8,a2), NOTE(D_4,a2), NOTE(D_4,g2),
	NOTE(D_2,c3), NOTE(D_4,p)
};

/** Sirene 1 */
const uint8_t Sirene1[] PROGMEM =
{
	'S', 'i', 'r', 'e', 'n', 'e', '1', '\0', '\0', '\0', '\0', '\0',
	SONG_SETTINGS(0, 1,	70),

	NOTE(D_32,400), NOTE(D_32,397), NOTE(D_32,394), NOTE(D_32,391), NOTE(D_32,388),
	NOTE(D_32,385), NOTE(D_32,382), NOTE(D_32,379), NOTE(D_32,376), NOTE(D_32,373),
	NOTE(D_32,370), NOTE(D_32,367), NOTE(D_32,364), NOTE(D_32,361), NOTE(D_32,358),
	NOTE(D_32,355), NOTE(D_32,352), NOTE(D_32,349), NOTE(D_32,346), NOTE(D_32,343),
	NOTE(D_32,340), NOTE(D_32,337), NOTE(D_32,334), NOTE(D_32,331), NOTE(D_32,328),
	NOTE(D_32,325), NOTE(D_32,322), NOTE(D_32,319), NOTE(D_32,316), NOTE(D_32,313),
	NOTE(D_32,310), NOTE(D_32,307), NOTE(D_32,304), NOTE(D_32,301), NOTE(D_32,298),
	NOTE(D_32,298), NOTE(D_32,301), NOTE(D_32,304), NOTE(D_32,307), NOTE(D_32,310),
	NOTE(D_32,313), NOTE(D_32,316), NOTE(D_32,319), NOTE(D_32,322), NOTE(D_32,325),
	NOTE(D_32,328), NOTE(D_32,331), NOTE(D_32,334), NOTE(D_32,337), NOTE(D_32,340),
	NOTE(D_32,343), NOTE(D_32,346), NOTE(D_32,349), NOTE(D_32,352), NOTE(D_32,355),
	NOTE(D_32,358), NOTE(D_32,361), NOTE(D_32,364), NOTE(D_32,367), NOTE(D_32,370),
	NOTE(D_32,373), NOTE(D_32,376), NOTE(D_32,379), NOTE(D_32,382), NOTE(D_32,385),
	NOTE(D_32,388), NOTE(D_32,391), NOTE(D_32,394), NOTE(D_32,397), NOTE(D_32,400)
};

/** Sirene 2 */
const uint8_t Sirene2[] PROGMEM =
{
	'S', 'i', 'r', 'e', 'n', 'e', '2', '\0', '\0', '\0', '\0', '\0',
	SONG_SETTINGS(3, 1, 2),

	NOTE(D_4,c2), NOTE(D_4,g2),
};

/** Whistle */
const uint8_t Whistle[] PROGMEM =
{
	'W', 'h', 'i', 's', 't', 'l', 'e', '\0', '\0', '\0', '\0', '\0',
	SONG_SETTINGS(0, 0, 54),

	NOTE(D_32,200), NOTE(D_32,195), NOTE(D_32,190), NOTE(D_32,185), NOTE(D_32,180),
	NOTE(D_32,175), NOTE(D_32,170), NOTE(D_32,165), NOTE(D_32,160), NOTE(D_32,155),
	NOTE(D_32,150), NOTE(D_32,145), NOTE(D_32,140), NOTE(D_32,135), NOTE(D_32,130),
	NOTE(D_32,125), NOTE(D_32,120), NOTE(D_32,115), NOTE(D_32,110), NOTE(D_32,105),
	NOTE(D_32,100), NOTE(D_8 ,p  ), NOTE(D_32,200), NOTE(D_32,195), NOTE(D_32,190),
	NOTE(D_32,185), NOTE(D_32,180), NOTE(D_32,175), NOTE(D_32,170), NOTE(D_32,165),
	NOTE(D_32,160), NOTE(D_32,155), NOTE(D_32,150), NOTE(D_32,145), NOTE(D_32,140),
	NOTE(D_32,135), NOTE(D_32,130), NOTE(D_32,125), NOTE(D_32,125), NOTE(D_32,130),
	NOTE(D_32,135), NOTE(D_32,140), NOTE(D_32,145), NOTE(D_32,150), NOTE(D_32,155),
	NOTE(D_32,160), NOTE(D_32,165), NOTE(D_32,170), NOTE(D_32,175), NOTE(D_32,180),
	NOTE(D_32,185), NOTE(D_32,190), NOTE(D_32,195), NOTE(D_32,200)
};

/** @def SONGS_COUNT Count of songs in program memory */
#define SONGS_COUNT	7

/** Table with all songs in program memory */
const uint8_t *songs_F[SONGS_COUNT] PROGMEM = {FurElise,
											   Mozart,
											   Minuet,
											   AuldLangSyne,
											   Sirene1,
											   Sirene2,
											   Whistle};
#else
/** @def SONGS_COUNT Count of songs in program memory */
#define SONGS_COUNT	2

/** Table with all songs in program memory */
const uint8_t *songs_F[SONGS_COUNT] PROGMEM = {FurElise,
											   Mozart};
#endif // __USE_ALL_PROGRAM_MEMORY_SONGS__

/*******************************************************************************
* Static Variables
*******************************************************************************/

/** Address of current song in program memory */
static const uint8_t *songNumber;

/*******************************************************************************
* Functions
*******************************************************************************/

/** Returns the number of songs in program memory
	@result The number of songs in program memory
*/
uint8_t SongGetCount(void)
{
	return SONGS_COUNT;
}

/** Selects a program memory song
	@param number The number of song to select
	@param currentSong A pointer to a SongInfo structute that will hold the
		song's information
*/
BOOL SongSelect(uint8_t number, SongInfo *currentSong)
{
	uint8_t i , byte;

	if (number >= SONGS_COUNT)
	{
		return FALSE;
	}

	songNumber = (uint8_t*) pgm_read_word(&songs_F[number]);

	for(i=0; i<SONG_NAME_SIZE_MAX; i++)
	{
		currentSong->name[i] = pgm_read_byte(songNumber + i);
	}
	currentSong->name[SONG_NAME_SIZE_MAX] = '\0';

	byte = pgm_read_byte(songNumber + i);
	currentSong->tempo = SETTINGS_B1_GET_TEMPO(byte);
	currentSong->repeat = SETTINGS_B1_GET_REPEAT(byte);
	i++;
	currentSong->size = SETTINGS_B0_GET_SIZE(pgm_read_byte(songNumber + i));
	currentSong->type = SONG_PROGMEM_TYPE;

	return TRUE;
}

/** Gets a note from the selected song
	@param noteNumber The number of the note requested
	@result The note selected
*/
Note SongGetNote(uint8_t noteNumber)
{
	uint8_t byte1, byte2;
	Note note;

	noteNumber = noteNumber << 1; // *2
	byte1 = pgm_read_byte(songNumber + SONGS_HEADER_BYTE_SIZE + noteNumber);
	byte2 = pgm_read_byte(songNumber + SONGS_HEADER_BYTE_SIZE + noteNumber + 1);

	note.duration = NOTE_BYTE1_GET_DURATION(byte1);
	note.tone = NOTE_BYTES_GET_TONE(byte1, byte2);

	return note;
}
