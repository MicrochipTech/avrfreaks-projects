;	File name: AT3283_2.inc
;
;	Command Language Processor
;
;
; ==============================================
;	monster - main command processing
;
;	no registers preserved

monster:
	rcall	miscinit	; various initializations

mloop:	clr	r0		; mouse 0
	; go grab any incoming messages from the mousies
	rcall	procmsg		; mouse 0 chk msgs
	inc	r0
	rcall	procmsg		; mouse 1 chk msgs
	inc	r0
	rcall	procmsg		; mouse 2 chk msgs

	lds	r0,bigcheese	; active mouse
	rcall	checkcmd	; anything from host?
	tst	r1
	brpl	goodcmd		; yes, and complete
	sbrc	r1,6
	rjmp	mloop		; buffer's empty
	sbrc	r1,5
	rjmp	mloop		; command is incomplete
	rcall	recv_get	; unknown - dump it
	rjmp	sendnak		; and squawk

goodcmd:
	push	r1		; the command number
	rcall	jumptable
	rjmp	cmd_0		; '0' - 1 byte
	rjmp	cmd_1		; '1' - 1 byte
	rjmp	cmd_2		; '2' - 1 byte
	rjmp	cmd_3		; 'B' - 1 byte
	rjmp	cmd_4		; 'D' - 2 bytes
	rjmp	cmd_5		; 'd' - 3 bytes
	rjmp	cmd_6		; 'L' - 5 bytes
	rjmp	cmd_7		; 'P' - 2 bytes
	rjmp	cmd_8		; 'R' - 2 bytes
	rjmp	cmd_9		; 'r' - 2 bytes
	rjmp	cmd_10		; 'S' - 1 byte
	rjmp	cmd_11		; 'W' - 3 bytes
	rjmp	cmd_12		; 'w' - 3 bytes
	rjmp	cmd_13		; 'X' - 1 byte
	rjmp	cmd_14		; 'x' - 1 byte
	rjmp	cmd_15		; 'Y' - 1 byte
	rjmp	cmd_16		; 'y' - 1 byte
	rjmp	cmd_17		; 'Z' - 1 byte
	rjmp	cmd_18		; 'z' - 1 byte
	rjmp	cmd_19		; 'I' - 1 byte
	rjmp	cmd_20		; 'e' - 1 byte
	rjmp	cmd_21		; 'E' - 1 byte
	rjmp	cmd_22		; 'p' - 3 bytes
	rjmp	cmd_23		; 'Q' - 2 bytes
	rjmp	cmd_24		; 'q' - 3 bytes
	rjmp	cmd_25		; 'U' - 2 bytes
	rjmp	cmd_26		; 'V' - 1 byte
	rjmp	cmd_27		; 'A' - 1 byte
	rjmp	cmd_28		; 'a' - 1 byte
	rjmp	cmd_29		; 'T' - 2 bytes
	rjmp	cmd_30		; 't' - 2 bytes
	rjmp	cmd_31		; '*' - 1 byte

sendnak:			; tell them a sad story
	ldi	r16,nak
snd2:	push	r16		; it's an arg
	rcall	xmit_put	; give it to them
	lds	r16,echo232	; are we echoing?
	tst	r16
	brne	ssd
	rjmp	mloop		; relative branch out of range

ssd:	ldi	r16,13		; yes - send CRLF
	push	r16
	rcall	xmit_put
	ldi	r16,10		; LF
	push	r16
	rcall	xmit_put
	rjmp	mloop

sendack:			; tell them a happy story
	ldi	r16,ack
	rjmp	snd2			; it's an arg

;	--------------------------------
cmd_0:	clr	r16			; '0' 1 byte: set mouse 0
cmd_0a:	sts	bigcheese,r16
	rjmp	sendack
;	--------------------------------
cmd_1:	ldi	r16,1			; '1' 1 byte: set mouse 1
	rjmp	cmd_0a
;	--------------------------------
cmd_2:	ldi	r16,2			; '2' 1 byte: set mouse 2
	rjmp	cmd_0a
