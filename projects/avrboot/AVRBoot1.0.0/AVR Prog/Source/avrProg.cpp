/**************************************************************************************
***************************************************************************************
***
***     File: avrProg.cpp
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Controls the programming session
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

#include "avrProg.h"
#include <iostream.h>
#include <windows.h>
#include "cserialport.h"
#include "programmer.h"

static BOOL ConnectToTarget( void );
static BOOL StartTimer( PHANDLE phTimerQueue, WAITORTIMERCALLBACK func );
static char WaitForCommand( void );

VOID CALLBACK TimerCallback( PVOID lpParameter, BOOLEAN TimerOrWaitFired );


BOOL avrProg_Program( const char *file ) {
	
    printf("*** AVR Prog v1.0, (C) 2010 Peter Eckstrand  ***\n");
    printf("\nOpening serial port...");
    
	/* Initialize serial port 38400, 8N1*/
	SerialPort_Init();
    
    /* Wait for target message on the serial port */
	if ( ConnectToTarget() == FAILURE ) {
        printf("No target found\nExiting\n");
		return FALSE;
	}

	/* User has 8 s to input command. If timed out, 'e' is entered automatically */
	/* and program exits */
	if ( WaitForCommand() == 'e' ) {
		return FALSE;
	}

	/* Programming request was input. Start the programming session */
	if ( ProgrammingSessionSetup( file ) == FAILURE ) {
		return FALSE;
	}
	
	if ( ProgrammingSessionRun() == FAILURE ) {
		return FALSE;
	}
	
	/* We're done. Let's close */
	ProgrammingSessionClose();
	
	return SUCCESS;
}


static BOOL ConnectToTarget( void ) {
    uint8_t rxData;
	DWORD received = 0;
	uint8_t timeout = 0;
	char input;

	cout << "Please connect target and perform a reset.\n";
    /* The bootloader in the target sends a 'b' at startup. */
    /* Wait for the 'b'. Maximum wait time is 10s */
	do {
	    received = SerialPort_Read(&rxData, 1);
		if ( received == 0 ) {
			timeout++;
		}
		else {
			if ( rxData == 'b' ) {
    	    	printf("Target connection established\n");
				break;
			}
			if ( rxData == 'a' ) {
    	    	printf("Target starting application\nExiting\n");
            	return FALSE;
			}
		}
	} while ( timeout < 5 );

	return ( timeout < 5 );
}


static BOOL StartTimer( PHANDLE phTimerQueue, WAITORTIMERCALLBACK func ) {
    
    HANDLE hTimer;
    *phTimerQueue = CreateTimerQueue();

    if ( NULL == *phTimerQueue ) {
        printf("CreateTimerQueue failed (%d)\n", GetLastError());
        return FAILURE;
    }

    if (!CreateTimerQueueTimer(&hTimer,*phTimerQueue,func,0,200,200,0)) {
        printf("CreateTimerQueueTimer failed (%d)\n", GetLastError());
        return FAILURE;
    }
    
    return SUCCESS;
}


VOID CALLBACK TimerCallback( PVOID lpParameter, BOOLEAN TimerOrWaitFired ) {
    DWORD written;
    static uint8_t counter = 0;
    INPUT_RECORD input[3];
    KEY_EVENT_RECORD kEcharEvt;
    KEY_EVENT_RECORD kNewLineEvt;
    KEY_EVENT_RECORD kReturnEvt;
    
	kEcharEvt.bKeyDown = TRUE;
	kEcharEvt.uChar.AsciiChar = 'e';
	kNewLineEvt.bKeyDown = TRUE;
    kNewLineEvt.uChar.AsciiChar = '\n';
    kReturnEvt.bKeyDown = TRUE;
    kReturnEvt.uChar.AsciiChar = '\r';
	input[0].EventType = KEY_EVENT;
	input[0].Event.KeyEvent = kEcharEvt;
	input[1].EventType = KEY_EVENT;
	input[1].Event.KeyEvent = kNewLineEvt;
	input[2].EventType = KEY_EVENT;
	input[2].Event.KeyEvent = kReturnEvt;
    
    /* This callback is called every 200 ms. After 40 calls, i.e 8 s, 'e' is   */
    /* written to the console, which will terminate the application. */
    if ( ++counter == 40 ) {
		WriteConsoleInput( GetStdHandle(STD_INPUT_HANDLE), input, 3, &written);
		printf("Timeout - Target starts application\n");
	}
	else {
		printf(".");
	}
}

static char WaitForCommand( void ) {
    char input;
    HANDLE hTimerQueue = NULL;
    
    /* Start the 8s timeout */
    if ( StartTimer( &hTimerQueue, TimerCallback ) == FAILURE ) {
		return 'e';
	}
	
	printf( "\nEnter command (program: p, exit: e): " );

	/* Wait for user input. Only 'p' or 'e' is accepted. */
	do {
		cin >> input;
	} while (( input != 'p' ) && ( input != 'e' ));

    DeleteTimerQueueEx(hTimerQueue,NULL);
    return input;
}


