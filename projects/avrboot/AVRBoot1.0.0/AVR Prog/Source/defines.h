/**************************************************************************************
***************************************************************************************
***
***     File: defines.h
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Definitions
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
#ifndef _DEFINES_H
#define _DEFINES_H
#include <windows.h>


typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef enum {
    DATA_RECORD = 0,
    END_OF_FILE_RECORD,
	EXTENDED_SEGMENT_ADDRESS_RECORD,
	START_SEGMENT_ADDRESS_RECORD,
	EXTENDED_LINEAR_ADDRESS_RECORD,
	START_LINEAR_ADDRESS_RECORD,
	UNDEFINED_RECORD
} RecordTypes_t;

#define TRUE 			1
#define FALSE 			0
#define SUCCESS 		TRUE
#define FAILURE 		FALSE
#define MAX_LENGTH  	256
#define PAGES       	128
#define BYTES_IN_PAGE	128

#endif
