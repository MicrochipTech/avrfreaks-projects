TX_Byte:
	ldi	bitcnt,8	;8 bits to send

TX_Byte_Lp:	
	rol	Txbyte	;rotate left through carry
	brcc	TX_Zero
TX_One:
	cbi	PORTD,TxD	;send '001'
	rcall	BitDelay	;0
	rcall	BitDelay	;0
	sbi	PORTD,TxD	;
	rcall	BitDelay	;1

	rjmp	TX_Byte_Next

TX_Zero:
	cbi	PORTD,TxD	;send '011'
	rcall	BitDelay	;0
	sbi	PORTD,TxD	;
	rcall	BitDelay	;1
	rcall	BitDelay	;1

TX_Byte_Next:	
	dec	bitcnt	; advance bit counter
	brne	TX_Byte_Lp	; loop
	ret
;*********************************************************************
BitDelay:			;300uS @ 3.6864 MHz
	ldi	temp, $03
	mov	dcount, temp
BitDelay0:
	ldi	temp, $79
	mov	dcount2,temp
BitDelay1:
	dec  dcount2
	brne BitDelay1
	dec  dcount
	brne BitDelay0
	ret
;*********************************************************************
startbit:
	cbi	PORTD,TxD	;send '001'
	rcall	BitDelay
	rcall	BitDelay
	sbi	PORTD,TxD	;
	rcall	BitDelay
	rcall	BitDelay
	ret
;*********************************************************************
PreambleTX:
	ldi	bitcnt,8	;8 bits to send

Preamble_Lp:	
	rol	Txbyte	;rotate left through carry
	brcc	Send_Zero

Send_One:
	sbi	PORTD,TxD	;
	rjmp	Preamble_Next

Send_Zero:
	cbi	PORTD,TxD	;send '011'
	rjmp	Preamble_Next ; to keep timing exact

Preamble_Next:	
	rcall	BitDelay	;
	dec	bitcnt	; advance bit counter
	brne	Preamble_Lp	; loop
	ret
;*********************************************************************