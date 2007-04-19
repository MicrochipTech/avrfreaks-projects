/*  This file is a part of ButterflySDK. ButterflySDK is a set of software
	modules used to program the Butterfly Evaluation Board for ATMEL.
	Emphasis is given in code re-use and in maintainability. ButterflySDK
	is compiled with GCC AVR compiler. Supporting tools are written in Python
	programming language.
	Copyright (C) 2007, Kostas Giannakakis

    ButterflySDK is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ButterflySDK is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ButterflySDK; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** @file c_ext.h Defines some useful extensions to the C language (Boolean
		type definition, setting and clearing I/O pins)
*/

#ifndef __C_EXT_HEADER__
#define __C_EXT_HEADER__

/** Boolean type definition. */
typedef uint8_t BOOL;

#define TRUE	1
#define FALSE	0

/** @def sbiBF(port,bit) Sets a port pin*/
#define sbiBF(port,bit)  ((port) |= (1<<(bit)))

/** @def cbiBF(port,bit) Clears a port pin*/
#define cbiBF(port,bit)  ((port) &= ~(1<<(bit)))

#endif //__C_EXT_HEADER__
