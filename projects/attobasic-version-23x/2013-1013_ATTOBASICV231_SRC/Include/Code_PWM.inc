.if PWM
;***********************************************************************
; PWMcommand:	8-bit PWM.  Sets channel 0 or 1 to the value passed
;***********************************************************************
PWMcommand:
ACRcommand:
	PUSHY							;save Y
	rcall	Get1stParm		;get the last parameter

#if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) )
 .if !SPI	;only if SPI is not enabled as OC1B shares SS pin on 
			;ATmega88/168/328 devices
#endif
	tst		DStkPtr					;DStkPtr = 0?
	brne	PWMcommand1				;no, more data on stack
	mov		T,U						;xfer PWM value to T
	clr		U						;default channel 0 (OC1A)
	rjmp	PWMcommand2				;set the registers

PWMcommand1:	;there are two values on the dstack
	push	U						;save channel # on stack
	GoSub	popU					;get PWM value into U
	mov		T,U						;xfer to T
	pop		U						;restore channel # from stack


	ldi		temp,1					;only values of [0..1] allowed
	cp		temp,U					;check for valid channel #
	brmi	PWMcommand9				;branch if allowable exceeded 
#if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) )
 .endif
#endif

PWMcommand2:	;there are two values on the dstack
	ldi		temp,(	(2 << COM1A0)	| \
					(2 << COM1B0)	| \
					(1 << WGM10) )
	STORE	TCCRA,temp 				; Fast 8-bit PWM
	ldi		temp,(	(1 << WGM12) | \
					(PWMCK << CS10) )
	STORE	TCCRB,temp 				; prescaler = 1
	clr		temp
	STORE	TCCRC,temp

#if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) )
 .if !SPI	;only if SPI is not enabled as OC1B shares SS pin on 
			;ATmega88/168/328 devices
#endif

	sbrs	U,0						;skip next if bit 1 set
	rjmp	PWMcommand2a			;set channel 0 (OC1A)
	rjmp	PWMcommand2b			;set channel 1 (OC1B)

#if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) )
 .endif
#endif
	
PWMcommand2a:	;set channel 0 (OC1A)
	SETB	PWMPORT-1,PWMBITA,temp	;set DDR to output for OCxA 
	STORE16	OCRAL,temp,T
	rjmp	PWMcommand8				;return

PWMcommand2b:	;set channel 1 (OC1B)
	SETB	PWMPORT-1,PWMBITB,temp	;set DDR to output for OCxA 
	STORE16	OCRBL,temp,T

PWMcommand8:
	POPY							;restore Y
	ret								;return to caller

PWMcommand9:
	STOREI	ERRCode,$0C				;error code "C"
	Goto	Error	
;
;***********************************************************************
; PWEcommand:	10-bit PWM.  Sets channel 0 or 1 to the value passed
;***********************************************************************
PWEcommand:
	PUSHY							;save Y
	rcall	Get1stParm		;get the last parameter
	mov		H,U						;xfer PWM value to T
	GoSub	popU					;get value into U

#if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) )
 .if !SPI	;only if SPI is not enabled as OC1B shares SS pin on 
			;ATmega88/168/328 devices
#endif
	tst		DStkPtr					;DStkPtr = 0?
	brne	PWEcommand1				;no, more data on stack
	mov		T,U						;xfer high PWM value to T
	mov		U,H						;xfer low PWM value to T
	clr		H						;default channel 0 (OC1A)
	rjmp	PWEcommand2				;set the registers

PWEcommand1:	;there are two values on the dstack
	push	U						;save channel # on stack
	GoSub	popU					;get PWM value into U
	mov		T,U						;xfer to T
	pop		U						;restore channel # from stack


	ldi		temp,1					;only values of [0..1] allowed
	cp		temp,H					;check for valid channel #
	brmi	PWMcommand9				;branch if allowable exceeded 
									;use PWM's error handler
#if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) )
 .endif
#endif

PWEcommand2:	;there are two values on the dstack
	ldi		temp,(	(2 << COM1A0)	| \
					(2 << COM1B0)	| \
					(3 << WGM10) )
	STORE	TCCRA,temp 				; Fast 10-bit PWM

	ldi		temp,(	(1 << WGM12) | \
					(PWMCK << CS10) )
	STORE	TCCRB,temp 				; prescaler = 1
	clr		temp
	STORE	TCCRC,temp


#if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) )
 .if !SPI	;only if SPI is not enabled as OC1B shares SS pin on 
			;ATmega88/168/328 devices
#endif

	sbrs	H,0						;skip next if bit 1 set
	rjmp	PWEcommand2a			;set channel 0 (OC1A)
	rjmp	PWEcommand2b			;set channel 1 (OC1B)

#if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) )
 .endif
#endif
	
PWEcommand2a:	;set channel 0 (OC1A)
	SETB	PWMPORT-1,PWMBITA,temp	;set DDR to output for OCxA 
	STORE16	OCRAL,T,H
	rjmp	PWEcommand8

PWEcommand2b:	;set channel 0 (OC1A)
	SETB	PWMPORT-1,PWMBITB,temp	;set DDR to output for OCxA 
	STORE16	OCRBL,T,H

PWEcommand8:
	POPY							;restore Y
	ret								;return to caller
;
;***************************************************************
; PWOcommand:	;Turn Pulse Width Modulation OFF.  Does not
;	tamper with port direction register
;***************************************************************
PWOcommand:				;Pulse Width Modulation OFF	
	clr		temp				;zero register
	STORE	TCCR1B,temp 		;disable all registers
	STORE	TCCR1A,temp
	STORE	TCCR1C,temp
	ret
;
.endif
