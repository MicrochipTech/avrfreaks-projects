/* ********************************************************************************
 *  textdisplay module for use with AVR series microcontrollers from Atmel        *
 ******************************************************************************** */
 
/*
    Copyright (C) 2005  Erik Häggström <xpress@xpress.mine.nu>

    Copyright notice:

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <avr/io.h>

#include "textdisplay.conf.h"
#include "textdisplay.h"
#include "textdisplay_extras.h"

#define TD_BAR_BLOCK_CHAR 0xFF
#define TD_BAR_EMPTY_CHAR 0x20
#define TD_BAR_END_CHAR 0x0F
#define TD_BAR_END_CHAR2 0x0E

void td_putBar(uint8_t value, uint8_t size, uint8_t x, uint8_t y) {
	uint8_t bigBlocks;
	uint8_t smallBlocks;
	static uint8_t endChar;
	
	/* Calculate number of blocks */
	bigBlocks=(uint8_t)(((uint16_t)value*size)>>8);
	smallBlocks=(uint8_t)((((uint16_t)value*size*5)>>8)-((uint16_t)bigBlocks*5));

	/* Set address */
	td_setDDaddress(td_getRowAddr(y)+x);
	
	/* Print blocks */
	td_putN(TD_BAR_BLOCK_CHAR,bigBlocks);
	
	endChar=(endChar+1)&0x01;
	if(endChar==0) {
		/* Edit last block */
		td_setCGaddress(TD_BAR_END_CHAR<<3);
		td_putN(~((0x01<<(5-smallBlocks))-1),8);
	
		/* Print last block */
		td_setDDaddress(td_getRowAddr(y)+x+bigBlocks);
		td_put(TD_BAR_END_CHAR);
	} else {
		/* Edit last block */
		td_setCGaddress(TD_BAR_END_CHAR2<<3);
		td_putN(~((0x01<<(5-smallBlocks))-1),8);
	
		/* Print last block */
		td_setDDaddress(td_getRowAddr(y)+x+bigBlocks);
		td_put(TD_BAR_END_CHAR2);
	}
	
	/* Print empty spaces */
	td_putN(TD_BAR_EMPTY_CHAR,size-(bigBlocks+1));
	
}
