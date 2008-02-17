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
* FILE: main.c
*
* This file includes main() and other related functions, mainly connected with the basic modes of
* operation
*
*
*
************************************************************************************************/




/********************************************************************************************
* --- IMPORTANT NOTES ---
* there are some symbols used in the comments which are written in capital letters.
* below is a legend of them, please note that "n" usually means an subsequential integer number.
*
* --- legend: ---
*
* TCn, code inserted for testing purposes,
* EOTCn, end of test code no.: "n"
*
**********************************************************************************************/


// --- LIBRARIES ---
/* libraries from GCC AND avrlibc */
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

// --- HEADER FILES ---
#include "init.h"
#include "main.h"
#include "PWM.h"
#include "PWM.c"
#include "ADC.h"
#include "ADC.c"
#include "regulator.h"
#include "regulator.c"
#include "UART.h"
#include "UART.c"



// --- GLOBAL VARIABLES (EXTERN) ---
#include "globals.h"

/***********************************************************************************
* variable "operation_mode" determines the operation mode of the program
* 0 stands for AUTO, 1 stands for MONITOR, 2 stands for MANUAL, 3 stands for SLEEP
***********************************************************************************/
enum operation_mode_t operation_mode=automatic;
enum operation_mode_t operation_m_backup=automatic;


uint8_t p=9;                               //P constant for regulation routine
uint8_t requested_value=50;                //desired value of enlightement
uint8_t ADC_input;                         //data achieved from the ADC_input routine
uint8_t PWM_input;                         //data sent to PWM_output routine
uint8_t monitor_counter=0;                 //data verification counter for monitor mode
uint8_t light1=150;                        //1st Light value
uint8_t light2=175;                        //2nd Light value
uint8_t light3=200;                        //3rd Light value
uint8_t light4=225;                        //4th Light value
uint8_t hysteresis=1;                      //hysteresis value


/***********************************************************************************
* Following strings and pointer arrays store information an allert
* text to be sent to user through UART. Data is stored in program memory.
***********************************************************************************/

char auto_text[] PROGMEM="Automatic operation mode, P regulation: ";
char monitor_text[] PROGMEM="\n --- Monitor operation mode: ---\n\r\ncIn;cMo;ADC;PWM\n\r";
char manual_text[] PROGMEM="Manual operation mode: ";
char sleep_text[] PROGMEM="Sleep mode. To wake up, press 'o' (on): ";
char auto_2_text[] PROGMEM="Automatic operation mode, open-loop digital regulation: ";
char* mode_text_ptr[5] PROGMEM = {auto_text, monitor_text, manual_text, sleep_text, auto_2_text};

char err_text[] PROGMEM="\n\r Error message: ";
char err_text_0[] PROGMEM="\n\r For shure happened something wierd. This message was not supposed to be seen.\n\r\n\r";
char err_text_1[] PROGMEM="\n\r Packet without data.\n\r\n\r";
char err_text_2[] PROGMEM="\n\r This is a command, not a parameter. Try lowercase.\n\r\n\r";
char err_text_3[] PROGMEM="\n\r This is a operation mode command.\n\r\n\r";
char err_text_4[] PROGMEM="\n\r 'x' returns controller to its  default state (Automatic).\n\r\n\r";
char err_text_5[] PROGMEM="\n\r Invalid input. At end of string press enter (return).\n\r\n\r";
char err_text_6[] PROGMEM="\n\r Unknown command or parameter.\n\r\n\r";
char* err_text_ptr[7] PROGMEM = {err_text_0, err_text_1, err_text_2, err_text_3, err_text_4, err_text_5, err_text_6};

/*************************************************************************************************************
* encoding table for 8bit bin to ASCII conversion, allocated in FLASH ROM memory
**************************************************************************************************************/

