;*******************************************************************************
; Title:	Philips RCMM IR reciever
; Author: 	Anders Runeson, arune@users.sf.net
; Version:	1.0.0
; Date:		2003-06-29
;
; Target:	AT90S2313 (May work on others)
;
; DESCRIPTION
; This application is a simple Philips RCMM IR reciever.
; The timing is adapted for 4 MHz crystal
;
; Usage: Call RecievIR from external interrupt or after a high (0v) signal is 
; detected on PIND,IR (already done in this example)
; RecievIR sets T-flag if error occured
;*******************************************************************************

		
.include "2313def.inc"

;Hardware:	(IR-module, something like http://www.everlight.com/pdf/IRM-8601V.PDF )
.equ	IR				=PD3

.equ	Uart_TxD			=PD1	;o_Data to UART is PD1
.equ	Uart_RxD			=PD0	;i_Data from UART is PD0

;Define regs used
.def	ir_command			=R11
.def	ir_command2			=R12
.def	ir_command3			=R13


.def	temp				=R17

.def	temp2				=R22
.def	parsertemp			=R23

.def	counter				=R31


;***** Other defs
;limits:
.equ	lim0	=3
.equ	lim1	=6
.equ	lim2	=8
.equ	lim3	=10
.equ	lim4	=13



;#############################################################
;#              RESET                                        #
;#############################################################
.ORG	0x00
		rjmp	RESET				;Reset Handler
.ORG	INT1addr
		rjmp	EXT_INT1			;IRQ0 Handler
	

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
		
		;cbi	DDRD,Uart_RxD			;UART RxD is input


		;INIT PORT B
		sbi	DDRB,Uart_TxD			;UART TxD is output
	
		;INIT INTERRUPT 
		ldi	temp,0b00001000			;Setup Interrupt Sense Control for 
		out	MCUCR,temp			;trigging on falling edge
		ldi	temp,(1<<INT1)			;Enable "External Interrupt1 Request" 
		out	GIMSK,temp

		;INIT UART
		ldi	temp,0b00011000
		out	UCR,temp
	
		ldi	temp,12			;25=9600, 12=19200, 
		out	UBRR,temp
	

		sei
	
	

;#############################################################
;#              MAIN                                         #
;#############################################################
;***** Program Execution Starts Here **************************************

Start:		
	
	rjmp	Start



;****** External Interrupt Routine 0 *****************************************
EXT_INT1:
		;Get IR data
		rcall	RecievIR
		brtc	No_error
		
		
		;IR recieve routine got an error, what to do:
		;...
		;...
		rjmp	out_of_int1


		No_error:
		;data recieved is in register
		;ir_command, ir_command2, ir_command3
		
		;...
		;...
		
		mov	temp,ir_command3
		rcall	uart_send
		mov	temp,ir_command2
		rcall	uart_send
		mov	temp,ir_command
		rcall	uart_send
		
		
	


	out_of_int1:
		ldi	temp,(1<<INTF1)		;clear interrupt flag
		out	GIFR,temp

	reti
	




;******* RecievIR ***************************************************************
; RCMM seems to use pulsewidth modulated signals.
; Thirteen pulses and four different pulselength is used
; The first pulse is discarded in my code, think it is startbit anyway.
; Next four make first byte (called ir_command3), think this byte 
; differs between remotes.
; Next eight pulses make second (ir_command2) and third (ir_command) byte
; they differs between buttons.
; Note: this is all hand-hacked using an oscilloscope, a Nokia-remote and a brain.

RecievIR:

		clt				;Clear T flag, if set: indikates error to calling function
		clr	ir_command

		;clear timer ovfl flag
		ldi	temp,(1<<TOV1)
		out	TIFR,temp
		
		
		ldi	counter,13
		
		next_bit:
		rcall	init_n_reset_timer
		
		wait_for_high:
		;check timer ovfl flag
		in	temp,TIFR
		sbrc	temp,TOV1
		rjmp	outRecievIR_Error
		;check for change on pin
		sbis	PIND,IR	
		rjmp	wait_for_high	
		
		rcall	init_n_reset_timer
		rcall	delay_short

		wait_for_low:
		;check timer ovfl flag
		in	temp,TIFR
		sbrc	temp,TOV1
		rjmp	outRecievIR_Error
		;check for change on pin
		sbic	PIND,IR	
		rjmp	wait_for_low	
		
		
		in	temp,TCNT1l	;dummyread
		in	temp,TCNT1h
		;test the time from T1h in intervals
		cpi	temp,lim0
		brlo	outRecievIR_Error	;if lower than lim0: error
		cpi	temp,lim1
		brlo	interval_done
		
		next_comp0:
		cpi	temp,lim2
		brsh	next_comp1
		sbr	parsertemp,0b00000001
		rjmp	interval_done
		
		next_comp1:
		cpi	temp,lim3
		brsh	next_comp2
		sbr	parsertemp,0b00000010
		rjmp	interval_done
		
		next_comp2:
		cpi	temp,lim4
		brsh	outRecievIR_Error	;if higher than lim0: error
		sbr	parsertemp,0b00000011
		
	interval_done:
		lsl	parsertemp
		rol	ir_command2
		rol	ir_command3
		lsl	parsertemp
		rol	ir_command2
		rol	ir_command3

		
		dec	counter
		brne	next_bit
		
		;this delays is to be sure to be outside the signal
		rcall	delay_mid	
		rjmp	outRecievIR
		
	outRecievIR_Error:
		set		;indicate error to calling function
		
	outRecievIR:

		mov	ir_command,parsertemp
ret
	

	
	init_n_reset_timer:
		;INIT TIMER1 - 16bit
		ldi	temp,0b00000000
		out	TCCR1B,temp
		clr	temp
		out 	TCNT1h,temp
		out 	TCNT1l,temp
		ldi	temp,0b00000001
		out	TCCR1B,temp
	ret



	delay_short:
		ldi	temp,15
		delay_short0:
		dec	temp
		brne	delay_short0
	ret


	delay_mid:
		ldi	temp,5
		delay_mid0:
		ldi	temp2,255
		delay_mid1:
		dec	temp2
		brne	delay_mid1
		dec	temp
		brne	delay_mid0
	ret



uart_send:
	sbi	USR,TXC			;Clear TXC bit (by writing 1 to it)

	waitloop:
	sbis	USR,UDRE
	rjmp	waitloop

	out	UDR,temp
		
ret
