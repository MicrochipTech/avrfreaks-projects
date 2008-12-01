/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\source\os\serial_tc.c $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.1 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:45:42 $
 *
 *  Copyright:    Copyright (C) 2000-2002 Stefan Heesch (heesch@cybervillage.de)
 *
 *  This software/hardware is free software/hardware; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the License, or (at your
 *  option) any later version.
 *
 *  This software is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 *  PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Description:  Implementation of serial port API
 *
 *  This implementation is based on communications routines in turbo C by
 *  by Peter Ibbotson / Borland
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\source\os\serial_tc.d $
 *  
 *    MicroLIB - serial port functions for MS-DOS / Turbo C++
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:45:42
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:50:18
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

 #ifndef __WIN32__
 #ifdef __TURBOC__

/*
==========================
c.language/listings #115, from barryn, 8544 chars, Sat Aug 29 17:17:28 1987
--------------------------
TITLE: Communications routines in Turbo C
 
   The following code shows how to take advantage of some of
the TurboC extensions to the C language to do asynchronous
communications (without having to write supporting assembly-
language routines).  The code was supplied by Peter Ibbotson of
Borland.  After the communications-related functions themselves,
you'll find a small main() section of code that uses those functions
to implement a rudimentary glass-TTY terminal program.  (I say
rudimentary to mean that control characters are not recognized and
there are no file transfer, dialing directory, etc. features.)
 
   First, here's Serialtc.C:
*/
 
/*       SERIALTC.C       */
 
#include        <os/serialtc.h>

#include        <dos.h>
#include        <stdio.h>
#include        <conio.h>
#include        <bios.h>
#include        <process.h>
 
int SError;
int portbase=0;
void interrupt (*oldvects[2])();
char ccbuf[SBUFSIZ];
int startbuf=0;
int endbuf=0;

/*      this does the hard work (handling interrupts)           */
 
void interrupt com_int (void)
 
{ disable ();
  if ((inportb (portbase+IIR) & RX_MASK)==RX_ID)
    { if (((endbuf+1) & 0x1fff)==startbuf)
      SError=BUFOVFL;
      ccbuf[endbuf++]=inportb (portbase+RX);
      endbuf&=0x1fff;
    };
  outportb (ICR,EOI);
  enable ();
}
 
/* this routine returns the currently value in the buffer */
 
int getccb(void)
 
{ int res;
  if (endbuf==startbuf) return (-1);
  res=(int) ccbuf[startbuf];
  startbuf=(startbuf+1) % SBUFSIZ;
  return (res);
}
 
void setvects (void)
 
{ oldvects[0]=getvect(0x0b);
  oldvects[1]=getvect(0x0c);
  setvect(0x0b,com_int);
  setvect(0x0c,com_int);
}
 
void resvects (void)
 
{ setvect(0x0b,oldvects[0]);
  setvect(0x0c,oldvects[1]);
}
 
void i_enable (int pnum)
 
{ int c;
  disable();
  c = inportb (portbase+MCR) | MC_INT;
  outportb (portbase+MCR,c);
  outportb (portbase+IER,RX_INT);
  c = inportb (IMR) & (pnum==2 ? IRQ3 : IRQ4);
  outportb (IMR,c);
  enable();
}
 
void i_disable (void)
 
{ int c;
  disable ();
  c=inportb (IMR) | ~IRQ3 | ~IRQ4;
  outportb (IMR,c);
  outportb (portbase + IER,0);
  c=inportb (portbase+MCR) & ~MC_INT;
  outportb (portbase+MCR,c);
  enable ();
}
 
void comon (void)
 
{  int c,pnum;
   pnum = portbase == COM1BASE ? 1 : 2;
   i_enable (pnum);
   c=inportb (portbase + MCR) | DTR | RTS;
   outportb (portbase + MCR,c);
}
 
void initserial (void)
 
{ endbuf=startbuf=0;
  setvects();
  comon();
};
 
void comoff (void)
 
{ i_disable ();
  outportb (portbase+MCR,0);
}
 
void closeserial(void)
 
{ comoff();
  resvects();
};
 
/* this outputs a serial character              */

int SerialOut (char x)
 
{ long timeout = 0x0000ffff;
  outportb (portbase+MCR,OUT2|DTR|RTS);
  /* wait for clear to send     */
  while ((inportb(portbase + MSR) & CTS)==0)
    if ((--timeout)==0) return (-1);
  timeout=0x0000ffff;
  /* wait for outport register to clear         */
  while ((inportb(portbase+LSR) & DSR)==0)
    if ((--timeout)==0) return (-1);
  disable ();
  outportb (portbase+TX,x);
  enable ();
  return (0);
}
 
/* this routine sets which port we are working with     */
 
int SetPort (int Port)
 
{ int far * RS232_Addr;
  int Offset;
 
  switch (Port)                         /* sort out the base address    */
  { case 1 : Offset=0x0000; break;      /* only ports one & two allowed */
    case 2 : Offset=0x0002; break;
    default : return (-1);
  };
  RS232_Addr=MK_FP(0x0040,Offset);      /* find out where the port is   */
  if (*RS232_Addr==0) return (-1);      /* if it ain't there return (-1)*/
  portbase=*RS232_Addr;                 /* otherwise set portbase       */
  return (0);
}
 
/* this routine sets the speed; will accept funny baud rates   */
 
int SetSpeed (int Speed)
 
{ char c;
  int divisor;
  if (Speed==0) return (-1);            /* avoid divide by zero */
  else divisor=(int)(115200L/Speed);
  if (portbase==0) return (-11);
  disable ();
  c=inportb (portbase+LCR);
  outportb (portbase+LCR,(c|0x80));                     /* set DLAB     */
  outportb (portbase+DLL,(divisor & 0x00ff));   /* set divisor  */
  outportb (portbase+DLH,((divisor>>8)&0x00ff));
  outportb (portbase+LCR,c);
  enable();
  return (0);
}
 
/*      This routine set the LCR                */
 
int SetOthers (int Parity,int Bits,int StopBit)
 
{ int temp;
  if (portbase==0) return (-1);
  if ((Parity<NO_PAR) || (Parity>OD_PAR)) return (-1);
  if ((Bits<5) || (Bits>8)) return (-1);
  if ((StopBit<1) || (StopBit>2)) return (-1);
  temp=Bits-5;
  temp|=((StopBit==1) ? 0x00 : 0x04);
  switch (Parity)
  { case NO_PAR : temp |= 0x00; break;
    case OD_PAR : temp |= 0x08; break;
    case EV_PAR : temp |= 0x18; break;
  }
  disable();            /* turn off interrupts */
  outportb (portbase+LCR,temp);
  enable();             /* turn em back on */
  return (0);
}

/*      This routine sets the ports                     */

int setserial (int Port,int Speed,int Parity,int Bits,int StopBit)

{ if (SetPort(Port)==-1) return(-1);
  if (SetSpeed(Speed)==-1) return(-1);
  if (SetOthers(Parity,Bits,StopBit)==-1) return (-1);
  return (0);
}

/*
==========================
c.language/listings #115, from barryn, 8544 chars, Sat Aug 29 17:17:28 1987

END

*/

 #endif
 #endif

