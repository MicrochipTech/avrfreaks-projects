/**************************************************************************************
***************************************************************************************
***
***     File: hexFile.h
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Header file
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

#include <cstdio>
#include "defines.h"



BOOL hexFile_Open( const char *fileName );
void hexFile_Close( void );
BOOL hexFile_EndOfFile( void );
void hexFile_ReadNextLine( void );
RecordTypes_t hexFile_CurrentLineRecordType( void );
uint8_t hexFile_CurrentLineLength( void );
uint16_t hexFile_CurrentLineStartAddress( void );
uint8_t hexFile_CurrentLineGetData( uint8_t byte );