uint8_t ASCII_number_table[256][3] PROGMEM= //prva vrednost pomeni št. stolpcev, druga pa št. vrstic
        {
            {'0','0','0'},
            {'0','0','1'},
            {'0','0','2'},
            {'0','0','3'},
            {'0','0','4'},
            {'0','0','5'},
            {'0','0','6'},
            {'0','0','7'},
            {'0','0','8'},
            {'0','0','9'},
            {'0','1','0'},
            {'0','1','1'},
            {'0','1','2'},
            {'0','1','3'},
            {'0','1','4'},
            {'0','1','5'},
            {'0','1','6'},
            {'0','1','7'},
            {'0','1','8'},
            {'0','1','9'},
            {'0','2','0'},
            {'0','2','1'},
            {'0','2','2'},
            {'0','2','3'},
            {'0','2','4'},
            {'0','2','5'},
            {'0','2','6'},
            {'0','2','7'},
            {'0','2','8'},
            {'0','2','9'},
            {'0','3','0'},
            {'0','3','1'},
            {'0','3','2'},
            {'0','3','3'},
            {'0','3','4'},
            {'0','3','5'},
            {'0','3','6'},
            {'0','3','7'},
            {'0','3','8'},
            {'0','3','9'},
            {'0','4','0'},
            {'0','4','1'},
            {'0','4','2'},
            {'0','4','3'},
            {'0','4','4'},
            {'0','4','5'},
            {'0','4','6'},
            {'0','4','7'},
            {'0','4','8'},
            {'0','4','9'},
            {'0','5','0'},
            {'0','5','1'},
            {'0','5','2'},
            {'0','5','3'},
            {'0','5','4'},
            {'0','5','5'},
            {'0','5','6'},
            {'0','5','7'},
            {'0','5','8'},
            {'0','5','9'},
            {'0','6','0'},
            {'0','6','1'},
            {'0','6','2'},
            {'0','6','3'},
            {'0','6','4'},
            {'0','6','5'},
            {'0','6','6'},
            {'0','6','7'},
            {'0','6','8'},
            {'0','6','9'},
            {'0','7','0'},
            {'0','7','1'},
            {'0','7','2'},
            {'0','7','3'},
            {'0','7','4'},
            {'0','7','5'},
            {'0','7','6'},
            {'0','7','7'},
            {'0','7','8'},
            {'0','7','9'},
            {'0','8','0'},
            {'0','8','1'},
            {'0','8','2'},
            {'0','8','3'},
            {'0','8','4'},
            {'0','8','5'},
            {'0','8','6'},
            {'0','8','7'},
            {'0','8','8'},
            {'0','8','9'},
            {'0','9','0'},
            {'0','9','1'},
            {'0','9','2'},
            {'0','9','3'},
            {'0','9','4'},
            {'0','9','5'},
            {'0','9','6'},
            {'0','9','7'},
            {'0','9','8'},
            {'0','9','9'},
            {'1','0','0'},
            {'1','0','1'},
            {'1','0','2'},
            {'1','0','3'},
            {'1','0','4'},
            {'1','0','5'},
            {'1','0','6'},
            {'1','0','7'},
            {'1','0','8'},
            {'1','0','9'},
            {'1','1','0'},
            {'1','1','1'},
            {'1','1','2'},
            {'1','1','3'},
            {'1','1','4'},
            {'1','1','5'},
            {'1','1','6'},
            {'1','1','7'},
            {'1','1','8'},
            {'1','1','9'},
            {'1','2','0'},
            {'1','2','1'},
            {'1','2','2'},
            {'1','2','3'},
            {'1','2','4'},
            {'1','2','5'},
            {'1','2','6'},
            {'1','2','7'},
            {'1','2','8'},
            {'1','2','9'},
            {'1','3','0'},
            {'1','3','1'},
            {'1','3','2'},
            {'1','3','3'},
            {'1','3','4'},
            {'1','3','5'},
            {'1','3','6'},
            {'1','3','7'},
            {'1','3','8'},
            {'1','3','9'},
            {'1','4','0'},
            {'1','4','1'},
            {'1','4','2'},
            {'1','4','3'},
            {'1','4','4'},
            {'1','4','5'},
            {'1','4','6'},
            {'1','4','7'},
            {'1','4','8'},
            {'1','4','9'},
            {'1','5','0'},
            {'1','5','1'},
            {'1','5','2'},
            {'1','5','3'},
            {'1','5','4'},
            {'1','5','5'},
            {'1','5','6'},
            {'1','5','7'},
            {'1','5','8'},
            {'1','5','9'},
            {'1','6','0'},
            {'1','6','1'},
            {'1','6','2'},
            {'1','6','3'},
            {'1','6','4'},
            {'1','6','5'},
            {'1','6','6'},
            {'1','6','7'},
            {'1','6','8'},
            {'1','6','9'},
            {'1','7','0'},
            {'1','7','1'},
            {'1','7','2'},
            {'1','7','3'},
            {'1','7','4'},
            {'1','7','5'},
            {'1','7','6'},
            {'1','7','7'},
            {'1','7','8'},
            {'1','7','9'},
            {'1','8','0'},
            {'1','8','1'},
            {'1','8','2'},
            {'1','8','3'},
            {'1','8','4'},
            {'1','8','5'},
            {'1','8','6'},
            {'1','8','7'},
            {'1','8','8'},
            {'1','8','9'},
            {'1','9','0'},
            {'1','9','1'},
            {'1','9','2'},
            {'1','9','3'},
            {'1','9','4'},
            {'1','9','5'},
            {'1','9','6'},
            {'1','9','7'},
            {'1','9','8'},
            {'1','9','9'},
            {'2','0','0'},
            {'2','0','1'},
            {'2','0','2'},
            {'2','0','3'},
            {'2','0','4'},
            {'2','0','5'},
            {'2','0','6'},
            {'2','0','7'},
            {'2','0','8'},
            {'2','0','9'},
            {'2','1','0'},
            {'2','1','1'},
            {'2','1','2'},
            {'2','1','3'},
            {'2','1','4'},
            {'2','1','5'},
            {'2','1','6'},
            {'2','1','7'},
            {'2','1','8'},
            {'2','1','9'},
            {'2','2','0'},
            {'2','2','1'},
            {'2','2','2'},
            {'2','2','3'},
            {'2','2','4'},
            {'2','2','5'},
            {'2','2','6'},
            {'2','2','7'},
            {'2','2','8'},
            {'2','2','9'},
            {'2','3','0'},
            {'2','3','1'},
            {'2','3','2'},
            {'2','3','3'},
            {'2','3','4'},
            {'2','3','5'},
            {'2','3','6'},
            {'2','3','7'},
            {'2','3','8'},
            {'2','3','9'},
            {'2','4','0'},
            {'2','4','1'},
            {'2','4','2'},
            {'2','4','3'},
            {'2','4','4'},
            {'2','4','5'},
            {'2','4','6'},
            {'2','4','7'},
            {'2','4','8'},
            {'2','4','9'},
            {'2','5','0'},
            {'2','5','1'},
            {'2','5','2'},
            {'2','5','3'},
            {'2','5','4'},
            {'2','5','5'},
        };






