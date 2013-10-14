;*************************************************************
;converts and prints the binary values held in inbyteh:inbytel 
;	as ASCII characters.
;*************************************************************
PRT4BIN_ASC:
	ldi		temp,2				;init loop counter
	push	inbytel				;save a copy of low byte
	push	inbyteh				;save a copy of high byte
	mov		inbytel,inbyteh		;copy high byte to inbytel
	sec							;set carry bit for high nibble
	GoSub	BIN2ASC				;convert the high nibble
	GoSub	emitchar			;send inbytel register 

	pop		inbytel				;restore high byte to inbytel
	clc							;clear carry bit for low nibble
	GoSub	BIN2ASC				;convert the low nibble
	GoSub	emitchar			;send inbytel register 
	dec		temp				;decrement loop counter
	brne	PRT4BIN_ASC1		;skip if not zero
	ret							;return to caller

PRT4BIN_ASC1:
	pop		inbyte

PRT2BIN_ASC:
	push	Wreg1				;save a copy of low byte
	sec							;set carry bit for high nibble
	GoSub	BIN2ASC				;convert the high nibble
	GoSub	TX_CHAR				;send it out
	pop		Wreg1				;restore YL to Wreg1
	clc							;clear carry bit for low nibble
	GoSub	BIN2ASC				;convert the high nibble
	GoSub	TX_CHAR				;send it out
	ret							;return






	push	inbytel					;save a copy of low byte
;	push	inbyteh					;save a copy of high byte
	mov		inbytel,inbyteh			;copy high byte to inbytel
	GoSub	byte_to_asciihex		;convert high byte to ASCII
	push	inbytel					;save low-byte from destruction
	mov		inbytel,inbyteh			;get high digit
	GoSub	emitchar				;print it
	pop		inbytel					;restore low digit
	GoSub	emitchar				;print it

	pop		inbytel					;restore free bytes
	GoSub	byte_to_asciihex		;convert low byte to ASCII
	push	inbytel					;save low-byte from destruction
	mov		inbytel,inbyteh			;get high digit
	GoSub	emitchar				;print it
	pop		inbytel					;restore low digit
	GoSub	emitchar				;print it

	ret								;return
;


;**********************************************************************
; BIN2TOASC5: Converts a 16-bit-binary to a 5 digit ASCII-coded decimal
; In: 16-bit-binary in rBin1H:L, Z points to the highest of 5 ASCII 
;	digits, where the result goes to
; Out: Z points to the beginning of the ASCII string, lea-ding zeros
;	are filled with blanks
; Used registers: 	rBin1H:L (content is not changed),
;   				rBin2H:L (content is changed), rmp
; Called subroutines: Bin2ToBcd5
;
BIN2TOASC5:
	rcall 	BIN2TOASC55 ; convert binary to BCD
	ldi 	rmp,4 ; Counter is 4 leading digits
	mov 	rBin2L,rmp
BIN2TOASC5a:
	ld 		rmp,z ; read a BCD digit
	tst 	rmp ; check if leading zero
	brne 	BIN2TOASC5b ; No, found digit >0
	ldi 	rmp,' ' ; overwrite with blank
	st 		z+,rmp ; store and set to next position
	dec 	rBin2L ; decrement counter
	brne 	BIN2TOASC5a ; further leading blanks
	ld 		rmp,z ; Read the last BCD
BIN2TOASC5b:
	inc 	rBin2L ; one more char
BIN2TOASC5c:
	subi 	rmp,-'0' ; Add ASCII-0
	st 		z+,rmp ; store and inc pointer
	ld 		rmp,z ; read next char
	dec 	rBin2L ; more chars?
	brne 	BIN2TOASC5c ; yes, go on
	sbiw 	ZL,5 ; Pointer to beginning of the BCD
	ret ; done
;



