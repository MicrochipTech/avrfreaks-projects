/*
 * ad9850.h
 *
 *  Created on: Nov 23, 2013
 *      Author: PBarnes AA1YY
 *      (c) Peter Barnes March 2014 findeborgen@gmail.com
 *
 */

#ifndef AD9850_H_
#define AD9850_H_

#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define Clock_Freq  125000000	// AD9850 crystal clock frequency in MHz

uint8_t tuning_byte_array[5];
unsigned long long two_raised_32 = 4294967296;		// 2 ^ 32
unsigned long tuning_word;
unsigned long clock = Clock_Freq;
unsigned long temp;
char freq_string[9];

// ATMEGA 8, internal clock @ 4MHz

// Parallel Mode:
// PB0-PB7 connected to ASD9850 module Data Pins 0-7
// PC0 connected to CLK (word_clk)
// PC1 connected to DATA
// PC2 connected to FQ (frequency update)
// PC3 connected to RST (reset)

// Serial Mode:
// D0, D1 of AD9850 module tied to Vcc. D2 tied to GND.
// PC0 (PIN 23) connected to CLK (word_clk)
// PC1 (pin 24) connected to DATA
// PC2 (pin 25) connected to FQ (frequency update)
// PC3 (pin 26) connected to RST (reset)


void setup_parallel(void)
{
	DDRB = 0xff;	// PORT B set for output
	DDRC = 0xcf;	// PORT C set for output on pins 0-3, input on pins 4,5
	PORTB = 0;		// put a '0' on all pins
	PORTC = 0x30;	// put a '0' on AD9850 control pins, weak pullups on PC4-5

	// reset the chip
	// pulse RST, CLK, FQ
	PORTC |= _BV(PC3);	//	set bit PC3: pulse the RESET line high
	_delay_us(4);		// wait
	PORTC &= ~(_BV(PC3));	// and bring the RESET low again
	_delay_us(4);	// wait
	PORTC |= _BV(PC0);	//	set bit PC0: pulse the CLK line high
	_delay_us(4);		// wait
	PORTC &= ~(_BV(PC0));	// and bring the CLK low again
	_delay_us(4);	// wait
	PORTC |= _BV(PC2);	//	set bit PC2: pulse the FQ line high
	_delay_us(4);		// wait
	PORTC &= ~(_BV(PC2));	// and bring the FQ low again
	_delay_us(4);	// wait
}

void setup_serial(void)   // D0, D1 of AD9850 module tied to Vcc. D2 tied to GND.
{
	DDRC = 0xcf;	// PORT C set for output on pins 0-3, input on pins 4,5
	PORTC = 0x30;	// put a '0' on AD9850 control pins, weak pullups on PC4-5
	// reset the chip
	// pulse RST, CLK, FQ
	PORTC |= _BV(PC3);	//	set bit PC3: pulse the RESET line high
	_delay_us(4);		// wait
	PORTC &= ~(_BV(PC3));	// and bring the RESET low again
	_delay_us(4);	// wait
	PORTC |= _BV(PC0);	//	set bit PC0: pulse the CLK line high
	_delay_us(4);		// wait
	PORTC &= ~(_BV(PC0));	// and bring the CLK low again
	_delay_us(4);	// wait
	PORTC |= _BV(PC2);	//	set bit PC2: pulse the FQ line high
	_delay_us(7);		// wait
	PORTC &= ~(_BV(PC2));	// and bring the FQ low again
	_delay_us(7);	// wait
	// set_up for serial transmission
	PORTC &= ~(_BV(PC1));	// ensure that the DATA (or D7) line is low

	PORTC |= _BV(PC0);	//	set bit PC0: pulse the CLK line high
	_delay_us(4);		// wait
	PORTC &= ~(_BV(PC0));	// and bring the CLK low again
	_delay_us(4);	// wait
	PORTC |= _BV(PC2);	//	set bit PC2: pulse the FQ line high
	_delay_us(7);		// wait
	PORTC &= ~(_BV(PC2));	// and bring the FQ low again
	_delay_us(7);	// wait
}

void pulse_clk(uint8_t data)
{
	PORTB = data;
	_delay_us(4);	// delay for data setup time as stated in datasheet
	PORTC |= _BV(PC0);	//	set bit PC0: pulse the CLK line high
	_delay_us(4);		// wait
	PORTC &= ~(_BV(PC0));	// and bring the CLK low again
	_delay_us(4);	// wait
}

unsigned long make_tuning_word (unsigned long freq)
{
	tuning_word = (two_raised_32 * freq) / clock;   // at 7100050 MHZ = 243955860
	//	add on some of the fractional portion lopped off by integer math above
	temp = two_raised_32 / (clock / 100);
	temp = temp % 100;
	temp = temp * (freq / 100000);
	tuning_word = tuning_word + temp;
	//
    return tuning_word;
}

void load_tuning_word_parallel(unsigned long tuning_word)	// LOAD TUNING WORD working
{
	// pulse the frequency update line
	// example: 7.0000 MHz = 0x00, 0x0e, 0x56, 0x14, 0x00
	//
	uint8_t i;
	for (i = 0; i <5; i++)
	{
		tuning_byte_array[i] = tuning_word & 0x00000000ff;	// isolate the lowest 8 bits
		tuning_word = tuning_word >> 8;	// trim off the least 8 bits
	}

	PORTC |= _BV(PC2);	//	set bit PC2: pulse the FQ line high to update frequency
	_delay_us(7);		//  wait
	PORTC &= ~(_BV(PC2));	// and bring the FQ low again
	_delay_us(7);	// wait

	for (i = 5; i > 0; i--)
	{
		pulse_clk(tuning_byte_array[i - 1]);	// load in the tuning word in 5 parts
	}

	// pulse the frequency update line
	PORTC |= _BV(PC2);	//	set bit PC2: pulse the FQ line high to update frequency
	_delay_us(7);		// wait
	PORTC &= ~(_BV(PC2));	// and bring the FQ low again
	_delay_us(7);	// wait
}

void load_tuning_word_serial(unsigned long tuning_word)	// shift out 40 bits, LSB of freq. 1st. 0-31 freq, 32-40 phase
{
	uint8_t i;
	unsigned long temp = tuning_word;
	for (i = 0; i <40; i++)
	{
		if ((temp & 1) == 1)	// if the bit = '1'
		{
			PORTC |= _BV(PC1);	//	set bit PC1: set the DATA line high
		}
		else
		{
			PORTC &= ~(_BV(PC1));	//	reset bit PC1: set the DATA line low
		}
		// bump the word clock update pin (CLK)
		PORTC |= _BV(PC0);	//	set bit PC0: set the CLK line high
		_delay_us(4);		// wait
		PORTC &= ~(_BV(PC0));	// and bring the CLK low again
		_delay_us(4);
		temp = temp >> 1;		// rotate from lsb to msb, testing each one
	}
	// pulse the frequency update line (FQ)
	PORTC |= _BV(PC2);	//	set bit PC2: pulse the FQ line high to update frequency
	_delay_us(7);		// wait
	PORTC &= ~(_BV(PC2));	// and bring the FQ low again
	_delay_us(7);	// wait
}

#endif /* AD9850_H_ */

