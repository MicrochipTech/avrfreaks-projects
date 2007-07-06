/*
Copyright David Gustafik, 2007

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <compat/deprecated.h>
#include "defs.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ISR(ADC_vect)
{
	unsigned char a = ADCH,in,b;
	signed char delta = adc_prev - a;	//calculate the difference between the last value and the current one. 
	wait_for_adc();//most likely useles...oh well, it's safer
	start_adc();	//kick of the ADC again. Although it would be most likely best to leave it in free running mode, i dont wanna experiment

	if (enabled == 1) return; //if the lock is already opened, we simply get out

	if(delta > MIN_DELTA || delta < -MIN_DELTA)
	{
		reset_timer();//we reset the nullification timer
		in = 'A';	//determining which value are we at
		if(a > 63 && a < 128) in = 'B';
		if(a > 127 && a < 192) in = 'C';
		if(a > 191) in = 'D';	//a bit brutal, i know that the oh so leet coders will find this a stupid way, but it works
	
		PORTD = 0x10*_BV((a&0xC0)/0x40) | _BV(0);//light the aproprioate leds

		adc_prev = a;	//set the previous value to the current value		
		if(prev_val == in) return;//skip the rest of the function if the value is equal to the last one
		prev_val = in;

		for(b=SEQ_LENGTH -1; b!=0; b--)
		{
			seq_comp[b] = seq_comp[b-1];//push the buffer one place ahead
		}
		seq_comp[0] = in;	//store the current value infront of the buffer

		show_sequence();	//send the current sequence out to the user
		if(compare_seq())	//match the combination and if it fits, open the lock
		{
			enabled = 1;	//the opening of the doors of hell...or just some lock or whatever. 
		}

	}
	
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ISR(USART_RXC_vect)
{
	unsigned char in = usart_getchar();//we free the buffer
	if(enabled == 0) 	//if the lock is not unlocked we ignore everything that comes our way...
	{
		incursions++;//someone was trying to gain acces without
		printf("Please unlock the lock before attemting to do any settings\n");
		return;
	}

	if(in == 'R')	// if the lock is unlocked and we received a reset command than we will enable reset
	{
		printf("Reseting codelock settings to default...");
		eeprom_write_byte((uint8_t*)SEQ_LENGTH+1,FIRTS_BOOT);	//tell the device that the codelock has not yet been set
		wdt_enable(1);//watchdog resets the codelock
		while(1);
	}

	printf("WTF?\n");//send the wtf signal to the user as he is providing unknown commands...
	return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ISR(TIMER1_OVF_vect)	//each 10 seconds or so of inactivity, the combination on the lock is reseted
{
	if(enabled == 1) return;	//if the lock is unlocked we ignore the interrupt
	printf("Reseting lock...\n");
	clear_sequence();	//reset the lock
	return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(void)
{
	io_init();//initialize the IO ports, timers and stuff

	stdout = &usart;	//set the output of the printf function to the usart

	load_settings();	//load the default settings

	start_adc();	//kick off the ADC	

	sei();	//enable interrupts

	while(1)
	{
		while(enabled == 0);	//wait until the lock is opened
		unlock();	//unlock
		wait(42000);	//wait approx 30seconds
		lock();	//locks the lock
		enabled = 0;	//resets the lock
	};
}
