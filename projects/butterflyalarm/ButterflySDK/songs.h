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

/** @file songs.h Definition of butterfly song format. Used by both the program memory
		and dataflash song modules.
*/

#ifndef __SONGS_HEADER__
#define __SONGS_HEADER__

/*******************************************************************************
* Definitions
*******************************************************************************/

/** @def SONG_NAME_SIZE_MAX Maximum size of song name */
#define SONG_NAME_SIZE_MAX	12

/** @def SONG_PROGMEM_TYPE Id indicating a song in program memory */
#define SONG_PROGMEM_TYPE	0

/** @def SONG_DATAFLASH_TYPE Id indicating a song in dataflash */
#define SONG_DATAFLASH_TYPE	1

/*******************************************************************************
* Structures
*******************************************************************************/

/** Song information structure */
typedef struct
{
	/** Song's name string */
	char name[SONG_NAME_SIZE_MAX + 1];

	/** Song's tempo. It is defined in steps of 10ms. */
	uint8_t tempo;

	/** Set to 1 if the song must be repeated when all the notes have been
		played */
	uint8_t repeat;

	/** Size of the song. This is the number of notes included in the song
	body. Two bytes are needed for every note. There is a 120 note limit. */
	uint8_t size;

	/** Type of the song. Either #SONG_PROGMEM_TYPE or #SONG_DATAFLASH_TYPE */
	uint8_t type;
} SongInfo;

/** Represents a musical note. A note is defined by its frequency (tone) and
	its duration. The allowed tones are defined in sound.h file. The duration
	could be 1, 1/2, 1/4, 1/8, 1/16, 1/32. Tone definitions in sound.h,
	duration definitions and #NOTE definition in this file, facilitate
	the song authoring procedure. */
typedef struct
{
	/** The duration of the note*/
	uint8_t duration;

	/** The tone of the note*/
	uint16_t tone;
} Note;

/*******************************************************************************
* NOTE Representation
*
* 15 14 13 12  11  10  9  8  7  6  5  4  3  2  1  0
*
* d2 d1 d0 t12 t11 t10 t9 t8 t7 t6 t5 t4 t3 t2 t1 t0
*
* Duration (d2:d0) Values 0..6 -> 1, 2, 4, 8, 16, 32
*
* Tone (t12:t0) Values 0..8191
*
*******************************************************************************/

#define	D_1		0xA0 // 1/32
#define	D_2		0x80 // 1/16
#define	D_4		0x60 // 1/8
#define	D_8		0x40 // 1/4
#define	D_16	0x20 // 1/2
#define	D_32	0x00 // 1

/*******************************************************************************
* Macros
*******************************************************************************/

/** @def NOTE(duration, tone)
	Creates a word (2-bytes) with the duration and tone information of a note.
	It serves in the creation of a compact scheme for storing notes. The
	ordering of bits is as follows:

	<table align="middle" border=1>
	<tr align="middle"><th>15</th><th>14</th><th>13</th><th>12</th><th>11</th>
		<th>10</th><th>9</th><th>8</th><th>7</th><th>6</th><th>5</th><th>4</th>
		<th>3</th><th>2</th><th>1</th><th>0</th></tr>
	<tr align="middle"><td>d2</td><td>d1</td><td>d0</td><td>t12</td><td>t11</td>
		<td>t10</td><td>t9</td><td>t8</td><td>t7</td><td>t6</td><td>t5</td>
		<td>t4</td><td>t3</td><td>t2</td><td>t1</td><td>t0</td></tr>
	</table>

	The 3 most significant bits define the duration. Allowed values are
	in the range 0..6, which correspond to durations 1, 1/2, 1/4, 1/8, 1/16,
	1/32 accordingly. The rest 13 bits define the tone, as the value written
	in the 16-bit register of the PWM timer. Definitions in sound.h provide
	a musical note to PWM value conversion.
*/
#define NOTE(duration, tone) (duration) | ((tone >> 8) & 0x1F) , (tone & 0xFF)

/** @def NOTE_BYTE1_GET_DURATION Retrieves the duration from the most
	significant byte of a note stored in program memory or in dataflash */
#define NOTE_BYTE1_GET_DURATION(byte1)	1 << ((byte1) >> 5)

/** @def NOTE_BYTES_GET_TONE Retrieves the tone from the note bytes
	stored in program memory or in dataflash */
#define NOTE_BYTES_GET_TONE(byte1, byte2) (((uint16_t) (byte1 & 0x1F)) << 8) \
										  | ((uint16_t) (byte2))

/** @def SONG_SETTINGS Bundles the settings of a song (tempo, repeat, size)
	in two bytes */
#define SONG_SETTINGS(tempo, repeat, size) (((tempo) << 4) | ((repeat) & 1)), \
										   (size)

/** @def SETTINGS_B1_GET_TEMPO Extracts the song's tempo from the settings
	bundled	bytes */
#define SETTINGS_B1_GET_TEMPO(settingsB1)	(((settingsB1) >> 4) & 0x0F)

/** @def SETTINGS_B1_GET_REPEAT Extracts the song's repeat value from the
	settings bundled bytes */
#define SETTINGS_B1_GET_REPEAT(settingsB1) 	((settingsB1) & 1)

/** @def SETTINGS_B0_GET_SIZE Extracts the song's size from the settings
	bundled	bytes */
#define SETTINGS_B0_GET_SIZE(settingsB0) 	(settingsB0)

/** @def SONGS_HEADER_BYTE_SIZE The size of the song's header in bytes */
#define SONGS_HEADER_BYTE_SIZE	SONG_NAME_SIZE_MAX + 2

#endif // __SONGS_HEADER__
