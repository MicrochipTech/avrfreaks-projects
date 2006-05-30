;**************************************************************************
;
; File: uart_util_rx_wait.S
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
;	void UART_wait_TX_empty(uint8_t device)
;
.macro	waitrx dev
5:
	lds	work1, _U_rx_len\dev
	tst	work1
	breq	5b
    
	ret
.endm

	.global	UART_wait_RX
	.func	UART_wait_RX
UART_wait_RX:
	choose_device 1f, 2f, 3f
3:
#ifndef SKIP_DEVNO_CHECK
	ret
#endif

1:
#ifdef USE_UART0
	waitrx 0
#endif /* USE_UART0 */

2:
#ifdef USE_UART1
	waitrx 1
#endif /* USE_UART1 */
	.endfunc

.end