/*********************************************************************************
*
*   Function name : ioinit
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calls initialization functions whenever the divece is restarted
*
*********************************************************************************/



void ioinit(void)
{
   
   // INITIALIZATION OF THE REGULATION INTERRUPT TIMER
   
   /*initialization of timer/counter 2A for Clear Timer on Compare (CTC) mode, using I/O timer with prescaler --- see init.h */
   TIMER_ISR_INIT;
   
   /* set interrupt (TIFR2,OCF2A) at compare match*/
   TIMER_ISR_TOP_INT_ENABLE;
   
   
   /* set initial value of the OCR*/
   OCR_ISR_INIT;
   
   /*set the digital output pins*/
   DIGITAL_OUTPUT_SETUP;
   
   // FUNCTION CALLS
   PWM_init();
   ADC_init();
   UART_init( MYUBRR );
   sei();   //Calling sei we set up the global interrupt flag (we enable interrupts globally)
}

/********************************************************************************
*
*   Function name : parameter_handling
*
*   Returns :       None
*
*   Parameters :    8 bit integer, array of 8 bit integers to interpret
*
*   Purpose :       Decodes incoming packets and executes the commands
*
*********************************************************************************/


uint8_t parameter_handling(uint8_t packet_lenght,uint8_t recived_data[]) // packed lenght in bytes
{
   // --- Variable declarations ---
   
   uint8_t i;                //decoding loop counter
   uint8_t j=1;              //decoding loop decade multiplier
   uint8_t data=0;           //decoding loop output and temporary value
   uint8_t com_status=0;     // status of packed header recognition 0==packed recognised
   uint8_t send_data_size=5; //changing this value can affect the packet_assembler function
   
   
   // array declarations
   uint8_t send_data[send_data_size];     // Array containing ASCII data to be send
   
   // --- Decoding loop --- decodes ASCII numbers into binary
   for (i=1;i<packet_lenght;i++) {
      data=data+(recived_data[packet_lenght-i]-48)*j;
      j=j*10;
      }
   
   switch (recived_data[0]) {
   
      // --- Data input section ---
      case ('p'):                     //set the P parameter for P regulation
         if (packet_lenght==1){
            error_handling(1);
         }
         else{
            p=data;
         }
         break;
      case ('r'):                   //set desired value
         if (packet_lenght==1){
            error_handling(1);
         }
         else{
            requested_value=data;
         }
         break;
   
      case ('f'):                   //set first parameter for open-loop regulation mode
         if (packet_lenght==1){
            error_handling(1);
         }
         else{
            light1=data;
         }
         break;
      case ('g'):                    //set second parameter for open-loop regulation mode
         if (packet_lenght==1){
            error_handling(1);
         }
         else{
            light2=data;
         }
         break;
      case ('h'):                   //set third parameter for open-loop regulation mode
         if (packet_lenght==1){
            error_handling(1);
         }
         else{
            light3=data;
         }
         break;
      case ('j'):                   //set fourth parameter for open-loop regulation mode
         if (packet_lenght==1){
            error_handling(1);
         }
         else{
            light4=data;
         }
         break;
         break;
      case ('y'):                   //hysteresis
         if (packet_lenght==1){
            error_handling(1);
         }
         else{
            hysteresis=data;
         }
         break;
   
      // --- Operation_mode section ---
   
      case ('m'):{                              //manual mode
         if (packet_lenght!=1){
            error_handling(3);
         }
         else{
            operation_m_backup=operation_mode;
            operation_mode=manual;
         }
         break;
      case ('s'):                               //sleep mode
         if (packet_lenght!=1){
            error_handling(3);
         }
         else{
            operation_m_backup=operation_mode;
            flash_string_send(mode_text_ptr[3]);
            operation_mode=sleep;
         }
         break;
      case ('a'):{                              //automatic mode
         if (packet_lenght!=1){
            error_handling(3);
         }
         else{
            operation_m_backup=operation_mode;
            operation_mode=automatic;
         }
      }
      break;
   
      case 'n':                                 //monitor mode
         if (packet_lenght!=1){
            error_handling(3);
         }
         else{
            operation_m_backup=operation_mode; //storing actual operation mode
            flash_string_send(mode_text_ptr[1]);
            operation_mode=monitor;             //setting operation monitor mode
         }
      break;
   
      case ('d'):{                              //open-loop regulation mode
         if (packet_lenght!=1){
            error_handling(3);
         }
         else{
            operation_m_backup=operation_mode; //backup current operation mode
            operation_mode=automatic_2;
         }
      }
      break;
   
   
   
         // --- Command section ---
   
      case 'P':                        //print to UART value of the P regulation parameter
         if (packet_lenght!=1){
            error_handling(2);
         }
         else{
            packet_assembler('p',p,send_data);
            UART_send(send_data,send_data_size);
         }
      break;
      case 'R':                        //print to UART desired value
         if (packet_lenght!=1){
            error_handling(2);
         }
         else{
            packet_assembler('r',requested_value,send_data);
            UART_send(send_data,send_data_size);
         }
      break;
      case 'F':                        //print to UART value of first open-loop parameter
         if (packet_lenght!=1){
            error_handling(2);
         }
         else{
            packet_assembler('f',light1,send_data);
            UART_send(send_data,send_data_size);
         }
      break;
      case 'G':                        //print to UART value of second open-loop parameter
         if (packet_lenght!=1){
            error_handling(2);
         }
         else{
            packet_assembler('g',light2,send_data);
            UART_send(send_data,send_data_size);
         }
      break;
      case 'H':                        //print to UART value of third open-loop parameter
         if (packet_lenght!=1){
            error_handling(2);
         }
         else{
            packet_assembler('h',light3,send_data);
            UART_send(send_data,send_data_size);
         }
      break;
      case 'J':                        //print to UART value of fourth open-loop parameter
         if (packet_lenght!=1){
            error_handling(2);
         }
         else{
            packet_assembler('j',light4,send_data);
            UART_send(send_data,send_data_size);
         }
      break;
      case 'Y':                        //print to UART value of hysteresis (open-loop mode)
         if (packet_lenght!=1){
            error_handling(2);
         }
         else{
            packet_assembler('y',hysteresis,send_data);
            UART_send(send_data,send_data_size);
         }
      break;
      case ('x'):                      // return to default operation mode
         if (packet_lenght!=1){
            error_handling(4);
         }
         else{
            operation_m_backup=operation_mode;
            operation_mode=automatic;
         }
      break;
   
   
      default:
         com_status=1;                    //return 1 of packet not recognised
      }
   }
   return (com_status);
}


