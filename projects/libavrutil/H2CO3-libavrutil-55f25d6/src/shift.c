/*
 * shift.c
 * libavrutil
 *
 * Created by Árpád Goretity on 26/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <util/delay.h>
#include "shift.h"

uint8_t avr_shift_byte_in_master(volatile uint8_t *dataport, uint8_t datapin, volatile uint8_t *clockport, uint8_t clockpin)
{
	uint8_t result = B00000000;
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		/* Generate a clock signal */
		avr_bit_set(*clockport, clockpin);

		/* Wait a bit */
		_delay_us(1);

		/* Read the next data bit */
		result |= (!!avr_bit_isset(*dataport, datapin)) << i;
		avr_bit_clear(*clockport, clockpin);
		_delay_us(1);
	}
	
	return result;
}

void avr_shift_byte_out_master(volatile uint8_t *dataport, uint8_t datapin, volatile uint8_t *clockport, uint8_t clockpin, uint8_t byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		/* Write out the actual bit of the data byte */
		avr_bit_write(*dataport, datapin, byte & _BV(i));
		/* Generate a clock signal */
		avr_bit_set(*clockport, clockpin);
		_delay_us(1);
		avr_bit_clear(*clockport, clockpin);
		_delay_us(1);
	}
}

uint8_t avr_shift_byte_in_slave(volatile uint8_t *dataport, uint8_t datapin, volatile uint8_t *clockport, uint8_t clockpin)
{
	uint8_t result = B00000000;
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		/* Wait for the clock signal */
		loop_until_bit_is_set(*clockport, clockpin);
		/* Read the next data bit */
		result |= (!!avr_bit_isset(*dataport, datapin)) << i;
		/* Wait for the clock signal to end */
		loop_until_bit_is_clear(*clockport, clockpin);
	}
	
	return result;
}

void avr_shift_byte_out_slave(volatile uint8_t *dataport, uint8_t datapin, volatile uint8_t *clockport, uint8_t clockpin, uint8_t byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		/* Wait for the clock signal */
		loop_until_bit_is_set(*clockport, clockpin);
		/* Write out the actual bit of the data byte */
		avr_bit_write(*dataport, datapin, byte & _BV(i));
		/* Wait for the clock signal to end */
		loop_until_bit_is_clear(*clockport, clockpin);
	}
}

