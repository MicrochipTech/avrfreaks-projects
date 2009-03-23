;-------------------------------------------------------------------------
;				Check password
;-------------------------------------------------------------------------
	ldi	yl, 10			; try it 10 times
checkpwd:
	ldi	zl, low (2*Password)
	ldi	zh, high(2*Password)
_cpw2:
	XLPM	r0, z+
	tst	r0
	breq	_cpw3			; end if zero byte
	rcall	getchar
	cp	r0, a0
	breq	_cpw2			; compare next byte
	dec	yl			; count down wrong compares
	breq	timeout
	rjmp	checkpwd		; try again
_cpw3:
;-------------------------------------------------------------------------
