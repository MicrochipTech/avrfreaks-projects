#ifndef UART_H
#define UART_H
/*************************************************************************
 Title	:   C include file for the UART library uart.c
 Author:    EMK
 Software:  AVR-GCC 3.3
 Hardware:  any AVR device ATMega32
***************************************************************************/

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000UL
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

// Buffer sizes must be 2^n
//
#define TBUFSIZE	16
#define RBUFSIZE	16

#define TMASK		(TBUFSIZE-1)
#define RMASK		(RBUFSIZE-1)

// Function prototypes
//

void USART_init(void);
char UART_get_chr(void);
void UART_put_chr(char c);
void UART_put_str(char *str);
char tbuflen(void);
char rbuflen(void);


// Static variables
//
volatile unsigned char tbuf[TBUFSIZE];  // TX buffer
volatile unsigned char rbuf[RBUFSIZE];  // RX buffer

volatile unsigned char t_in;            // TX buffer in index
volatile unsigned char t_out;           // TX buffer out index

volatile unsigned char r_in;            // RX buffer in index
volatile unsigned char r_out;           // RX buffer out index


#endif //UART_H


