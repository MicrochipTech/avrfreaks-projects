;**************************************************************************
;
; File: uart_get.S
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

	.section .noinit
_U_rx_getbuf:	.space 1

	.section .text

;****************************************************************************
;
;	int UART_get(uint8_t device)
;
;
	.global	UART_get
	.func	UART_get
UART_get:
	ldi	P2, lo8(_U_rx_getbuf)
	ldi	P3, hi8(_U_rx_getbuf)
	rcall	UART_getchar
	and	P0, P0
	breq	1f
	load_Z	_U_rx_getbuf
	ld	P0, Z		; return character when success
	clr	P1
	ret
1:
	ser	P0		; return -1 in case of error
	ser	P1
	ret
	.endfunc

.end

