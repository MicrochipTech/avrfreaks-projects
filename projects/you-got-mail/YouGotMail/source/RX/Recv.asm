;***********************************************************************
;receiver code for RLP434A module
;
.NOLIST
.include "2313def.inc"		;
.LIST
;
.equ	BAUD		=103 		;2400 bps at 4.00 MHz.

.equ	first_byte	=$69
.equ	second_byte	=$42

.equ	LED	=5			;LED = PD5
.equ	DING	=6			;Ding = PD6

.def	B1		=R1		;first byte
.def	B2		=R2		;second byte
.def	command	=R3		;command byte 
.def	LRC		=R4		;all three previous bytes exored
.def	dcount	=R5
.def	dcount2	=R6
.def	dcount3	=R7

.def	temp		=R16		;temporary storage register.
.def	temp2		=R17		;temporary storage register.
.def	txbyte 	=R18		;transmit data
.def	rxbyte 	=R19		;Received data
.def	bitcnt	=R20		;bit counter
.def	count		=R21
;=======================================================================
.cseg
.org	0	
	rjmp	reset			;

reset:
	ldi	temp,low(RAMEND)	;init Stack Pointer
	out	SPL,temp          	
	
	ldi	temp,0b00111110 	;port D setup
	out	PORTD,temp

	ldi	temp,0b01111110	;
	out	DDRD,temp

; Setup UART
	ldi	temp,BAUD
	out	UBRR,temp		;Set baud rate

	ldi	temp,0b00011000
	out	UCR,temp		;Enable UART Tx and Rx

	ldi	temp,first_byte
	mov	B1,temp

	ldi	temp,second_byte
	mov	B2,temp
;==============================================================
;transmitter just sent 69 42 01 2A
;you can change the first three bytes to anything
;they must match transmitter
;
start:
	clr	LRC
	rcall	receive			;receive data from uart

	cp	rxbyte,B1			;compare received byte with 0x69
	brne	start				;if its different, restart

;we are here because first received byte compares to 0x69
	rcall	receive			;get second byte
	cp 	rxbyte,B2			;should compare with B2
	brne	start				;if it's different, restart 
;====================================================================
;we are here because first and second bytes compared successfully
next:
	rcall	receive			;get third byte (command byte)
	mov	command,rxbyte 		;save this byte to command

	rcall	receive     		;receive the 4th byte
	
	eor	LRC,B1			;00 ^ 69 = 69
	eor	LRC,command   		;69 ^ 01 = 68
	eor	LRC,B2			;68 ^ 42 = 2A
	cp	rxbyte,LRC			;compare checksum with 4th received byte
	brne	start				;if checksum is different, go to receive new bytes
;-------------------------------------------------------------------
	cbi	PORTD,LED		;LED ON
Test:	
	ldi	count,5		;ring buzzer 5 times

WaitLp:
	sbi	PORTD,Ding		;buzzer on
	rcall	wait			;wait 1 second
	cbi	PORTD,Ding		;buzzer off
	rcall	wait			;wait 1 second

	dec	count
	brne	WaitLp
stop:
	rjmp	stop			;wait until hardware reset
;****************************************************************
;1 sec delay @ 4MHz
wait:	
	ldi	temp,20
	mov	dcount2,temp
	ldi	temp,255
	mov	dcount3,temp
	ldi	temp,255
	mov	dcount,temp
wait1:
	dec	dcount
	brne	wait1
	dec	dcount3
	brne	wait1
	dec	dcount2
	brne	wait1
	ret
;****************************************************************
;Receiving routine RxD
;****************************************************************
receive:
	sbis	USR,RXC		;wait here forever until
	rjmp	receive		;we get a byte
	in	Rxbyte,UDR
	ret
;******************************************************************
.exit
