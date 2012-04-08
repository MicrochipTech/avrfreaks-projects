/**********************
 * irtst_rcv.c file *
 **********************/

/************************************************************************

   This program is similar to LIRC' mode2 utility.
   Program enters endless loop where reads and prints on screen
   all data received from the IRmega hardware over virtual
   serial port.
   
   Received data can be saved to text file using output redirection.
   For example, the command:
        
        irtst_rcv > irtst.log

   saves all received data to the irtst.log file. As all comments
   (including line numbers) go to the standard error output, the file
   will contain just raw data. This allows simple modification to
   source file for irtst_snd utility. Program ends if CTRL+C pressed.

**************************************************************************
*  RS232 LIBRARY (http://www.teuniz.net/RS-232) included
*
* Author: Teunis van Beelen
* Copyright (C) 2005, 2006, 2007, 2008, 2009 Teunis van Beelen
* teuniz@gmail.com
***************************************************************************
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 2 of the License.
***************************************************************************/

#ifndef rs232_INCLUDED
#define rs232_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#ifdef __linux__

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#else

#include <windows.h>

#endif

int OpenComport(int, int);
int PollComport(int, unsigned char *, int);
int SendByte(int, unsigned char);
int SendBuf(int, unsigned char *, int);
void CloseComport(int);
void cprintf(int, const char *);
int IsCTSEnabled(int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

#ifdef __linux__   /* Linux */

int Cport[22],
    error;

struct termios new_port_settings,
       old_port_settings[22];

char comports[22][13]={
     "/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4",
     "/dev/ttyS5","/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9",
     "/dev/ttyS10","/dev/ttyS11","/dev/ttyS12","/dev/ttyS13",
     "/dev/ttyS14","/dev/ttyS15","/dev/ttyUSB0","/dev/ttyUSB1",
     "/dev/ttyUSB2","/dev/ttyUSB3","/dev/ttyUSB4","/dev/ttyUSB5"};

int OpenComport(int comport_number, int baudrate) {
  int baudr;

  if((comport_number>21)||(comport_number<0)) {
    fprintf(stderr, "illegal comport number\n");
    return(1);
  }

  switch(baudrate) {
    case      50 : baudr = B50; break;
    case      75 : baudr = B75; break;
    case     110 : baudr = B110; break;
    case     134 : baudr = B134; break;
    case     150 : baudr = B150; break;
    case     200 : baudr = B200; break;
    case     300 : baudr = B300; break;
    case     600 : baudr = B600; break;
    case    1200 : baudr = B1200; break;
    case    1800 : baudr = B1800; break;
    case    2400 : baudr = B2400; break;
    case    4800 : baudr = B4800; break;
    case    9600 : baudr = B9600; break;
    case   19200 : baudr = B19200; break;
    case   38400 : baudr = B38400; break;
    case   57600 : baudr = B57600; break;
    case  115200 : baudr = B115200; break;
    case  230400 : baudr = B230400; break;
    case  460800 : baudr = B460800; break;
    case  500000 : baudr = B500000; break;
    case  576000 : baudr = B576000; break;
    case  921600 : baudr = B921600; break;
    case 1000000 : baudr = B1000000; break;
    default      : fprintf(stderr, "invalid baudrate\n");
                   return(1);
                   break;
  }

  Cport[comport_number] = open(comports[comport_number], O_RDWR | O_NOCTTY | O_NDELAY);
  if(Cport[comport_number]==-1) {
    perror("unable to open comport ");
    return(1);
  }

  error = tcgetattr(Cport[comport_number], old_port_settings + comport_number);

  if(error==-1) {
    close(Cport[comport_number]);
    perror("unable to read portsettings ");
    return(1);
  }

  memset(&new_port_settings, 0, sizeof(new_port_settings));  /* clear the new struct */

  new_port_settings.c_cflag = baudr | CS8 | CLOCAL | CREAD;
  new_port_settings.c_iflag = IGNPAR;
  new_port_settings.c_oflag = 0;
  new_port_settings.c_lflag = 0;
  new_port_settings.c_cc[VMIN] = 0;      /* block untill n bytes are received */
  new_port_settings.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */
  error = tcsetattr(Cport[comport_number], TCSANOW, &new_port_settings);

  if(error==-1) {
    close(Cport[comport_number]);
    perror("unable to adjust portsettings ");
    return(1);
  }
  return(0);
}

int PollComport(int comport_number, unsigned char *buf, int size) {
  int n;

  /* __STRICT_ANSI__ is defined when the -ansi option is used for gcc */

#ifndef __STRICT_ANSI__     /* SSIZE_MAX is defined in limits.h */
  if(size>SSIZE_MAX)  size = (int)SSIZE_MAX;
#else
  if(size>4096)  size = 4096;
#endif

  n = read(Cport[comport_number], buf, size);

  return(n);
}

int SendByte(int comport_number, unsigned char byte) {
  int n;

  n = write(Cport[comport_number], &byte, 1);
  if(n<0)  return(1);

  return(0);
}

int SendBuf(int comport_number, unsigned char *buf, int size) {
  return(write(Cport[comport_number], buf, size));
}

void CloseComport(int comport_number) {
  close(Cport[comport_number]);
  tcsetattr(Cport[comport_number], TCSANOW, old_port_settings + comport_number);
}

/*
  Constant  Description
  
  TIOCM_LE  DSR (data set ready/line enable)
  TIOCM_DTR DTR (data terminal ready)
  TIOCM_RTS RTS (request to send)
  TIOCM_ST  Secondary TXD (transmit)
  TIOCM_SR  Secondary RXD (receive)
  TIOCM_CTS CTS (clear to send)
  TIOCM_CAR DCD (data carrier detect)
  TIOCM_CD  Synonym for TIOCM_CAR
  TIOCM_RNG RNG (ring)
  TIOCM_RI  Synonym for TIOCM_RNG
  TIOCM_DSR DSR (data set ready)
*/

int IsCTSEnabled(int comport_number) {
  int status;

  status = ioctl(Cport[comport_number], TIOCMGET, &status);

  if(status&TIOCM_CTS) return(1);
  else return(0);
}

#else         /* windows */

HANDLE Cport[16];

char comports[16][10]={
     "\\\\.\\COM1",  "\\\\.\\COM2",  "\\\\.\\COM3",  "\\\\.\\COM4",
     "\\\\.\\COM5",  "\\\\.\\COM6",  "\\\\.\\COM7",  "\\\\.\\COM8",
     "\\\\.\\COM9",  "\\\\.\\COM10", "\\\\.\\COM11", "\\\\.\\COM12",
     "\\\\.\\COM13", "\\\\.\\COM14", "\\\\.\\COM15", "\\\\.\\COM16"
};

char baudr[64];

int OpenComport(int comport_number, int baudrate) {
  if((comport_number>15)||(comport_number<0)) {
    fprintf(stderr, "illegal comport number\n");
    return(1);
  }

  switch(baudrate) {
    case     110 : strcpy(baudr, "baud=110 data=8 parity=N stop=1"); break;
    case     300 : strcpy(baudr, "baud=300 data=8 parity=N stop=1"); break;
    case     600 : strcpy(baudr, "baud=600 data=8 parity=N stop=1"); break;
    case    1200 : strcpy(baudr, "baud=1200 data=8 parity=N stop=1"); break;
    case    2400 : strcpy(baudr, "baud=2400 data=8 parity=N stop=1"); break;
    case    4800 : strcpy(baudr, "baud=4800 data=8 parity=N stop=1"); break;
    case    9600 : strcpy(baudr, "baud=9600 data=8 parity=N stop=1"); break;
    case   19200 : strcpy(baudr, "baud=19200 data=8 parity=N stop=1"); break;
    case   38400 : strcpy(baudr, "baud=38400 data=8 parity=N stop=1"); break;
    case   57600 : strcpy(baudr, "baud=57600 data=8 parity=N stop=1"); break;
    case  115200 : strcpy(baudr, "baud=115200 data=8 parity=N stop=1"); break;
    case  128000 : strcpy(baudr, "baud=128000 data=8 parity=N stop=1"); break;
    case  256000 : strcpy(baudr, "baud=256000 data=8 parity=N stop=1"); break;
    default      : fprintf(stderr, "invalid baudrate\n");
                   return(1);
                   break;
  }

  Cport[comport_number] = CreateFileA(comports[comport_number],
                      GENERIC_READ|GENERIC_WRITE,
                      0,                          /* no share  */
                      NULL,                       /* no security */
                      OPEN_EXISTING,
                      0,                          /* no threads */
                      NULL);                      /* no templates */

  if(Cport[comport_number]==INVALID_HANDLE_VALUE) {
    fprintf(stderr, "unable to open comport\n");
    return(1);
  }

  DCB port_settings;
  memset(&port_settings, 0, sizeof(port_settings));  /* clear the new struct  */
  port_settings.DCBlength = sizeof(port_settings);

  if(!BuildCommDCBA(baudr, &port_settings)) {
    fprintf(stderr, "unable to set comport dcb settings\n");
    CloseHandle(Cport[comport_number]);
    return(1);
  }

  if(!SetCommState(Cport[comport_number], &port_settings)) {
    fprintf(stderr, "unable to set comport cfg settings\n");
    CloseHandle(Cport[comport_number]);
    return(1);
  }

  COMMTIMEOUTS Cptimeouts;

  Cptimeouts.ReadIntervalTimeout         = MAXDWORD;
  Cptimeouts.ReadTotalTimeoutMultiplier  = 0;
  Cptimeouts.ReadTotalTimeoutConstant    = 0;
  Cptimeouts.WriteTotalTimeoutMultiplier = 0;
  Cptimeouts.WriteTotalTimeoutConstant   = 0;

  if(!SetCommTimeouts(Cport[comport_number], &Cptimeouts)) {
    fprintf(stderr, "unable to set comport time-out settings\n");
    CloseHandle(Cport[comport_number]);
    return(1);
  }
  return(0);
}

int PollComport(int comport_number, unsigned char *buf, int size) {
  int n;

  if(size>4096)  size = 4096;

/* added the void pointer cast, otherwise gcc will complain about */
/* "warning: dereferencing type-punned pointer will break strict aliasing rules" */

  ReadFile(Cport[comport_number], buf, size, (LPDWORD)((void *)&n), NULL);

  return(n);
}

int SendByte(int comport_number, unsigned char byte) {
  int n;

  WriteFile(Cport[comport_number], &byte, 1, (LPDWORD)((void *)&n), NULL);
  if(n<0)  return(1);

  return(0);
}

int SendBuf(int comport_number, unsigned char *buf, int size) {
  int n;

  if(WriteFile(Cport[comport_number], buf, size, (LPDWORD)((void *)&n), NULL)) 
    return(n);

  return(-1);
}

void CloseComport(int comport_number) {
  CloseHandle(Cport[comport_number]);
}

int IsCTSEnabled(int comport_number) {
  int status;

  GetCommModemStatus(Cport[comport_number], (LPDWORD)((void *)&status));

  if(status&MS_CTS_ON) return(1);
  else return(0);
}

#endif

/* sends a string to serial port */

void cprintf(int comport_number, const char *text) {
  while(*text != 0)   SendByte(comport_number, *(text++));
}


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

void zzzz(int n) {  // sleep for n msec
#ifdef _WIN32
	Sleep(n);
#else
	usleep(1000*n);
#endif
}


/********************************************************************/


int main(void) {

   int comport=6;   // COM7 (/dev/ttyS6 in Linux)
   int bdrate=115200;
   int i, n, x, to, cntr;
   char c;
   unsigned long int data;  // received data buffer

   unsigned char HexTab[256] = {  // character decoding table
   
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
   };


   if(OpenComport(comport, bdrate)) {  // initialize port 
	  return(0);
   }

   fprintf(stderr, "Receiving data from COM%d\n\n", comport+1);

   
   while (1) {   

      cntr = 0; // reset item counter 
      data = 0; // clear data buffer

      cprintf(comport, "$");       //reset buffers

      while (1) { // endless wait for first char
         n = PollComport(comport, &c, 1); // poll RX buffer
         if (n>0) break;
         zzzz(5); // sleep 5 msec
      }
      
      to = 0; // reset timeout counter

      while (to<20)  {
         if(n>0) {
            x = HexTab[c];
            if (x<16) {   // hex char
               data = (data<<4) + x; // hex char
            } else {      // flag
               data &= 0xFFFFFF; // mask out unused bits
               fprintf(stderr, "%03d: ", cntr);
               printf("%06lx%c\n", data, c);
               data = 0;  // clear data
               cntr += 1; 
            }
            to = 0; // fresh data so reset timeout counter
            
         } else { // no character received

            to += 1; // no data so increment timeout counter ...
            zzzz(5); // ... and wait for 5 msec
         }
         n = PollComport(comport, &c, 1); // get next char

      } // end if no data for 100ms

      printf("\n%d items received\n", cntr-1);
      fprintf(stderr, "\n");
      
   } // end of while loop 
   
} // end of main
