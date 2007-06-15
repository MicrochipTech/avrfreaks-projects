;	File Name: AT3283_6.inc
;
;	more miscellaneous routines

; ==============================================
;	procmsg - process a mouse message, if any
;
;	r0 - mouse number
;	call procmsg
;	  brne error (1st byte read invalid,
;	              or mouse not initialized)
;	  breq ok, including no message waiting
;	  T is set if msg recvd, clear otherwise

procmsg:
	rcall	pktrdy		; is there a packet ready?
	brne	okproc		; yes
	clt			; no - T = 0 means no msg
	ret

okproc:	rcall	rdmmsg		; put new msg into our buf
	breq	okpr		; it looks good
	set			; we did have a msg (a bad one)
	ret			; zero is cleared = err

okpr:	push	ZH
	push	ZL
	push	r17
	push	r16
	push	r4
	push	r3

;	we have a new message and the current situation. we need to see if any
;	buttons have changed, note any overflow bits, and accumulate the offsets
	rcall	moff2z		; point at the mouse's definition
	ldd	r16,Z+mo_msg	; grab the new message's header byte
	mov	r3,r16		; save it in r3
	; header: 7=Yov,6=Xov,5=Ysgn,4=Xsgn,3=1,2=Mbtn,1=Rbtn,0=Lbtn
	ldd	r17,Z+mo_but	; current button/ovflo stat
	mov	r4,r17		; save it in r4

	eor	r17,r16		; this will flag changed bits
	andi	r17,0x07	; keep low 3 bits (btns: MRL)
	std	Z+mo_chg,r17	; what buttons changed
	or	r16,r4		; header | current
	andi	r16,0xc0	; keep the overflow bits in r16
	mov	r17,r3		; orig header
	andi	r17,0x07	; keep button bits in r17
	or	r17,r16		; toss in overflow bits
	std	Z+mo_but,r17	; new button & ovflo stats

	ser	r16		; extended sign bit
	sbrs	r3,4		; check header x sign
	clr	r16		; it's positive
	ldd	r17,Z+mo_msg+1	; x displacement
	rcall	xaddmoff

	ser	r16		; extended sign bit
	sbrs	r3,5		; check header y sign
	clr	r16		; it's positive
	ldd	r17,Z+mo_msg+2	; y displacement
	rcall	yaddmoff

	clr	r16		; assume positive z disp
	ldd	r17,Z+mo_msg+3	; z displacement
	; extra button bits go away here on 5 button mice
	andi	r17,0x0f	; drop high bits
	sbrs	r17,3		; check header y sign
	rjmp	ypos		; it's positive
	ser	r16		; neg - extended sign
	ori	r17,0xf0	; more extended sign
ypos:	rcall	zaddmoff

	ldd	r17,Z+mo_chg	; what buttons changed
	tst	r17		; any?
	breq	nolck		; jump if no changes
	call	savcur		; go save this location

	push	r5
	mov	r5,r17		; r5 has button changes
	ldd	r16,Z+mo_but	; buttons now, and ovrflo
	andi	r16,0x07	; keep buttons only
	mov	r4,r16		; r4 has current buttons
	ldi	r16,0x03	; do toggles and follows locks
	mov	r3,r16		; r3 has locks to perform
	rcall	applylocks	; jump in the pit
	pop	r5

nolck:	sez			; z set means ok
	set			; t set means msg recv
	
	pop	r3
	pop	r4
	pop	r16
	pop	r17
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	rdmmsg - read a mouse message into buf
;
;	r0 - mouse number
;	call rdmmsg
;	brne error	(1st byte bad or no init)
;	breq ok
;
;	full msg must already be in recv buffer,
;	so call msgrdy prior to calling this

