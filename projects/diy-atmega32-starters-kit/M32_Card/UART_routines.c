//**************************************************************
//******** FUNCTIONS FOR SERIAL COMMUNICATION USING UART *******
//**************************************************************
//Controller:	ATmega32 (16Mhz Crystal)
//Compiler:		ICCAVR
//Author:		CC Dharmani,Chennai, India
//Date:			Dec 2007
//**************************************************************

#include "UART_routines.h"
#include "LCD_routines.h"
#include <iom32v.h>
#include <macros.h>


//**************************************************
//Function to receive a single byte
//*************************************************
unsigned char receiveByte( void )
{
	unsigned char data, status;
	
	while(!(UCSRA & (1<<RXC))); 	// Wait for incomming data
	
	status = UCSRA;
	data = UDR;

	return(data);
}

//***************************************************
//Function to transmit a single byte
//***************************************************
void transmitByte( unsigned char data )
{
	while ( !(UCSRA & (1<<UDRE)) )
		; 			                /* Wait for empty transmit buffer */
	UDR = data; 			        /* Start transmition */
}

//***************************************************
//Function to transmit a string in Flash
//***************************************************
void transmitString_F(const unsigned char* string)
{
    while (*string)
		transmitByte(*string++);
}

//***************************************************
//Function to transmit a string in RAM
//***************************************************
void transmitString(unsigned char* string)
{
    while (*string)
		transmitByte(*string++);
}
