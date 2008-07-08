/**
* @file init.c
*
* Copyright (c) 2004 Atmel.
*
* @brief Ce fichier permet de
*
* This file is included by all source files in order to access to system wide
* configuration.
* @version 1.0 (CVS revision : 1.5)
* @date 2006/03/08 17:02:18 (created on 06/04/2004)
* @author raubree (created by Emmanuel David)

*****************************************************************************/

#include <avr/io.h>

/**
* @brief ports direction configuration, timer 0 configuration, run the PLL, allow interruptions
*/
void init(void) {
    /*************************************************************************************/
    /*           ports direction configuration                                           */
    /*************************************************************************************/

      DDRB = 0xC3;
      DDRC = 0x89;
      DDRD = 0x01;
      DDRE = 0x02;

      PORTC = 0x06;     /* enable pull up */

    /*************************************************************************************/
    /*     Timer 0 Configuration : generates the sampling fréquency                      */
    /*************************************************************************************/
      TCCR0A = (1<<WGM01);   // mode CTC : Clear Timer on Compare
      TCCR0B = (1<<CS02);    // f_quartz = 8 MHz / 256 = 32 kHz
//      OCR0A = 0x20;          // one interruption every ms
      OCR0A = 0x10;          // one interruption every 0.5ms
      TIMSK0 = (1<<OCIE0A);  // allow interruption when timer=compare

      SREG|=0x80;            // allow interruptions,
}
