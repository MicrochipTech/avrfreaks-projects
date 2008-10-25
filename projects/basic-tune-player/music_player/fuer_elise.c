#include <avr/io.h>

#include "song.h"


//typedef struct {
//	uint16_t freq;	//measured in Hz
//	uint16_t time;	//measured in ms
//} note_t;
static note_t fuer_elise_notes[] = {
	{E2, 250},
	{D2S, 250},
	{E2, 250},
	{D2S, 250},
	{E2, 250},
	{B2, 250},
	{D2, 250},
	{C2, 250},
	{A2, 500},
	{REST, 250},
	{C1, 250},
	{E1, 250},
	{A2, 250},
	{B2, 500},
	{REST, 250},
	{E1, 250},
	{G1S, 250},
	{B2, 250},
	{C2, 500},
};


song_t song = {
	.note_delay = 0,
	.repeat_song = 0,
	.notes = fuer_elise_notes,
	.length = sizeof(fuer_elise_notes) / sizeof(note_t),
};




/*
static note_t fuer_elise_notes[] = {
	{330, 250},
	{311, 250},
	{330, 250},
	{311, 250},
	{330, 250},
	{247, 250},
	{294, 250},
	{262, 250},
	{220, 500},
	{360, 250},
	{131, 250},
	{165, 250},
	{220, 250},
	{247, 500},
	{360, 250},
	{165, 250},
	{208, 250},
	{247, 250},
	{263, 500},
};
*/