;	--------------------------------
cmd_3:	rcall	chkmouse		; 'B' 1 byte: read switches
	breq	sendnak
	
	rcall	moff2z
	ldd	r16,Z+mo_but		; buttons and overflows
	mov	r1,r16			; save. YX---MRL is order
	lsr	r16			; building -----LMR
	andi	r16,0x03		; keep MR
	sbrc	r1,0
	sbr	r16,0x04		; put L back in

	rcall	bin2hex
	push	r16			; arg to xmit_put
	rcall	xmit_put
	rjmp	sendack
;	--------------------------------
;	for output, set DDR <- 1
cmd_4:	rcall	prepport		; 'D' 2 bytes: DPort - port to output
	breq	sendnak4		; zero set means port error

	lds	XL,pindef+w_ddr		; DDR address for this port
	clr	XH			; they're small addresses
	ser	r16
	st	X,r16			; all bits set
	rjmp	sendack
;	--------------------------------
;	for output, set DDR <- 1
cmd_5:					; 'd' 3 bytes: dPortPin - set pin to output
	lds	r1,cmdbuf+1		; 2nd byte is port
	lds	r2,cmdbuf+2		; 3rd byte is pin
	rcall	unfold2			; convert to something useful
	breq	sendnak4		; shame on your port and pin

	lds	XL,pindef+w_ddr		; DDR address for this port
	clr	XH			; they're small addresses
	lds	r17,pindef+w_or		; the OR mask (force one bit)
	ld	r16,X			; read DDRx
	or	r16,r17			; add in our bit
	st	X,r16			; put it back with bit
	rjmp	sendack
;	--------------------------------
sendnak4:				; relative jump out of reach
	rjmp	sendnak
;	--------------------------------
cmd_6:	rcall	chkmouse		; 'L' 5 bytes: LBtnPortPinMode
	breq	sendnak4

	lds	r18,cmdbuf+4		; 5th byte is mode
	cpi	r18,'0'			; lower limit
	brcs	sendnak4
	cpi	r18,'5'			; upper limit
	breq	cmd_6c
	brcc	sendnak4
cmd_6c:	andi	r18,0x0f		; convert ascii to binary

	lds	r1,cmdbuf+2		; 3rd byte is port
	lds	r2,cmdbuf+3		; 4th byte is pin
	rcall	unfold2			; get some addresses in pindef
	breq	sendnak4

	lds	r16,cmdbuf+1		; button
	rcall	but2lock		; get Z -> lock block
	breq	sendnak4		; bad button designation

	ldi	r16,5			; 5 bytes in pindef
	ldi	XH,high(pindef)
	ldi	XL,low(pindef)

cmd_6b:	ld	r17,X+			; copy pindef() into locks
	st	Z+,r17
	dec	r16
	brne	cmd_6b
	st	Z,r18			; and the mode (follows lock info)

	rcall	moff2z			; go apply the locks now
	ldd	r18,Z+mo_but		; buttons and overflows
	andi	r18,0x07		; button bits only
	mov	r4,r18
	clr	r3
	inc	r3			; r3 = 1 means follow only
	rcall	applylocks
	rjmp	sendack
;	--------------------------------
;	for input with pullup, DDR <- 0 then PORT <- 1
cmd_7:	rcall	prepport		; 'P' 2 bytes: port input with pullup
	breq	sendnak2		; zero set means port error

	lds	XL,pindef+w_ddr		; DDR address for this port
	clr	XH			; they're small addresses
	st	X,XH			; zero the DDR
	ser	r16
	lds	XL,pindef+w_port	; the PORT address
	st	X,r16			; all bits set
	rjmp	sendack
;	--------------------------------
cmd_8:	rcall	rdport			; 'R' 2 bytes: RPort - read high nibble
	breq	sendnak2
	swap	r16
	rjmp	cmd_9a
;	--------------------------------
cmd_9:	rcall	rdport			; 'r' 2 bytes: rPort - read low nibble
	breq	sendnak2
cmd_9a:	andi	r16,0x0f		; keep low order bits
	rcall	bin2hex			; convert to ascii hex
	push	r16
	rcall	xmit_put		; give them hex digit
	rjmp	sendack
