#ifndef SONG_H
#define SONG_H
/***********************************************************
 * This file, and everything in it were written by Tev Olsen. 
 * I give permission for anyone to use it for any purpose.
 * <insert beerware licence here>
 **********************************************************/

#include <stdint.h>



typedef struct {
	uint16_t freq;	//measured in Hz
	uint16_t time;	//measured in ms (duration)
} note_t;

typedef struct {
	uint16_t note_delay;	//The pause between notes. This pause is done by cutting notes short (to prevent the timing of notes being wrong)(milliseconds)
	uint8_t repeat_song;	//this is a boolean of whether or not to repeat the song after it's finished. 
	note_t *notes;			//the array of notes in the song.
	uint16_t length;		//the number of notes. (calculate by sizeof(array)/sizeof(array[0]))
} song_t;

extern song_t song;	//This global variable is to allow music_player to 'see' the song which is to be created in it's own .c file. 
//by making this global, you can change which song is to be played simply by changing which <song>.c file is compiled in.

//These are defines to make it easier to set write up the song. C2 is middle C, C1 is one octave lower and C3 is one octave higher. 
//C2S is middle C sharp. 
#define A1 110
#define A1S 117
#define B1 124
#define C1 131
#define C1S 139
#define D1 147
#define D1S 156
#define E1 165
#define F1 175
#define F1S 185
#define G1 196
#define G1S 208
#define A2 220
#define A2S 233
#define B2 247
#define C2 262
#define C2S 277
#define D2 294
#define D2S 311
#define E2 330
#define F2 349
#define F2S 370
#define G2 392
#define G2S 415
#define A3 440
#define A3S 466
#define B3 494
#define C3 523
#define C3S 554
#define D3 587
#define D3S 622
#define E3 659
#define F3 698
#define F3S 740
#define G3 784
#define G3S 830

#define REST 10	//a rest is done using a frequency outside human hearing range.
#endif	// #ifndef SONG_H
