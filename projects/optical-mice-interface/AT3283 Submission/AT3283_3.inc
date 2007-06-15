;	File name: AT3283_3.inc
;
;	various and sundry support routines
;
; ==============================================
;	gettimer - read a timer error flag
;
;	r0 - mouse whose error flag to read
;	returns: zero set if ok, cleared if error

gettimer:
	push	ZH
	push	ZL
	push	r16

	rcall	moff2z
	ldd	r16,Z+mo_err
	tst	r16

	pop	r16
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	stoptimer - stop the timer
;
;	r0 - mouse number

stoptimer:
	push	ZH
	push	ZL
	push	r16

	rcall	moff2z
	clr	r16			; set mode = 0
	std	Z+mo_tmode,r16

	pop	r16
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	starttimer - start the timer
;
;	r0 - mouse whose timer we're starting
;	r16 - if positive, number of milliseconds (1 - 127, 0 = 255)
;	    - if bit 7 set, ms * 256 in low 7 bits
;	this clears the timer error flag

starttimer:
	push	ZH
	push	ZL
	push	r18

	rcall	moff2z
	clr	r18
	std	Z+mo_tmode,r18		; stop it
	std	Z+mo_err,r18		; clear error

	tst	r16
	brpl	tb8			; jump for 8 bit timer

	std	Z+mo_tlo,r18		; low <- 0
	mov	r18,r16
	andi	r18,0x7f
	breq	noway			; not that long
	std	Z+mo_thi,r18		; high <- low bits of r16
	ldi	r18,2			; mode = 2 is 16 bit
	rjmp	tbon

tb8:	std	Z+mo_tlo,r16		; low <- r16
	std	Z+mo_thi,r18		; high <- 0
	ldi	r18,1			; mode = 1 is 8 bit

tbon:	std	Z+mo_tmode,r18		; set mode to start

noway:	pop	r18
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	killmouse - disable mouse interrupt
;
;	r0 - mouse in question
killmouse:
	push	r16
	push	r17

	in	r17,gicr		; has int flags
	mov	r16,r0
	cpi	r16,1
	breq	kill1
	brcs	kill0
	ldi	r16,0xdf		; bit 5 is int2
	rjmp	kila
kill0:	ldi	r16,0xbf		; bit 6 is int0
	rjmp	kila
kill1:	ldi	r16,0x7f		; bit 7 is int1
kila:	and	r17,r16			; drop intx bit
	out	gicr,r17		; and put it back

	pop	r17
	pop	r16
	ret

; ==============================================
;	revivemouse - enable mouse interrupt
;
;	r0 - mouse in question

revivemouse:
	push	r16
	push	r17

	in	r17,gicr		; has int flags
	mov	r16,r0
	cpi	r16,1
	breq	rev1
	brcs	rev0
	ldi	r16,0x20		; bit 5 is int2
	rjmp	reva
rev0:	ldi	r16,0x40		; bit 6 is int0
	rjmp	reva
rev1:	ldi	r16,0x80		; bit 7 is int1
reva:	or	r17,r16			; set intx bit
	out	gicr,r17		; and put it back

	pop	r17
	pop	r16
	ret

; ==============================================
;	miscinit - miscellaneous initialization
;
;	r16 not preserved

miscinit:
	clr	r16
	sts	take5,r16		; 5 ms timer
	sts	abbrev,r16		; abbreviate output
	sts	bigcheese,r16		; default to mouse 0
	push	r16			; high baud rate as arg
	ldi	r16,26			; 19200 @ 8MHz
	push	r16			; low baud rate arg
	rcall	usart_init		; init USART
	rcall	mr_init			; init mouse bufs & defs

	ldi	r16,0x0a		; falling edge on int0 & 1
;					; int2 is falling by default
	out	mcucr,r16

	ldi	r16,0x0d		; set up timer
	out	tccr0,r16		; CTC w/ 1K prescaler
	ldi	r16,8			; 8 = 1ms interrupts
	out	ocr0,r16		; compare value

	clr	r16
	out	tcnt0,r16		; timer counter
	ldi	r16,0x02		; turn on ints for compare 0
	out	timsk,r16		; and away we go

	sei				; interrupts enabled
	ret

; ==============================================
;	timer 0 interrupt handler
;
;	we're running an 8Mhz clock with 1K prescaler
;	with a count of 8 = 1ms interrupts
;
;	we have a total of 12 clocks, 4 per mouse
;	  8 & 16 bit counters (only one can be active)
;	  packet timer for obtaining a complete packet
;	  packet byte timer for byte-to-byte timing

i_tim0c:				; interrupt: timer 0 compare
	push	r16
	in	r16,sreg		; status register
	push	r16
	push	r17
	push	ZH
	push	ZL

