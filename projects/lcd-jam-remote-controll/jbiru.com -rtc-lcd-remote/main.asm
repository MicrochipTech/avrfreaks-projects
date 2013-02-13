;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn
;=======================================
; 
;	oleh	: jbiru
;	tanggal	: 06/03/2012
;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	- Remote controll SM-809 [061]
;	- RTC DS1302
;	- USART COM
;	- timer 2 auto reload
;=======================================
.nolist
.include "m16def.inc"
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.include "inc\sysdef.inc"
.include "inc\smacro.inc"
.list
;---------------------------------------
.cseg
.org	0
	rjmp	sys_main
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		c_tmra	= f_cpu/32/9600
.equ		c_ita	= 96
.equ		c_itb	= 5
;---------------------------------------
.org	OC2addr
;=======================================
	setb	sys_sts, s_base
	in	bkupsreg, sreg
	push	A
	push	B
	;----------------------------
	; remote controll
	;============================
	rcall	ird_sclk
	;~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	sdjnz	v_ita, systmrar
	smov	v_ita, c_ita
	setb	sys_sts, s_ita
	;-----------------------------
	sdjnz	v_itb, systmrar
	smov	v_itb, c_itb
	setb	sys_sts, s_itb
systmrar:	pop	B
	pop	A
	out	sreg, bkupsreg
	reti
;---------------------------------------
;=======================================
;	fungsi baca remote controll
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.nolist
.include "inc\sysird.inc"
.include "inc\sysinit.inc"
.list
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
clr_even:	clrb	evn_sts, s_evbtn
	clrb	evn_sts, s_evclk
	clrb	evn_sts, s_evmin
	ret
;--------------------------------------
;oooooooooooooooooooooooooooooooooooooo
;======================================
;	main program
;--------------------------------------
sys_main:	outp	SPL, low(v_syssp)
	outp	SPH, high(v_syssp)
	rcall	sys_init
	rcall	rtc_init
	rcall	lcd_init
;--------------------------------------
	sei
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	clr	D
sys_srun:	;----------------------------
	jnb	sys_sts, s_ita, sys_srun
	clrb	sys_sts, s_ita
	inc	D
	;----------------------------
	rcall	get_evir
	rcall	sys_slcd
	;============================
	jnb	sys_sts, s_itb, sys_srun
	clrb	sys_sts, s_itb
	;----------------------------
	rcall	get_evbt
	rcall	sys_sclk
	;----------------------------
	rcall	sys_stsk
	;============================
	rcall	clr_even
	rjmp	sys_srun
;=======================================
;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sys_stsk:	jnb	evn_sts, s_evbtn, sys_ntsk
	lds	A, v_evbtn
	cjne	A, sp_xent, sysstsk2
	mov	A, tsk_sts
	jb	A, cs_stgl, systsk1s
	jb	A, cs_menu, systsk11
	ret
systsk1s:	rcall	curhide
	rcall	set_stgl
systsk10:	rjmp	strmenus
systsk11:	rjmp	tsk_menu
systsk12:	rjmp	tsk_spsw
sysstsk2:	cjne	A, sp_iesc, sysstsk3
	mov	A, tsk_sts
	jb	A, cs_stgl, systsk1s
	jb	A, cs_spsw, systsk10
str_tsks:	rcall	curhide
	clr	tsk_sts
	rcall	lcdtime
	rcall	lcddate
	ret
sysstsk3:	cjne	A, sp_menu, sysstsk4
	rcall	str_pasw
	ret
sysstsk4:	cjne	A, sp_menu, sysstska
	rcall	str_spsw
	ret
sysstska:	mov	A, tsk_sts
	jnb	A, cs_stgl, systskas
	rjmp	tsk_stgl
systskas:	jnb	A, cs_menu, systska1
	rjmp	tsk_menu
systska1:	jnb	A, cs_spsw, systska2
	rjmp	tsk_spsw
systska2:	jnb	A, cs_pasw, systskar
	rjmp	tsk_pasw	
systskar:	ret
sys_ntsk:	mov	A, tsk_sts
	jnz	A, sysntskr
	jnb	evn_sts, s_evclk, sysntskr
	rcall	lcdtime
	jnb	evn_sts, s_evmin, sysntskr
	rcall	lcddate
sysntskr:	ret
;=======================================
.include "tsk\tskstgl.inc"
.include "tsk\tskpasw.inc"
.include "tsk\tskmenu.inc"
;---------------------------------------
.include "inc\sysrtc.inc"
.include "inc\syslcd.inc"
;---------------------------------------
;.include "tmp\sysdbg.inc"
;---------------------------------------

;---------------------------------------
;=======================================
; end of program
;=======================================


