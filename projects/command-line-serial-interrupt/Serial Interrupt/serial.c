//*****************************************************************************
//
// File Name	: 'serial.c'
// Title		: Serial Interrupt Command line
// Author		: Agung Bakhtiar - Copyright (C) 2009
// Created		: 2009-03-03
// Revised		: 2009-03-06
// Version		: 1.1
// Target MCU	: Atmel AVR series
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h> 
#include <string.h>
//#include <stdlib.h>
#include <util/delay.h>
#define F_CPU 4000000  				// Micro controller crystal freq
#define BAUD 9600
#define UBRR_VAL F_CPU/16/BAUD-1

// Delay function
 
void delay_ms(int delay) {
        int i;
        for (i=0;i<=delay;i++) {
                _delay_ms(1);
        }
}

// Initialing serial

void init_uart(unsigned int ubrr) {
        UBRRH = (unsigned char)(ubrr>>8);
        UBRRL = (unsigned char)(ubrr);
        UCSRB |= (1<<RXEN);
        UCSRB |= (1<<TXEN);
        UCSRB |= (1<<RXCIE);
        UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
}

// send a single character

void send_char(unsigned char data) {
        while (!(UCSRA & (1<<UDRE)));
        UDR = data;
}

// send a string (multiple character)

void send_string(char *data) {
        while (*data) {
                send_char(*data);
                data++;
        }
}



#define RXB8 1
#define TXB8 0
#define UPE 2
#define OVR 3
#define FE 4
#define UDRE 5
#define RXC 7

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<OVR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

#define RX_BUFFER_SIZE 4                  	/*Number of char of our command
											  interrupt will active every recieve a single character, so
											  when does the command will execute is when the buffer recieve
											  characters with this number of lenght

*/

char rx_buffer[RX_BUFFER_SIZE];				
char command_1[RX_BUFFER_SIZE] = "YAAA";
char command_2[RX_BUFFER_SIZE] = "YBBB";
char command_3[RX_BUFFER_SIZE] = "YCCC";


#if RX_BUFFER_SIZE<256
unsigned char rx_wr_index,rx_rd_index,rx_counter;
#else
unsigned int rx_wr_index,rx_rd_index,rx_counter;
#endif

bool rx_buffer_overflow;				
bool status_perintah;                   // status of command


char slaju[10];
char realdata[2];
double LAJU;
int intrealdata;

ISR(USART_RX_vect) 
{

unsigned char status,data; 
status=UCSRA;
data=UDR;



if (data=='Y'){ // command will be identified when starting with "Y" Character
status_perintah=1;
} 
if (status_perintah==1)
{
	rx_buffer[rx_wr_index]=data;
		if (++rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
		if (++rx_counter == RX_BUFFER_SIZE)
		{
		rx_counter=0; 
		status_perintah=0;
			if (!memcmp(rx_buffer, command_1, 4*sizeof(char))) {  // if buffer has recieved 4 character then execute wha
				send_string("Command 1 (AAA) success");
				}
			
			else if (!memcmp(rx_buffer, command_2, 4*sizeof(char))) {
				send_string("Command 2 (BBB) success");

			}
			else if (!memcmp(rx_buffer, command_3, 4*sizeof(char))) {
				send_string("Command 2 (CCC) success");

			}
			else {
				send_string("No Comment identified");
		
			}
		rx_buffer_overflow=1;  
		return 0;
		};
}
else
{
	status_perintah=0;
}
}

int main(void) {
intrealdata=500;
init_uart(UBRR_VAL);
send_string("Mulai");		
sei(); // Eneable Interrupt

        while (1) {
                send_string("==");
				delay_ms(500);
        }
        return 1;
}