;*********************************************
;TLP434A Transmitter code
;this one with no interrupts, uses software uart to transmit
;acts on NO magnet switch connecting ground
;
.NOLIST
.include "2313def.inc"		;
.LIST
;
.equ	TxD		= 0		;TX pin is PB0 (output)
.equ	first_byte	=$69
.equ	second_byte	=$42

.def	B1		=R1		;first byte in packet
.def	B2		=R2		;second byte in packet
.def	command	=R3		;command byte
.def	LRC		=R4		;previous 3 bytes xored together

.def	dcount	=R5		;delay counter
.def	dcount2	=R6		;delay counter
.def	dcount3	=R7		;delay counter

.def	temp		=R16		;temporary storage register
.def	temp2		=R17		;temporary storage register
.def	Txbyte	=R18		;Data to be transmitted

.def	bitcnt	=R22		;bit counter
.def	count		=R23
;***********************************************************************
.cseg
.org	0
	rjmp 	reset			;Reset handler
;---------------------------------------------------
reset:
	ldi	temp,low(RAMEND)	;init Stack Pointer
	out	SPL,temp          	
;---------------------------------------------------
	ldi	temp,0b11111111 	;PB0 output
	out	DDRB,temp

	ldi	temp,0b11111111
	out	portB,temp      	;enable Data-pin TxD (PB0)
;---------------------------------------------------
	ldi	temp,first_byte
	mov	B1,temp

	ldi	temp,second_byte
	mov	B2,temp

	ldi	temp,0x01		;
	mov	command,temp

;wake up the receiver
Preamble:
	ldi	txbyte,0x55
	rcall	transmit

	ldi	txbyte,0xAA
	rcall	transmit

	ldi	txbyte,0x55
	rcall	transmit
;---------------------------------------------------------
;packet will be 69 42 CMD LRC
;you can change the first three bytes to whatever you want
;but they must match receiver!
;
Enable_RF:
	ldi	count,4
load:
	clr	LRC
	mov	txbyte,B1			
	rcall	transmit		;1) send 0x69

	mov	txbyte,B2	
	rcall	transmit		;2) send 0x42

	mov	txbyte,command		
	rcall	transmit		;3) send 0x01 (command)

	eor	LRC,B1

	eor	LRC,B2		;4) 69 ^ 42 = 2B
					
	eor	LRC,command		;5) 2B ^ 01 = 2A

	mov	txbyte,LRC		;	
	rcall	transmit		;send out LRC byte

;so we just sent 69 42 01 2A

	dec	count			;send the packet 4 times
	brne	load	
stop:
	rjmp	stop			;once door is closed, power will be disconnected
;*********************************************************************
;*			Transmit routine
;*********************************************************************
transmit:
	ldi	bitcnt,10		;1+8+sb (sb is # of stop bits)
	com	Txbyte		;Invert everything
	sec				;Start bit

putchar0:
	brcc	putchar1		;If carry set
	cbi	PORTB,TxD		;send a '0'
	rjmp	putchar2		;else	

putchar1:
	sbi	PORTB,TxD		;send a '1'
	nop

putchar2:
	rcall	UART_delay		;One bit delay
	rcall	UART_delay

	lsr	Txbyte		;Get next bit
	dec	bitcnt		;If not all bit sent
	brne	putchar0		;send next
	ret				;return
;*********************************************************************
;*
;* "UART_delay"
;*
;* This delay subroutine generates the required delay between the bits when
;* transmitting and receiving bytes. The total execution time is set by the
;* constant "b":
;*
;*	3xb + 7 cycles (including rcall and ret)
;*********************************************************************
.equ	b=135	;2400 bps @ 4 MHz crystal, +5V
.equ	c=2

UART_delay:
	ldi	temp2,c
UART_delay2:
	ldi	temp,b
UART_delay1:
	dec	temp
	brne	UART_delay1
	dec	temp2
	brne	UART_delay2
	ret
;***************************************************
.exit
