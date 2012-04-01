/*
 * main.c
 * Beep
 *
 * Created on 16/02/2012.
 * by H2CO3
 * License: CreativeCommons Attribution-ShareAlike Unported 3.0 License
 */

#include <stdlib.h>
#include <math.h>
#include <avrutil/avrutil.h>

int main()
{
	avr_bit_set(DDRB, PIN1);
	
	while (1)
	{
		avr_sound_start(440);
		avr_delay_ms(500);
		avr_sound_stop();
		avr_delay_ms(500);
	}
	
	return 0;
}

