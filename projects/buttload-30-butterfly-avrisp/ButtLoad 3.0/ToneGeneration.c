/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#include "ToneGeneration.h"

/* Tone values are compressed to save on space at the expense of resolution.
   When played, the tone value is expanded to 16-bit and shifted left 5 times
   to give 8 bits of resolution. As the exact frequency of the tones isn't
   important, this is an acceptable tradeoff. Lower values = higher tone. Note
   that the tone generation is syncronous; program execution halts while the
   tone plays.                                                                 */

                                        // Tone1,.......,ToneN, 0x00
const uint8_t ToneSeq_Startup[]  PROGMEM = {156, 125,  94,  63, 0x00};
const uint8_t ToneSeq_SyncFail[] PROGMEM = { 94, 156,           0x00};
const uint8_t ToneSeq_ProgDone[] PROGMEM = {156,  94,           0x00};
const uint8_t ToneSeq_ProgFail[] PROGMEM = {156, 171, 203,      0x00};
const uint8_t ToneSeq_VolTest[]  PROGMEM = {156,                0x00};
const uint8_t ToneSeq_Sleep[]    PROGMEM = { 94, 125, 156,      0x00};
const uint8_t ToneSeq_Resume[]   PROGMEM = {156, 125,  94,      0x00};
const uint8_t ToneSeq_Error[]    PROGMEM = {200, 220,           0x00};

uint8_t ToneVol = 0;

// ======================================================================================

/*
 NAME:      | TG_PlayToneSeq
 PURPOSE:   | Plays the passed tone sequence out of the Butterfly's piezo speaker
 ARGUMENTS: | Pointer to the desired tone sequence array
 RETURNS:   | None
*/
void TG_PlayToneSeq(const uint8_t* Sequence)
{
	if (!(ToneVol))                               // If no volume (off), skip tone playing
	  return;
	else if (ToneVol > 80)
	  ToneVol = 80;

	OCR1A  = ToneVol;                             // Set the tone volume via the global register
	TCCR1A = (1 << COM1A1);                       // OC1A set when counting up, cleared when counting down
	TCCR1B = ((1 << WGM13) | (1 << CS10));        // Phase/Freq correct PWM mode, turn on with prescale of 1
	
	for (;;)
	{	
		uint8_t Tone = pgm_read_byte(Sequence++); // Fetch the next tone from the table

		TCNT1 = 0;
		ICR1  = ((uint16_t)Tone << 5);            // Set the ICR register - play the tone

		if (!(Tone))                              // Having the end tone check here ensures that ICR1 and TCNT1 are cleared on exit due to 0x00 terminator
		  break;

		MAIN_Delay10MS(15);                       // Delay between tones - 150ms
	}

	TCCR1A = 0;
	TCCR1B = 0;
}
