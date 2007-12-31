/*================================================================================
	2.4 Ghz R/C Control
	rc_pwm.c

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
#include <ctype.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "rc_global.h"
#include "rc_pwm.h"
#include "rc_serial.h"

uint8_t pinlevelD;
uint8_t compare[RC_PWM_MAX_NUM_CH];
volatile uint8_t compBuff[RC_PWM_MAX_NUM_CH];
volatile uint8_t timerIRQ = 0;

void PWM_Init(void)
{
	uint8_t i;

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

	// Set channel pins as output
	DDRD = _BV(RC_PWM_PIN_CH0) | _BV(RC_PWM_PIN_CH1);

	// Set channel outputs high
	pinlevelD = _BV(RC_PWM_PIN_CH0) | _BV(RC_PWM_PIN_CH1);

	// Initialize all channels
	for(i = 0 ; i < RC_PWM_MAX_NUM_CH ; i++) {
		compare[i] = RC_PWM_DEFAULT_PWM;
        compBuff[i] = RC_PWM_DEFAULT_PWM;
	}
}

void PWM_Update_Val(uint8_t ch, uint8_t val)
{
	compBuff[ch] = val; 
}

ISR(TIMER0_COMPA_vect)
{
	static uint16_t	softCount = RC_PWM_MAX_SOFTCOUNT - 1;

	// Update PWM channel outputs
	PORTD = pinlevelD;
	
	if(++softCount == RC_PWM_MAX_SOFTCOUNT) {
		// Update new values after PWM cycle
		compare[0] = compBuff[0];
		compare[1] = compBuff[1];

		// Set channel outputs high
		pinlevelD = _BV(RC_PWM_PIN_CH0) | _BV(RC_PWM_PIN_CH1);
		softCount = 0;
	}

	// clear port pin on compare match (executed on next interrupt)
	if(compare[0] == softCount)  {
		pinlevelD &= ~_BV(RC_PWM_PIN_CH0);
	}
	if(compare[1] == softCount) {
		pinlevelD &= ~_BV(RC_PWM_PIN_CH1);
	}

	timerIRQ = 1;
}
