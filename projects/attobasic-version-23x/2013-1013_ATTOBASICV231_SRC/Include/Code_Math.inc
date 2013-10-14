;***************************************************************************
;* "bin2BCD16" - 16-bit Binary to BCD conversion
;*
;* This subroutine converts an 16-bit number (inbyteh:inbytel) to a 6-digit
;* BCD number saved in temporary BCD regs (U:T:H).
;* Uses: inbyteh, inbytel, temp, H, T, U, outchar, r1 and r0
;***************************************************************************
;
BIN2BCD16:
#if ( (defined(__ATmega32U4__) || defined(__AT90USB1286__) ) && USB )
	GoSub	D_USBINT			;disable USB ints
#endif
	COPY16	r1,r0,inbyteh,inbytel	;xfer number to r1:r0
	clc							;clear carry bit
	ldi		temp,16				;set bit counter
	clr		U					;clear temp BCD registers
	clr		T
	clr		H
BITLOOP16:
	rol		r0
	rol		r1
	rol		U
	rol		T
	rol		H
	dec		temp
	brne	BITADJ16
#if ( (defined(__ATmega32U4__) || defined(__AT90USB1286__) ) && USB )
	GoSub	E_USBINT			;re-enable USB ints
#endif
	ret							;we're done
BITADJ16:
	mov		inchar,U
	rcall	ADJBIT16
	mov		U,inchar
	mov		inchar,T
	rcall	ADJBIT16
	mov		T,inchar
	mov		inchar,H
	rcall	ADJBIT16
	mov		H,inchar
	rjmp	BITLOOP16

;*******************************************
;ADJBIT16: Supports the BIN2BCD16 routine
;*******************************************
ADJBIT16:
	mov		outchar,inchar
	subi	outchar, -3			;add 3
	sbrc	outchar,3
	mov		inchar,outchar
	mov		outchar,inchar
	subi	outchar,-(0x30)		;add 0x30
	sbrc	outchar,7
	mov		inchar,outchar
	ret
;
;*****************************************************************************
byte_to_asciihex:     ;convert byte in inbytel to ascii HEX in inbyteh,inbytel
	mov		inbyteh,inbytel
	swap	inbyteh				;swap nibbles
	andi	inbyteh,0x0F		;strip high nibble
	subi	inbyteh,-$30		;add $30
	cpi		inbyteh,$3A    
	brlo	PC+2 				;If less than 9 skip next instruction
	subi	inbyteh,$F9			;add 8 to ASCII (if data greater than 9)
     ; byte in inbyteh represents upper nybble that was in inbytel at start

	andi	inbytel,0x0F		;convert the lower nybble to ascii byte
	subi	inbytel,-$30		;add $30
	cpi		inbytel,$3A     
	brlo	PC+2				;If less than 9 skip next instruction
	subi	inbytel,$F9 		;add 8 to ASCII (if data greater than 9)
     ; byte in inbyteh represents upper nybble that was in inbytel at start
	ret
;
;*****************************************************************************
asciihex_to_byte:     	;convert ascii in inbyteh,inbytel to byte in inbytel
	sbrc	inbyteh,6	;convert high byte
	subi	inbyteh,$f7	;add     inbyte,temp     ;if bit 6 is set, add $09
	andi	inbyteh,$0F

	sbrc	inbytel,6	;convert low byte
	subi	inbytel,$f7	;add     inbyte,temp     ;if bit 6 is set, add $09
	andi	inbytel,$0F

	swap	inbyteh				;swap nibbles
	or		inbytel,inbyteh		;combine them
	ret
;
;******************************************************************************
binarytodecimal:	;Enter with 8 bit value in U, Exits with numerals in H,T,U
					;with Hundreds, Tens, and Units.
	clr		H
	clr		T
anotherh:
	subi 	U,100	;Find out how many hundreds in U
	brcs	hdone
	inc		H
	Goto	anotherh
hdone:
	subi	U,-100	;Subtracted one too many -add back.
anothert:
	subi	U,10
	brcs	tdone
	inc		T
	Goto	anotherT
tdone:
	subi	U,-10
	ret
;
;*****************************************************************************
decimaltobinary:	;Convert decimal number in H,T,U registers (hundreds,tens,units) to 
			;binary value in U register.
			;But first check to see if nummber exceeds 255 decimal.
	cpi		H,3
	brpl	exceeded255
	cpi		H,2
	brmi	numisok
	cpi		T,6
	brpl	exceeded255
	cpi		T,5
	brmi	numisok
	cpi		U,6
	brpl	exceeded255
