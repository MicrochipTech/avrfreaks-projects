
.cseg


;****************************************************************
; DiSPLayRom - Send a null terminated message to the output stream
; This version works with
; micros that don't support enhanced LPM
; instructions.
; Uses R0 and R16
;****************************************************************
sendstring:
	pop	ZH
	pop	ZL		;move Stack Pointer to Z-register
	push	temp1
	push	r0

	lsl	ZL		;shift Z-register over for
	rol	ZH		;LPM operations
DR10:	lpm	 		;byte character from rom
	adiw	ZL,1		;inc Z-register
	mov	temp1,r0
	cpi	temp1,0		;test for end of string
	breq	Rone0		;rjmp when end of string
	rcall	stream_out	;diSPLay data
	rjmp	DR10

Rone0:	lsr	ZH		;restore the Stack by pointing
	ror	ZL		;Z just past the rom-based string
	pop	r0
	pop	temp1
	push	ZL		;then push it on the Stack so
	push	ZH		;the return operation places it
	ret			;in the Program Counter




; *****************************************************************************
; DESCR: sends the low-nibble to the o/p stream (in Ascii)
; ENTRY: data in temp1
; EXIT: data sent
; CHANGES: temp1
; USES: -
; *****************************************************************************
h_a:	push	ZL
	push	ZH
	ldz	rom,table1
	andi	temp1,0b00001111
	add	ZL,temp1
	brcc	J2
	inc	ZH
J2:	lpm	temp1,z
	pop	ZH
	pop	ZL
	ret

table1:	.db	'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'





; *****************************************************************************
; DESCR: o/p's temp1 as ascii decimal to the o/p stream
; ENTRY: data in temp1
; EXIT: data sent
; CHANGES: none
; USES: h_a, stream_out
; *****************************************************************************
decout:
	push	loopcounter	; temp storage
	push	temp3		; temp storage
	push	temp1
	push	temp2

	mov	temp2,temp1
	clr	loopcounter
	clr	temp3		;clear result (hundreds)

bBCD8_10:
	subi	temp2,100	;input = input - 10
	brcs	bBCD8_20	;abort if carry set
	inc	temp3		;inc MSD
	rjmp	bBCD8_10	;loop again

bBCD8_20:			; hundreds now in temp3
	subi	temp2,-100	;compensate extra subtraction

bBCD8_30:
	subi	temp2,10
	brcs	bBCD8_40
	inc	loopcounter	; tens in loopcounter
	rjmp	bBCD8_30

bBCD8_40:
	subi	temp2,-10
	mov	temp1,temp3	; output hundreds
	rcall	h_a
	rcall	stream_out

Tns:	mov	temp1,loopcounter	; output tens
	rcall	h_a
	rcall	stream_out
unts:	mov	temp1,temp2	; output units
	rcall	h_a
	rcall	stream_out

	pop	temp2
	pop	temp1
	pop	temp3
	pop	loopcounter
	ret




.dseg
Ppointer:	.byte	2	; mem print pointer
Pcount:		.byte	1	; chars printed to mem
.cseg
; *****************************************************************************
; DESCR: 'Print' a char to ram
; ENTRY: char in temp1, memory pointer at Ppointer
; EXIT: -
; CHANGES: ram, Ppointer, Pcount
; USES: -
; *****************************************************************************
Pmem:	push	xl
	push	xh
	push	temp1

	lds	temp1,Pcount
	cpi	temp1,maxdata-2		; check if TX buffer full
	breq	nost			; if full, just exit
	inc	temp1
	sts	Pcount,temp1

	loadX	Ppointer
	pop	temp1
	st	x+,temp1
	storeX	Ppointer
	rjmp	nopop
nost:	pop	temp1
nopop:	pop	xh
	pop	xl
	ret



; *****************************************************************************
; DESCR: sends a char to the choosen o/p stream. (serial or memory buffer)
; ENTRY: [stream_destination] loaded with stream value. 0=null, 1=serial, 2=memory
; EXIT: -
; CHANGES: 
; USES: 
; *****************************************************************************
stream_out:
	push	temp2
	lds	temp2,stream_destination
	tst	temp2
	brne	somewhere
	rjmp	str_ex		; null so just exit
somewhere:
;	jeq	temp2,serial_stream,serout	; no serial out used
	jeq	temp2,memory_stream,memout
	rjmp	str_ex

;serout:	rcall	sersend0
;	rjmp	str_ex

memout:	rcall	Pmem
str_ex:	pop	temp2
	ret	





; *****************************************************************************
; DESCR: 1ms software delay
; ENTRY: 
; EXIT: 1ms later
; CHANGES: acch:accl (r25:r24)
; USES: 
; *****************************************************************************
msone:	cydelay	8000		; 1ms at 8mhz
	ret


; *****************************************************************************
; DESCR: variable ms delay
; ENTRY: ms count in temp1
; EXIT: temp1-ms later
; CHANGES: temp1
; USES: msone
; *****************************************************************************
delsoft:
	push	accL
	push	accH
wait:	rcall	msone
	dec	temp1
	brne	wait
	pop	accH
	pop	accL
	ret




;***** software multiply 8-bit * 8-bit from atmel app note AVR200b.asm

mpy8u:	push	temp1
	push	temp2
	push	temp3

	clr	temp3		;clear result High byte
	lsr	temp2		;shift multiplier
	
	brcc	noad80		;if carry set
	add	temp3,temp1	;add multiplicand to result High byte
noad80:	ror	temp3		;shift right result High byte 
	ror	temp2		;rotate right result L byte and multiplier

	brcc	noad81		;if carry set
	add	temp3,temp1	;add multiplicand to result High byte
noad81:	ror	temp3		;shift right result High byte 
	ror	temp2		;rotate right result L byte and multiplier

	brcc	noad82		;if carry set
	add	temp3,temp1	;add multiplicand to result High byte
noad82:	ror	temp3		;shift right result High byte 
	ror	temp2		;rotate right result L byte and multiplier

	brcc	noad83		;if carry set
	add	temp3,temp1	; add multiplicand to result High byte
noad83:	ror	temp3		;shift right result High byte 
	ror	temp2		;rotate right result L byte and multiplier

	brcc	noad84		;if carry set
	add	temp3,temp1	;add multiplicand to result High byte
noad84:	ror	temp3		;shift right result High byte 
	ror	temp2		;rotate right result L byte and multiplier

	brcc	noad85		;if carry set
	add	temp3,temp1	;add multiplicand to result High byte
noad85:	ror	temp3		;shift right result High byte 
	ror	temp2		;rotate right result L byte and multiplier

	brcc	noad86		;if carry set
	add	temp3,temp1	;add multiplicand to result High byte
noad86:	ror	temp3		;shift right result High byte 
	ror	temp2		;rotate right result L byte and multiplier

	brcc	noad87		;if carry set
	add	temp3,temp1	;add multiplicand to result High byte
noad87:	ror	temp3		;shift right result High byte 
	ror	temp2		;rotate right result L byte and multiplier

	mov	r0,temp2	; answer copied to r0/r1 as per hardware mult
	mov	r1,temp3

	pop	temp3
	pop	temp2
	pop	temp1
	ret











