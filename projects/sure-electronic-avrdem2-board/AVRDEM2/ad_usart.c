// ad_usart.c
//
// source file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated

#include "ad_usart.h"

// -------------------------------------------------------------
// putchar() - write character to USART

int putchar(char c) {
  if (_textmode && c == '\n') putchar('\r');
  while (!(UCSRA & (1<<UDRE))) ;        // wait for OK to send
  return (UDR = c);                     // send and return it
}

// -------------------------------------------------------------
// getchar() - check for and read character from USART if available
//
// returns: -1 if no char; ASCII value if available

int getchar(void) {
  if (UCSRA & (1 << RXC)) {
    return UDR;
  } else {
    return -1;
  }

//  this is the version that waits
//  while (!(UCSRA & (1<<RXC))) ;         // wait for char
//  return UDR;                           // read and return it
}

// -------------------------------------------------------------
// put_CR() - write a CR/LF pair to the serial port

void put_CR(void) {
  putchar('\r');
  putchar('\n');
}

// -------------------------------------------------------------
// put_str(*str, cr) - write string to serial port
//
// str - pointer to string
// cr  - if nonzero, append CR/LF after string

void put_str(char *s, char cr) {
  char c;

  while (c = *s++) putchar(c);
  if (cr) put_CR();
}

// -------------------------------------------------------------
// put_fstr(*str, cr) - write flash string to serial port
//
// str - pointer to string in flash
// cr  - if nonzero, append CR/LF after string

void put_fstr(__flash char *s, char cr) {
  char c;

  while (c = *s++) putchar(c);
  if (cr) put_CR();
}

// -------------------------------------------------------------
// init_usart() - initialize USART

void init_usart(int brate) {
  _textmode = 1;

  UBRRH = (char) (brate >> 8);              // baud rate
  UBRRL = (char) brate;
  UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);  // 8N2
  UCSRB = (1<<RXEN)|(1<<TXEN);              // enable Rx & Tx
}
