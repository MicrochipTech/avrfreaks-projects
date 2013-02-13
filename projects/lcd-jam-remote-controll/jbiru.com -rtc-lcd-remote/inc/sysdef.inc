;=======================================
;	Program seri AVR
;=======================================
;	definisi variable utama
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		cr	= 13
.equ		lf	= 10
.equ		esc	= 27
.equ		eop	= 26
.equ		sod	= 25
.equ		nod	= 24
.equ		eod	= 23
;=======================================
.equ		f_cpu	= 11059200
;---------------------------------------

;=======================================
;	maping internal EEPROM
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.dseg
.org	0
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	standart EEPROM content 
;---------------------------------------
ad_jbiruinf:	.byte	40
ad_sid:		.byte	1
ad_prodID:	.byte	16
ad_proddate:	.byte	6
ad_prodstr:	.byte	24
;---------------------------------------
ad_admin:		.byte	8
ad_opts:		.byte	1
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Users application
;---------------------------------------
ad_ktemp:		.byte	1
ad_limtemp:	.byte	1
ad_offlcd:	.byte	1
ad_chktemp:	.byte	1
ad_dlyON:		.byte	3
ad_dlyOFF:	.byte	3
ad_dlywrk:	.byte	3
ad_xxxxxx:	.byte	1
;---------------------------------------


;=======================================
.def		bkupsreg	= R2
.def		v_act	= R3
;---------------------------------------
.def		sys_sts	= R4
.def		ird_sts	= R6
.def		evn_sts	= R7
.def		tsk_sts	= R8
.def		exa_sts	= R5
;---------------------------------------
;	sys_sts
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		s_base	= 0
.equ		s_ita	= 1
.equ		s_itb	= 2
.equ		s_lcdini	= 3 
.equ		s_lcdact	= 4
.equ		s_evslcd	= 5
.equ		s_barmn	= 6
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;---------------------------------------
;	evn_sts
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		s_evbtn	= 0
.equ		s_evclk	= 1
.equ		s_evmin	= 2
;---------------------------------------
;	tsk_sts
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		cs_stgl	= 0
.equ		cs_pasw	= 1
.equ		cs_menu	= 2
.equ		cs_spsw	= 3
;---------------------------------------
.def		A	= R16
.def		B	= R17
.def		C	= R18
.def		D	= R19
.def		E	= R20
.def		F	= R21
;=======================================
.dseg
.org	0x64
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ttlstack:		.byte	36 ; total stack pointer
v_syssp:		.byte	1
;=======================================
v_ita:		.byte	1
v_itb:		.byte	1
;---------------------------------------
;	remote controll
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
v_irdata:		.byte	9
;---------------------------------------
;	RTC DS 1302
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
v_rtcdta:		.byte	10
;---------------------------------------
;	Variable task
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
v_pmenu:		.byte	5
v_ptask:		.byte	16
;=======================================

;---------------------------------------
