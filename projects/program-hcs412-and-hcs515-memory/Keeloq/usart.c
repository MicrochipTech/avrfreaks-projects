// AVR306: Using the AVR UART in C
// Routines for interrupt controlled USART
// Last modified: 14-05-2008
// Modified by: AR
//  for atmega88

/* Includes */
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "usart.h"


/* Static Variables */
static unsigned char usart_rxbuf[USART_RX_BUFFER_SIZE];
static volatile unsigned char usart_rxhead;
static volatile unsigned char usart_rxtail;
static unsigned char usart_txbuf[USART_TX_BUFFER_SIZE];
static volatile unsigned char usart_txhead;
static volatile unsigned char usart_txtail;
static volatile unsigned char usart_lastrxerror;

//--------------------------------------------------------
// Name: usart_init
// Desc: init the USART module 
// Rets: nothing
// Date: 2008/04/03
// Modf:
//
void usart_init(unsigned int baudrate){
	unsigned char x;

	/* Set the baud rate */
	UBRR0H=(unsigned char) (baudrate>>8);                  
	UBRR0L=(unsigned char) baudrate;
	
	/* Enable UART receiver and transmitter */
	UCSR0B=_BV(RXCIE0)|_BV(RXEN0)|_BV(TXEN0);
	UCSR0A=_BV(U2X0);
	
	/* Set frame format: 8 data 2stop */
	//UCSR0C = (1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);    //For devices with Extended IO
	UCSR0C=_BV(UCSZ01)|_BV(UCSZ00);   //For devices without Extended IO
	
	/* Flush receive buffer */
	x = 0; 			   
	usart_rxtail = x;
	usart_rxhead = x;
	usart_txtail = x;
	usart_txhead = x;
}

//--------------------------------------------------------
// Name: ISR
// Desc: interrupt on RX
// Rets: nothing
// Date: 2008/04/03
// Modf:
//
ISR(USART_RX_vect) {
	unsigned char data;
	unsigned char tmphead;
	unsigned char usr;
	unsigned char lastrxerror;
	
	usr=UCSR0A;
	data=UDR0;   /* Read the received data */                 
	tmphead=(usart_rxhead + 1) & USART_RX_BUFFER_MASK;	/* Calculate buffer index */
	lastrxerror=(usr&(_BV(FE0)|_BV(DOR0)));
	
	if(tmphead == usart_rxtail){
		lastrxerror=USART_BUFFER_OVERFLOW;/* ERROR! Receive buffer overflow */
	}else{
		usart_rxhead=tmphead;      /* Store new index */
		usart_rxbuf[tmphead]=data; /* Store received data in buffer */
		};
	usart_lastrxerror=lastrxerror;
}

//--------------------------------------------------------
// Name: ISR data empty register
// Desc: called when the usart ready to transmit the next byte
// Rets: nothing
// Date: 2008/04/03
// Modf:
//
ISR(USART_UDRE_vect){
	unsigned char tmptail;

	if(usart_txhead != usart_txtail){   	/* Check if all data is transmitted */	
		tmptail=(usart_txtail + 1 ) & USART_TX_BUFFER_MASK;	/* Calculate buffer index */
		usart_txtail=tmptail;      /* Store new index */
		UDR0=usart_txbuf[tmptail];  /* Start transmition */
	}
		else{
		UCSR0B &= ~_BV(UDRIE0);         /* Disable UDRE interrupt */
		};
}

//--------------------------------------------------------
// Name: usart_getc
// Desc: return byte from ringbuffer 
// Rets: byte from ringbuffer
// Date: 2008/04/03
// Modf:
//
unsigned char usart_getc(void){
	unsigned char tmptail;
	
	while(usart_rxhead == usart_rxtail);  /* Wait for incomming data */
	tmptail=( usart_rxtail + 1 ) & USART_RX_BUFFER_MASK;/* Calculate buffer index */	
	usart_rxtail=tmptail;                /* Store new index */	
	return usart_rxbuf[tmptail];           /* Return data */
}

//--------------------------------------------------------
// Name: usart_putc
// Desc: write byte to ringbuffer
// Rets: nothing
// Date: 2008/04/03
// Modf:
//
void usart_putc(unsigned char data){
	unsigned char tmphead;
	
	tmphead = ( usart_txhead + 1 ) & USART_TX_BUFFER_MASK;/* Calculate buffer index */ 
	while ( tmphead == usart_txtail );/* Wait for free space in buffer */
	usart_txbuf[tmphead]=data;           /* Store data in buffer */
	usart_txhead=tmphead;                /* Store new index */
	UCSR0B|=_BV(UDRIE0);                    /* Enable UDRE interrupt */
}

//--------------------------------------------------------
// Name: usart_puts
// Desc: send string
// Rets: nothing
// Date: 2008/04/03
// Modf:
//
void usart_puts(const char *s){
	while(*s){
		usart_putc(*s++);
	};
}

//--------------------------------------------------------
// Name: usart_puti
// Desc: send integer as ASCII 
// Rets: nothing
// Date: 2008/04/03
// Modf:
//
void usart_puti( const int val ){
   char buffer[sizeof(int)*8+1];
    
   usart_puts( itoa(val, buffer, 10) );
}

//--------------------------------------------------------
// Name: usart_puts_p
// Desc: send string from flash 
// Rets: nothing
// Date: 2008/04/03
// Modf:
//
void usart_puts_p(const char *progmem_s ){
    register char c;
    
    while ( (c = pgm_read_byte(progmem_s++)) ) 
      usart_putc(c);

}

//--------------------------------------------------------
// Name: usart_puthex_nibble
// Desc: put lower nibble as hex to ASCII
// Rets: nothing
// Date: 2008/04/03
// Modf:
//
void usart_puthex_nibble(const unsigned char b){
   unsigned char  c = b & 0x0f;
   
	if (c>9) c += 'A'-10;
   else c += '0';
   usart_putc(c);
}

//--------------------------------------------------------
// Name: usart_puthex_byte
// Desc: put upper and lower nibble as hex to ASCII 
// Rets: nothing
// Date: 2008/04/03
// Modf:
//
void usart_puthex_byte(const unsigned char  b){
   usart_puthex_nibble(b>>4);
   usart_puthex_nibble(b);
} 

//--------------------------------------------------------
// Name: buffer_stat
// Desc: receive buffer status
// Rets: 0 if buffer empty
// Date: 2008/04/03
// Modf:
//
unsigned char buffer_stat(void){
	return(usart_rxhead != usart_rxtail); /* Return 0 (FALSE) if the receive buffer is empty */
}
