/**************************************************************************************
***************************************************************************************
***
***     File: util.cpp
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Useful stuff
***		Version: 1.0.0
***
***		Change log:
***		2010-03-01: 1.0.0 First release
***
***************************************************************************************
***************************************************************************************
***
***		This file is part of AVRBoot.
***
***		AVRBoot is free software: you can redistribute it and/or modify
***     it under the terms of the GNU General Public License as published by
***     the Free Software Foundation, either version 3 of the License, or
***     (at your option) any later version.
***
***     AVRBoot is distributed in the hope that it will be useful,
***     but WITHOUT ANY WARRANTY; without even the implied warranty of
***     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
***     GNU General Public License for more details.
***
***     You should have received a copy of the GNU General Public License
***     along with AVRBoot.  If not, see <http://www.gnu.org/licenses/>.
***
***
***************************************************************************************
***************************************************************************************
*/
#include "util.h"

const uint8_t aToF[ 6 ] = { 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

uint8_t ASCIToHex( uint8_t c1, uint8_t c2 ) {
	uint8_t output = 0;
	if (( 0x30 <= c1 ) && ( c1 <= 0x39 )) {
		output = ( ( c1 - 0x30 ) << 4 );
	}
	else if (( 0x41 <= c1 ) && ( c1 <= 0x46 )) {
        output = ( aToF[ c1 - 0x41 ] << 4 );
	}
	else if (( 0x61 <= c1 ) && ( c1 <= 0x66 )) {
        output = ( aToF[ c1 - 0x61 ] << 4 );
	}

    if (( 0x30 <= c2 ) && ( c2 <= 0x39 )) {
		output |= ( c2 - 0x30 );
	}
	else if (( 0x41 <= c2 ) && ( c2 <= 0x46 )) {
        output |= aToF[ c2 - 0x41 ];
	}
	else if (( 0x61 <= c2 ) && ( c2 <= 0x66 )) {
        output |= aToF[ c2 - 0x61 ];
	}
	return output;
}
