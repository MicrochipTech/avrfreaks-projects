/*
	serialio.c

*/
#include <io.h>
#include <pgmspace.h>
#include <interrupt.h>
#include <sig-avr.h>
#include "..\avrx\avrx.h"
#include "serialio.h"
#include <inttypes.h> 
#include "stdio.h"

#define FALSE 0
#define TRUE 1
#define UCR_INIT  ((1<<TXEN) | (1<<RXEN) | (1<<RXCIE))

#if defined (__AVR_ATmega161__) 
#define UBRR UBRR0	
#define UCR UCSR0B	
#define USR UCSR0A	
/* UCSR0B */
#define RXCIE RXCIE0
#define TXCIE TXCIE0
#define UDRIE UDRIE0
#define RXEN RXEN0
#define	TXEN TXEN0
#define UDR UDR0
#define UDRE UDRE0

#endif

#if defined (__AVR_ATmega163__) || defined (__AVR_ATmega32__ ) ||\
    defined (__AVR_ATmega323__) || defined (__ATmega323__)     ||\
    defined (__AVR_ATmega8__) || defined (__ATmega8__)
	
#define UCR UCSRB
#define USR UCSRA
#endif


static Mutex RxReady, TxReady;
static uint8_t RxChar, prevchar, pushedflag;

// Initialize the USART hardware with the passed baud rate value.

void InitSerialIO(uint8_t ubrr)
{
    outp(ubrr, UBRRL);
    outp(UCR_INIT, UCR);
    sbi(PORTD, PD0);
    pushedflag=FALSE;
}

// Just signal that a charactor has been received.

AVRX_SIGINT(SIG_UART_RECV)
{
    cbi(UCR, RXCIE);                // 10-11 moved out side of prolog
    IntProlog();
    sei();                          // 10-11 added
    RxChar = inp(UDR);
    AvrXIntSetSemaphore(&RxReady);  // 10-11 added "int"
    sbi(UCR, RXCIE);
    Epilog();
}

// Shut down interrupt source, signal waiting task that
// UDR is ready for another charactor.

AVRX_SIGINT(SIG_UART_DATA)
{
    cbi(UCR, UDRIE);
    IntProlog();
    sei();                          // 10-11 added
    AvrXIntSetSemaphore(&TxReady);  // 10-11 added "int"
    Epilog();
}

// Get a charactor from the input stream

int AVRXGetChar(void)
{
    if (pushedflag == TRUE)
    {
        pushedflag = FALSE;
        return prevchar;
    }
    AvrXWaitSemaphore(&RxReady);
    return RxChar;
}

// Put a charactor to the output stream

int AVRXPutChar(char d)
{
    if ((inp(USR) & BV(UDRE)) == 0)
    {
        sbi(UCR, UDRIE);
        AvrXWaitSemaphore(&TxReady);
    }
    outp(d, UDR);
	return 0;
}

// Get a charactor and echo it as well

int _getcharecho(void)
{
    uint8_t t = getchar();
    putchar(t);
    return t;
}

// Push a charactor into the input buffer,
// returning TRUE if OK, FALSE if full.

int _pushchar(uint8_t d)
{
    prevchar = d;
    pushedflag = TRUE;
    return TRUE;
}

// Need PutString, PutHex, GetHex...

void _putflashstring(const char *s)
{
    char d;
    while((d = pgm_read_byte_near((unsigned)s)))
    {
        putchar(d);
        s++;
    }
}
