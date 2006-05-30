/*******************************************************************************
 * File: uart_util_sleep_rx.c
 * Author: Artur Lipowski <LAL@pro.onet.pl>
 * Version: 3.0, May 27 2006
 * Copyright 2003-2006 Artur Lipowski
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *******************************************************************************/

#include <avr/interrupt.h>
#include <uart_util.h>

/*******************************************************************************
 *
 *
 *
 *******************************************************************************/
void UART_sleep_RX(uint8_t device)
{
     cli();
     while (0 == UART_get_RX_len(device))
     {
            MCUCR |= _BV(SE);
            asm volatile("sei\n\t"
                         "sleep\n\t"
                         ::);
            MCUCR &= ~_BV(SE);
    }
    sei();
}

