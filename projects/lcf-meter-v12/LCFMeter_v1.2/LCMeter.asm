
;************************************************************************
;*                            LCF Meter v1.2                            *
;*																		*
;*						 	  by  Hal Foster                            *
;*                              01/24/2009								*
;*																		*
;*																		*
;*		Revisions:	1.0	Initial release									*
;*					1.2 Minor changes to main program itself, major		*
;*						changes to included libraries.  Changes in		*
;*						program mostly to acomodate these.				*
;************************************************************************




.nolist
.include "m88def.inc"
.list



;==========================================================================
;=				         CALIBRATION/CONTROL VALUES							  =
;==========================================================================

.equ	CAL_CAP		=	1000000		; value in pF (1uF = 1000000)
									; any value in the range of .1 uf to 1uF is
									; recommended - while the relative value of
									; the cap isn't critical, the absolute value
									; entered here *is* - as is the stability.
									; A higher value will give slightly better
									; resolution.

;*************************************************************************
.def	AvgA1		= r10
.def	AvgA2		= r11
.def	AvgA3		= r12
.def	AvgA4		= r13


.def	Zero		= r15
.def	Temp		= r16
.def	Temp2		= r17
.def	Temp3		= r18
.def	Temp4		= r19
.def	TCnt_B0		= r20
.def	TCnt_B1		= r21
.def	TCnt_B2		= r22
.def	TCnt_B3		= r23
.def	Arg			= r24
.def	CurMode		= r25
.def	CRange		= r26
.def	Loop		= r27

.equ	CS_DDR		= DDRC
.equ	CS_PORT		= PORTC
.equ	CS_PIN		= PINC
.equ	CS_5UA		= 0
.equ	CS_500UA	= 1
.equ	CS_50MA		= 2
.equ	CS_ZERO		= 3
.equ	CS_PINL		= 4
.equ	CS_PINH		= 5

.equ	SEL_DDR		= DDRB
.equ	SEL_PORT	= PORTB
.equ	SEL_PIN		= PINB
.equ	SEL_F		= 5
.equ	SEL_C		= 4
.equ	SEL_L		= 3
.equ	SEL_ZERO	= 2

.equ	FLAGS		= GPIOR0
.equ	F_T1SEC		= 0
.equ	F_ZEROSW	= 1

;						hgfedcba
.equ	MODE_F		= 0b00010000
.equ	MODE_C		= 0b00001001
.equ	MODE_CCAL	= 0b00001100
.equ	MODE_L		= 0b00100001
.equ	MODE_LCAL	= 0b00100001

.equ	IS_DDR		= DDRD
.equ	IS_PORT		= PORTD
.equ	IS_A		= 0
.equ	IS_B		= 1
.equ	IS_C		= 2
.equ	IS_D		= 3
.equ	IS_E		= 6
.equ	IS_F		= 7


;==========================================================================
;                             LCD Pin Assignments
;==========================================================================
.equ	LCD_DATA_PORT	= PORTB			;Data line
.equ	LCD_DATA_DDR 	= DDRB			;
.equ	LCD_DATA_BIT 	= 1
.equ	LCD_CLK_PORT 	= PORTB			;Clock line
.equ	LCD_CLK_DDR 	= DDRB			;
.equ	LCD_CLK_BIT 	= 0
.equ	LCD_CPL			= 16			; characters per line for module
;==========================================================


.org 0x00

;*************************************************************************
;************************** For Mega48/88/168 ****************************

	rjmp INT_RESET 				; Reset Handler
	reti;rjmp INT_INT0 			; External IRQ0 Handler
	reti;rjmp INT_INT1			; External IRQ1 Handler
	rjmp INT_PCINT0 			; PCINT0 Handler
	reti;rjmp INT_PCINT1 		; PCINT1 Handler
	reti;rjmp INT_PCINT2		; PCINT2 Handler
	reti;rjmp INT_WDT 			; Watchdog Interrupt Handler
	reti;rjmp INT_TIM2_COMPA 	; Timer2 Compare A Handler
	reti;rjmp INT_TIM2_COMPB 	; Timer2 Compare B Handler
	reti;rjmp INT_TIM2_OVF 		; Timer2 Overflow Handler
	reti;rjmp INT_TIM1_CAPT 	; Timer1 Capture Handler
	rjmp INT_TIM1_COMPA		 	; Timer1 Compare A Handler
	reti;rjmp INT_TIM1_COMPB 	; Timer1 Compare B Handler
	rjmp INT_TIM1_OVF	 		; Timer1 Overflow Handler
	reti;rjmp INT_TIM0_COMPA 	; Timer0 Compare A Handler
	reti;rjmp INT_TIM0_COMPB 	; Timer0 Compare B Handler
	rjmp INT_TIM0_OVF	 		; Timer0 Overflow Handler
	reti;rjmp INT_SPI_STC		; SPI Transfer Complete Handler
	reti;rjmp INT_USART_RXC		; USART RX Complete Handler
	reti;rjmp INT_USART_UDRE	; USART UDR Empty Handler
	reti;rjmp INT_USART_TXC		; USART TX Complete Handler
	reti;rjmp INT_ADC 			; ADC Conversion Handler
	reti;rjmp INT_EE_RDY 		; EEPROM Ready Handler
	reti;rjmp INT_ANA_COMP 		; Analog Comparator Handler
	reti;rjmp INT_TWI			; TWI Interface Handler
	reti;rjmp INT_SPM_RDY		; Store Program Memory Ready Handler

