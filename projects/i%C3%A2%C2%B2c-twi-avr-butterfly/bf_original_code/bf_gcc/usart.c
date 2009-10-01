//***************************************************************************
//
//  File........: usart.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: avr-gcc / avr-libc
//
//  Description.: AVR Butterfly USART routines
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - LHM
//  2004            - corrected comment (only receiver enabled)     - mt
//  20070517        - option to enable transmit                     - mt
//***************************************************************************

//mtA
//#include <inavr.h>
//#include "iom169.h"
#include <avr/io.h>
//mtE
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
    UBRRH = (unsigned char)(baudrate>>8);
    UBRRL = (unsigned char)baudrate;

    // Enable 2x speed
    UCSRA = (1<<U2X);

#ifdef WITH_TRANSMIT
    // enabled receive and transmit
    UCSRB = (1<<RXEN)|(1<<TXEN)|(0<<RXCIE)|(0<<UDRIE);
#else
    // Enable receive. transmit stays disabled
    // In the default BF demo-application no data gets 
    // send (only receive in vcard).
    UCSRB = (1<<RXEN)|(0<<TXEN)|(0<<RXCIE)|(0<<UDRIE);
#endif

    // Async. mode, 8N1
    UCSRC = (0<<UMSEL)|(0<<UPM0)|(0<<USBS)|(3<<UCSZ0)|(0<<UCPOL);
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
#ifdef WITH_TRANSMIT
void Usart_Tx(char data)
{
    while (!(UCSRA & (1<<UDRE)));
    UDR = data;
}
#endif


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
    while (!(UCSRA & (1<<RXC)));
    return UDR;
}
