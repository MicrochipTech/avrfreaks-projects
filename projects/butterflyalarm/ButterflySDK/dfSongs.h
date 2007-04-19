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

/** @file dfSongs.h Dataflash Song Book module header file.
*/

#ifndef __DATAFLASH_SONGS_HEADER__
#define __DATAFLASH_SONGS_HEADER__

#include "songs.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

#define DF_SONG_ID_VOID			0xFF
#define DF_SONG_ID_DELETED		0xFE
#define DF_SONG_ID_VALID(id)	((id) < 250)

#define DF_SONG_MAX_SIZE		120		// Notes = 240 bytes

/*******************************************************************************
* Exported Functions
*******************************************************************************/

extern void DfSongsInit(void);
extern BOOL DfSongsSelectNextSong(SongInfo *currentSong, BOOL moveForward);
extern BOOL DfSongsSelectSong(uint8_t id, SongInfo *currentSong);
extern Note DfSongsGetNote(uint8_t noteNumber);
extern uint8_t DfSongsGetSelectedSongId(void);
extern BOOL DfSongDelete(uint8_t id);
extern BOOL DfSongsWriteSongStart(void);
extern void DfSongsWriteSongInsertByte(uint16_t pos, uint8_t byte);
extern void DfSongsWriteSongFinish(void);
extern BOOL DfSongsMaintain(void);

extern void DfSongsTask(void);

#endif // __DATAFLASH_SONGS_HEADER__
