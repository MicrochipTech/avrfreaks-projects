#ifndef _TIMER_H
#define _TIMER_H

/******************************************************************************
 *
 * ID      : $Id: timer.h 11 2010-10-24 16:42:52Z SH $ 
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
 * Includes
 ******************************************************************************/	
 #include <avr/io.h>
 #include "config.h"
 

/******************************************************************************
 * Timer interrupts
 ******************************************************************************/	
 #define TIMER1_COMPA_INT_ENABLE 	TIMSK |=  (1 << OCIE1A)
 #define TIMER1_COMPA_INT_DISABLE 	TIMSK &= ~(1 << OCIE1A)


/******************************************************************************
 * PWM control for LEDs
 ******************************************************************************/	
 volatile unsigned char pwm;
 volatile unsigned char red;
 volatile unsigned char green;
 volatile unsigned char blue;


/******************************************************************************
 * Function prototypes
 ******************************************************************************/	
 void InitializeTimer (void);
 void SetColors( unsigned char red, unsigned char green, unsigned char blue);
 
#endif //_TIMER_H
