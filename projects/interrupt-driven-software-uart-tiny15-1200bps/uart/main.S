#include <avr/io.h>
#include "UART.h"

.global main

main:
	; calibrate clock.  YOU MUST USE YOUR CALIBRATION VALUE FROM YOUR TINY15.
	; dont use 0x60 and just expect it to work.
	ldi	r31,			0x60
	out	_SFR_IO_ADDR(OSCCAL),		r31

	rcall	UART_init

	loop:

	mov	r31,				rx_buffer_valid
	cpi	r31,				1
	brge	ECHO_BACK

	rjmp	loop

ECHO_BACK:

#ifdef UART_GETCHAR_RETADDR
	rjmp UART_getchar
	.global UART_GETCHAR_RETADDR
	UART_GETCHAR_RETADDR:
#else
	rcall	UART_getchar
#endif

#ifdef UART_PUTCHAR_RETADDR
	rjmp UART_putchar
	.global UART_PUTCHAR_RETADDR 
	UART_PUTCHAR_RETADDR:
#else
	rcall UART_putchar
#endif

	rjmp	loop