;	just in case we're needing a short break
	lds	r16,take5
	tst	r16
	breq	no5
	dec	r16
	sts	take5,r16

;	first do the line timers (8 and/or 16 bit)
no5:	ldi	ZH,high(moff)
	ldi	ZL,low(moff)
	adiw	ZH:ZL,mo_tmode		; first of 4 timer bytes
	ldi	r16,3			; looking at 3 mice

it0l:	ld	r17,Z			; timer mode (0 = stopped)
	cpi	r17,1
	brcs	it03			; stopped - move on
	breq	it04			; jump if 8 bit

;	we're here for 16 bit timer (mo_tmode = 2)
	push	YH
	push	YL
	ldd	YL,Z+ztlo		; low order
	ldd	YH,Z+zthi		; high order
	sbiw	YH:YL,1
	std	Z+ztlo,YL
	std	Z+zthi,YH
	pop	YL
	pop	YH
	breq	sttim
	rjmp	it03

;	come here if 8 bit timer (mo_tmode = 1)
it04:	ldd	r17,Z+ztlo		; low order timer (Z -> mo_tmode)
	dec	r17
	std	Z+ztlo,r17
	brne	it03			; if not zero, done

;	come here on user timer runout on either timer
sttim:	clr	r17			; stop the timer
	st	Z,r17			; mo_tmode = 0 means stop
	ldi	r17,1			; error flag
	std	Z+zerr,r17		; mo_err = 1 for error
	rcall	timeout			; go do packet processing

it03:	dec	r16			; counting passes
	breq	it99			; done
	adiw	ZH:ZL,mblksize		; next mouse
	rjmp	it0l

;	next do the packet and packet byte timers
it99:	ldi	ZH,high(pack)
	ldi	ZL,low(pack)
	ldi	r16,3			; again, 3 mice

it10:	ldd	r17,Z+ptrun		; packet timer status
	tst	r17
	breq	it11			; jump if not running
	ldd	r17,Z+ptimer		; get current value
	dec	r17			; counting down
	std	Z+ptimer,r17		; put it away
	brne	it11			; nonzero is fine

;	we had a packet timer runout
	clr	r17			; stop the timer
	std	Z+ptrun,r17
	rcall	prunout			; go process it

it11:	ldd	r17,Z+pbtrun		; packet byte timer status
	tst	r17
	breq	it13			; jump if not running
	ldd	r17,Z+pbtimer		; get current value
	dec	r17			; counting down
	std	Z+pbtimer,r17		; put it away
	brne	it13			; nonzero is fine

;	we had a packet byte timer runout
	clr	r17			; stop the timer
	std	Z+pbtrun,r17
	rcall	pbrunout		; go process it

it13:	dec	r16			; counting passes
	breq	it98			; done
	adiw	ZH:ZL,packsize		; next mouse
	rjmp	it10

it98:	pop	ZL
	pop	ZH
	pop	r17
	pop	r16
	out	sreg,r16		; restore status reg
	pop	r16
	reti

; ==============================================
;	int0 interrupt handler
i_irq0:
	push	r16
	in	r16,sreg		; status register
	push	r16
	push	r0

	clr	r0
	rcall	mint2			; do the mouse interrupt stuff

	pop	r0
	pop	r16
	out	sreg,r16		; restore status reg
	pop	r16
	reti

; ==============================================
;	int1 interrupt handler
i_irq1:
	push	r16
	in	r16,sreg		; status register
	push	r16
	push	r0

	clr	r0
	inc	r0
	rcall	mint2			; do the mouse interrupt stuff

	pop	r0
	pop	r16
	out	sreg,r16		; restore status reg
	pop	r16
	reti

; ==============================================
;	int2 interrupt handler
i_irq2:
	push	r16
	in	r16,sreg		; status register
	push	r16
	push	r0

	clr	r0
	inc	r0
	inc	r0
	rcall	mint2			; do the mouse interrupt stuff

	pop	r0
	pop	r16
	out	sreg,r16		; restore status reg
	pop	r16
	reti

; ==============================================
;	jumptable - execute single word jumptable
;
;	push	offset (0 to n - 1)
;	call    jumptable
;	rjmp	offset = 0
;	rjmp	offset = 1
;	rjmp	etc.

jumptable:
	push	YL		; save scratch regs
	push	YH
	push	XL
	push	r0

	in	YL,SPL		; get current stack pointer
	in	YH,SPH
	ldd	r0,y+7		; get pushed argument
	ldd	XL,Y+6		; low byte of orig rtn addr
	add	XL,r0		; add the argument
	std	Y+7,XL		; put away new low byte where arg was
	ldd	XL,Y+5		; high byte of orig rtn addr
	brcc	ncarry		; if carry, bump it one
	inc	XL
