/* copyright
   * (c) 2004 Stephen Humble stephenhumble at telstra.com
   *
   *  This file is part of robotx see http://Robotx.sourceforge.net
   *  dual sensorless motor driver firmware.
   *
   *  This is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
*/
// USART basic communications routines
#include <avr/io.h>
#include <avr/timer.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include "uart.h"
#include "fail.h"
#include "common.h"

#define TX_BUF_SIZE             64
#define RX_BUF_SIZE             32
#define TX_BUF_MASK             ( TX_BUF_SIZE - 1 )
#define RX_BUF_MASK             ( RX_BUF_SIZE - 1 )

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <sig-avr.h>
#include <interrupt.h>
#include "monorail.h"
#include "base64enc.h"

// simplex RS-485 RS-422 TX enable pin
#define TXEN_PORT PORTB
#define TXEN_DDR DDRB
#define TXEN_PIN PINB
#define TXEN_BIT 1
#define RECIEVE TXEN_PORT&=~_BV(TXEN_BIT)
#define TRANSMIT TXEN_PORT|=_BV(TXEN_BIT)
// pin 22

extern void *usart_data();  // this is the name of the usart rx thread entry point
volatile uint8_t                tx_head;
volatile uint8_t                tx_tail;
volatile uint8_t                  tx_buf[ TX_BUF_SIZE ];

volatile uint8_t         rx_head;
volatile uint8_t                  rx_tail;
volatile uint8_t                  rx_buf[ RX_BUF_SIZE ];

/* these buffers are needed to reduce the effect of interrupt code on the
   communications routines - as RS485 transmissions should be packaetised
*/
volatile long int rxchar;

// * UART Baud rate generation settings:
// * With 4.0 Mhz clock, UBRR=12 => 19200 baud
// * With default oscillator and OSCCAL=0xFF, UBRR=11 ==> 9600 baud.
// * With default oscillator and OSCCAL=0xFF, UBRR= 5 ==> 19200 baud.

/*   select baud rate to use */
//#define UBRRVAL ((CLOCK/(16*9600))-1)
//#define UBRRVAL ((CLOCK/(16*19200))-1)
//#define UBRRVAL ((CLOCK/(16*38400))-1)
//#define UBRRVAL ((CLOCK/(16*57600))-1)
#define UBRRVAL ((CLOCK/(16*115200))-1)
//#define UBRRVAL ((CLOCK/(16*230400))-1)

void   set_UBRR(uint16_t rate)
{
   rate&=0x0fff;
   UBRRL=(uint8_t)rate;
   UBRRH=(uint8_t)(rate>>8);
}

static inline uint8_t
   uart_send_empty( void )
   {
      return tx_head == tx_tail;
   }

void init_uart( void )
{
   UCSRB|=_BV(U2X); // baud rate doubler
   set_UBRR(UBRRVAL);  // 8mhz 9600 baud
#if CLOCK<=2000000
#error CLOCK rate is to low to support 38400 baud comms
#endif
   //set_UBRR(51);  // 8mhz 9600 baud
   //set_UBRR(26);  // 8mhz 19200 baud
   //set_UBRR(25);  // 16mhz 38400 baud
   //set_UBRR(12);  // or 13   8mhz 38400 baud
   //set_UBRR(8);  // 8mhz 57600 baud
   UCSRB|=_BV(RXEN );        // Enable the UART for receiving
   UCSRB|=_BV(TXEN );        // Enable the UART for sending

   TXEN_DDR|=_BV(TXEN_BIT);
   // uart TX and RX
   DDRD&=~_BV(0); // rx
   PORTD|=_BV(0); // use internal pullup to reduce rx noise
   DDRD|=_BV(1);  // TX

   UCSRC = _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1) ;// no parity 8 data 1 stop asynchronous

   rx_head = rx_tail = 0;
   tx_head = tx_tail = 0;
   UCSRB|=_BV(RXCIE );  // enable rx interrupts
   rxchar=0;
}

