;**************************************************************************
;
; File: uart_put.S
; Author: Artur Lipowski <lal@pro.onet.pl>
; Version: 3.0, May 27 2006
; Copyright 2003-2006 Artur Lipowski
;
; No guarantees, warrantees, or promises, implied or otherwise.
; May be used for hobby or commercial purposes provided copyright
; notice remains intact.
;
;**************************************************************************

#include "uart_macros.S"

	.section .text

;****************************************************************************
;
;	int UART_put(uint8_t device, uint8_t a_byte)
;
;
	.global	UART_put
	.func	UART_put
UART_put:
	rcall	UART_putchar
	tst	P0
	breq	1f
	clr	P0		; return 0 when success
	clr	P1
	ret
1:
	ser	P0		; return -1 in case of error
	ser	P1
	ret
	.endfunc

.end

