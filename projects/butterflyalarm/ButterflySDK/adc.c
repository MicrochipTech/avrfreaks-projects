/*  This file is a part of ButterflySDK. ButterflySDK is a set of software
	modules used to program the Butterfly Evaluation Board for ATMEL.
	Emphasis is given in code re-use and in maintainability. ButterflySDK
	is compiled with GCC AVR compiler. Supporting tools are written in Python
	programming language.
	Copyright (C) 2007, Kostas Giannakakis

    ButterflySDK is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ButterflySDK is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ButterflySDK; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** @file adc.c Analog-To-Digital Converter Module. Contains functions for
		  performing Analog-To-Digital Conversions.
*/

#include <avr/io.h>
#include <inttypes.h>

#include "c_ext.h"

/*******************************************************************************
* Forward Declarations
*******************************************************************************/

uint16_t ADCRead(void);

/*******************************************************************************
* Functions
*******************************************************************************/

/**
	Initializes an ADC channel. Only one channel can be active at a given
	time.
	@param channel The number of ADC channel to initialize.
*/
void ADCInit(uint8_t channel)
{
	// External AREF and ADCx
	ADMUX = channel;

	// set ADC prescaler to , 1MHz / 8 = 125kHz
	ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);

	// Dummy
	ADCRead();
}

/**
	Closes an ADC channel.
*/
void ADCDisable(void)
{
	// Disable the VCP
	cbiBF(PORTF,PF3);
	cbiBF(DDRF,DDF3);

	// Disable the ADC
	cbiBF(ADCSRA, ADEN);
}

/** Performs an Analog-To-Digital Conversion. #ADCInit must have been be
	called beforehand.
	@result The outcome of the conversion. The 10 Least Significant Bits
	are used.
*/
uint16_t ADCRead(void)
{
	uint8_t i;
	uint16_t valueADC;
	uint16_t meanValueADC = 0;

	// To save power, the voltage over the LDR and the NTC is turned off when
	// not used. This is done by controlling the voltage from a I/O-pin.
	// PORTF3 enables/disables the VCP (VC-peripheral)
	sbiBF(PORTF, PF3);
	sbiBF(DDRF, DDF3);

	// Enable the ADC
	sbiBF(ADCSRA, ADEN);

	// Do a dummy readout first (do single conversion - wait for ADIF flag)
	ADCSRA |= (1<<ADSC);
	while(!(ADCSRA & 0x10));

	// 8-times oversampling
	for(i=0;i<8;i++)
	{
		ADCSRA |= (1<<ADSC);
		while(!(ADCSRA & 0x10));

		valueADC = ADCL;
		valueADC += (ADCH << 8);

		meanValueADC += valueADC;
	}

	meanValueADC = meanValueADC >> 3;

	// Disable the VCP
	cbiBF(PORTF,PF3);
	cbiBF(DDRF,DDF3);

	// Disable the ADC
	cbiBF(ADCSRA, ADEN);

	return meanValueADC;
}