;	--------------------------------
cmd_10:	rcall	chkmouse		; 'S' 1 byte: set origin
	breq	sendnak2

	rcall	setrefpnt
	rcall	zerobt
	rjmp	sendack
;	--------------------------------
sendnak2:				; relative jump out of reach
	rjmp	sendnak
;	--------------------------------
cmd_11:					; 'W' 3 bytes: WPortPin - Write 1 to pin
	lds	r1,cmdbuf+1		; 2nd byte is port
	lds	r2,cmdbuf+2		; 3rd byte is pin
	rcall	unfold2			; convert to something useful
	breq	sendnak2		; shame on your port and pin
	lds	XL,pindef+w_port	; PORT address for this port
	clr	XH			; they're small addresses
	lds	r17,pindef+w_or		; the OR mask (force one bit)
	ld	r16,X			; read PORTx
	or	r16,r17			; add in our bit
	st	X,r16			; put it back with bit set
	rjmp	sendack
;	--------------------------------
cmd_12:					; 'w' 3 bytes: wPortPin - write 0 to pin
	lds	r1,cmdbuf+1		; 2nd byte is port
	lds	r2,cmdbuf+2		; 3rd byte is pin
	rcall	unfold2			; convert to something useful
	breq	sendnak2		; shame on your port and pin
	lds	XL,pindef+w_port	; PORT address for this port
	clr	XH			; they're small addresses
	lds	r17,pindef+w_and	; the AND mask (drops one bit)
	ld	r16,X			; read PORTx
	and	r16,r17			; drop our bit
	st	X,r16			; put it back with bit reset
	rjmp	sendack
;	--------------------------------
cmd_13:	rcall	chkmouse		; 'X' 1 byte: read current X offset
	breq	sendnak2

	ldi	r16,mo_x0
c_13a:	rcall	moff2z
	add	ZL,r16
	brcc	c_13b
	inc	ZH
c_13b:	rcall	out3bytenum
	rjmp	sendack
;	--------------------------------
cmd_14:	rcall	chkmouse		; 'x' 1 byte: read saved X offset
	breq	sendnak3
	
	clr	r16			; x is first
	rjmp	savsend
;	--------------------------------
cmd_15:	rcall	chkmouse		; 'Y' 1 byte: read current Y offset
	breq	sendnak3

	ldi	r16,mo_y0
	rjmp	c_13a
;	--------------------------------
cmd_16:	rcall	chkmouse		; 'y' 1 byte: read saved Y offset
	breq	sendnak3

	ldi	r16,3			; y is second (3 bytes in)
	rjmp	savsend
;	--------------------------------
sendnak3:				; relative jump out of reach
	rjmp	sendnak
;	--------------------------------
cmd_17:	rcall	chkmouse		; 'Z' 1 byte: read current Z offset
	breq	sendnak3

	ldi	r16,mo_z0
	rjmp	c_13a
;	--------------------------------
cmd_18:	rcall	chkmouse		; 'z' 1 byte: read saved Z offset
	breq	sendnak3

	ldi	r16,6			; z is third (6 bytes in)
	rjmp	savsend
;	--------------------------------
cmd_19:	rcall	initmouse		; 'I' 1 byte: init mouse
	breq	pass2			; didn't work
	rjmp	sendack

;	it didn't take on first try.  wait 5 ms, and try again
pass2:	ldi	r16,5
	sts	take5,r16		; special timer
wt5:	lds	r16,take5
	tst	r16	
	brne	wt5			; wait until it goes to zero
	rcall	initmouse		; try again, once
	breq	sendnak9
	rjmp	sendack
;	--------------------------------
cmd_20:	clr	r16			; 'e' 1 byte: echo off
c_20a:	sts	echo232,r16
	rjmp	sendack
;	--------------------------------
sendnak9:
	rjmp	sendnak			; relative branch out of reach
;	--------------------------------
cmd_21:	ser	r16			; 'E' 1 byte: echo on
	rjmp	c_20a