static inline void write_to_uart( void )
{
   uint8_t                 tmp_tail;
   if( tx_head == tx_tail )
   {
      cbi( UCSRB, UDRIE ); // disable register empty interrupt
      UCSRB|=_BV(TXCIE);  // enable transmission complete interrupt now
      return;
   }
   tx_tail = tmp_tail = ( tx_tail + 1 ) & TX_BUF_MASK;
   UCSRA|=_BV(TXC);  // clear any pending transmission interrupts
   outp( tx_buf[tmp_tail], UDR );
}


SIGNAL( SIG_UART_DATA )
{
   write_to_uart();
}

SIGNAL( SIG_UART_TRANS)
{
   UCSRB&=~_BV(TXCIE);  // clear TX complete interrupt enable
   RECIEVE;  // back to to recieve mode
}

void put_bin(unsigned char c)
{
   char x=0;
   do{
	if(bit_is_set(c,7)){
			      putc('1');
			   }else{
				   putc('0');
				}
	c=c<<1;
     }while(x++<7);
}

void put_i16(int16_t c)
{
   puts(itoa(c,&buf,10));
}

void put_i32(int32_t c)
{
   puts(ltoa(c,&buf,10));
}

void put_u32(uint32_t c)
{
   puts(ultoa(c,&buf,10));
}

/* send a carridge return - also will be hook in for packetisation */
void put_cr()
{
   putc('\n');
   putc('\r');
}

void putw(int16_t c)
{
   putc(c>>8);
   putc(c);
}

void putl(int32_t c)
{
   putw(c>>16);
   putw(c);
}

uint8_t putf(float c)
{
   union {
	    unsigned long val;
	    float f;
	 } mem;
   mem.f = c;
   //assert(sizeof(float) == sizeof(unsigned int));
   putc( (mem.val & 0xFF000000) >> 24);
   putc( (mem.val & 0x00FF0000) >> 16);
   putc( (mem.val & 0x0000FF00) >>  8);
   putc( (mem.val & 0x000000FF));
}

void putc(uint8_t c )
{
   uint8_t                 tmp_head;
   tmp_head = ( tx_head + 1 ) & TX_BUF_MASK;
   if( tmp_head == tx_tail )
      return;
   tx_buf[ tx_head = tmp_head ] = c;
   UCSRB|=_BV(UDRIE);
   TRANSMIT;
}

// blocking write
int8_t write(char *buf, int8_t count)
{
   int8_t ct;
   for(ct=0;ct<count;ct++){
			     putc(*buf);
			     buf++;
			  }
   return count;
}

//sync wait for previous transmissions to complete
void syncwait()
{
   loop_until_bit_is_clear( UCSRB, UDRIE );
   return;
}

uint8_t getc(uint8_t *c)
{
   //cbi( UCSRB, RXCIE );  // disable rx interrupts
   //cli();
   uint8_t			tmp_tail;
   if( rx_head == rx_tail )
      //sbi( UCSRB, RXCIE );  // enable rx interrupts
   //sei();
   return 0;

   rx_tail = tmp_tail = ( rx_tail + 1 ) & RX_BUF_MASK;
   *c = rx_buf[ tmp_tail ];
   // sbi( UCSRB, RXCIE );  // enable rx interrupts
   // sei();
   return 1;
}

static inline void
   read_from_uart( void )
   {
      char			c = UDR;
      uint8_t			tmp_head;
      tmp_head = (rx_head + 1) & RX_BUF_MASK;
/* Check for free buffer */
      if( tmp_head == rx_tail )
	 return;
      rx_buf[ rx_head = tmp_head ] = c;
      //putc(c);  //echo back
   }

char read(char *data,char count)
{
   char x=0;
   for(x=0;x<count;x++)
   {
      while(getc(data)==0)
      {
	 // goto  sleep for a while mabey a character will arrive when we wake up
	 set_sleep_mode(SLEEP_MODE_IDLE);
	 sleep_mode();
      }
      data++;
   }
}

// raw data
uint16_t getw(uint16_t **c)
{
   uint16_t v=**c;
   *c++;
   return(v);
}

// raw data
uint32_t geti(uint32_t **c)
{
   uint32_t v=**c;
   *c++;
   return(v);
}


SIGNAL( SIG_UART_RECV )
{
   GREEN_ON;
   read_from_uart();
   rxchar++;
   GREEN_OFF;
}

