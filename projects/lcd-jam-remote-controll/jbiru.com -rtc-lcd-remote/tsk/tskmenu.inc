;=======================================
;ooooooooooooooooooooooooooooooooooooooo
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		c_MaxMenu	= 1
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		mms	= 0x0fa
.equ		mme	= 0x0fb
.equ		mss	= 0x0fc
.equ		mse	= 0x0fd
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		v_menu	= v_pmenu
.equ		v_sbmenu	= v_pmenu+1
.equ		v_mna	= v_pmenu+2
.equ		v_mnb	= v_pmenu+3
.equ		v_mnc	= v_pmenu+4
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MenuItem:	.db	mms,1,"[Setting] "
	.db	mss,1,"1. Pump  #1 "
	.db	mss,2,"2. Temperature"
	.db	mss,3,"3. Delay ON "
	.db	mss,4,"4. Delay OFF"
	.db	mss,5,"5. Date & Time"
	.db	mss,6,"6. Password "
	.db	mss,7,"7. Temp. Const"
	.db	mss,8,"8. Dly. Limit "
	.db	mss,9,"9. Dly. OFF LCD "
	.db	mss,10,"10. Reset "
	.db	mss, mse
	.db	mme,eop,eop,eop
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
setlcspr:	lds	D, v_mnc
	jz	D, setlcspn
setlcsp1:	ldi	A, ' '
	rcall	lcdprn
	djnz	D, setlcsp1
setlcspn:	ret
bar_menu:	clrb	sys_sts, s_barmn
	smov	v_mnc, 16
	table 	MenuItem
barmenus:	lpm
	mov	A, R0
	cjne	A, mme, barmenu1
	setb	sys_sts, s_barmn
	ret
barmenu1:	cjne	A, mms, barmenu3
	adiw	ZH:ZL, 1
	lpm
	mov	A, R0
	lds	C, v_menu
	cp	A, C
	brne 	barmenu3
	rcall	lcdLine1
barmenu2:	adiw	ZH:ZL, 1
	lpm
	mov	A, R0
	cjne	A, mss, barmenu4
	rjmp	setlcspr
barmenu3:	adiw	ZH:ZL, 1
	rjmp	barmenus
barmenu4:	rcall	lcdprn
	sdec	v_mnc
	rjmp	barmenu2
;---------------------------------------
sub_menu:	clrb	sys_sts, s_barmn
submenus:	smov	v_mnc, 16
	lpm
	mov	A, R0
	cjne	A, mss, submenu3
	adiw	ZH:ZL, 1
	lpm
	mov	A, R0
	cjne	A, mse, submenu1
	setb	sys_sts, s_barmn
	ret
submenu1:	lds	C, v_sbmenu
	cp	A, C
	brne	submenu3
	rcall	lcdLine2
submenu2:	adiw	ZH:ZL, 1
	lpm
	mov	A, R0
	cjne	A, mss, submenu4
	rjmp	setlcspr
submenu3:	adiw	ZH:ZL, 1
	rjmp	submenus
submenu4:	cjne	A, '#', submenu5
	adiw	ZH:ZL, 1
	push	ZH
	push	ZL
	;rcall	extmenus
	pop	ZL
	pop	ZH
	rjmp	submenu6
submenu5:	rcall	lcdprn
submenu6:	sdec	v_mnc
	rjmp	submenu2
;=======================================
;	Fungsi Tambahan Menu
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		;------------------
		;rcall	sid_sout
		;------------------
		;------------------
		;rcall	sid_sout
		;------------------
/*
.txtmpum:	.db	"[Manual]",eop, eop
.txtppum:	.db	"[progrm]",eop, eop

extmenus:	lpm
	mov	A, R0
	cjne	A, '1',extmen1r
	jb	s_sysauto,extmen11
	table	txtmpump
extmen1s:	rjmp	lcd_str
extmen11:	table	txtppum
	rjmp	lcd_str
extmen1r:	ret
*/
;=======================================
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
str_menu:	smov	v_sbmenu, 1
	smov	v_menu, 1
strmenus:	rcall	bar_menu
	rcall	sub_menu
	rcall	curHide
	clr	tsk_sts
	setb	tsk_sts, cs_menu
strmenur:	ret
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
tsk_menu:	lds	A, v_evbtn
	cjne	A, sp_xent, tskmen1s
tskmenus:	lds	A, v_menu
	swap	A
	lds	C, v_sbmenu
	or	A, C
	sts	v_mnb, A
tskmenu0:	cjne	A, 0x15, tskmenu1
	jmp	str_stgl
tskmenu1:	cjne	A, 0x1A, tskmenu2
sys_arst:	;=============================
	; Hardware Reset System 
	;-----------------------------
	; for ATMEGA 16
	;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	in 	A, WDTCR
	ori	A, (1<<WDTOE)|(1<<WDE)
	out	WDTCR, A
	ret
tskmenu2:	cjne	A, 0x16, tskmenu3
	rjmp	str_spsw
tskmenu3:	cjne	A, 0x11, tskmenu4

	;jmp	submenu1
	ret
tskmenu4:	cjne	A, 0x13, tskmenu5

	;jmp	str_tdly
	ret
tskmenu5:	cjne	A, 0x14, tskmenu6

	;jmp	str_tdly
	ret
tskmenu6:	cjne	A, 0x12, tskmenu7

	;jmp	str_stmp
	ret
tskmenu7:	cjne	A, 0x17, tskmenu8

	;jmp	str_stmp
	ret
tskmenu8:	cjne	A, 0x18, tskmenu9

	;jmp	str_stmp
	ret
tskmenu9:	cjne	A, 0x19, tskmenua

	;jmp	str_stmp
tskmenua:	ret
;=======================================
;---------------------------------------
tskmen1s:	cjne	A, sp_plus,tskmen11
	sinc	v_menu
	lds	A, v_menu
	cjne	A, c_maxmenu+1, tskmen21
	smov	v_menu, 1
	rjmp	tskmen12
tskmen11:	cjne	A, sp_min,tskmen2s	
	sdec	v_menu
	lds	A, v_menu
	jnz	A, tskmen21
	smov	v_menu, c_maxmenu
tskmen12:	smov	v_sbmenu, 1
	rjmp	tskmen21
	;=============================
tskmen2s:	cjne	A, sp_dn, tskmen3s
	sinc	v_sbmenu
tskmen21:	rcall	bar_menu
	rcall	sub_menu
	jb	sys_sts, s_barmn, tskmen22
	ret
tskmen22:	smov	v_sbmenu, 1
	rjmp	tskmen21
tskmen3s:	cjne	A, sp_up,tskmen4s
	sdec	v_sbmenu
	lds	A, v_sbmenu
	cjne	A, 0, tskmen21
	smov	v_sbmenu, 1
	ret
tskmen4s:	cpi	A, 10
	brlo	tskmen41
	ret
tskmen41:	sts	v_sbmenu, A
	rjmp	tskmen21
;=======================================

