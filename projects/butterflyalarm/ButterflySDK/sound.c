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

/** @file sound.c Sound module. Provides functions for playing tones
		using the 16-bits Timer1 in PWM mode. It is used from the Button
		module (click sound) and the Songs modules.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "c_ext.h"
#include "sound.h"
#include "timer0.h"
#include "songs.h"
#include "pmSongs.h"
#include "dfSongs.h"

/*******************************************************************************
* Macros
*******************************************************************************/

#define SOUND_STATUS_SET_BEEP_START() soundState |= 1;
#define SOUND_STATUS_SET_BEEP_STOP() soundState &= ~1;
#define SOUND_STATUS_SET_SONG_START() soundState |= 2;
#define SOUND_STATUS_SET_SONG_STOP() soundState &= ~2;
#define SOUND_STATUS_BEEP_IS_ON() (soundState & 1)
#define SOUND_STATUS_SONG_IS_ON() (soundState & 2)

/*******************************************************************************
* Global Variables
*******************************************************************************/

/** Information of the song selected. There is only a single song selected at
	any given time. */
SongInfo currentSong;

/*******************************************************************************
* Static Variables
*******************************************************************************/

/** Sound module state. Bit 0 indicates whether a beep is being played. Bit 1
	indicates whether a song is being played. */
static uint8_t soundState;

/** Volume of sound */
static uint8_t soundVolume;

/** Index of the next note during song playback */
static uint8_t soundNoteIndex;

/** Tempo of song being played */
static uint8_t soundTempo;

/** Duration of note currently being played */
static uint8_t soundDuration;

/*******************************************************************************
* Functions
*******************************************************************************/

/** Initializes Sound module. */
void SoundInit(void)
{
	soundVolume = 80;
	soundState = 0;

	// Set a initial value in the OCR1A-register
	OCR1AH = 0;

	// Adjust the volume on the buzzer, lower value => higher volume
	OCR1AL = soundVolume;
}

/** Increases volume one step up. */
void SoundVolumeUp(void)
{
	if(soundVolume >= 80)
		soundVolume = 80;
	else
		soundVolume += 5;

	OCR1AH = 0;
	OCR1AL = soundVolume;
}

/** Decreases volume one step down. */
void SoundVolumeDown(void)
{
	if(soundVolume < 11)
		soundVolume = 6;
	else
		soundVolume -= 5;

	OCR1AH = 0;
	OCR1AL = soundVolume;
}

/** Enables Timer1. */
void SoundTimer1Enable(void)
{
	// Set OC1A when upcounting, clear when downcounting
	TCCR1A = (1<<COM1A1);

	// Phase/Freq-correct PWM, top value = ICR1
	TCCR1B = (1<<WGM13);

}

/** Starts playing a single tone. Does nothing if a song is being played at
	the same time. Used to produce the beep sound, when a button is pressed.
	The sound plays until #SoundStop is called. The playback of a song can't
	startm, while a tone is playing.
	@param tone The tone frequency to play
*/
void SoundStartTone(uint16_t tone)
{
	if (SOUND_STATUS_SONG_IS_ON())
		return;

	cli();

	SoundTimer1Enable();

	// Timer0 keeps the right beat
	Timer0Start();

	TCNT1H = 0;
	TCNT1L = 0;

	ICR1H = (tone >> 8);
	ICR1L = (tone & 0x00FF);

	sei();

	SOUND_STATUS_SET_BEEP_START()

	// Start Timer 1
	sbiBF(TCCR1B, CS10);
}

/** Stops the playing of a tone. */
void SoundStop(void)
{
	SOUND_STATUS_SET_BEEP_STOP();

	if (!SOUND_STATUS_SONG_IS_ON())
	{
		// Stop Timer 1
		//cbiBF(TCCR1B, CS10);
		TCCR1B = 0;
		TCCR1A = 0;

		Timer0Stop();
	}
}

/** Starts playing the current song. */
void SoundSongStart(void)
{
	cli();
	// Timer0 keeps the right beat
	Timer0Start();
	SoundTimer1Enable();
	sei();

	soundNoteIndex = 0;
	soundTempo = 0;
	soundDuration = 0;
	soundTempo = currentSong.tempo;
	SOUND_STATUS_SET_SONG_START();

	// Start Timer 1
	sbiBF(TCCR1B, CS10);
}

/** Stops song playback. */
void SoundSongStop(void)
{
	SOUND_STATUS_SET_SONG_STOP();
	if (!SOUND_STATUS_BEEP_IS_ON())
	{
		// Stop Timer 1
		cbiBF(TCCR1B, CS10);

		Timer0Stop();
	}
}

/** Tests whether a song is being played
	@result TRUE if a song is being played
*/
BOOL SoundIsSongPlaying(void)
{
	return SOUND_STATUS_SONG_IS_ON();
}

/** Tests whether a song or a tone is being played
	@result TRUE if a song or a tone is being played
*/
BOOL SoundIsOn(void)
{
	return SOUND_STATUS_SONG_IS_ON() || SOUND_STATUS_BEEP_IS_ON();
}

/** Sound Module ISR task. It must be called exactly every 10ms from the Timer0
	interrupt service routine. During song playback it changes the note being
	played.
*/
void SoundISRTask(void)
{
	if (!SOUND_STATUS_BEEP_IS_ON())
	{
		if (SOUND_STATUS_SONG_IS_ON())
		{
			if(!soundTempo)
			{
				// Check if the length of the tone has "expired"
				if(soundDuration)
				{
					soundDuration--;
				}
				else if (soundNoteIndex < currentSong.size)
				{
					Note note;

					if (currentSong.type == SONG_PROGMEM_TYPE)
						note = SongGetNote(soundNoteIndex);
					else
						note = DfSongsGetNote(soundNoteIndex);

					soundDuration = note.duration;

					if (note.tone == P)
						cbiBF(TCCR1B, CS10);
					else
						sbiBF(TCCR1B, CS10);

					cli();
					TCNT1H = 0;
					TCNT1L = 0;
					ICR1H = (note.tone >> 8);
					ICR1L = (note.tone & 0x00FF);
					sei();

					soundNoteIndex++;
					if (soundNoteIndex == currentSong.size && currentSong.repeat)
						soundNoteIndex = 0;
				}
				else
				{
					// Would only get here if the song must not repeat
					SOUND_STATUS_SET_SONG_STOP();
					if (!SOUND_STATUS_BEEP_IS_ON())
					{
						cbiBF(TCCR1B, CS10);
					}
				}
				soundTempo = currentSong.tempo;
			}
			else
			{
				soundTempo--;
			}
		}
	}
}

