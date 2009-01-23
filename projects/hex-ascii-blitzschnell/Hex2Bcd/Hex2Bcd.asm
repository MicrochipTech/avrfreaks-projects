; Hexadecimal byte to 3 BCD Characters with leading zeros suppression

; 24 Mhz AVR168

.def	MulLo		= r0
.def	MulHi		= r1
.def	Ones		= r2

.def	Temp		= r16
.def	HexByt		= r17
.def	Huns		= r18
.def	Tens		= r19

;------	
HexBcd:	
	ldi	Huns,	$30		; Preload Hundreds Character to Ascii "0"

	cpi	HexByt,200		; If Hexadecimal Value less than 200 then skip
	brcs	pc+4
	subi	HexByt,200		; Set Hundreds Character to Ascii "2"
	ldi	Huns,	$32
	rjmp	pc+5

	cpi	HexByt,100		; If Hexadecimal Value less than 100 then skip
	brcs	pc+3
	subi	HexByt,100		; Set Hundreds Character to Ascii "1"
	ldi	Huns,	$31

	ldi 	Temp,	26		; Adjust Tens Character
    	mul 	HexByt,Temp
	mov	Tens,	MulHi

	ldi 	Temp,	10		; Adjust Ones Character
    	mul 	MulHi,	Temp
	sub	HexByt,MulLo
	ori	HexByt,$30
	mov	Ones,	HexByt		

	cpi	Huns,	$30		; Skip if Hundreds Character = Ascii "0"
	brne	pc+2
	ldi	Huns,	$20		; Set Hundreds Character to Ascii "Space"			

	cpi	Tens,	0		; Skip if Tens Character = 0		
	brne	pc+5

	cpi	Huns,	$20		; Skip if Huns Character = Ascii "Space"	
	brne	pc+3
	ldi	Tens,	$20		; Set Tens Character to Ascii "Space"	
	rjmp	pc+2

	ori	Tens,	$30		; Adjust Huns Ascii Character	
	


	

