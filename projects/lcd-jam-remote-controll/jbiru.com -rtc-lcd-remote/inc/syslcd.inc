;=======================================
;	system LCD
;---------------------------------------
.equ		lcd_out	= PORTB
.equ		lcd_ddr	= DDRB
.equ		lcd_inp	= PINB
.equ		lcd_enb	= 3
.equ		lcd_srd	= 2
.equ		lcd_rsd	= 1
;---------------------------------------
.equ		lsd_out	= PORTC
.equ		lsd_ddr	= DDRC
.equ		lsd_act	= 7
.equ		c_lcdport	= 0b11111110
;=======================================
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;=======================================
;	Print to LCD
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		c_lcddly	= f_cpu/1000/150
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.equ		c_lcdsstr	= 0
.equ		c_lcdinia	= 3
.equ		c_lcdinib	= 4
.equ		c_lcdinic	= 5
.equ		c_lcdinid	= 6
.equ		c_lcdinie	= 7
.equ		c_lcdinif	= 8
.equ		c_lcdinig	= 9
.equ		c_lcdinih	= 10
.equ		c_lcdinii	= 11
;---------------------------------------
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
lcd_init:	outp	lcd_ddr, c_lcdport
	outp	lcd_out, c_lcdport & ~(1<<lcd_srd)
	sbi	lsd_ddr, lsd_act
	sbi	lsd_out, lsd_act
	rjmp	str_slcd
;---------------------------------------
lcd_enbs:	ldi	A, c_lcddly
lcd_dly:	djnz	A, lcd_dly
	sbi	lcd_out, lcd_enb
	ldi	A, c_lcddly
lcddlys:	djnz	A, lcddlys
	cbi	lcd_out, lcd_enb
	ret
;---------------------------------------
lcdIns:	mov	R0, A
	andi	A, 0xf0
	out	lcd_out, A
	rcall	lcd_enbs
	mov	A, R0
lcdInit:	swap	A
	andi	A, 0xf0
	out	lcd_out, A
	rcall	lcd_enbs
	mov	A, R0
	ret
toxleft:	rcall	toLeft
	djnz	C,toxLeft
	ret
toxright:	rcall	toRight
	djnz	C,toxright
	ret
toLeft:	ldi	A, 0b00010000
	rjmp	lcdIns
toRight:	ldi	A, 0b00010100
	rjmp	lcdIns
shRight:	rcall	ToRight
	ldi	A, 0b00011000
	rjmp	lcdIns
toHome:	ldi	A, 0b00000010
	rjmp	lcdIns
curShow:	ldi	A, 0b00001101
	rjmp	lcdIns
curHide:	ldi	A, 0b00001100
	rjmp	LcdIns
gotoxy:	sbr	A, (1<<7)
	rjmp	lcdIns
lcdnum:	ori	A, 0x30
lcdprn:	mov	R0, A
	andi	A, 0xf0
	sbr	A, (1<<lcd_rsd)
	out	lcd_out, A
	rcall	lcd_enbs
	mov	A, R0
	swap	A
	andi	A, 0xf0
	sbr	A, (1<<lcd_rsd)
	out	lcd_out, A
	rcall	lcd_enbs
	ret
lcdline1:	ldi	A, 0
	rjmp	gotoxy
lcdline2:	ldi	A, 40
	rjmp	gotoxy
clrLine:	push	D
	ldi	D, 16
clrlinea:	ldi	A, ' '
	rcall	lcdprn
	djnz	D, clrlinea
	pop	D
	ret
clrLines:	rcall	lcdline1
	rcall	clrLine
clrLine2:	rcall	lcdline2
	rcall	clrLine
	rjmp	lcdline2
clrLine1:	rcall	lcdline1
	rcall	clrLine
	rjmp	lcdline1
;---------------------------------------
lcd_str:
lcdprint:	lpm
	mov	A, R0
	cjne	A, eop, lcdprnts
	ret
lcdprnts:	rcall	lcdprn
	adiw	ZH:ZL, 1
	rjmp	lcdprint
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;=======================================
off_slcd:	clrb	sys_sts, s_lcdact
	sbi	lsd_out, lsd_act
	ret