.org INT_VECTORS_SIZE


;*************************************************************************
.include "Util.asm"
.include "Math_v2.asm"
.include "LCD_Serial.asm"
.include "Bin2Dec.asm"


;*************************************************************************
.DSEG

OutBuf:			.BYTE   20
OutBufLen:		.BYTE	1
LastL:			.BYTE	4
SelIn:			.BYTE	1

.ESEG

LCal:			.dd		0

CZero5uA:		.dd		0
CZero500uA:		.dd		0
CZero50mA:		.dd		0
CCal5uA:		.dd		0
CCal500uA:		.dd		0
CCal50mA:		.dd		0


.CSEG

LVal:			.db "###0.000"
LValLen:		.db	8
LNeg:			.db "-##0.000"
LNegLen:		.db	8
Freq:			.db	"##,###,##0"
FreqLen:		.db 10
FPStr:			.db " ##0.000"
FPStrLen:		.db 8

CalStr:			.db "Calibrating: ",34,0
CalStr2:		.db	255,255,255,0
CalStrL2:		.db	"Cal:",0

CStr1:			.db	"Cap:",0
CStr2:			.db	"Ran",1,"e:",0
RStr5uA:		.db	"  5 ",0b11100100,"A",0
RStr500uA:		.db	"500 ",0b11100100,"A",0
RStr50mA:		.db	" 50 mA",0

Rstr1:			.db	"#0.000"
Rstr1Len:		.db	6
Rstr2:			.db	" #0.00"
Rstr2Len:		.db	6
Rstr3:			.db	" ###.0"
Rstr3Len:		.db	6
Rstr4:			.db	" .0000"
Rstr4Len:		.db	6
Rstr5:			.db	"##,##0"
Rstr5Len:		.db	6
Rstr6:			.db	"  ###0"
Rstr6Len:		.db	6


Char1:			.db	0b00000000, 0b00000000, 0b00001111, 0b00010001, 0b00010001, 0b00001111, 0b00000001, 0b00001110



SUStr1:			.db "-- LCF Meter --",0
SUStr2:			.db	" v1.2 01-24-09 ",0


;*************************************************************************************

.MACRO	CS_50mA_On							; Turn on 50ma current source
	cbi		CS_PORT, CS_ZERO
	sbi		CS_PORT, CS_50MA
.ENDM

;*****

.MACRO	CS_500uA_On							; Turn on 500ua current source
	cbi		CS_PORT, CS_ZERO
	sbi		CS_PORT, CS_500UA
.ENDM

;*****

.MACRO	CS_5uA_On							; Turn on 5ua current source
	cbi		CS_PORT, CS_ZERO
	sbi		CS_PORT, CS_5UA
.ENDM

;*****

.MACRO	CS_Off								;Turn all current sources off
	rcall	_CS_Off
.ENDM

;*****

.MACRO B2DDef		;B2DDef <MaskPtr>	- most binary to decimal conversions in the app use the same source and buffer
	PushZ
	ldi		ZH, high(@0*2)					;MskLenPtr
	ldi		ZL, low(@0*2)
	rcall	_B2DDefSub
	PopZ
.ENDM

;*****

.MACRO		Timer1_On						; turn on timer1
	SetB	TCCR1B, 0						; no prescaling
.ENDM

;*****

.MACRO		Timer1_Off						; turn timer1 off
	ClrB	TCCR1B, 0
.ENDM


;************************************************************************************
INT_RESET: 									; reset Interrupt
	
	cli
	
