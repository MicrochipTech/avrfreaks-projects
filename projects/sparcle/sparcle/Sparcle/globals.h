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
* FILE: globals.h
*
*
* This file includes refernce to global variables stored in FLASH ROM and other globals
*
*
*
************************************************************************************************/


#ifndef  GLOBAL_VARIABLES_INC_
#define  GLOBAL_VARIABLES_INC_

/**********************************************************************************
* variable "operation_mode" determines the operation mode of the program
* 0 stands for AUTO, 1 stands for MONITOR, 2 stands for MANUAL, 3 stands for SLEEP
***********************************************************************************/

enum operation_mode_t {automatic=0,monitor=1,manual=2,sleep=3,automatic_2=4};

extern enum operation_mode_t operation_mode;
extern enum operation_mode_t operation_m_backup;

/***********************************************************************************
* Following strings and pointer arrays store information an allert
* text to be sent to user through UART. Data is stored in program memory.
***********************************************************************************/

extern uint8_t ASCII_number_table[256][3] PROGMEM; //2 dimmensinal array for 8bit integer to ASCII no. conversion
//TC
//extern uint8_t recived_data[5];                //field of recived data through UART interface
//EOTC
extern uint8_t p;                                  //p (power) constant of regulator routine
extern uint8_t requested_value;                    //requested value of enlightement
extern uint8_t ADC_input;                          //data achieved from the ADC_input routine
extern uint8_t PWM_input;                          //data sent to PWM_output routine
extern uint8_t monitor_counter;                    //data verification counter for monitor mode
extern uint8_t light1;                             //1st Light value
extern uint8_t light2;                             //2nd Light value
extern uint8_t light3;                             //3rd Light value
extern uint8_t light4;                             //4th light value
extern uint8_t hysteresis;                         //hysteresis value

extern char auto_text[] PROGMEM;                           //Char array anouncing P regulation mode
extern char monitor_text[] PROGMEM;                        //Char array anouncing monitor mode
extern char manual_text[] PROGMEM;                         //Char array anouncing manual mode
extern char sleep_text[] PROGMEM;                          //Char array anouncing sleep mode
extern char auto_2_text[] PROGMEM;                         //Char array anouncing 2nd regulation mode
extern char* mode_text_ptr[5] PROGMEM;                     //Array of pointers to the arrays above.

extern char err_text[] PROGMEM;                            //Char array containing the general error text
extern char err_text_0[] PROGMEM;                          //following arrays contain specific error texts
extern char err_text_1[] PROGMEM;
extern char err_text_2[] PROGMEM;
extern char err_text_3[] PROGMEM;
extern char err_text_4[] PROGMEM;
extern char err_text_5[] PROGMEM;
extern char err_text_6[] PROGMEM;
extern char* err_text_ptr[7] PROGMEM;                      //Array of pointers to the arrays above.


#endif