rdmmsg:	push	ZH
	push	ZL
	push	r18
	push	r17
	push	r16
	push	r1

	rcall	moff2z		; pointer to moff()
	ldd	r17,Z+mo_msgsz	; message size
	tst	r17
	breq	noin		; not initialized

	adiw	ZH:ZL,mo_msg
	rcall	getmbyte	; header of msg
	breq	noin		; can't happen
	st	Z+,r1
	mov	r18,r1		; hold header out

	rcall	getmbyte	; byte 2 of msg
	breq	noin		; can't happen
	st	Z+,r1

	rcall	getmbyte	; byte 3 of msg
	breq	noin		; can't happen
	st	Z+,r1		; ignore error rtn
	
	clr	r1		; assume 3 byte msg
	cpi	r17,4
	brne	no4
	rcall	getmbyte	; 4th byte into r1
	breq	noin		; can't happen
no4:	st	Z+,r1		; byte 4 (or 0) of msg

	andi	r18,0x08	; header byte
	breq	noin		; bit 3 must be set
	sez			; zero set returns OK
	rjmp	bnn
noin:	clz			; zero clear rtns error

bnn:	pop	r1
	pop	r16
	pop	r17
	pop	r18
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	pktrdy - has our mouse queued a packet?
;
;	r0 - mouse to check
;	call pktrdy
;	  breq not ready
;	  brne packet ready (decrements counter, so
;	    caller has to go ahead and grab the packet)

pktrdy:	push	YH
	push	YL
	push	r18

	rcall	pack2y
	ldd	r18,Y+pready
	tst	r18
	breq	nopk			; zero is none

	cli				; no interrupts while
	ldd	r18,Y+pready		; we update this counter
	dec	r18
	std	Y+pready,r18		; update count
	sei				; all done - interrupts on
	clz				; zero clear means OK

nopk:	pop	r18
	pop	YL
	pop	YH
	ret

; ==============================================
;	#addmoff - add 9 bit number to a 24 bit number
;
;	Z - points to moff()
;	r16 - second & higher bytes (ext. sign)
;	r17 - low order byte
;	entry points:
;		xaddmoff - add to x location
;		yaddmoff - add to y location
;		zaddmoff - add to z location

xaddmoff:
	push	ZH
	push	ZL
	adiw	ZH:ZL,mo_x0
	rjmp	xad
yaddmoff:
	push	ZH
	push	ZL
	adiw	ZH:ZL,mo_y0
	rjmp	xad
zaddmoff:
	push	ZH
	push	ZL
	adiw	ZH:ZL,mo_z0

xad:	push	r18
	ld	r18,Z			; low order
	add	r18,r17
	st	Z+,r18
	ld	r18,Z			; 2nd byte
	adc	r18,r16
	st	Z+,r18
	ld	r18,Z			; high byte
	adc	r18,r16
	st	Z,r18			; Z+ if more

	pop	r18
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	setrefpnt - zero out the reference location
;
;	r0 - mouse in question (preserved)
;	zeroes out the 9 bytes of location data
;	and the overflow bits

setrefpnt:
	push	ZH
	push	ZL
	push	r0
	push	r16

	rcall	moff2z
	ldd	r16,Z+mo_but
	andi	r16,0x3f	; drop ovrflo bits
	std	Z+mo_but,r16

	adiw	ZH:ZL,mo_x0	; low byte of x
	clr	r0
	ldi	r16,9		; next 9 bytes are we
onwg1:	st	Z+,r0
	dec	r16
	brne	onwg1

	pop	r16
	pop	r0
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	initmouse - initialize mouse r0
;
;	mouse number in r0 (preserved)
;	returns: zero set on error, cleared if OK

initmouse:
	push	ZH
	push	ZL
	push	YH
	push	YL
	push	r16
	push	r1

	rcall	pack2y				; Y -> pack
	clr	r16
	std	y+perr,r16			; we're doing initialization
	rcall	zerobt				; kill the transition stuff

	rcall	moff2z				; hit it with a stick
	call	killmouse			; turn off interrupts
	ldi	r16,50				; wait a little while (50ms)
	rcall	starttimer			; to let things settle down
winit:	rcall	gettimer			; wait for timeout
	breq	winit

	clr	r1
	std	Z+mo_hd,r1			; set buffer empty
	std	Z+mo_t,r1			; tail of list
	std	Z+mo_sts,r1			; line status is idle
	std	Z+mo_err,r1			; clear error flag

	ldi	r16,packsize			; zero out the packet stuff
