;************************************************************************/
;*                                                                      */
;*                      I2C Sniffer					*/
;*                                                                      */
;*              Author: Peter Dannegger                                 */
;*                                                                      */
;************************************************************************/
.nolist
.include "tn85def.inc"

.equ	TXD_PORT	= PORTB
.equ	TXD		= PB1			; must be OC1A !

.equ	I2C_PORT	= PORTB
.equ	SCL		= PB2			; must be USI !
.equ	SDA		= PB0

.equ	START	= 's'
.equ	ACK	= 'a'
.equ	NACK	= 'n'
.equ	STOP	= 'p'

.equ	XTAL		= 14745600
;.equ	BAUD		= 115200
;.equ	BAUD		= 230400
.equ	BAUD		= 460800
.equ	INVERT		= 1<<COM1A0		; inverted
;.equ	INVERT		= 0			; non inverted

.equ	TXD_DDR		= TXD_PORT - 1
.equ	I2C_DDR		= I2C_PORT - 1
.equ	I2C_PIN		= I2C_PORT - 2
;-------------------------------------------------------------------------
;				Register usage
;
.def	sregi		= r0		; save sreg in interrupt
.def	tx_data		= r1		; UART data byte
.def	fifol		= r2
.def	fifoh		= r3
.def	txipl		= r4		; tx uart in pointer
.def	txiph		= r5
.def	b0		= r6		; scratch register
.def	b1		= r7
.def	b2		= r8
.def	get_ack		= r9
					; r10 .. r15 free

.def	fifo_end_hi	= r16
.def	tx_low		= r17		; send high bit
.def	tx_high		= r18		; send low bit
.def	tx_state	= r19
.def	a0		= r20		; scratch register
					; r21 .. r25 free
;-------------------------------------------------------------------------
.dseg
.org	SRAM_START
fifo_start:
	.byte	502
fifo_end:
;-------------------------------------------------------------------------
.cseg
.list
	rjmp	init				; must be rjmp !
;-------------------------------------------------------------------------
.include "uart.inc"
.list
;-------------------------------------------------------------------------
init:
	ldi	zl, low (RAMEND)
	ldi	zh, high(RAMEND)
	out	SPL, zl
	out	SPH, zh
	ldi	zl,  low(XTAL / BAUD)
	ldi	zh, high(XTAL / BAUD)
	rcall	init_uart
	sei
	rcall	puts
	.db	"I2C-Sniffer:", 13, 10, 0, 0
init_i2c:
	ldi	a0, 1<<USIWM1^1<<USIWM0		; I2C, no counter
	out	USICR, a0
	sbi	I2C_PORT, SCL
	sbi	I2C_DDR, SCL
	sbi	I2C_PORT, SDA
	cbi	I2C_DDR, SDA
;-------------------------------------------------------------------------
i2c_sniff:
	in	a0, USISR
	sbrs	a0, USIPF			; STOP received ?
	rjmp	_isn1
	ldi	a0, 1<<USIWM1^1<<USIWM0		; I2C, no counter
	out	USISR, a0
	ldi	a0, 1<<USIPF			; clear USISIF
	out	USISR, a0
	rcall	puts
	.db	STOP, 13, 10, 0
	rjmp	i2c_sniff
;-------------------------------------------------------------------------
_isn1:
	sbrs	a0, USIOIF			; Data / ACK received ?
	rjmp	_isn3
	in	b0, USIDR
	dec	get_ack				; toggle get ACK / Data
	sbrs	get_ack, 0
	rjmp	_isn2
	ldi	a0, 1<<USIOIF^(0x0F & -2)	; count 2 edges
	out	USISR, a0
	mov	a0, b0
	rcall	puthex
	rjmp	i2c_sniff
_isn2:
	ldi	a0, 1<<USIOIF^(0x0F & -16)	; count 16 edges
	out	USISR, a0
	ldi	a0, NACK
	sbrs	b0, 0
	ldi	a0, ACK 
	rcall	putchar
	rjmp	i2c_sniff
;-------------------------------------------------------------------------
_isn3:
	sbrs	a0, USISIF			; START received ?
	rjmp	i2c_sniff
_isn4:
	sbic	I2C_PIN, SCL			; wait until SCL = low
	rjmp	_isn4
	ldi	a0, 1<<USIWM1^1<<USIWM0^1<<USICS1^1<<USICS0;
	out	USICR, a0			; I2C, get data on neg. edge
	ldi	a0, 1<<USISIF^(0x0F & -16)
	out	USISR, a0			; clear USISIF, count 16 edges
	clr	get_ack
	ldi	a0, START
	rcall	putchar
	rjmp	i2c_sniff
;-------------------------------------------------------------------------