ncarry:	std	Y+6,XL		; put away new high where low was
	ldd	XL,Y+1		; save r0 value
	std	Y+5,XL		; overlay old hi rtn addr

	pop	r0		; original push value
	pop	XL		; restore everyone
	pop	YH
	pop	YL
	pop	r0		; the one we moved
	ret			; jump to modified and moved address

; ==============================================
;	looktable - look up byte and jump on match
;
;	push value to match (0-255)
;	call looktable
;	dw	val1,address if jmparg = val1
;	dw	val2,address if jmparg = val2
;	...	...
;	dw	-1,address for no match

looktable:
	push	YL		; save scratch regs
	push	YH
	push	ZL
	push	ZH
	push	r24
	push	r25
	push	r0

	in	YL,SPL		; copy of revised stack pointer
	in	YH,SPH
	ldd	ZH,Y+8		; high rtn address (word address, not byte)
	andi	ZH,0x1f		; manual pg 10 says to mask
	ldd	ZL,Y+9		; low rtn address
	lsl	ZL		; mult by 2 for byte address
	rol	ZH		; needed for lpm instr

	std	Y+8,r0		; overlay high rtn with r0
	ldd	r0,Y+10		; the argument (match value)

lloop:	lpm	r25,Z+		; match value
	lpm	r24,Z+		; high data byte, usually zero
	tst	r24
	brne	useme
	cp	r25,r0		; what we want
	breq	useme
	adiw	ZH:ZL,2
	rjmp	lloop

useme:	lpm	r0,Z+		; low target address
	std	Y+10,r0		; overwrite orig arg
	lpm	r0,Z+		; high target address
	std	Y+9,r0		; overwrite orig low rtn addr

	pop	r0		; dummy pop
	pop	r25		; restore scratch regs
	pop	r24
	pop	ZH
	pop	ZL
	pop	YH
	pop	YL
	pop	r0		; added this one
	ret			; go to modified and moved return address

; ==============================================
;	fetcharg1 - grab one stacked argument and diddle the stack
;
;	this is for routines that use a single pushed arg
;	call this to return pushed arg in r0
;	when done, pop r0 and ret
;
;	e.g.	push arg
;		call xyz
;
;	xyz:	call fetcharg1
;		r0 now has pushed arg
;		...
;		pop r0
;		ret
; note: you must call this prior to any other pushes!

fetcharg1:
	push	r0		; return arg here
	push	r1		; scratch
	push	YL
	push	YH

	in	YL,SPL		; copy of revised stack pointer
	in	YH,SPH

	ldd	r1,Y+9		; hold arg value
	ldd	r0,Y+8		; orig lo rtn addr
	std	Y+9,r0
	ldd	r0,Y+7		; orig hi rtn addr
	std	Y+8,r0
	ldd	r0,Y+4		; r0 we just pushed
	std	Y+7,r0
	std	Y+4,r1		; put the arg away

	pop	YH
	pop	YL
	pop	r1
	pop	r0		; this is arg; original deeper in stack
	ret

; ==============================================
;	fetcharg2 - grab two stacked arguments and diddle the stack
;
;	this is for routines that use two pushed args
;	call this to return pushed args in r0 and r1
;	when done, pop r0 and r1 and ret
;
;	e.g.	push arg1 (ends up in r0)
;		push arg2 (ends up in r1)
;		call xyz
;
;	xyz:	call fetcharg2
;		r0 now has first pushed arg
;		r1 now has second pushed arg
;		...
;		pop r1
;		pop r0
;		ret
; note: you must call this prior to any other pushes!

fetcharg2:
	push	r0		; return first arg here
	push	r1		; return second arg here
	push	YL
	push	YH

	in	YL,SPL		; copy of revised stack pointer
	in	YH,SPH

	ldd	r1,Y+10		; temp store arg1
	ldd	r0,Y+8		; lo rtn
	std	Y+10,r0
	ldd	r0,Y+4		; orig r0
	std	Y+8,r0
	std	Y+4,r1		; arg1 into stack

	ldd	r1,Y+9		; temp store arg2
	ldd	r0,Y+7		; hi rtn
	std	Y+9,r0
	ldd	r0,Y+3		; orig r1
	std	Y+7,r0
	std	Y+3,r1		; arg2 into stack

	pop	YH
	pop	YL
	pop	r1		; second arg; orig deeper
	pop	r0		; first arg; orig deeper
	ret

; ==============================================
;	convhex16 - convert ascii hex digit in r16
;
;	r16 - ascii hex coming in ('a' - 'f', 'A' - 'F', '0' - '9')
;
;	returns:
;		if OK, 0 - 15 in r16, zero flag clear
;		if error, orig char in r16, zero flag set

convhex16:
	cpi	r16,'0'
	brcs	nhex1			; jump if < '0'
	cpi	r16,'9'
	breq	ishex1
	brcc	nhex1			; jump if > '9'
