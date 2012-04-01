/*
 * delay.c
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include "delay.h"

void avr_delay_ms(uint32_t ms)
{
	while (ms >= 1000)
	{
		_delay_ms(1000);
		ms -= 1000;
	}
	while (ms >= 100)
	{
		_delay_ms(100);
		ms -= 100;
	}
	while (ms >= 10)
	{
		_delay_ms(10);
		ms -= 10;
	}
	while (ms)
	{
		_delay_ms(1);
		ms--;
	}
}

