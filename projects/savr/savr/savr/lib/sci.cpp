/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>

#include <savr/sci.h>
#include <savr/queue.h>

#if defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
    #define __BAUD_HIGH    UBRRH
    #define __BAUD_LOW     UBRRL
    #define __CTRLA        UCSRA
    #define __CTRLB        UCSRB
    #define __CTRLC        UCSRC
    #define __CTRLB_UDRIE  UDRIE
    #define __CTRLB_RXCIE  RXCIE
    #define __CTRLB_RXEN   RXEN
    #define __CTRLB_TXEN   TXEN
    #define __CTRLC_ENABLE _BV(URSEL)
    #define __CTRLC_UCSZ1  UCSZ1
    #define __CTRLC_UCSZ0  UCSZ0
    #define __DATAR        UDR
    #define __RX_VECT      USART_RXC_vect
    #define __TX_VECT      USART_UDRE_vect
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) \
   || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168__)
    #define __BAUD_HIGH    UBRR0H
    #define __BAUD_LOW     UBRR0L
    #define __CTRLA        UCSR0A
    #define __CTRLB        UCSR0B
    #define __CTRLC        UCSR0C
    #define __CTRLB_UDRIE  UDRIE0
    #define __CTRLB_RXCIE  RXCIE0
    #define __CTRLB_RXEN   RXEN0
    #define __CTRLB_TXEN   TXEN0
    #define __CTRLC_ENABLE 0
    #define __CTRLC_UCSZ1  UCSZ01
    #define __CTRLC_UCSZ0  UCSZ00
    #define __DATAR        UDR0
    #define __RX_VECT      USART_RX_vect
    #define __TX_VECT      USART_UDRE_vect
#else
#error Unsupported AVR target for SCI interface
#endif

static FILE mystdout;
static FILE mystdin;

static int PutChar(char, FILE *);
static int GetChar(FILE *);

#define __GETBAUD(base, baud) (base/16/(baud)-1)

//! Circular recieve buffer
static Queue TxBuffer;
//! Circular transmit buffer
static Queue RxBuffer;



/**
 * Put a character into the UART queue
 *
 * Blocking Function - Place a single character on the TxBuffer
 */
int
PutChar(char input, FILE * stream)
{
    uint8_t err = 0;
    if(input == '\n')
        PutChar('\r', stream);

    do{
        cli();
        err = TxBuffer.Enq(input);
        sei();
    }while(err);
    __CTRLB |= _BV(__CTRLB_UDRIE);
    return 0;
}


/**
 * Get a character from the UART queue
 *
 * Blocking Function - Get next char on the RxBuffer
 */
int
GetChar(FILE * stream)
{
    char ret_val = 0;
    uint8_t err  = 0;
    do{
        cli();
        err = RxBuffer.Deq((uint8_t *)&ret_val);
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
SCI::Init(uint32_t sysClock, uint32_t baud)
{
    // Set Baud Rate.
    uint16_t brate = (uint16_t)__GETBAUD(sysClock, baud);
    __BAUD_HIGH = (uint8_t)(brate>>8);
    __BAUD_LOW  = (uint8_t)(brate);

    /* Enable Rx and Tx, and interrupt */
    __CTRLB = _BV(__CTRLB_RXCIE) | _BV(__CTRLB_RXEN) | _BV(__CTRLB_TXEN);

    /* Frame Format - 8 data, no parity */
    /* NEED URSEL FOR MEGA16/32 */
    __CTRLC = __CTRLC_ENABLE | _BV(__CTRLC_UCSZ1) | _BV(__CTRLC_UCSZ0);// | _BV(UPM1) | _BV(UPM0);

    stdout  = &mystdout;
    stdin   = &mystdin;
    fdev_setup_stream(stdout, PutChar, NULL, _FDEV_SETUP_WRITE);
    fdev_setup_stream(stdin,  NULL, GetChar, _FDEV_SETUP_READ);
}


/**
 * Handle recieved data
 */
ISR(__RX_VECT)
{
    uint8_t rx_data = __DATAR;
    RxBuffer.Enq(rx_data); // Fail silently
}


/**
 * Handle transmitting data
 */
ISR(__TX_VECT)
{
    uint8_t tx_data;
    uint8_t err = 0;
    err = TxBuffer.Deq(&tx_data);
    if(err)
        __CTRLB &= ~_BV(__CTRLB_UDRIE);
    else
        __DATAR = tx_data;
}
