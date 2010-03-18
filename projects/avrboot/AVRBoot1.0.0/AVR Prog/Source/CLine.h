/**************************************************************************************
***************************************************************************************
***
***     File: CLine.h
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Definition of the CLine class
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

#include "defines.h"
/* Line class representing the lines in an Intel hex file. */

class CLine {
    uint16_t startAddress;
	uint8_t length;
	RecordTypes_t recordType;
	uint8_t data[ MAX_LENGTH ];
public:
	CLine( void );
	void SetStartAddress( uint16_t address );
	uint16_t StartAddress( void );
	void SetLength( uint8_t length );
	uint8_t Length( void );
	void SetRecordType( RecordTypes_t recordType );
	RecordTypes_t RecordType( void );
	uint8_t *Data( void );
	void ExtractLineInfo( void );
	uint8_t GetData( uint8_t offset );
};