numisok:
	tst		T					;Accumulate the number of tens
	breq	moreh
	ldi		temp,10
	add		U,temp				;Add  10
	dec		T	
	brne	decimaltobinary
moreh:
	tst		H					;Accumulate the number of hundreds
	brne	notdoneconv
	ret							;When hundreds = zero, return
notdoneconv:
	ldi		temp,100
	clc	
	add		U,temp				;Add 100	
	dec		H
	brcc	moreh				;If carry is not set, continue
exceeded255:
	STOREI	ERRCode,$01			; **** ecode is now a high register
	Goto	Error	
;
.if RTC
;************************************************************************
; BIN4ASC:	32-bit binary to 10 digit ASCII conversion routine.  
;	Upon entry, the 32-bit value is in registers ZH:ZL:YH:YL.
; 	Each of the 10 ASCII digits is send directly to the terminal.
; 	D9/8/7/6/5/4/3/2/1 ASCII can also be called to convert a 
;	  9/8/7/6/5/4/3/2/1 digit number to ASCII.
;	Uses: outchar, YH:L and ZH:L
;	Note: This routine is larger in size than others available but only
;	  uses 5 registers.
;************************************************************************
BIN4ASC:
D10ASC:
	ldi		outchar,0x2f		;Init counter to ASCII "0" less 1 
LOOP1000M:
	inc		outchar				;count up 1000M "counts" 
	SUBI32	ZH,ZL,YH,YL,1000000000
	brcc	LOOP1000M			;IF >=1000M THEN subtract again 
	ADDI32	ZH,ZL,YH,YL,1000000000
	GoSub	emitchar			;send the value

D9ASC:
	ldi		outchar,0x2f		;Init counter to ASCII "0" less 1 
LOOP100M:
	inc		outchar				;count up 100M "counts" 
	SUBI32	ZH,ZL,YH,YL,100000000
	brcc	LOOP100M			;IF >=100M THEN subtract again 
	ADDI32	ZH,ZL,YH,YL,100000000
	GoSub	emitchar			;send the value 

D8ASC:
	ldi		outchar,0x2f		;Init counter to ASCII "0" less 1 
LOOP10M:
	inc		outchar				;count up 10M "counts" 
	SUBI32	ZH,ZL,YH,YL,10000000
	brcc	LOOP10M				;IF >=10M THEN subtract again 
	ADDI32	ZH,ZL,YH,YL,10000000
	GoSub	emitchar			;send the value 

D7ASC:
	ldi		outchar,0x2f		;Init counter to ASCII "0" less 1 
LOOP1M:
	inc		outchar				;count up 1M "counts" 
	SUBI32	ZH,ZL,YH,YL,1000000
	brcc	LOOP1M				;IF >=1M THEN subtract again 
	ADDI32	ZH,ZL,YH,YL,1000000
	GoSub	emitchar			;send the value 

D6ASC:
	ldi		outchar,0x2f		;Init counter to ASCII "0" less 1 
LOOP100K:
	inc		outchar				;count up 100K "counts" 
	SUBI32	ZH,ZL,YH,YL,100000
	brcc	LOOP100K			;IF >=100K THEN subtract again 
	ADDI32	ZH,ZL,YH,YL,100000
	GoSub	emitchar			;send the value 

D5ASC:
	ldi		outchar,0x2f		;Init counter to ASCII "0" less 1 
LOOP10K:
	inc		outchar				;count up 10K "counts" 
	SUBI32	ZH,ZL,YH,YL,10000
	brcc	LOOP10K				;IF >=10K THEN subtract again 
	ADDI32	ZH,ZL,YH,YL,10000
	GoSub	emitchar			;send the value

D4ASC:	;(ZH:ZL == 0) so ignore ZH:ZL from here 
	ldi		outchar,0x2f		;Init counter to ASCII "0" less 1 
LOOP1K:
	inc		outchar				;count up 1k "counts" 
	SUBI16	YH,YL,1000
	brcc	LOOP1K				;IF >=1K THEN subtract again 
	ADDI16	YH,YL,1000
	GoSub	emitchar			;send the value 
.endif

D3ASC:
	ldi		outchar,0x2f		;Init counter to ASCII "0" less 1 