wii:	st	Y+,r1
	dec	r16
	brne	wii

	rcall	clock2input			; probably unnecessary, but...
	rcall	data2input
	clc					; need to write a 0 to
	rcall	writedata			; make sure pullups are off
	rcall	revivemouse			; bring up its interrupts

	ldi	YH,high(iseq1<<1)		; send 3 resets
	ldi	YL,low(iseq1<<1)
	rcall	mdialog
	tst	r1
	brne	iee

	ldi	YH,high(iseq2<<1)		; check for scroll mouse
	ldi	YL,low(iseq2<<1)
	rcall	mdialog
	tst	r1
	brne	iee

	; we're not doing the intellimouse check

	ldi	YH,high(iseq3<<1)		; set parameters and enable
	ldi	YL,low(iseq3<<1)
	rcall	mdialog
	tst	r1
	breq	eok

;	initialization failed - kill off the mouse
iee:	clr	r1
	std	Z+mo_msgsz,r1			; it's unitialized
	rcall	killmouse			; disable interrupts
	sez					; zero set is bad
	rjmp	irtn

;	initialization succeeded - set up packet management
eok:	rcall	pack2y				; Y -> packet stuff
	ldd	r16,Z+mo_msgsz			; message size (3 or 4)
	std	Y+psize,r16			; save in packet info
	cpi	r16,3
	breq	msg3
	lds	r16,ptval4			; the 4 byte packet timer
	rjmp	putms
msg3:	lds	r16,ptval3			; the 3 byte packet timer
putms:	std	Y+ptval,r16			; save this timer value
	ldi	r16,1
	std	Y+psts,r16			; set line to packet idle
	clz					; zero cleared is success

irtn:	pop	r1
	pop	r16
	pop	YL
	pop	YH
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	mdialog - engage a chat with a mouse
;
;	r0 - mouse to speak with
;	Z  - points at moff() for this mouse
;	Y  - points at message (Y advances):
;		s - send next char (2 hex digits) to mouse
;		r - wait to receive following seq from mouse (1 to 3, 2 hex digits)
;		i - receive and define ID (00 or 03)
;		y - check previously received ID (00 or 03)
;		x - stop
;	returns:
;		r1 = 0  OK
;		   = 1  mouse timeout (character level or message level)
;		   = 2  reply didn't match expected
;		   = 3  error (FE) from mouse
;		   = 4  bad msg format
;		   = 5  mouse ID mismatch
;
;	mouse shouldn't be talking to us when we start this

mdialog:
	push	r2
	push	r5
	push	r16
	push	r17
	push	r18
	push	r19

	clr	r17			; =0, send, =1, receive
mdlp:	rcall	nextup			; get next char in string
	push	r1			; returned val = arg
	rcall	jumptable		; transfer to whomever
	rjmp	srx			; 0 = hex value in r2
	rjmp	sw_s			; 1 = 's'
	rjmp	sw_r			; 2 = 'r'
	rjmp	xquit			; 3 = 'x'
	rjmp	sw_idr			; 4 = 'i'
	rjmp	sw_idc			; 5 = 'y'
	rjmp	sw_pause		; 6 = 'w'
	rjmp	xerr			; 7 = error

xerr:	ldi	r16,4			; bad message format
xerr2:	mov	r1,r16
	rjmp	xqret

;	------------ switch to receive
sw_r:	ldi	r17,1			; switch to receive
	rjmp	mdlp

;	------------ switch to send
sw_s:	clr	r17			; switch to send
	rjmp	mdlp

;	------------ read ID and compare to stored
sw_idc:	tst	r17			; read ID and compare it
	breq	xerr			; must be in receive mode
	ldd	r2,Z+mo_id		; previously saved ID
	rjmp	srx2			; go receive it hopefully

;	------------ read ID and store it
sw_idr:	tst	r17			; read ID and store it
	breq	xerr			; must be in receive mode
	ldi	r18,3			; r18 = 3: special read and store
	rjmp	srx1

;	------------ we have a hex value in r2
srx:	tst	r17			; send or receive hex value in r2
	breq	xsndx			; jump if sending

srx2:	mov	r5,r2			; the data we're expecting
	clr	r18			; r18 = 0: read and compare

