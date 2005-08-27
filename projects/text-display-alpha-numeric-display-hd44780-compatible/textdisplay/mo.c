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

#include "textdisplay.h"
#include "textdisplay_extras.h"

int main(void) {
	uint16_t i;
	uint16_t j;
	
	// Init display
	td_init(20,2);
	
	// Turn display on
	td_displayControl(TD_CTRL_DISPLAY_ON|TD_CTRL_CURSOR_OFF|TD_CTRL_BLINKING_OFF);
	
	// Print a string
	td_putStr("Test of display",0,0);

	td_putCh('a',0,1);
	td_putCh('b',1,1);
	td_putCh('c',2,1);
	td_putCh('d',3,1);
	td_putCh('e',4,1);
	td_putCh('1',5,1);
	td_putCh('2',6,1);
	td_putCh('3',7,1);
	td_putCh('4',8,1);
	td_putCh('5',9,1);

	// Wait
	for(i=0;i<3000;i++) {
		for(j=0;j<60000;j++);
	}

	// Demonstrate bar
	td_clearRow(0);
	td_putStr("Test of bar",0,0);
	for(i=0x00;i<0xFF;i++) {
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		td_putBar(i,20,0,1);
	}
	for(i=0xFF;i>0x00;i--) {
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		for(j=0;j<60000;j++);
		td_putBar(i,20,0,1);
	}
	
	td_clearDisplay();
	
	td_moveCursor(2,0);
	td_putN('T',4);
	td_putN('H',4);
	td_putN('E',4);
	td_moveCursor(2,1);
	td_putN('E',4);
	td_putN('N',4);
	td_putN('D',4);
	
	// Wait
	for(i=0;i<3000;i++) {
		for(j=0;j<60000;j++);
	}
	
	// Turn display off
	td_displayControl(TD_CTRL_DISPLAY_OFF|TD_CTRL_CURSOR_OFF|TD_CTRL_BLINKING_OFF);

	return(0);
}
