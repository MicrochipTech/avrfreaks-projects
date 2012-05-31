/////////////////////////////////////////
//
// File : uart.c
//
// Serial Port UART Buffered, Interrupt driven I/O
// Register names for non-Mega chips differ.
// See uart.h for the API (function calls)
//
// Warning: input overflow is not handled well.
//
// Version 1.1
//    4/06, adaptation: stevech@san.rr.com
//          Adapted from the "simpleuart" project uploaded to avrfreaks.net
//          At avrfreaks.net, version 1.1 is Project "AVR SimpleUART - buffered/interrupt driven serial".
//          The attached file is called "AVR SimpleUART.zip"
//          http://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_type=project&item_id=563
//
// Version 2.0
//    2012, V2.0 : updated by Kun.io
//            Streaming functions removed.
//            The code is now also for newer chips.
//            Function UART_gets rewritten, the old one didn't handle the maximum buffer size very good.
//            Autodetect CR,LF,CRLF added.
//            Tested with ATmega8 and ATmega328.
//            Tested with avr gcc.
//            Uploaded as Project at http://www.avrfreaks.net "AVR SimpleUART V2"
//              http://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_id=3523&item_type=project
//          V2.1 : UART_puts_P : added 'const' to parameter.
//                 Defines added to run code on AT90USB1286 (Teensy Board)
//


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include "uart.h"

#define TMASK (TBUFSIZE-1)
#define RMASK (RBUFSIZE-1)

// The variables are static (only valid withing this file) and volatile (used in interrupts).

static volatile unsigned char tbuf[TBUFSIZE];    // transmit buffer
static volatile unsigned char rbuf[RBUFSIZE];    // receive buffer

static volatile unsigned char t_in;              // buffer head/tail indicies
static volatile unsigned char t_out;             // NOTE! these are 1 byte; no atomic for double byte needed

static volatile unsigned char r_in;
static volatile unsigned char r_out;

static volatile unsigned char cr_flag;           // added for autodetect CR,LF,CRLF


//////////////////////////////////////////////////////////////
// Initialization of 1st UART
//
void UART_init (void)
{
  // First init for the UART

  cli();                  // no interrupts during initialization.

  // turn everything off
  UCSR0A = 0x00;          // writing zero to UCSRnA is probably not needed.
  UCSR0B = 0x00;          // disable everything in UCSRnB

  // set baudrate, the UBBR_VALUE is defined in uart.h
  UBRR0L = (uint8_t) (UBBR_VALUE);
  UBRR0H = (uint8_t) (UBBR_VALUE >> 8);

  // 8 Databits, receive and transmit enabled, receive complete interrupt enabled
  UCSR0B = _BV(RXCIE0) | _BV(TXEN0) | _BV(RXEN0);

  // Set the most used serial settings: asynchrone, no parity, 8 bit, 1 stop bit.
  // The ATmega8 uses an URSEL bit, which is not present on newer chips.
#if defined (__AVR_ATmega8__)
  UCSRC = _BV(URSEL) | _BV(UCSZ01) | _BV(UCSZ00); // for ATmega8
#else
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);             // for ATmega328
#endif

  cr_flag = false;                                // set default, no 'cr' detected yet.
  t_in = t_out = r_in = r_out = 0;                // set all buffer indices to zero.

  sei();                                          // enable interrupts.
}


///////////////////////////////////////////////////
// Send byte
//
// If the character does not fit in buffer, it will still be transmitted, but it waits
// until characters are transmitted and free space is available in the buffer.
//
void UART_putc (unsigned char c)
{
  // If the transmit-buffer is full, wait for it, interrupts will empty it
  // There is no timeout.
  while((TBUFSIZE - UART_tbuflen()) <= 2)
    ;

  // Add data to the transmit buffer, enable transmit interrupts
  tbuf[t_in & TMASK] = c;       // set character in circular buffer
  t_in++;                       // increment head index
  UCSR0B |= _BV(UDRIE0);        // Enable UDR empty interrupt, the ISR will move from buffer to UART
}


