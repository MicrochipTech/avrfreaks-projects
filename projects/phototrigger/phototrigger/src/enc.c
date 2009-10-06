/*
 * enc.c
 *
 *  Created on: 2009-08-30
 *      Author: liku
 *
 *      This file contains routines to handle encoder with integrated switch.
 *      Encoder is handled by overflow interrupt from T0.
 */

#include <inttypes.h>
#include <stdbool.h>

#include <avr/interrupt.h>

#include <util/delay.h>

#include "hw.h"
#include "enc.h"
#include "rtc.h"


static volatile uint8_t kleft = 0, kright = 0, kswitch = 0;
static uint8_t kskip = KEYDEFENCSKIP;

ISR(TIMER0_OVF_vect)
{
	const  uint16_t patt     = 0x1e1e; // 00|01|11|10|00|01|11|10 - special pattern to recognize direction
	static uint8_t  lastpos  = 0;
	static uint8_t  lastkeys = 0;
	uint8_t keys; // current keys state
	uint8_t enc;  // encoder state

	keys = ENCPIN & ENCMASK; // read current switch and encoder state

	if(lastkeys == keys) // any change since last state? If not - exit interrupt
		return;
	lastkeys = keys;

	rtcUpdInactTimer(); // update timer so LCD backlight will stay on

	if(!(keys & _BV(ENCSW))) // check switch
		kswitch++;

	enc = keys>>ENC0; // extract encoder state

	for(uint8_t i=1; i<=4; i++)
	{
		if(enc == (patt>>(i*2) & 0x03)) // move pattern and see if matches current encoder state
		{
			// compare against last position in patters
			if(i-1 == lastpos)
				kleft++;
			else if(i+1 == lastpos)
				kright++;
			// else - probably some error - skip

			lastpos = i;
 			// job done - exit!
			break;
		}
	}
}

/*
 * gets key from buffer
 */
key_t encGetKey(bool waitforkey)
{
	if(waitforkey)
	{
		while(	(kleft < kskip) &&
				(kright < kskip) &&
				(!kswitch));

		_delay_ms(KEYREPEATRATE);
	}

	if(kright > kskip)
	{
		kright = 0;
		return KEYRIGHT;
	}
	else if(kswitch)
	{
		kswitch = 0;
		return KEYSWITCH;
	}
	else if(kleft > kskip)
	{
		kleft = 0;
		return KEYLEFT;
	}

	return KEYNONE;
}

/*
 * clears keyboard buffer
 */
void encClrKeyBuf(void)
{
	kright = kleft = kswitch = 0;
}
