;---------------------------------------
;ooooooooooooooooooooooooooooooooooooooo
;	fungsi remote controll 
;=======================================
.equ		sp_dlybtn	 = 31
.equ		sp_ectbtn	 = 30
;=======================================
.equ		sp_none	 = 255
.equ		sp_tvav	 = 15
;---------------------------------------
.equ		sp_mute	 = 25
.equ		sp_pns	 = 26
.equ		sp_pip	 = sp_pns
.equ		sp_tmr	 = sp_pns
.equ		sp_menu	 = 20
.equ		sp_zoom	 = 21
.equ		sp_iok	 = 22
.equ		sp_pwr	 = 23
.equ		sp_fca	 = 24
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		sp_iesc	 = 14
.equ		sp_ient	 = 13
.equ		sp_xent	 = sp_tvav
;=======================================
.equ		sp_min	 = 16
.equ		sp_plus	 = 17
.equ		sp_up	 = 18
.equ		sp_dn	 = 19
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;=======================================
;	.org	0x40
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	SM-809E [061] Remote Code
;	Universal remote code 061
;sssssssssssssssssssssssssssssssssssssss
ird_data:	.db	7,1,1,1,0,0,0,0,0,0,0,sp_up	; ch+
	.db	7,1,1,1,1,0,0,0,0,0,0,sp_plus	; vol+
	.db	7,1,1,1,1,1,1,1,1,1,1,sp_pip	; pip,pns,tmr
	.db	7,1,1,1,1,1,2,0,0,0,0,8	; 8
	.db	7,1,1,1,1,2,0,0,0,0,0,sp_zoom ; zoom
	.db	7,1,1,1,1,3,0,0,0,0,0,4	; 4
	.db	7,1,1,1,2,0,0,0,0,0,0,sp_menu ; menu
	.db	7,1,1,1,2,1,0,0,0,0,0,sp_tvav ; tv-av
	.db	7,1,1,1,2,2,0,0,0,0,0,6	; 6
	.db	7,1,1,1,3,0,0,0,0,0,0,sp_mute ; mute
	.db	7,1,1,1,3,1,0,0,0,0,0,0	; 0
	.db	7,1,1,1,4,0,0,0,0,0,0,2	; 2
	.db	7,1,1,2,0,0,0,0,0,0,0,sp_dn   ; ch-
	.db	7,1,1,2,1,0,0,0,0,0,0,sp_iok	; sok
	.db	7,1,1,2,1,1,0,0,0,0,0,sp_pwr	; pwr
	.db	7,1,1,2,1,1,1,0,0,0,0,sp_fca	; A
	.db	7,1,1,2,1,2,0,0,0,0,0,7	; 7
	.db	7,1,1,2,2,0,0,0,0,0,0,sp_ient ; #
	.db	7,1,1,2,2,1,0,0,0,0,0,sp_iesc ; *
	.db	7,1,1,2,3,0,0,0,0,0,0,3	; 3
	.db	7,1,1,3,0,0,0,0,0,0,0,sp_min	; vol-
	.db	7,1,1,3,2,0,0,0,0,0,0,5	; 5
	.db	7,1,1,4,1,0,0,0,0,0,0,9	; 9
	.db	7,1,1,5,0,0,0,0,0,0,0,1	; 1
	.db	255,255,255,255,255,255
	.db	255,255,255,255,255,255
	.db	255,255,255,255,255,255
	.db	255,255,255,255,255,255
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;ooooooooooooooooooooooooooooooooooooooo
.equ		v_irdph	 = v_irdata+0
.equ		v_irdpl	 = v_irdata+1
.equ		v_irclk	 = v_irdata+2
.equ		v_irpos	 = v_irdata+3
.equ		v_evird	 = v_irdata+4
.equ		v_irdly	 = v_irdata+5
.equ		v_irtd	 = v_irdata+6
.equ		v_evbtn	 = v_irdata+7
.equ		v_dlybtn	 = v_irdata+8
;=======================================
.equ		s_ird	 = 0
.equ		s_irpress	 = 1
.equ		s_irerror	 = 2
.equ		s_ircode	 = 3
.equ		s_irtd	 = 4
.equ		s_evkey	 = 5
.equ		s_dlybtn	 = 6
;---------------------------------------
.equ		ird_sen	= 0
.equ		ird_inp	= PINB
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.macro	pushird
	sts	v_irdpl, ZL 
	sts	v_irdph, ZH
