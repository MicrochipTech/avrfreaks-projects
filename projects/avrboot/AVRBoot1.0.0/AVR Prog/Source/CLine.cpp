/**************************************************************************************
***************************************************************************************
***
***     File: CLine.cpp
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Implements the CLine class for representing the lines in an intel
***		hex file
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
#include "CLine.h"
#include "util.h"

CLine::CLine( void ) {
	this->startAddress = 0x0000;
	this->length = 0;
	this->recordType = UNDEFINED_RECORD;
}

RecordTypes_t CLine::RecordType( void ) {
	return recordType;
}


uint8_t* CLine::Data( void ) {
	return data;
}


uint8_t CLine::Length( void ) {
	return length;
}


uint16_t CLine::StartAddress( void ) {
	return startAddress;
}


uint8_t CLine::GetData( uint8_t offset ) {
	return ASCIToHex( data[ 9 + 2 * offset ], data[ 10 + 2 * offset ] );
}


void CLine::ExtractLineInfo( void ) {
	recordType = (RecordTypes_t)ASCIToHex( data[ 7 ], data[ 8 ] );
	length = ASCIToHex( data[ 1 ], data[ 2 ] );
	startAddress = ( ASCIToHex( data[ 3 ], data[ 4 ] ) << 8 ) | ASCIToHex( data[ 5 ], data[ 6 ] );
}
