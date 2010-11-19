
.equ	cr		= 0x0d
.equ	lf		= 0x0a
.equ	backspace	= 8
.equ	maxchar		= 32			; size of linebuf


	.dseg

paramcount:		.byte		1	; number of items found in linebuffer
charcount:		.byte		1	; how may chars in linebuf
linebuf:		.byte		maxchar	; buffer for input text
Ploc:			.byte		2	; parameter location pointer


	.cseg


check_for_input:
	rcall	sersample0	; rets C=char in temp1, NC=no char
	brcs	doit
	ret

doit:	rcall	to_lowercase	; all uppercase ascii to lower-case
	cpi	temp1,cr	; is it an end-of-line char?
	brne	notdun		; if not, jump
	clr	temp1		; so it is a CR, replace with a zero
	rcall	saveit		; save as end of string so now ascii-z

; at this point there are options.
; OPTION1. We are communicating by serial to THIS node, ie we are LOCAL so
; just go to the local section which parses and actions the command.

; OPTION2. We have 'connected' to a REMOTE node by the RF link so once the
; line has been 'entered' ie the CR (0X0d) has been received, copy the line to the
; transmit buffer, package it up and send it to the 'connected' node as a
; command packet (a cmdpack, see RFM12B.asm)

	lds	temp1,outgoing_connect	; get connect state
	jeq	temp1,false,local	; if NOT connected, process locally
	jmp	send_linebuffer		; else send linebuffer to remote node


local:	rcall	parser		; remove whitespace, count items. C=items exist

	clr	temp1
	sts	charcount,temp1	; reset linebuf count
; =============================================================================
	jmp	docom		; now go and process line (cmd_tables.asm)
; =============================================================================


notdun:	cpi	temp1,backspace	; is it a backspace char?
	brne	notback		; if no, jmp fwd
; char is a backspace so....
	lds	temp2,charcount	; get count of chars in buffer
	cpi	temp2,0		; is it=0? ie trying to backspace an empty buffer
	brne	remove		; if buffer not empty, jmp fwd
;	send	0x07		; bell char to stream-out
	rjmp	alldone		; exit path

remove:	dec	temp2		; reduce buffer count
	sts	charcount,temp2	; resave count
	send	0x08		; send BS char
	rjmp	alldone		; exit path

notback:
	lds	temp2,charcount	; get the buffer count
	cpi	temp2,maxchar-1	; is it too full?
	brne	notful		; if no, skip fwd
	rjmp	alldone		; exit

notful:	rcall	to_lowercase	; buffer not full, convert char to lowercase
	rcall	saveit		; save char in buffer
alldone:
	clc			; not a completed line so NC yet
	ret


saveit:	rcall	stream_out
	push	temp1
	ldZ	ram,linebuf
	clr	temp1			; =0
	lds	temp2,charcount		; items in buffer
	add	zl,temp2		; add as offset
	adc	zh,temp1
	pop	temp1
	st	z,temp1
	inc	temp2			; inc char counter
	sts	charcount,temp2
	ret

; *****************************************************************************
; DESCR: converts alph's to lowecase
; ENTRY:
; EXIT:
; CHANGES:
; USES:
; *****************************************************************************
to_lowercase:
	cpi	temp1,'Z'+1
	brsh	noconv
	cpi	temp1,'A'
	brlo	noconv
	push	temp2
	ldi	temp2,0x20
	add	temp1,temp2
	pop	temp2
noconv:
	ret




; *****************************************************************************
; DESCR: counts items in buffer (separated by whitespace- replaced by zeros)
; ENTRY: data in linebuf
; EXIT: parameters counted
; CHANGES: linebuf, paramcount
; USES:
; *****************************************************************************
parser:
	lds	temp1,charcount		; get char count
	tst	temp1			; test if empty (ie just a cr)
	brne	scanline		; if NZ then scan line
	clc				; show no data
	ret
scanline:
	mov	loopcounter,temp1	; copy char count
	ldZ	ram,linebuf		; point to data
	rcall	eat_whitespace		; replaces whitespace with 0's
	rcall	count_params		; how many items on cmd-line
	tst	proc_count		; check if ANY actual params found
	brne	gotsome
	clc				; for none. Could just be a line
	ret				;..of whitespace

; must have found at least one paramater to get past here
gotsome:
	sec
	ret


; *****************************************************************************
; DESCR: counts the parameters on the commandline
; ENTRY: data in linebuf
; EXIT: 
; CHANGES:
; USES:
; *****************************************************************************
count_params:
	clr	proc_count		; temp param count
	lds	loopcounter,charcount	; get char count
	ldZ	ram,linebuf		; point to data

nixt:	ld	temp1,z+		; get a char, inc pointer
	rcall	test_whitespace		; look for NOT whitespace
	brcc	eatchar			; CC=not whitespace

