.if LPMD
;***********************************************	
;SLP command - Enter Sleep Mode
;*****************************************************
SLPcommand:
	clr		U					;default to "0"
	Gosub	pushU				;save it on the stack
	rcall	Get1stParm			;get the last parameter
	tst		U					;test for zero
	breq	SLPcommand2			;branch if zero (hardware IRQ only)

	;user has passed a number for use with the watchdog timeout
	cpi		U,WDMAX+1			;greater than max prescaler +1?
	brcs	SLPcommand1			;branch if allowable 
	STOREI	ERRCode,$16			;error code "16"
	rcall	error

SLPcommand1:
	wdr							;reset watchdog
	CLRB	MCUSR,WDRF,temp		;clear WDRF flag
	mov		temp,U				;copy of U in temp register
	andi	U,0x07				;mask bits 2:0

	lsl		temp				;shift bit 3 left by 2 bits
	lsl		temp
	andi	temp,(1<<WDP3)		;strip all other bits
	ori		temp,(1<<WDIE)		;set "interupt mode"
	or		U,temp				;timout now in U

	;set the WD to "interrupt mode" (0<<WDE | 1<<WDIE)
	cli							;disable interrupts to set WDT
	STOREI	WDTCSR,(1<<WDCE|1<<WDE),temp
	STORE	WDTCSR,U			;set Watchdog timer to timeout

	LOAD	temp,GPIOR0			;get IRQ flags
	andi	temp,~( 1<<RTCIRQ | \
					1<<DDSIRQ | \
					1<<WDTIRQ | \
					1<<USBIRQ | \
					1<<ACOIRQ )	;clear and  
	STORE	GPIOR0,temp			;store them 

	rjmp	SLPcommand3			;jump and execute sleep
;
SLPcommand2:
	rcall	ADCen				;enable ADC hardware for register change
	CLRB	ADCSRB,ACME,T		;insure ACME bit cleared to start
	SKBC	GPIOR1,ACOCh1,U		;skip next if user has ADCn selected
	rjmp	SLPcommand2a		;using analog comp input, continue

	STOREI	ADCSRA,(1<<ADIF),T	;clear ADCSRA to disable ADC
	STOREI	ADCSRB,(1<<ACME),T	;set ACME bit for ACO MUX select
	LOAD	temp,ADMUX			;get current ADMUX
	andi	temp,~(15<<MUX0)	;strip off MUX bits
	or		temp,U				;add in AC MUX select bits
	STORE	ADMUX,temp			;save it

SLPcommand2a:
	SETB	ACSR,ACI,T			;insure no pending AC interrupts
	rcall	ADCden				;disable ADC hardware and short delay

SLPcommand3:	;enter "sleep", all hardware interrrupts should be enabled
	sei							;insure global interrupts are enabled
	sleep						;sleep
	nop							;for benefit of the aleep instruction

.if (RTC || DDS)	;only if DDS or RTC enabled
	LOAD	U,GPIOR0			;get IRQ flags into U
	andi	U,(1<<RTCIRQ|1<<DDSIRQ)	;mask out IRQ flags
	breq	SLPcommand3a			;not DDS or RTC IRQ, continue
  .if RTC			;only if RTC enabled
	SKBC	GPIOR0,RTCIRQ		;skip next if RTC did not interrupt
	CLRB	GPIOR0,RTCIRQ		;clear the RTCIRQ flag
  .endif
.if DDS			;only if DDS enabled
	SKBC	GPIOR0,DDSIRQ		;skip next if DDS did not interrupt
	CLRB	GPIOR0,DDSIRQ		;clear the DDSIRQ flag
  .endif
.endif

SLPcommand3a:
	SKBC	UCSRA,RXC,temp		;skip next RX_RDY did not interrupt
	ret							;was RX_RDY, so exit

SLPcommand3b:	;waiting for watchdog to timeout
	SKBS	GPIOR0,WDTIRQ		;skip next if WDTIRQ
	rjmp	SLPcommand3c		;check for ACO trigger
	CLRB	GPIOR0,WDTIRQ		;clear WDTIRQ flag
	ret							;return to caller

SLPcommand3c:	;waiting for ACO to trigger
	SKBS	GPIOR0,ACOIRQ		;skip next if ACOIRQ
	rjmp	SLPcommand3			;loop back to sleep
	CLRB	GPIOR0,ACOIRQ		;clear WDTIRQ flag
	ret							;return to caller
;	
.endif
