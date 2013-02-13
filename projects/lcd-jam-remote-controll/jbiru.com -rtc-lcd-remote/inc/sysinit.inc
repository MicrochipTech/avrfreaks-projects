;---------------------------------------
;=======================================
sys_init:
;---------------------------------------
tma_init:	outp	ASSR, 0
	outp	TCCR2, 3 | (1<<WGM21)
	outp	TCNT2, 0
	outp	OCR2, c_tmra
;---------------------------------------
;	enable interupt timer 2
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	outp	TIMSK, (1<<OCIE2)
;=======================================
;---------------------------------------
.equ		baudrate	= 115200
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		n_ubrrh	= 0
.equ		n_ubrrl	= f_cpu/8/baudrate-1
.equ		n_ucsra	= (1<<U2X)
.equ		n_ucsrb	= (1<<RXEN) | (1<<TXEN)
.equ		n_ucsrc	= (1<<URSEL) | (3<<UCSZ0)
;=======================================
usc_init:	outp	UBRRH, n_ubrrh
	outp	UBRRL, n_ubrrl
	outp	UCSRA, n_ucsra
	outp	UCSRB, n_ucsrb
	outp	UCSRC, n_ucsrc
;--------------------------------------
	smov	v_ita, c_ita
	smov	v_itb, c_itb
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	clr	sys_sts
	clr	ird_sts
	clr	exa_sts	
	clr	evn_sts
	clr	tsk_sts
	ret
;======================================
;	internal EEPROM
;--------------------------------------
put_eepr:	in 	R1, SREG
	out	EEARL, YL
	out	EEARH, YH
	out	EEDR, A
	cli
	sbi	EECR, EEMWE
	sei
	sbi	EECR, EEWE
puteeprs:	sbic	EECR, EEWE
	rjmp	puteeprs
	out	SREG, R1
	ret
get_eepr:	sbic	EECR,EEWE
	rjmp	get_eepr
	out	EEARH, YH
	out 	EEARL, YL
	sbi 	EECR, EERE
	in	A, EEDR
	ret
;======================================
;	usart
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sid_sout:	;----------------------------
usc_sout:	jnbi	UCSRA, UDRE, usc_sout
	out	UDR, A
	ret
sid_sinp:	;-----------------------------
usc_sinp:	jnbi	UCSRA, RXC, usc_sinp
	in	A, UDR
uscsinpr:	ret
usc_xstr:	lpm	
	mov	A, R0
	cpi	A, eop
	breq	uscsinpr
	rcall	usc_sout
	adiw	ZH:ZL, 1
	rjmp	usc_xstr
;---------------------------------------
