; This file holds the code for the ADC and the Analog Comparator
;
.if ADConv
;***************************************************************
ADCcommand:	;Perform 8 bit A to D conversion for channel specified and put result on stack
		;If no channel is specified, convert channel zero
	push	DStkPtr				;save data stack pointer before interpreter is
								; called
	GoSub	interpretlinev		;process line
	clr 	U					;default to channel "0"
	pop		temp				;restore DStkPtr to temp
	sub		temp,DStkPtr		;DStkPtr = 0?
	breq	ADCcommand1			;yes, no parameters on the data stack, continue
	GoSub	popU				;fetch the value on the data statck

ADCcommand1:
	STOREI	ADCSRB,ADCSRB_L,temp;Set ADC control reg B
#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__)
	CLRB	PRR,PRADC,temp		;insure ADC enabled in PRR
	LOAD 	temp,ADMUX			;get ADCMUX
	andi	temp,~(15<<MUX0)	;strip off MUX bits
	andi	U,(15<<MUX0)		;16 channels for Mega88/168				;
#elif defined(__AT90USB1286__)
	CLRB	PRR0,PRADC,temp		;insure ADC enabled in PRR0
	LOAD 	temp,ADMUX			;get ADCMUX
	andi	temp,~(31<<MUX0)	;strip off MUX bits
	andi	U,(31<<MUX0)		;32 channels for usb1286
#elif defined(__ATmega32U4__) || \
	  defined(__ATmega2560__)
	CLRB	PRR0,PRADC,temp		;insure ADC enabled in PRR0
	;64 channel combinations for Mega32U4 and Meag2560
	LOAD 	temp,ADMUX			;get ADCMUX
	andi	temp,~(31<<MUX0)	;strip MUX bits
	CLRB	ADCSRB,MUX5,T		;clear MUX5 bit
	sbrs	U,bit5				;skip next if bit 5 set
	rjmp	PC+5				;skip MACRO
	SETB	ADCSRB,MUX5,T		;set MUX5 bit
	andi	U,(31<<MUX0)		;64/2 channel combinations for Mega32U4
#endif
	or		temp,U				;mask in desired MUX channel
	STORE	ADMUX,temp			;save it

	;set up to average four reads
	clr		U					;zero registers
	clr		T
	ldi		H,ADC_Avg			;load loop counter

ADCcommand2:
	STOREI 	ADCSRA,ADCSRA_L,temp;Set ADC control reg A and start a conv.

	WAIT4ADCRDY					;wait for ADC ready

	LOAD	inbytel,ADCL		;Get the 10 bit result
	LOAD	inbyteh,ADCH
	add		U,inbytel
	adc		T,inbyteh
	dec		H					;decrement loop cpunter
	brne	ADCcommand2			;loop till zero

	;shift result right for 8-bit result
	ldi		H,ADC_Shft			;load loop counter
ADCcommand3:
	lsr		T					;shift result right
	ror		U
	dec		H					;decrement counter
	brne	ADCcommand3

	Gosub	pushU				;Push the data onto the stack

	rcall	ADCden				;diable ADC hardware
	ret
;
;*********************************************************
ADRcommand:
	clr 	U
	Gosub	pushU				;Put default channel on stack
	rcall	Get1stParm			;get the last parameter
	ldi		temp,1				;only values of [0..1] allowed
	cp		temp,U
	brmi	ADRcommand9			;branch if allowable exceeded 
	rcall	ADCen					;initialize ADC
	LOAD	temp,ADMUX			;get current MUX value
	andi	temp,~(3<<REFS0)	;ref to external
	sbrc	U,0					;skip next if bit 0 set
	rjmp	ADRcommand1
	ori		temp,(3<<REFS0)		;ref to internal 1.1V
ADRcommand1:
	STORE	ADMUX,temp			;save it
	ret

ADRcommand9:
	STOREI	ERRCode,$0C			;error code "C"
	Goto	error
;*********************************************************
.endif
;
;*********************************************************
; ADCen:	Enables the ADC hardware by clearing the PRR
;*********************************************************
ADCen:
#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__)
	CLRB	PRR,PRADC,temp		;enable ADC in PRR
#elif defined(__ATmega32U4__) || \
		defined(__AT90USB1286__) || \
		defined(__ATmega2560__)
	CLRB	PRR0,PRADC,temp		;enable ADC in PRR0