;	ldi		ZL, low(SRAM_START)		; ------|
;	ldi		ZH, high(SRAM_START)	;		|
;	clr		r16						;		|
;	ldi		r17, high(RAMEND+1)		;		|
;ELoop:								;		| Initialize SRAM to 0
;	st		Z+, r16					;		| to make debugging.in IDE easier
;	cpi		ZL, low(RAMEND+1)		;		| Can be removed if desired.
;	cpc		ZH, r17					;		|
;	brne	ELoop					;		|
;	clr		r16						;		|
;	clr`	r17						;-------|


	ldi 	Temp,high(RAMEND)				; Set Stack Pointer to top of RAM
	out		SPH, Temp
	ldi 	Temp,low(RAMEND)
	out		SPL, Temp

	SetB	IS_DDR, IS_A					; set up IO pins
	SetB	IS_DDR, IS_B
	SetB	IS_DDR, IS_C
	SetB	IS_DDR, IS_D
	SetB	IS_DDR, IS_E
	SetB	IS_DDR, IS_F
	ClrB	IS_PORT, IS_A
	ClrB	IS_PORT, IS_B
	ClrB	IS_PORT, IS_C
	ClrB	IS_PORT, IS_D
	ClrB	IS_PORT, IS_E
	ClrB	IS_PORT, IS_F

	ldi		CurMode, MODE_C
	rcall	SetMode

	SetB	CS_DDR, CS_50MA
	ClrB	CS_PORT, CS_50MA
	SetB	CS_DDR, CS_500UA
	ClrB	CS_PORT, CS_500UA
	SetB	CS_DDR, CS_5UA
	ClrB	CS_PORT, CS_5UA
	SetB	CS_DDR, CS_ZERO
	SetB	CS_PORT, CS_ZERO
	
	ClrB	CS_DDR, CS_PINL
	ClrB	CS_PORT, CS_PINL
	ClrB	CS_DDR, CS_PINH
	ClrB	CS_PORT, CS_PINH

	ClrB	DDRD, 4							; freq in as input
	ClrB	PORTD, 4						; no pull up

	ClrB	DDRD, 5							; no input, will conect to GND
	ClrB	PORTD, 5						; no pull up




	ClrB	SEL_DDR, SEL_F
	SetB	SEL_PORT, SEL_F
	ClrB	SEL_DDR, SEL_C
	SetB	SEL_PORT, SEL_C
	ClrB	SEL_DDR, SEL_L
	SetB	SEL_PORT, SEL_L
	ClrB	SEL_DDR, SEL_ZERO
	SetB	SEL_PORT, SEL_ZERO
	SetB	PCICR, 0						; enable pin change int on PCINT0-7
	SetB	PCMSK0, 2						; pin PA2 - Zero
	SetB	PCMSK0, 3						; pin PA3 - L
	SetB	PCMSK0, 4						; pin PA4 - C
	SetB	PCMSK0, 5						; pin PA5 - F


	
	Timer1_Off
	StoreI	OutBufLen, 20					; set byte for length of output buffer for Bin2Dec routine
	out		FLAGS, ZERO
	ldi		Temp, 0
	mov		Zero, Temp

	rcall	LCD_Init
	LCD_Cmdi 0b00001100

	
	LCD_CmdI	0b01001000
	ldi		Temp, 8							; send descending "g" to CGRAM
	LoadZ	Char1*2
CharLoop:
	lpm		Temp2, Z+
	LCD_Char	Temp2
	dec		Temp
	brne	CharLoop

	LCD_Locate 0,0
	LCD_CStringZ	SUStr1
	LCD_Locate 1,0
	LCD_CStringZ	SUStr2
	Delay1ms	3000
	LCD_Cls

	ldi 	CurMode, MODE_F					; set startup mode - change to MODE_C or MODE_L if desired
	rcall	SetMode
	sts		SelIn, Zero
	sei
	rjmp Main

;*************************************************************************

INT_TIM1_COMPA:				 				; Timer1 Compare A Handler
	push	Temp							; ISR triggers after a 1 sec delay
	in		Temp, TCNT0						; to capture frequency
	mov		TCnt_B0, Temp					; save lsb of frequency
	Store	TCCR0B, Zero	
	Store	TCCR1B, Zero
	SetB	FLAGS, F_T1SEC					; set 1-sec elapsed flag
	pop		Temp
	reti

;*************************************************************************

INT_TIM0_OVF:			 					; Timer0 Overflow Handler
	push	Temp
	ldi		Temp, 1
	add		TCnt_B1, Temp					; increment 3 msbs of counter
	adc		TCnt_B2, Zero		
	adc		TCnt_B3, Zero
	pop		Temp
	reti

;*************************************************************************

INT_PCINT0:				 					; PCINT0 Handler
	push	Temp							; called when a mode/zero button pressed
	in		Temp, SREG
	push	Temp
	push	Temp2
	Load	Temp, SEL_PIN					; load bit status for port
	ldi		Temp2, 0b00111100				; mask off ones we are interested in
	and		Temp, Temp2
	eor		Temp, Temp2
	sts		SelIn, Temp						; save for later use
	pop		Temp2
	pop		Temp
	out		SREG, Temp
	pop		Temp
	reti


;*************************************************************************

INT_TIM1_OVF:			 					; Timer1 Overflow Handler
	push	Temp
	ldi		Temp, 1
	add		TCnt_B2, Temp					; increment 2 msbs of counter
	adc		TCnt_B3, Zero
	pop		Temp
	reti



;*************************************************************************
;*************************************************************************
;*************************************************************************

Main:
	lds		Temp, SelIn						; load mode/zero switch status
	tst		Temp
	brne	_MDoChange						; if not 0, something changed
	rjmp	_MDoCurMode		

_MDoChange:
	sbrc	Temp, SEL_F						; was F switch pushed?
	rjmp	_MDoF							; yes, change mode
	sbrc	Temp, SEL_C						; was C switch pushed?
	rjmp	_MDoC							; yes, change mode
	sbrc	Temp, SEL_L						; was L switch pushed?
	rjmp	_MDoL							; yes, change mode
	sbrc	Temp, SEL_ZERO					; was Zero switch pushed?
	rjmp	_MDoZero						; yes, zero current mode
	rjmp	_MDone
_MDoF:
	cpi		CurMode, MODE_F					; don't change if already changed
	brne	_MDoF2
	rjmp	_MDone
_MDoF2:
	ldi 	CurMode, MODE_F					; change it
	rcall	SetMode
	LCD_CLS
	rjmp	_MDone
_MDoC:
	cpi		CurMode, MODE_C					; don't change if already changed
	breq	_MDone
	ldi 	CurMode, MODE_C					; change it
	rcall	SetMode	
	LCD_CLS
	rjmp	_MDone
_MDoL:
	cpi		CurMode, MODE_L					; don't change if already changed
	breq	_MDone
	ldi 	CurMode, MODE_L					; change it
	rcall	SetMode
	LCD_CLS
	rjmp	_MDone
_MDoZero:
	SetB	FLAGS, F_ZEROSW					; call calibration routine
	rcall	DoCalibrate	
_MDone:
	sts		SelIn, Zero
	Delay1ms	200							; wait for relays to settle after change
	rjmp	Main

_MDoCurMode:
	cpi		CurMode, MODE_C					; measure C
	brne	_MNoC
	rcall	GetC
	rjmp	_MDoneLoc
_MNoC:
	cpi		CurMode, MODE_L					; measure L
	brne	_MNoL
	rcall	GetL
	rjmp	_MDoneLoc
_MNoL:
	cpi		CurMode, MODE_F					; measure F
	brne	_MDoneLoc
	rcall	GetF
_MDoneLoc:
	rjmp	Main

;*************************************************************************
;*************************************************************************
;*************************************************************************

;*************************************************************************
GetF:


	rcall	_GetFreq						; get freq count
	StoreI8mVar2		10					; adjust for prescaler
	rcall	mul32
	B2DDef	FreqLen
	LCD_Locate	0,0							; display it
	LCD_CharI	'F'
	LCD_CharI	':'
	LCD_CharI	' '
	LCD_DString OutBuf+10, 10
	LCD_CharI	' '
	LCD_CharI	'h'
	LCD_CharI	'z'
	LCD_Locate	1,0
	LCD_CharI	'T'
	LCD_CharI	':'
	rcall CopymVar1to2						; calculate period
	StoreI64mVar1	1000000000000
	rcall	div64r
	StoreI32mVar2		1000000
	rcall	mComp64
	brsh	GF1
	LCD_Locate 1,14							; show in ns
	LCD_CharI	'n'
	LCD_CharI	's'
	rjmp	GFLoc0
GF1:
	StoreI16mVar2		1000
	rcall	Div64
	StoreI32mVar2		1000000
	rcall	mComp64
	brsh	GF2
	LCD_Locate 1,14							; show in us
	LCD_CharI	0b11100100
	LCD_CharI	's'
	rjmp	GFLoc0
GF2:
	StoreI16mVar2		1000
	rcall	Div64
	StoreI32mVar2		1000000
	rcall	mComp64
	brsh	GF3
	LCD_Locate 1,14							; show in ms
	LCD_CharI	'm'
	LCD_CharI	's'
	rjmp	GFLoc0
GF3:
	StoreI16mVar2		1000
	rcall	Div64
	LCD_Locate 1,14							; show in s
	LCD_CharI	's'
GFLoc0:
	B2DDef	FPStrLen
	LCD_Locate	1,5
	LCD_DString OutBuf+12, 8
	ret



;*************************************************************************

GetC:
	
	LCD_Locate	0,2
	LCD_CStringZ	CStr1
	LCD_Locate	0,14
	LCD_CharI	'F'
	LCD_Locate	1,0
	LCD_CStringZ	CStr2
	LCD_Locate	0,6
	ldi		CRange, CS_50MA					; try 50ma
	rcall	GetCTime
	rcall	CCompare
	cpi		Arg, TRUE
	breq	GCTry500uA
	rjmp	GCCalc
GCTry500uA:
	ldi		CRange, CS_500UA				; try 500ua
	rcall	GetCTime
	rcall	CCompare
	cpi		Arg, TRUE
	breq	GCUse5uA
	rjmp	GCCalc
GCUse5uA:
	ldi		CRange, CS_5UA					; use 5ua
	rcall	GetCTime
GCCalc:
	rcall	ClrmVar1
	Store	mVar1_B0, TCnt_B0				; store count
	Store	mVar1_B1, TCnt_B1
	Store	mVar1_B2, TCnt_B2
	Store	mVar1_B3, TCnt_B3
	StoreI32mVar2		CAL_CAP				; store reference cap value
	rcall	mul32							; and multiply
	rcall	ClrmVar2						; 
	cpi		CRange, CS_50MA					; load calibration value depending on range used
	brne	GCNot50ma
	LoadZ	CCal50mA						; set pointer for 50ma
	rjmp	GCCalc2
GCNot50ma:
	cpi		CRange, CS_500uA
	brne	GCNot500ua
	LoadZ	CCal500uA						; set pointer for 500ua
	rjmp	GCCalc2
GCNot500ua:
	LoadZ	CCal5uA							; set pointer for 5ua
GCCalc2:
	rcall	ReadEPROM						; load saved calibration value from EPROM
	Store	mVar2_B3, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_B2, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_B1, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_B0, Arg
	adiw	Z, 1
	rcall	div64							; divide to find DUT value in pf
	StoreI16mVar2		1000
	rcall	mComp64
	brsh	GCT1
	rjmp	GCLoc0							; <1000pf
GCT1:
	StoreI8mVar2		100
	rcall	Div64
	StoreI16mVar2		10000
	rcall	mComp64
	brsh	GCT2
	rjmp	GCLoc1							; <1uf
GCT2:
	StoreI8mVar2		10
	rcall	Div64
	StoreI16mVar2		10000
	rcall	mComp64
	brsh	GCT3
	rjmp	GCLoc2							;<10uf
GCT3:
	StoreI8mVar2		10
	rcall	Div64
	StoreI16mVar2		10000
	rcall	mComp64
	brsh	GCT4
	rjmp	GCLoc3							;<100uf
GCT4:
	StoreI8mVar2		10
	rcall	Div64
	StoreI16mVar2		10000
	rcall	mComp64
	brsh	GCT5
	rjmp	GCLoc4							; <1000uf
GCT5:
	StoreI8mVar2		10
	rcall	Div64
	rjmp	GCLoc5							;>=1000uf
GCLoc0:										; <1000 pf		xxx.x pf
	B2DDef	RStr6Len
	LCD_DString OutBuf+14, 6
	LCD_Locate	0,13
	LCD_CharI	0b11110000					; "p"
	rjmp	GCLDoneB
GCLoc1:										; <   1 uf		.	xxxx uf
	B2DDef	RStr4Len
	LCD_DString OutBuf+14, 6
	rjmp	GCLDone
GCLoc2:										; <  10 uf		x.xxx uf
	B2DDef	RStr1Len
	LCD_DString OutBuf+14, 6
	rjmp	GCLDone
GCLoc3:										; < 100 uf		xx.xx uf
	B2DDef	RStr2Len
	LCD_DString OutBuf+14, 6
	rjmp	GCLDone
GCLoc4:										; <1000 uf		xxx.x uf
	B2DDef	RStr3Len
	LCD_DString OutBuf+14, 6
	rjmp	GCLDone
GCLoc5:										; >=10,000 uf	x,xxx uf
	B2DDef	RStr5Len
	LCD_DString OutBuf+14, 6				; show result
	rjmp	GCLDone
GCLDone:
	LCD_Locate	0,13
	LCD_CharI	0b11100100					; "u"
GCLDoneb:
	LCD_Locate	1,9							; show range used
	cpi		CRange, CS_50MA
	brne	GCLoc6
	LCD_CStringZ	RStr50mA
	rjmp	GCLExit
GCLoc6:
	cpi		CRange, CS_500uA
	brne	GCLoc7
	LCD_CStringZ	RStr500uA
	rjmp	GCLExit
GCLoc7:
	LCD_CStringZ	RStr5uA
GCLExit:	
	Delay1ms	100
	ret


;*************************************************************************

GetCTime:									; times interval for voltage to change from VL to VH

	push	Temp
	Timer1_Off								; make sure timer and current sources are off
	CS_Off
_GCTC:	
	sbis	CS_PIN, CS_PINL					; make sure cap is discharged
	rjmp	_GCTC
	Delay1ms 50								; additional delay in case of voltage "rebound"
	clr		Temp
	StoreI	TCCR1B, 0
	StoreI	TIMSK1, 0b00000001				; enable overflow int
	clr		TCnt_B2
	clr		TCnt_B3
	StoreI	TCNT1H, 0
	StoreI	TCNT1L, 0
	cpi		CRange, CS_5UA					; Load offset for current range and turn current on
	brne	_GCTNot5UA
	LoadZ	CZero5uA
	CS_5uA_On
	rjmp	_GCT0
_GCTNot5UA:
	cpi		CRange, CS_500uA
	brne	_GCTIs50mA
	LoadZ	CZero500uA
	CS_500uA_On
	rjmp	_GCT0
_GCTIs50mA:		
	LoadZ	CZero50mA
	CS_50mA_On
	rjmp	_GCT0							; keep times consistant
_GCT0:	
	sbic	CS_PIN, CS_PINL					; wait for V accross DUT to reach VL
	rjmp	_GCT0
	Timer1_On								; start timerwhen it does
_GCT1:
	sbic	CS_PIN, CS_PINH					; wait for V accross DUT to reach VH
	rjmp	_GCT1
	Timer1_Off								; stop timer when it does
	CS_Off
	Load	Temp, TCNT1L					; load 2 LSBs
	mov		TCnt_B0, Temp
	Load	Temp, TCNT1H
	mov		TCnt_B1, Temp
	
	subi	TCnt_B0, 11						; subtract overhead
	sbc		TCnt_B1, Zero
	sbc		TCnt_B2, Zero
	sbc		TCnt_B3, Zero
	;
	in		Temp, Flags
	sbrc	Temp, F_ZEROSW					; check if being called by zero routine
	rjmp	GCTInCal						; skip next if so
	
	rcall	ClrmVar1
	rcall	ClrmVar2
	Store	mVar1_b0, TCnt_B0				; store count
	Store	mVar1_b1, TCnt_B1
	Store	mVar1_b2, TCnt_B2
	Store	mVar1_b3, TCnt_B3
	rcall	ReadEPROM						; read offset
	Store	mVar2_b3, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b2, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b1, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b0, Arg
	rcall	mComp64
	brlo	GCTNoSub
	rcall	mSub32							; and subtract from count	
	Load	TCnt_B0, mVar1_b0
	Load	TCnt_B1, mVar1_b1
	Load	TCnt_B2, mVar1_b2
	Load	TCnt_B3, mVar1_b3
	rjmp	GCTInCal
GCTNoSub:	
	clr		TCnt_B0
	clr		TCnt_B1
	clr		TCnt_B2
	clr		TCnt_B3
GCTInCal:
	pop		Temp
	ret

;*************************************************************************

CCompare:									; compare count to 100,000 for scaling	
	push	Temp
	ldi		Temp, low(100000)
	cp		TCnt_B0, Temp
	ldi		Temp, byte2(100000)
	cpc		TCnt_B1, Temp
	ldi		Temp, byte3(100000)
	cpc		TCnt_B2, Temp
	ldi		Temp, byte4(100000)
	cpc		TCnt_B3, Temp
	brsh	CCIsLower
	ldi		Arg, TRUE
	rjmp	CCDone
CCIsLower:
	ldi		Arg, FALSE
CCDone:
	pop		Temp
	ret


;*************************************************************************

GetL:
	in		Arg, Flags
	sbrc	Arg, F_ZEROSW					; skip if in calibration 
	rjmp	GLInCal
	LCD_Locate	0, 1						; set up display
	LCD_CharI	'L'
	LCD_CharI	':'
	LCD_Locate	1, 0
	LCD_CharI	'@'
	LCD_CharI	'F'
	LCD_CharI	':'
	LCD_Locate	1, 14
	LCD_CharI	'h'
	LCD_CharI	'z'
GLInCal:
	rcall	_GetFreq						; get frequency
	in		Arg, Flags
	sbrc	Arg, F_ZEROSW
	rjmp	GLInCal2						; skip if in calibration
	B2DDef	FreqLen
	LCD_Locate 1, 3
	LCD_DString OutBuf+10, 10				; display frequency
	cpi		TCnt_B0, 10						; load zero if <= count (test leads open)
	cpc		TCnt_B1, Zero
	cpc		TCnt_B2, Zero
	cpc		TCnt_B3, Zero
	brsh	GLInCal2
	rcall	ClrmVar1
	LCD_Locate	0, 14
	rjmp	GLF1
GLInCal2:	
	StoreI16mVar2		62832				; calculate inductance using 1/(2*pi*sqrt(L*C)) solving for L
	rcall	mul32
	StoreI16mVar2		16384
	rcall	div64r
	rcall	CopymVar1to2
	StoreI64mVar1		97656250000
	rcall	div64r
	rcall	CopymVar1to2
	rcall	mul32
	rcall	CopymVar1to2
	rcall	mAdd64
	StoreI32mVar2		256000				; answer in  nH
	rcall	div64r
	LoadZ	LastL							; store result for later
	Load	Temp, mVar1_b3		
	st		Z+, Temp
	Load	Temp, mVar1_b2		
	st		Z+, Temp
	Load	Temp, mVar1_b1		
	st		Z+, Temp
	Load	Temp, mVar1_b0		
	st		Z+, Temp
	Load	Temp, FLAGS
	sbrc	Temp, F_ZEROSW
	rjmp	GLFEnd

	LoadZ	LCal							; load offset value
	rcall	ReadEPROM		
	mov		Temp4, Arg
	adiw	Z, 1
	rcall	ReadEPROM	
	mov		Temp3, Arg
	adiw	Z, 1
	rcall	ReadEPROM	
	mov		Temp2, Arg
	adiw	Z, 1
	rcall	ReadEPROM	
	mov		Temp, Arg
	Load	Arg, mVar1_b0
	cp		Arg, Temp
	Load	Arg, mVar1_b1
	cpc		Arg, Temp2
	Load	Arg, mVar1_b2
	cpc		Arg, Temp3
	Load	Arg, mVar1_b3
	cpc		Arg, Temp4
	brlo	LIsNeg							; check if result would be negative
	rjmp	LNotNeg
LIsNeg:
	rcall	CopymVar1to2
	rcall	CLRmVar1
	Store	mVar1_b0, Temp
	Store	mVar1_b1, Temp2
	Store	mVar1_b2, Temp3
	Store	mVar1_b3, Temp4
	rcall	mSub32
	B2DDef	LNegLen
	LCD_Locate 0, 5
	LCD_DString OutBuf+12, 8
	ret
LNotNeg:									; subtract offset value
	rcall	ClrmVar2
	Store	mVar2_b0, Temp
	Store	mVar2_b1, Temp2
	Store	mVar2_b2, Temp3
	Store	mVar2_b3, Temp4
	rcall	mSub32
	LCD_Locate	0, 14
	rcall	LCompare
	cpi		Arg, TRUE						; is result less than 1,000,000?
	brne	GLF0
	LCD_CharI	0b11100100					; answer in uh
	LCD_CharI	'h'
	rjmp	GLFDone
GLF0:
	StoreI16mVar2	1000
	rcall	div64r
	rcall	LCompare
	cpi		Arg, TRUE						; is result less than 1,000,000?
	brne	GLF2
GLF1:	
	LCD_CharI	'm'							; answer in mh
	LCD_CharI	'H'
	rjmp	GLFDone
GLF2:
	LCD_CharI	'H'							; answer in H
	LCD_CharI	' '
GLFDone:
	B2DDef	LValLen
	LCD_Locate 0, 5
	LCD_DString OutBuf+12, 8
GLFEnd:	
	ret

;*************************************************************************

LCompare:

	Load	Arg, mVar1_b0					; compare to 1,000,000
	ldi		Temp, low(1000000)
	cp		Arg, Temp
	Load	Arg, mVar1_b1
	ldi		Temp, byte2(1000000)
	cpc		Arg, Temp
	Load	Arg, mVar1_b2
	ldi		Temp, byte3(1000000)
	cpc		Arg, Temp
	Load	Arg, mVar1_b3
	ldi		Temp, byte4(1000000)
	cpc		Arg, Temp
	brlo	LCIsLower
	ldi		Arg, FALSE
	rjmp	LCDone
LCIsLower:
	ldi		Arg, TRUE
LCDone:
	ret

;*************************************************************************************

ReadEPROM:
	sbic	EECR, EEPE
	rjmp	ReadEPROM
	out		EEARH, ZH
	out		EEARL, ZL
	sbi		EECR, EERE
	in		Arg, EEDR
	ret

;*************************************************************************************

WriteEPROM:
	sbic	EECR, EEPE
	rjmp	WriteEPROM
	out		EEARH, ZH
	out		EEARL, ZL
	out		EEDR, Arg
	sbi		EECR, EEMPE
	sbi		EECR, EEPE
	ret

;*************************************************************************************

SetMode:
	
	ClrB	IS_PORT, IS_A					; set output pins for current mode
	ClrB	IS_PORT, IS_B
	ClrB	IS_PORT, IS_C
	ClrB	IS_PORT, IS_D
	ClrB	IS_PORT, IS_E
	ClrB	IS_PORT, IS_F
	sbrc	CurMode, 0
	SetB	IS_PORT, IS_A
	sbrc	CurMode, 1
	SetB	IS_PORT, IS_B
	sbrc	CurMode, 2
	SetB	IS_PORT, IS_C
	sbrc	CurMode, 3
	SetB	IS_PORT, IS_D
	sbrc	CurMode, 4
	SetB	IS_PORT, IS_E
	sbrc	CurMode, 5
	SetB	IS_PORT, IS_F
	Delay1ms	100
	ret

;*************************************************************************

DoCalibrate:
	
	cpi		CurMode, MODE_F					; if current mode is freq, exit (no cal)
	brne	DC0
	ClrB	FLAGS, F_ZEROSW
	ret
DC0:
	push	Arg
	push	Temp
	rcall	ClearAvg
	LCD_Cls
	LCD_Locate 0,0
	LCD_CStringZ 	CalStr
	cpi		CurMode, MODE_L					; calibrating L?
	breq	DoLCal
	cpi		CurMode, MODE_C					; calibrating C?
	breq	_DCIsC
	rjmp	_DCDone
_DCIsC:
	rjmp	DoCCal
DoLCal:
	LCD_CharI	'L'
	LCD_CharI	34
	LCD_Locate 1,0
	LCD_CStringZ	CalStr2
	ldi		CurMode, MODE_LCAL				; change mode for calibration
	rcall	SetMode
	Delay1ms	500
	rcall	GetL
	rcall	GetL
	ldi		Arg, 4
_DLCLoop0:									; get 4 samples
	push	Arg
	rcall	GetL
	pop		Arg
	LoadZ	LastL + 4
	ld		Temp, -Z
	add		AvgA1, Temp
	ld		Temp, -Z
	adc		AvgA2, Temp
	ld		Temp, -Z
	adc		AvgA3, Temp
	ld		Temp, -Z
	adc		AvgA4, Temp
	LCD_CStringZ	CalStr2					; something to watch...
	LCD_CharI	255
	Delay1ms	100
	dec		Arg
	brne	_DLCLoop0
	rcall	ClrmVar1
	Store	mVar1_b0, AvgA1
	Store	mVar1_b1, AvgA2
	Store	mVar1_b2, AvgA3
	Store	mVar1_b3, AvgA4
	StoreI8mVar2	4						; average 4 samples
	rcall	Div64r
	LoadZ	LCal							; save calibration value
	Load	Arg, mVar1_b3
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b2
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b1
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b0
	rcall	WriteEPROM
	ldi		CurMode, MODE_L
	rcall	SetMode
	LCD_Locate	1,0
	ldi		Arg, 16
_DLCLoop1:									; clear lower line of display
	LCD_CharI	32
	dec		Arg
	brne	_DLCLoop1
	LCD_Locate 1,0
	LCD_CStringZ	CalStrL2
	B2DDef	LValLen
	LCD_Locate 1, 5
	LCD_DString OutBuf+12, 8				; show calibration result
	Delay1ms	3000
	rjmp	_DCDone
DoCCal:
	LCD_CharI	'C'
	LCD_CharI	34
	LCD_Locate 1,0
	Delay1ms	200
	ldi		CRange, CS_5UA					; get offset value
	rcall	GetCZero
	LoadZ	CZero5uA						; and save
	Load	Arg, mVar1_b3
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b2
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b1
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b0
	rcall	WriteEPROM
	ldi		CRange, CS_500UA				; get offset value
	rcall	GetCZero						; and save
	LoadZ	CZero500uA
	Load	Arg, mVar1_b3
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b2
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b1
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b0
	rcall	WriteEPROM
	ldi		CRange, CS_50MA					; get offset value
	rcall	GetCZero						; and save
	LoadZ	CZero50mA
	Load	Arg, mVar1_b3
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b2
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b1
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b0
	rcall	WriteEPROM
	ClrB	FLAGS, F_ZEROSW
	ldi		CurMode, MODE_CCAL				; start calibrating each range
	rcall	SetMode
	Delay1ms	200
	ldi		CRange, CS_5UA					; calibrate 5ua
	rcall	GetCZero
	rcall 	ClrmVar2
	LoadZ	CZero5uA
	rcall	ReadEPROM
	Store	mVar2_b3, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b2, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b1, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b0, Arg
	rcall	mSub32							; subtract offset
	LoadZ	CCal5uA
	Load	Arg, mVar1_b3
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b2
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b1
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b0
	rcall	WriteEPROM
	ldi		CRange, CS_500UA				; calibrate 500ua
	rcall	GetCZero
	rcall 	ClrmVar2
	LoadZ	CZero500uA
	rcall	ReadEPROM
	Store	mVar2_b3, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b2, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b1, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b0, Arg
	rcall	mSub32							; subtract offset
	LoadZ	CCal500uA
	Load	Arg, mVar1_b3
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b2
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b1
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b0
	rcall	WriteEPROM
	ldi		CRange, CS_50MA					; calibrate 50ma
	rcall	GetCZero
	rcall 	ClrmVar2
	LoadZ	CZero50mA
	rcall	ReadEPROM
	Store	mVar2_b3, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b2, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b1, Arg
	adiw	Z, 1
	rcall	ReadEPROM
	Store	mVar2_b0, Arg
	rcall	mSub32							; subtract offset
	LoadZ	CCal50mA
	Load	Arg, mVar1_b3
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b2
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b1
	rcall	WriteEPROM
	adiw	Z, 1
	Load	Arg, mVar1_b0
	rcall	WriteEPROM

	ldi		CurMode, MODE_C					; restore mode
	rcall	SetMode
	Delay1ms	200
_DCDone:
	LCD_Cls
	ClrB	FLAGS, F_ZEROSW
	pop		Temp
	pop		Arg
	ret

;*************************************************************************

GetCZero:
	ldi		Temp4, 16						; calculates offset for cap in open circuit
	rcall	ClearAvg
_GCZLoop:	
	rcall	GetCTime
	add		AvgA1, TCnt_B0
	adc		AvgA2, TCnt_B1
	adc		AvgA3, TCnt_B2
	adc		AvgA4, TCnt_B3
	dec		Temp4
	brne	_GCZLoop
	rcall	ClrmVar1
	Store	mVar1_b0, AvgA1
	Store	mVar1_b1, AvgA2
	Store	mVar1_b2, AvgA3
	Store	mVar1_b3, AvgA4
	StoreI8mVar2	16						; average 16 samples
	rcall	div64r
	LCD_CStringZ	CalStr2
	ret

;*************************************************************************

ClearAvg:
	clr		AvgA1
	clr		AvgA2
	clr		AvgA3
	clr		AvgA4
	ret

;*************************************************************************************

_B2DDefSub:									; shorthand call to Bin2Dec function specific to this module
	PushX
	PushY
	push	SFTemp
	ldi		XH, high(mVar1_b0+1)			;Input LSB
	ldi		XL, low(mVar1_b0+1)
	ldi		SFTemp, 4
	ldi		YH, high(OutBufLen)				;OutLenPtr
	ldi		YL, low(OutBufLen)
	rcall	_B2DStart
	pop		SFTemp
	PopY
	PopX
	ret

;*************************************************************************************

_CS_Off:									; turn off all current sources
	cbi		CS_PORT, CS_50MA
	cbi		CS_PORT, CS_5UA
	cbi		CS_PORT, CS_500UA
	sbi		CS_PORT, CS_ZERO
	ret

;*************************************************************************************

_GetFreq:									; routine to capture 1 sec count of T0
	ClrB	FLAGS, F_T1SEC					; initialize everything
	Store	TCCR1B, Zero	
	Store	TCNT1H, Zero
	Store	TCNT1L, Zero
	Store	TCCR0B, Zero
	Store	TCNT0, Zero
	clr		TCnt_B1							; clear initial count
	clr		TCnt_B2
	clr		TCnt_B3
	StoreI	OCR1AH, 0x4C					; load timer 1 compare register for 1 sec delay (20Mhz clk)
	StoreI	OCR1AL, 0x4B
	SetB	TIMSK1, 1
	SetB	TIMSK0, 0
	StoreI	TCCR1B, 0b00000101				; start T1
	StoreI	TCCR0B, 0b00000111				; start T0
GFR1:
	Load	Temp, FLAGS						; load flags
	sbrs	Temp, F_T1SEC					; 1 sec elapsed?
	rjmp	GFR1							; no, wait some more
	StoreI	TCCR1B, 0						; stop counters
	StoreI	TCCR0B, 0
	ClrB	TIMSK1, 1
	ClrB	TIMSK0, 0
	rcall	CLRmVar1
	Store	mVar1_b0, TCnt_B0				; save count to mVar1
	Store	mVar1_b1, TCnt_B1
	Store	mVar1_b2, TCnt_B2
	Store	mVar1_b3, TCnt_B3
	ret

