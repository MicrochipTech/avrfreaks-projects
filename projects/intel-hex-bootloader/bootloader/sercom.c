/*******************************************************************************
* Filename: sercom.c
*
* Author: Preston K. Manwaring & Donald Wichern
*
* Date: July 3, 2003
*
* Description: This file contains functions to write to the UART.
*
*******************************************************************************/

/*--- INCLUDE FILES ----------------------------------------------------------*/
#include "sercom.h"

/*--- unsigned char Hit0( void ) ---------------------------------------------*/
unsigned char Hit0( void )
{
	if( UCSR0A & (1<<RXC0) )
		return(1);
		
	else
		return(0);
}

/*--- void SerComInit( unsigned int baud ) -----------------------------------*/
void SerCom0Init( unsigned int baud ) 
{
	// set baud rate
	UBRR0H = (unsigned char)(baud>>8);
	UBRR0L = (unsigned char)(baud);
	// enable receiver and transmitter
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	// set frame format 8 data bits, 1 stop bit
	UCSR0C = (1<<URSEL0) | (3<<UCSZ00);

} 

/*--- void PutChar0( unsigned char byte ) ------------------------------------*/
void PutChar0( unsigned char byte ) 
{

	// wait for empty transmit buffer
	while ( !( UCSR0A & (1<<UDRE0) ) );
	
	// send data
	UDR0 = byte;

}

/*--- unsigned char GetChar0( void ) -----------------------------------------*/
unsigned char GetChar0( void ) 
{

	// wait for data ...
	while ( !( UCSR0A & (1<<RXC0) ) );
	
	// return data
	return( UDR0 );

}

/*--- void PutString0( unsigned char *str )-----------------------------------*/
void PutString0( unsigned char *str )
{

	unsigned char i = 0;

	while ( str[i] ) {
		PutChar0( str[i] );
		i++;
	}

}

/*--- void PutInt0( long numi ) ----------------------------------------------*/
void PutInt0( long numi ) 
{
	unsigned char i;
	unsigned char nums[10];
	
	if ( numi < 0 ) {
		PutChar0( '-' );
		numi = -numi;
	}
	
	i = 0;
	do {	
		nums[i] = numi%10;
		numi = numi/10;
		i++;
	} while ( numi );
	
	while ( i ) {
		i--;
		PutChar0( nums[i] + '0' );
	}

}

void PutHex0( unsigned char data )
{
    unsigned char ascii[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    PutChar0( ascii[(data >> 4) & 0x0F] );
    PutChar0( ascii[data & 0x0F]);
}
