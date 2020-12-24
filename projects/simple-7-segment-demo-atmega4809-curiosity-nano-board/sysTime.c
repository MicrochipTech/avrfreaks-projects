/* 
 * The MIT License
 *
 * Copyright 2020 CmdrZin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * sysTime
 *
 * Created: 11/9/2020 1:36:30 PM
 *  Author: Chip
 *
 * Time and Clock utilities for the ATmega4808/4809
 * NOTE: Bit definitions are in iom4809.h
 *
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

#include "sysTime.h"

/* *** ONLY enable ONE timer/counter *** */
#define USE_TCA0	true
#define USE_TCB0	false
#define USE_TCB1	false
#define USE_TCB2	false
#define USE_TCB3	false

static uint32_t totalMilliseconds;

static uint8_t st_10msFlags;

/* Initialize system clock and timers. ATmega4808/4809 */
void init_time()
{
	// Initialize MCU clock for 20 MHz
	CPU_CCP = CCP_IOREG_gc;		// unlock Change Protected Registers
	CLKCTRL_MCLKCTRLB = 0;		// Clear PEN bit to disable prescaler
#if USE_TCA0
	// Initialize TCA0 to generate an OVF interrupt every 1 ms.
	TCA0_SINGLE_PER = 0x4E1F;							// 20,000 - 1
	TCA0_SINGLE_CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;    // WGMODE[2:0] = 000
	TCA0_SINGLE_CTRLECLR = TCA_SINGLE_DIR_bm;			// Set DIR to up
	TCA0_SINGLE_INTCTRL = TCA_SINGLE_OVF_bm;			// Set OVF interrupt flag
	TCA0_SINGLE_CTRLA = TCA_SINGLE_ENABLE_bm;			// turn ON the timer. CLKSEL[2:0]=000 (Clock/1)
#endif
#if USE_TCB0
	// Initialize TCB0 to generate an OVF interrupt every 1 ms.
	TCB0_CCMP = 0x4E1F;									// 20,000 - 1
	TCB0_CTRLB = TCB_CNTMODE_INT_gc;				    // CNTMODE[2:0] = 000
	TCB0_INTCTRL = TCB_CAPT_bm;							// Set CAPT interrupt flag
	TCB0_CTRLA = TCB_ENABLE_bm;							// turn ON the timer. CLKSEL[2:0]=000 (Clock/1)
#endif
#if USE_TCB1
	// Initialize TCB1 to generate an OVF interrupt every 1 ms.
	TCB1_CCMP = 0x4E1F;									// 20,000 - 1
	TCB1_CTRLB = TCB_CNTMODE_INT_gc;				    // CNTMODE[2:0] = 000
	TCB1_INTCTRL = TCB_CAPT_bm;							// Set CAPT interrupt flag
	TCB1_CTRLA = TCB_ENABLE_bm;							// turn ON the timer. CLKSEL[2:0]=000 (Clock/1)
#endif
#if USE_TCB2
	// Initialize TCB1 to generate an OVF interrupt every 1 ms.
	TCB2_CCMP = 0x4E1F;									// 20,000 - 1
	TCB2_CTRLB = TCB_CNTMODE_INT_gc;				    // CNTMODE[2:0] = 000
	TCB2_INTCTRL = TCB_CAPT_bm;							// Set CAPT interrupt flag
	TCB2_CTRLA = TCB_ENABLE_bm;							// turn ON the timer. CLKSEL[2:0]=000 (Clock/1)
#endif
#if USE_TCB3
	// Initialize TCB0 to generate an OVF interrupt every 1 ms.
	TCB3_CCMP = 0x4E1F;									// 20,000 - 1
	TCB3_CTRLB = TCB_CNTMODE_INT_gc;				    // CNTMODE[2:0] = 000
	TCB3_INTCTRL = TCB_CAPT_bm;							// Set CAPT interrupt flag
	TCB3_CTRLA = TCB_ENABLE_bm;							// turn ON the timer. CLKSEL[2:0]=000 (Clock/1)
#endif
	// NOTE: Global interrupts are enabled with sei() after all interrupt
	// generating modules are initialized.
	
	st_10msFlags = 0;
}

/* Return the total number of milliseconds since the project started. */
uint32_t st_millis()
{
	uint32_t temp;				// make a holder for the counter.

	cli();						// Turn OFF interrupts to avoid corruption during a multi-byte read.
	temp = totalMilliseconds;	// get a copy while interrupts are disabled.
	sei();						// Turn interrupts back ON.

	return temp;				// return a 'clean' copy of the counter.
}

/* flag 0:7 */
bool is10msFlagSet( uint8_t flag )
{
	uint8_t mask = 0x01;
	bool result = false;

	// Position the mask bit to the flag bit to test
	mask <<= flag;
	// Test if the flag bit is set
	if( st_10msFlags & mask )
	{
		result = true;
		st_10msFlags &= ~mask;		// clear flag
	}

	return result;
}

#if USE_TCA0
/* This interrupt service is called every 1 ms. */
ISR(TCA0_OVF_vect)
{
	static uint8_t counter = 0;

	++totalMilliseconds;

	if(++counter >= 10) {
		st_10msFlags = 0xFF;
		counter = 0;
	}

	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;	// clear interrupt flag.
}
#endif

#if USE_TCB0
/* This interrupt service is called every 1 ms. */
ISR(TCB0_INT_vect)
{
	static uint8_t counter = 0;

	++totalMilliseconds;

	if(++counter >= 10) {
		st_10msFlags = 0xFF;
		counter = 0;
	}

	TCB0.INTFLAGS = TCB_CAPT_bm;	// clear interrupt flag.
}
#endif

#if USE_TCB1
/* This interrupt service is called every 1 ms. */
ISR(TCB1_INT_vect)
{
	static uint8_t counter = 0;

	++totalMilliseconds;

	if(++counter >= 10) {
		st_10msFlags = 0xFF;
		counter = 0;
	}

	TCB1.INTFLAGS = TCB_CAPT_bm;	// clear interrupt flag.
}
#endif

#if USE_TCB2
/* This interrupt service is called every 1 ms. */
ISR(TCB2_INT_vect)
{
	static uint8_t counter = 0;

	++totalMilliseconds;

	if(++counter >= 10) {
		st_10msFlags = 0xFF;
		counter = 0;
	}

	TCB2.INTFLAGS = TCB_CAPT_bm;	// clear interrupt flag.
}
#endif

#if USE_TCB3
/* This interrupt service is called every 1 ms. */
ISR(TCB3_INT_vect)
{
	static uint8_t counter = 0;

	++totalMilliseconds;

	if(++counter >= 10) {
		st_10msFlags = 0xFF;
		counter = 0;
	}

	TCB3.INTFLAGS = TCB_CAPT_bm;	// clear interrupt flag.
}
#endif
