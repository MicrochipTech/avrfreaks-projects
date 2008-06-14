.nolist
.include "tn85def.inc"

.equ    TXD_PORT        = PORTB
.equ    TXD             = PB1			; must be OC1A !

.equ    TXD_DDR         = TXD_PORT - 1

.equ	XTAL		= 14745600
;.equ	BAUD		= 1200
;.equ	BAUD		= 115200
;.equ	BAUD		= 230400
.equ	BAUD		= 460800
;.equ	INVERT		= 1<<COM1A0		; inverted
.equ	INVERT		= 0			; non inverted
;-------------------------------------------------------------------------
;				Register usage
;
.def	sregi		= r2		; save sreg in interrupt
.def	tx_data		= r3
.def	fifol		= r4
.def	fifoh		= r5
.def	txipl		= r6		; tx uart in pointer
.def	txiph		= r7

.def	b0		= r8		; scratch register
.def	b1		= r9
.def	b2		= r10
					; r11 .. r15 free

.def	fifo_end_hi	= r16
.def	tx_low		= r17		; send high bit
.def	tx_high		= r18		; send low bit
.def	tx_state	= r19

.def	a0		= r20		; scratch register
.def	a1		= r21
.def	c0		= r22
.def	c1		= r23
.def	c2		= r24
.def	c3		= r25
;-------------------------------------------------------------------------
.dseg
.org	SRAM_START
fifo_start:
	.byte	502
fifo_end:
;-------------------------------------------------------------------------
.cseg
;.list
	rjmp	init				; must be rjmp !
;-------------------------------------------------------------------------
.include "uart.inc"
;-------------------------------------------------------------------------
.list
init:
	ldi	r16, high(RAMEND)
	out	SPH, r16
	ldi	r16, low(RAMEND)
	out	SPL, r16
	ldi	zl, low (XTAL / BAUD)
	ldi	zh, high(XTAL / BAUD)
	rcall	init_uart
	sei
					; Test UART output
	rcall	puts
	.db	"UART test program:", 13, 10, 13, 10, 0, 0
	ldi	c0, 0
	ldi	c1, 0
	ldi	c2, 0
	ldi	c3, 0
main:
	subi	c0, -1			; count up 32 bit value
	sbci	c1, -1
	sbci	c2, -1
	sbci	c3, -1

	mov	a0, c3			; display it hexadecimal 
	rcall	puthex
	mov	a0, c2
	rcall	puthex
	mov	a0, c1
	rcall	puthex
	mov	a0, c0
	rcall	puthex

	rcall	puts			; next line
	.db	13, 10, 0, 0
	rjmp	main
;-------------------------------------------------------------------------
