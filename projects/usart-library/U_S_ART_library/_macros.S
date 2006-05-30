;**************************************************************************
;
; File: uart_macros.S
; Author: Artur Lipowski <LAL@pro.onet.pl>
; Version: 3.0, May 27 2006
; Copyright 2003-2006 Artur Lipowski
;
; No guarantees, warrantees, or promises, implied or otherwise.
; May be used for hobby or commercial purposes provided copyright
; notice remains intact.
;
;**************************************************************************

#include <avr/io.h>
#if defined(__AVR_LIBC_VERSION__) && __AVR_LIBC_VERSION__ >=  10400UL
#include <avr/interrupt.h>
#else
#include <avr/signal.h>
#endif

#include "uart_guard.h"

#ifndef __tmp_reg__
#define __tmp_reg__     r0
#endif
#ifndef __zero_reg__
#define __zero_reg__    r1
#endif

P0     = 24			; first incoming parameter (MSB)
P1     = 25			;
P2     = 22			; 2nd incoming parameter
P3     = 23			;
P4     = 20			; 3rd incoming parameter
P5     = 21			;
P6     = 18			; 4th incoming parameter
P7     = 19			;

work1	= 18
work2	= 19
work3	= 20
loZ     = 30
hiZ     = 31

;****************************************************************************
;
;	load_Z
;
.macro load_Z p
	ldi	r30, lo8(\p)
	ldi	r31, hi8(\p)
.endm

#define TX_FULL_ERROR   _BV(1)
#define RX_FULL_ERROR   _BV(2)

#define DEFAULT_QUEUE_LENGTH 8


#ifdef USE_UART0

#ifdef UART0_TX_BUFSIZE
#if (UART0_TX_BUFSIZE < 0) || (UART0_TX_BUFSIZE > 255)
#error "UART0_TX_BUFSIZE (buffer size) is not between 0 and 255"
#endif
BUFSIZE_TX0 = UART0_TX_BUFSIZE
#else
BUFSIZE_TX0 = DEFAULT_QUEUE_LENGTH
#endif

#ifdef UART0_RX_BUFSIZE
#if (UART0_RX_BUFSIZE < 0) || (UART0_RX_BUFSIZE > 255)
#error "UART0_RX_BUFSIZE (buffer size) is not between 0 and 255"
#endif
BUFSIZE_RX0 = UART0_RX_BUFSIZE
#else
BUFSIZE_RX0 = DEFAULT_QUEUE_LENGTH
#endif

#endif /* ifdef USE_UART0 */


#ifdef USE_UART1
	
#ifdef UART1_TX_BUFSIZE
#if (UART1_TX_BUFSIZE < 0) || (UART1_TX_BUFSIZE > 255)
#error "UART1_TX_BUFSIZE (buffer size) is not between 0 and 255"
#endif
BUFSIZE_TX1 = UART1_TX_BUFSIZE
#else
BUFSIZE_TX1 = DEFAULT_QUEUE_LENGTH
#endif

#ifdef UART1_RX_BUFSIZE
#if (UART1_RX_BUFSIZE < 0) || (UART1_RX_BUFSIZE > 255)
#error "UART1_RX_BUFSIZE (buffer size) is not between 0 and 255"
#endif
BUFSIZE_RX1 = UART1_RX_BUFSIZE
#else
BUFSIZE_RX1 = DEFAULT_QUEUE_LENGTH
#endif

#endif /* ifdef USE_UART1 */

;****************************************************************************
;
;	choose_device
;
.macro choose_device	handle_uart0, handle_uart1, bad_param

#ifndef SKIP_DEVNO_CHECK
	cpi	P0, 0
#ifdef USE_UART0
	breq	\handle_uart0
#else /* USE_UART0 */
	breq	\bad_param
#endif /* USE_UART0 */

#ifdef USE_UART1
	cpi	P0, 1
	breq	\handle_uart1
#endif /* USE_UART1 */
#endif /* ifndef SKIP_DEVNO_CHECK */

.endm


;****************************************************************************
;
;	ext_out
;
.macro ext_out	io, reg
.ifge	63 - \io
	out	_SFR_IO_ADDR(\io), \reg
.else
	sts	_SFR_MEM_ADDR(\io), \reg
.endif
.endm

;****************************************************************************
;
;	ext_in
;
.macro ext_in	reg, io
.ifge	63 - \io
	in	\reg, _SFR_IO_ADDR(\io)
.else
	lds	\reg, _SFR_MEM_ADDR(\io)
.endif
.endm


