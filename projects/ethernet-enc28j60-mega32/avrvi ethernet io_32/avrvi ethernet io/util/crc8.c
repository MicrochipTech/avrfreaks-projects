/*
,-----------------------------------------------------------------------------------------.
| util/crc8
|-----------------------------------------------------------------------------------------
| this file implements some crc8 routines
| - based on code by peter danegger
| - ..._rev0x07 is used by mca25 mux protocol! <---- NOT REALLY TESTED! THERE MIGHT BE A HIDDEN BUG!
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
#include "crc8.h"

unsigned char crc8_calc(unsigned char *data, unsigned char crc_start, unsigned int len){
	unsigned int i;
	unsigned char crc = crc_start;

	for(i=0; i<len; i++){
		crc = crc8_calc_byte(crc,data[len]); //pgm_read_byte(&crc8_lookuptable[crc ^ data[len]]);
	}
	return crc;
}

//crc8, reversed, poly 0x07
unsigned char crc8_calc_byte_rev0x07(unsigned char crc, unsigned char data){
	data ^= crc;
	crc = 0;
	if( data & 0x01 ) crc  = 0x91;
	if( data & 0x02 ) crc ^= 0xE3;
	if( data & 0x04 ) crc ^= 0x07;
	if( data & 0x08 ) crc ^= 0x0E;
	if( data & 0x10 ) crc ^= 0x1C;
	if( data & 0x20 ) crc ^= 0x38;
	if( data & 0x40 ) crc ^= 0x70;
	if( data & 0x80 ) crc ^= 0xE0;
  return crc;
}

unsigned char crc8_calc_byte(unsigned char crc, unsigned char data){
	data ^= crc;
	crc = 0;
	if( data & 0x01 ) crc = 0x5E;
	if( data & 0x02 ) crc ^= 0xBC;
	if( data & 0x04 ) crc ^= 0x61;
	if( data & 0x08 ) crc ^= 0xC2;
	if( data & 0x10 ) crc ^= 0x9D;
	if( data & 0x20 ) crc ^= 0x23;
	if( data & 0x40 ) crc ^= 0x46;
	if( data & 0x80 ) crc ^= 0x8C;
  return crc;
}
