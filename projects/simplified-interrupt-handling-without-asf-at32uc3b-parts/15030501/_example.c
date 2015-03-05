/*********************************************
* TCJ 3-4-15
*
* intcb_example.c
*
* 15030401
*
* program to demonstrate use of intcb.h
*
* 
* Chip type           : at32uc3b1256
* Clock frequency     : main clock at 48 MHz using external 12 MHz crystal
*
* note that when chip starts up in dfu mode the external crystal is also default and it works
* automatically for USB (for an 8, 12, or 16 MHz crystal). 
* 
*
* In order to boot into dfu mode and thus allow programming, it is necessary to do a reset (bring the reset pin
* 47 down then up) while PA13 (pin 25) is down. This is done with pushbuttons. 
*
***********************************************
*
* This program demonstrates 4 concurrent interrupts:
*
*	1. INTCB1: toggle LED2 (PA21) every 1 second using RTC interrupt (0.5 Hz blink)
*
*	2. INTCB2: toggle LED1 (PA08) using GPIO interrupt from pushbutton on PA13 
*
*	3. INTCB14: toggle LED0 (PA07) every 175 ms using interrupt from TC1 (2.9 Hz blink)
*
*	4. INTCB14: toggle TP (PA22) every 1 ms using interrupt from TC2 
*					(500 Hz square wave visible with oscilloscope or audible with headphones of impedance > 200 ohms)
*
*	Program MUST be compiled using -std=gnu99 compiler option
*
*	Program should work unaltered on EVK1101, but hasn't been tested on one
*
*
*********************************************/

#include <avr32/io.h>
#include <stdlib.h>
#include <inttypes.h>

#define INTCB1			// must be defined before #include of "intcb.h" -  1 is RTC (but also external interrupt and pwr mgr)
#define INTCB2			// must be defined before #include of "intcb.h" -  2 is GPIO
#define INTCB14			// must be defined before #include of "intcb.h" - 14 is TC

#include "intcb.h"



void intcb1(void)										// the RTC interrupt handler
{
	AVR32_GPIO.port[0].ovrt = 1 << 21;					// toggle PA21

	AVR32_RTC.ICR.topi = 1;								// clear interrupt
}


void intcb2(void)										// the GPIO interrupt handler
{
	AVR32_GPIO.port[0].ovrt = 1 << 8;					// toggle PA08
	AVR32_GPIO.port[0].ifrc = 1 << 13;					// clear the GPIO interrupt from PA13

														// note that we have not looked to see which line caused the interrupt,
														// which would be necessary if there were more than 1 GPIO interrupt.
														// this can be done by checking AVR32_GPIO.port[0].ifr
}



void intcb14(void)										// the TC interrupt handler
{
	
	if(AVR32_TC.channel[2].SR.cpcs)						// if the interrupt is from TC2 RC compare (also, reading SR clears interrupt)
	{
		AVR32_GPIO.port[0].ovrt = 1 << 22;				// toggle PA22
	}

	if(AVR32_TC.channel[1].SR.cpcs)						// if the interrupt is from TC1 RC compare (also, reading SR clears interrupt)
	{
		AVR32_GPIO.port[0].ovrt = 1 << 7;				// toggle PA7
	}
}

/*
void intcb14(void)										// the TC interrupt handler - alternative method for identifying source of interrupt
{
	uint32_t irr14word = AVR32_INTC.irr[14];			// read IRR register once, because reading it seems to clear it (undocumented feature)
		
	if(irr14word & 0x04)								// if the interrupt is from TC2 
	{
		AVR32_GPIO.port[0].ovrt = 1 << 22;				// toggle PA22
		AVR32_TC.channel[2].sr;							// read the TC2 status register to clear interrupt		
	}

	if(irr14word & 0x02)								// if the interrupt is from TC1 
	{
		AVR32_GPIO.port[0].ovrt = 1 << 7;				// toggle PA7
		AVR32_TC.channel[1].sr;							// read the TC2 status register to clear interrupt
	}
}
*/