;	--------------------------------
;	for input with pullup, DDR <- 0 then PORT <- 1
cmd_22:					; 'p' 3 bytes: pin input, pullup
	lds	r1,cmdbuf+1		; 2nd byte is port
	lds	r2,cmdbuf+2		; 3rd byte is pin
	rcall	unfold2			; convert to something useful
	breq	sendnak3		; shame on your port and pin

	lds	XL,pindef+w_ddr		; DDR address for this port
	clr	XH			; they're small addresses
	lds	r17,pindef+w_and	; the AND mask (drop a bit)
	ld	r16,X			; read DDRx
	and	r16,r17			; kill our bit
	st	X,r16			; put it back

	lds	XL,pindef+w_port	; PORT address for this port
	ld	r16,X			; read PORTx
	lds	r17,pindef+w_or		; the OR mask
	or	r16,r17			; kill our bit
	st	X,r16			; put it back
	rjmp	sendack
;	--------------------------------
;	for input without pullup, DDR <- 0 then PORT <- 0
cmd_23:	rcall	prepport		; 'Q' 2 bytes: port input, no pullup
	breq	sendnak9		; zero set means port error

	lds	XL,pindef+w_ddr		; DDR address for this port
	clr	XH			; they're small addresses
	st	X,XH			; zero the DDR
	lds	XL,pindef+w_port	; the PORT address
	st	X,XH			; zero the PORT
	rjmp	sendack
;	--------------------------------
;	for input without pullup, DDR <- 0 then PORT <- 0
cmd_24:	lds	r1,cmdbuf+1		; 'q' 3 bytes: pin input, no pullup
	lds	r2,cmdbuf+2		; 2nd port, 3rd byte pin
	rcall	unfold2			; convert to something useful
	breq	sendnak9		; shame on your port and pin

	lds	XL,pindef+w_ddr		; DDR address for this port
	clr	XH			; they're small addresses
	lds	r17,pindef+w_and	; the AND mask (drop a bit)
	ld	r16,X			; read DDRx
	and	r16,r17			; kill our bit
	st	X,r16			; put it back

	lds	XL,pindef+w_port	; PORT address for this port
	ld	r16,X			; read PORTx
	and	r16,r17			; kill our bit
	st	X,r16			; put it back
	rjmp	sendack
;	--------------------------------
cmd_25:	rcall	chkmouse		; 'U' 2 bytes: unlock button
	breq	sendnak5

	lds	r16,cmdbuf+1
	rcall	but2lock		; get Z -> lock block
	breq	sendnak5		; bad button designation

	clr	r16			; new mode is 0
	std	Z+w_mode,r16
	rjmp	sendack
;	--------------------------------
cmd_26:	ldi	ZH,high(version<<1)	; 'V' 1 byte: version number
	ldi	ZL,low(version<<1)
	lpm	r16,Z+
	push	r16
	rcall	xmit_put
	lpm	r16,Z+
	push	r16
	rcall	xmit_put
	rjmp	sendack
;	--------------------------------
cmd_27:	ser	r16			; 'A' 1 byte: suppression off
c27a:	sts	abbrev,r16
	rjmp	sendack
;	--------------------------------
cmd_28:	clr	r16			; 'a' 1 byte: suppression on
	rjmp	c27a
;	--------------------------------
cmd_29:	rcall	rdportp			; 'T' 2 bytes: TPort - read high nibble
	breq	sendnak5
	swap	r16
	rjmp	c_30a
;	--------------------------------
cmd_30:	rcall	rdportp			; 't' 2 bytes: tPort - read low nibble
	breq	sendnak5
c_30a:	andi	r16,0x0f		; keep low order bits
	rcall	bin2hex			; convert to ascii hex
	push	r16
	rcall	xmit_put		; give them hex digit
	rjmp	sendack