back:	dec	loopcounter
	brne	nixt
; all chars done
	sts	paramcount,proc_count
	ret

eatchar:
	inc	proc_count		; a param so count it
	dec	loopcounter
	brne	naxt
	ret

naxt:	ld	temp1,z+
	rcall	test_whitespace		; now look for NOT whitespace
	brcs	back			; cs=whitespace
	dec	loopcounter
	brne	naxt
	ret



; *****************************************************************************
; DESCR: checks if a char is a space/comma/tab char
; ENTRY: char in temp1
; EXIT: C=whitespace, NC=no whitespace
; CHANGES: C
; USES: none
; *****************************************************************************
test_whitespace:
	jeq	temp1,' ',white	; space
	jeq	temp1,',',white	; comma
	jeq	temp1,9,white	; tab
	jeq	temp1,0,white	; null
	clc
	ret
white:	sec
	ret

; *****************************************************************************
; DESCR: works thru the linebuf and replaces whitespace with 0's
; ENTRY: data in linebuf >=1 char
; EXIT: whitespace converted
; CHANGES: linebuf
; USES: none
; *****************************************************************************
eat_whitespace:
	ld	temp1,z
	rcall	test_whitespace
	brcc	nowh
	clr	temp1
nowh:	st	z+,temp1
	dec	loopcounter
	brne	eat_whitespace
	ret





; *****************************************************************************
; DESCR: tests if a char is (lowercase) alpha or number (A/N)
; ENTRY: char in temp1
; EXIT: char in temp1, C=yes for A/N  NC=not A/N
; CHANGES: C flag
; USES: none
; *****************************************************************************
test_AN:
	cpi	temp1,'z'+1	; test if > ascii 9
	brsh	noalpha
	cpi	temp1,'a'	; test if < ascii 0
	brlo	noalpha

	cpi	temp1,'9'+1	; test if > ascii 9
	brsh	noalpha
	cpi	temp1,'0'	; test if < ascii 0
	brlo	noalpha
; if a valid alpha-numeric (lower case only) then set C end exit
	sec
	ret
; if not then clear C
noalpha:
	clc
	ret


; *****************************************************************************
; DESCR: clears charcount to restart line aquisition
; ENTRY: -
; EXIT: charcount=0
; CHANGES: charcount
; USES: -
; *****************************************************************************
reset_line:
	clr	temp1
	sts	charcount,temp1
	ret



; *****************************************************************************
; DESCR: shows complete linebuf
; ENTRY: data to show
; EXIT: sent to serial
; CHANGES:
; USES:
; *****************************************************************************
slb:	ldZ	ram,linebuf
	lds	loopcounter,charcount

dis:	ld	temp1,z+
	cpi	temp1,0
	brne	dus
	rcall	hexout
	rjmp	das
dus:	rcall	stream_out
das:	dec	loopcounter
	brne	dis
	ret

; *****************************************************************************
; DESCR: gets address of a paramater from line buffer
; ENTRY: temp1=param number (1...X)
; EXIT: C=Ploc(ram) has address of required param. NC=not found (not enuf params)
; CHANGES: Ploc
; USES: -
; *****************************************************************************
get_param:
	clr	proc_count			; local count
	tst	temp1
	breq	badP			; if=0, bad
	lds	temp2,paramcount
	inc	temp2
	cp	temp1,temp2
	brsh	badP			; if higher than available, exit
	mov	temp2,temp1		; required param to locate
	ldZ	ram,linebuf

; dont have to worry about char count
plq:	ld	temp1,z+
	tst	temp1
	breq	plq
; falls thru when NZ
	inc	proc_count			; found param
	cp	proc_count,temp2		; is it the one we want
	breq	foundit
; now work through param till Z again

plq2:	ld	temp1,z+
	tst	temp1
	brne	plq2
	rjmp	plq
foundit:
	sbiw	zh:zl,1			; back-off the pointer by one
	storeZ	Ploc
	sec				; flag found it
	ret
badP:	clc
	ret




; *****************************************************************************
; DESCR: gets the length of an ascii-Z string
; ENTRY: string in ram at X
; EXIT: length in loopcounter
; CHANGES: temp1, loopcounter, X
; USES: -
; *****************************************************************************
getlen:
	ld	temp1,x+
	tst	temp1
	breq	finlen
	inc	loopcounter
	rjmp	getlen
finlen:	ret




; *****************************************************************************
; DESCR: sends a found param to serial
; ENTRY: must have used get_param first
; EXIT: param sent to output stream
; CHANGES:
; USES:
; *****************************************************************************
show_param:
	loadZ	Ploc
fet:	ld	temp1,z+
	tst	temp1
	brne	sho
	ret
sho:	rcall	stream_out
	rjmp	fet


