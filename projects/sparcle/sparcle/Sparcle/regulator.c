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
* FILE: regulator.c
*
*
* This file includes Regulation related functions. All functions here are part of the interrupt
* handler
*
*
*
************************************************************************************************/


// --- GLOBAL VARIABLES (EXTERN) ---
#include "globals.h"

/********************************************************************************
*
*   Function name : P_regulator 
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Actual regulation implementation
*
*********************************************************************************/

void P_regulator(void)
{
   
   
   monitor_counter = monitor_counter+1;
   
   int tmp;
   uint8_t light; // calculated light value
   
   
   // reading and writing of data
   ADC_input=ADC_output();
   PWM_output(PWM_input);
   //tmp=ADC_input;
   
   // regulation implementation, simple P regulator
   
   light=(255-ADC_input);
   
   tmp=(requested_value-light)*p;
   if (tmp<0) tmp=0;
   if (tmp>255) tmp=255;
   
   PWM_input=tmp;

}

/********************************************************************************
*
*   Function name : manual
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Takes care of the regulation process
*
*********************************************************************************/
void manual_m (void)
{
   ADC_input=ADC_output();
   PWM_output(PWM_input);
   PWM_input=requested_value;
   
   

}

void sleep_m(void)
{

}


/********************************************************************************
*
*   Function name : regulation
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Takes care of the regulation process
*
*********************************************************************************/

void regulation (void)
{
   if (operation_mode != operation_m_backup) {           //in case of change of mode, turn off al lights
      PWM_PIN_OFF;
      DIGI_OUT0_OFF;
      DIGI_OUT1_OFF;
      DIGI_OUT2_OFF;
      DIGI_OUT3_OFF;
      }
   
   if (operation_mode==automatic || operation_mode==monitor){
      P_regulator();
      }
   if (operation_mode==manual){
      manual_m();
      }
   if (operation_mode==sleep) {
      sleep_m();
      }
   if (operation_mode==automatic_2) {
      regulator();
      }
}


void regulator (void)
{
   
   
   //uint8_t comp_value;                    //value used for compare operations
   ADC_input=ADC_output();                //reading from ADC
   //comp_value=(255-ADC_input);          //Calculating the compare value
   
   static uint8_t previous_ADC=0;         //used hysteresis implementation
   
   if ((previous_ADC-ADC_input)>0) {         //is the measured light decreasing?
      if (ADC_input<(light1+hysteresis)) {
         DIGI_OUT0_OFF;
         DIGI_OUT1_OFF;
         DIGI_OUT2_OFF;
         DIGI_OUT3_OFF;
      }
      else if ((ADC_input>=(light1+hysteresis))&&(ADC_input<(light2+hysteresis))) {
         DIGI_OUT0_ON;
         DIGI_OUT1_OFF;
         DIGI_OUT2_OFF;
         DIGI_OUT3_OFF;
      }
      else if ((ADC_input>=(light2+hysteresis))&&(ADC_input<(light3+hysteresis))) {
         DIGI_OUT0_ON;
         DIGI_OUT1_ON;
         DIGI_OUT2_OFF;
         DIGI_OUT3_OFF;
      }
      else if ((ADC_input>=(light3+hysteresis))&&(ADC_input<(light4+hysteresis))) {
         DIGI_OUT0_ON;
         DIGI_OUT1_ON;
         DIGI_OUT2_ON;
         DIGI_OUT3_OFF;
      }
         else {
         DIGI_OUT0_ON;
         DIGI_OUT1_ON;
         DIGI_OUT2_ON;
         DIGI_OUT3_ON;
      }
   }


   else if ((previous_ADC-ADC_input)<0) {         //is the measured light increasing?


      if (ADC_input >= (light4-hysteresis)){
         DIGI_OUT0_ON;
         DIGI_OUT1_ON;
         DIGI_OUT2_ON;
         DIGI_OUT3_ON;
      }
   
      else if ((ADC_input >= (light3-hysteresis)) && (ADC_input < (light4-hysteresis))) {
         DIGI_OUT0_ON;
         DIGI_OUT1_ON;
         DIGI_OUT2_ON;
         DIGI_OUT3_OFF;
      }
      else if ((ADC_input >= (light2-hysteresis))&&(ADC_input < (light3-hysteresis))) {
         DIGI_OUT0_ON;
         DIGI_OUT1_ON;
         DIGI_OUT2_OFF;
         DIGI_OUT3_OFF;
      }
      else if ((ADC_input >= (light1-hysteresis))&&(ADC_input < (light2-hysteresis))) {
         DIGI_OUT0_ON;
         DIGI_OUT1_OFF;
         DIGI_OUT2_OFF;
         DIGI_OUT3_OFF;
      }
   
      else {
         DIGI_OUT0_OFF;
         DIGI_OUT1_OFF;
         DIGI_OUT2_OFF;
         DIGI_OUT3_OFF;
      }
   
   }
   
   else ;

}




