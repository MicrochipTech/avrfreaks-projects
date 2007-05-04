/*
 * a.lp_mp3 - Open Source Atmel AVR based MP3 Player
 * Copyright (c) 2003-2004 K. John '2B|!2B' Crispin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
 *
 * Feedback, Bugs.... mail john{AT}phrozen.org
 *
 */ 
 
 // TODO:
 //		add preprocessors for different cpu speeds
 
#include <avr/io.h>
#include "types.h"

// delays n microseconds
// for 8MHZ 8 ticks
// for 4 mhz 4 ticks
// assuming a for loop is 3 commands in asm -->  inc, tst, jmp
// if a loop takes more or less then 3 let me know. this is sort of a guess
void delay_us(u16 n) {
	for(;n>0;n--){
#if F_CPU > 8000000
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
#endif
		NOP;NOP;NOP;NOP;NOP;
	};
};

void delay_ms(u16 m){
	u16 n;
	for(;m>0;m--){
		for(n = 1000 ;n>0;n--){
#if F_CPU > 8000000
			NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
#endif
			NOP;NOP;NOP;NOP;NOP;
		};
	};
};
