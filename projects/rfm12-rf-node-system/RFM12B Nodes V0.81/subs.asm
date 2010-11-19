
.cseg


;****************************************************************
; DiSPLayRom - Send a null terminated message to the output stream
; This version works with
; micros that don't support enhanced LPM
; instructions.
; Uses Z,R0,temp1
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
	breq	Rone0		;jmp when end of string
	rcall	stream_out	;diSPLay data
	rjmp	DR10

Rone0:	lsr	ZH		;restore the Stack by pointing
	ror	ZL		;Z just past the rom-based string
	pop	r0
	pop	temp1
	push	ZL		;then push it on the Stack so
	push	ZH		;the return operation places it
	ret			;in the Program Counter



; hex to ascii by lookup table
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
; DESCR: sends a char to the o/p stream as two ascii hex chars
; ENTRY: data in temp1
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
hexout:
	push	temp1
	push	temp1
	swap	temp1
	rcall	h_a
	rcall	stream_out
	pop	temp1
	rcall	h_a
	rcall	stream_out
	pop	temp1
	ret


; *****************************************************************************
; DESCR: sends a char to the output stream as ascii binary (8 chars)
; ENTRY: data in temp1
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
binout:
	push	temp1
	push	temp2
	ldi	temp2,8

do_:	rol	temp1
	push	temp1
	brcc	do0
	ldi	temp1,'1'
	rjmp	doS
do0:	ldi	temp1,'0'
doS:	call	stream_out
	pop	temp1
	dec	temp2
	brne	do_

	pop	temp2
	pop	temp1
	ret


; *****************************************************************************
; DESCR: sends a char to the output stream as ascii decimal
; ENTRY: data in temp1
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
decout:
	push	loopcounter	; temp storage
	push	proc_count	; temp storage
	push	temp1
	push	temp2

	mov	temp2,temp1
	clr	loopcounter
	clr	proc_count		;clear result MSD

bBCD8_10:
	subi	temp2,100		;input = input - 10
	brcs	bBCD8_20		;abort if carry set
	inc	proc_count		;inc MSD
	rjmp	bBCD8_10		;loop again

bBCD8_20:
	subi	temp2,-100	;compensate extra subtraction

bBCD8_30:
	subi	temp2,10
	brcs	bBCD8_40
	inc	loopcounter
	rjmp	bBCD8_30

bBCD8_40:
	subi	temp2,-10

	mov	temp1,proc_count
	jeq	temp1,0,Tns1	; blank leading zero
	call	h_a
	call	stream_out
	rjmp	Tns
Tns1:
;	jeq	loopcounter,0,unts

Tns:	mov	temp1,loopcounter
	call	h_a
	call	stream_out
unts:	mov	temp1,temp2
	call	h_a
	call	stream_out

	pop	temp2
	pop	temp1
	pop	proc_count
	pop	loopcounter
	ret


;------------------------------------------------------------------------------
.dseg
result16:	.byte	5
.cseg

;
; Convert the word in accH:accL to an ASCII string in ram
; accH,accL :16 bit Input value (msb:lsb)
;
; Ascii output stored at [result16] in ram. Stored in 5 ascii
; chars, msb to lsb in increasing memory.
;
word2ascii:
	push	accL			; save source number
	push	accH
	push	r0
	push	r1

	ldi	temp1,HIGH(10000)	; Set the decimal divider value to 10,000
	mov	r1,temp1
	ldi	temp1,LOW(10000)
	mov	r0,temp1
	rcall	fpconv10d		; get ASCII digit by repeated subtraction

	sts	result16,temp1

	ldi	temp1,HIGH(1000)	; Set the decimal divider value to 1,000
	mov	r1,temp1
	ldi	temp1,LOW(1000)
	mov	r0,temp1
	rcall	fpconv10d		; get ASCII digit by repeated subtraction

	sts	result16+1,temp1

	clr	r1			; Set the decimal divider value to 100
	ldi	temp1,100
	mov	r0,temp1
	rcall	fpconv10d		; get the next ASCII digit

	sts	result16+2,temp1

	ldi	temp1,10		; Set the decimal divider value to 10
	mov	r0,temp1
	rcall	fpconv10d		; get the next ASCII digit

	sts	result16+3,temp1

	ldi	temp1,'0'		; convert the rest to an ASCII char
	add	temp1,accL

	sts	result16+4,temp1

	pop	r1
	pop	r0
	pop	accH			; restore source number
	pop	accL
	ret
;
; Convert binary word in accH:accL to a decimal digit by substracting
; the decimal divider value in r1:r0 (1000, 100, 10)
;
fpconv10d:
	ldi	temp1,'0'	; start with decimal value 0
fpconv10d1:
	cp	accL,r0	; Compare word with decimal divider value
	cpc	accH,r1
	brcc	fpconv10d2	; Carry clear, subtract divider value
	ret			; done subtraction
fpconv10d2:
	sub	accL,r0	; subtract divider value
	sbc	accH,r1
	inc	temp1		; up one digit
	rjmp	fpconv10d1	; once again



; *****************************************************************************
; DESCR: software delay of 50ms @8mhz
; ENTRY: 
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
; 50ms delay
wait:	ldi	loopcounter,10
paws:	cydelay	40000		; 5ms
	dec	loopcounter
	brne	paws
	ret








; *****************************************************************************
; DESCR: sends a cr & lf to the output stream
; ENTRY: -
; EXIT: two chars sent
; CHANGES: -
; USES: stream_out
; *****************************************************************************
newln:	push	temp1
	ldi	temp1,0x0d
	rcall	stream_out
	ldi	temp1,0x0a
	rcall	stream_out
	pop	temp1
	ret



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
	cpi	temp1,maxdata-1		; check if full
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
	jeq	temp2,serial_stream,serout
	jeq	temp2,memory_stream,memout
	rjmp	str_ex		; unknown so just exit

serout:	call	sersend0
	rjmp	str_ex

memout:	call	Pmem
str_ex:	pop	temp2
	ret	