;	--------------------------------
cmd_31:	rcall	moff2z			; '*' 1 byte: send error report
	rcall	pack2y
	ldd	r16,Z+mo_but		; buttons and overflows
	mov	r17,r16
	andi	r16,0xc0		; keep YX overflow flags
	andi	r17,0x3f		; drop the overflow flags
	std	Z+mo_but,r17		; keep buttons asis

	ldd	r17,Y+preject		; rejected packet count
	andi	r17,0x3f		; not really necessary
	or	r17,r16			; r17 now has YX######
	clr	r16
	std	Y+preject,r16		; clear the error count

	mov	r16,r17
	swap	r16			; high order first
	andi	r16,0x0f
	rcall	bin2hex
	push	r16			; arg to xmit_put
	rcall	xmit_put		; send it out

	mov	r16,r17			; now do low order
	andi	r16,0x0f
	rcall	bin2hex
	push	r16			; arg to xmit_put
	rcall	xmit_put
	rjmp	sendack	

;	--------------------------------
sendnak5:				; relative jump out of reach
	rjmp	sendnak

;	--------------------------------
;	process x, y, and z commands (not a sub)
;	r16 has 0, 3 or 6 in it for x, y, or z
;	r0 has mouse in it
savsend:
	ldi	ZH,high(btsav)
	ldi	ZL,low(btsav)
	mov	r17,r0
	cpi	r17,1			; which mouse?
	breq	ms1
	brcs	ms0
	adiw	ZH:ZL,btsavsize		; 54 bytes
ms1:	adiw	ZH:ZL,btsavsize
ms0:	lds	r17,cmdbuf+1		; LMR or lmr
	push	r17
	rcall	looktable
	.dw	'r',mms5
	.dw	'R',mms4
	.dw	'm',mms3
	.dw	'M',mms2
	.dw	'l',mms1
	.dw	'L',mms0
	.dw	-1,sendnak

mms5:	adiw	ZH:ZL,btblksz		; 'r' is the 6th block
mms4:	adiw	ZH:ZL,btblksz		; 'R' is the 5th block
mms3:	adiw	ZH:ZL,btblksz		; 'm' is the 4th block
mms2:	adiw	ZH:ZL,btblksz		; 'M' is the 3rd block
mms1:	adiw	ZH:ZL,btblksz		; 'l' is the 2nd block
mms0:	add	ZL,r16			; 'L' is 1st. add x, y, or z
	brcc	mmsa
	inc	ZH

mmsa:	rcall	out3bytenum
	rjmp	sendack

; ==============================================
;	chkmouse - see if mouse is initialized
;
;	r0 - mouse in question
;	returns: zero set if not, cleared if OK

chkmouse:
	push	ZH
	push	ZL
	push	r1

	rcall	moff2z
	ldd	r1,Z+mo_msgsz
	tst	r1

	pop	r1
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	but2lock - convert button letter to lock addr
;
;	r0 - mouse number
;	r16 has ascii button ('L', 'M', or 'R')
;	returns: Z pointing at btn's lock block
;	         zero flag set if error

but2lock:
	rcall	moff2z
	cpi	r16,'L'
	breq	b25_1
	cpi	r16,'M'
	breq	b25_2
	cpi	r16,'R'
	brne	b25_3

	adiw	ZH:ZL,mo_rlock		; right locks
	rjmp	b25_a
b25_1:	adiw	ZH:ZL,mo_llock		; left locks
	rjmp	b25_a
b25_2:	adiw	ZH:ZL,mo_mlock		; middle locks
b25_a:	clz				; clear zero = ok
	ret

b25_3:	sez				; didn't recognize
	ret

; ==============================================
;	followlocks - apply follow locks for this guy
;
;	Y - points at lock definition block
;	r16 - bit 0 - current state of button

followlocks:
	push	r17
	push	r18
	push	r19
	push	XH
	push	XL

	ldd	XL,Y+w_port
	clr	XH			; X points at our PORT
	ld	r19,X			; read the port

	ldd	r17,Y+w_mode		; grab mode of lock
	cpi	r17,1			; 1 = follow, 2 = invert
	breq	mf1			; jump if follow
	cpi	r17,2			; check for invert
	brne	ffrtn			; no - return

	tst	r16			; it's an invert
	breq	ff1			; jump to put out a 1
ff2:	ldd	r18,Y+w_and		; write 0 - get AND mask
	and	r19,r18			; drop our bit
	rjmp	ff3

