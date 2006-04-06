/////////////////////////////////////////
// v1.1
// Serial Port UART Buffered, Interrupt driven I/O
// Tested with ATmega8. Register names for non-Mega chips differ.
// See UART.H for the API (function calls)
//
// Adapted from the "simpleuart" project uploaded to avrfreaks.net
//    adaptation: stevech@san.rr.com 4/06

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <pgmspace.h>
#include "UART.h"

#define TMASK		(TBUFSIZE-1)
#define RMASK		(RBUFSIZE-1)

volatile unsigned char tbuf[TBUFSIZE];	// buffers
volatile unsigned char rbuf[RBUFSIZE];

volatile unsigned char t_in;		//buffer head/tail indicies
volatile unsigned char t_out;		// NOTE! these are 1 byte; no atomic for double byte needed

volatile unsigned char r_in;
volatile unsigned char r_out;

///////////////////////////////////////////////////   
/* Receive interrupt */   
ISR(SIG_UART_RECV) {  
	unsigned char c;
	
	c = UDR;					// Get received char
	rbuf[r_in & RMASK] = c;		// put in buffer (no check for overflow)
	r_in++;		// RMASK makes it unnecessary to range limit this
}
///////////////////////////////////////////////////
/* Transmit complete interrupt */ 
ISR(SIG_UART_DATA) {  /* USART transmit Data Register Empty */
	
	if(t_in != t_out) {		// is buffer empty?
		UDR = tbuf[t_out & TMASK];
		t_out++;	 // TMASK makes it unnecessary to range limit this
	}
	else {
		UCSRB &= ~(1<<UDRIE);  // buffer is empty, disable interrupt
	}
}


///////////////////////////////////////////////////////////////
// Get string that ends with a \r  but subject to max chars
int UART_gets(char *p, int max)  {
	int n = 0;
	
	while (n < max) {
		*p = UART_getc();
		if (*p++ == '\r')  {
			break;
		}
		++n;
	}
	*p = 0;
	return(n);
}
///////////////////////////////////////////////////////////////
// Print string from RAM
void UART_puts(char *p)  {
	
	while (*p)
		UART_putc(*p++);
}
///////////////////////////////////////////////////////////////
// Print string from flash memory
void UART_puts_P( char *p)  {
	char b;
	
	while ( (b = pgm_read_byte(p++) ) != 0)
		UART_putc(b);
}

///////////////////////////////////////////////////
// Number of bytes as yet untransmitted
//    don't call this from an ISR
unsigned char UART_tbuflen(void) {

	return(t_in - t_out);
}

///////////////////////////////////////////////////
// unbuffered I/O send byte
void UART_putc(unsigned char c)  {
	
	/* if buffer is full wait, interrupts will empty it */
	while((TBUFSIZE - UART_tbuflen()) <= 2)
		;
	/* Add data to the transmit buffer, enable transmit interrupts */
	tbuf[t_in & TMASK] = (unsigned char)c;
	t_in++;							// increment head index
	UCSRB |= (1<<UDRIE);		// Enable UDR empty interrupt
								// the ISR will move from buffer to UART
}	

///////////////////////////////////////////////////
// buffered I/O send byte
int UART_fputc(char c, FILE* stream) {
	
	UART_putc( (unsigned char)c );
	return(0);
}

///////////////////////////////////////////////////
// Number of bytes in receive buffer
// call this to avoid blocking in UART_getchar()
unsigned char UART_rbuflen(void) {

	return (unsigned char)(r_in - r_out);
}


////////////////////////////////////////////////////
//  I/O get char
unsigned char UART_getc(void) {
	unsigned char c;

	while(UART_rbuflen() == 0)  // wait for data. see also UART_rbuflen()
		;
	
	c = rbuf[r_out & RMASK];
	r_out++;
	
	return(c);
}


//////////////////////////////////////////////////////////////
// buffered I/O get char
int UART_fgetc(FILE* stream) {
	
	return(int)(UART_getc());
}


//////////////////////////////////////////////////////////////
// Initialization of 1st UART
void UART_init(void) {
	/* First init for the UART */

	UBRRL = UBBRL_VALUE;	
	UCSRB = (1<<RXCIE)|(1<<TXEN)|(1<<RXEN);			// 8 Databits, receive and transmit enabled, receive and transmit complete interrupt enabled
	// do these in main()
	//fdevopen( UART_fputc, UART_fgetc );			// establish buffered I/O callbacks
	//sei();												// Global interrupt enable
}

