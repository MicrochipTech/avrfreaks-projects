;----------------------------------------------------------------------------------------------------
;------------------- MAIN LOOP & INIT ---------------------------------------------------------------
;----------------------------------------------------------------------------------------------------
.include	"m8515def.inc"

;**********************************************************************
;timer Int
.equ	tiREL			=	227	
;max.pwm steps
.equ	pwmMAX		=	0x1f	;0..31
;
;**********************************************************************
.equ	slotCNT		=	0x60	;time slot counter
.equ	Ipwm			=	0x61	;ist wert
.equ	SpwmRED		=	0x62	;rot soll wert
.equ	SpwmGREEN		=	0x63	;grün soll wert
.equ	SpwmBLUE		=	0x64	;blau soll wert
.equ	LEDcnt		=	0x65	;led counter

;**********************************************************************
;RGB...
.equ	RGBpointer		=	0x66
.equ	loadPOINTERed	=	0x67
.equ	loadPOINTER		=	0x68
;counter
.equ	TEcntL		=	0x69
.equ	TEcntH		=	0x6a
.equ	TEmaxLD		=	0x19	;2fff		;zeit


;loader flags
.equ	RGBdoFLAG		=	0x6b
;.equ	newLOADED		=	0
.equ	EDITrun		=	1
.equ	LOADe2p		=	2
.equ	SEQUslot		=	3

;dynapara flags
.equ	udFLAG		=	0x6c
.equ	udRED			=	0
.equ	udGREEN		=	1
.equ	udBLUE		=	2





;funktion flags
.equ	RGBfunkA		=	0x6d

.equ	fullCOPY		=	0
.equ	lineCOPY		=	1
.equ	singleCOPY		=	2
.equ	toggleFU		=	3
.equ	dimmFU		=	4
.equ	blendFU		=	5
.equ	changeSZEN		=	6
.equ	sequFU		=	7

.equ	RGBfunkB		=	0x6e
.equ	AtogFL		=	0
.equ	BtogFL		=	1
.equ	togDIM		=	2


;......................................................................
;e2p adressing
.equ	E2Pstart		=	0x9	;speicherbereich ab 0x9
.equ	E2Ppointer		=	0x6f

;................................................................
;RGB RAM
.equ	RGBstorage		=	0x96
.equ	edRGBstorage	=	0xfa



;...............................................................
;help var
.equ	helpVARa		=	0x70
.equ	helpVARb		=	0x71

.equ	helpVARred		=	0x72
.equ	helpVARgreen	=	0x73
.equ	helpVARblue		=	0x74

.equ	rgbHELL		=	0x75
.equ	cntSZEN		=	0x76
.equ	cntTIM		=	0x77

;szene
.equ	szene			=	0x78

;................................................................
;i2c slave interface
.equ	i2cFLAG		=	0x79		;i2c status flags
;flags
.equ	i2cCST		=	0		;start condition erkannt
.equ	i2cSTOP		=	1		;stop condition generieren
.equ	i2cNACK		=	2		;nack
.equ	i2cACK		=	3		;acknowledge senden
.equ	i2cSDA		=	4		;temp. ablage pinlevel
.equ	i2cSCL		=	5		; "         "        "
.equ	i2cACTIV		=	6		;kommunikation aktiv
.equ	i2cBUSY		=	7		;bus nicht frei...


;
.equ	i2cTIMER		=	0x7a		;timer	
.equ	i2cADR		=	0x7b		;i2c slaveadress
.equ	i2bBcnt		=	0x7c		;bit counter
.equ	i2cDAT		=	0x7d		;data
.equ	i2cSTATE		=	0x7e		;status

;i2c if
.equ	sclPIN		= 	6		; SCL    Pin#6 (portd)
.equ	sdaPIN		= 	7		; SDA    Pin#7 (portd)
;konstant
.equ	i2cTIT		=	10		;3timeout konstante
.equ	i2cSLAVEadr		=	0xf8		;1111 100x	;master setzt x=0 write

;................................................................................
;temp. var
.equ	rgbHELLte		=	0x7f
.equ	cntSZENte		=	0x80
.equ	cntTIMte		=	0x81
.equ	szeneTE		=	0x82
.equ	RGBfunkAte		=	0x83
.equ	RGBdoFLAGte		=	0x84
	
;----------------------------------------------------------------
;interrupt adressen
;----------------------------------------------------------------

