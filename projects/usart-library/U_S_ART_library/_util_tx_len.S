;**************************************************************************
;
; File: uart_util_tx_len.S
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
;	uint8_t UART_get_TX_len(uint8_t device)
;
.macro	gettxlen dev
	lds	P0, _U_tx_len\dev
	clr	P1
	ret
.endm

	.global	UART_get_TX_len
	.func	UART_get_TX_len
UART_get_TX_len:
	choose_device 1f, 2f, 3f
3:
#ifndef SKIP_DEVNO_CHECK
	ret
#endif

1:
#ifdef USE_UART0
	gettxlen 0
#endif /* USE_UART0 */

2:
#ifdef USE_UART1
	gettxlen 1
#endif /* USE_UART1 */
	.endfunc

.end

