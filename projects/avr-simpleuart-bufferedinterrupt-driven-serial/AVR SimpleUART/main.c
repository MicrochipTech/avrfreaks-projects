// v1.0 stevech@san.rr.com

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UART.h"


// test/demo of UART.c routines
// change baud rate versus CPU clock freq in UART.h.


int main(void) {
    unsigned char s[80];
	int n;
	
		
    UART_init();	// in UART.c
	sei(); 			// Global interrupt enable
	
	//fdevopen( UART_fputc, UART_fgetc );			// establish buffered I/O callbacks

	for(;;) {
		// this demos the small memory function set. See below for FILE buffered version.
		UART_puts_P(PSTR("Enter text\r\n"));
		n = UART_gets(s, sizeof(s)-2);
		s[strlen(s)-1] = 0;	// drop the ending \r
		UART_puts_P(PSTR("You entered '"));
		UART_puts(s);
		UART_puts_P(PSTR("' length="));
		UART_puts(itoa(n, s, 10));
		UART_puts_P(PSTR("\n\r"));
		
		// uncomment this to use the buffered I/O (memory hog). 
		//   and fdevopen(), above too.
		
		/* Same function as printf() but the format string resides in flash */
		//printf_P(PSTR("Enter text\r\n"));
		//fgets(s, sizeof(s)-1, stdin);
		//s[strlen(s)-2] = 0;	// drop the ending \r\n
		//printf_P(PSTR("\r\nYou entered '%s'\r\n"),s);
	}
}

