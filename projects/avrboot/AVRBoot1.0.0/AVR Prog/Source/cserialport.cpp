/**************************************************************************************
***************************************************************************************
***
***     File: cserialport.cpp
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Implementation of a serial port wrapper class.
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

#include "cserialport.h" // class's header file
#include "RS232_init.h"
#include <windows.h>

CSerialPort port;

// class constructor
CSerialPort::CSerialPort()
{
	hPort = NULL;
}

// class destructor
CSerialPort::~CSerialPort()
{
	CloseHandle( hPort );
}


void CSerialPort::Init( void ) {
    init_comm( &hPort,&dcb, 38400, 8, ONESTOPBIT, FALSE);
}

DWORD CSerialPort::Write( uint8_t *buf, uint8_t length ) {
	DWORD written;
	WriteFile(hPort,buf,length,&written,NULL);
	return written;
}


DWORD CSerialPort::Read( uint8_t *buf, uint8_t length ) {
	DWORD read;
	ReadFile(hPort,buf,length,&read,NULL);
	return read;
}

void SerialPort_Init( void ) {
	port.Init();
}


void SerialPort_Close( void ) {
    delete( &port );
}

DWORD SerialPort_Write( uint8_t *buf, uint8_t length ) {
	return port.Write( buf, length );
}


DWORD SerialPort_Read( uint8_t *buf, uint8_t length ) {
	return port.Read( buf, length );
}