mf1:	tst	r16			; it's a follow
	breq	ff2			; jump to put out a 0
ff1:	ldd	r18,Y+w_or		; write 1 - get OR mask
	or	r19,r18			; turn it on
ff3:	st	X,r19			; put it back

ffrtn:	pop	XL
	pop	XH
	pop	r19
	pop	r18
	pop	r17
	ret

; ==============================================
;	togglelocks - apply toggle locks for this guy
;
;	Y - points at lock definition block
;	r16 - bit 0 - current state of button
;	r17 - bit 0 - whether it's changed

togglelocks:
	tst	r17
	breq	tl_1			; no change = done

	push	r18
	ldd	r18,Y+w_mode		; grab mode of lock
	cpi	r18,3			; ignore modes 1 & 2
	brcs	tl_3
	push	XH
	push	XL
	push	r19

	cpi	r18,4			; our modes are 3 to 5
	brcs	togit			; 3 = every chg toggles
	breq	z21			; 4 = on 0 to 1 

	tst	r16			; 5 = on 1 to 0
	breq	togit
	rjmp	tl_2

z21:	tst	r16			; toggle if r16 = 1
	breq	tl_2

;	time to toggle our bit
togit:	ldd	XL,Y+w_port
	clr	XH			; X points at our PORT
	ld	r19,X			; read the port
	ldd	r18,Y+w_or		; get the OR mask
	eor	r19,r18			; flip that dog
	st	X,r19			; home, boy

tl_2:	pop	r19
	pop	XL
	pop	XH
tl_3:	pop	r18
tl_1:	ret

; ==============================================
;	applylocks - apply any locks for this guy, part 1
;
;	r0 - mouse number
;	r3 - if bit 0 set, apply modes 1 & 2 (follow & invert)
;	     if bit 1 set, apply modes 3 - 5 (toggles)
;	r4 - current button state (bit 2 = M, 1 = R, 0 = L)
;	r5 - change bits (also MRL, only used for toggles)

applylocks:
	push	YH
	push	YL
	push	ZH
	push	ZL
	push	r16
	push	r17

	rcall	moff2z
;	see if the left button has a lock
	ldd	r16,Z+mo_llock+w_mode
	tst	r16
	breq	nlbtn			; mode = 0 means no locks

	mov	YH,ZH
	mov	YL,ZL
	adiw	YH:YL,mo_llock		; Y points to left lock defs
	mov	r16,r4			; extract current btn bit
	andi	r16,0x01		; L bit's at 0

	sbrs	r3,0			; do we want follow locks?
	rjmp	nofl1			; jump if no
	rcall	followlocks		; go do them

nofl1:	sbrs	r3,1			; do we want toggle locks?
	rjmp	nlbtn			; jump if no

	mov	r17,r5			; extract change bit
	andi	r17,0x01
	rcall	togglelocks

;	see if the middle button has a lock
nlbtn:	ldd	r16,Z+mo_mlock+w_mode
	tst	r16
	breq	nmbtn			; mode = 0 means no locks

	mov	YH,ZH
	mov	YL,ZL
	adiw	YH:YL,mo_mlock		; Y points to middle lock defs
	mov	r16,r4			; extract current btn bit
	lsr	r16			; M bit's at 2
	lsr	r16
	andi	r16,0x01		; now at 0

	sbrs	r3,0			; do we want follow locks?
	rjmp	nofl2			; jump if no
	rcall	followlocks		; go do them

nofl2:	sbrs	r3,1			; do we want toggle locks?
	rjmp	nmbtn			; jump if no

	mov	r17,r5			; extract change bit
	lsr	r17
	lsr	r17
	andi	r17,0x01
	rcall	togglelocks
		
;	see if the right button has a lock
nmbtn:	ldd	r16,Z+mo_rlock+w_mode
	tst	r16
	breq	nrbtn			; mode = 0 means no locks

	mov	YH,ZH
	mov	YL,ZL
	adiw	YH:YL,mo_rlock		; Y points to right lock defs
	mov	r16,r4			; extract current btn bit
	lsr	r16			; R bit's at 1
	andi	r16,0x01		; now at 0

	sbrs	r3,0			; do we want follow locks?
	rjmp	nofl3			; jump if no
	rcall	followlocks		; go do them

