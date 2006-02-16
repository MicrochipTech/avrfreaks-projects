;***** Convertion FF to 255 ************************************************
;
; Project		;Brick For Programs
; Filename		:FF_to_255.asm
; Target		:TH03_V1.asm (Thermometer with I2C bus)
; Device		:All
; Created		:2003.04.02
; Last Updated	:
; Version		:1.0
;
; DESCRIPTION	Transformation of hexadecimal representation of number in decimal.
;				The decimal range is limited up to 255
;***************************************************************************
	
;***** **********************************	
.def	dp	=r20
.def	bin_data	=r18;(i2cdata=r18)
	.equ minus	=7	;if bit is set: value from -25 to -0) 
.def	unit	=r1	;unit counter
.def	ten		=r2	;tens counter
.def	hund	=r3	;hundres counter
.def	calc	=r23;It is possible this superfluous, =temp

CONVERT_FF_255:

	sbrs flags,lf
	rjmp confirmed
load_err:
	ldi temp,28		; load symbol "-"
	sts S_SYM0, temp
	sts S_SYM1, temp
	sts S_SYM2, temp
	sts S_SYM3, temp
	ret

confirmed:
	clr unit	;clear unit counter
	clr ten		;clear tens counter
	clr hund	;clear hundres counter
	ldi temp,29	;load "blank"
	sts S_SYM0, temp
	sts S_SYM1, temp
	sts S_SYM2, temp
	sts S_SYM3, temp

mov temp, bin_data
sbrc temp, minus
rcall PREPARE_MINUS

;*** The maximal value on an input	FF ***
		
DECI_10:
	mov calc,bin_data	;load counter low byte
	andi calc,0xf0	;mask low nibbles
	swap calc		;swap nibbles
CHECK_DECI_10:
	cpi calc,0		;is it =0?
	breq DECI_1		;yes, go decode unit
	dec calc		;dec tens
	ldi dp,1
	add ten,dp	;add 1 to counter tens
	ldi dp,6
	add unit,dp	;add 6 to counter unit
	rjmp CHECK_DECI_10	;check tens again
		
DECI_1:
	mov calc,bin_data	;load counter low byte
	andi calc,0x0f	;mask high byte
CHECK_DECI_1:
	cpi calc,0		;is it =0?
	breq FIN		;yes, go to decode symbol 1
	dec calc
	inc unit		;inc counter unit
	rjmp CHECK_DECI_1	;check unit again
		
FIN:	
;*****Correction (10/10/10/10)*********************************		

lds1:	mov calc,unit	;load counter unit
sym1:	cpi calc,10		;is it >=10?
		brlo lds10		;no, go to decode symbol 10
		subi calc,10	;substract 10
		inc ten			;inc counter tens
		rjmp sym1		;check up again
lds10:
		mov unit,calc	;load in the counter of unit the rest
		mov calc,ten	;load counter tens
sym10:	cpi calc,10		;is it >=10?
		brlo lds100		;no, go to decode symbol 100
		subi calc,10	;substract 10
		inc hund		;inc counter hundres
		rjmp sym10		;check up again
lds100:
		mov ten,calc	;load in the counter of tens the rest
		mov calc,hund	;load counter hundres
		mov hund,calc	;load in the counter of hundres the rest

;----- To keep result in the SRAM and to hide non-significant zero -----
		tst hund
		breq NOT_STORE_HUND
		sts S_SYM2, hund
		rjmp STORE_TEN
NOT_STORE_HUND:
		tst ten
		breq NOT_STORE_TEN
STORE_TEN:
		sts S_SYM1, ten
NOT_STORE_TEN:
		sts S_SYM0, unit

		ret
;******************************************************************

PREPARE_MINUS:

	com bin_data
	ldi temp,28
	sts S_SYM3, temp
	ret
	