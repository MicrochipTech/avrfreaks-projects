;------------------------------	check, if watchdog active ----------------
	wdr
	xin	a0, WDTCSR
	mov	a1, a0
	ori	a0, 1<<WDCE			; change enable
	ori	a1, 1<<WDP2^1<<WDP1^1<<WDP0	; 2s
	xout	WDTCSR, a0
	xout	WDTCSR, a1
;-------------------------------------------------------------------------

