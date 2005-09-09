/*******************************************************************************
* Filename: sercom.h
*
* Author: Preston K. Manwaring & Donald Wichern
*
* Date: July 3, 2003
*
* Description: This file contains headers for sercom.c
*
*******************************************************************************/

#ifndef SERCOM_H
#define SERCOM_H

#include <avr/io.h>

/*--- FUNCTION PROTOTYPES for SERCOM.C --------------------------------------*/
unsigned char Hit0( void );
void SerCom0Init( unsigned int baud );
void PutChar0( unsigned char byte );
unsigned char GetChar0( void );
void PutString0( unsigned char *str );
void PutInt0( long numi );
void PutHex0( unsigned char data );

#endif

/* values based on 16 MHz */
#define BAUD_2400  416
#define BAUD_9600  103
#define BAUD_19200 51
