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

#ifndef _MY_TYPES___
#define _MY_TYPES___

#include <avr/io.h>
#include <avr/interrupt.h>

#define NOP 	asm volatile("nop")
#define SLEEP 	asm volatile("sleep")

#define false 	0x00
#define FALSE 	0x00

#define true 	0x01
#define TRUE 	0x01

typedef unsigned char	u08;
typedef signed char	s08;
typedef unsigned int 	u16;
typedef signed int 	s16;
typedef unsigned long	u32;
typedef signed long	s32;
typedef u08 BOOL;
typedef u08 bool;

#define HIBYTE(x)		((x&0xff00)>>8)
#define LOBYTE(x)		(x&0xff)
#define TO_HIBYTE(x)	(u16)(x<<8)
#define TO_LOBYTE(x)	(u08)(x>>8)
#define TO_UPPER(x)		((x>='a') && (x<='z'))?(x+('A'-'a'):(x)
#define NULL 0x00

#define cbi(a, b) a &= ~(1<<(b))
#define sbi(a, b) a |= (1<<(b))
#define outp(a, b) b = (a)
#define inp(a) (a)

#endif

