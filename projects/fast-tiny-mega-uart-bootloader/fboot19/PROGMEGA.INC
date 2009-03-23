;-------------------------------------------------------------------------
;		 		Program User Flash
;-------------------------------------------------------------------------
;
;program:
	ldi	xl, low (ProgBuff)
	ldi	xh, high(ProgBuff)
	movw	yh:yl, xh:xl
;------------------------------ Receive data into buffer -----------------
_pro1:
	rcall	getchar
	brne	_pro2
	rcall	getchar
	subi	a0, ESC_SHIFT
	set				; end mark received
	breq	_pro3			; A5,80 = end mark
_pro2:
	clt
	st	x+, a0
	cpi	xl, low (ProgBuffEnd)
	ldi     a0, high(ProgBuffEnd)
	cpc	xh, a0
	brne	_pro1
;-------------------------------------------------------------------------
_pro3:
	cp	xl, yl
	cpc	xh, yh
	breq	_pro7			; no new page, get out
	ldi	xl, low (ProgbuffEnd)
	ldi	xh, high(ProgBuffEnd)
	subi	zl, low (-2*BufferSize)
	sbci	zh, high(-2*BufferSize)	; point to next buffer
;------------------------------ Fill page buffer -------------------------
_pro4:
	ldi	a1, PageSize
_pro5:
	sbiw	zh:zl, 2
	ld	r1, -x			; fill buffer downward
	ld	r0, -x
	ldi	a0, 1<<SPMEN		; fill buffer command
	rcall	do_spm
	dec	a1
	brne	_pro5
;------------------------------ Avoid self destruction ! -----------------
	cpi	zl, low (2*BootStart)
	ldi	a1, high(2*BootStart)
	cpc	zh, a1                  ; below bootloader ?
	brcc	_pro6
;------------------------------ Erase page -------------------------------
	ldi	a0, 1<<PGERS^1<<SPMEN
	rcall	do_spm
;------------------------------ Program page -----------------------------
	ldi	a0, 1<<PGWRT^1<<SPMEN	; write page command
	rcall	do_spm_rerww
	cp	xl, yl
	cpc	xh, yh
	brne	_pro4
	brts	_pro7			; get out
;------------------------------ Next Buffer ------------------------------
	subi	zl, low (-2*BufferSize)
	sbci	zh, high(-2*BufferSize)
	ldi	a0, CONTINUE
	rcall	putchar
	rjmp	_pro1
;-------------------------------------------------------------------------
do_spm_rerww:
	rcall	do_spm
	ldi	a0, 1<<RWWSRE^1<<SPMEN
do_spm:
	xout	SPMCSR, a0
	spm
_dsp1:
	xin	a0, SPMCSR
	sbrc	a0, SPMEN
	rjmp	_dsp1
	ret
;-------------------------------------------------------------------------
_pro6:
	rjmp	main_error		; error, size exceeded
_pro7:
	rjmp	main_ok
;-------------------------------------------------------------------------
