;	Serial line Rx/Tx 8 or 16 bits master/slave
;	(C) 2000,2001 DILOGIC d.o.o.
;	author:  Dejan Durdenic dilogic@zg.tel.hr
;
;		Version 1.1
;		08.02.2001 
;


;		Port D used for Serial line I/O and configuration
;
;		Pin 0 - RX/TX mode 	-> 1=TX    ,0=RX
;		Pin 1 - Master/slave	-> 1=Master,0=Slave
;		Pin 2 - RX Turbo mode   -> 1=Normal,0=Turbo mode (in normal mode 4 equal data packets are required
;			         				  for output to change)
;		Pin 3 - reserved for future use
;		Pin 4 - reserved for future use
;		Pin 5 - Hold I/O 	-> in master/slave operation connect those pins together on both
;					   master and slave devices
;		Pin 6 - Serial data I/O	-> serial data input/output
;
;
;		Port B used for parallel data I/O
;
;		on RX device, configured for open-drain outputs, default state being off
;		when there is no data stream at input, all outputs go to default off state
;



.include "1200def.inc"

.equ		TIMER_PRESCALER	=	2
.equ		TIMER_CONST   	=	(255-16)

.equ		RXTX	= 0			
.equ		MSSLV	= 1
.equ		TURBO	= 2
.equ		HOLD 	= 5
.equ		LINE	= 6

.def		temp_reg	=	r16
.def		temp_reg1	=	r17

.def		hist1	=	r20
.def		hist2	=	r21
.def		hist3	=	r22

.CSEG

			rjmp	MAIN

			.org	OVF0addr

			rjmp	TIM0_OVF	; TIMER0 irq handler

;			Timer interrupt routine - active in TX mode only
	
TIM0_OVF:	ldi		r31,TIMER_CONST
			out		TCNT0,r31
			sbrs	r30,1
			reti

			sbrs	r30,0
			rjmp	TIM0_OVF1
						
TIM0_OVF0:	nop
			cbi		DDRD,LINE
			rjmp	TIM0_EXIT

TIM0_OVF1:	sbi		DDRD,LINE

TIM0_EXIT:	cbr		r30,2
			reti

;			end of timer interrupt routine

;			Main program starts here

MAIN:		
						
			ldi		temp_reg,low(RAMEND)
			out		SPL,temp_reg			;init Stack Pointer, if there is one
			ldi		temp_reg,high(RAMEND)
			out		SPH,temp_reg

			wdr
			ldi		temp_reg,8
  			out		WDTCR,temp_reg
			ldi		temp_reg,0x1F			;port D is inputs with pull-ups on lower 5 pins
			out		PORTD,temp_reg
			sbis	PIND,RXTX				;Select RX-TX mode
			rjmp	RXMODE
			
TXMODE:		clr		temp_reg
			out		DDRB,temp_reg	
			ldi		temp_reg,0xFF	
			out		PORTB,temp_reg			; Port B is input with pull-ups
			ldi		temp_reg,TIMER_CONST
			out		TCNT0,temp_reg
			ldi		r30,3					; line will default to high state
			ldi		temp_reg,TIMER_PRESCALER
			out		TCCR0,temp_reg
			ldi		temp_reg,2
			out		TIMSK,temp_reg			; enable timer0 interrupt
			sei
			cbi		PORTD,LINE				; Disable line output (hi-z)
			sbis	PIND,MSSLV
			rjmp	TXSLAVE

TXMASTER:	sbi		DDRD,HOLD				; PD5 is output for slave sync
			ldi		r30,2					; line will default to 1
			sbi		PORTD,HOLD				; set HOLD to one
			
TXMASTER0:	wdr
			rcall	SEND_START
			in 		r0,PINB
			rcall	SEND_BYTE
TXMASTER1:	sbrc	R30,1					
			rjmp	TXMASTER1
			ldi		r30,3					; Set line to one...
TXMASTER2:	sbrc	r30,1 
			rjmp	TXMASTER2				; wait last bit to go out...
			cbi		PORTD,HOLD				; signal slave
			rcall	WAIT_IDLE				; idle waits 10 bit times
			sbi		PORTD,HOLD
			rjmp	TXMASTER0
			
TXSLAVE:	cbi		DDRD,HOLD				; Port D pin5 is input
			sbi		DDRD,4
			cbi		DDRD,LINE				; disable line output

TXSLAVE0:	wdr
			cli
			in		R0,PINB					; read new data
			ldi		r30,0
			rcall	SEND_BYTES
TXSLAVE2: 	sbrc	R30,1					; wait last bit to go out...
			rjmp	TXSLAVE2
			ldi		r30,3
TXSLAVE3: 	sbrc	R30,1					; wait last bit to go out...
			rjmp	TXSLAVE3
			cbi		DDRD,LINE				; set line to 1
			cli
TXSLAVE4:	sbis	PIND,HOLD				; wait master to finish
			rjmp	TXSLAVE4
			rjmp	TXSLAVE0
			
;			Receiver part of the code
;			Wait valid start bit, measure time
;			and sample at middle of the bits

RXMODE:		
			clr		temp_reg
			out		PORTB,temp_reg
			ldi		temp_reg,0xFF			; port B is push-pull output
			out		DDRB,temp_reg			; port B is output - open collector
			ldi		temp_reg,0x5F			; port D is inputs with pull-ups on lower 5 pins and data line
			out		PORTD,temp_reg
RX_INIT:
			clr		hist1
			clr		hist2
			clr		hist3
						
RXLOOP:		wdr	
			clr		r8						; r8 holds start bit duration
WAIT_START1:
			sbic	PIND,LINE				; wait start bit
			rjmp	WAIT_START1
