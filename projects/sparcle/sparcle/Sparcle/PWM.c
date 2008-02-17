/******************************************************************************
* PROGRAM:     Lučka - Spark
* PURPOSE:     Experimetnal enlightement regulator
* COPYRIGHT:   Radovid Drobnjak, University of Maribor
* LICENSE:     Licenced under the BSD license, see below.
******************************************************************************/

/***************************************************************************************************
Copyright (c) 2007, Radovid Drobnjak, University of Maribor

This material was originally written and compiled by Radovid Drobnjak at
Faculty of Electrical Engieering and Computer Science and Faculty of Economics and Business, University of Maribor, Slovenia, 2007

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the project nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************************************/

/************************************************************************************************
* FILE: PWM.c
*
*
* This file includes PWM related functions.
*
*
*
************************************************************************************************/



// --- GLOBAL VARIABLES (EXTERN) ---
#include "globals.h"


/********************************************************************************
*
*   Function name : PWM_init
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Sets the needed flags to ensure correct working of PWM
*
*********************************************************************************/

void PWM_init (void)
{

   /* initialization of output pin*/
   PWM_PIN_ON;
   
   /* initialization of the timer/counter */
   TIMER_PWM_INIT;
   
   OCR_PWM_INIT_VALUE;

}


/********************************************************************************
*
*   Function name : PWM_output
*
*   Returns :       None
*
*   Parameters :    8 bit decimal number which controlls the width of the pulses 
*
*   Purpose :       the integer parameter is written to Output Compare Register (OCR).
*
*********************************************************************************/

void PWM_output (uint8_t PWM_input) 
{
   /*
   this is for shutting up the pin completely (if desired output value is 0, although it stil emmits some current
   */
   if (PWM_input == 0) {
      PWM_PIN_OFF; 
      }
   else {
      PWM_PIN_ON;
      OCR_PWM = PWM_input;
      }

   //TC1
   //static uint8_t i=0;
   //OCR_PWM = i++;
   //EOTC1

}
