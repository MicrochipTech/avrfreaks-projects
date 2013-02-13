;=======================================
;ooooooooooooooooooooooooooooooooooooooo
;	task set tanggal dan jam
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;=======================================
.equ		v_idstgl	= v_ptask
.equ		v_pstglh	= v_ptask+1
.equ		v_pstgll	= v_ptask+2
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set_stgl:	lds	A, v_idstgl
	mov	D, v_act
	cjne	D, 0, setstgl1
	rjmp	set_jam
setstgl1:	cjne	D, 1, setstgl2
	rjmp	set_men
setstgl2:	cjne	D, 2, setstgl3
	rjmp	set_tgl
setstgl3:	cjne	D, 3, setstgl4
	rjmp	set_bln
setstgl4:	cjne	D, 4, setstgl5
	rjmp	set_thn
setstgl5:	rjmp	set_day
;=======================================
get_stgl:	mov	D, v_act
	cjne	D, 0, getstgl1
	lds	A, v_hour
	sts	v_idstgl, A
	rjmp	dspstgls
getstgl1:	cjne	D, 1, getstgl2
	lds	A, v_min
	sts	v_idstgl, A
	rjmp	dsptgl11	
getstgl2:	cjne	D, 2, getstgl3
	lds	A, v_date
	sts	v_idstgl, A
	rjmp	dsptgl21	
getstgl3:	cjne	D, 3, getstgl4
	lds	A, v_month
	sts	v_idstgl, A
	rjmp	dsptgl31
getstgl4:	cjne	D, 4, getstgl5
	lds	A, v_year
	sts	v_idstgl, A
	rjmp	dsptgl41	
getstgl5:	lds	A, v_day
	sts	v_idstgl, A
	rjmp	dsptgl5s	
;---------------------------------------
dsp_stgl:	mov	D, v_act
	cjne	D, 0, dsptgl1s
dspstgls:	ldi	C, 10
dspstgl1:	rcall	lcdline1
dspstgl2:	rcall	toxright
dspstgl3:	lds	A, v_idstgl
	rcall	bcdascii
	sts	v_pstglh, A
	rcall	lcdnum
	mov	A, C
	sts	v_pstgll, A
	rcall	lcdnum
	rcall	toleft
	rjmp	toleft
dsptgl1s:	cjne	D, 1, dsptgl2s
dsptgl11:	ldi	C, 13
	rjmp	dspstgl1
dsptgl2s:	cjne	D,  2, dsptgl3s
dsptgl21:	rcall	lcdline2
	rjmp	dspstgl3
dsptgl3s:	cjne	D,  3, dsptgl4s
dsptgl31:	ldi	C, 3
dsptgl32:	rcall	lcdline2
	rjmp	dspstgl2
dsptgl4s:	cjne	D, 4, dsptgl5s
dsptgl41:	ldi	C, 6
	rjmp	dsptgl32
dsptgl5s:	rcall	dsptgl5n
	lds	A, v_idstgl
	rcall	lcdDay
	rcall	toleft
	rcall	toleft
	ldi	A, ' '
	rcall	lcdprn
dsptgl5n:	rcall	lcdline2
	ldi	C, 9
	rjmp	toxright
;---------------------------------------
txtstgl1:	.db	"Set Jam  [  :  ] ",eop
txtstgl2:	.db	"  /  /           ",eop
;---------------------------------------
str_stgl:	clr	tsk_sts
	setb	tsk_sts, cs_stgl
	clr	v_act
	rcall	lcdline1
	table	txtstgl1
	rcall	lcdprint
	rcall	lcdline2
	table	txtstgl2
	rcall	lcdprint
	clr	v_act
	ldi	E, 6
strstgls:	rcall	get_stgl
	inc	v_act
	djnz	E, strstgls
	rcall	curshow
	clr	v_act
	rjmp	get_stgl
;=======================================
tsk_stgl:	lds	A, v_evbtn
	cjne	A, sp_plus, tsktgl2s
	rcall	set_stgl
	inc	v_act
	mov	D, v_act
	cjne	D, 6, tsktgl1s
	clr	v_act
tsktgl1s:	rjmp	get_stgl
tsktgl2s:	cjne	A, sp_min, tsktgl3s
	rcall	set_stgl
	dec	v_act
	mov	D, v_act
	cjne	D, 0xff, tsktgl1s
	ldi	D, 5
	mov	v_act, D
	rjmp	get_stgl
tsktgl3s:	mov	D, v_act
	cjne	D, 5,tsktgl5s
	cjne	A, sp_dn, tsktgl4s
	sdec	v_idstgl
	lds	A, v_idstgl
	jnz	A, tsktgl31
	smov	v_idstgl, 7
tsktgl31:	rjmp	dsp_stgl
tsktgl4s:	cjne	A, sp_up, tsktgl5r
	sinc	v_idstgl
	lds	A, v_idstgl
	cjne	A, 8, tsktgl31
	smov	v_idstgl, 1
	rjmp	dsp_stgl
tsktgl5s:	cpi	A, 10
	brlo	tsktgl51
tsktgl5r:	ret
tsktgl51:	lds	A, v_pstgll
	sts	v_pstglh, A
	lds	A, v_evbtn
	sts	v_pstgll,A
	lds	A, v_pstglh
	swap	A
	andi	A, 0xf0
	mov	C, A
	lds	A, v_pstgll
	or	A, C
	sts	v_idstgl,A
	rjmp	dsp_stgl
;---------------------------------------
;=======================================