int main(void)
{
	setup_intcb();									// setup interrupt handling (function in intcb.h)
	
// set up main clock

	AVR32_PM.MCCTRL.osc0en = 1;						// enable oscillator 0 (12 MHz crystal)
	AVR32_PM.OSCCTRL0.mode = 7;						// select crystal > 8 MHz

	AVR32_PM.PLL[0].pllen = 1;						// enable PLL0
	AVR32_PM.PLL[0].pllosc = 0;						// OSC0 is source for PLL0
	AVR32_PM.PLL[0].pllmul = 3;						// multiply by 8 = 96 MHz
	AVR32_PM.PLL[0].plldiv = 0;						// no input division
	AVR32_PM.PLL[0].pllopt = 3;						// divide output by 2 = 48 MHz

	while(!(AVR32_PM.POSCSR.lock0 & 1));			// wait for PLL0 to lock

	AVR32_PM.MCCTRL.mcsel = 2;						// PLL0 is source for main clock

// Note that PBA clock is equal to main clock (48 MHz) as I have left pbadiv = 0



//------  set up RTC to generate periodic interrupt at 1 Hz, triggering intcb1 (the RTC interrupt)


	while(AVR32_RTC.CTRL.busy) {}					// wait til RTC wants to be written
	AVR32_RTC.CTRL.en = 1;							// enable RTC
	while(AVR32_RTC.CTRL.busy) {}					// wait again
	AVR32_RTC.CTRL.clk32 = 0;						// use 115 kHz RC 'slow clock' rather than 32 kHz oscillator
	while(AVR32_RTC.CTRL.busy) {}					// wait some more
	AVR32_RTC.CTRL.clken = 1;						// enable clock to RTC

	while(AVR32_RTC.CTRL.busy) {}					// finally, the last wait
	AVR32_RTC.top = 57500;							// (slow clock/2)/57500 = 1 Hz
	AVR32_RTC.IER.topi = 1;							// enable RTC interrupt

//------  since intcb1 interrupt from RTC is intended to toggle PA21, we configure PA21:

	AVR32_GPIO.port[0].gpers = 1 << 21;		// GPIO enable register, set means line is gpio (line is PA21)
	AVR32_GPIO.port[0].oders = 1 << 21;		// line is driven	



//------  set up PA13 as pushbutton input, triggering intcb2 (the GPIO interrupt)
 	 
	AVR32_GPIO.port[0].gpers = 1 << 13;				// GPIO enable register, set means line is gpio  (line is PA13)
	AVR32_GPIO.port[0].oders = 0 << 13;				// line is not driven (default is 0, so this isn't needed except to remind me)
	AVR32_GPIO.port[0].puer = 1 << 13;				// pull up enable
	
	AVR32_GPIO.port[0]. imr0 = 0 << 13;				// interrupt triggered on falling edge
	AVR32_GPIO.port[0]. imr1 = 1 << 13;				// interrupt triggered on falling edge
	AVR32_GPIO.port[0].gfer = 1 << 13;				// glitch filter to debounce pushbutton, we hope
	AVR32_GPIO.port[0].ier = 1 << 13;				// interrupt enable

//------  since intcb2 interrupt from GPIO is intended to toggle PA08, we configure PA08:

	AVR32_GPIO.port[0].gpers = 1 << 8;		// GPIO enable register, set means line is gpio (line is PA8)
	AVR32_GPIO.port[0].oders = 1 << 8;		// line is driven



//------  set up TC1 to generate periodic interrupt at 5.7 Hz, triggering intcb14 (the TC interrupt)

	AVR32_TC.channel[1].CMR.waveform.tcclks = 4;	// select TIMER_CLOCK5 = PBA Clock/128 = 375 kHz
	AVR32_TC.channel[1].CMR.waveform.wave = 1;		// wave, not capture
	AVR32_TC.channel[1].CMR.waveform.wavsel = 2;	// UP mode, clear CV on RC compare

	AVR32_TC.channel[1].rc = 65535;					// compare register RC - we're dividing by 65535 (largest possible divisor)

	AVR32_INTC.IPR[14].intlevel = 2;				// raise interrupt priority to level 2 for interrupt group 14 (the TC)
 	AVR32_TC.channel[1].IER.cpcs = 1;				// enable interrupt on CV = RC

	AVR32_TC.channel[1].CCR.clken = 1;				// enable the clock
	AVR32_TC.channel[1].CCR.swtrg = 1;				// won't work without this!

//------  since intcb14 interrupt from TC1 is intended to toggle PA07, we configure PA07:

	AVR32_GPIO.port[0].gpers = 1 << 7;		// GPIO enable register, set means line is gpio (line is PA7)
	AVR32_GPIO.port[0].oders = 1 << 7;		// line is driven


	 
//------  set up TC2 to generate periodic interrupt at 1 kHz, triggering intcb14 (the TC interrupt)


	AVR32_TC.channel[2].CMR.waveform.tcclks = 1;	// select TIMER_CLOCK2 = PBA Clock/2 = 24 MHz
	AVR32_TC.channel[2].CMR.waveform.wave = 1;		// wave, not capture
	AVR32_TC.channel[2].CMR.waveform.wavsel = 2;	// UP mode, clear CV on RC compare

	AVR32_TC.channel[2].rc = 24000;					// compare register RC - we're dividing by 24000

 	AVR32_TC.channel[2].IER.cpcs = 1;				// enable interrupt on CV = RC

	AVR32_TC.channel[2].CCR.clken = 1;				// enable the clock
	AVR32_TC.channel[2].CCR.swtrg = 1;				// won't work without this!

//------  since intcb14 interrupt from TC2 is intended to toggle PA22, we configure PA22:

	AVR32_GPIO.port[0].gpers = 1 << 22;		// GPIO enable register, set means line is gpio (line is PA22)
	AVR32_GPIO.port[0].oders = 1 << 22;		// line is driven


//------

	START_GLOB_INTS									// enable global interrupts

	while(1)	{}									// forever loop

}



