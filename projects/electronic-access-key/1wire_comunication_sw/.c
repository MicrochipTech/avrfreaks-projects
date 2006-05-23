//********************************************************************
//  CPU:         ATMEL "ATmega128" (single chip mode, clock 16 MHz)
//  FILE:        UART.h
//  DESCRIPTION: Procedures to read/write byte from/to UART0
//  VERSION:     1.0
//  DATE:        May 2006
//  AUTHOR:      Ing. Domenico Tupputi
//*********************************************************************

//----------------------------------------------------------------------
// HEADER FILES
//----------------------------------------------------------------------
#include "1w_header.h"

//-----------------------------------------------------------------------
// DESCRIPTION:  		Procedure to set UART0 speed @ 9600 bps 												 
// PARAMETERS IN:         
// PARAMETERS OUT:		  
// LOCAL VARIABLE:       
// GLOBAL VARIABLE:     
// COSTANTS:     
//-----------------------------------------------------------------------
void uart0_init_9600(void)
{
  UCSR0B = 0x00; 
  UCSR0A = 0x00;
  UCSR0C = 0x06;
  UBRR0L = 0x67; 
  UBRR0H = 0x00; 
  UCSR0B = 0x18;
}


//-----------------------------------------------------------------------
// DESCRIPTION:  		Procedure to set UART0 speed @ 115200 bps 												 
// PARAMETERS IN:         
// PARAMETERS OUT:		  
// LOCAL VARIABLE:       
// GLOBAL VARIABLE:     
// COSTANTS:     
//-----------------------------------------------------------------------
void uart0_init_115200(void)
{
  UCSR0B = 0x00; 
  UCSR0A = 0x00;
  UCSR0C = 0x06;
  UBRR0L = 0x08; 
  UBRR0H = 0x00; 
  UCSR0B = 0x18;
}


//--------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to read one byte from UART0 (polling mode)												 
// PARAMETERS IN:         
// PARAMETERS OUT:		unsigned char byte;  
// LOCAL VARIABLE:      unsigned char byte; 
// GLOBAL VARIABLE:     
// COSTANTS:     
//---------------------------------------------------------------------------
unsigned char ReceiveByte( void )
{  	
  unsigned char byte;
  
  while ( !(UCSR0A & (1<<RXC0)) ); 	  /* Wait for incomming data */
  byte = UDR0;     		              /* Return the data */
  return byte;            	          
}



//-------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to write one byte to UART0 (polling mode)												 
// PARAMETERS IN:       unsigned char data  
// PARAMETERS OUT:		  
// LOCAL VARIABLE:       
// GLOBAL VARIABLE:     
// COSTANTS:     
//-------------------------------------------------------------------------
void TransmitByte( unsigned char data )
{
  while ( !(UCSR0A & (1<<UDRE0)) );     /* Wait for empty transmit buffer */
  UDR0 = data; 			              /* Start transmittion */
}		  							         


