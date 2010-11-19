


; *****************************************************************************
; DESCR: converts a ascii number to a 8-bit value
; ENTRY: asciiZ string at Ploc. Decimal, hex or binary
; EXIT: C=value in temp1. NC=bad number
; CHANGES: temp1, X
; USES: 
; *****************************************************************************
read_number:
	push	loopcounter
	push	temp2
	push	proc_count
	clr	loopcounter
	clr	proc_count			; temp sum
	loadX	Ploc
	rcall	getlen				; how long is the string

	loadX	Ploc
	jeq	loopcounter,10,read_binary	; 8 bits +0b
	jeq	loopcounter,4,read_hex		; 2 hex digits + 0x
	cpi	temp1,4				; at most, 3 digits
	brlo	read_decimal
; a bad numnber format if here
bad_number_exit:
	pop	proc_count
	pop	temp2
	pop	loopcounter
	clc
	ret

good_number_exit:
	pop	proc_count
	pop	temp2
	pop	loopcounter
	sec
	ret




; *****************************************************************************
; DESCR: converts ascii decimal to 8-bit value
; ENTRY: string at X
; EXIT: C=value in temp1. NC=fail (bad ascii decinal)
; CHANGES: 
; USES: 
; *****************************************************************************
read_decimal:
	jeq	loopcounter,1,doU	; just units
	jeq	loopcounter,2,doT	; just tens & units
; so should be hundreded + tens + units
	ld	temp1,x+		; get hundreds
	rcall	test_number
	brcc	badD
	cpi	temp1,3
	brsh	badD
	ldi	temp2,100
	mul	temp1,temp2
	add	proc_count,r0
doT:	ld	temp1,x+		; get ascii tens
	rcall	test_number
	brcc	badD
	ldi	temp2,10
	mul	temp1,temp2
	add	proc_count,r0
doU:	ld	temp1,x			; get ascii units
	rcall	test_number
	brcc	badD
	add	proc_count,temp1
	brcs	badD			; if overflow, bad number	
	mov	temp1,proc_count
	rjmp	good_number_exit
badD:	rjmp	bad_number_exit



; *****************************************************************************
; DESCR: converts ascii-hex to 8-bit value
; ENTRY: string at X. 
; EXIT: C=value in temp1, NC=fail (bad ascii-hex)
; CHANGES: 
; USES: 
; *****************************************************************************
read_hex:
	ld	temp1,x+
	jne	temp1,'0',bad_number_exit
	ld	temp1,x+
	jne	temp1,'x',bad_number_exit
; hex digits always in the form 0x??
	ld	temp1,x+
	rcall	test_hex
	brcc	badD
	mov	temp2,temp1
	swap	temp2			; nibble to high position
	ld	temp1,x+
	rcall	test_hex
	brcc	badD
	or	temp1,temp2		; merge nibbles
	jmp	good_number_exit	




; *****************************************************************************
; DESCR: converts ascii-binary string to a 8-bit value
; ENTRY: asciiZ string at X of type 0b????????
; EXIT: if OK, value in temp1
; CHANGES: temp1
; USES: -
; *****************************************************************************
read_binary:
	ld	temp1,x+		; 1st get and check preceeding bytes
	jne	temp1,'0',bad_number_exit
	ld	temp1,x+
	jne	temp1,'b',bad_number_exit

; next should be the 8 bits of '1' or '0'
	ldi	loopcounter,8		; 8 bits (ascii bytes) to check
	ldi	temp2,0b10000000	; starting 'value' for adding loop

Bloop:	ld	temp1,x+		; get string char
	rcall	test_number		; returns actual value if good number
	brcs	B5			; if not a number then exit
	rjmp	bad_number_exit
B5:	cpi	temp1,2			; check if >=2
	brlo	B6			; if 2..9 then bad number
	rjmp	bad_number_exit
B6:	tst	temp1			; test for zero
	breq	B4			; if=0, dont add
	add	proc_count,temp2	; is a '1' so add in its value to total
B4:	lsr	temp2			; shift 'value' bit to next position
	dec	loopcounter		; all bits done?
	brne	Bloop			; if no, loop to get next
; all 8 bits done so good exit and value in temp1
	mov	temp1,proc_count
	rjmp	good_number_exit



; *****************************************************************************
; DESCR: checks if a char is a valid ascii hex char (lowercase)
; ENTRY: char in temp1
; EXIT: C=converted digit in temp1. NC=bad digit
; CHANGES: temp1
; USES: test_number, test_Ahex
; *****************************************************************************
test_hex:
	rcall	test_number
	brcs	goodhex
	rcall	test_Ahex
	brcs	goodhex
