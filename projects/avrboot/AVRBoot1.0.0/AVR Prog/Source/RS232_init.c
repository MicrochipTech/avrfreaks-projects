/**************************************************************************************
***************************************************************************************
***
***     File: RS232_init.c
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Windows serial port configuration
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
#include <windows.h>
#include <stdio.h>
#include "RS232_init.h"

COMMPROP pcCommProp;
COMMCONFIG lpCC;
COMMTIMEOUTS lpCommTimeouts;

int init_comm(HANDLE *hPort,DCB *dcb, int baudrate, int bytesize, byte stopbits,BOOL ovrlpd)
{
    BOOL fSuccess=FALSE;
    if (!ovrlpd)
     *hPort = CreateFile( "COM1",
                    GENERIC_READ | GENERIC_WRITE,
                    0,    // must be opened with exclusive-access
                    NULL, // no security attributes
                    OPEN_EXISTING, // must use OPEN_EXISTING
                    0,    // not overlapped I/O
                    NULL  // hTemplate must be NULL for comm devices
                    );
     else 
      *hPort = CreateFile( "COM1",
                    GENERIC_READ | GENERIC_WRITE,
                    0,    // must be opened with exclusive-access
                    NULL, // no security attributes
                    OPEN_EXISTING, // must use OPEN_EXISTING
                    FILE_FLAG_OVERLAPPED,    // overlapped I/O
                    NULL  // hTemplate must be NULL for comm devices
                    );

   if (*hPort == INVALID_HANDLE_VALUE) 
   {
       // Handle the error.
       printf ("CreateFile failed with error %d.\n", GetLastError());
       system("PAUSE");
       return 0;
   }
   
   fSuccess = GetCommState(*hPort, dcb);
   if (!fSuccess) 
   {
      // Handle the error.
      printf ("GetCommState failed with error %d.\n", GetLastError());
      system("PAUSE");
      return 0;
   }
   dcb->BaudRate=baudrate;
   dcb->ByteSize=bytesize;
   dcb->fParity=FALSE;
   dcb->Parity=0;
   dcb->StopBits=stopbits;
   
   SetCommState(*hPort, dcb);
   GetCommState(*hPort, dcb);
   /*printf ("Baudrate %d.\n", dcb->BaudRate);
   printf ("ByteSize %d.\n", dcb->ByteSize);
   printf ("DCBlength %d.\n", dcb->DCBlength);
   printf ("EofChar %d.\n", dcb->EofChar);
   printf ("ErrorChar %d.\n", dcb->ErrorChar);
   printf ("EvtChar %d.\n", dcb->EvtChar);
   printf ("AbortOnError %d.\n", dcb->fAbortOnError);
   printf ("XoffChar %d.\n", dcb->XoffChar);
   printf ("XonChar %d.\n", dcb->XonChar);
   printf ("DsrSensitivityr %d.\n", dcb->fDsrSensitivity);
   printf ("DtrControl %d.\n", dcb->fDtrControl);
   printf ("OutxCtsFlow %d.\n", dcb->fOutxCtsFlow);
   printf ("OutxDsrFlow %d.\n", dcb->fOutxDsrFlow);
   printf ("RtsControl %d.\n", dcb->fRtsControl);*/
   
   if (!GetCommTimeouts(*hPort,&lpCommTimeouts))
    {
       printf ("GetCommTimeouts failed with error %d.\n", GetLastError());
       system("PAUSE");
       return 0;
    }
   /*printf ("ReadIntervalTimeout %d.\n", lpCommTimeouts.ReadIntervalTimeout);
   printf ("ReadTotalTimeoutConstant %d.\n", lpCommTimeouts.ReadTotalTimeoutConstant);
   printf ("ReadTotalTimeoutMultiplier %d.\n", lpCommTimeouts.ReadTotalTimeoutMultiplier);
   printf ("WriteTotalTimeoutConstant %d.\n", lpCommTimeouts.WriteTotalTimeoutConstant);
   printf ("WriteTotalTimeoutMultiplier %d.\n", lpCommTimeouts.WriteTotalTimeoutMultiplier);*/
   lpCommTimeouts.ReadIntervalTimeout=0;
   lpCommTimeouts.ReadTotalTimeoutConstant = 2000;
   lpCommTimeouts.ReadTotalTimeoutMultiplier=0;
   lpCommTimeouts.WriteTotalTimeoutConstant=0;
   lpCommTimeouts.WriteTotalTimeoutMultiplier=0;
   
   
   if (!SetCommTimeouts(*hPort,&lpCommTimeouts))
    {
       printf ("SetCommTimeouts failed with error %d.\n", GetLastError());
       system("PAUSE");
       return 0;
    }
    if (!SetupComm(*hPort,200,200))
       {
       printf ("SetupComm failed with error %d.\n", GetLastError());
       system("PAUSE");
       return 0;
    }
	printf ("Port opened\n");
   return 1;
}

int change_timeout(HANDLE *hPort,int value)
{
     if (!GetCommTimeouts(*hPort,&lpCommTimeouts))
     {
       printf ("GetCommTimeouts failed with error %d.\n", GetLastError());
       system("PAUSE");
       return 0;
     }
     else
       if (value==0)
        lpCommTimeouts.ReadIntervalTimeout=0;
       if (value==1)
          lpCommTimeouts.ReadIntervalTimeout=MAXDWORD;
     return SetCommTimeouts(*hPort,&lpCommTimeouts);
}
