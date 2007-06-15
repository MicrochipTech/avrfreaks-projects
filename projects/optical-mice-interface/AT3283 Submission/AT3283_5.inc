;	File name: AT3283_5.inc
;
;	Support for mouse I/O
;
; ==============================================
;	sendmouse - send a byte to a mousie
;
;	r0 - mouse number (0 - 2)
;	r1 - byte to send, preserved
;	call sendmouse
;	  brne  mouse is busy, thank you
;	  breq  byte was sent (includes timeout error)
;
;	this manually puts out the start bit and then
;	passes it off to the interrupt handler.

sendmouse:
	push	ZH
	push	ZL
	push	r16

	rcall	moff2z			; Z -> our definition block
	ldd	r16,Z+mo_sts		; get current line status
	tst	r16
	breq	sok			; 0 is truly idle
	cpi	r16,4			; 4 is idle, once line is high
	brne	mbzy			; otherwise it's busy

	ldi	r16,2			; 2 milliseconds
	call	starttimer
wwx:	rcall	readclock		; wait until it clock line is up
	brcs	sokk			; jump if up
	ldd	r16,Z+mo_err		; otherwise check timeout
	tst	r16
	breq	wwx			; zero means no timeout
	rjmp	xbzy			; on timeout we give up

sokk:	rcall	stoptimer	
sok:	ldi	r16,1			; we're setting up a send
	std	Z+mo_sts,r16		; set status to 1

;	when we set the clock to output, it will go low because we did not
;	enable pullups (i.e., port = 0). this will cause a falling edge
;	interrupt which the interrupt handler will ignore
	rcall	clock2output		; set clock line to output
	ldi	r16,16			; doc says 15ms max for mouse
	rcall	starttimer		; to respond with clock

	; now we need to wait at least 100 microsecs (800 clock cycles)

	std	Z+mo_s,r1		; the outgoing character
	clr	r16
	std	Z+mo_sc,r16		; number of bits sent
	std	Z+mo_sp,r16		; parity calculation

wl:	dec	r16			; 256 loops should do it
	brne	wl
wla:	dec	r16			; a little extra to make sure
	brne	wla

;	when we set the data line to output, it will go low because we
;	did not enable pullups (port = 0).  this is our start bit.	
	rcall	data2output		; set data as output

	ldi	r16,20			; let things settle a little
ww1:	dec	r16
	brne	ww1

;	setting the clock to input will tristate its output, and the
;	mouse's pullup will bring it high if there's a mouse plugged in
	rcall	clock2input		; set clock to input

;	now it's all up to the mouse to provide a clock.  if it's not
;	there, it probably won't.  if it does, we shoot the data out
;	when the clock falls (in the interrupt handler).
	ldi	r16,2			; sending in progress
	std	Z+mo_sts,r16

;	now we sit and let the interrupt handler do the rest
wlx:	ldd	r16,Z+mo_sts		; wait for status = 4 or timeout
	cpi	r16,4
	breq	xbzy			; yup - ok to go
	ldd	r16,Z+mo_err		; timeout sets this to 1
	tst	r16
	breq	wlx
xbzy:	sez				; set zero flag (success or timeout)

