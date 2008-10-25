#include <avr/io.h>

#include "song.h"


//typedef struct {
//	uint16_t freq;	//measured in Hz
//	uint16_t time;	//measured in ms
//} note_t;


static note_t happy_birthday_notes[] = {
	{262, 500},
	{262, 500},
	{294, 1000},
	{262, 1000},
	{349, 1000},
	{330, 2000},
	{262, 500},
	{262, 500},
	{294, 1000},
	{262, 1000},
	{392, 1000},
	{349, 2000},
	{262, 500},
	{262, 500},
	{523, 1000},
	{440, 1000},
	{349, 1000},
	{330, 1000},
	{294, 3000}, 
	{466, 500},
	{466, 500},
	{440, 1000},
	{349, 1000},
	{392, 1000},
	{349, 1000},
};



song_t song = {
	.note_delay = 50,
	.repeat_song = 0,
	.notes = happy_birthday_notes,
	.length = sizeof(happy_birthday_notes) / sizeof(note_t),
};
