/*
avr_uart.h

provide functions to set up uart

Copyright Henrik Björkman www.eit.se/hb 2007

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

If you do modify it please make a note about it in the history 
section below.


History

2005-02-16
Modified for atmega8 and renamed. 
Henrik Björkman

2007-03-15
Modified for atmega88.
Henrik Björkman

*/

#ifndef AVR_UART_H
#define AVR_UART_H


void uart_init(void);
void uart_putchar( unsigned char data );
unsigned char uart_waitchar(void);
int uart_getchar(void);

void uart_print_P(const char *addr);






#endif // AVR_UART_H