nofl3:	sbrs	r3,1			; do we want toggle locks?
	rjmp	nrbtn			; jump if no

	mov	r17,r5			; extract change bit
	lsr	r17
	andi	r17,0x01
	rcall	togglelocks

nrbtn:	pop	r17
	pop	r16
	pop	ZL
	pop	ZH
	pop	YL
	pop	YH
	ret

; ==============================================
;	prepport - get addresses for port oriented cmds
;
;	port is in cmdbuf + 1
;	returns zero flag set for bad port
;	uses r1, r2, & r16

prepport:
	lds	r1,cmdbuf+1	; 2nd byte is port
	ldi	r16,'0'
	mov	r2,r16		; any bit will do
	rcall	unfold2		; get some addresses
	ret

; ==============================================
;	rdportp - read port PORT data
;
;	port is in cmdbuf + 1
;	returns:
;	  zero flag set if error (breq)
;	  r16 has port data otherwise

rdportp:
	rcall	prepport
	breq	rdbye2			; zero set means port error
	lds	XL,pindef+w_port	; PORT address for this port
	clr	XH			; they're small addresses
	ld	r16,X			; read PORTx
	clz				; zero reset means ok
rdbye2:	ret

; ==============================================
;	rdport - read port PIN data
;
;	port is in cmdbuf + 1
;	returns:
;	  zero flag set if error (breq)
;	  r16 has port data otherwise

rdport:	rcall	prepport
	breq	rdbye		; zero set means port error
	lds	XL,pindef+w_pin	; PIN address for this port
	clr	XH		; they're small addresses
	ld	r16,X		; read PINx
	clz			; zero reset means ok
rdbye:	ret

; ==============================================
;	pullcmd	- read a command into cmdbuf
;	r19 - number of bytes
;
;	we've been assured by reliable sources the bytes are there

pullcmd:
	push	r19
	push	r0
	push	ZH
	push	ZL

	ldi	ZH,high(cmdbuf)
	ldi	ZL,low(cmdbuf)
pulp:	rcall	recv_get
	st	Z+,r0
	dec	r19
	brne	pulp

	pop	ZL
	pop	ZH
	pop	r0
	pop	r19
	ret

; ==============================================
;	checkcmd - see if there's a complete command in the buffer
;
;	call checkcmd
;	  tst r1
;	  brpl ok    r1 has offset of complete command (0 - ?)
;	r1 bit 7 set (negative) means not a complete command:
;	  sbr- r1,6  r1 = 0xcf  bit 6 = 1 means empty
;	  sbr- r1,5  r1 = 0xaf	bit 5 = 1 means incomplete
;	  sbr- r1,4  r1 = 0x9f  bit 4 = 1 means unknown
;
;	if the command is complete, it is read into cmdbuf() and
;	removed from the receive buffer

checkcmd:
	push	r16
	push	r17

	lds	r16,rhead		; see if buffer has anything
	lds	r17,rtail
	cp	r16,r17			; equal means empty (usually true)
	brne	chk1

	ldi	r16,0xcf		; 11001111 means empty
	mov	r1,r16
	rjmp	chk2

chk1:	push	r18
	push	r19
	push	ZH
	push	ZL
	push	YH
	push	YL

	ldi	YH,high(rbuf)		; point y at recv buffer
	ldi	YL,low(rbuf)
	add	YL,r17			; add tail offset
	brcc	chk4
	inc	YH			; carried into high byte
chk4:	ld	r16,Y			; get command in buf

	; r16 has the incoming command. look down the crec list for
	; a match, noting how many bytes it needs (r19) and how far
	; down the list the match is (r1)
	clr	r1			; r1 is counting
	ldi	ZH,high(crec<<1)	; point to commands
	ldi	ZL,low(crec<<1)		; (2 bytes each)
chklp:	lpm	r18,Z+			; next cmd from flash
	lpm	r19,Z+			; and its byte count
	cpi	r18,'~'			; end of list
	breq	chk3			; if we hit it, a problem
	cp	r16,r18			; is we us?
	breq	chk5			; yup - go away
	inc	r1			; we're counting on you
	rjmp	chklp			; try again