srx1:	ldd	r16,Z+mo_tmode		; is the timer running?
	tst	r16			; if yes, don't start it
	brne	arx1
	ldi	r16,128+8		; otherwise, two seconds to hear back
	rcall	starttimer
arx1:	rcall	getmbyte		; anything from mr. mouse?
	brne	got1			; jump if yes, data in r1

	rcall	gettimer		; timeout? (our 2 sec or a recv err)
	breq	arx1			; no error - try again
	ldi	r16,1			; error stopped timer. return a 1
	rjmp	xerr2	

got1:	rcall	stoptimer		; made it!
	mov	r16,r1
	cpi	r16,0xfe		; mouse's error code
	brne	got2
	ldi	r16,3
	rjmp	xerr2

got2:	tst	r18			; looking for id byte?
	breq	srnorm			; jump if no

	; we just read an ID and need to save it (if it's 00 or 03)
	; and set the message size (00 = 3, 03 = 4)
	ldi	r19,3			; assume id = 00
	tst	r1			; read id: 00 or 03 OK
	breq	idgood
	cp	r1,r18			; r18 has a 3 in it
	brne	xsrbad
	ldi	r19,4			; it's a scroll mouse

idgood:	std	Z+mo_id,r1		; save ID
	std	Z+mo_msgsz,r19		; save message size
mdlpw:	rjmp	mdlp

srnorm:	cp	r1,r5			; have vs. want
	breq	mdlpw			; ok, so go on...

xsrbad:	ldi	r16,2			; didn't match
	rjmp	xerr2

;	we need to send out the hex value in r2
xsndx:	mov	r1,r2
xss:	call	sendmouse
	breq	xssc
	rcall	gettimer
	breq	xss			; try again
	rjmp	xssd
xssc:	rcall	gettimer		; check for timeout
	breq	mdlpw			; no error - next please
xssd:	ldi	r16,1			; timeout error
	rjmp	xerr2

;	------------ spare
sw_pause:
	rjmp	mdlp

;	------------ enough is enough - quit
xquit:	clr	r1			; normal termination

xqret:	pop	r19
	pop	r18
	pop	r17
	pop	r16
	pop	r5
	pop	r2
	ret

; ==============================================
;	nextup - decode a predefined mouse communication string
;
;	Y - points to next character in string (Y is advanced)
;	returns (in r1):
;		0  hex value in r2
;		1  's'
;		2  'r'
;		3  'x'
;		4  'i'
;		5  'y'
;		6  'w'
;		7  error (incomplete hex or unrecognized char)

nextup:	push	r16
	push	ZH
	push	ZL

	mov	ZH,YH
	mov	ZL,YL
	lpm	r16,Z+				; get the dog
	adiw	YH:YL,1
;	push	r16				; debug echo =====
;	rcall	xmit_put
	rcall	convhex16			; try to convert to hex
	brne	hx				; if hex, zero clear
	clr	r1				; becomes the return value
	push	r16				; arg to looktable
	rcall	looktable
	.dw	's',pl_1
	.dw	'r',pl_2
	.dw	'x',pl_3
	.dw	'i',pl_4
	.dw	'y',pl_5
	.dw	'w',pl_6
	.dw	'S',pl_1
	.dw	'R',pl_2
	.dw	'X',pl_3
	.dw	'I',pl_4
	.dw	'Y',pl_5
	.dw	'W',pl_6
	.dw	-1,pl_7

hx:	mov	r2,r16				; the converted value
	swap	r2				; move into high nibble
	lpm	r16,Z+				; get next character
	adiw	YH:YL,1
;	push	r16				; debug echo ======
;	rcall	xmit_put
	rcall	convhex16
	breq	bdhex				; zero set is bad
	or	r2,r16				; this is the low half
	clr	r1
	rjmp	pl_0

bdhex:	clr	r1
pl_7:	inc	r1
pl_6:	inc	r1
pl_5:	inc	r1
pl_4:	inc	r1
pl_3:	inc	r1
pl_2:	inc	r1
pl_1:	inc	r1

pl_0:	pop	ZL
	pop	ZH
	pop	r16
	ret
