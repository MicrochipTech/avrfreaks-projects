.if DDS
;*****************************************************************
; DDS: - Outputs a frequency on the defined port pin at the 
;	6-BCD-digit freqency held in the X/Y/Z variables.  Called with 
;	0 on the stack to disable DDS 1 on the stack to enable
;	Without a variable on the stack disables DDS as well as 
;	X/Y/Z = 0.
DDScommand:
	PUSHY						;save Y (program line pointer)
	clr		U					;default to '0'
	Gosub	PushU				;save on datastack
	rcall	Get1stParm			;get the last parameter

	cpi		U,2
	brge	DDScommand9			;branch if allowable exceeded 
	sbrc	U,0					;skip next if bit 0 clear
	rjmp	DDScommand1			;enable DDS and set frequency

	rcall	DDS_TIMDINIT		;disable DDS TIMER 
	SETYPTR	DDSaddrL			;point Y to DDS registers in RAM
	st		Y+,temp				;clear them (temp = 0 from TIM2_DINIT)
	st		Y+,temp
	st		Y,temp

	CLRB	DDSPort,DDSOut		;clear output bit
	CLRB	DDSPort-1,DDSOut	;change DDR to input

	POPY						;restore Y
	ret	

DDScommand1:					;enable DDS
	SetYPtr	(VARSTOR+23)			;point Y to variable 'X'
	rcall	FRQ2DDSADD			;convert BCD (H,T,L) to DDS accumulator 
								; adder values

	clr		temp				;zero DDSaccmH
	STORE	DDSacch,temp		;store to high byte of DDS accumulator
	
	SETB	DDSPort-1,DDSOut	;change DDR to output
	CLRB	DDSPort,DDSOut		;clear output bit

	rcall	DDS_TIMINIT			;enable DDS TIMER

	POPY						;restore Y
	ret

DDScommand9:
	STOREI	ERRCode,$0C			;error code "C"
	Goto	error
;
;***********************************************************************
;TimerX  OCRxA Service
;	DDS Accululator Update
;***********************************************************************
;output frequency (using 24 bit accumulator) :
;
;	f = deltaPhase * fClock/2^24
;
;	fClock is in this case the CPU clock divided by the
;	number of cycles to output the data ( 5uS or 50 cycles @ 10MHz)
;
;	f = addrl/addrm/addrh * (10000000/50)/16777216
;	f = addrl/addrm/addrh * 0.0119 Hz resolution

; Original DDS code written by Jesper Hansen jesperh@telia.com
;	http://www.myplace.nu/avr/minidds/index.htm
;***********************************************************************
;	accl,accm,acch is the phase accumulator
; 	addrl,addrm,addrh is the adder value determining frequency
;
;	add value to accumulator
;	load byte from current table in RAM
;	output high bit to port
;
; Timer/Counter2 Compare Match A
;
DDS_int:
.if DDS		;execute only if DDS enabled
	push	H					;2 - save H
	LOAD	H,SREG				;2 - save SREG
	push	H					;2
	push	T					;2 - save T
	push	U					;2 - save U
	PUSHZ						;4 - save Z

.ifdef RAMPZ					;need RAMPZ to access >64KW FLASH
	ldi		H,(BYTE3(2*sqrwave));1 - set 64K page number
	STORE	RAMPZ,H				;2
.endif
	ldi		ZH,high(2*sqrwave)	;2 - point Z to last wavetable index
	LOAD	ZL,DDSacch			;1 - get accmH from RAM

	LOAD	U,DDSaddrL			;1 - get table pointer increment low byte
	LOAD	T,DDSaddrM			;1 - get table pointer increment mid byte
	LOAD	H,DDSaddrH			;1 - get table pointer increment high byte

	ADD24	acch,accm,accl,H,T,U;3 - add it all up

.ifdef RAMPZ					;need RAMPZ to access >64KW FLASH
	elpm	U,Z					;3 - get value from table
.else
	lpm		U,Z					;3 - get value from table
.endif
	STORE	DDSacch,ZL			;1 - save new table pointer to RAM
	sbrs	U,0					;2/3 - skip next if bit 0 = 1 
	rjmp	DDS_int1			;2 - bit 0 = 1
	nop							;1 - for symetry
	SETB	DDSPort,DDSout		;1 Set DDS Bit out
	rjmp	DDS_int_out			;2 - exit

DDS_int1:
	CLRB	DDSPort,DDSout		;1 Clear DDS Bit out
	nop							;1 - for symetry
	nop							;1 - for symetry