;*************************************************************
;BIN2ASC: converts a binary byte to ASCII char
;	inbytel contains the value
;	  If carry set, processes high nibble
;	Returns with ascii digit in inbytel
;*************************************************************
BIN2ASC:
	brcc    BIN2ASC1
	swap    inbytel
BIN2ASC1:
	andi    Wreg1,0xF			;strip of high nibble
	cpi		Wreg1,10
	brlo    BIN2ASC2
	subi    Wreg1,-(65-10)		;add ascii 'A'
	ret
BIN2ASC2:
    subi    Wreg1,-48			;add ascii '0'
	ret
;


	subi	inbytel,pmembot			;free bytes in page
	sbci	inbyteh,pmembotpage		;free pages
	GoSub	PRT4BIN2ASC				;print the free bytes
	



	push	inbytel					;save free byte count
	ldi		temp,255				;get mulitply value
	mul		inbyteh,temp			;multiply
	adc		r0,temp					;add additonal w/ carry
	brcc	sizecommand1			;skip if no carry
	inc		r1						;carry over to r1
sizecommand1:	;r1:r0 contains the number of pages free
	pop		temp					;restore free byte count to temp reg
	adc		r0,temp					;add additonal w/ carry
	brcc	sizecommand2			;skip if no carry
	inc		r1						;carry over to r1
sizecommand2:	;r1:r0 contains the number of bytes free
	nop


	push	inbyteh					;convertion trashes inbyteh
	GoSub	byte_to_asciihex		;convert free bytes to ascii
			;we have ASCII "free pages" 
	push	inbytel					;save register
	mov		outchar,inbyteh			;move to register
	GoSub	emitchar				;send ASCII
	pop		inbytel					;retreive next digit
	GoSub	emitchar				;send ASCII

	pop		inbyteh					;retrieve free bytes



;***********************************************************************
;convert "digits" stored in 'Wreg1' to two ASCII BCD digits and send 
;  them out the serial link
;***********************************************************************
;
PRT_2BCD:
	push	Wreg1				;save a copy
	andi	Wreg1,0xf0			;strip off low-nibble
	swap	Wreg1				;put into low-nibble
	subi	Wreg1,-(0x30)		;make it ASCII
	rcall	TX_CHAR				;send 10's digit
	pop		Wreg1				;retrieve the copy
	andi	Wreg1,0x0f			;strip off low-nibble
	subi	Wreg1,-(0x30)		;make it ASCII
	rcall	TX_CHAR				;send 1's digit
	ret
;

;****************************************************************************
;CRC16: calculates the CRC-16 of a series of data ( x^16 + x^12 + x^5 + 1 ).
;	- Call with Y pointing to start of data in RAM.
;	- Call with byte count in r1:r0
;	- Returns with CRC-16 in H:T
;	Destroys temp, H, T, inbyteh, inbytel and Y pointers.
;****************************************************************************
CRC16:
   ; Load the last CRC value, but swapped (low <-> high)
	ldi		H,HIGH(CRC16Poly)	;seed CRC16 accumulator
	ldi		T,LOW(CRC16Poly)

CRC16a:
	ld		U,-Z				;load data and decrement pointer

	eor		T,U					;First XOR

	mov		inbytel,T			;Second XOR
	swap	inbytel				;These instructions are faster than 
	andi	inbytel,0x0F		; executing 4 times "lsr 4".
	eor 	T,inbytel

	mov		inbytel,T			;Third XOR
	swap	inbytel				;These instructions are faster than 
	andi	inbytel,0xF0		; executing 4 times "lsr 4".
	eor		H,inbytel

	mov		inbytel,T			;Fourth XOR
	swap	inbytel
	mov		inbyteh,inbytel
	andi	inbytel,0xF0
	andi	inbyteh,0x0F
	lsl		inbytel
	rol		inbyteh
	eor		H,inbyteh
	eor		T,inbytel

	dec		r0						;decrement byte counter
	brne	CRC16a					;loop till byte count reached

	ret


