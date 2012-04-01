/*
 * main.c
 * Rotate
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
	avr_servo_init();
	avr_servo_attach(TIMER1A);
	
	int16_t pos = 0;
	int8_t step = 64;
	while (1)
	{
		avr_servo_set_pos(TIMER1A, pos);
		pos += step;
		if (pos > 1024 || pos < 16)
		{
			step *= -1;
		}
		
		avr_delay_ms(200);
	}
	
	return 0;
}

