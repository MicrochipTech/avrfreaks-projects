/******************************************************************************
 *
 * ID      : $Id: main.c 11 2010-10-24 16:42:52Z SH $ 
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
 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "config.h"
 #include "timer.h"
 #include "net.h"


/******************************************************************************
 * Inialize hardware
 ******************************************************************************/
 void Initialize()
 {
  /* Initialize timer and network
   */
   InitializeTimer();
	InitializeNetwork();

  /* Enables interrupts
   */	
	ETH_INT_ENABLE;
   TIMER1_COMPA_INT_ENABLE;
	sei(); 
 }


/******************************************************************************
 * Main loop
 ******************************************************************************/
 int main(void)
 {  

  /* Setup hardware
   */  
   Initialize();

  /* Main loop: handles incoming ethernet frames
   */
	while(1)
	{
		HandleNetworkData();
   }
    
  /* We should never come here ...
   */
   return(0);
 }

/******************************************************************************
 * The end.
 ******************************************************************************/
