;=======================================
;ooooooooooooooooooooooooooooooooooooooo
;	Scurity Password...
;---------------------------------------
.equ		c_maxpsw	= 6
;---------------------------------------
.equ		v_dpsw	= v_ptask
.equ		v_epsw	= v_ptask+5
.equ		v_vpsw	= v_ptask+6
.equ		v_setpsw	= v_ptask+7
.equ		v_newpsw	= v_ptask+8
;---------------------------------------	
def_psw:	.db	"724298", eop, 0
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
chk_mpsw:	syptr	ad_admin
	sxptr	v_dpsw
	ldi	D, c_maxpsw
chkmpsw1:	rcall	get_eepr
	ld	C, X+
	adiw	YH:YL, 1
	cp	A, C
	brne	chkmpswr
	djnz	D, chkmpsw1
	set
chkmpswr:	ret
chk_dpsw:	table 	def_psw
	sxptr	v_dpsw
	ldi	D, c_maxpsw
chkdpsw1:	lpm
	adiw	ZH:ZL, 1
	ld	C, X+
	mov	A, R0
	cp	A, C
	brne	chkdpswr
	djnz	D, chkdpsw1
	set
chkdpswr:	ret
;---------------------------------------
txtpasw1:	.db	"Password? ",eop,0
txtpasw2:	.db	"------",eop,0
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
str_pasw:	clr	tsk_sts
	setb	tsk_sts, cs_pasw
strpasws:	rcall	clrlines
	rcall	lcdline1
	table	txtpasw1
	rcall	lcd_str
strpasw1:	rcall	lcdline2
	table	txtpasw2
	rcall	lcd_str
	smov	v_vpsw, low(v_dpsw)
	rcall	lcdline2
strpaswr:	ret
;=======================================
tsk_pasw:	lds	A, v_evbtn
	cpi	A, 10
	brlo	tskpasw1
tskpaswr:	ret
tskpasw1:	lds	A, v_vpsw
	cjne	A, v_epsw, tskpaswb
	rcall	tskpaswc
	clt
	rcall	chk_dpsw
	brts	tskpasw2
	rcall	chk_mpsw
	brts	tskpasw3
	rjmp	str_tsks
	;------------------
	;rjmp	str_srun
	;==================
tskpasw2:	rcall	setpdefa
tskpasw3:	rjmp	str_menu
tskpaswb:	ldi	A, 0xdb
	rcall	lcdprn
tskpaswc:	lds	A, v_evbtn
	ori	A, 0x30
	lds	XL, v_vpsw
	clr	XH
	st	X+, A
	sinc	v_vpsw
	ret
;---------------------------------------
txtdfpsw:	.db	"000000"
;=======================================
setpdefa:	table	txtdfpsw
	syptr	ad_admin
	ldi	C, 6
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set_defa:	lpm
	mov	A, R0
	rcall	put_eepr
	adiw	ZH:ZL, 1
	adiw	YH:YL, 1
	djnz	C, set_defa
	ret
;=======================================
;	Ubah password
;	v_subact
;	0: ad_admin
;	1: ad_user
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
txtBaru:	.db	"Passw Baru?",eop
txtConf:	.db	"Konfirmasi?",eop
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
savenpsw:	syptr	ad_admin
	sxptr	v_newpsw
	ldi	D, c_maxpsw
savnpsws:	ld	A, X+
	rcall	put_eepr
	adiw	YH:YL, 1
	djnz	D, savnpsws
	ret
;--------------------------------------
dspspsws:	rcall	clrlines
	rcall	lcdline1
	rcall	lcdprint
	rjmp	strpasw1
str_spsw:	clr	tsk_sts
	setb	tsk_sts, cs_spsw
	rcall	strpasws
	smov	v_setpsw, 0
strspswr:	ret
tskspswb:	rjmp	tskpaswb
;-------------------------------------
tsk_spsw:	lds	C, v_setpsw
	cjne	C, 3, tskspsw0
	rjmp	strmenus
tskspsw0:	lds	A, v_evbtn
	cpi	A, 10
	brlo	tskspsw1
	ret
tskspsw1:	lds	A, v_vpsw
	cjne	A, v_epsw, tskspswb
	rcall	tskpaswc
	lds	A, v_setpsw
	jnz	A, tsksps1s
	clt
	rcall	chk_mpsw
	brts	tskspss1
tskspsw2:	rjmp	strmenus
tskspss1:	sinc	v_setpsw
	table	txtBaru
	rjmp	dspspsws
tsksps1s:	cjne	A, 1, tsksps2s
	sxptr	v_dpsw
	syptr	v_newpsw
	ldi	C, c_maxpsw
tsksps11:	ld	A, X+
	st	Y+, A
	djnz	C,tsksps11
	sinc	v_setpsw
	table	txtConf
	rjmp	dspspsws
tsksps2s:	cjne	A, 2, tskspswr
	sxptr	v_dpsw
	syptr	v_newpsw
	ldi	D, c_maxpsw
tsksps21:	ld	A, X+
	ld	C, Y+
	cp	A, C
	brne	tskspsw2
	djnz	D, tsksps21
	rcall	savenpsw
	rcall	clrline1
	table	txtspswa
	rcall	lcd_str
	rcall	clrline2
	table	txtspswb
	rcall	lcd_str
	sinc	v_setpsw
tskspswr:	ret
;---------------------------------------
txtspswa:	.db	"Password ",eop
txtspswb:	.db	"telah berubah... ",eop
;---------------------------------------
;=======================================

/*
*/
