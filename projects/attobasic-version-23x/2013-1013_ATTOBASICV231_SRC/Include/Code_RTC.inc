#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) || \
	defined(__ATmega32U4__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__)
.if RTC
;.include "Include/Defs_RTC.inc"			;include DEF's
;
;***********************************************************************
;OC0A_int:  ISR for TMR0 Compare Match A (OCR0A).  Increments the 
;	32-bit sequential counter stored in RAM@RTTReg.  The clock source
;	is momentarily switch off while the low byte register is read.
;***********************************************************************
OC0A_int:
	push	temp				;save all registers used		
	PUSHY
	PUSHZ

	LOAD	ZH,SREG				;fetch SREG
	push	ZH					;save SREG

OC0A_int1:	;no need to stop TMR0 as interrupts are disabled during ISR
	tst		RTCcntr				;[RTCcntr] == 0?
	breq	OC0A_int1a			;yes, process RTC count
	dec		RTCcntr				;decrement RTCcntr
	rjmp	OC0A_int1z			;skip processing RTC count

OC0A_int1a:
	LOAD16	ZL,ZH,RTCReg+0		;load 32-bits to registers ZH:ZL,YH:YL
	LOAD16	YL,YH,RTCReg+2

	ADDI32	ZH,ZL,YH,YL,1		;add 1 to count	

	STORE16	RTCReg,ZL,ZH		;store new count (upper bytes)
	STORE16	RTCReg+2,YL,YH		;store new count (lower bytes)

	LOAD	RTCcntr,RTCMul		;reload RTC multiplier counter
OC0A_int1z:
;
.if DHT
OC0A_int2:	;if the DHT routines are enabled then keep track of BUSY flag
	LOAD16	ZH,ZL,RT_TMR		;load 16-bits to registers ZH:ZL
	CPI16	ZH,ZL,0				;timer = 0?
	breq	OC0A_int2a			;timer is zero

	Subi16	ZH,ZL,1				;decrement timer
	STORE16	RT_TMR,ZH,ZL
	SETB	GPIOR1,DHTBusy,temp	;set DHT busy flag
	rjmp	OC0A_int2b			;jump
;
OC0A_int2a:
	CLRB	GPIOR1,DHTBusy,temp	;
;
OC0A_int2b:
.endif

OC0A_int_out:
	SETB	GPIOR0,RTCIRQ		;set the RTCIRQ flag to let others know
								; how we generated the IRQ
	pop		ZH					;restore SREG
	STORE	SREG,ZH

	POPZ						;restore all registers used
	POPY
	pop		temp
	reti						;return from interupt
;
;***********************************************************************
; RTIcommand: RTI - Set real-time counter interval
;***********************************************************************
RTIcommand:
	Gosub	interpretlinev		;Get the value onto data stack
	Gosub	popU				;retrieve # from data stack

	cpi		U,3					;"0" thru "2" only
	brlo	RTIcommand0			;branch if lower
	STOREI	ERRCode,$16			;error code "16"
	Goto	error

RTIcommand0:
	rcall	RTC_TIMINIT			;re-init TMR0 and 32-bit counter

	tst		U					;zero?
	brne	RTIcommand1			;branch if not
	ldi		U,RTCMul_0			;default load value
	rjmp	RTIcommand9			;save the new value

RTIcommand1:
	cpi		U,1					;one?
	brne	RTIcommand2			;branch if not
	ldi		U,RTCMul_1			;10x load value
	rjmp	RTIcommand9			;save the new value

RTIcommand2:
	ldi		U,RTCMul_2			;100x load value

RTIcommand9:
	STORE	RTCMul,U			;save multiplier value in RAM
	mov		RTCcntr,U			;load RTC interrupt counter

	ret							;return to caller
;
;***********************************************************************
;RTRcommand: RTR - Resets the real-time counter to zero
;***********************************************************************
RTRcommand:
	PUSHY						;save Y-ptr
	SETYPTR	RTCReg				;point Y to RTCReg
	clr		temp				;init to zero

	st		Y+,temp				;clear 32-bits (4 bytes)
	st		Y+,temp
	st		Y+,temp
	st		Y,temp
	POPY						;restore Y-ptr

	ret							;return to caller
