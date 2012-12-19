


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
; DESCR: software delay of 1ms @8mhz
; ENTRY: ms count to delay in temp1
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
; temp1-ms delay
sd1:	push	r24
	push	r25
paws:	cydelay	8000
	dec	temp1
	brne	paws
	pop	r25
	pop	r24
	ret



; *****************************************************************************
; DESCR: software delay of 100ms @8mhz
; ENTRY: temp1=number of 100ms steps to delay
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
sd2:	push	r24
	push	r25
	push	temp2

sd3:	ldi	temp2,20	; inner loop of 20 for 0.1 sec
sd4:	cydelay	40000	; 5ms
	dec	temp2
	brne	sd4		; falls through after 0.1 sec

	dec	temp1
	brne	sd3		; repeat 0.1 sec delay temp1 times

	pop	temp2
	pop	r25
	pop	r24
	ret


; **********************************************************************************************
; taken from AVR freaks. random num gen
;
;peret - Oct 12, 2007 - 02:12 AM
;Post subject: Here's a good, quick RNG. 
;The period (2^31-1) is prime so it's not easy to short-circuit it. 
;Half the time it takes 6 cycles and the other half it takes 15, 
;for an average of 10.5, if it's register-based as shown. 
;You can make a quicker, shorter one by using a 15 or 16 bit algorithm instead of 31. 
;How to choose the polynomial? It's a black art, but CRC polynomials generally 
;work well. 
;In fact this kind of RNG is mathematically the same as taking the CRC of an 
;infinite string of 1's. If you already have a CRC routine in your code you get an RNG for free.
;
;Code:
; 31-bit random number
; primitive polynomial order 31
; (1)0100000 10101010 10010101 10110101 (20 AA 95 B5)
; shift before XOR
rand_31:
	lsl	r4		; shift first
	rol	r5
	rol	r6
	rol	r7
	sbrs	r7,7		; test MSB	
	ret			; clear, no XOR

	push	temp2
	ldi	temp2,0xB5	; first poly byte
	eor	r4,temp2	; XOR it in
	ldi	temp2,0x95	; second byte of poly
	eor	r5,temp2
	ldi	temp2,0xAA	; same routine
	eor	r6,temp2
	ldi	temp2,0x20
	eor	r7,temp2
	pop	temp2
	ret 
; *************************************************************************************************

; gets a number between 0..9
getone:	rcall	rand_31
	mov	temp1,r6
	andi	temp1,0b00001111
	cpi	temp1,10
	brsh	getone		; just loop till in range 0..9
	ret




; *****************************************************************************
; DESCR: sends a char to the choosen o/p stream. (serial or memory buffer)
; ENTRY: [stream_destination] loaded with stream value. 
; 0=null, 1=serial, 2=memory, 3=lcd
; EXIT: -
; CHANGES: 
; USES: 
; *****************************************************************************

; bits of this are removed as not present in this skeleton project
stream_out:
	push	temp2
	lds	temp2,stream_destination
	tst	temp2
	brne	somewhere
	rjmp	str_ex			; null so just exit
somewhere:
;	jeq	temp2,serial_stream,serout
;	jeq	temp2,memory_stream,memout
	jeq	temp2,lcd_stream,lcdout
	rjmp	str_ex			; unknown so just exit

;serout:	call	sersend		; char to serial port
;	rjmp	str_ex

memout:
;	call	Pmem		; 'print' the char to ram
str_ex:	pop	temp2
	ret

lcdout:	call	lcd_char
	rjmp	str_ex




