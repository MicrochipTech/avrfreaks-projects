
/****************************************************************************
 Title  :   C Test program for the DELAY FUNCTIONS library (test_delay.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      26/Oct/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/


#if AVRGCC_VERSION == 330
#include <avr/io.h>
#else
#include <io.h>
#endif
#include "delay.h"



void main(void)
{
unsigned int x=0;
#define TIME_L10_SEC ( 10000*TIME_L1_MS  )

DDRB=0xFF;
x=1000;

       DELAY_US(100);

while(1)
    {
       PORTB=0;
       DELAY_LONG(TIME_L10_SEC);      /* 10 SECOND LEDS ON IN STK500 */
       PORTB=0xFF;
       DELAY_MS(1000);                /* 1 SECOND LEDS OFF IN STK500 */
       PORTB=0;
       DELAY_MS(10000);               /* 10 SECOND LEDS ON IN STK500 */
       PORTB=0xFF;
       DELAY_MS(x);                   /* 1 SECOND LEDS OFF IN STK500 */
       PORTB=0;
       delay_ms(10000);               /* 10 SECOND LEDS ON IN STK500 */
       PORTB=0xFF;
       delay_ms(1000);                /* 1 SECOND LEDS OFF IN STK500 */

    }


return;
}
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

