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

/** @file circBuffer.h Simple implementation of a circular buffer. The buffer
		is implemented as a data structure, with a fixed buffer size. Macros
		are supplied for writing to and reading from the buffer. The buffer
		size must be a power of 2, for the macros to operate correctly. This
		is done for efficiency reasons. Three buffers, with sizes of 8, 16
		and 32 are currently defined. Since this a definitions only
		implementation, there is no cost at all, if the definitions do not
		appear in code. The buffers are used for buffering data from
		communications interrupt service routines (UART Rx, UART Tx, SPI Rx,
		SPI Tx).
		*/

#ifndef __CIRCULAR_BUFFER_HEADER__
#define __CIRCULAR_BUFFER_HEADER__

#include <inttypes.h>

/*******************************************************************************
* General Definitions
*******************************************************************************/

#define CIRC_BUFFER_DEFINITION(size) \
typedef struct				\
{							\
	uint8_t data[(size)];	\
	uint8_t wIndex;			\
	uint8_t rIndex;			\
} CircBuffer##size

#define CIRC_BUFFER_INIT(buffer)	buffer.wIndex = buffer.rIndex = 0;

#define CIRC_BUFFER_WRITE(size, buffer, byte) \
			buffer.data[buffer.wIndex & (size - 1)] = byte; \
			buffer.wIndex++

#define CIRC_BUFFER_READ(size, buffer, byte)	\
			byte = buffer.data[buffer.rIndex & (size - 1)]; \
			buffer.rIndex++

#define CIRC_BUFFER_UNREAD_BYTES(size, buffer) \
			buffer.wIndex >= buffer.rIndex ? buffer.wIndex - buffer.rIndex : \
									(size - 1) - buffer.rIndex + buffer.wIndex


/*******************************************************************************
* Circular Buffer Size 8
*******************************************************************************/

CIRC_BUFFER_DEFINITION(8);

#define CIRC_BUFFER8_INIT(buffer) CIRC_BUFFER_INIT(buffer)
#define CIRC_BUFFER8_WRITE(buffer, byte) CIRC_BUFFER_WRITE(8, buffer, byte)
#define CIRC_BUFFER8_READ(buffer, byte) CIRC_BUFFER_READ(8, buffer, byte)
#define CIRC_BUFFER8_UNREAD_BYTES(buffer) CIRC_BUFFER_UNREAD_BYTES(8, buffer)

/*******************************************************************************
* Circular Buffer Size 16
*******************************************************************************/

CIRC_BUFFER_DEFINITION(16);

#define CIRC_BUFFER16_INIT(buffer) CIRC_BUFFER_INIT(buffer)
#define CIRC_BUFFER16_WRITE(buffer, byte) CIRC_BUFFER_WRITE(16, buffer, byte)
#define CIRC_BUFFER16_READ(buffer, byte) CIRC_BUFFER_READ(16, buffer, byte)
#define CIRC_BUFFER16_UNREAD_BYTES(buffer) CIRC_BUFFER_UNREAD_BYTES(16, buffer)

/*******************************************************************************
* Circular Buffer Size 32
*******************************************************************************/

CIRC_BUFFER_DEFINITION(32);

#define CIRC_BUFFER32_INIT(buffer) CIRC_BUFFER_INIT(buffer)
#define CIRC_BUFFER32_WRITE(buffer, byte) CIRC_BUFFER_WRITE(32, buffer, byte)
#define CIRC_BUFFER32_READ(buffer, byte) CIRC_BUFFER_READ(32, buffer, byte)
#define CIRC_BUFFER32_UNREAD_BYTES(buffer) CIRC_BUFFER_UNREAD_BYTES(32, buffer)

#endif //__CIRCULAR_BUFFER_HEADER__
