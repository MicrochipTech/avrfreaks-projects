;	File name: AT3283_4.inc
;
;	USART support for ATmega16

; ==============================================
;	initialize the USART
;
;	push high baud
;	push low baud
;	call usart_init

usart_init:
	call	fetcharg2		; get args in r0 & r1
	push	r16

	out	ubrrh,r0
	out	ubrrl,r1
	rcall	xr_init			; init holding buffers
	ldi	r16,u_noint
	out	ucsrb,r16
	ldi	r16,(1<<ursel)|(3<<ucsz0)	; (1<<USBS)
	out	ucsrc,r16

	clr	r16
	sts	echo232,r16		; no echo is default
	
	pop	r16
	pop	r1			; pushed by fetcharg2
	pop	r0
	ret

; ==============================================
;	USART char received interrupt

i_usartrx:
	push	r16
	in	r16,sreg		; status register
	push	r16
	push	r17
	push	XH
	push	XL

	in	r16,udr			; read the character
	lds	r17,rhead
	ldi	XH,high(rbuf)		; point x at buffer
	ldi	XL,low(rbuf)
	add	XL,r17			; add to base address
	brcc	nove
	inc	XH			; carried into high byte
nove:	st	X,r16			; put char away
	inc	r17			; chase your tail
	andi	r17,rmask		; mask for buffer size
	sts	rhead,r17

	lds	r17,echo232		; see if we're echoing
	tst	r17
	breq	noech
	push	r16
	rcall	xmit_put

noech:	pop	XL			; restore Xreg
	pop	XH
	pop	r17
	pop	r16
	out	sreg,r16		; restore status reg
	pop	r16
	reti

; ==============================================
;	USART UDR empty interrupt handler

i_usartun:
	push	r16
	in	r16,sreg		; status register
	push	r16
	push	r0

	lds	r0,xhead		; any more to send?
	lds	r16,xtail
	cp	r0,r16
	brne	yup
	
	ldi	r16,u_noint		; no - disable interrupt
	out	ucsrb,r16
	rjmp	nope


yup:	push	XH			; save Xreg
	push	XL

	ldi	XH,high(xbuf)		; point x at buffer
	ldi	XL,low(xbuf)
	add	XL,r16			; add to base address
	brcc	novb
	inc	XH			; carried into high byte
novb:	ld	r0,X			; get byte to send
	out	udr,r0			; byte into transmitter
	inc	r16			; chase your tail
	andi	r16,xmask		; mask for buffer size
	sts	xtail,r16

	pop	XL			; restore Xreg
	pop	XH
nope:	pop	r0
	pop	r16
	out	sreg,r16		; restore status reg
	pop	r16
	reti

; ==============================================
; get character from receive buffer
;	call	recv_test
;	breq	empty
;	call	recv_get
;	returns char in r0
;	returns zero flag about buf sts (=0 means now empty)
;
; note: this returns a character regardless, so check recv_test first!

recv_get:
	push	XH
	push	XL
	push	r16
	push	r17

	ldi	XH,high(rbuf)	; point x at buffer
	ldi	XL,low(rbuf)
	lds	r16,rtail	; current tail offset
	add	XL,r16		; add to base address
	brcc	nova
	inc	XH		; carried into high byte
nova:	ld	r0,X		; grab the character
	inc	r16		; incr circular offset
	andi	r16,rmask	; mask for buffer size
	sts	rtail,r16	; new tail pointer

	lds	r17,rhead	; head offset
	cp	r16,r17		; set zero flag more chars

	pop	r17
	pop	r16
	pop	XL
	pop	XH
	ret

; ==============================================
; check for data in receive buffer
;	call	recv_test
;	breq	empty
;	brne	not empty

recv_test:
	push	r0
	push	r1
	lds	r0,rhead
	lds	r1,rtail
	cp	r0,r1		; set flags
	pop	r1
	pop	r0
	ret

