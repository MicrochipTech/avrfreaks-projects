/**************************************************************************************
***************************************************************************************
***
***     File: programmer.cpp
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Reads the image hex file and transmits it to the target using a
***     serial communication protocol
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

#include "programmer.h"
#include "cserialport.h"
#include "hexFile.h"

static void FlashBufferClear( void );
static BOOL FlashBufferFill( void );
static BOOL FlashBufferGetLineData( void );

static uint8_t nPages;
static uint8_t nBytesInPage;
static uint8_t *pFlash = NULL;

BOOL ProgrammingSessionSetup( const char *file ) {
	uint8_t cmd;
	
    /* Send programming request to target */
    cmd = 0x70;
	SerialPort_Write(&cmd,1);
	
	/* Receive the flash size bytes */
	SerialPort_Read( &nBytesInPage, 1 );
	SerialPort_Read( &nPages, 1 );
	
	pFlash = (uint8_t*)malloc( nPages * ( nBytesInPage + 1 ) );
	
	if ( pFlash == NULL ) {
        printf("Could not allocate local flash buffer\n");
		return FAILURE;
	}
	
    /* Open hex file */
	if ( hexFile_Open( file ) == FAILURE ) {
        printf("Could not open file\n");
        return FAILURE;
	}

    printf("File opened\n");

	/* Prepare the local flash image buffer */
	FlashBufferClear();
	
	printf("\n");
	/* Get data from hex file */
    return FlashBufferFill();
}


BOOL ProgrammingSessionRun( void ) {
    uint8_t rxData;
    DWORD written;
    DWORD received;
    uint8_t page;
	
    for ( page = 0; page < nPages; ++page) {
		/* First byte 0? Send this page to target */
        if ( pFlash[ page * ( nBytesInPage + 1 )] == 0 ) {
			/* Send page number */
            SerialPort_Write( &page, 1 );

			/* Send the page data */
			SerialPort_Write( &pFlash[ (page * ( nBytesInPage + 1 )) + 1 ], nBytesInPage );

			/* Wait for write confirmation */
			rxData = 0x00;
            SerialPort_Read( &rxData, 1 );
            
			if ( rxData == 'w' ) {
				printf("Page %i written\n", page);
			}
			else {
                printf("Did not receive page write confirmation from target\n");
                return FAILURE;
			}
		}
	}

	/* All modified pages written. Send termination character */
    rxData = 0xff;
    SerialPort_Write( &rxData, 1 );

    /* Wait for application start */
    SerialPort_Read( &rxData, 1 );
	printf("Starting application\n");

    return SUCCESS;
}


void ProgrammingSessionClose( void ) {
    hexFile_Close();
    SerialPort_Close();
}


static void FlashBufferClear( void ){
    for (int i = 0; i < ( nPages * ( nBytesInPage + 1 ) ); ++i) {
		pFlash[ i ] = 0xff;
	}
}


static BOOL FlashBufferFill( void ) {
    /* Read each line of the input hex file, put the data in correct page */
	while ( hexFile_EndOfFile() == FALSE ) {
		hexFile_ReadNextLine();
		printf( "Recordtype: %i, StartAddress: 0x%04x, Length: %i ",
			hexFile_CurrentLineRecordType(), hexFile_CurrentLineStartAddress(),
			hexFile_CurrentLineLength() );

		if ( hexFile_CurrentLineRecordType() == DATA_RECORD ) {
			if ( FlashBufferGetLineData() == FAILURE ) {
				return FAILURE;
			}
		}
	}
	return SUCCESS;
}


static BOOL FlashBufferGetLineData( void ) {
	uint16_t page;
	uint16_t offset;
	/* For each byte in current line: calculate page and offset, and copy to buffer */
	for ( int byte = 0; byte < hexFile_CurrentLineLength(); ++byte ) {
		page = ( hexFile_CurrentLineStartAddress() + byte ) / nBytesInPage;
		offset = ( hexFile_CurrentLineStartAddress() + byte ) - ( page * nBytesInPage );
		if ( page >= nPages  ) {
            printf("\nError: hexfile contains more pages than target\n");
			return FAILURE;
		}
		if ( offset >= nBytesInPage ) {
            printf("\nError: hexfile pagesize is larger than target page size\n");
		}
		pFlash[ page * ( nBytesInPage + 1 ) + offset + 1 ] = hexFile_CurrentLineGetData( byte );
		pFlash[ page * ( nBytesInPage + 1 ) ] = 0;
		printf("%02X",hexFile_CurrentLineGetData( byte ));
	}
	printf("\n");
	return SUCCESS;
}
