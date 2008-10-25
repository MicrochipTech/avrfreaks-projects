#include "song.h"

static note_t entertainer_notes[] = {
	{D3, 250},
	{E3, 250},
	{C3, 250}, 
	{A3, 500}, 
	{B3, 250}, 
	{G2, 250}, 
	{REST, 250},
	{D3, 250},
	{E3, 250},
	{C3, 250}, 
	{A3, 500}, 
	{B3, 250}, 
	{G2, 250}, 
	{REST, 250},	//separated every line of original music
	
	{D2, 250}, 
	{E2, 250}, 
	{C2, 250}, 
	{A2, 500},
	{B2, 250}, 
	{A2, 250}, 
	{G1S, 250}, 
	{G1, 250},
	{B3, 250}, //this is a funky 3 notes at once thing (B3, F3, and G3).
	{REST, 250}, 
	{D2, 250}, 
	{D2S, 250}, 
	
	{E2, 250}, 
	{C3, 500}, 
	{E2, 250}, 
	{C3, 500}, 
	{E2, 250}, 
	{C3, 1250},
	{REST, 250}, 
	{C3, 250}, 
	{D3, 250}, 
	{D3S, 250}, 
	
	{E3, 250}, 
	{C3, 250}, 
	{D3, 250}, 
	{E3, 500},
	{B3, 250}, 
	{D3, 500},
	{C3, 1000},
	{REST, 500},
	{D2, 250}, 
	{D2S, 250}, 
	{E2, 250}, 
	{C3, 500},
	{E2, 250}, 
	
	{C3, 500},
	{E2, 250}, 
	{C3, 1250},
	{REST, 500},
	{A3, 250}, 
	{G2, 250}, 
	{F2S, 250}, 
	{A3, 250}, 
	{C3, 250}, 
	{E3, 500}, //carries onto start of next line
	
	{D3, 250}, 
	{C3, 250}, 
	{A3, 250}, 
	{D3, 1000},
	{REST, 500},
	{D2, 250},
	{D2S, 250},
	{E2, 250}, 
	{C3, 500},
	{E2, 250},
	{C3, 500},
	{E2, 250},
	{C3, 1250},	//carries
	
	{REST, 250},
	{C3, 250},
	{D3, 250},
	{D3S, 250},
	{E3, 250},
	{C3, 250},
	{D3, 250},
	{E3, 500},
	{B3, 250},
	{D3, 500},
	
	{C3, 1000},
	{REST, 500},
	{C3, 250},
	{D3, 250},
	{E3, 250},
	{C3, 250},
	{D3, 250},
	{E3, 500},
	{C3, 250},
	{D3, 250},
	{C3, 250},
	{E3, 250},
	{C3, 250},
	{D3, 250},
	{E3, 500}, //carry
	
	{C3, 250},
	{D3, 250},
	{C3, 250},
	{E3, 250},
	{C3, 250},
	{D3, 250},
	{E3, 500},
	{B3, 250},
	{D3, 500},
	{C3, 250},
	{REST, 7000},
};


song_t song = {
	.note_delay = 100,
	.repeat_song = 0,
	.notes = entertainer_notes,
	.length = sizeof(entertainer_notes) / sizeof(note_t),
};


