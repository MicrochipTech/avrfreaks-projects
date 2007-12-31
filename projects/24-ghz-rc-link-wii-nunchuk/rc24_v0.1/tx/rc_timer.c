/*================================================================================
	2.4 Ghz R/C Control
	rc_timer.c

	Copyright ©2007 Ajay Grewal

THIS SOFTWARE AND DOCUMENTATION IS PROVIDED "AS IS," AND COPYRIGHT HOLDER MAKES NO
REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO,
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE
USE OF THE SOFTWARE OR DOCUMENTATION WILL NOT INFRINGE ANY THIRD PARTY PATENTS,
COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.

COPYRIGHT HOLDER WILL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL OR
CONSEQUENTIAL DAMAGES ARISING OUT OF ANY USE OF THE SOFTWARE OR DOCUMENTATION.

You may freely copy and redistribute this software if no fee is charged for use,
copying or distribution.  Such redistributions must retain the above copyright
notice and disclaimer.
================================================================================*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "rc_global.h"
#include "rc_timer.h"

volatile uint8_t timerIRQ = 0;

void Timer_Init(void)
{
	// Clear Output Compare A Match flag
	TIFR0 = (1 << OCF0A);          

	// Enable Output Compare A Match Interrupt
	TIMSK0 = (1 << OCIE0A);        

	// Enable CTC mode
	TCCR0A = _BV(WGM01);

	// Set TOP
	OCR0A = RC_TIMER0_TOP;

	// Start time with no prescalar
	TCCR0B = (1 << CS00);
}

ISR(TIMER0_COMPA_vect)
{
	timerIRQ = 1;
}

