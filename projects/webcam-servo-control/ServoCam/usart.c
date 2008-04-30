//***************************************************************************
//
//  File........: usart.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: IAR EWAAVR 4.20a
//
//  Description.: AVR Butterfly USART routines
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - LHM
//
//***************************************************************************

#include <inavr.h>

#include "iom169.h"
#include "main.h"
#include "usart.h"


/*****************************************************************************
*
*   Function name : USART_Init
*
*   Returns :       None
*
*   Parameters :    unsigned int baudrate
*
*   Purpose :       Initialize the USART
*
*****************************************************************************/
void USART_Init(unsigned int baudrate)
{
    // Set baud rate
    UBRR0H = (unsigned char)(baudrate>>8);
    UBRR0L = (unsigned char)baudrate;

    // Enable 2x speed
    UCSR0A = (1<<U2X0);

    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(0<<TXEN0)|(0<<RXCIE0)|(0<<UDRIE0);

    // Async. mode, 8N1
    UCSR0C = (0<<UMSEL0)|(0<<UPM00)|(0<<USBS0)|(3<<UCSZ00)|(0<<UCPOL0);
}


/*****************************************************************************
*
*   Function name : Usart_Tx
*
*   Returns :       None
*
*   Parameters :    char data: byte to send
*
*   Purpose :       Send one byte through the USART
*
*****************************************************************************/
void Usart_Tx(char data)
{
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}


/*****************************************************************************
*
*   Function name : Usart_Rx
*
*   Returns :       char: byte received
*
*   Parameters :    None
*
*   Purpose :       Receives one byte from the USART
*
*****************************************************************************/
char Usart_Rx(void)
{
    while (!(UCSR0A & (1<<RXC0)));
    return UDR0;
}
