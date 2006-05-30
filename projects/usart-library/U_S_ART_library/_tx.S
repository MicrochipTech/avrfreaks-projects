;**************************************************************************
;
; File: uart_tx.S
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
_U_tx_buf0:     .space BUFSIZE_TX0
_U_tx_head0:	.space 1
_U_tx_tail0:	.space 1
_U_tx_len0:	    .space 1

; global variables
		.global _U_tx_len0
#endif


#ifdef USE_UART1
_U_tx_buf1:	    .space BUFSIZE_TX1
_U_tx_head1:	.space 1
_U_tx_tail1:	.space 1
_U_tx_len1:	    .space 1

; global variables
		.global _U_tx_len1
#endif

	
	.section .text
;****************************************************************************
;
; data register empty interrupt routine
;
.macro	sig_data dev
	push	__tmp_reg__
	in	__tmp_reg__, _SFR_IO_ADDR(SREG)
	push	work1
	lds	work1, _U_tx_len\dev
	tst	work1				        ; check if there is something to send
	breq	1f
send_byte\dev:
	dec	work1				        ; adjust queue length
	sts	_U_tx_len\dev, work1
	push	work2
	push	loZ
	push	hiZ
	lds	work1, _U_tx_head\dev
	load_Z	_U_tx_buf\dev
	add	loZ, work1
	adc	hiZ, __zero_reg__		    ; calculate _U_tx_buf[_U_tx_head]
	ld	work2, Z
	set_UDR \dev, work2			    ; UDR = _U_tx_buf[_U_tx_head]
	pop	hiZ
	pop	loZ
	pop	work2
	inc	work1                       ; calculate new head pointer
	cpi	work1, BUFSIZE_TX\dev
	brne	store_head_ptr\dev
	clr	work1
store_head_ptr\dev:
	sts	_U_tx_head\dev, work1
1:
	pop	work1
	out	_SFR_IO_ADDR(SREG), __tmp_reg__
	pop	__tmp_reg__
	reti
.endm

#ifdef USE_UART0

#if defined(SIG_UART_DATA)
	.global SIG_UART_DATA
SIG_UART_DATA:
#elif defined(SIG_UART0_DATA)
	.global SIG_UART0_DATA
SIG_UART0_DATA:
#elif defined(SIG_USART0_DATA)
	.global SIG_USART0_DATA
SIG_USART0_DATA:
#elif defined(SIG_USART_DATA)
	.global SIG_USART_DATA
SIG_USART0_DATA:
#else
#error "This chip has no UART interrupts defined"
#endif

	sig_data 0

#endif /* ifdef USE_UART0 */


#ifdef USE_UART1

#if defined(SIG_UART1_DATA)
	.global SIG_UART1_DATA
SIG_UART1_DATA:
#elif defined(SIG_USART1_DATA)
	.global SIG_USART1_DATA
SIG_USART1_DATA:
#else
#error "This chip has no UART interrupts defined"
#endif

	sig_data 1

#endif /* ifdef USE_UART1 */


;****************************************************************************
;
;	void UART_clear_TX_buf(uint8_t device)
;
.macro	tx_clear_buf dev
	clearbit_CR \dev, UDRIE, work1
	rjmp	UART_init_TX
.endm

	.global	UART_clear_TX_buf
	.func	UART_clear_TX_buf
UART_clear_TX_buf:

	choose_device 1f, 2f, 3f
3:
#ifndef SKIP_DEVNO_CHECK
	ret
#endif

1:
#ifdef USE_UART0
	tx_clear_buf 0
#endif /* USE_UART0 */

2:
#ifdef USE_UART1
	tx_clear_buf 1
#endif /* USE_UART1 */

	.endfunc


;****************************************************************************
;
;	uint8_t UART_putchar(uint8_t device, uint8_t a_byte)
;
;
.macro	uputchar dev
	clearbit_CR \dev, UDRIE, work1
	lds	work1, _U_tx_len\dev
	cpi	work1, BUFSIZE_TX\dev	    ; check if TX buffer is full
	brne	check_if_tx_empty\dev
	setbit_CR \dev, UDRIE, work1
	lds	work1, _U_error\dev
	ori	work1, TX_FULL_ERROR
	sts	_U_error\dev, work1	        ; if not set TX_FULL_ERROR flag
	clr	P0			                ; and return 0
	ret
check_if_tx_empty\dev:
	tst	work1
	brne	store_byte\dev
	sbis_SR \dev, UDRE, work2	    ; if it is safe to write to the UDR - just do it
	rjmp	store_byte\dev		    ; omitting buffers
	set_UDR \dev, P2		        ; it also initiates transfer for the first character
	rjmp	putchar_finish_OK_no_ISR\dev
store_byte\dev:
	inc	work1
	sts	_U_tx_len\dev, work1	    ; store queue new length
	lds	work1, _U_tx_tail\dev
	load_Z	_U_tx_buf\dev
	add	loZ, work1
	adc	hiZ, __zero_reg__
	st	Z, P2			            ; _U_tx_buf[_U_tx_tail] = a_byte
	setbit_CR \dev, UDRIE, work1    ; at this point RXE interrupt can be reenabled
	inc	work1			            ; calculate new tail pointer
	cpi	work1, BUFSIZE_TX\dev
	brne	store_tail_ptr\dev
	clr	work1
store_tail_ptr\dev:
	sts	_U_tx_tail\dev, work1
putchar_finish_OK_no_ISR\dev:
	lds	work1, _U_error\dev
	andi	work1, ~TX_FULL_ERROR	; clear TX_FULL_ERROR flag
	sts	_U_error\dev, work1
	ldi	P0, 1				        ; all OK - return 1
	ret
.endm

	.global	UART_putchar
	.func	UART_putchar
UART_putchar:
	choose_device 1f, 2f, 3f
3:
#ifndef SKIP_DEVNO_CHECK
	ret
#endif

1:
#ifdef USE_UART0
	uputchar 0
#endif /* USE_UART0 */

2:
#ifdef USE_UART1
	uputchar 1
#endif /* USE_UART1 */
	.endfunc

;****************************************************************************
;
;       void UART_init_TX(uint8_t device)
;
.macro	tx_init dev
	sts	_U_tx_len\dev, __zero_reg__
	sts	_U_tx_head\dev, __zero_reg__
    sts	_U_tx_tail\dev, __zero_reg__
	lds	work1, _U_error\dev
	andi	work1, ~TX_FULL_ERROR
	sts	_U_error\dev, work1
	set_CR_bits \dev, _BV(TXEN), work1
    ret
.endm

        .global UART_init_TX
        .func   UART_init_TX
UART_init_TX:
	choose_device 1f, 2f, 3f
3:
#ifndef SKIP_DEVNO_CHECK
	ret
#endif

1:
#ifdef USE_UART0
	tx_init 0
#endif /* USE_UART0 */

2:
#ifdef USE_UART1
	tx_init 1
#endif /* USE_UART1 */
        .endfunc


.end

