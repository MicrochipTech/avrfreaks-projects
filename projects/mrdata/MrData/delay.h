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
 
#ifndef _DELAY_H_
#define _DELAY_H_

#include "types.h"

#define DELAY_1SEC  	{delay_ms(1000);}
void delay_us(u16 n);
void delay_ms(u16 n);

#endif


