; universal signed, unsigned decimal number to ASCII convert
; MSB -> LSB = r23:r22 ZH:ZL
_print24bit:		; Print unsigned 24 bit values
	clr	r23
	rjmp	_prnum

_printsdecw:		; Print signed decimal (16 bit)
	sbrs	zh,7		; Is the number < 0 ?
	rjmp	_printdecw	; No, print 0 to 32767
	ser 	r22
	ser 	r23
	rjmp	_printsdecdw
	
_printdecb:			; Print (8 bit) value
	clr 	zh

_printdecw:			; Print unsigned decimal (16 bit)
	clr 	r22
	clr  	r23

_printsdecdw:		; Print signed decimal (32 bit)
	sbrs	r23,7
	rjmp	_printdecdw
	com 	zl
	com 	zh
	com 	r22
	com 	r23
	subi	zl,-1
	sbci	zh,-1
	sbci	r22,-1
	sbci	r23,-1
	ldi 	r24,'-'
	rcall	_putchar

_printdecdw:		; Print unsigned decimal (32 bit)
_prnum:
	push	yl
	push	yh
	clr 	r1
	clr 	r2
	clr 	r3
	clr 	r4
	clr 	r5
	ldi 	r20,32
_prnum1:
	ldi 	yl,1
	clr  	yh
	ldi 	r21,5
_prnum2:
	ldi 	r25,51
	ld  	r24,y
	add 	r24,r25
	sbrc 	r24,7
	andi	r25,15
	sbrc	r24,3
	andi	r25,240
	sub 	r24,r25
	st  	y+,r24
	dec 	r21
	brne	_prnum2
	lsl 	zl
	rol  	zh
	rol 	r22
	rol 	r23
	rol 	r1
	rol 	r2
	rol 	r3
	rol 	r4
	rol 	r5
	dec 	r20
	brne 	_prnum1
	clt
	ldi 	zl,6
	clr 	zh
_prnum3:
	ld  	r24,-z
	rcall	_prnum5
	cpi 	zl,2
	brsh 	_prnum3
	brts	_prnum4
	set
	mov 	r24,r1
	rcall	_prnum6
_prnum4:
	pop 	yh
	pop 	yl
	ret
_prnum5:
	push 	r24
	swap	r24
	rcall	_prnum6
	pop  	r24
_prnum6:
	andi	r24,0x0f	; convert to ASCII and eliminate leading zeros
	subi	r24,-'0'
	brts	_putchar
	cpi 	r24,'0'
	brne	_prnum7
	; if you want leading spaces for nice format, add here "ldi r24, ' '  rjmp _putchar
	ret
_prnum7:
	set
_putchar:
	rjmp	_my_output_char_routine	; RS232, LCD, I2C etc, etc...
