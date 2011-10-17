
//**************************************************
// ***** SOURCE FILE : UART_routines.c ******
//**************************************************
#ifndef _UART_ROUTINES_C
#define _UART_ROUTINES_C

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "UART_routines.h"

void uart1_init(void)
{
	UCSR1A=0x00;
	UCSR1B = (1<<RXEN)|(1<<TXEN);
//	UCSR1C = (1<<URSEL1)|(1<<UCSZ0)|(1<<UCSZ1);
	UBRR1H = HI(baud);
	UBRR1L = LO(baud);
}

//**************************************************
//Function to receive a single byte
//*************************************************
unsigned char receiveByte( void )
{
	unsigned char data, status;
	
	while(!(UCSR1A & (1<<RXC1))); 	// Wait for incomming data
	
	status = UCSR1A;
	data = UDR1;
	
	return(data);
}

//***************************************************
//Function to transmit a single byte
//***************************************************
void transmitByte( unsigned char data )
{
	while ( !(UCSR1A & (1<<UDRE1)) )
		; 			                /* Wait for empty transmit buffer */
	UDR1 = data; 			        /* Start transmition */
}


//***************************************************
//Function to transmit hex format data
//first argument indicates type: CHAR, INT or LONG
//Second argument is the data to be displayed
//***************************************************
void transmitHex( unsigned char dataType, unsigned long data )
{
unsigned char count, i, temp;
unsigned char dataString[] = "0x        ";

if (dataType == CHAR) count = 2;
if (dataType == INT) count = 4;
if (dataType == LONG) count = 8;

for(i=count; i>0; i--)
{
  temp = data % 16;
  if((temp>=0) && (temp<10)) dataString [i+1] = temp + 0x30;
  else dataString [i+1] = (temp - 10) + 0x41;

  data = data/16;
}

//transmitString (dataString);  // ??????????????????????????????????
}

//***************************************************
//Function to transmit a string in Flash
//***************************************************
void transmitString_F(char* string)
{
  while (pgm_read_byte(&(*string)))
   transmitByte(pgm_read_byte(&(*string++)));
}

//***************************************************
//Function to transmit a string in RAM
//***************************************************
void transmitString(char* string)
{
  while (*string)
   transmitByte(*string++);
}

//************ END ***** www.dharmanitech.com *******

#endif //_UART_ROUTINES_