DDS_int_out:
	POPZ						;4 - restore Z
	pop		U					;2 - restore U
	pop		T					;2 - restore T
	pop		H					;restore SREG
	STORE	SREG,H				;2 - save SREG
	pop		H					;2 - restore H
	reti						;4
.endif
;
;***************************************************************************
;* FRQ2DDSADD - sets up the DDS frequency adder registers from 
;	the 6-digit packed-BCD "current" frequency stored in H,T,U when 
;	Gosubed and saves the accumulator addresses in RAM.
;***************************************************************************
FRQ2DDSADD:							;set up registers for new frequency
	SetYPtr	(VARSTOR+23)			;point Y to variable 'X'

	clr		H						;zero the temporary address registers
	clr		T
	clr		U

	ldi		temp,0xf0				;set up "striper"
	mov		r1,temp					;put it in r1

FRQ2DDSADD100KHZ:
	ld		r0,Y					;load variable 'X' to r0 (BCDH digits)
;	lds		r0,DDSbcdH				;get upper digits, 100K and 10K
	and		r0,r1					;strip low nible
	swap	r0						;and swap nible, 100K
FRQ2DDSADD100KHZa:
	tst		r0						;check for zero
	breq	FRQ2DDSADD10KHZ			;is zero, so skip
	ldi		inbytel,LOW(_100KHz)		;add up the count values
	adc		U,inbytel
	ldi		inbyteh,BYTE2(_100KHz)
	adc		T,inbyteh
	ldi		temp,BYTE3(_100KHz)
	adc		H,temp
	dec		r0						;dec register
	rjmp	FRQ2DDSADD100KHZa		;and loop till zero

FRQ2DDSADD10KHZ:
	ld		r0,Y+					;re-load variable 'X' to r0 (BCDH digits)
;	lds		r0,DDSbcdH				;get upper digits, 100K and 10K
	swap	r1
	and		r0,r1					;strip high nible, 10K
FRQ2DDSADD10KHZa:
	tst		r0						;check for zero
	breq	FRQ2DDSADD1KHZ			;is zero, so skip
	ldi		inbytel,LOW(_10KHz)		;add up the count values
	adc		U,inbytel
	ldi		inbyteh,BYTE2(_10KHz)
	adc		T,inbyteh
	ldi		temp,BYTE3(_10KHz)
	adc		H,temp
	dec		r0						;dec register
	rjmp	FRQ2DDSADD10KHZa		;and loop till zero

FRQ2DDSADD1KHZ:
	ld		r0,Y					;load variable 'Y' to r0 (BCDM digits)
;	lds		r0,DDSbcdM				;get middle digits, 1K and 100
	swap	r1
	and		r0,r1					;strip low nible
	swap	r0						;and swap nible, 1K
FRQ2DDSADD1KHZa:
	tst		r0						;check for zero
	breq	FRQ2DDSADD100HZ			;is zero, so skip
	ldi		inbytel,LOW(_1KHz)		;add up the count values
	adc		U,inbytel
	ldi		inbyteh,BYTE2(_1KHz)
	adc		T,inbyteh
	ldi		temp,BYTE3(_1KHz)
	adc		H,temp
	dec		r0						;dec register
	rjmp	FRQ2DDSADD1KHZa			;and loop till zero

FRQ2DDSADD100HZ:
	ld		r0,Y+					;re-load variable 'Y' to r0 (BCDM digits)
;	lds		r0,DDSbcdM				;get middle digits, 1K and 100
	swap	r1
	and		r0,r1					;strip high nible, 100
FRQ2DDSADD100HZa:
	tst		r0						;check for zero
	breq	FRQ2DDSADD10HZ			;is zero, so skip
	ldi		inbytel,LOW(_100Hz)		;add up the count values
	adc		U,inbytel
	ldi		inbyteh,BYTE2(_100Hz)
	adc		T,inbyteh
	ldi		temp,BYTE3(_100Hz)
	adc		H,temp
	dec		r0						;dec register
	rjmp	FRQ2DDSADD100HZa		;and loop till zero

FRQ2DDSADD10HZ:
	ld		r0,Y					;load variable 'Z' to r0 (BCDL digits)
;	lds		r0,DDSbcdL				;get low digits, 10 and 1
	swap	r1
	and		r0,r1					;strip low nible
	swap	r0						;and swap nible, 10
