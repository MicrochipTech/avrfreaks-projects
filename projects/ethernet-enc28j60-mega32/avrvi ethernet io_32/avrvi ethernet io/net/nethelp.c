/*
,-----------------------------------------------------------------------------------------.
| net/nethelp
|-----------------------------------------------------------------------------------------
| this file implements some helper functions
| - checksum generation -> originally written bei ulrich radig (his rtl8019 webserver)
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

#include "nethelp.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

unsigned int nethelp_checksum(unsigned char *buffer, unsigned int len, unsigned long csum32){
	unsigned int  res16 = 0x0000;
	unsigned char data_hi;
	unsigned char data_lo;
	
	//loop through buffer
	while(len > 1){
		//read data_hi/lo from pointer:
		data_hi = *buffer++;
		data_lo = *buffer++;

		//generate 16bit var from both:
		res16 = ((data_hi << 8) + data_lo);
		
		//add to csum
		csum32 = csum32 + res16;

		//decrement length (we have read 2 bytes from ptr)
		len -=2;
	}

	//is len odd -> add again last hi value & lo = 0
	if(len > 0){
		//read data_hi/lo from pointer & generate 16bit var:
		data_hi = *buffer;
		res16   = data_hi<<8;

		//add to csum
		csum32 = csum32 + res16;
	}
	
	//to complement calc:
	csum32 = ((csum32 & 0x0000FFFF)+ ((csum32 & 0xFFFF0000) >> 16));
	csum32 = ((csum32 & 0x0000FFFF)+ ((csum32 & 0xFFFF0000) >> 16));	
	res16  =~(csum32 & 0x0000FFFF);
		
	return (res16);
}
