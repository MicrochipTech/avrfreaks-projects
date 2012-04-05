/*
 * Tone.cpp
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include "Tone.h"

avrutil::Tone::Tone(uint16_t freq)
{
	this->freq = freq;
}

void avrutil::Tone::start()
{
	avr_sound_start(this->freq);
}

void avrutil::Tone::stop()
{
	avr_sound_stop();
}

