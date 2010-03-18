/**************************************************************************************
***************************************************************************************
***
***     File: hexFile.cpp
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Implements the CFile class that represents an intel hex file.
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

#include "hexFile.h"
#include "CLine.h"
#include <cstdio>


class CFile {
	FILE *file;
	CLine *currentLine;
public:      
	CFile( void );
	FILE *File( void );
	void Open( const char *fileName );
	void Close( void );
	BOOL isFileOpen();
	CLine *CurrentLine( void );
};


CFile f;

CFile::CFile( void ) {
	this->file = NULL;
	this->currentLine = new CLine();
}


FILE *CFile::File( void ) {
	return file;
}


void CFile::Open( const char *fileName ) {
     file = fopen( fileName, "r" );
}


void CFile::Close( void ) {
     fclose( file );
     file = NULL;
}


BOOL CFile::isFileOpen( void ) {
    return (!( file ==  NULL ));
}


CLine* CFile::CurrentLine( void ) {
	return currentLine;
}


BOOL hexFile_Open( const char *fileName ) {
     f.Open( fileName );
     return f.isFileOpen();
}


void hexFile_Close( void ) {
     f.Close();
}


void hexFile_ReadNextLine( void ) {
    fgets((char*)f.CurrentLine()->Data() , 0xffff, f.File());
	f.CurrentLine()->ExtractLineInfo();
}


RecordTypes_t hexFile_CurrentLineRecordType( void ) {
	return f.CurrentLine()->RecordType();
}


BOOL hexFile_EndOfFile( void ) {
	return ( hexFile_CurrentLineRecordType() == END_OF_FILE_RECORD );
}


uint8_t hexFile_CurrentLineLength( void ) {
	return f.CurrentLine()->Length();
}


uint16_t hexFile_CurrentLineStartAddress( void ) {
	return f.CurrentLine()->StartAddress();
}


uint8_t hexFile_CurrentLineGetData( uint8_t byte ) {
	return f.CurrentLine()->GetData( byte );
}