.cseg
.org	0
	;------------------> jump table fuer at90s8515!!!
	rjmp	init			   ;RESaddr =0x000	;Reset vector Adress

.org	0x7
	rjmp	_SLOT000make			    ;OVF0addr=0x07	;Overflow 0 Interrupt Vector Address

;----------------------------------------------------------------------
;init counters,stack,timer...
;----------------------------------------------------------------------
.org	0xf
init:
	;stack
	ldi	r16,high(ramend)
	out	sph,r16
	ldi	r16,low(ramend)
	out	spl,r16
	



;-------------------clear sram complete 0x100 headroom ! ----------------------------------------------------------------------------
	clr	r16
	ldi	zh,0
	ldi	zl,0x60	;sram startadresse
_CLRsram:
	st	z+,r16
	
	cpi	zl,low(ramend)	;Compare low byte
	ldi	r17,high(ramend)	;
	cpc	zh,r17		;Compare high byte
	;
	breq	_TI0init
	rjmp	_CLRsram

;----------------------------------------------------------------------
_TI0init:
	;timer0 overflow-interrupt alle 32,5µs

	;Fck(7,3728Mhz)/8=921600Hz/30= 30720Hz <=> 1/30720 =32,55µs
	;
	;tiREL=(256+1)-30= 227
	clr	r16
	sbr	r16,1<<toie0	;enable overflow interrupt
	out	timsk,r16
	clr	r16
	sbr	r16,1<<cs01	
	out	tccr0,r16		;prescale /8
	
	ldi	r16,tiREL		;µs reload 
	out	tcnt0,r16
;-----------------------------------------------------------------------
	clr	r16
	out	portb,r16
	ldi	r16,0b00000111
	out	ddrb,r16
	;
	ldi	r16,0xff
	out	porta,r16
	out	ddra,r16
	out	portc,r16
	out	ddrc,r16
	;i2c
	ldi	r16,0b11000000	;6&7 pullup TEST 
	out	portd,r16
	ldi	r16,0b00111111	;6&7 input
	out	ddrd,r16

;----------------------------------------------------------------------
;TEST
	ldi	r16,0x1f
	sts	SpwmRED,r16		;red soll-counter
	ldi	r16,0x1f
	sts	SpwmGREEN,r16	;green soll-counter
	ldi	r16,0x1f
	sts	SpwmBLUE,r16	;blue soll-counter
	;rgb do flags =0
	clr	r16
;	sbr	r16,1<<EDITrun
;	sbr	r16,1<<LOADe2p
	sts	RGBdoFLAG,r16
	sts	RGBdoFLAGte,r16

	;loader funktion
	clr	zh
	sts	RGBfunkB,zh

;	sbr	zh,1<<fullCOPY
;	sbr	zh,1<<toggleFU
;	sbr	zh,1<<sequFU
;	sbr	zh,1<<dimmFU
;	sbr	zh,1<<lineCOPY
;	sbr	zh,1<<blendFU
	sts	RGBfunkA,zh
	sts	RGBfunkAte,zh

	;e2p adressing default
	clr	zh
	sts	E2Ppointer,zh
	sts	loadPOINTER,zh
	sts	loadPOINTERed,zh

	;help var's
	ldi	zh,pwmMAX
	sts	helpVARred,zh
	sts	helpVARgreen,zh
	sts	helpVARblue,zh

	;helligkeit max.
	ldi	zh,pwmMAX
	sts	rgbHELL,zh	;0=max. helligkeit  pwmMAX= aus
	sts	rgbHELLte,zh	;0=max. helligkeit  pwmMAX= aus

	;szenentimer
	ldi	zh,TEmaxLD
	sts	cntSZEN,zh
	sts	cntSZENte,zh

	;ablauftimer
	ldi	zh,TEmaxLD
	sts	cntTIM,zh
	sts	cntTIMte,zh
	;szene 0-9
	ldi	zh,0	
	sts	szene,zh
	sts	szeneTE,zh

	;set slave adress
	ldi	zh,i2cSLAVEadr
	sts	i2cADR,zh

	;i2c flags
	clr	r17
	sbr	r17,1<<i2cBUSY	;bus scannen
	sts	i2cFLAG,r17

;----------------------------------------------------------------------
	sei
_MAINloop:
	rjmp	_MAINloop
;----------------------------------------------------------------------



.include "TIslotB.asm"
.include "PWMloopB.asm"
.include "PWMloader.asm"
.include "table.asm"
.include "i2cSLAVEX.asm