; so is not in the hex range [0..f]
	clc
	ret
goodhex:
	sec
	ret

; *****************************************************************************
; DESCR: checks if an ascii char is a number (0...9)
; ENTRY: char in temp1
; EXIT: C=number, NC=not a number. converted value in temp1
; CHANGES: C
; USES: none
; *****************************************************************************
test_number:
	cpi	temp1,'9'+1	; test if > ascii 9
	brsh	nonum
	cpi	temp1,'0'	; test if < ascii 0
	brlo	nonum
; if a valid number, set C
	clc
	sbci	temp1,0x30	; convert to base value
	sec
	ret
; if not then clear C
nonum:
	clc
	ret


; *****************************************************************************
; DESCR: checks if an ascii char is a hex allowable alpha (a...f)
; ENTRY: char in temp1
; EXIT: C=is a hex value (Converted value in temp1), NC=not a hex value
; CHANGES: C
; USES: none
; *****************************************************************************
test_Ahex:
	cpi	temp1,'f'+1	; test if > ascii f
	brsh	nonum
	cpi	temp1,'a'	; test if < ascii a
	brlo	nonumA
; it is a valid digit
	clc
	sbci	temp1,87	; converts ascii to raw digit
	sec
	ret
; if not then clear C
nonumA:
	clc
	ret



; *****************************************************************************
; DESCR: displays a text string for the over-air data rate
; ENTRY: temp1=rate number, Z loaded with start of rate table
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
show_text:
	clc
	rol	temp1
	clr	temp2

	add	zl,temp1
	adc	zh,temp2
	lpm	r0,z+
	push	r0
	lpm	r0,z
	mov	zh,r0
	pop	zl

	ijmp


ratetext:
	.dw	ir0, ir1, ir2, ir3, ir4, ir5, ir6, ir7
	.dw	ir8, ir9, ir10, ir11, ir12, ir13, ir14, ir15

ir0:	stringout "0.607"
	rjmp	tail
ir1:	stringout "1.232"
	rjmp	tail
ir2:	stringout "2.395"
	rjmp	tail
ir3:	stringout "4.789"
	rjmp	tail
ir4:	stringout "7.037"
	rjmp	tail
ir5:	stringout "9.579"
	rjmp	tail
ir6:	stringout "14.368"
	rjmp	tail
ir7:	stringout "19.157"
	rjmp	tail
ir8:	stringout "28.736"
	rjmp	tail
ir9:	stringout "38.314"
	rjmp	tail
ir10:	stringout "43.103"
	rjmp	tail
ir11:	stringout "49.261"
	rjmp	tail
ir12:	stringout "57.471"
	rjmp	tail
ir13:	stringout "68.966"
	rjmp	tail
ir14:	stringout "86.207"
	rjmp	tail
ir15:	stringout "114.943"

tail:	stringout	"Kb"
	ret



chantext:
	.dw	chn0, chn1, chn2, chn3, chn4, chn5, chn6, chn7, chn8, chn9
	.dw	chn10, chn11, chn12, chn13, chn14, chn15,  chn16, chn17, chn18


chn0:	stringout "430.5"
	rjmp	CHtail
chn1:	stringout "431"
	rjmp	CHtail
chn2:	stringout "431.5"
	rjmp	CHtail
chn3:	stringout "432"
	rjmp	CHtail
chn4:	stringout "432.5"
	rjmp	CHtail
chn5:	stringout "433"
	rjmp	CHtail
chn6:	stringout "433.5"
	rjmp	CHtail
chn7:	stringout "434"
	rjmp	CHtail
chn8:	stringout "434.5"
	rjmp	CHtail
chn9:	stringout "435"
	rjmp	CHtail
chn10:	stringout "435.5"
	rjmp	CHtail
chn11:	stringout "436"
	rjmp	CHtail
chn12:	stringout "436.5"
	rjmp	CHtail
chn13:	stringout "437"
	rjmp	CHtail
chn14:	stringout "437.5"
	rjmp	CHtail
chn15:	stringout "438"
	rjmp	CHtail
chn16:	stringout "438.5"
	rjmp	CHtail
chn17:	stringout "439"
	rjmp	CHtail
chn18:	stringout "439.5"
	
CHtail:	stringout	"Mhz"
	ret









