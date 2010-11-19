

;for 8Mhz @ non-doubled baud-rate
;.equ	bd2400	= 207
;.equ	bd4800	= 103
;.equ	bd9600	= 51
;.equ	bd14400	= 34
;.equ	bd19200	= 25
;.equ	bd28800	= 16
;.equ	bd38400	= 12
;.equ	bd57600	= 8

;for 8Mhz @ doubled baud-rate
.equ	bd2400	= 416
.equ	bd4800	= 207
.equ	bd9600	= 103
.equ	bd14400	= 68
.equ	bd19200	= 51
.equ	bd28800	= 34
.equ	bd38400	= 25
.equ	bd57600	= 16


; ram usage for serial routines

.dseg                        ; Start data segment

lastrx:		.byte	4	; always the latest RX'ed byte

startC:		.byte	1



.cseg

; ********************* remember to add line to set RS485 direction ************
init_ser0:
	rcall	flush_rx
	ldi	temp1,2
	sts	UCSR0A,temp1		; doubles rate

	ldi	temp1,low (16)		; set baud rate
	sts	UBRR0L,temp1
	ldi	temp1,high (16)
	sts	UBRR0H,temp1

	ldi	temp1,(1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)	; Enable TX,RX +RX-int only
	sts	UCSR0B,temp1			; write config reg
	ldi	temp1,0b00000110		; 8 bits, 1 stop, no parity
	sts	UCSR0C,temp1
	ret




; *************** RX int handler ***********************
;
USART_RXC:
	push	temp2
	push	temp1
	in	temp1,SREG
	push	temp1
	push	ZL
	push	ZH

	lds	temp1,rxcount		; get current count in buffer
	cpi	temp1,inbufflen-1	; is it = max-1
	brne	Sok			; if not full, continue
; buffer full so get RX data
	load	temp2,UDR0		; get RX'ed byte
	rjmp	AB1			; store it in the pipeline

Sok:	inc	temp1			; not full yet so inc count
	sts	rxcount,temp1		; and resave

	ldz	ram,inbuffer
	lds	temp1,rxhead
	clr	temp2
	add	ZL,temp1
	adc	ZH,temp2

	load	temp2,UDR0		; get RX'ed data
	st	z,temp2			; save into ram buffer

	inc	temp1
	andi	temp1,(inbufflen-1)	; so wraps the input buffer
	sts	rxhead,temp1

AB1:	; the pipeline is not used in this application
	lds	temp1,lastrx+2		; always the latest 4 bytes
	sts	lastrx+3,temp1
	lds	temp1,lastrx+1
	sts	lastrx+2,temp1
	lds	temp1,lastrx
	sts	lastrx+1,temp1
	sts	lastrx,temp2		; always the latest byte

	pop	ZH
	pop	ZL
	pop	temp1
	out	SREG,temp1
	pop	temp1
	pop	temp2
	reti




getrxcount:
	lds	temp1,rxcount
	ret

getabyte:
	push	temp2

getaB:	lds	temp1,rxcount
	tst	temp1
	breq	getaB		; loops till data available

	cli			; stop ints
	lds	temp1,rxcount
	dec	temp1
	sts	rxcount,temp1	; dec count saved

	push	ZL
	push	ZH
	clr	temp1
	ldi	ZL,low(inbuffer)
	ldi	ZH,high(inbuffer)
	lds	temp2,rxtail
	add	ZL,temp2
	adc	ZH,temp1

	ld	temp1,z		; get from ram buffer

	inc	temp2
	andi	temp2,inbufflen-1 ; so wraps the pointer
	sts	rxtail,temp2

	sei			; re-enable ints

	pop	ZH
	pop	ZL
	pop	temp2
	ret



sersample0:
	lds	temp1,rxcount	; see if any data
	tst	temp1
	brne	fetchSer
	clc
	ret
fetchSer:
	rcall	getabyte
	sec
	ret






flush_rx:
	cli
	clr	temp1			; stop rx,tx, rx-int
	sts	rxcount,temp1		; zero count
	sts	rxtail,temp1
	sts	rxhead,temp1
; and wipe last-4-bytes store
	sts	lastrx,temp1
	sts	lastrx+1,temp1
	sts	lastrx+2,temp1
	sts	lastrx+3,temp1
	sei
	ret



; data to send in temp1
sersend0:
	push	temp1
; Wait for empty transmit buffer
J6:	lds	temp1,UCSR0A	; read status reg
	sbrs	temp1,5		; test if ok to tx
	rjmp	J6		; if not, loop and wait
	pop	temp1
	sts	UDR0,temp1	; data out
	ret