; ==============================================
;	xmit_put - send a byte via the USART
;
;	push byte
;	call xmit_put
;
;	this waits until the thing goes out

xmit_put:
	call fetcharg1		; get arg into r0
	push	r0
bz:	rcall	x_put_nw
	brcs	bz
	pop	r0		; fetcharg1 pushed this
	ret

; ==============================================
;	x_put_nw - send a byte on the USART if possible
;
;	push byte
;	call x_put_nw
;
;	if the buffer is empty and transmitter available,
;	    the character is sent immediately
;	or, if the buffer has room, stick it in
;	or, if the buffer is full, return
;
;	returns:
;	  sent or buffered: carry clear
;	  full: carry set, byte still on stack

x_put_nw:
	call	fetcharg1	; get arg into r0
	push	r16
	push	r17

	lds	r16,xhead	; head offset
	lds	r17,xtail	; tail offset
	cp	r16,r17		; buffer empty?
	brne	notmt

	sbis	ucsra,udre	; can we send?
	rjmp	notmt		; no - it's in use
	out	udr,r0		; yes - send char
	ldi	r17,u_int	; buffer empty interrupt
	out	ucsrb,r17	; is now enabled
	rjmp	byenow		; and we're done

notmt:	inc	r16		; check for full buffer
	andi	r16,xmask	; it's a circular buf
	cp	r16,r17		; if head + 1 = tail, full
	brne	room

	pop	r17		; clear our local pushes
	pop	r16
	push	r0		; put the arg on
	rcall	diddleme	; go manipulate the stack
	pop	r0		; r0 we came in with
	sec			; set carry for full buf
	ret

room:	lds	r16,xhead
	push	XH
	push	XL

	ldi	XH,high(xbuf)	; point x at buffer
	ldi	XL,low(xbuf)
	add	XL,r16		; add to base address
	brcc	novr
	inc	XH		; carried into high byte
novr:	st	X,r0		; put their byte away
	inc	r16		; incr circular offset
	andi	r16,xmask	; mask for buffer size
	sts	xhead,r16	; new head pointer

	pop	XL
	pop	XH
byenow:
	pop	r17
	pop	r16
	pop	r0		; fetcharg1 pushed this
	clc			; clear carry means OK
	ret

; ==============================================
;	diddleme - play with the stack
;
;	specialty routine for x_put_nw to undo some
;	stack manipulations
;
;	at time of call, stack is:
;		+1) return address to our caller (2 bytes)
;		+3) arg to x_put_nw call
;		+4) orig r0 at time of x_put_nw call
;		+5) return to caller to x_put_nw (2 bytes)
;
;	on return (prior to ret), it will be:
;		+1) return address to our caller (2 bytes)
;		+3) orig r0
;		+4) return to caller to x_put_nw (2 bytes)
;		+6) arg
;
;	caller pops r0, does a ret, and the orig arg is
;	on the stack ready for another call to x_put_nw

diddleme:
	push	YH		; save scratch regs
	push	YL		; these change the offsets, of course
	push	r16
	push	r17

	in	YL,SPL		; copy of revised stack pointer
	in	YH,SPH

	ldd	r16,Y+7		; the arg

	ldd	r17,Y+8		; the r0 value
	std	Y+7,r17

	ldd	r17,Y+9		; rtn addr bytes
	std	Y+8,r17
	ldd	r17,Y+10
	std	Y+9,r17

	std	Y+10,r16	; put the arg back

	pop	r17
	pop	r16
	pop	YL
	pop	YH
	ret

; ==============================================
;	xr_init - initialize the transmit and receive buffers
;
;	data goes in at the head, comes out at the tail

xr_init:
	push	r0

	clr	r0
	sts	xhead,r0	; xmit head
	sts	xtail,r0	; xmit tail
	sts	rhead,r0	; receiver head
	sts	rtail,r0	; receiver tail

	pop	r0
	ret
