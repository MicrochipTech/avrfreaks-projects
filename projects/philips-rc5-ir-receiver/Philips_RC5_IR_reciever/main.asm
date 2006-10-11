;*******************************************************************************
; Title:	Philips RC5 IR reciever
; Author: 	Anders Runeson, arune@users.sf.net
; Version:	1.0.0
; Date:		2003-06-29
;
; Target:	AT90Sxxxx (All AVR Devices)
;
; DESCRIPTION
; This application is a simple Philips RC5 IR reciever.
; It removes the toggle bit that the standard supports.
; The timing is adapted for 4 MHz crystal
;
; Usage: Call RecievIR from external interrupt or after a high (0v) signal is 
; detected on PIND,IR
; RecievIR sets T-flag if error occured (if maybe not philips remote)
;*******************************************************************************

		
.include "2313def.inc"

;Hardware:	(IR-module, something like http://www.everlight.com/pdf/IRM-8601V.PDF )
.equ	IR				=PD2

;Define regs used
.def	ir_system			=R10
.def	ir_command			=R11
.def	bitcnt				=R31
.def	recieve				=R29
.def	ir_byte_cnt			=R21


.def	temp				=R17

.def	subsubTemp			=R22
.def	subsubTemp2			=R23


;***** Other 
.equ	c1				=85
		

;#############################################################
;#              RESET                                        #
;#############################################################
.ORG	0x00
		rjmp	RESET				;Reset Handler
.ORG	INT0addr
		rjmp	EXT_INT0			;IRQ0 Handler
	

RESET:
		cli					;Disable interrupts
;#############################################################
;#              INIT                                         #
;#############################################################

		;INIT STACKPOINTER
		ldi 	temp,low(RAMEND)
		out 	SPL,temp

		;INIT PORT D
		cbi	DDRD,IR				;IR-mod is input
		sbi	PORTD,IR			;use pullup


		;INIT PORT B

	
		;INIT INTERRUPT (Used for example)
		ldi		temp,0b00000010		;Setup Interrupt Sense Control for trigging on falling edge
		out		MCUCR,temp
		ldi		temp,(1<<INT0)		;Enable "External Interrupt0 Request" 
		out		GIMSK,temp

	

		sei
	
	

;#############################################################
;#              MAIN                                         #
;#############################################################
;***** Program Execution Starts Here **************************************

Start:		
	


	rjmp	Start






;****** External Interrupt Routine 0 *****************************************
EXT_INT0:

	;Get IR data
		rcall	RecievIR
		brtc	No_error
		
		;IR recieve routine got an error, what to do:
		;...
		;...
		rjmp	out_of_int0
		
		No_error:

		;data recieved is in register
		;ir_system, tells us what type of remote it is
		;ir_command, tells us what button the user pressed
		
		;...
		;...

	out_of_int0:
	reti
	




;******* RecievIR ***************************************************************
RecievIR:
		clt				;Clear T flag, if set: indikates error to calling function
		clr	ir_byte_cnt

		rcall	IR_bit_delay		;1/4 IR period-time
		
		ldi	bitcnt,8
		
	NextIRByte:	
		clr	recieve
	  	IRSub1:    
		lsl	recieve			;Shift left
		sbis	PIND,IR			;If IR input is low(1!)
		sbr	recieve,1		;   store 1 in bit 0 of recieve
		
		rcall	IR_bit_delay		;1/4 IR period-time
		rcall	IR_bit_delay		;1/4 IR period-time

		dec	bitcnt
		breq	no_more_bits
		
	w8_flank:
		
		sbic	PIND,IR			;If IR input is high(0!)
		rjmp	w8_end_bit1
		
		w8_end_bit0:
		;wait for end of bit (low=5v)
		sbis	PIND,IR
		rjmp	w8_end_bit0
		rjmp	go_delay

		w8_end_bit1:
		;wait for end of bit (high=0v)
		sbic	PIND,IR
		rjmp	w8_end_bit1
		
		go_delay:
		rcall	IR_bit_delay		;1/4 IR period-time
		rjmp	IRSub1
		
	no_more_bits:
	
		cpi	ir_byte_cnt,0
		brne	not_address

		cpi	recieve,0		;If all bits in byte is 0, then quit IR Reciev	
		brne	no_problem		;this applies only to system byte, command byte can be 0
		
		sbrs	recieve,7
		rjmp	stbit_error
		
		sbrc	recieve,6
		rjmp	no_problem
	
	stbit_error:
		set				;set t flag to indicate that error occured
		rjmp	IR_Reciev_Done
		
	no_problem:
		cbr	recieve,0b11100000
		
		mov	ir_system,recieve

		ldi	bitcnt,6
		inc 	ir_byte_cnt
		clr	recieve
		rjmp	w8_flank
		
		not_address:
		mov	ir_command,recieve
		
	  	IR_Reciev_Done:
	ret
	
	
;Delay in between bits (1/4 ir period time, full period time: 1778us)

IR_bit_delay:
		ldi	subsubTemp,c1		;timer_short=c1
IR_bit_delay0:
		ldi	subsubTemp2,5
IR_bit_delay1:

		dec	subsubTemp2
		brne	IR_bit_delay1
		dec	subsubTemp
		brne	IR_bit_delay0
	
	ret