FRQ2DDSADD10HZa:
	tst		r0						;check for zero
	breq	FRQ2DDSADD1HZ			;is zero, so skip
	ldi		inbytel,LOW(_10Hz)		;add up the count values
	adc		U,inbytel
	ldi		inbyteh,BYTE2(_10Hz)
	adc		T,inbyteh
	ldi		temp,BYTE3(_10Hz)
	adc		H,temp
	dec		r0						;dec register
	rjmp	FRQ2DDSADD10HZa			;and loop till zero

FRQ2DDSADD1HZ:
	ld		r0,Y					;re-load variable 'Z' to r0 (BCDL digits)
;	lds		r0,DDSbcdL				;get low digits, 10 and 1
	swap 	r1
	and		r0,r1					;strip high nible, 1
FRQ2DDSADD1HZa:
	tst		r0						;check for zero
	breq	FRQ2DDSADD_out			;is zero, so skip
	ldi		inbytel,LOW(_1Hz)			;add up the count values
	adc		U,inbytel
	ldi		inbyteh,BYTE2(_1Hz)
	adc		T,inbyteh
	ldi		temp,BYTE3(_1Hz)
	adc		H,temp
	dec		r0						;dec register
	brne	FRQ2DDSADD1HZa			;and loop till zero

FRQ2DDSADD_out:
	STORE	DDSaddrL,U				;save addrl to RAM
	STORE	DDSaddrM,T				;save addrm to RAM
	STORE	DDSaddrH,H				;save addrh to RAM
	ret
;
;***********************************************************************
;INITIALIZES TIMx COUNTER AND OCRxA ENABLES INTERUPT
;***********************************************************************
;set up Timer
DDS_TIMINIT:
;clear interupt and zero the counter register
;enable TIMx in PRRx
	CLRB	DDS_PRR,DDS_PRTIM,temp	;enable TIMx in PRR

	clr		temp
	STORE	DDS_TIFR,temp
	STORE	DDS_TCNT,temp
	STORE	DDS_TCCRA,temp
	STORE	DDS_TCCRB,temp
	STORE	DDS_OCRA,temp
	STORE	DDS_OCRB,temp

#if defined(__ATmega32U4__)
	STORE	DDS_TCCRC,temp
	STORE	DDS_TCCRD,temp
	STORE	DDS_TCCRE,temp
;	STORE	DDS_OCRC,temp
#endif

#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__)
;
;load compare count register DDS_OCRxA with DDSCnt-1
	ldi		temp,DDSCnt-1
	STORE	DDS_OCRA,temp
;Set Waveform Generator Mode to OCRA Interupt
	ldi		temp,(0<<COM2A0|0<<COM2B0|2<<WGM20)
#elif defined(__ATmega32U4__)
;load compare count register DDS_OCRxA with DDSCnt-1
	ldi		temp,DDSCnt-1
	STORE	DDS_OCRC,temp

;Set Waveform Generator Mode to OCRA Interupt
	ldi		temp,(0<<DDS_COMA0|0<<DDS_COMB0|0<<DDS_PWMA)
#else
#error	"!! Invalid Target MCU !!"
#endif
	STORE	DDS_TCCRA,temp

;Set clock source and waveform mode
#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__)
;
	ldi		temp,(0<<DDS_WGM2|DDSPre)
	STORE	DDS_TCCRB,temp

;enable Timer OCRA Interupts
	SETB	DDS_TIMSK,DDS_OCIEA,temp
#elif defined(__ATmega32U4__)
	ldi		temp,DDSPre
	STORE	DDS_TCCRB,temp

;enable Timer OCRC Interupts
	SETB	DDS_TIMSK,DDS_OCIEA,temp
#else
#error	"!! Invalid Target MCU !!"
#endif

DDS_TIMINIT_out:
	ret							;return from subroutine
;
;***********************************************************************
;DEINITIALIZES TIM2 COUNTER AND OCR2A DISABLES INTERUPT
;***********************************************************************
DDS_TIMDINIT:
	clr		temp				;clear registers
	STORE	DDS_TIFR,temp
	STORE	DDS_TCCRA,temp
	STORE	DDS_TCCRB,temp
	STORE	DDS_TCNT,temp
	STORE	DDS_OCRB,temp
	STORE	DDS_OCRA,temp
#if defined(__ATmega32U4__)
	STORE	DDS_TCCRC,temp
	STORE	DDS_TCCRD,temp
	STORE	DDS_TCCRE,temp
#endif

	CLRB	DDS_TIMSK,DDS_OCIEA,temp
	SETB	DDS_PRR,DDS_PRTIM,temp		;disable TIMx in PRRx

DDS_TIMDINIT_out:
	ret
;
;***********************************************************************
.endif
