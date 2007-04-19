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

/** @file hw.c Hardware driver. Provides the routine that initializes all
	hardware drivers.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include "c_ext.h"
#include "button.h"
#include "lcd.h"
#include "rtc.h"
#include "clock.h"
#include "sensors.h"
#include "sound.h"
#include "dataflash.h"
#include "spi.h"
#include "timer0.h"
#include "usart.h"

/** Stalls the execution of the program for a number of millisec. The usage of
	this function is best avoided. Timers should be used instead. It is
	provided here only for being used by the OSCCALCalibrate routine.
	@param millisec Number of msec to wait
*/
void Delay(unsigned int millisec)
{
	uint8_t i;

	while (millisec--)
	{
		for (i=0; i<125; i++)
		{
			asm volatile ("nop"::);
		}
	}
}

/** Calibrate the internal OSCCAL byte, using the external 32,768 kHz crystal
	as reference. This function is necessary in order to achieve a reliable
	Usart baud rate.
*/
void OSCCALCalibrate(void)
{
	unsigned char calibrate = FALSE;
	int temp;
	unsigned char tempL;

	// Set Clock Prescaler Change Enable
	CLKPR = (1<<CLKPCE);

	// Set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
	CLKPR = (1<<CLKPS1) | (1<<CLKPS0);

	// Sisable OCIE2A and TOIE2
	TIMSK2 = 0;

	// Select asynchronous operation of timer2 (32,768kHz)
	ASSR = (1<<AS2);

	// Set timer2 compare value
	OCR2A = 200;

	// Delete any interrupt sources
	TIMSK0 = 0;

	// Start timer1, timer2 with no prescaling
	TCCR1B = (1<<CS10);
	TCCR2A = (1<<CS20);

	// Wait for TCN2UB and TCR2UB to be cleared
	while((ASSR & 0x01) | (ASSR & 0x04));

	// Wait for external crystal to stabilise
	Delay(1000);

	while(!calibrate)
	{
		cli();

		// Delete TIFR1 flags
		TIFR1 = 0xFF;
		TIFR2 = 0xFF;

		// Clear timer1, timer2 counters
		TCNT1H = 0;
		TCNT1L = 0;
		TCNT2 = 0;

		// Wait for timer2 compare flag
		while ( !(TIFR2 & (1<<OCF2A)) );

		// Stop timer1
		TCCR1B = 0;

		sei();

		if ( (TIFR1 & (1<<TOV1)) )
		{
			// If timer1 overflows, set the temp to 0xFFFF
			temp = 0xFFFF;
		}
		else
		{   // Read out the timer1 counter value
			tempL = TCNT1L;
			temp = TCNT1H;
			temp = (temp << 8);
			temp += tempL;
		}

		if (temp > 6250)
		{
			// The internal RC oscillator runs to fast, decrease the OSCCAL
			OSCCAL--;
		}
		else if (temp < 6120)
		{
			// The internal RC oscillator runs to slow, increase the OSCCAL
			OSCCAL++;
		}
		else
		{
			// The interal RC oscillator has been calibrated
			calibrate = TRUE;
		}

		// Start timer1
		TCCR1B = (1<<CS10);
	}
}

/** Configures the necessary AVR registers and initializes all the hardware
	drivers. */
void HWInit(void)
{
	OSCCALCalibrate();

	// Set Clock Prescaler Change Enable
	CLKPR = (1<<CLKPCE);

	// Set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
	CLKPR = (1<<CLKPS1) | (1<<CLKPS0);

	// Disable Analog Comparator (power save)
	ACSR = (1<<ACD);

	// Disable Digital input on PF0-2 (power save)
	DIDR0 = (7<<ADC0D);

	// Enable pull-up on PB0-3, PE4-7   ???
	PORTB = (15<<PB0);
	PORTE = (15<<PE4);

	// Configure OC1A as output, set it to high level
	sbiBF(DDRB, 5);
	sbiBF(PORTB, 5);

	ButtonInit();
	LcdInit();
	RtcInit();
	SoundInit();
	UsartDisable();

	Timer0Stop();
	SpiDisable();
	DATAFLASH_CHIP_SELECT_INACTIVE();
}

/** Sets the micro-controller in Power-save or idle mode to save power. */
void PowerSave(void)
{
	if (!IsUsartEnabled() && !SoundIsOn())
	{
		// Enable Power-save mode
		SMCR = (3<<SM0) | (1<<SE);
		asm volatile ("sleep"::);
	}
	else
	{
		// Enable idle mode
		SMCR = (1<<SE);
		asm volatile ("sleep"::);
	}
	// Just woke, disable sleep
	SMCR = 0;
}
