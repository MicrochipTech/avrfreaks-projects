/**
 * input.c
 *
 * key input
 */

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/io.h>

#include "io.h"
#include "timing.h"
#include "common.h"

extern uint32_t ticks;
volatile uint8_t onsleep;

uint32_t ctl_power_start=0;

#define CTL_POWER 			2
#define CTL_POWER_TIEOUT	(60*16)

void IORefreshPower (void)
{
	/**  reset at every key release **/
	ctl_power_start = ticks;
}

void IOPowerOff (void)
{
	// exclude interrupts
	SREG &= 0x7f;

	PORTB &= ~(1<<CTL_POWER);
}

void InitIO (void)
{
	/** ADC pins as input, PORTB CTL_POWER as output **/
	DDRC	=	GLOBAL_DIRC;
	DDRB	=	GLOBAL_DIRB;

	/** at boot, take POWER control, ad pullups for I0 and I1 **/
	PORTB	=  (1<<CTL_POWER) | 0x03;

	IORefreshPower();
}

#if 0
/**
 * sleeping procedure routines, can be still useful.
 */
ISR(PCINT0_vect)
{
	onsleep=0;
}

void IntOnChangheON (void)
{
	/* enable PCINT0 and 1 for wake up */
	PCMSK0 |= (1<<PCINT0);
}

void IntOnChangheOFF (void)
{
	PCICR  &= ~(1<<PCIE0);
}

void GoSleeping(void)
{
	/* disable ADC completely */
	ADCSRA = 0;

	IntOnChangheON();

	DDRB = GLOBAL_DIRB; PORTB=0x03;
	DDRD = GLOBAL_DIRD; PORTD=0x00;
	DDRC = GLOBAL_DIRC;

	onsleep=1;

	/* prepare for power-save mode */
	SMCR |= ((1<<SE) | (1<<SM0) | (1<<SM1));

	while (onsleep)
	{
		// exit everytime timer2 trigger
		__asm__ volatile ("sleep");
	}

	IntOnChangheOFF();
}
#endif

unsigned char GetKey (void)
{
	if (!(PINB&0x01)) { IORefreshPower(); return KEY_LEFT; }
	if (!(PINB&0x02)) { IORefreshPower(); return KEY_RIGHT; }

	return KEY_NONE;
}

void IOUpdate (void)
{
	//return;

	/** max 60 seconds allowed **/
	if (ctl_power_start && ticks>(ctl_power_start+CTL_POWER_TIEOUT))
	{
		IOPowerOff();

		for (;;);

		/** and from here there is nothing **/
	}
}
