/*
* Program: Multitherm - A multiple DS18x20 thermometer control and logging device
* File Name: delay.c
* Purpose: Precise AVR delay functions without the avr/delay overhead
*
* Description:
*  Precise Delay Functions 
*  V 0.5, Martin Thomas, 9/2004
*
*  In the original Code from Peter Dannegger a timer-interrupt
*  driven "timebase" has been used for precise One-Wire-Delays.
*  My loop-approach is less elegant but may be more usable 
*  as library-function. Since it's not "timer-dependent"
*  See also delay.h.
*
*  Inspired by the avr-libc's loop-code
*
*  RTB - 09/20 - added 1us delay timed for ATMega running at 8MHz
*
*
*    Programmer: Tom Blough
*   Host System: ATMega16L tested with both internal RC osc at 8MHz
*                and external 8MHz crystal
*  Date Created: 2007/09/20
*      Revision: $WCREV$  $WCDATE$
* Modifications:
*/

// $Header: C:\\RCS\\C\\Documents\040and\040Settings\\tblough\\My\040Documents\\Electronics\\AVR\\Projects\\multitherm\\delay.c,v 1.1 2007-10-28 19:34:29+08 tblough Exp tblough $

#include <avr/io.h>
#include <avr/io.h>
#include <inttypes.h>

#include "delay.h"

void delayloop32(uint32_t loops) 
{
  __asm__ volatile ( "cp  %A0,__zero_reg__ \n\t"  \
                     "cpc %B0,__zero_reg__ \n\t"  \
                     "cpc %C0,__zero_reg__ \n\t"  \
                     "cpc %D0,__zero_reg__ \n\t"  \
                     "breq L_Exit_%=       \n\t"  \
                     "L_LOOP_%=:           \n\t"  \
                     "subi %A0,1           \n\t"  \
                     "sbci %B0,0           \n\t"  \
                     "sbci %C0,0           \n\t"  \
                     "sbci %D0,0           \n\t"  \
                     "brne L_LOOP_%=            \n\t"  \
                     "L_Exit_%=:           \n\t"  \
                     : "=w" (loops)              \
					 : "0"  (loops)              \
                   );                             \
    
	return;
}
