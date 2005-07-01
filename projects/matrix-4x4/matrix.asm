

;					MBTRIX 4 X 4
;
;
;
;the file recognises the key of the matrix keyboBrd 
;
;
;and stores the result at the r21. 
;
;This program isn't for the STK500 as the I/O is inverted
;
;


.include "2313def.inc"


.def key=r0	
.def temp=r16


	.org 20


	.cseg
		ldi temp,$0F		;PORTB IIIIOOOO
       	out ddrB,temp	;the 4 MSB bits are congigured 
		ldi temp,$FF		;as inputs and the 4 LSB bits 
		out ddrD,temp	;are configured as outputs.
		ldi temp,$64
		out spl,temp	;initialize stack pointer
		clr r10
		clr r11
		ldi temp,$01
		sts $A0,temp
		ldi temp,$04
		sts $A1,temp	
		ldi temp,$07
		sts $A2,temp
		ldi temp,$0E
		sts $A3,temp

		.cseg
init:	nop
		.include "outmatrix.asm"
		
		ldi r21,$FF
main:	ldi r17,$01
		out portB,r17
		nop
		in r20,pinB	
		cpi r20,$81		;check if the D key is pressed
		breq E			;
		cpi r20,$41		;check if the 0 key is pressed
		breq zero			;	
		cpi r20,$21		;check if the E key is pressed
		breq F			;
		cpi r20,$11		;
		breq D			;check if the F key is pressed
		rjmp main1

D:		ldi r21,$0D
		mov key,r21
		rjmp prog
	

zero:	ldi r21,$00
		mov key,r21
		rjmp prog
	

E:		ldi r21,$0E
		mov key,r21
		rjmp prog
	

F:		ldi r21,$0F
		mov key,r21
		rjmp prog
	

	.cseg
main1:	ldi r17,$02
		out portB,r17
		nop	;
		in r20,pinB		;
		cpi r20,$82		;check if the 7 key is pressed
		breq seven		;
		cpi r20,$42		;check if the 8 key is pressed
		breq eight		;
		cpi r20,$22		;
		breq nine		;check if the 9 key is pressed
		cpi r20,$12		;
		breq C			;check if the C key is pressed
		rjmp main2	

seven:	ldi r21,$07		;  -----------------------------
		mov key,r21
		rjmp prog
						; |	----------------------------						    |
						; | 4 x 4 mBtrix keyboBrd       | 
eight:	ldi r21,$08		; |	---------------------------                            |
		mov key,r21
		rjmp prog
						; | | 1 |  | 2 |  | 3 |  | A |  | 
						; |                             |
nine:	ldi r21,$09		; | | 4 |  | 5 |  | 6 |  | B |  |
		mov key,r21
		rjmp prog
						; |                             |
						; | | 7 |  | 8 |  | 9 |  | C |  |
C:		ldi r21,$0C		; |                             |
		mov key,r21
		rjmp prog		; | | E |  | 0 |  | F |  | D |  |
			
						;  -----------------------------

	.cseg
main2:	ldi r17,$04
		out portB,r17
		nop
		in r20,pinB
		cpi r20,$84		;check if the 4 key is pressed
		breq four		;
		cpi r20,$44 	;check if the 5 key is pressed
		breq five		;
		cpi r20,$24		;
		breq six		;check if the 6 key is pressed
		cpi r20,$14		;
		breq B			;check if the B key is pressed
		rjmp main3
	 
four:	ldi r21,$04
		mov key,r21
		rjmp prog
	
five:	ldi r21,$05
		mov key,r21
		rjmp prog
		

six:	ldi r21,$06
		mov key,r21
		rjmp prog
	

B:		ldi r21,$0B
		mov key,r21
		rjmp prog
	


	.cseg
main3:	ldi r17,$08
		out portB,r17
		nop
		in r20,pinB
		cpi r20,$88		;check if the 1 key is pressed
		breq one
		cpi r20,$48		;check if the 2 key is pressed
		breq two		
		cpi r20,$28		;check if the 3 key is pressed
		breq three
		cpi r20,$18		;check if the B key is pressed
		breq A


  		rjmp prog

one:	ldi r21,$01
		mov key,r21
		rjmp prog
		

two:	ldi r21,$02
		mov key,r21
		rjmp prog
	
three:	ldi r21,$03
		mov key,r21
		rjmp prog
	

A:		ldi r21,$0A
		mov key,r21
		rjmp prog


prog:     NOP

		rjmp init






