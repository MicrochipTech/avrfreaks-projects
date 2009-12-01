/*
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation, 
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "./config.h"
#include "./uart.h"
#include "./util.h"


uint8 LedCount;
uint8 LedDuration, SavedLedDuration;
volatile uint8 Timer1Counter;
volatile uint8 Timer1Counter2;
//
// TC1 is used for general timeout purposes
//	LED flashing
//	Command Timeout
//	General Purpose Delay
//
// Set prescaler to clkio/1024. At 20 Mhz, this is 51.2 uSec/
// Using a Timer1CountUp value of 1953 yields a match every 100 mSec
//
#if F_CPU==20000000UL
#define Timer1CountUp 1953
#elif F_CPU==8000000UL
#define Timer1CountUp 781
#elif F_CPU==1000000UL
#define Timer1CountUp 97
#else
error: F_CPU undefined
#endif

void InitTC1(void)
{
	TCCR1A = 0;
	TCCR1B = (1<<CS02)|(0<<CS01)|(1<<CS00);
	OCR1AH = (Timer1CountUp >> 8);
	OCR1AL = (Timer1CountUp & 0xff);
}

void EnableTC1(void)
{
	TIFR1 = (1<<OCF1A);		//clear pending match
	TIMSK1 = (1<<OCIE1A);	//Enable TC0.match
	Timer1Counter = 0;
	Timer1Counter2 = 0;
	LedDuration = SavedLedDuration = LedCount = 0;
}

ISR(TIMER1_COMPA_vect)
{
	TCNT1H = 0;
	TCNT1L = 0;
	if (Timer1Counter != 0) {
		--Timer1Counter;
	}
	if (Timer1Counter2 != 0) {
		--Timer1Counter2;
	}
	if (LedDuration != 0) {
		if (--LedDuration == 0) {
			if (--LedCount) {
#if USE_LED
				sbi(LEDI,LED_BIT);		// more flashes to go
#endif
				LedDuration = SavedLedDuration;
			}
		}
	}
	AppTimer1Stuff();
}

//
// These functions work only if TC1 is active
//
void delayHalfSecond(void)
{
	Timer1Counter = 5;
	while (Timer1Counter) ;
}

void delay100msecs(void)
{
	Timer1Counter = 1;
	while (Timer1Counter) ;
}

void delay300msecs(void)
{
	Timer1Counter = 3;
	while (Timer1Counter) ;
}

void delayOneSecond(void)
{
	Timer1Counter = 10;
	while (Timer1Counter) ;
}

//
// Start an LED flash.
//
void FlashLed(uint8 duration, uint8 cnt)
{
	if (!LedDuration & ! LedCount) {
		SavedLedDuration = LedDuration = duration;		// timer1 is running at this time so first interval uncertain
		LedCount = (cnt + 1) & 0xfe;	// Must be even to have LED off at the end
#if USE_LED
		sbi(LEDO,LED_BIT);			// LED on
#endif
	}
}

void BlinkFastNoInts(void)
{
#if USE_LED
	for (int i = 0; i < 10; ++i) {
		sbi(LEDO,LED_BIT);
		_delay_ms(100);
		cbi(LEDO,LED_BIT);
		_delay_ms(100);
	}
#endif
}

void assertFailed(const char* str, int line)
{
    UART_PrintfStr("ASSERT: ");
    UART_PrintfStr((char *)str);
    UART_PrintfStr(" line ");
    UART_Printfint(line);
    FlashLed(5, 3);
}