#endif

	WAIT4ADCRDY					;wait for ADC ready
	GoSub	Delay10uS			;delay a short time
	ret							;return to caller
;
;*********************************************************
; ADCden:	Dis-enables the ADC hardware by setting the PRR
;*********************************************************
ADCden:
#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__)

	SETB	PRR,PRADC,temp		;disable ADC in PRR

#elif defined(__ATmega32U4__) || \
		defined(__AT90USB1286__) || \
		defined(__ATmega2560__)

	  SETB	PRR0,PRADC,temp		;disable ADC in PRR0

#endif
	GoSub	Delay10uS			;delay a short time
	ret							;return to caller
;
.if ( (DATAREC) && (ADConv) )
;*********************************************************
;	ADGcommand:	select CH0 or CH1 gains
;*********************************************************
ADGcommand:
	rcall	Get1stParm		;get the last parameter
	ldi		temp,$3				;only values of [0..3] allowed
	cp		temp,U				;within limits?
	brpl	ADGcommand1			;branch if within limits
	STOREI	ERRCode,$0D			;"0-3" only, error code "D"
	Goto	error				;exit with error message

ADGcommand1:
	mov		T,U					;move gain to T
	Gosub	popU				;get channel into U
	cpi		U,2					;only values of [0..1] allowed
	brcs	ADGcommand2			;branch if within limits
	STOREI	ERRCode,$0C			;"0-1" only, error code "C"
	Goto	error				;exit with error message

ADGcommand2:	;channel 0
	LOAD	temp,ADCGAIN		;get current port bits into temp
	sbrc	U,0					;skip next if bit 0 clear
	rjmp	ADGcommand3			;process channel 1
	lsl		T					;shift gain bits into bit position 2 and 3
	lsl		T
	andi	temp,~( (1<<ADC0_R0) | \
					(1<<ADC0_R1) )	;strip gain bits 0 and 1
	rjmp	ADGcommand9			;jump to combine gain

ADGcommand3:	;channel 1
	swap	T					;swap gain bits into bit position 4 and 5
	andi	temp,~( (1<<ADC1_R0) | \
					(1<<ADC1_R1) )	;strip gain bits 0 and 1
ADGcommand9:
	or		temp,T				;set new gain bits
	STORE	ADCGAIN,temp		;set gain bits to port
	ret
;
;*********************************************************
;	ADScommand: select CH0 or CH1 to AD536
;*********************************************************
ADScommand:
	clr 	U
	Gosub	pushU				;Put default channel on stack
	rcall	Get1stParm		;get the last parameter
	cpi		U,2					;only values of [0..1] allowed
	brcs	ADScommand1			;branch if allowable
	STOREI	ERRCode,$0C			;error code "C"
	Goto	error

ADScommand1:
	CLRB	AD536IO,AD536CH		;default is channel 0
	sbrc	U,0					;skip next if bit 0 clear
	SETB	AD536IO,AD536CH		;set to channel 1
	ret
;
;*********************************************************
;	ADUcommand:	select TRMS or dBV from AD536
;*********************************************************
ADUcommand:
	clr 	U
	Gosub	pushU				;Put default channel on stack
	rcall	Get1stParm		;get the last parameter
	cpi		U,2					;only values of [0..1] allowed
	brcs	ADUcommand1			;branch if allowable exceeded 
	STOREI	ERRCode,$0C			;error code "C"
	Goto	error

ADUcommand1:
	CLRB	AD536IO,AD536OS		;default is TRMS
	sbrc	U,0					;skip next if bit 0 clear
	SETB	AD536IO,AD536OS		;set to dBV readings
	ret
;
;*********************************************************
.endif
;

;********************************************************************
; ACOcommand:	leaves the value of the analog comparator output 
;	 on the stack, normally "1" for high and "0" for low but is
;	 inverted for use with the data recorder.
;	Also checks the state of the ACOCh1 and ACOCh0 flags to select
;	 the proper source pin for the analog comp. 
;********************************************************************
ACOcommand:		;analog comparator output on stack ("1" for high)
	rcall	ADCen				;enable ADC hardware for register change
	STORE	ADCSRB,temp
	STOREI	ADCSRA,(1<<ADIF),temp
	LOAD	U,GPIOR1			;get flags
	sbrc	U,ACOCh1			;skip next if not AIN-
	rjmp	ACOcommand1			;AIN- pin already selected, continue

	STOREI	ADCSRB,(1<<ACME),temp
	LOAD	temp,ADMUX			;get current ADMUX
	andi	temp,~(15<<MUX0)	;strip off MUX bits
	or		temp,U				;add in AC MUX select bits
	STORE	ADMUX,temp			;save it

