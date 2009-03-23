;-------------------------------------------------------------------------
;		 		Verify User Flash
;-------------------------------------------------------------------------
;
_ver1:
	rcall	getchar
	brne	_ver2			; not COMMAND ?
	rcall	getchar
	subi	a0, ESC_SHIFT
	breq	_ver4			; COMMMAND + not COMMAND = End
_ver2:
.if FLASHEND > 0x7FFF
	out	RAMPZ, zx
	elpm	r0, z
	adiw	zh:zl, 1
	adc	zx, zerol		; 24 bit addition
.else
	lpm	r0, z+
.endif
.ifndef BootFuse
	cpi	zl, 3
	cpc	zh, zerol
	brcs	_ver1			; exclude jump to bootloader
.endif
	cpse	r0, a0
	set
	rjmp	_ver1
_ver4:
	brts	main_error		; error, Flash not equal
	rjmp	main_ok
;-------------------------------------------------------------------------