str_slcd:	ldi	A, c_lcdsstr
	mov	v_act, A
	setb	sys_sts, s_lcdini
	cbi	lsd_out, lsd_act
strslcdr:	ret
sys_slcd:	jnb	sys_sts, s_lcdini, strslcdr
sysslcds:	inc	v_act
	mov	A, v_act
	cjne	A, c_lcdinia, sysslcd1
	ldi	A, 0b0011
	rjmp	lcdInit
sysslcd1:	cjne	A, c_lcdinib, sysslcd2
	ldi	A, 0b0011
	rjmp	lcdInit
sysslcd2:	cjne	A, c_lcdinic, sysslcd3
	ldi	A, 0b0011
	rjmp	lcdInit
sysslcd3:	cjne	A, c_lcdinid, sysslcd4
	ldi	A, 0b0011
	rcall	lcdInit
	ldi	A, 0b0010
	rcall	lcdInit
	ldi	A, 0b00101000
	rjmp	lcdIns
sysslcd4:	cjne	A, c_lcdinie, sysslcd5
	ldi	A, 0b00000110
	rjmp	lcdIns
sysslcd5:	cjne	A, c_lcdinif, sysslcd6
	ldi	A, 0b00000001
	rjmp	lcdIns
sysslcd6:	cjne	A, c_lcdinig, sysslcd7
	ldi	A, 0b00001100
	rjmp	lcdIns
sysslcd7:	cjne	A, c_lcdinih,sysslcd8
	ldi	A, 0b00010100
	rjmp	lcdIns
sysslcd8:	cjne	A, c_lcdinii,sysslcdr
	setb	sys_sts, s_lcdact
	clrb	sys_sts, s_lcdini
	setb	sys_sts, s_evslcd
sysslcdr:	ret
;=======================================
;ooooooooooooooooooooooooooooooooooooooo
;---------------------------------------
bcdascii:	push	B
	mov	B, A
	andi	A, 0x0f
	mov	C, A
	mov	A, B
	swap	A
	andi	A, 0x0f
	pop	B
	ret
;---------------------------------------
.equ		c_spr_sdate= '-'
.equ		c_spr_stime= ':'
;---------------------------------------
strHari:	.db	"Minggu, ", eop, 0
	.db	"Senin, ", eop, 0, 0
	.db	"Selasa, ", eop, 0
	.db	"Rabo, ", eop, 0, 0, 0
	.db	"Kamis, ", eop, 0, 0
	.db	"Jum",39,"at, ", eop, 0
	.db	"Sabtu, ", eop, 0, 0
;---------------------------------------
lcdDay:	jz	A, lcdDayr
	dec	A
	table	strHari
	jnz	A, lcdDays
	rjmp	lcdprint
lcdDays:	cpi	A, 7
	brlo	lcdDay1
lcdDayr:	ret
lcdDay1:	adiw	ZH:ZL, 10
	djnz	A, lcdDay1
	rjmp	lcdprint
;=======================================
lcdfdta:	rcall	bcdascii
	rcall	lcdnum
	mov	A, C
	rjmp	lcdnum
lcdsdta:	ldi	A, c_spr_stime
	rjmp	lcdsdtc
lcdsdtb:	ldi	A, c_spr_sdate
lcdsdtc:	rjmp	lcdprn
lcddate:	rcall	clrline1
lcddates:	lds	A, v_day
	rcall	lcdday
	lds	A, v_date
	rcall	lcdfdta
	rcall	lcdsdtb
	lds	A, v_month
	rcall	lcdfdta
	rcall	lcdsdtb
	lds	A, v_year
	rjmp	lcdfdta
lcdtime:	rcall	clrline2
lcdtimes:	lds	A, v_hour
	rcall	lcdfdta
	rcall	lcdsdta
	lds	A, v_min
	rcall	lcdfdta
	rcall	lcdsdta
	lds	A, v_sec
	rjmp	lcdfdta
;=======================================
;---------------------------------------
;	end lcd
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
