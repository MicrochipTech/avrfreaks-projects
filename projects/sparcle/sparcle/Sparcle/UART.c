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
* FILE: UART.c
*
*
* This file includes UART related functions which manage not only UART communication, but also error handling code.
*
*
*
*
************************************************************************************************/

// --- GLOBAL VARIABLES (EXTERN) ---

#include "globals.h"

/********************************************************************************
*
*   Function name : UART_init
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Sets the needed flags to ensure correct working of UART
*
*********************************************************************************/
void UART_init(unsigned int ubrr)
{
   /*set baud rate*/
   
   UBRRH = (unsigned char) (ubrr>>8);
   UBRRL = (unsigned char)ubrr;
   
   //set usart to double speed mode
   //DOUBLE_SPEED_USART;
   
   // set data frame to 8bit, with 1 start and 2 stop bits
   SET_DATA_FRAME;
   
   // set even parity checking
   EVEN_PARITY; //actually not implemented, check init.h
   
   // enable the the UART reciver and transmitter
   ENABLE_RX_TX;
   

}

/********************************************************************************
*
*   Function name : error_handling
*
*   Returns :       None
*
*   Parameters :    8 bit integer
*
*   Purpose :       Sends back communication error messages
*
*********************************************************************************/

void error_handling (uint8_t error)
{
   
   uint8_t i;
   
   /* Print error no.to terminal */
   flash_string_send(err_text);            //error message
   for(i=0;i<3;i++){
      while( !( UCSRA & (1<<UDRE)));
      UDR = pgm_read_byte(&ASCII_number_table[error][i]);
      }
   flash_string_send(err_text_ptr[error]); //print additional text to terminal

}


/********************************************************************************
*
*   Function name : UART_recive
*
*   Returns :       8 bit integer
*
*   Parameters :    Pointer to the data field (array) to fill
*
*   Purpose :       Recives UART packet and stores it into an array (interpreter would be included)
*
*********************************************************************************/


uint8_t UART_recive(uint8_t data_field_ptr[])
{
   
   uint8_t i;
   
   
   for (i=0;i<5;i++)
      {
      while( !( UCSRA & (1<<RXC)) );   //retrives data from USART buffer
         data_field_ptr[i]=UDR;
      while( !( UCSRA & (1<<UDRE)));   //displays on terminalrecived character
         UDR = data_field_ptr[i];
      if (data_field_ptr[i]==13){  //end of reciving packet, actually rapresented by ASCII symbol CR
         break;
      }
   }
   
   while( !( UCSRA & (1<<UDRE))); //send to comport ASCII value LF
      UDR = 10;
   return(i);
}



/********************************************************************************
*
*   Function name : packet_assembler
*
*   Returns :       None
*
*   Parameters :    2 - 8 bit pointers &  array send_data[]
*
*   Purpose :       Assembler of data packets to be sent through UART_send
*********************************************************************************/

void packet_assembler (uint8_t head, uint8_t data, uint8_t send_data_ptr[])
{
   uint8_t i;
   
   send_data_ptr[0]=head;
   send_data_ptr[1]='=';
   
   for(i=0;i<3;i++){
      send_data_ptr[i+2]=pgm_read_byte(&ASCII_number_table[data][i]);
      }

}




/********************************************************************************
*
*   Function name : UART_send
*
*   Returns :       None
*
*   Parameters :    16 bit integer, pointer to the data array to be sent
*
*   Purpose :       Sends data arrays with a fixed no. of elements through UART
*********************************************************************************/

void UART_send (uint8_t send_data_ptr[], uint8_t data_size)
{
   uint8_t i;
   
   
   for(i=0;i<data_size;i++){
      while( !( UCSRA & (1<<UDRE)));
      UDR = send_data_ptr[i];
      }
   
   while( !( UCSRA & (1<<UDRE))); //send to comport ASCII value CR
      UDR = 13;
   while( !( UCSRA & (1<<UDRE))); //send to comport ASCII value LF
      UDR = 10;

}

/********************************************************************************
*
*   Function name : flash_string_send
*
*   Returns :       None
*
*   Parameters :    16 bit integer, pointer to the data string in program flash to be sent
*
*   Purpose :       Sends data strings stored in flash program memory through the USART interface
*********************************************************************************/

void flash_string_send (char string_ptr[])
{
   // variable declaration
   uint8_t i=0;                                 // counter
   
   //following loop is executed until we reach end of the string to be sent
   while (pgm_read_byte(&string_ptr[i]) != 0){
      while( !( UCSRA & (1<<UDRE)));
      UDR = pgm_read_byte(&string_ptr[i++]);
      }
}


//TC
/********************************************************************************
*
*   Function name : UART_transmit_TF / UART_transmit_TFb
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Testing code
*
*********************************************************************************/

void UART_transmit_TF(uint8_t data_input)
{
   // while( !( UCSRA & (1<<UDRE)));
   //     UDR = (data_input);
   uint8_t i;
   for(i=0;i<3;i++){
      while( !( UCSRA & (1<<UDRE)));
      UDR = pgm_read_byte(&ASCII_number_table[data_input][i]);
      }
   while( !( UCSRA & (1<<UDRE)));
      UDR = 10;
   while( !( UCSRA & (1<<UDRE)));
      UDR = 13;

}

void UART_transmit_TFb(uint8_t data_input)
{
   while( !( UCSRA & (1<<UDRE)));
      UDR = ('T');
   
   uint8_t i;
   for(i=0;i<3;i++){
      while( !( UCSRA & (1<<UDRE)));
      UDR = pgm_read_byte(&ASCII_number_table[data_input][i]);
      }
   while( !( UCSRA & (1<<UDRE)));
      UDR = 10;
   while( !( UCSRA & (1<<UDRE)));
      UDR = 13;

}



// EOTC
