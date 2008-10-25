/***************************************************************************************************************************
 * <tao570@uow.edu.au> wrote this file. As long as you retain this notice you can do whatever you want with this stuff.
 * If we meet some day, and you think this stuff is worth it, you can buy me a beer in return,
 * Tev Olsen
 * 
 ****************************************************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "song.h"
#include "setup.h"


//Uses timer1, OCR1B for output.	//ICR1 is used for the TOP value - which determines the frequency. OCR1B should be set to half of ICR1 at most times.
//TIMER0 is used for timing


void timing_init(void) {
	TCCR0 = (1 << WGM01) | (1 << CS01);
	OCR0 = 125;
	TIMSK |= (1 << OCIE0);
}

void sound_init(void) {
	TCCR1A = (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);	//clear output on match, set on timer reset. WGM is part of setting it to fast PWM using ICR1 as TOP.
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);	//second part of fast PWM mode. 
	DDRD = (1 << 4);	//sets the pin to output.
	
}



//returns how long the note should be played for. 
uint16_t start_note(note_t note) {
	uint16_t period = (F_CPU / note.freq);// >> prescaler_bits;//(((uint32_t)frequency << 16) / F_CPU) >> prescaler_bits;
	
	OCR1B = period/2;//the actual output. Designed to have 50% duty cycle for simplicity. 

	OCR1A = period;	//used for determining the period.
	
	return note.time;
}



void music_1ms(void) {
	static uint16_t time_remaining = 0;//the amount of time remaining for the current note to play (in milliseconds)
	static uint16_t note_index = 0;//this is the array index for the note. 

	//this should be fairly self explanatory. time_remaining is measured in milliseconds
	if(time_remaining == 0) {
		if(note_index >= song.length) {	//tests if the note index is >= the number of notes in the song.
			if(song.repeat_song) {		//you can set the song to repeat.
				note_t temp_note = {REST, 5000};//if it does repeat, a 5 second delay is inserted. 
				time_remaining = start_note(temp_note);
				note_index = 0;	//this is all that's required to resart the song.
			} else {
				ICR1 = 0xffff;	//if the song isn't to repeat, need to create duty cycle of either 0 or 100%, in this case 0%
				OCR1B = 0;
				return;		//stops the song once it's finished.	- basically means that music_1ms won't actually do anything. 
			}
		}
		time_remaining = start_note(song.notes[note_index++]);	//self-expanatory. starts note, setting the time remaining to the length of the note.
	} else if(time_remaining <= song.note_delay) {	
		OCR1B = OCR1A;//this performs the delay between notes. It does so by cutting notes short. 
		time_remaining--;
	} else {
		time_remaining--;
	}

}



ISR(TIMER0_COMP_vect)
{
	music_1ms();
}


int main(void) {
	
	sound_init();
	timing_init();
	
	sei();	

	for(;;) {
		;
	}

	
	return 0;
}



