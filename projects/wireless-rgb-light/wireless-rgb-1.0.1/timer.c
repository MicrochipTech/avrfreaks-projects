/******************************************************************************
 *
 * ID      : $Id: timer.c 26 2010-11-21 12:47:24Z SH $ 
 * Revsion : $Rev: 26 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-11-21 13:47:24 +0100 (So, 21 Nov 2010) $   
 *
 ******************************************************************************
 *
 * License
 *
 * Copyright (c) 2010 by Stefan Heesch, http://www.heesch.net
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 ******************************************************************************/


/******************************************************************************
 * Include files
 ******************************************************************************/
 #include <avr/interrupt.h>
 #include "timer.h"
 #include "led.h"

/******************************************************************************
 * Initialize Timer 1
 *
 * -> Generate an interrupt with 31.25 kHz
 *
 ******************************************************************************/
 void InitializeTimer (void)
 {

  /* Initialize counting register and pwm index
   */
	TCNT1 = 0;
   pwm = 0;
   SetColors(0,0,0);

  /* With 16 MHz cpu clock set the prescaler to 8 and start the timer
   */
   TCCR1B |= (1<<WGM12) | (0<<CS12 | 1<<CS11 | 0<<CS10);

  /* Set compare register to 64
   */
	OCR1A = 64;

 };



/******************************************************************************
 * SetColors
 ******************************************************************************/
 void SetColors( unsigned char r, unsigned char g, unsigned char b)
 {
    TIMER1_COMPA_INT_DISABLE;

    red   = r;
    green = g;
    blue  = b;

    TIMER1_COMPA_INT_ENABLE;
 }


/******************************************************************************
 * ISR for Timer 1 / Compare match A 
 ******************************************************************************/
 ISR (TIMER1_COMPA_vect)
 {
    TIMER1_COMPA_INT_DISABLE;

    if ( ++pwm == 0 )
    {
       if ( red > 0 )
       {
           LED_On ( RED   );
       }
       
       if ( green > 0 )
       {
           LED_On ( GREEN );  
       }
       
       if ( blue > 0)
       {
           LED_On ( BLUE );
       }
    }

   /* Pulse width modulation of LEDs
    */
    if ( pwm > red  )
    {
       LED_Off( RED );
    }
        
    if ( pwm > green )
    {
       LED_Off ( GREEN );
    }

    if ( pwm > blue )
    {
       LED_Off ( BLUE );
    }

    TIMER1_COMPA_INT_ENABLE;


 }
