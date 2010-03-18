/**************************************************************************************
***************************************************************************************
***
***     File: cserialport.h
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Definition of a serial port wrapper class
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

#ifndef CSERIALPORT_H
#define CSERIALPORT_H
#include <windows.h>
#include "defines.h"
/*
 * Serial port wrapper class
 */
class CSerialPort
{
	HANDLE hPort;
	DCB dcb;
	public:
		// class constructor
		CSerialPort();
		// class destructor
		~CSerialPort();
		void Init( void );
		DWORD Write( uint8_t *buf, uint8_t length );
		DWORD Read( uint8_t *buf, uint8_t length );
};


void SerialPort_Init( void );
void SerialPort_Close( void );
DWORD SerialPort_Write( uint8_t *buf, uint8_t length );
DWORD SerialPort_Read( uint8_t *buf, uint8_t length );
#endif // CSERIALPORT_H
