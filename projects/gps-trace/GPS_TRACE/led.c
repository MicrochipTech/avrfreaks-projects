#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "led.h"

void led_init()
{
   DDRC |= (1<<PC0);
   DDRC |= (1<<PC1);
   DDRC |= (1<<PC2);
   ledstategreen = 0;
   ledstatered = 0 ;
}


void led_yellow_on()
{
	PORTC |= (1 << PC1);
}

void led_yellow_off()
{
	PORTC &= ~(1 << PC1);
}

void led_red_on()
{
	PORTC |= (1 << PC0);	
}

void led_red_off()
{
	PORTC &= ~(1 << PC0);
}

void led_green_on()
{
	PORTC |= (1 << PC2);
}

void led_green_off()
{
	PORTC &= ~(1 << PC2);
}

void led_toggle_green()
{
	if (!ledstategreen) 
	{
		led_green_on();
		ledstategreen=1;
	}
	else
	{
		led_green_off();
		ledstategreen=0;
	}
}

void led_toggle_red()
{
	if (!ledstatered) 
	{
		led_red_on();
		ledstatered=1;
	}
	else
	{
		led_red_off();
		ledstatered=0;
	}
}


