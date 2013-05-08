/*
 * main.c
 *
 *  Created on: Apr 8, 2013
 *      Author: Peter Barnes
 *
 *      Atmega 8 and ST7920-based 12864 display
 *      Serial data at 4800 baud (or whatever you choose below that) applied to pin 2, Rxd
 *      LED on PD2 will light up when receiving characters, will extinguish only when the 4-byte buffer is flushed by a carriage return or some other non-alphanumeric character.
 *
 *      12864 pins:
 *      1	Gnd
 *      2	Vcc
 *      3	Vo (contrast-to center of 10K pot between Vcc and Gnd)
 *      4	PC0
 *      5	PC1
 *      6	PC2
 *      7	PB0
 *      8	PB1
 *      9	PB2
 *      10	PB3
 *      11	PB4
 *      12	PB5
 *      13	PB6
 *      14	PB7
 *      15	Vcc	(PSB - parallel or serial. We're using the parallel interface.)
 *      16	Not Connected
 *      17	Atmega pin 1 (reset)
 *      18	Not Connected
 *      19	Vcc
 *      20	Gnd
 *
 */

#include "st7920.h"
#include <avr/interrupt.h>

// USART STUFF
#define UART_BAUDRATE 4800    //  4800 baud is the maximum possible with a 8MHz clock and this display
#define BAUD_PRESCALE (((F_CPU / (UART_BAUDRATE * 16UL))) - 1)
uint8_t usart_byte;
uint8_t byte_recvd_flag;

void USART_init();
char USARTReadChar(void);
uint8_t minibuffer[4];	// holder for received usart characters
//


int main(void)
{
	DDRD = 0xfe;    // set pin PD0 (USART RXD) for input
	USART_init();
	sei();

	int i, j, k;
    char string1[16] = "      USART     ";
    char string3[16] = "    4800 Baud   ";
    char string2[16] = "       8N1      ";
    char string4[16] = "  Barnes - 2013 ";
    DATAPORT_DIR = 0xff;    // set DATAPORT to output
    DATA_PORT = 0x00;    // set outputs to 0 initially
    COMMAND_DIR = 0x07;    // set command port to output
    COMMAND_PORT = 0x00;    // set outputs to 0 initially
    clcd_init();    // initialize as character display
    cdisplay_clear();    // clear character screen and reset address to 0
    // set_cgram_address(0, 0);
    for (i = 0; i < 16; ++i) {
        write_data(string1[i]);
    }
    for (i = 0; i < 16; ++i) {
        write_data(string2[i]);
    }
    for (i = 0; i < 16; ++i) {
        write_data(string3[i]);
    }
    for (i = 0; i < 16; ++i) {
        write_data(string4[i]);
    }
    _delay_ms(3000);
    cdisplay_clear();

    glcd_enable();
    glcd_fill(0x0000);

    draw_vertical_line(0,0,127);
    draw_vertical_line(0,63,127);

//	for (j = 0; j < 8; j++)
//    {
//			write_4chars_xy(j, 3, 32, 49, 50, 51);
//	}

    i=0;
    k=2;	// y column counter (will be multiplied by 6 to allow for the height of the characters, so value = 0 - 10.
	j=0;	// x row counter (0-7)
	while(1)
	{
		//Read data
	    usart_byte = USARTReadChar();
        {
            PORTD |= 4;		// turn on led on PD2
        	if ((usart_byte < 32) | (usart_byte > 127))	// NOTE: I've given up trying to implement a carriage return - takes too much time, we lose characters
			{
        		usart_byte = 31;	//
            }
       		minibuffer[i]= usart_byte;
       	    i++;
       	    if (i==4)
       	    {
       	    	PORTD &= 0xfb;	// turn led off (buffer is empty)
       	    	i=0;
       	    	write_4chars_xy(j, k, minibuffer[0], minibuffer[1], minibuffer[2], minibuffer[3]);
       	    	j++;
       	    	if (j==8)
       	    	{
       	    		j=0;
       	    		k += 6;		//  bump down to the next line
       	    		if (k > 59)	// if we've reached the bottom line
       	    		{
       	    			k = 2;
       	    		}
       	    	}
       	    }
       	    byte_recvd_flag = 0;        // and reset flag, wait for next character from USART
      	}
    }
}


void USART_init()
    {
		UCSRC=(1<<URSEL)|(3<<UCSZ0); // async., 8N1
		UCSRB=(1<<RXEN)|(1<<TXEN);
        UBRRL = BAUD_PRESCALE;
        UBRRH = (BAUD_PRESCALE >> 8);
    }

void USART_send_char(char b)
    {
        UDR = b;
        while(!(UCSRA&(1<<TXC)));
    }

char USARTReadChar()
{
   //	loop until data is available
   while(!(UCSRA & (1<<RXC))){}
   return UDR;
}
