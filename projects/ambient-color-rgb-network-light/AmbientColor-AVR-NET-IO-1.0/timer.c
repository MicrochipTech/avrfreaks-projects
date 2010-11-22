/******************************************************************************
 *
 * ID      : $Id: timer.c 11 2010-10-24 16:42:52Z SH $ 
 * Revsion : $Rev: 11 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-10-24 18:42:52 +0200 (So, 24 Okt 2010) $   
 *
 ******************************************************************************
 *
 * The MIT License
 *
 * Copyright (c) 2010 by Stefan Heesch, http://www.heesch.net
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
