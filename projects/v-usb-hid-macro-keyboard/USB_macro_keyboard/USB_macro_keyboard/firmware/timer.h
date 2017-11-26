/*	Timer1 procedures - generic timeouts and USB polling calls
*   Copyright (C) 2017  Ondra Vlk
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef TIMER_INCLUDED
#define TIMER_INCLUDED

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>        //Resetting watch-dog timer
#include <avr/sleep.h>      //Delay loop - sleep mode has to be defined before delayTimer1 is called, also sei() has to be active

#include "main.h"
#include "usbdrv.h"
#include "hid.h"

void timerInit();
void delayTimer1(uint8_t cycles);

volatile uint8_t timeout = 0;   //Timer 1 down counter - used for delay function

//Interrupt driven ~50Hz timer1 to poll USB connection and generaly give info that it is alive.
//ISR_NOBLOCK prevents obscuring of USB protocols
ISR(TIMER1_COMPA_vect, ISR_NOBLOCK)
{
	//wdt_reset();        //Prevent reset from watchdog //moved from timer which is always triggered (original function of WDT was lost?)
	usbPoll();          //Prevent disconnection from USB by polling at min. 20Hz

	//lock-key sensing
	if(lockTimeout > 0) {
		--lockTimeout;
	} else {
		lockChangesNumNo = 0;
		lockChangesCapsNo = 0;
	}

	if(timeout) {--timeout;}
}

//Init ~50Hz CTC timer with overflow interrupt
#define F_TIMER1     50UL          //Timer operating frequency - has to corelate with timerInit values
void timerInit()
{
	OCR1B = 0xFF;       //Set the value to TOP which should not be reached in ctc mode - prevent some misbehavior of COMPB if any
	TIMSK = 0x40;       //Enable COMPA interrupt flags
	TCNT1 = 0x00;       //Reset counter to BOTTOM value
	GTCCR = 0x02;       //Reset prescaler

#if F_CPU == 12000000UL
	OCR1C = 234;
	TCCR1 = 0x8B;       //Start clock in CTC mode with prescaler 1024
#elif F_CPU == 16500000UL
	OCR1C = 160;
	TCCR1 = 0x8C;       //Start clock in CTC mode with prescaler 2048
#else
#error "Timer not defined for this clock"
#endif // F_CPU
	OCR1A = OCR1C;      //To trigger interrupt at MAX value of CTC counter
}

//Interrupt driven delay on timer 1; timeout is in timer1 cycles
void delayTimer1(uint8_t cycles)
{
	timeout = cycles;

	while(timeout) {    //Since USB is probably waking up the device with INT0 interrupt the while(cyles--) can't be used directly for delay
		wdt_reset();
		sleep_mode();
	}
}

#endif // TIMER_INCLUDED
