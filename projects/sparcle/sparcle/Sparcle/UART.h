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
* FILE: UART.h
*
*
* This file includes UART related function prototypes.
*
*
*
*
************************************************************************************************/

// --- FUNCTION PROTOTYPES ---

void UART_init(unsigned int ubrr);        // initialization function

// main communication functions

uint8_t UART_recive(uint8_t data_field_ptr[]);                // reciving function

void UART_send (uint8_t send_data_ptr[], uint8_t data_size); // function for sending packets in stored data arrays
void error_handling (uint8_t error);      // function for handling detected errors


// data managing communication functions

void BIN_to_ASCII (uint8_t bin);                   // function for encoding 8bit binnary in ASCII numbers
void packet_assembler (uint8_t head, uint8_t data, uint8_t send_data_ptr[]); // assembler of data packets to be sent
void flash_string_send (char string_ptr[]);              //string sending function

// functions for testing and debugging use
//TC

void UART_transmit_TF(uint8_t data_input);  //function made for testing purposes
void UART_transmit_TFb(uint8_t data_input); //function made for testing purposes - version b

//EOTC












