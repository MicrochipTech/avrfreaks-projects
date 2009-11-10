//********************************************************************************************
//
// File : adc.c implement for on-board temparature sensor and ADC0
//
//********************************************************************************************
//
// Copyright (C) 2007
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
// This program is distributed in the hope that it will be useful, but
//
// WITHOUT ANY WARRANTY;
//
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin St, Fifth Floor, Boston, MA 02110, USA
//
// http://www.gnu.de/gpl-ger.html
//
//********************************************************************************************
#include <avr/io.h>
#include "adc.h"


#define 	outb(addr, data)   addr = (data)
#define 	inb(addr)   (addr)
#define 	outw(addr, data)   addr = (data)
#define 	inw(addr)   (addr)
#define 	outp(addr, data)   addr = (data)
#define 	inp(addr)   (addr)
#define 	BV(bit)   (1<<(bit))
#define 	cbi(reg, bit)   reg &= ~(BV(bit))
#define 	sbi(reg, bit)   reg |= (BV(bit))
#define 	cli()   __asm__ __volatile__ ("cli" ::)
#define 	sei()   __asm__ __volatile__ ("sei" ::)
#define 	DDR(x)   ((x)-1)
#define 	PIN(x)   ((x)-2)

//********************************************************************************************
//
// Function : adc_read
// Description : read ADC value, select ADC channel to read by channel argument
//
//********************************************************************************************
unsigned int adc_read ( char channel )
{
	unsigned char temp;
	// Analog channel selection
	ADMUX = ((ADMUX) & ~0x1f) | (channel & 0x1f);
	
	// Start conversion
	ADCSRA |= _BV(ADSC);

	// Wait until conversion complete
	while( bit_is_set(ADCSRA, ADSC) );
	// CAUTION: READ ADCL BEFORE ADCH!!!
	return ((ADCL) | ((ADCH)<<8));
//	temp = ADCL;
//	return (ADCH);
}
//********************************************************************************************
//
// Function : adc_init
// Description : Initial analog to digital convertion
//
//********************************************************************************************
//void adc_init ( void ) __attribute__ ((naked));
void adc_init ( void )
{

	ADCSRA |= _BV(ADEN); // for now we don't do this in the ISR | _BV(ADIE);
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1); // clk is / 64

/*
	// ADC enable, Prescaler divide by 128, ADC clock = 16MHz/128 = 125kHz
	ADCSRA = _BV(ADEN) |   _BV(ADPS1) ; // _BV(ADFR) |

	// Select Vref, internal Vref 2.56V and external capacitor | _BV(ADLAR) _BV(REFS1) |
	ADMUX =  _BV(REFS0)| _BV(ADLAR) ;
*/
}