chk5:	andi	r19,0x0f		; convert ascii byte cnt

	; see if there are enough chars for a complete command
	lds	r17,rhead		; head offset
	lds	r18,rtail		; tail offset
	sub	r17,r18			; head - tail
	brpl	chk7			; if positive, no wrap
	ldi	r18,rbufsize		; size of recv buffer
	add	r17,r18			; take care of wrap
chk7:	cp	r17,r19			; have vs. need
	brcs	chk8			; jump if incomplete

	rcall	pullcmd			; read the command
	rjmp	chk10

chk8:	ldi	r16,0xaf		; 10101111 means incomplete
	rjmp	chk9
chk3:	ldi	r16,0x9f		; 10011111 means unknown cmd
chk9:	mov	r1,r16

chk10:	pop	YL
	pop	YH
	pop	ZL
	pop	ZH
	pop	r19
	pop	r18
chk2:	pop	r17
	pop	r16
	ret

; ==============================================
;	savcur - save current location to btsav()
;
;	when a button changes, we save the current offset so the
;	host can read where the mouse was when a button was last
;	clicked or released.  consequently there are 2 save areas
;	per button (click & release), 3 buttons per mouse, and 3
;	mice, or 18 save areas at 9 bytes each (3 x, 3 y, & 3 z).
;
;	r0 - mouse number
;	Z - points to moff()
;	r17 - low 3 bits have btn chg bits; 1 if changed
;
;	we'll load the new msg hdr into r3:
;	  r3 tells us the type of transistion (H->L or L->H)
;	  r0 tells us how far into btsav() to offset
;	  Z tells us where to get our data to copy
;	  r17 tells us which place(s) in btsav() to copy to

savcur:	push	r3
	push	XH
	push	XL

	ldd	r3,Z+mo_msg		; new msg header
	ldi	XH,high(btsav)		; X -> mouse 0 save area
	ldi	XL,low(btsav)
	tst	r0
	breq	sm0a			; done if mouse 0
	sbrc	r0,1
	adiw	XH:XL,btsavsize		; mouse 2 only
	adiw	XH:XL,btsavsize		; mouse 1 or 2

;	X points to our save block, left button, H->L transition
;	now look at the three bits of r17 (MRL) to see whom to save
sm0a:	sbrs	r17,0			; bit 0 is left button
	rjmp	not_l
	push	XH			; left button changed
	push	XL			; X already points at left btn
	sbrc	r3,0			; look at new value
	adiw	XH:XL,btblksz		; if our r3 bit is set, it's L->H
	rcall	copybt
	pop	XL
	pop	XH

not_l:	sbrs	r17,1			; bit 1 is right button
	rjmp	not_r
	push	XH			; right button changed
	push	XL
	adiw	XH:XL,4*btblksz		; right button is 36 bytes in
	sbrc	r3,1			; look at new value
	adiw	XH:XL,btblksz		; if our r3 bit is set, it's L->H
	rcall	copybt
	pop	XL
	pop	XH

not_r:	sbrs	r17,2			; bit 2 is middle button
	rjmp	not_m
	adiw	XH:XL,2*btblksz		; M btn changed; 18 bytes in
	sbrc	r3,2			; look at new value
	adiw	XH:XL,btblksz		; if our r3 bit is set, it's L->H
	rcall	copybt

not_m:	pop	XL
	pop	XH
	pop	r3
	ret

; ==============================================
;	copybt - save current mouse position
;
;	Z - points to moff() for our mouse (preserved)
;	X - points to destination of move (destroyed)
;
copybt:	push	ZH
	push	ZL
	push	r17
	push	r0

	adiw	ZH:ZL,mo_x0		; where the current loc is
	ldi	r17,btblksz		; 9 bytes per location
cpylp:	ld	r0,Z+
	st	X+,r0
	dec	r17
	brne	cpylp

	pop	r0
	pop	r17
	pop	ZL
	pop	ZH
	ret

