/*
,-----------------------------------------------------------------------------------------.
| io/clock
|-----------------------------------------------------------------------------------------
| this file implements a very basic clock
| - use 7.3728 Mhz Clock -> /1024/7200 = 1s clock
| - enables global interrupts (sei()) !
| - interrupt routine is small, need to call clock_do() 
|   to recalc the time at least every 255 seconds ! (call it ~ every second!)
|
| Author   : {{removed according to contest rules}}
|            -> circuitcellar.com avr design contest 2006
|            -> Entry #AT2616
|
|-----------------------------------------------------------------------------------------
| License:
| This program is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free Software
| Foundation; either version 2 of the License, or (at your option) any later
| version.
| This program is distributed in the hope that it will be useful, but
|
| WITHOUT ANY WARRANTY;
|
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with
| this program; if not, write to the Free Software Foundation, Inc., 51
| Franklin St, Fifth Floor, Boston, MA 02110, USA
|
| http://www.gnu.de/gpl-ger.html
`-----------------------------------------------------------------------------------------*/

#include "clock.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

//clock (hour,min,sec)
unsigned char clock[3];

//count how many interrupts passed since last clock_do() call
volatile unsigned char clock_intcount;
//stores the nes second/minute flag
volatile unsigned char clock_new_flag;
//used as a timer for uart io (mca25) for example
volatile unsigned char clock_timeout_timer;

void clock_init(){
	clock_new_flag=0; 

	//enable interrupt:
	TIMSK |= _BV(TOIE1);

	//init clock:
	clock[CLOCK_SEC]  = 0;
	clock[CLOCK_MIN]  = 0;
	clock[CLOCK_HOUR] = 0;
	clock_timeout_timer = 0;
	clock_intcount = 0;

	//set prescaler to 1024
	TCCR1B |= (1<<CS10 | 0<<CS11 | 1<<CS12);

	TCNT1 = CLOCK_TCNT;	

	//interrups enable
	sei();
	#if CLOCK_DEBUG
	softuart_puts_progmem("CLK : init done.\r\n");
	#endif
}

//timer overflow interrupt:
SIGNAL (SIG_OVERFLOW1){
	//set timer again:
	TCNT1 = CLOCK_TCNT;

	clock_intcount++;

	if (clock_timeout_timer>0)
		clock_timeout_timer--;

	//set new second flag:
	clock_new_flag |= (1<<CLOCK_SEC);
}

//timer isr needs to be as short as possible (uart 460kbaud!!)
//-> do the clock calc here, assumes that we call clock_do() at least every 60 seconds
//   if you need the correct time, always call clock_do() before using the clock[] array
void clock_do(void){
	unsigned char new_secs;

	new_secs = clock_intcount;

	if (new_secs > 0){
		clock_intcount = clock_intcount - new_secs;

		//increase clock:
		clock[CLOCK_SEC] += new_secs;

		if (clock[CLOCK_SEC] >= 60){
			clock[CLOCK_MIN] += clock[CLOCK_SEC]/60;
			clock[CLOCK_SEC]  = clock[CLOCK_SEC]%60;
		
			//new minute flag
			clock_new_flag |= (1<<CLOCK_MIN);
		}

		if (clock[CLOCK_MIN] >= 60){
			clock[CLOCK_HOUR] += clock[CLOCK_MIN]/60;
			clock[CLOCK_MIN]   = clock[CLOCK_MIN]%60;
			
			//new hour flag:
			clock_new_flag |= (1<<CLOCK_HOUR);
		}	
		
		if (clock[CLOCK_HOUR] >= 24){
			clock[CLOCK_HOUR] = 0;
		}
		#if CLOCK_DEBUG
		softuart_puts_progmem("CLK : added ");
		softuart_put_uint8(new_secs);
		softuart_puts_progmem("s -> ");
		softuart_put_uint8(clock[CLOCK_HOUR]);	  softuart_putc(':');
		softuart_put_uint8(clock[CLOCK_MIN]);	    softuart_putc(':');
		softuart_put_uint8(clock[CLOCK_SEC]);
		softuart_putnewline();
		#endif
	}
}