/********************************************************************************
*
*   Function name : auto_mode
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Handles auto & manual operation modes
*
*********************************************************************************/

void auto_mode(void)
{
   uint8_t packet_lenght;       // stores the value of lenght of the recived packet
   uint8_t com_status=0;          // status of packed header recognition 0==packed recognised
   uint8_t data_field[5];
   
   packet_lenght=UART_recive(data_field); // call of the packed reciving routine
   
   switch (packet_lenght) {     //decisions to make, according to the recived packed lenght
      case 0: ; //if recived empty packet, do nothing
      break;
      case 1:
      case 2:
      case 3:
      case 4:
         com_status=parameter_handling(packet_lenght,data_field);
      break;
      default:
         error_handling(5); //send data frame error message
      }
   
   if (com_status==1)
      error_handling(6);   //unknown parameter or command error
   
   sleep_enable();                  //enabling sleeping mode
   set_sleep_mode(SLEEP_MODE_IDLE); //selecting sleeping mode
   sleep_mode();                    //entering sleep

}

/********************************************************************************
*
*   Function name : monitor_mode
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       outputs a constant flow of i/o values untill a stop command
*
*********************************************************************************/


void monitor_mode(void)
{
   uint8_t i;                                //general counter
   uint8_t stop_command=0;                   //variable containing input from UART
   static uint8_t comp_monitor_counter=0;    // variable for monitoring the correct workof monitor_mode
   
   if( (UCSRA & (1<<RXC))) {   //retrives data from USART buffer
      stop_command=UDR;
      }
   if (stop_command=='x'){
      operation_mode=operation_m_backup; //restoring previous operation mode
      }
   
   /* This section enables and displays to the terminal monitor counters which are counting the number of loops performed by the ISR function and the monitor_mode function*/
   
   
   for(i=0;i<3;i++){
         while( !( UCSRA & (1<<UDRE)));
         UDR = pgm_read_byte(&ASCII_number_table[monitor_counter][i]); //displaying the ISR counter
         }
   comp_monitor_counter=comp_monitor_counter+1; // incrementing the monitor counter
   while( !( UCSRA & (1<<UDRE)));
   UDR = ';';
   for(i=0;i<3;i++){
      while( !( UCSRA & (1<<UDRE)));
      UDR = pgm_read_byte(&ASCII_number_table[comp_monitor_counter][i]); // displaying the Monitor counter
      }
   
   while( !( UCSRA & (1<<UDRE)));
   UDR = ';';
   
   // Send curent ADC value
   for(i=0;i<3;i++){
      while( !( UCSRA & (1<<UDRE)));
      UDR = pgm_read_byte(&ASCII_number_table[ADC_input][i]);
      }
      while( !( UCSRA & (1<<UDRE)));
      UDR = ';';
   // Send curent PWM value
   for(i=0;i<3;i++){
      while( !( UCSRA & (1<<UDRE)));
      UDR = pgm_read_byte(&ASCII_number_table[PWM_input][i]);
      }
   
      while( !( UCSRA & (1<<UDRE))); //send to comport ASCII value CR
         UDR = 13;
      while( !( UCSRA & (1<<UDRE))); //send to comport ASCII value LF
         UDR = 10;

}

