;**************************************************************************
;
; File: uart_rx.S
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
;
; RX/TX data buffers are treated as ring structures
; pointers (head, tail) to actual data are stored as ofssets
;
#ifdef USE_UART0
_U_rx_buf0:	    .space BUFSIZE_RX0
_U_rx_head0:	.space 1
_U_rx_tail0:	.space 1
_U_rx_len0:	    .space 1

; global variables
		.global _U_rx_len0
#endif


#ifdef USE_UART1
_U_rx_buf1:	    .space BUFSIZE_RX1
_U_rx_head1:	.space 1
_U_rx_tail1:	.space 1
_U_rx_len1:	    .space 1

; global variables
		.global _U_rx_len1
#endif

	.section .text
;****************************************************************************
;
; receive complete interrupt routine(s)
; in case of full RX buffer following bytes are lost
; and in such case RX_FULL_ERROR flag is set 
;
.macro	sig_recv dev
	push	__tmp_reg__
	in	__tmp_reg__, _SFR_IO_ADDR(SREG)
	push	work1
	push	work2
	load_SR \dev, work2
	get_UDR \dev, work1		    ; read UDR ASAP and unconditionally
	andi	work2, FE | PE		; in case of frame or parity errors
	brne	1f			        ; do not load received byte into buffer
	lds	work2, _U_rx_len\dev
	cpi	work2, BUFSIZE_RX\dev
	brne	store_byte\dev
	lds	work1, _U_error\dev
	ori	work1, RX_FULL_ERROR
	sts	_U_error\dev, work1
	rjmp	1f
store_byte\dev:
	inc	work2
	sts	_U_rx_len\dev, work2	; store new value of queue length
	lds	work2, _U_rx_tail\dev
	push	loZ
	push	hiZ
	load_Z	_U_rx_buf\dev
	add	loZ, work2
	adc	hiZ, __zero_reg__
	st	Z, work1		        ; store received byte in _U_rx_buf[tail] 
	inc	work2			        ; calculate new tail pointer
	cpi	work2, BUFSIZE_RX\dev
	brne	store_tail_ptr\dev
	clr	work2
store_tail_ptr\dev:
	sts	_U_rx_tail\dev, work2	; store new value of tail pointer
	pop	hiZ
	pop	loZ
1:
	pop	work2
	pop	work1
	out	_SFR_IO_ADDR(SREG), __tmp_reg__
	pop	__tmp_reg__
	reti
.endm

#ifdef USE_UART0

#if defined(SIG_UART_RECV)
	.global SIG_UART_RECV
SIG_UART_RECV:
#elif defined(SIG_UART0_RECV)
	.global SIG_UART0_RECV
SIG_UART0_RECV:
#elif defined(SIG_USART0_RECV)
	.global SIG_USART0_RECV
SIG_USART0_RECV:
#elif defined(SIG_USART_RECV)
	.global SIG_USART_RECV
SIG_USART_RECV:
#else
#error "This chip has no UART interrupts defined"
#endif

	sig_recv 0

#endif /* ifdef USE_UART0 */


#ifdef USE_UART1
#if defined(SIG_UART1_RECV)
	.global SIG_UART1_RECV
SIG_UART1_RECV:
#elif defined(SIG_USART1_RECV)
	.global SIG_USART1_RECV
SIG_USART1_RECV:
#else
#error "This chip has no UART interrupts defined"
#endif

	sig_recv 1

#endif /* ifdef USE_UART1 */


;****************************************************************************
;
;	void UART_clear_RX_buf(uint8_t device)
;
;
.macro	rx_clear_buf dev
	clearbit_CR \dev, RXCIE, work1
	rjmp	UART_init_RX
.endm

	.global	UART_clear_RX_buf
	.func	UART_clear_RX_buf
UART_clear_RX_buf:

	choose_device 1f, 2f, 3f
3:

#ifndef SKIP_DEVNO_CHECK
	ret
#endif

1:
#ifdef USE_UART0
	rx_clear_buf 0
#endif /* USE_UART0 */

2:
#ifdef USE_UART1
	rx_clear_buf 1
#endif /* USE_UART1 */

	.endfunc

;****************************************************************************
;
;	uint8_t UART_getchar(uint8_t device, uint8_t *a_byte)
;
.macro	getchar dev
	clearbit_CR \dev, RXCIE, work1
	lds	work1, _U_rx_len\dev
	tst	work1
	brne	buf_not_empty\dev
	setbit_CR \dev, RXCIE, work1
	clr	P0				            ; if RX buffer is empty return 0
	ret
buf_not_empty\dev:
	dec	work1				        ; adjust queue length
	sts	_U_rx_len\dev, work1
	lds	work1, _U_rx_head\dev
	load_Z	_U_rx_buf\dev
	add	loZ, work1
	adc	hiZ, __zero_reg__
	ld	work2, Z			        ; work2 = _U_rx_buf[_U_rx_head]
	inc	work1				        ; calculate new head pointer
	cpi	work1, BUFSIZE_RX\dev
	brne	store_head_ptr\dev
	clr	work1
store_head_ptr\dev:
	sts	_U_rx_head\dev, work1
	setbit_CR \dev, RXCIE, work1	; (re)enable RX interrupt ASAP
	mov	loZ, P2
	mov	hiZ, P3
	st	Z, work2			        ; *a_byte = work2
	lds	work1, _U_error\dev
	andi	work1, ~RX_FULL_ERROR	; clear RX_FULL_ERROR flag
	sts	_U_error\dev, work1
	ldi	P0, 1				        ; all OK - return 1
	ret
.endm

	.global	UART_getchar
	.func	UART_getchar
UART_getchar:
	clr	P1

	choose_device 1f, 2f, 3f
3:
#ifndef SKIP_DEVNO_CHECK
	ret
#endif

1:
#ifdef USE_UART0
	getchar 0
#endif /* USE_UART0 */

2:
#ifdef USE_UART1
	getchar 1
#endif /* USE_UART1 */

	.endfunc

;****************************************************************************
;
;       void UART_init_rx(uint8_t device)
;
.macro	rx_init dev
    sts	_U_rx_head\dev, __zero_reg__
    sts	_U_rx_tail\dev, __zero_reg__
    sts	_U_rx_len\dev, __zero_reg__
	lds	work1, _U_error\dev
	andi	work1, ~RX_FULL_ERROR
	sts	_U_error\dev, work1
	get_UDR \dev, work1
	set_CR_bits \dev, _BV(RXCIE)|_BV(RXEN), work1
    ret
.endm

        .global UART_init_RX
        .func   UART_init_RX
UART_init_RX:
	choose_device 1f, 2f, 3f
3:
#ifndef SKIP_DEVNO_CHECK
	ret
#endif

1:
#ifdef USE_UART0
	rx_init 0
#endif /* USE_UART0 */

2:
#ifdef USE_UART1
	rx_init 1
#endif /* USE_UART1 */
        .endfunc


.end

