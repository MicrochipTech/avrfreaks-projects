;---------------------------------------
;ooooooooooooooooooooooooooooooooooooooo
;=======================================
;	debug command
;---------------------------------------
;---------------------------------------
dbg_reep:	syptr	ad_jbiruinf
	ldi	C, 40
dbgreeps:	rcall	get_eepr
	rcall	sid_sout
	adiw	YH:YL, 1
	djnz	C, dbgreeps
	ret
;---------------------------------------
dbg_rram:	sxptr	0x70
	ldi	C, 8
dbgrrams:	ld	A, X+
	rcall	sid_sout
	djnz	C, dbgrrams
	ret
;=======================================





