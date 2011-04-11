;Monday, March 15, 2010, 10:43:09 AM
;RF transmitter encoder
;
;In order to use this code, you must have an external 3.6864MHz clock
;(timing is tight)
;it WILL NOT work with any other clock unless timing is changed
;code works with AT90S2313
;
;* must have good antenna! I used an 8" piece of copper wire
;* stripped from an RG6 coax cable (center conductor)
;
;transmitter is activated by a push switch on PD2
;when PD2 is low, the transmitter sends the packet
;
.NOLIST
.include "2313def.inc"
.LIST
;
.def	count		= r4
.def	dcount	= r5
.def	dcount2	= r6
.def	dcount3	= r7

.def	temp	 =	r16
.def	bitcnt = 	r17	;bit counter
.def	Txbyte = 	r18	;needed for software transmit routine

.equ	address 	= 0x0A
.equ  command 	= 0x69
;
;PortD
.equ	TxD	= 1		;TX pin is PD1 (output)
;--------------------------------------------------------
.cseg
.org 0
	rjmp	Start
;--------------------------------------------------------
.org 0x0B
Start:
	ldi  temp,low(RAMEND)	;Initialize stackpointer
	out	SPL,temp		

	ldi	temp,0b11111111
	out	PORTD,temp
	ldi	temp,0b11111010	;PD0,PD2 = inputs
	out	DDRD,temp
;--------------------------------------------------------
Main:
	sbic	PIND,2		;wait for button to be pushed
	rjmp	Main

	ldi	temp,50
	mov	dcount,temp
	rcall	Delay			;50mS delay for debouncing
MainA:
	sbis	PIND,2		;wait for button to be released
	rjmp	MainA
;---------------------------------------------------------------
	ldi	TxByte,0x55
	rcall	PreambleTX

	ldi	TxByte,0xAA
	rcall	PreambleTX

	ldi	TxByte,0xFF
	rcall	PreambleTX

	ldi	temp,4		;send the packet 4 times
	mov	count,temp
	rcall	startbit

TXLoop:
	ldi	TxByte,address
	rcall	TX_Byte
	ldi	TxByte,command
	rcall	TX_Byte

	dec	count
	brne	TXLoop

	rjmp	main
;***************************************************************
.include "softuart.inc"
.include "delay.inc"
;***************************************************************
.exit