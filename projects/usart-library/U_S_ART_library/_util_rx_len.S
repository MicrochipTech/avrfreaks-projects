;**************************************************************************
;
; File: uart_util_rx_len.S
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
;	uint8_t UART_get_RX_len(uint8_t device)
;
.macro	getrxlen dev
	lds	P0, _U_rx_len\dev
	clr	P1
	ret
.endm

	.global	UART_get_RX_len
	.func	UART_get_RX_len
UART_get_RX_len:
	choose_device 1f, 2f, 3f
3:
#ifndef SKIP_DEVNO_CHECK
	ret
#endif

1:
#ifdef USE_UART0
	getrxlen 0
#endif /* USE_UART0 */

2:
#ifdef USE_UART1
	getrxlen 1
#endif /* USE_UART1 */
	.endfunc

.end

