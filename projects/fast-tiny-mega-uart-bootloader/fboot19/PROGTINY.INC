;-------------------------------------------------------------------------
;		 		Program User Flash
;-------------------------------------------------------------------------
;
	ldi	xl, low (ProgBuff)
	ldi	xh, high(ProgBuff)
	movw	yh:yl, xh:xl
;---------------------- Receive data for one Page ------------------------
_pro1:
	brts	_pro5
	rcall	getchar
	brne	_pro2
	rcall	getchar
	subi	a0, ESC_SHIFT
	set				; end mark received set
	breq	_pro3
	clt
_pro2:
	st	x+, a0
	cpi	xl, low(ProgBuffEnd)	; since buffer size below 256
	brne	_pro1
;-------------------------------------------------------------------------
_pro3:
	cp	zl, zerol
	cpc	zh, zerol		; z = 0x0000 ?
	brne	_pro5
;------------------------------ Insert rjmp to boot loader ---------------
	ld	appl, y
	ldd	apph, y+1
	subi	appl, low (BootStart - 0x1001)	; new application jump
	sbci	apph, high(BootStart - 0x1001)
	ldi	a0, low (BootStart-1)
	ldi	a1, high(BootStart-1 + 0xC000)  ; = RJMP
	st	y, a0
	std	y+1, a1				; replace by bootloader jump
;-------------------------------------------------------------------------
;               Erase application Flash backward (avoid lock out)
;-------------------------------------------------------------------------
	ldi     zl, low (2*BootStart)
	ldi     zh, high(2*BootStart)
_pro4:
	subi    zl, low (2*PageSize)
	sbci    zh, high(2*PageSize)
	ldi	a0, 1<<PGERS^1<<SPMEN
	out     SPMCSR, a0
	SPM                             ; CPU halted until erase done
	brne    _pro4			; until Z = 0x0000
;-------------------------------------------------------------------------
_pro5:
	brtc	_pro6
	ldi	xl, low (ProgBuffEnd - 2)
	ldi	xh, high(ProgBuffEnd - 2)
	st	x+, appl
	st	x+, apph
;---------------------- Fill page buffer ---------------------------------
_pro6:
	subi	zl, low (-PageSize * 2)
	sbci	zh, high(-PageSize * 2)	; point to next page
_pro7:
	sbiw	zh:zl, 2
	ld	r1, -x			; fill buffer downward
	ld	r0, -x
	ldi	a0, 1<<SPMEN		; fill buffer command
	out	SPMCSR, a0
	SPM
	cpi	xl, low(ProgBuff)
	brne	_pro7
;---------------------- Program page -------------------------------------
	ldi	a0, 1<<PGWRT^1<<SPMEN	; write page command
	out	SPMCSR, a0
	SPM
;---------------------- Next Page ----------------------------------------
	subi	zl, low (-PageSize * 2)
	sbci	zh, high(-PageSize * 2)	; next page
	brts	_pro8
	ldi	a0, CONTINUE
	rcall	putchar
_pro8:
	cpi	zl, low( 2*BootStart)
	ldi	a0, high(2*BootStart)
	cpc	zh, a0                  ; last page reached ?
	brcs	_pro1
	brts	_pro9
	rjmp	main_error		; error, size exceeded
_pro9:
	rjmp	main_ok
;-------------------------------------------------------------------------
