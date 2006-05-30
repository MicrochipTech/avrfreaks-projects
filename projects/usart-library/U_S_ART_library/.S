;**************************************************************************
;
; File: uart.S
; Author: Artur Lipowski <lal@pro.onet.pl>
; Version: 3.0, May 27 2006
; Copyright 2003-2006 Artur Lipowski
; No guarantees, warrantees, or promises, implied or otherwise.
; May be used for hobby or commercial purposes provided copyright
; notice remains intact.
;
;**************************************************************************

#include "uart_macros.S"


	.section .noinit

#ifdef USE_UART0
_U_error0:	.space 1
		.global _U_error0
#endif

#ifdef USE_UART1
_U_error1:	.space 1
		.global _U_error1
#endif


	.section .text

;****************************************************************************
;
;	void UART_get_error(uint8_t device)
;
	.global	UART_get_error
	.func	UART_get_error
UART_get_error:
	clr	P1
	choose_device 1f, 2f, 3f
3:
#ifndef SKIP_DEVNO_CHECK
	; in case when bad device number supplied return 0xff
	ser	P0
	ret
#endif

1:
#ifdef USE_UART0
	lds	P0, _U_error0
	ret
#endif /* USE_UART0 */
2:
#ifdef USE_UART1
	lds	P0, _U_error1
	ret
#endif /* USE_UART1 */

	.endfunc


;****************************************************************************
;
;	void UART_set_speed(uint8_t device, uint16_t UBRR_val)
;
	.global	UART_set_speed
	.func	UART_set_speed
UART_set_speed:

#ifdef USE_USART
	andi	P3, 0xf		; clear 4 MSB of the UBRRH
#endif /* USE_USART */

	choose_device 1f, 2f, 3f
3:
#ifndef SKIP_DEVNO_CHECK
	ret
#endif

1:
#ifdef USE_UART0

#ifdef HAS_2UARTS
	ext_out	UBRR0H, P3
	ext_out	UBRR0L, P2
#elif defined(USE_USART)
	ext_out	UBRRH, P3
	ext_out	UBRRL, P2
#else /* defined(USE_USART) */
	out	_SFR_IO_ADDR(UBRR), P2
#endif /* HAS_2UARTS */

	sts	_U_error0, __zero_reg__
	ret
#endif /* USE_UART0 */

2:
#ifdef USE_UART1
	ext_out	UBRR1H, P3
	ext_out	UBRR1L, P2
	sts	_U_error1, __zero_reg__
	ret
#endif /* USE_UART1 */

	.endfunc

.end

