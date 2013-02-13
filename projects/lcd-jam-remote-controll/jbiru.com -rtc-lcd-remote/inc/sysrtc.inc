;=======================================
;	system RTC DS1302
;---------------------------------------
.equ		v_sec	= v_rtcdta
.equ		v_min	= v_rtcdta+1
.equ		v_hour	= v_rtcdta+2
.equ		v_date	= v_rtcdta+3
.equ		v_month	= v_rtcdta+4
.equ		v_day	= v_rtcdta+5
.equ		v_year	= v_rtcdta+6
.equ		v_contr	= v_rtcdta+7
.equ		v_tsec	= v_rtcdta+8
.equ		v_tmin	= v_rtcdta+9
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		rtc_slc	= 4 ; [7]
.equ		rtc_sda	= 2 ; [6]
.equ		rtc_rst	= 3 ; [5]
.equ		rtc_ddr	= DDRC
.equ		rtc_inp	= PINC
.equ		rtc_out	= PORTC
;---------------------------------------
rtc_sdly:	push	B
	ldi	B, 20
rtcsdlys:	djnz	B, rtcsdlys
	pop	B
	ret
;=======================================
;	Fungsi umum RTC
;---------------------------------------
.equ		rtc_sec	= 0
.equ		rtc_min	= 1
.equ		rtc_hour	= 2
.equ		rtc_date	= 3
.equ		rtc_month	= 4
.equ		rtc_day	= 5
.equ		rtc_year	= 6
.equ		rtc_cont	= 7
.equ		rtc_tric	= 8
;=======================================
sys_sclk:	ldi	A, rtc_sec
	rcall	rtc_read
	lds	B, v_tsec
	cp	A, B
	breq	syssclkr
	sts	v_tsec, A
	setb	evn_sts, s_evclk
syssclks:	ldi	XH, high(v_rtcdta)
	ldi	XL, low(v_rtcdta)
	ldi	A, 0b10111111
	sbi	rtc_out, rtc_rst
	rcall	rtc_sout
	ldi	B, 8
syssclk1:	rcall	rtc_sinp
	st	X+, A
	djnz	B, syssclk1
	cbi	rtc_out, rtc_rst
;---------------------------------------
	lds	A, v_min
	lds	B, v_tmin 
	cp	A, B
	breq	syssclkr
	sts	v_tmin, A
	setb	evn_sts, s_evmin
syssclkr:	ret
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rtc_init:	sbi	rtc_ddr, rtc_sda
	sbi	rtc_ddr, rtc_slc
	sbi	rtc_ddr, rtc_rst
	cbi	rtc_out, rtc_sda
	cbi	rtc_out, rtc_slc
	cbi	rtc_out, rtc_rst
	rcall	clr_con
	ldi	A, rtc_sec
	rcall	rtc_read
	jnb	A, 7, rtcinits
	clrb	A, 7
	rcall	set_det
rtcinits:	ldi	A, 0b10101011
	ldi	C, rtc_tric
	rcall	rtc_save
	ret
;---------------------------------------
rtc_sout:	push	B
	ldi	B, 8
rtcsouts:	cbi	rtc_out, rtc_slc
	cbi	rtc_out, rtc_sda
	sbrc	A, 0
	sbi	rtc_out, rtc_sda
	rcall	rtc_sdly
	sbi	rtc_out, rtc_slc
	rcall	rtc_sdly
	ror	A
	djnz	B, rtcsouts
	cbi	rtc_out, rtc_slc
	pop	B
	ret
rtc_sinp:	push	B
	cbi	rtc_ddr, rtc_sda
	ldi	B, 8
	ldi	A, 0x0
rtcsinps:	cbi	rtc_out, rtc_slc
	rcall	rtc_sdly
	sbi	rtc_out, rtc_slc
	clc
	sbic	rtc_inp, rtc_sda
	sec
	ror	A
	rcall	rtc_sdly
	djnz	B, rtcsinps
	cbi	rtc_out, rtc_slc
	sbi	rtc_ddr, rtc_sda
	pop	B
	ret
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rtc_read:	rol	A
	sbr	A, (1<<7)
rtcreads:	sbr	A, (1<<0)
	sbi	rtc_out, rtc_rst
	rcall	rtc_sout
	rcall	rtc_sinp
	cbi	rtc_out, rtc_rst
	ret
ram_read:	rol	A
	sbr	A, (1<<7)
	sbr	A, (1<<6)
	rjmp	rtcreads
ram_save:	push	A
	mov	A, C
	rol	A
	sbr	A, (1<<7)
	sbr	A, (1<<6)
	rjmp	rtcsaves
rtc_save:	push	A
	mov	A, C
	rol	A
	sbr	A, (1<<7)
rtcsaves:	cbr	A, (1<<0)
	sbi	rtc_out, rtc_rst
	rcall	rtc_sout
	pop	A
	rcall	rtc_sout
  	cbi	rtc_out, rtc_rst
	ret
;---------------------------------------
;ooooooooooooooooooooooooooooooooooooooo
;	Setting_Clock
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
clr_con:	ldi	A, 0
	ldi	C, rtc_cont
	rjmp	rtc_save
set_det:	ldi	C, rtc_sec
	rjmp	rtc_save
set_men:	ldi	C, rtc_min
	rjmp	rtc_save
set_jam:	ldi	C, rtc_hour
	rjmp	rtc_save
set_tgl:	ldi	C, rtc_date
	rjmp	rtc_save
set_bln:	ldi	C, rtc_month
	rjmp	rtc_save
set_day:	ldi	C, rtc_day
	rjmp	rtc_save
set_thn:	ldi	C, rtc_year
	rjmp	rtc_save
;---------------------------------------
;=======================================