mbzy:	pop	r16
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	mint2 - mouse interrupt handler, part deux
;
;	mouse number will be in r0 (0, 1, or 2)
;	r16 may be used without save/restore (caller does it)
;	end with ret, not reti
;
;	status:	0 - idle (initial and after read or write)
;		1 - sending setup in progress (ignore interrupts)
;		2 - sending data
;		3 - receiving data
;		4 - received stop bit (ok for new receive, but
;		    if sendmouse sees this it has to wait for
;		    the line to go high
;		5 - waiting ack for send

mint2:	push	ZH
	push	ZL

	rcall	moff2z
	ldd	r16,Z+mo_sts		; current line status
	push	r16			; it's arg to jumptable
	rcall	jumptable
	rjmp	strtrecv		; 0 - this int's a start bit
	rjmp	ignore			; 1 - ignore (sendmouse did it)
	rjmp	sendbits		; 2 - we're clocking bits out
	rjmp	recvbits		; 3 - we're clocking bits in
	rjmp	strtrecv		; 4 - it's a start bit
	rjmp	waitack			; 5 - it's the send ack

;	---------------------------------
;	the line is idle (0) or we received a stop bit on the last clock (4),
;	and we've had a falling edge. the mouse is sending us a start bit
strtrecv:				; it's a start bit for receive
	rcall	startofbyte		; packet mgmt
	ldi	r16,3			; should wrap up in a couple ms
	rcall	starttimer
	clr	r16
	std	Z+mo_r,r16		; received byte = 0
	std	Z+mo_rc,r16		; received bit count = 0
	std	Z+mo_rp,r16		; received parity = 0
	ldi	r16,3			; status: receive in progress
	std	Z+mo_sts,r16
	
ignore:	pop	ZL
	pop	ZH
	ret

;	---------------------------------
;	reading bits from the mouse (data, parity, stop)
recvbits:
	ldd	r16,Z+mo_rc		; # bits received so far
	inc	r16			; count this one
	std	Z+mo_rc,r16

	cpi	r16,9
	breq	pbit			; if 9, this is parity bit
	brcs	dbit			; if <9, it's a data bit

	ldd	r16,Z+mo_r		;  doggies, it's the stop bit
	push	XH
	push	XL
	rcall	mhinc2x			; point to buffer head w/ inc
	st	X,r16			; put our byte away
	pop	XL
	pop	XH
	rcall	endofbyte		; packet mgmt

	rcall	stoptimer
	ldi	r16,4			; stop bit is still active
	std	Z+mo_sts,r16		; stat = 4, need to wait it out
	rjmp	ignore

dbit:	set				; T=1 means do parity
	rcall	readdata		; read bit -> carry
	ldd	r16,Z+mo_r		; the character so far
	ror	r16			; shift bit into high end
	std	Z+mo_r,r16		; and put it away
	rjmp	ignore

pbit:	clt				; T=0 means no parity
	call	readdata		; read parity bit -> carry
	
	; check receive parity here, or not

	rjmp	ignore

;	---------------------------------
;	sending bits to the mouse (data, parity, stop)
sendbits:
	ldd	r16,Z+mo_sc		; # bits sent so far
	inc	r16			; count this one
	std	Z+mo_sc,r16
	cpi	r16,9			; 1-8, data; 9 par, 10 stop
	breq	parbit			; jump if parity bit
	brcs	dtabit			; jump if data bits

;	this is the stop bit going out - set data line to input
	rcall	data2input		; switch to input
;	if the parity bit were a 1, our pullups are now enabled
	clc				; need to write a 0
	rcall	writedata		; make sure pullups are off

	ldi	r16,5			; there's one more clock coming
	std	Z+mo_sts,r16		; status: waiting ack
	rjmp	ignore

dtabit:	cpi	r16,1			; is this data0?
	brne	dta2			; jmp if no

	rcall	stoptimer		; end of timed cycle for setup,
	ldi	r16,3			; docs say 2 ms to stop bit
	rcall	starttimer		; for sending character

dta2:	ldd	r16,Z+mo_s		; outgoing character
	ror	r16			; low order bit -> carry
	std	Z+mo_s,r16		; put back remainder
	brcc	wrdta
	ldd	r16,Z+mo_sp		; parity accumulation
	inc	r16			; we're counting ones
	std	Z+mo_sp,r16
wrdta:	rcall	writedata		; away she goes
	rjmp	ignore

parbit:	ldd	r16,Z+mo_sp		; bit 0 = our even parity
	com	r16			; switch to odd
	ror	r16			; put it into carry
	rcall	writedata		; and write it
	rjmp	ignore

;	---------------------------------
;	waiting for the ack following a send
waitack:
	rcall	stoptimer		; we made it
	ldi	r16,4			; the clock's still low - sending
	std	Z+mo_sts,r16		; anything must wait, receive OK
	rjmp	ignore

; ==============================================
;	zerobt - zero the btsav area for a mouse
;
;	r0 - mouse number

zerobt:
	push	ZH
	push	ZL
	push	r16
	push	r17

	ldi	ZH,high(btsav)		; Z -> mouse 0
	ldi	ZL,low(btsav)
	tst	r0
	breq	zer1
	adiw	ZH:ZL,btsavsize		; Z -> mouse 1
	sbrc	r0,2
	adiw	ZH:ZL,btsavsize		; Z -> mouse 2

zer1:	ldi	r16,btsavsize		; number of bytes
	clr	r17
zer2:	st	Z+,r17
	dec	r16
	brne	zer2

	pop	r17
	pop	r16
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	mr_init - initialize the mouse receive buffers, etc
;
;	general initialization, performed once

mr_init:
	push	r0
	push	r16
	push	ZH
	push	ZL
	push	YH
	push	YL

;	zero out moff(), btsav(), and pack()
	clr	r0
	ldi	ZH,high(moff)
	ldi	ZL,low(moff)
	ldi	r16,3 * mblksize	; number of bytes in all 3
lpmr:	st	Z+,r0
	dec	r16
	brne	lpmr

	ldi	r16,3 * btsavsize	; number of bytes
	ldi	ZL,low(btsav)
	ldi	ZH,high(btsav)		; transition save area
lpmr2:	st	Z+,r0
	dec	r16
	brne	lpmr2

	ldi	r16,3 * packsize	; number of bytes
	ldi	ZL,low(pack)
	ldi	ZH,high(pack)		; transition save area
lpmr3:	st	Z+,r0
	dec	r16
	brne	lpmr3

;	unfold the "wiring" data into moff() starting at wd_pin
	ldi	ZH,high(wiring<<1)
	ldi	ZL,low(wiring<<1)
	ldi	YH,high(moff)
	ldi	YL,low(moff)
	adiw	YH:YL,wd_pin		; where I/O stuff starts, mouse 0

	ldi	r16,6			; 6 pairs of items
lkz:	rcall	unfold			; do a pair + store 5 bytes
	dec	r16
	breq	jnxt			; are we done with all 6?
	sbrs	r16,0			; no - do post processing
	rjmp	jeven
	adiw	YH:YL,5			; odd: move from data to clock
	rjmp	lkz
jeven:	adiw	YH:YL,mblksize - 5	; even: on to next mouse's pin
	adiw	ZH:ZL,1			; jump over input separator
	rjmp	lkz

jnxt:	pop	YL
	pop	YH
	pop	ZL
	pop	ZH
	pop	r16
	pop	r0
	ret

; ==============================================
;	unfold - put a hardware pin definition into mcom()
;
;	Z - points to 2 char ascii string in CSEG defining the pin
;           Z is returned incremented to following character
;	Y - points to a wd_pin entry in mcom (preserved)
;	we will define:
;		Y+w_pin   PINx address (with 32 added)
;		Y+w_port  PORTx address (with 32 added)
;		Y+w_ddr   DDRx address (with 32 added)
;		Y+w_and   mask with our pin off, all others on
;		Y+w_or    mask with our pin on, all others off

unfold:	push	r1
	push	r2

	lpm	r1,Z+			; port letter
	lpm	r2,Z+			; pin number
	rcall	unfold2			; store data into pindef()
	; we're ignoring the error return here, so beware!

	lds	r1,pindef+w_pin		; this is ugly but as
	std	Y+w_pin,r1		; efficient as a loop
	lds	r1,pindef+w_port
	std	Y+w_port,r1
	lds	r1,pindef+w_ddr
	std	Y+w_ddr,r1
	lds	r1,pindef+w_and
	std	Y+w_and,r1
	lds	r1,pindef+w_or
	std	Y+w_or,r1

	pop	r2
	pop	r1
	ret

; ==============================================
;	unfold2 - put a hardware pin definition into pindef()
;
;	r1 - ascii port number ('A' to 'D')
;	r2 - ascii pin number ('0' to '7')
;	rcall unfold2
;	  brne OK
;	  breq error (something out of range)
;
;	we will define pindef()
;		(0)   PINx address (with 32 added)
;		(1)   PORTx address (with 32 added)
;		(2)   DDRx address (with 32 added)
;		(3)   mask with our pin off, all others on (AND)
;		(4)   mask with our pin on, all others off (OR)

unfold2:
	push	r16
	push	r17

	mov	r16,r1			; ascii port A - D
	cpi	r16,'A'
	brcs	u2err
	cpi	r16,'D'
	breq	u2ok1
	brcc	u2err
u2ok1:	mov	r17,r2			; ascii pin number
	cpi	r17,'0'
	brcs	u2err
	cpi	r17,'9'
	breq	u2ok2
	brcc	u2err

u2ok2:	push	r18

	subi	r16,'A'			; convert to decimal 0-3
	subi	r17,'0'			; convert to decimal 0-7
	clr	r18			; build a mask
	sec				; set carry
ush:	rol	r18			; shift carry bit across
	tst	r17
	breq	ubd
	dec	r17			; countdown to our bit
	rjmp	ush
ubd:	sts	pindef+w_or,r18		; the "OR" mask - 1 bit on
	com	r18			; flip the bits
	sts	pindef+w_and,r18		; the "AND" mask - 1 bit off

	ldi	r17,0x39		; address of PINA, with $20 added
ubf:	tst	r16			; subtract 3 per port from address
	breq	ube
	dec	r17
	dec	r17
	dec	r17
	dec	r16
	rjmp	ubf

ube:	sts	pindef+w_pin,r17	; PINx
	inc	r17
	sts	pindef+w_ddr,r17	; DDRx
	inc	r17
	sts	pindef+w_port,r17	; PORTx

	pop	r18
	clz			; success clears zero (brne ok)
	rjmp	urtn

u2err:	sez			; error sets zero (breq err)
urtn:	pop	r17
	pop	r16
	ret

; ==============================================
;	mrecv_test - check for data in mouse receive buffer
;
;	r0 - mouse number (0 - 2)
;	call	mrecv_test
;	breq	empty
;	brne	not empty

mrecv_test:
	push	ZH
	push	ZL
	push	r16
	push	r17
	
	rcall	moff2z			; get z -> our mouse def
	ldd	r16,Z+mo_hd		; head
	ldd	r17,Z+mo_t		; tail
	cp	r16,r17

	pop	r17
	pop	r16
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	moff2z - make Z point to mouse r0's definition block
;
;	r0 - mouse number (0 - 2)
;	returns Z -> moff(r0 * mblksize)

moff2z:	ldi	ZL,low(moff)
	ldi	ZH,high(moff)
	tst	r0
	breq	mrb			; right answer for 0
	adiw	ZH:ZL,mblksize		; advance to mouse 1
	sbrc	r0,1			; if mouse 1, we're done
	adiw	ZH:ZL,mblksize		; advance to mouse 2
mrb:	ret

; ==============================================
;	mbuf2x - make X point to r0's mouse buffer
;
;	r0 - mouse number (0 - 2)
;	returns X -> mbuf(mbufsize * r0)

mbuf2x:	ldi	XL,low(mbuf)
	ldi	XH,high(mbuf)
	tst	r0
	breq	mre			; right answer for 0
	adiw	XH:XL,mbufsize		; advance to mouse 1
	sbrc	r0,1			; if mouse 1, we're done
	adiw	XH:XL,mbufsize		; advance to mouse 2
mre:	ret

; ==============================================
;	get X to point at head or tail of a mouse buffer
;
;	entry points:
;	   mhead2x - make X -> head of r0's mouse buffer
;	   mtail2x - make X -> tail of r0's mouse buffer
;	   mhinc2x - make X -> head, post increment head ptr
;	   mtinc2x - make X -> tail, post increment tail ptr
;
;	interrupt handler uses mhinc2x to store at head
;	app uses mtinc2x to retrieve from tail
;
;	r0 - mouse number (0 - 2)
;	returns X -> current head/tail in mbuf()
;	   and optionally bumps head/tail offset by one

mhead2x:
	push	r16
	push	r17
	ldi	r16,mo_hd		; offset to head
	clr	r17			; no post increment
	rjmp	mheadtail
mtail2x:
	push	r16
	push	r17
	ldi	r16,mo_t		; offset to tail
	clr	r17			; no post increment
	rjmp	mheadtail
mhinc2x:
	push	r16
	push	r17
	ldi	r16,mo_hd		; offset to head
	ldi	r17,1			; post increment
	rjmp	mheadtail
mtinc2x:
	push	r16
	push	r17
	ldi	r16,mo_t		; offset to tail
	ldi	r17,1			; post increment

mheadtail:
	push	ZH
	push	ZL

	rcall	moff2z			; make Z point at our blk
	add	ZL,r16			; add head or tail offset
	brcc	mpass
	inc	ZH
mpass:	ld	r16,Z			; get head or tail value
	rcall	mbuf2x			; make X point to buffer
	add	XL,r16
	brcc	mpass2
	inc	XH
mpass2:	tst	r17			; post incrementing?
	breq	mpass3			; skip if not
	inc	r16			; incr the head or tail
	andi	r16,mrmask		; keep it in range
	st	Z,r16			; put it away

mpass3:	pop	ZL
	pop	ZH
	pop	r17
	pop	r16
	ret

; ==============================================
;	clock2output - set clock as output
;
;	Z  - points to moff() block for this mouse
;	we need to write a 1 to the clock's DDR register bit

clock2output:
	push	XH
	push	XL
	push	r16
	push	r17

	ldd	r17,Z+wc_or		; mask to force one bit
	ldd	XL,Z+wc_ddr		; address of DDRx
	clr	XH			; they're small...
	ld	r16,X			; read DDRx
	or	r16,r17			; add our bit
	st	X,r16			; put it back with bit

	pop	r17
	pop	r16
	pop	XL
	pop	XH
	ret

; ==============================================
;	data2output - set data as output
;
;	Z  - points to moff() block for this mouse
;	we need to write a 1 to the data DDR register bit

data2output:
	push	XH
	push	XL
	push	r16
	push	r17

	ldd	r17,Z+wd_or		; mask to force one bit
	ldd	XL,Z+wd_ddr		; address of DDRx
	clr	XH			; they're small...
	ld	r16,X			; read DDRx
	or	r16,r17			; add our bit
	st	X,r16			; put it back with bit

	pop	r17
	pop	r16
	pop	XL
	pop	XH
	ret

; ==============================================
;	data2input - set mouse data line as input
;
;	Z  - points to moff() block for this mouse
;	we need to write a 0 to the data DDR register bit

data2input:
	push	XH
	push	XL
	push	r16
	push	r17

	ldd	r17,Z+wd_and		; mask to lose one bit
	ldd	XL,Z+wd_ddr		; address of DDRx
	clr	XH			; they're small...
	ld	r16,X			; read DDRx
	and	r16,r17			; drop our bit
	st	X,r16			; put it back sans bit

	pop	r17
	pop	r16
	pop	XL
	pop	XH
	ret

; ==============================================
;	clock2input - set clock as input
;
;	Z  - points to moff() block for this mouse
;	we need to write a 0 to the clock's DDR register bit

clock2input:
	push	XH
	push	XL
	push	r16
	push	r17

	ldd	r17,Z+wc_and		; mask to lose one bit
	ldd	XL,Z+wc_ddr		; address of DDRx
	clr	XH			; they're small...
	ld	r16,X			; read DDRx
	and	r16,r17			; drop our bit
	st	X,r16			; put it back sans bit

	pop	r17
	pop	r16
	pop	XL
	pop	XH
	ret

; ==============================================
;	readdata - read mouse data bit into carry reg
;
;	Z  - points to moff() block for this mouse
;	T flag - if zero, don't do parity calculation

readdata:
	push	XH
	push	XL
	push	r17
	push	r18

	ldd	r17,Z+wd_or		; mask to keep one bit
	ldd	XL,Z+wd_pin		; address of PINx
	clr	XH			; they're small...
	ld	r18,X			; read PINx
	and	r18,r17			; mask all but our bit
	breq	zippy			; if zero, done
	brtc	zappy			; T = 0, no parity

	ldd	r17,Z+mo_rp		; bump the parity count
	inc	r17
	std	Z+mo_rp,r17

zappy:	sec				; it was a one
	rjmp	zrtn
zippy:	clc				; it was a zero

zrtn:	pop	r18
	pop	r17
	pop	XL
	pop	XH
	ret

; ==============================================
;	readclock - read clock bit into carry reg
;
;	Z  - points to moff() block for this mouse

readclock:
	push	XH
	push	XL
	push	r17
	push	r18

	ldd	r17,Z+wc_or		; mask to keep one bit
	ldd	XL,Z+wc_pin		; address of PINx
	clr	XH			; they're small...
	ld	r18,X			; read PINx
	and	r18,r17			; mask all but our bit
	breq	cippy			; if zero, done
	sec				; it was a one
	rjmp	crtn
cippy:	clc				; it was a zero

crtn:	pop	r18
	pop	r17
	pop	XL
	pop	XH
	ret

; ==============================================
;	writedata - write mouse data bit
;
;	Z  - points to moff() block for this mouse
;	carry flag - bit to write

writedata:
	push	XH
	push	XL
	push	r16
	push	r17

	ldd	XL,Z+wd_port		; address of PORTx
	clr	XH			; they're small...
	ld	r16,X			; read PORTx
	brcc	wr0a			; jump if writing 0
	ldd	r17,Z+wd_or		; writing a 1
	or	r16,r17			; turn our bit on
	rjmp	away2
wr0a:	ldd	r17,Z+wd_and		; writing a 0
	and	r16,r17			; drop our bit
away2:	st	X,r16			; write PORTx

	pop	r17
	pop	r16
	pop	XL
	pop	XH
	ret

; ==============================================
;	writeclock - write a mouse clock bit
;
;	Z  - points to moff() block for this mouse
;	carry flag - bit to write

writeclock:
	push	XH
	push	XL
	push	r16
	push	r17

	ldd	XL,Z+wc_port		; address of PORTx
	clr	XH			; they're small...
	ld	r16,X			; read PORTx
	brcc	write0
	ldd	r17,Z+wc_or		; write a 1
	or	r16,r17			; turn bit on
	rjmp	away
write0:	ldd	r17,Z+wc_and		; write a 0
	and	r16,r17			; drop our bit
away:	st	X,r16

	pop	r17
	pop	r16
	pop	XL
	pop	XH
	ret

; ==============================================
;	getmbyte - maybe get a byte from a mouse
;
;	r0 - mouse number (0 - 3, preserved)
;	call getmbyte
;	  breq no data available
;	  brne data in r1

getmbyte:
	rcall	mrecv_test
	brne	rddta
	ret

rddta:	push	XH
	push	XL
	rcall	mtinc2x		; point at tail data
	ld	r1,X		; get it
	clz			; tell them we have it
	pop	XL
	pop	XH
	ret