ACOcommand1:
	rcall	ACOinit				;insure Analog Comp is initialized

#if DREC	;invert output for data recorder
	ldi		U,1
#else
	clr		U
#endif
	SKBC	ACSR,ACO,romreg
#if DREC	;invert output for data recorder
	clr		U
#else
	ldi		U,1
#endif
	rcall	ADCden				;disable ADC hardware
	GoSub	pushU				;put value on data stack
	GoSub	interpretlinev		;execute any further commands
	ret
;
;********************************************************************
; ACIcommand:	enable or disable the analog comparator's interrupt
;	and set the edge on which to trigger.  Tests for the AC reference
;	being set to > 0, errors if not.
;********************************************************************
ACIcommand:
	rcall	ACOinit				;insure Analog Comp is initialized

	clr		U					;default is to disable interrupts
	GoSub	pushU				;put value on data stack
	rcall	Get1stParm		;get the last parameter
	cpi		U,2					;check for 2 (error if so)
	brcs	ACIcommand1			;not "2+", branch
	STOREI	ERRCode,$16			;value higher than "1", error
	Goto	error
	ret

ACIcommand1:
	LOAD	temp,ACSR			;get current ACSR into temp
	andi	temp,(1<<ACD)		;clear all but ACD
	STORE	ACSR,temp			;set ACSR to disable spurious interrupts

	tst		U					;check for 0
	brne	ACIcommand2			;not "0", branch
	ret							;interrupts already disabled, return

ACIcommand2:
#if DREC	;only for data recorder
	load	T,OCRAL				;fetch the current PWM value
	tst		T					;is it zero?
	brne	ACIcommand3			;greater than 0 so branch
	
	STOREI	ERRCode,$17			;PWM not set, error
	Goto	error
	ret
#endif
ACIcommand3:	;set ACO to trigger on change of state
	ori		temp,(1<<ACI);enable interrupt
	STORE	ACSR,temp			;set ACSR
	ori		temp,((1<<ACI)|(1<<ACIE));enable interrupt
	STORE	ACSR,temp			;set ACSR
	ret
;
;********************************************************************
; ACScommand: select CH0 or CH1 to the Analog Comparator's "-" input
;	This routine only sets or clears the flag as the "ACOcommand"
;	routine is responsible for selecting the channel
;********************************************************************
ACScommand:
	ldi		U,2
	Gosub	pushU				;Put default channel "2" on stack
	rcall	Get1stParm		;get the last parameter
	cpi		U,3					;only values of 0,1 or 2 allowed
	brcs	ACScommand1			;carry set, branch
	STOREI	ERRCode,$16			;error code "16"
	Goto	error
	ret

ACScommand1:
	LOAD	temp,GPIOR1			;get current flags
	andi	temp,~(3<<ACOCh0)	;strip off ACO select bits
	or		temp,U				;OR in ACO select bits
	STORE	GPIOR1,temp			;save GPIOR1
	ret
;
;********************************************************************
ACOinit:	;initialize analog comparator
	LOAD	temp,ACSR			;get current value of ACSR
	andi	temp,(1<<ACIE)		;leave ACIE bit alone
	ori		temp,ACSR_L			;OR in the init bits
	STORE	ACSR,temp			;save ACSR
	ldi		temp,(	(1<<AIN1D) | \
					(1<<AIN0D) );turn off digital circuits
	STORE	DIDR1,temp
	GoSub	Delay10uS			;short delay 
	ret							;return to caller
;
;********************************************************************
; ACI_int:	executed when analog comparator's interrupt triggers.
;	This routine only sets the ACOIRQ flag, which is cleared by the 
;	"SLP" command.
;********************************************************************
ACI_int:
	SETB	GPIOR0,ACOIRQ		;set ACOIRQ flag
	reti							;return to caller
;
