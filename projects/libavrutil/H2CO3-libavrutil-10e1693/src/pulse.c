/*
 * pulse.c
 * libavrutil
 *
 * Created by Árpád Goretity on 26/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <util/delay.h>
#include "delay.h"
#include "pulse.h"


void avr_pulse_generate_us(volatile uint8_t *port, uint8_t pin, uint16_t us)
{
	avr_bit_set(*port, pin);
	avr_delay_us(us);
	avr_bit_clear(*port, pin);
}

void avr_pulse_generate_ms(volatile uint8_t *port, uint8_t pin, uint32_t ms)
{
	avr_bit_set(*port, pin);
	avr_delay_ms(ms);
	avr_bit_clear(*port, pin);
}

void avr_pulse_generate_inv_us(volatile uint8_t *port, uint8_t pin, uint16_t us)
{
	avr_bit_clear(*port, pin);
	avr_delay_us(us);
	avr_bit_set(*port, pin);
}

void avr_pulse_generate_inv_ms(volatile uint8_t *port, uint8_t pin, uint32_t ms)
{
	avr_bit_clear(*port, pin);
	avr_delay_ms(ms);
	avr_bit_set(*port, pin);
}

uint32_t avr_pulse_measure_us(volatile uint8_t *port, uint8_t pin)
{
	uint32_t us = 0;

	/* Wait for the pin to go low (to previous pulse to end, if any) */
	loop_until_bit_is_clear(*port, pin);

	/* Then wait for it to go high */
	loop_until_bit_is_set(*port, pin);

	/* Start counting */
	while (avr_bit_isset(*port, pin))
	{
		us++;
		_delay_us(1);
	}
	
	return us;
}

uint32_t avr_pulse_measure_ms(volatile uint8_t *port, uint8_t pin)
{
	uint32_t ms = 0;

	/* Wait for the pin to go low (to previous pulse to end, if any) */
	loop_until_bit_is_clear(*port, pin);

	/* Then wait for it to go high */
	loop_until_bit_is_set(*port, pin);
	
	/* Start counting */
	while (avr_bit_isset(*port, pin))
	{
		ms++;
		_delay_ms(1);
	}
	
	return ms;
}

uint32_t avr_pulse_measure_inv_us(volatile uint8_t *port, uint8_t pin)
{
	uint32_t us = 0;

	/* Wait for the pin to go high (to previous pulse to end, if any) */
	loop_until_bit_is_set(*port, pin);

	/* Then wait for it to go low */
	loop_until_bit_is_clear(*port, pin);
	
	/* Start counting */
	while (avr_bit_isclear(*port, pin))
	{
		us++;
		_delay_us(1);
	}
	
	return us;
}

uint32_t avr_pulse_measure_inv_ms(volatile uint8_t *port, uint8_t pin)
{
	uint32_t ms = 0;

	/* Wait for the pin to go high (to previous pulse to end, if any) */
	loop_until_bit_is_set(*port, pin);

	/* Then wait for it to go low */
	loop_until_bit_is_clear(*port, pin);
	
	/* Start counting */
	while (avr_bit_isclear(*port, pin))
	{
		ms++;
		_delay_ms(1);
	}
	
	return ms;
}