#ifdef USE_USART
#ifdef HAS_2UARTS
#define CR0_REG UCSR0B
#define CR1_REG UCSR1B
#else /* HAS_2UARTS */
#define CR0_REG UCSRB
#endif /* HAS_2UARTS */
#else /* USE_USART */
#define CR0_REG UCR
#endif /* USE_USART */

;****************************************************************************
;
;	set_CR_bits
;
.macro set_CR_bits dev, bits, tmp
.if \dev
crreg = CR1_REG
.else
crreg = CR0_REG
.endif
	;in	\tmp, _SFR_IO_ADDR(crreg)
	ext_in	\tmp, crreg
	ori	\tmp, \bits
	ext_out	crreg, \tmp
	;out	_SFR_IO_ADDR(crreg), \tmp
.endm



;****************************************************************************
;
;	setbit_CR
;
.macro setbit_CR dev, bit, tmp
.if \dev
crreg = CR1_REG
.else
crreg = CR0_REG
.endif
.ifge	63 - crreg
	sbi	_SFR_IO_ADDR(crreg), \bit
.else
	set_CR_bits \dev, _BV(\bit), \tmp
.endif
.endm

;****************************************************************************
;
;	clearbit_CR
;
.macro clearbit_CR dev, bit, tmp
.if \dev
crreg = CR1_REG
.else
crreg = CR0_REG
.endif
.ifge	63 - crreg
	cbi	_SFR_IO_ADDR(crreg), \bit
.else
	ext_in	\tmp, crreg
	andi	\tmp, ~_BV(\bit)
	ext_out	crreg, \tmp
.endif
.endm



#ifdef HAS_2UARTS
#define UDR0_REG UDR0
#define UDR1_REG UDR1
#else
#define UDR0_REG UDR
#endif /* HAS_2UARTS */

;****************************************************************************
;
;	sbic_CR
;
.macro sbic_CR dev, bit, tmpreg
.if \dev
srreg = CR1_REG
.else
srreg = CR0_REG
.endif
.ifge	63 - srreg
	sbic	_SFR_IO_ADDR(srreg), \bit
.else
	lds	\tmpreg, _SFR_MEM_ADDR(srreg)
	sbrc	\tmpreg, \bit
.endif
.endm

;****************************************************************************
;
;	sbis_CR
;
.macro sbis_CR dev, bit, tmpreg
.if \dev
srreg = CR1_REG
.else
srreg = CR0_REG
.endif
.ifge	63 - srreg
	sbis	_SFR_IO_ADDR(srreg), \bit
.else
	lds	\tmpreg, _SFR_MEM_ADDR(srreg)
	sbrs	\tmpreg, \bit
.endif
.endm

;****************************************************************************
;
;	get_UDR
;
.macro get_UDR dev, reg
.if \dev
udrreg = UDR1_REG
.else
udrreg = UDR0_REG
.endif
	;in	\reg, _SFR_IO_ADDR(udrreg)
	ext_in	\reg, udrreg
.endm

;****************************************************************************
;
;	set_UDR
;
.macro set_UDR dev, reg
.if \dev
udrreg = UDR1_REG
.else
udrreg = UDR0_REG
.endif
	;out	_SFR_IO_ADDR(udrreg), \reg
	ext_out	udrreg, \reg
.endm


#ifdef USE_USART
#ifdef HAS_2UARTS
#define SR0_REG UCSR0A
#define SR1_REG UCSR1A
#else /* HAS_2UARTS */
#define SR0_REG UCSRA
#endif /* HAS_2UARTS */
#else /* USE_USART */
#define SR0_REG USR
#endif /* USE_USART */

;****************************************************************************
;
;	load_SR
;
.macro load_SR dev, dstreg
.if \dev
srreg = SR1_REG
.else
srreg = SR0_REG
.endif
	ext_in	\dstreg, srreg
.endm

;****************************************************************************
;
;	sbic_SR
;
.macro sbic_SR dev, bit, tmpreg
.if \dev
srreg = SR1_REG
.else
srreg = SR0_REG
.endif
.ifge	63 - srreg
	sbic	_SFR_IO_ADDR(srreg), \bit
.else
	lds	\tmpreg, _SFR_MEM_ADDR(srreg)
	sbrc	\tmpreg, \bit
.endif
.endm

;****************************************************************************
;
;	sbis_SR
;
.macro sbis_SR dev, bit, tmpreg
.if \dev
srreg = SR1_REG
.else
srreg = SR0_REG
.endif
.ifge	63 - srreg
	sbis	_SFR_IO_ADDR(srreg), \bit
.else
	lds	\tmpreg, _SFR_MEM_ADDR(srreg)
	sbrs	\tmpreg, \bit
.endif
.endm


