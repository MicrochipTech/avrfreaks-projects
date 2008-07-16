/*
,-----------------------------------------------------------------------------------------.
| io/mpf10
|-----------------------------------------------------------------------------------------
| this file implements some basic functions to control a led flasher (mpf10 from sony ericsson)
| - see http://avr.auctionant.de/mpf-10_handyblitz/ for hw description
|
| Author   : {{removed according to contest rules}}
|            -> circuitcellar.com avr design contest 2006
|            -> Entry #AT2616
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
#include "mpf10.h"
#include "../main.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

//intialise
void mpf10_init(void){
	//set io pin as output:
	MPF10_ENABLE_PORT |= (1<<MPF10_ENABLE_PIN);

	//set lamp off:
	MPF10_DISABLE();
}

//fade light in/out (100khz(?) soft pwm)
void mpf10_fade(signed char fade){
	unsigned char time_on;

	if (fade == MPF10_FADE_IN)
		time_on = 0;
	else
		time_on = 100;

	//fade slow in !
	for(unsigned char i=0; i<100; i++){
		for(unsigned char l=0; l<4; l++){
			for(unsigned char t=0; t<100; t++){
				if (t<=time_on)
					MPF10_ENABLE();
				else
					MPF10_DISABLE();
				//delay:
				_delay_us(20);
			}
		}
		time_on+=fade;
	}
}