ishex1:	andi	r16,0x0f		; keep low 4 bits
	rjmp	hexrtn

nhex1:	cpi	r16,'a'
	brcs	nhex2
	cpi	r16,'f'
	breq	ishex2
	brcc	nhex2
ishex2:	subi	r16,'a' - 10		; 'a' -> 10
	rjmp	hexrtn

nhex2:	cpi	r16,'A'
	brcs	nhex3
	cpi	r16,'F'
	breq	ishex3
	brcc	nhex3
ishex3:	subi	r16,'A' - 10		; 'A' -> 10
hexrtn:	clz				; zero clear means OK
	ret

nhex3:	sez				; zero set means not hex
	ret

; ==============================================
;	prepnum - prepare a number for hex output
;
;	Z - points to low order byte (preserved)
;	6 hex digits are put into pnum()
;	r20 has leading '0' or 'F' count
;	r21 has 1 for leading zeros, 2 for leading 'F'

prepnum:
	push	YH
	push	YL
	push	r16
	push	r18

	clr	r20		; count leading chars
	clr	r21		; what kind (none now)
	ldi	YH,high(pnum)	; where it goes
	ldi	YL,low(pnum)

	ldi	r18,6		; nibble number going out now
	adiw	ZH:ZL,3		; start at high order byte + 1

o3lp:	sbrs	r18,0		; odd or even?
	rjmp	o3grab		; even - need to get byte
	mov	r16,r0		; odd - use copy we previously saved
o3la:	andi	r16,0x0f	; keep nibble
	; we have our next nibble masked off
	tst	r21		; are we checking?
	brmi	o3no		; not any more
	tst	r16
	breq	o33		; this is a zero
	cpi	r16,15		; 
	breq	o33f		; this is an F

o3kill:	sbr	r21,0x80	; r21 neg means no more checking
	rjmp	o3no		; put it away

o33:	sbrc	r21,1		; we have a zero
	rjmp	o3kill		; but we're suppressing F's
	inc	r20		; zero is good - count it
	sbr	r21,0x01	; mark that we're doing zeros
	rjmp	o3no		; next input char

o33f:	sbrc	r21,0		; we have an F
	rjmp	o3kill		; but we're suppressing 0's
	inc	r20		; F is good - count it
	sbr	r21,0x02	; mark that we're doing F's
	rjmp	o3no

o3grab:	ld	r16,-Z		; grab a byte
	mov	r0,r16		; save a copy
	swap	r16		; want high nibble first
	rjmp	o3la

o3no:	rcall	bin2hex		; convert nibble to hex digit
	st	Y+,r16		; put ascii char into pnum()
	dec	r18		; how many to process
	brne	o3lp		; do it 6 times

	andi	r21,0x03	; drop bit 7 - rtn 1 or 2
	cpi	r20,6		; make 6 suppressions into 5
	brne	o3rtn
	ldi	r20,5

o3rtn:	pop	r18
	pop	r16
	pop	YL
	pop	YH
	ret

; ==============================================
;	out3bytenum - send 3 byte number on usart
;
;	Z - points to low order byte (garbaged)
;	output in hex. if abbrev = 0, suppress
;	leading zeros and sign extensions

out3bytenum:
	push	r21
	push	r20
	push	r17
	push	r16

	rcall	prepnum			; format the number
	ldi	ZH,high(pnum)		; where prepnum put it
	ldi	ZL,low(pnum)
	lds	r16,abbrev		; are we suppressing?
	tst	r16
	brne	nosup			; nonzero means no

	tst	r20			; leading char count
	breq	nosup			; nothing to suppress anyway

	sbrs	r21,1
	rjmp	szer			; r21 = 1 means suppress zeros
	ldi	r16,'-'			; r21 = 2 means suppress F's
	push	r16
	rcall	xmit_put		; leading '-' = compression

szer:	add	ZL,r20			; how many to hop over
	brcc	szer1
	inc	ZH
szer1:	ldi	r16,6
	sub	r16,r20			; how many are left to do?
	rjmp	slpx

nosup:	ldi	r16,6
slpx:	ld	r17,Z+
	push	r17
	rcall	xmit_put
	dec	r16
	brne	slpx

	pop	r16
	pop	r17
	pop	r20
	pop	r21
	ret

; ==============================================
;	bin2hex - convert nibble to hex
;
;	r16 - incoming binary value (0 - 15)
;	r16 - returned hex digit ('0' - '9', 'A' - 'F') or 'X' if error

bin2hex:
	cpi	r16,16
	brcs	bin1
	ldi	r16,'X'
	ret
bin1:	push	r17
	cpi	r16,10
	brcs	binlo
	ldi	r17,'A'-10
	rjmp	binr
binlo:	ldi	r17,'0'
binr:	add	r16,r17
	pop	r17
	ret