LOOP100:
	inc		outchar				;count up 100 "counts" 
	SUBI16	YH,YL,100
	brcc	LOOP100				;IF >=100 THEN subtract again 
	ADDI16	YH,YL,100
	GoSub	emitchar			;send the value 

D2ASC:	;(YH==0) so ignore YH from here 
	ldi		outchar,0x2f		;Init counter to ASCII "0" less 1 
LOOP10:
	inc		outchar				;count up 10 "counts" 
	subi	YL,10
	brcc	LOOP10				;IF >=10 THEN subtract again 
	subi	YL,-10
	GoSub	emitchar			;send the value 

D1ASC:
	mov		outchar,YL 
	subi	outchar,-0x30		;convert to ASCII
	GoSub	emitchar			;send the value			;send out a space 

	ret							;return to caller
;
.if DHT
;***************************************************************************
;*
;* MPY16U: 16x16 Bit Unsigned Multiplication
;*
;* This subroutine multiplies the two 16-bit register variables 
;* mp16uH:mp16uL and mc16uH:mc16uL.
;* The result is placed in m16u3:m16u2:m16u1:m16u0.
;*  
;*	mc16uH:L	=YH:YL					; multiplicand
;*	mp16uH:L	=H:T					; multiplier
;*	m16u3:0		=inbyteh:inbytel:H:T	; result 3:0
;*	mcnt16u		=temp					; loop counter
;***************************************************************************
;
MPY16U:	
	LOAD	temp,SREG			;save SREG
	push	temp
	cli							;disable global ints
	ldi		temp,16				;init loop counter
	clr		inbyteh				;clear 2 highest bytes of result
	clr		inbytel
	lsr		H					;shift LSB into SREG_C
	ror		T

MPY16U_1:	
	brcc	MPY16U_2			;if bit 0 of multiplier set
	add		inbytel,YL			;add multiplicand Low to byte 2 of res
	adc		inbyteh,YH			;add multiplicand high to byte 3 of res
MPY16U_2:	
	ror		inbyteh				;shift right result byte 3
	ror		inbytel				;rotate right result byte 2
	ror		H					;rotate result byte 1 and multiplier High
	ror		T					;rotate result byte 0 and multiplier Low
	dec		temp				;decrement loop counter
	brne	MPY16U_1			;if not done, loop more
	pop		temp				;restore SREG
	STORE	SREG,temp
	ret
;
;***************************************************************************
;*
;* DIV16U:	16/16 Bit Unsigned Division
;*
;* This subroutine divides the two 16-bit numbers 
;* "dd16uH:dd16uL" (dividend) and "dv16uH:dv16uL" (divisor). 
;* The result is placed in "dres16uH:dres16uL" and the remainder in
;* "drem16uH:drem16uL".
;*  
;* 			  H:T / inchar:outchar = H:T, rem inbyte[h:l]
;*---------
;*	dd16uH		=H			dd16uL		=T			;dividend
;*	dv16uH		=inchar		dv16uL		=outchar	;divisor
;*---------
;*	dres16uH	=H			dres16uL	=T			;result
;*	drem16uH	=inbyteh	drem16uL	=inbytel	;remainder
;*  
;*	dcnt16u		=temp							;loop counter
;*
;***************************************************************************
;
DIV16U:	
	LOAD	temp,SREG			;save SREG
	push	temp
	cli							;disable interrupts
	clr		inbytel				;clear remainder Low byte
	sub		inbyteh,inbyteh		;clear remainder High byte and carry
	ldi		temp,17				;init loop counter
DIV16U_1:	
	rol		T					;shift left dividend
	rol		H
	dec		temp				;decrement counter
	brne	DIV16U_2			;if done
	pop		temp				;restore SREG
	STORE	SREG,temp
	ret							;return
DIV16U_2:	
	rol		inbytel				;shift dividend into remainder
	rol		inbyteh
	sub		inbytel,outchar		;remainder = remainder - divisor
	sbc		inbyteh,inchar
	brcc	DIV16U_3			;if result negative
	add		inbytel,outchar		;restore remainder
	adc		inbyteh,inchar
	clc							;clear carry to be shifted into result
	rjmp	DIV16U_1			;else
DIV16U_3:	
	sec							;set carry to be shifted into result
	rjmp	DIV16U_1
;
.endif
;