/********************************************************************************
*
*   Function name : sleeping_mode
*
*   Returns :       None
*
*   Parameters :    None
*   Purpose :      Puts the controller to sleep and wakes it up.
*
*********************************************************************************/
void sleeping_mode(void)
{
   
   uint8_t on_command=0;
   
   
   if( (UCSRA & (1<<RXC))) {   //retrives data from USART buffer
      on_command=UDR;
      }
   if (on_command=='o'){
      sleep_disable();               //disabling sleeping
      while( !( UCSRA & (1<<UDRE)));
         UDR = 'o';
      while( !( UCSRA & (1<<UDRE))); //send to comport ASCII value CR
         UDR = 13;
      while( !( UCSRA & (1<<UDRE))); //send to comport ASCII value LF
         UDR = 10;
      operation_mode=operation_m_backup; //restoring previous operation mode
   
      }
   sleep_enable();                  //enablink sleeping mode
   set_sleep_mode(SLEEP_MODE_IDLE); //selecting sleeping mode
   sleep_mode();                    //entering sleep

}



/********************************************************************************
*
*   Function name : ISR (interrupt call)
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       It is the interrupt routine whoch handles the regulation
*
*********************************************************************************/

/* it starts when the compare match flag is set up.*/
ISR (TIMER_ISR_COMP_VECT)

{

   regulation(); //see regulator.c

}


/********************************************************************************
*
*   Function name : main
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       It is the main loop of the program
*
*********************************************************************************/

int main(void)
{
   
   
   // FUNCTION CALLS
   ioinit();
   
   
   
   // MAIN LOOP
   
   for (;;){   //endless loop, progrma never exits
      //TC
      //flash_string_send(mode_text_ptr[operation_mode]);
      //EOTC
   
   /* 
   on start of every loop the switcher checks the desired operation mode and redirects
   the program as desired
   */
   
      switch (operation_mode)
      {
      case automatic:
         flash_string_send(mode_text_ptr[0]);
         auto_mode();
      break;
      case monitor:
         monitor_mode();
      break;
      case manual:
         flash_string_send(mode_text_ptr[2]);
         auto_mode();
      break;
      case sleep:
         sleeping_mode();
      break;
         case automatic_2:
         flash_string_send(mode_text_ptr[4]);
         auto_mode();
      }
   
   }
   return (0);
}

