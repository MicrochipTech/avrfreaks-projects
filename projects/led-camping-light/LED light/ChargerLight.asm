.include "tn15def.inc"

;----------------------------------------
; register definitions
.def	status = R2

.def	temp = R16			; general temporary register
.def	tempa = R17			; used in TIM0_OVF
.def	tempb = R18			; second temporary register
.def	brightness = R19	; 1 to 9
.def	cycle_cnt = R20		; 1 to 3
.def	del_cnt = R21		; dec'd in TIM0_OVF


.equ	CYCLE = 3		; 1 to 3
.equ	MAX = 10		; brightness
.equ	FREQ = 0xAD		; 3.3333mSec
.equ	SWITCH = 3		; PB3



.cseg
;----------------------------------------
;Reset handler
	.org $0000
	rjmp	STARTUP
;----------------------------------------
; INT0
	reti
;----------------------------------------
; Pin Change
	reti
;----------------------------------------
; Timer1 Compare
	reti
;----------------------------------------
;Timer 1 Overflow
	reti
;----------------------------------------
;Timer0 overflow handler
	rjmp	TIM0_OVF
;----------------------------------------
; Eprom ready
	reti
;----------------------------------------
;Analogue Compare
	reti
;----------------------------------------
;ADC Conversion
	reti


;-------------------------------------------
; Timer0 Overflow Interrupt - 3.3333 mSec
; dec cycle_cnt and del_cnt till zero	
;-------------------------------------------
;
TIM0_OVF:
	in		status,SREG
	ldi		tempa,FREQ
	out		TCNT0,tempa
; if del_cnt non-zero, dec till 0
	tst		del_cnt
	breq	TIM_O1
	dec		del_cnt
; cycle_cnt goes from 1 to 3
TIM_O1:
	inc		cycle_cnt
	cpi		cycle_cnt,CYCLE+1
	brne	TIM_02
	ldi		cycle_cnt,1
TIM_02:
	out		SREG,status
	reti
		

;----------------------------------------
; LED lookup table
;----------------------------------------
LOOKUP:
	.DB	0x01,0x00,0x00,0x01,0x01,0x00,0x01,0x01,0x01,0x03,0x01,0x01,0x03,0x03,0x01,0x03,0x03,0x03,0x07,0x03,0x03,0x07,0x07,0x03,0x07,0x07,0x07


;----------------------------------------
; This is where we setup the ports and
; peripherals
;----------------------------------------
;
STARTUP:	
; PB0,PB1,PB2 are outputs
	ldi		temp,0x07
	out		DDRB,temp
;	Pull-ups enabled PB3 for SW1
	ldi		temp,0x08
	out		PORTB,temp
; set up internal oscillator
	ldi		temp,0x60
	out		OSCCAL,temp
; CLK/64 3.333 mSec clock
	ldi		temp,0x03
	out		TCCR0,temp
	ldi		temp,0
	out		TCCR1,temp
; Timer 0 Overflow interrupts 3.333 mSec
	ldi		temp,(1<<TOIE0)
	out		TIMSK,temp
; Load timer 0
	ldi		temp,FREQ
	out		TCNT0,temp
; set up variables
	ldi		cycle_cnt,1
	ldi		brightness,1
	sei


;----------------------------------------
; This is where the program loops
;----------------------------------------
;
A_LOOP:
	tst		del_cnt				; are we delaying?
	brne	A_LOOP10			; yes!
	sbic	PINB,SWITCH			; no! is button pressed?
	rjmp	A_LOOP10			; no!
	inc		brightness			; yes! so inc brightness
	ldi		del_cnt,100			; 333.3 mSec delay
	cpi		brightness,MAX		; wrap around?
	brne	A_LOOP10			; no!
	ldi		brightness,1		; yes!
; We will set the pointer to the start of the appropriate '1 of 3' 
; byte patterns in the table LOOKUP. Then we will add the cycle_cnt
; number to the pointer to point to the appropriate display pattern.
A_LOOP10:	
	ldi 	ZH, high(LOOKUP<<1)	; Initialize Z pointer
	ldi 	ZL, low(LOOKUP<<1)
	mov		temp,brightness
A_LOOP12:
	dec		temp				; allow for 1 origin, brightness
	breq	A_LOOP15			; if eq 0 then at start
	ldi		tempb,3				; else add 3
	add		ZL,tempb
	rjmp	A_LOOP12			; back for more
A_LOOP15:
	add		temp,cycle_cnt		; offset to cycle_cnt byte
	dec		temp				; allow for 1 origin
	add		ZL,temp				; now pointing at LED pattern
	lpm							; R0 now has LED pattern
	ldi		temp,0x08
	or		R0,temp				; make sure SWITCH pullup is high
	out		PORTB,R0			; O/P LED state
	rjmp	A_LOOP




