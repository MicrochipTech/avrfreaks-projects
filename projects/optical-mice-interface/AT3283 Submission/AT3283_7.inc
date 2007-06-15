;	File Name: AT3283_7.inc
;
;	packet management routines

; ==============================================
;	pbrunout - the packet byte timer ran out
;
;	this is called when the packet byte timer
;	runs out (from the timer interrupt handler)
;	r0 does not have our mouse number:
;	  r16 - loop counter
;	    = 3 for mouse 0
;	    = 2 for mouse 1
;	    = 1 for mouse 2
;	Z -> packet info (Y usually does)
;	this timer only runs during receiving

pbrunout:
;	we'll handle this like a packet timing
;	error since it probably means we lost
;	a byte somewhere
	rcall	prunout
	ret

; ==============================================
;	prunout - the packet timer ran out
;
;	this is called when the packet timer
;	runs out (from the timer interrupt handler)
;	r0 does not have our mouse number:
;	  r16 - loop counter
;	    = 3 for mouse 0
;	    = 2 for mouse 1
;	    = 1 for mouse 2
;	Z -> packet info (Y usually does)
;	this timer only runs during receiving

prunout:
	push	ZH
	push	ZL
	push	YH
	push	YL
	push	r17
	push	r0

	mov	YH,ZH
	mov	YL,ZL			; Y -> packet
	clr	r17
	std	Y+ptrun,r17		; stop packet timer
	std	Y+pbtrun,r17		; stop packet byte timer

	ldi	r17,3
	sub	r17,r16
	mov	r0,r17			; mouse number into r0
	rcall	moff2z			; Z -> moff

	ldd	r17,Y+phead		; orig buf head ptr
	std	Z+mo_hd,r17		; put it back to kill bytes
	ldd	r17,Y+preject		; reject count
	inc	r17
	sbrs	r17,6			; stop at 63
	std	Y+preject,r17
	ldi	r17,1			; set packets to idle
	std	Y+psts,r17	

	pop	r0
	pop	r17
	pop	YL
	pop	YH
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	startofbyte - a start bit was received
;
;	this is called when the receiver kicks in.
;	called from the intx interrupt handler, so r0
;	has our mouse number

startofbyte:
	push	r16
	push	YH
	push	YL

	rcall	pack2y			; Y -> packet info
	ldd	r16,Y+psts		; status
	cpi	r16,1
	brcs	ssrtn			; ignore init
	brne	srecv
	
;	we're idle - start timers, save head, etc.
	ldd	r16,Y+ptval		; start the packet timer
	std	Y+ptimer,r16
	ldi	r16,1
	std	Y+ptrun,r16

	clr	r16
	std	Y+perr,r16		; clear packet error flag
	std	Y+pbytes,r16		; and received byte count

	push	ZH
	push	ZL
	rcall	moff2z
	ldd	r16,Z+mo_hd		; save head pointer
	pop	ZL
	pop	ZH
	std	Y+phead,r16

	ldi	r16,2			; set receive mode
	std	Y+psts,r16

;	we're receiving - restart timer
srecv:	ldi	r16,pbtval		; start/restart byte timer
	std	Y+pbtimer,r16		; counter value
	ldi	r16,1
	std	Y+pbtrun,r16		; run flag

ssrtn:	pop	YL
	pop	YH
	pop	r16
	ret

; ==============================================
;	endofbyte - a character was put away
;
;	this is called when the stop bit is received
;	and the character is put away.
;	called from the intx interrupt handler, so r0
;	has our mouse number

endofbyte:
	push	r16
	push	r17
	push	YH
	push	YL

	rcall	pack2y			; Y -> packet info
	ldd	r16,Y+psts		; status
	tst	r16
	breq	esrtn			; ignore init

	ldd	r16,Y+pbytes		; update packet byte count
	inc	r16
	std	Y+pbytes,r16
	ldd	r17,Y+psize		; # bytes in packet
	cp	r16,r17
	brne	esrtn			; done if not the last one

	clr	r16
	std	Y+ptrun,r16		; stop the packet timer
	std	Y+pbtrun,r16		; stop the packet byte timer
	ldd	r16,Y+perr		; packet error flag
	tst	r16			; is it zero?
	breq	goodpk			; if so, good packet

;	we had a bad packet - clean up a bit
	push	ZH
	push	ZL
	rcall	moff2z
	ldd	r16,y+phead		; orig buf head ptr
	std	Z+mo_hd,r16		; put it back to kill bytes
	ldd	r16,Y+preject		; reject count
	inc	r16
	sbrs	r16,6			; stop at 63
	std	Y+preject,r16
	rjmp	pkidle			; go set packet idle

goodpk:	ldd	r16,Y+pready		; update packet counter
	inc	r16
	std	Y+pready,r16
pkidle:	ldi	r16,1			; set packets to idle
	std	Y+psts,r16

esrtn:	pop	YL
	pop	YH
	pop	r17
	pop	r16
	ret

; ==============================================
;	timeout - timer runout
;
;	r16 - loop counter
;	  = 3 for mouse 0
;	  = 2 for mouse 1
;	  = 1 for mouse 2
;	this is called from inside the timer interrupt

timeout:
	push	ZH
	push	ZL
	push	YH
	push	YL
	push	r17
	push	r0

	ldi	r17,3			; figure out which mouse
	sub	r17,r16
	mov	r0,r17			; mouse to r0
	rcall	pack2y			; Y -> packet block
	ldd	r17,Y+psts		; get packet status
	tst	r17
	breq	intme			; ignore during initialization

	rcall	moff2z			; Z -> moff
	ldd	r17,Z+mo_sts		; are we sending or receiving?
	cpi	r17,3			; only 3 (receiving) is important to us
	brne	intme			; because sendmouse handles its own errors

;	we've had a timeout during receiving a byte. the timer is started
;	when the start bit is received, and stopped when the stop bit is
;	received. ergo, if it runs out something not nice has happened, so
;	we'll be naive and just hit things with a stick.
;	this timer has already been stopped by the caller.
;	the byte timer will fire soon, and toss the packet, and, if we're in
;	the middle of a packet, later the tail end of it will get rejected as
;	an incomplete following packet.  Eventually everything will catch up.

	ldi	r17,1
	std	Y+perr,r17		; flag packet bad
	clr	r17
	std	Z+mo_sts,r17		; set line idle

intme:	pop	r0
	pop	r17
	pop	YL
	pop	YH
	pop	ZL
	pop	ZH
	ret

; ==============================================
;	pack2y - make Y point to r0's packet definition
;
;	r0 - mouse number (0 - 2)
;	returns Y -> pack(packsize * r0)

pack2y:	ldi	YL,low(pack)
	ldi	YH,high(pack)
	tst	r0
	breq	pmre			; right answer for 0
	adiw	YH:YL,packsize		; advance to mouse 1
	sbrc	r0,1			; if mouse 1, we're done
	adiw	YH:YL,packsize		; advance to mouse 2
pmre:	ret
