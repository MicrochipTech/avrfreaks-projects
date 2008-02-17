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
* FILE: ADC.c
*
*
* This file includes ADC related functions
*
*
*
************************************************************************************************/


// --- GLOBAL VARIABLES (EXTERN) ---
#include "globals.h"


/********************************************************************************
*
*   Function name : ADC_init
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Sets the needed flags to ensure correct working of ADC
*
*   Notes:          For details see init.h
*
*********************************************************************************/

void ADC_init (void)
{

   /* initialization of the demuxer */
   DEMUXER_INIT;

   /* prescaler initialization */

   ADC_PRESCALER_INIT;
   // Turn the ADC ON
   ADC_INIT;

   /* initialization of the current source for the LDR */
   VCP_INIT;


}

/********************************************************************************
*
*   Function name : ADC_output
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Returns the outut value of ADC (bytes ADCH and ADCL)
*
*   Notes:
*
*********************************************************************************/


uint8_t ADC_output(void)
{

   // --- initialization of variables ---


   uint8_t ADC_final;   //the final ADC value
   uint8_t i;           //first ADC loop counter
   static uint8_t j;    //second ADC loop counter
   uint8_t k;           //field management counter (second loop)
   uint16_t ADCs = 0; //result of the preeliminary sampling loop (first loop)

   uint16_t ADC_sum=0; // sum of the values stored in the ADC_field[] field

   // --- initialization of static variables ---

   // variable which determines if the second loop is on the first run or not.
      static enum run_t { first, next } run=first;

   // field which stores the sampled data from first loop and is used for data processing in the second loop
   static uint8_t ADC_field[LOOPS];
   /* START SINGLE CONVERSION -- warmup of ADC*/
   ADC_SINGLE_START;


   /*********************************************************************************
   * First ADC loop
   * The preeliminary sampling and averaging loop, the recommended no. of LOOPS is 16
   * Max value at refreshing hrequency of 39Hz is 32, see init.h
   **********************************************************************************/

   for(i=0;i<32;i++)
   {
      while(!(ADCSRA & 0x10));
      ADCs += ADCH;
   }
   ADCs = ADCs >> 5;     // average the samples



   /*********************************************************************
   * Second ADC loop
   * In the first run we create a field of measurements. 
   * In every nex run we update the field and calculate the average value 
   **********************************************************************/


   switch (run)
   {
      // in the first run we 
      case first:
      ADC_field[j]=ADCs;
      if (j==LOOPS) {
      run=next;
      }
      j++;
      break;

      case next:
         for(k=0;k<LOOPS;k++)
      {
      ADC_field[k]=ADC_field[k+1];
      }
      ADC_field[LOOPS-1]=ADCs;
            for(k=0;k<LOOPS;k++) {
         ADC_sum += ADC_field[k];
      }
      break;

   }
   ADC_final=ADC_sum/LOOPS;

//TC
   // if (run==first){
   //    ADC_final=ADCs;
   //    }
//EOTC

   return ADC_final;

}
