;Infrared Entry Alert Door Chime
;sender 
;Monday, February 07, 2011, 8:25:10 AM
;runs @ 4MHz clock
;WORKING! sends 0x55 continuously
;
; +5V <---[A Led K]---[220 Ohm]---> Pb.3 for 2313.
; using TIMER1, no interrupts are used
; The resistor must be connected to the OC1(A) pin , in this case PB.3
;
;program fuses for attiny2313:
;FuseLow: 0xFD
;FuseHigh: 0xDF
;FuseExt: 0xFF
;
.nolist
.include "tn2313def.inc"
.list
;
.def	dcount  =r5
.def	dcount2 =r6
.def	dcount3 =r7
;
.def	temp = r16
.def	temp1 = r17
.def	bitcnt = r18
.def	command = r19
;
.equ	cmd	= 0x55
.cseg
.org	0
;
	rjmp   Start	  	;	
;------------------------------------------------------
Start:
	ldi	temp, LOW(ramend)	; 
 	out	SPL, temp     	; 

	sbi	DDRB, 3		; PB3 = output
;------------------------------------------------------
MainLoop:
	ldi	command, cmd
	rcall	Transmit_IR	;

 	rcall	Wait100mS
	rjmp	MainLoop	; 
;------------------------------------------------------
Transmit_IR:
	ldi	temp, 0x40	; 
	out	TCCR1A, temp ;
 
	ldi	temp, 0b00001001	; 0x09 
	out	TCCR1B, temp  	; clear Timer1 on compare match/prescale CK

	ldi	temp, 0x00	; 
	ldi	temp1, 0x00	; 
	out	TCNT1H, temp1 ; 
	out	TCNT1L, temp ; 

	ldi	temp, 0x37	; 
	ldi	temp1, 0x00	; 
	out	OCR1AH, temp1 ; 
	out	OCR1AL, temp ; 

	rcall	SendStartBit ;send first start bit
	rcall	SendStartBit ;send second start bit

	ldi	bitcnt, 8	;8 bits to send
;------------ 
;|||||_____ = Logic 0
;
;_____||||| = Logic 1
;
;------------
TX_Loop:
	lsl	command		; 
	brsh  TX_Zero		; send "0" 

	ldi	temp, 0b11000000	; 0xC0 send "1"
	out	TCCR1A, temp  	; OC1 line high (off)
	rcall	Del889uS	 	; 

	ldi	temp, 0b01000000	; 0x40 
	out	TCCR1A, temp  	; Toggle the OC1 Output Line on compare
	rjmp	TX_Lp1	  	; 

TX_Zero:
	ldi	temp, 0x40		; send "0" 
	out	TCCR1A, temp  	; 
	rcall	Del889uS	 	; 

	ldi	temp, 0xC0		; 
	out	TCCR1A, temp  	; 

TX_Lp1:
	rcall  Del889uS	 	; 

	dec	bitcnt		; 
	brne  TX_Loop	  	; 

	ldi	temp, 0xC0		; 
	out	TCCR1A, temp  	; 
	ret		   		; 
;------------------------------------------------------
;send "0"
SendStartBit:
	ldi	temp, 0b11000000	; 0xC0
	out	TCCR1A, temp  	; OC1 line high (off)
	rcall	Del889uS	 	; 

	ldi	temp, 0b01000000	; 0x40
	out	TCCR1A, temp  	; Toggle the OC1 Output Line on compare
	rcall	Del889uS	 	; 

	ldi	temp, 0b11000000	; 0xC0
	out	TCCR1A, temp  	; OC1 line high (off)
	ret
;------------------------------------------------------
.include "Delay.inc"	
;------------------------------------------------------
.exit