;
;***********************************************************************
;RTPcommand: RTP - Print real-time counter value or assign to variable
;***********************************************************************
RTPcommand:
	push	outchar				;save all used registers
	PUSHY
	PUSHZ

	SETYPTR	RTCReg				;point Y to RTCReg
	sbrc	XL,VARSTK			;skip next if VARSTK flag clear
	rjmp	RTPcommand2			;just print full 32-bit RTC value

RTPcommand1:
	ldd		U,Y+3				;fetch lowest 8-bits
	GoSub	pushU				;save value on stack
	rjmp	RTPcommand9			;exit

RTPcommand2:
	LOAD16	ZL,ZH,RTCReg+0		;load 32-bits to registers ZH:ZL,YH:YL
	LOAD16	YL,YH,RTCReg+2
	GoSub	BIN4ASC				;convert 32-bit RTC count to ASCII
	GoSub	crlf				;send a CR/LF combo

RTPcommand9:
	POPZ						;restore all pointer registers
	POPY
	pop		outchar
	ret							;return to caller
;
.if INTREG	;only if internal variables
;************************************************************
; RT3const: returns the value of the byte of RTCReg@RAM
;	The lookup table is included for ease of decoding.
;************************************************************
RTCidxtbl:	;index table
	.db		LOW(RTCReg+3), \
			LOW(RTCReg+2), \
			LOW(RTCReg+1), \
			LOW(RTCReg+0)		;MSD to LSD
;
RTCcommand:
	rcall	Get1stParm			;get the last parameter
	ldi		temp,3				;only values of [0..3] allowed
	cp		temp,U
	brmi	RTCcommand9			;branch if allowable exceeded 

	SetZPtr	(2*RTCidxtbl)		;point to index table above
	rcall	PGMReadByte			;datastack = FLASH @Z
	ret
;
RTCcommand9:
	STOREI	ERRCode,$0D			;error code "D"
	Goto	error
;
.endif	;from INTREG
;
;***********************************************************************
;INITIALIZES TIM0 COUNTER AND OCR0A ENABLES INTERUPT
;***********************************************************************
RTC_TIMINIT:
	PUSHY						;save Y-ptr
	SETYPTR	RTCReg				;point Y to RTCReg
	clr		temp				;init to zero

	st		Y+,temp				;clear 32-bits (4 bytes)
	st		Y+,temp
	st		Y+,temp
	st		Y,temp

;clear interupt and zero the counter register
;enable TIMx in PRRx
	CLRB	RTC_PRR,RTC_PRTIM,temp	;enable TIM0 in PRR

	clr		temp				;clear all registers
	STORE	RTC_TIFR,temp
	STORE	RTC_TCNT,temp
	STORE	RTC_TCCRA,temp
	STORE	RTC_TCCRB,temp
	STORE	RTC_OCRA,temp
	com		temp				;set OCR0B to max count
	STORE	RTC_OCRB,temp

	;load compare count register RTC_OCR0A with default
	STOREI	RTC_OCRA,RTC_OCRA_L,temp

	;Set Waveform Generator Mode to OCR0A Interupt
	STOREI	RTC_TCCRA,(0<<COM0A0|0<<COM0B0|2<<WGM00),temp

	;Set clock source and waveform mode
	STOREI	RTC_TCCRB,(0<<RTC_WGM2|RTC_CLK),temp

	;enable Timer OCRA Interupts
	SETB	RTC_TIMSK,RTC_OCIEA,temp

RTC_TIMINIT_out:
	POPY						;restore Y-ptr
	ret							;return from subroutine
;
;***********************************************************************
;DEINITIALIZES TIM0 COUNTER AND OCR0A DISABLES INTERUPT
;***********************************************************************
RTC_TIMDINIT:
	clr		temp				;clear registers
	STORE	RTC_TIFR,temp
	STORE	RTC_TCCRA,temp
	STORE	RTC_TCCRB,temp
	STORE	RTC_TCNT,temp
	STORE	RTC_OCRB,temp
	STORE	RTC_OCRA,temp

	CLRB	RTC_TIMSK,RTC_OCIEA,temp
	SETB	RTC_PRR,RTC_PRTIM,temp		;disable TIMx in PRRx

RTC_TIMDINIT_out:
	ret
;
.endif
#else
#error	"!! Invalid Target MCU !!"
#endif