WAIT_FIRST:	
			nop
			nop
			nop
			nop								; add some dead time here
			inc		r8
			sbis	PIND,LINE
			rjmp	WAIT_FIRST
			
			rcall	READ_BYTE
			brcs	RXLOOP					; if bad timing, ignore received data
			
			sbis	PIND,MSSLV				; if slave, receive another byte
			rjmp	RXSLAVE

			rcall	RXIDLE					; wait 10 bit times			
			rjmp	RXEND

RXSLAVE:	rcall	READ_BYTE	
			brcs	RXLOOP					; if bad timing, ignore received data
			
RXEND:		sbis	PIND,TURBO
			rjmp	RXEND0					; if turbo, no checks for 4 equal samples
			cp		r0,hist3				; all four samples must be the same...
			brne	RXEND1
			cp		r0,hist2
			brne	RXEND1
			cp		r0,hist1
			brne	RXEND1
			
RXEND0:		com		r0						; complement output (drivers invert !)
			out		PORTB,r0
RXEND1:		mov		hist1,hist2				; update history buffer
			mov		hist2,hist3
			mov		hist3,r0		
			rjmp	RXLOOP

			
; 			TX mode subroutines

SEND_BYTE:	ldi		temp_reg1,8

SEND_BYTE0:	sbrc	r0,0
			rjmp	SEND_BYTE2
SEND_BYTE1:	rcall	SEND_ZERO
			rjmp	SEND_BYTE3
SEND_BYTE2:	rcall	SEND_ONE

SEND_BYTE3:	ror		r0
			dec		temp_reg1
			brne	SEND_BYTE0
			ret
			
SEND_ONE:	sbrc	R30,1		; wait last transition to go out
			rjmp	SEND_ONE						
			ldi		R30,3
			sei
SEND_ONE1:	sbrc	R30,1
			rjmp	SEND_ONE1
			ldi		R30,3
SEND_ONE2:	sbrc	R30,1
			rjmp	SEND_ONE2
			ldi		R30,2
			ret

SEND_ZERO:	sbrc	R30,1		; wait last transition to go out
			rjmp	SEND_ZERO						
			ldi		R30,3
			sei
SEND_ZERO1:	sbrc	R30,1
			rjmp	SEND_ZERO1
			ldi		R30,2
SEND_ZERO2:	sbrc	R30,1
			rjmp	SEND_ZERO2
			ldi		R30,2
			ret

SEND_BYTES:	ldi		temp_reg1,8
SEND_BYTES0:sbrc	r0,0
			rjmp	SEND_BYTES2
SEND_BYTES1:rcall	SEND_ZEROS
			rjmp	SEND_BYTES3
SEND_BYTES2:rcall	SEND_ONES
SEND_BYTES3:ror		r0
			dec		temp_reg1
			brne	SEND_BYTES0
			ret
			
SEND_ONES:	sbrc	R30,1		; wait last transition to go out
			rjmp	SEND_ONES						
			ldi		R30,3
SEND_ONES0:	sbic	PIND,HOLD
			rjmp	SEND_ONES0
			sei
SEND_ONES1:	sbrc	R30,1
			rjmp	SEND_ONES1
			ldi		R30,3
SEND_ONES2:	sbrc	R30,1
			rjmp	SEND_ONES2
			ldi		R30,2
			ret

SEND_ZEROS:	sbrc	R30,1		; wait last transition to go out
			rjmp	SEND_ZEROS
			ldi		R30,3
SEND_ZEROS0:sbic	PIND,HOLD
			rjmp	SEND_ZEROS0
			sei
SEND_ZEROS1:sbrc	R30,1
			rjmp	SEND_ZEROS1
			ldi		R30,2
SEND_ZEROS2:sbrc	R30,1
			rjmp	SEND_ZEROS2
			ldi		R30,2
			ret

WAIT_IDLE:	ldi		temp_reg,30
WAIT_IDLE0:	sbrc	r30,1
			rjmp	WAIT_IDLE0
           	ldi		r30,7
			dec		temp_reg
			brne	WAIT_IDLE0	
			ret

SEND_START:	ldi		temp_reg,3
;   		sbi		DDRD,LINE			
SEND_START0:sbrc	r30,1
			rjmp	SEND_START0
			ldi		r30,2
			dec		temp_reg
			brne	SEND_START0
			ret

;			RX mode subroutines

MEASURE_TIMES:
			mov		r6,r8
			mov		r7,r8
WAIT_LOW:	nop
			nop
			nop
			nop
			dec		r6
			breq	MEAS_ERR
			sbic	PIND,LINE
			rjmp	WAIT_LOW
WAIT_HIGH:	nop
			nop
			nop
			nop
			dec		r7
			breq	MEAS_ERR
			sbis	PIND,LINE
			rjmp	WAIT_HIGH
			clc
			ret
			
MEAS_ERR:	sec
			ret			

READ_BYTE:	ldi		temp_reg,8				; 8 bits per frame
			clr		r0
RXFIRST:	lsl		r0
			rcall	MEASURE_TIMES
			brcs	READ_ERROR				; if error - end immediatelly
			cp		r7,r6
			brlo	RXZERO
			inc		r0						; decoded bit is one !
RXZERO:		dec		temp_reg
			brne	RXFIRST
			clc
READ_ERROR:	ret

RXIDLE:		ldi		temp_reg,10 			; wait 10 bit times (skip slave byte if master)
RXIDLE1:	mov		r6,r8
RXIDLE2:	nop
			nop
			nop
			nop
			dec		r6
			brne	RXIDLE2
			dec		temp_reg
			brne	RXIDLE1
			ret