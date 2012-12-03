/*! \file a2d.c \brief Analog-to-Digital converter function library. */
//*****************************************************************************
//
// File Name	: 'a2d.c'
// Title		: Analog-to-digital converter functions
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2002-04-08
// Revised		: 2002-09-30
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
// revision by Gvl 26/04/2006
// Version		: 1.2
/// for GSM_ControlSystem "GSA" V1.0  Copyright (C) 2012 Easy System
//*****************************************************************************
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/sleep.h>

#include "a2d.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Software flag used to indicate when
/// the a2d conversion is complete.
volatile unsigned char a2dCompleteFlag;
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// functions
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// initialize a2d converter
void a2dInit(void)
{
//	ADCSR |= _BV(ADEN);	// enable ADC (turn on ADC power)

	a2dSetPrescaler(ADC_PRESCALE);	// set default prescaler

//	a2dSetReference(ADC_REFERENCE_256V);
	//external VCC reference
	a2dSetReference(ADC_REFERENCE_AVCC);

	ADMUX &= ~_BV(ADLAR);    		// set to right-adjusted result

  // SET POWER Noise reduction when calling idle
  set_sleep_mode(SLEEP_MODE_ADC);

	a2dCompleteFlag = false;		// clear conversion complete flag
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// configure A2D converter clock division (prescaling)
void a2dSetPrescaler(unsigned char prescale)
{
   ADCSR = (ADCSR & ~ADC_PRESCALE_MASK) | prescale;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// configure A2D converter voltage reference
void a2dSetReference(unsigned char ref)
{
	ADMUX = (ADMUX & ~ADC_REFERENCE_MASK) | (ref<<6);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Perform a 10-bit conversion
// starts conversion, waits until conversion is done, and returns result
unsigned short a2dConvert10bit(unsigned char ch)
{
	ADCSR |= _BV(ADIE);			// enable ADC interrupts
	ADCSR |= _BV(ADEN);		// enable ADC (turn on ADC power)

	a2dCompleteFlag = false;	// clear conversion complete flag

	// set channel
	ADMUX  =  (ADMUX & ~ADC_MUX_MASK) | (ch & ADC_MUX_MASK);

	// clear hardware "conversion complete" flag
	ADCSR |= _BV(ADIF);

  sleep_enable();
  sleep_cpu();
  sleep_disable();

  //only for sycronization. When here interrupt is done (maybe cpu enabled from other interrupts)
  while(!a2dCompleteFlag);			// wait until conversion complete

	ADCSR &= ~_BV(ADIE);				// disable ADC interrupts
	ADCSR &= ~_BV(ADEN);			// disable ADC (turn off ADC power)

	// CAUTION: MUST READ ADCL BEFORE ADCH!!!
	return ADCL | (ADCH<<8);	// read ADC (full 10 bits);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Interrupt handler for ADC complete interrupt.
ISR(ADC_vect)
{
	// set the a2d conversion flag to indicate "complete"
	a2dCompleteFlag = true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