///////////////////////////////////////////////////////////////
// Send string from RAM
//
void UART_puts (char *p)
{
  while (*p)
    UART_putc(*p++);
}


///////////////////////////////////////////////////////////////
// Send string from flash memory
//
void UART_puts_P (const char *p)
{
  char b;

  while ( (b = pgm_read_byte(p++)) != '\0')
    UART_putc(b);
}


////////////////////////////////////////////////////
// Receive byte
//
// This function just gets a single byte.
// The byte can be anything: control characters, linefeeds, etc.
//
unsigned char UART_getc (void)
{
  unsigned char c;

  while (UART_rbuflen() == 0)  // Wait for data. See also UART_rbuflen()
    ;

  c = rbuf[r_out & RMASK];
  r_out++;

  return(c);
}


///////////////////////////////////////////////////////////////
// Get string that ends with a CR, LF or CRLF,  but subject to max chars
//
// The return value is the string length.
//
// The string is terminated with '\0' if it reaches the maximum size.
//   But if the maximum size is reached, the function still waits for a CR or LF.
//
// A flag 'cr_flag' is used for autodetection of CR,LF,CRLF.
//
// Note that due to the autodetection of CR,LF,CRLF, the UART_rbuflen()
// is not the same as the strlen() of the returned string.
//    For example: UART_rbuflen() could be 2, and the string could be empty (or not).
//
// Note that mixing UART_gets() and UART_getc() could break the autodetection of CR,LF,CRLF.
//
int UART_gets (char *buffer, int bufsize)
{
  int i = 0;                             // index for buffer, and character counter.
  uint8_t eol = false;                   // flag for end-of-line

  if (bufsize > 0)
  {
    while (!eol)
    {
      buffer[i] = (char) UART_getc();    // get a fresh character from the input buffer
      if (buffer[i] == '\r')             // check for CR
      {
        cr_flag = true;
        eol = true;                      // end this line
      }
      else if (buffer[i] == '\n')        // check for LF
      {
        if (cr_flag)                     // Previous character was CR, ignore the LF, don't advance index.
          cr_flag = false;
        else                             // No CR flag: end this line.
          eol = true;
      }
      else
      {
        cr_flag = false;                 // normal character, clear flag
        if (i < (bufsize - 1))           // is there still a place for the next character ?
          i++;                           // advance to next position.
      }
    }

    // The CR ('\r') or LF ('\n') is overwritten by the string terminator.
    buffer[i] = '\0';                    // End the string with '\0'
  }

  // Return the length of the string.
  // It could be a truncated string if too many characters were received.
  // It shou be the same as strlen(buffer)
  return (i);
}


///////////////////////////////////////////////////
// Number of bytes as yet untransmitted
// Don't call this from an ISR
//
unsigned char UART_tbuflen (void)
{
  return(t_in - t_out);
}


///////////////////////////////////////////////////
// Number of bytes in receive buffer
// Call this to avoid blocking in UART_getc()
//
unsigned char UART_rbuflen (void)
{
  return (r_in - r_out);
}


///////////////////////////////////////////////////
// Transmit complete interrupt
//
// USART transmit Data Register Empty interrupt.
//
ISR (USART_UDRE_vect)           // changed into new name
{
  if(t_in != t_out)             // is buffer empty ?
  {
    UDR0 = tbuf[t_out & TMASK];
    t_out++;                    // TMASK makes it unnecessary to range limit this
  }
  else
  {
    UCSR0B &= ~_BV(UDRIE0);     // buffer is empty, disable interrupt
  }
}


///////////////////////////////////////////////////
// Receive interrupt
//
// In case of an overflow of the receive buffer,
// the old data is overwritten, but not in a circular way.
// Good:
//    A CR or LF at the end is still passed on.
// Bad:
//    The characters before the CR or LF might all be gone,
//    if the number of received characters is the same as the input buffer.
//
ISR (USART_RX_vect)           // changed into new name
{
  rbuf[r_in & RMASK] = UDR0;  // Put received char in buffer (no check for overflow)
  r_in++;                     // RMASK makes it unnecessary to range limit this
}
