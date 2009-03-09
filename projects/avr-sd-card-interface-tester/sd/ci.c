/*************************************************************//**
* \file sci.c
*
* SCI/UART interface
*
* This interface relies on avr-libc. It binds the compile-time
* configured UART to stdin and stdout, allowing the user to
* read/write data over the serial line using printf and similar
* calls.
*
* \author Stefan Filipek
* \date March 2009
*
******************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>
#include "sci.h"
#include "queue.h"

#ifndef __B_RATE
  #define __B_RATE 9600
#endif

int SCI_PutChar(char, FILE *);
int SCI_GetChar(FILE *);

//! Calculation for prescaler. Define F_CPU in Makefile.
#define UBRRH_BAUD (F_CPU/16/__B_RATE-1)

//! Circular recieve buffer
static Queue SCI_TxBuffer;
//! Circular transmit buffer
static Queue SCI_RxBuffer;

//! Special stdout to bind to serial port
static FILE mystdout = FDEV_SETUP_STREAM(SCI_PutChar, NULL, _FDEV_SETUP_WRITE);
//! Special stdin to bind to serial port
static FILE mystdin  = FDEV_SETUP_STREAM(NULL, SCI_GetChar, _FDEV_SETUP_READ);


/**
 * Put a character into the UART queue
 *
 * Blocking Function - Place a single character on the TxBuffer
 */
int
SCI_PutChar(char input, FILE * stream) {
  uint8_t err = 0;
  if(input == '\n')
    SCI_PutChar('\r', stream);

  do{
    cli();
    err = !en_q(input, &SCI_TxBuffer);
    sei();
  }while(err);
  UCSRB |= _BV(UDRIE);
  return 0;
}

/**
 * Get a character from the UART queue
 *
 * Blocking Function - Get next char on the RxBuffer
 */
int
SCI_GetChar(FILE * stream) {
  char ret_val;
  uint8_t err = 0;
  do{
    cli();
    err = !de_q((uint8_t *)&ret_val, &SCI_RxBuffer);
    sei();
  }while(err); // Poll till something is in buffer
  return ret_val;
}

/**
 * Initialize the SCI
 *
 * This must be called to initialize the SCI and bind
 * stdin and stdout to the serial port.
 */
void
SCI_Init(void) {
  /* Set Baud Rate. */
  UBRRH = (uint8_t)(UBRRH_BAUD>>8);
  UBRRL = (uint8_t)(UBRRH_BAUD);

  /* Enable Rx and Tx, and interrupt */
  UCSRB = _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);

  /* Frame Format - 8 data, no parity */
  /* NEED URSEL FOR MEGA16/32 */
  UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);// | _BV(UPM1) | _BV(UPM0);

  stdout = &mystdout;
  stdin = &mystdin;


}

/**
 * Handle recieved data
 */
ISR(SIG_USART_RECV) {
  uint8_t rx_data = UDR;
  en_q(rx_data, &SCI_RxBuffer); // Fail silently
  //en_q(rx_data, &SCI_TxBuffer); UCSRB |= _BV(UDRIE); // echo?
}

/**
 * Handle transmitting data
 */
ISR(SIG_USART_DATA) {
  uint8_t tx_data;
  uint8_t err = 0;
  err = !de_q(&tx_data, &SCI_TxBuffer);
  if(err)
    UCSRB &= ~_BV(UDRIE);
  else
    UDR = tx_data;
}