.endm
.macro	popird
	lds	ZL, v_irdpl 
	lds	ZH, v_irdph
.endm
;---------------------------------------
.equ		c_irtclk	= 8
.equ		c_irtend	= 64
.equ		c_irtd	= 22
.equ		c_dtaidx	= 12
.equ		c_dlybtn	= 30 ; 1,5 s
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ird_sclk:	lds	A, v_irdly
	cjne	A, c_irtend, irdclk11
	jnb	ird_sts, s_irpress, irdclk1s
	clrb	ird_sts, s_irpress
	jb	ird_sts, s_irerror, irdclksr
	push	ZH
	push	ZL
	push	R0
	popird
	adiw	ZH:ZL, c_dtaidx-1
	lpm
	sts	v_evird, R0
	setb	ird_sts, s_ircode	
irdclksd:	pop	R0
	pop	ZL	
	pop	ZH
irdclkse:	smov	v_irdly, 0
irdclksr:	ret
irdclk1s:	dec	A
irdclk11:	inc	A
	sts	v_irdly, A
	jnbi	ird_inp, ird_sen, irdclk2s
	jnb	ird_sts, s_ird, irdclksr
	clrb	ird_sts, s_ird
	cpi	A, (c_irtclk + 1)
	brsh	irdclk12
	lds	A, v_irclk
	inc	A
	sts	v_irclk, A
	rjmp	irdclkse
irdclk12:	setb	ird_sts, s_irpress
	clrb	ird_sts, s_irerror
	smov	v_irclk, 0
	smov	v_irpos, 0
	smov	v_irdpl, low(ird_data * 2)
	smov	v_irdph, high(ird_data * 2)
	rjmp	irdclkse
irdclk2s:	jb	ird_sts, s_ird, irdclksr
	setb	ird_sts, s_ird
	push	ZH
	push	ZL
	push	R0
	cpi	A, (c_irtclk + 1)
irdclk23:	brlo	irdclksd
	jb	ird_sts, s_irerror, irdclksd
	jnb	ird_sts, s_irpress, irdclksd
	lds	A, v_irclk
	jz	A, irdclksd
irdclk24:	popird
	lds	A, v_irpos
	add	ZL, A
	ldi	A, 0
	adc	ZH, A
	lpm
	mov	B, R0
	cpi	B, 0xff
	brne	irdclk25
	setb	ird_sts, s_irerror
	rjmp	irdclksd
irdclk25:	lds	A, v_irclk
	cp	B, A
	brne	irdclk26
	sinc	v_irpos
	smov	v_irclk, 0
	rjmp	irdclksd
irdclk26:	popird
	ldi	A, c_dtaidx
	add	ZL, A
	ldi	A, 0
	adc	ZH, A
	pushird
	rjmp	irdclk24
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;ooooooooooooooooooooooooooooooooooooooo
;	event ir data (remote)
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
get_evir:	jnb	ird_sts, s_irtd, getevirs
	sdjnz	v_irtd, getevirs
	clrb	ird_sts, s_irtd
getevirs:	jnb	ird_sts, s_ircode, getevirr
	clrb	ird_sts, s_ircode
	smov	v_irtd, c_irtd
	jb	ird_sts, s_irtd, getevirr
	setb	ird_sts, s_irtd
	setb	ird_sts, s_evkey
getevirr:	ret
;=======================================
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
get_evbt:	jnb	ird_sts, s_evkey, getevbts
	clrb	ird_sts, s_evkey
	lds	A, v_evird
	sts	v_evbtn, A
	setb	evn_sts, s_evbtn
str_kdly:	smov	v_dlybtn, c_dlybtn
	clrb	ird_sts, s_dlybtn
	ret
getevbts:	jb	ird_sts, s_dlybtn, getevbtr
	sdjnz	v_dlybtn, getevbtr
	setb	ird_sts, s_dlybtn
	lds	A, v_evbtn
	cpi	A, 10
	brsh	getevbtr
	smov	v_evbtn, sp_dlybtn
	setb	evn_sts, s_evbtn
getevbtr:	ret
;=======================================
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
