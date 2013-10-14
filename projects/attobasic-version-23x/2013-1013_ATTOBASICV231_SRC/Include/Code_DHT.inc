.if (DHT && RTC)
;********************************************************************
; DHTcommand: Read DHTxx temperature in Celsius.  The value returned
;	is integer and also stored in RAM@RT_I.  If parameter of "1" is 
;	passed then the data is converted to Fahrenheit..
;********************************************************************
DHTcommand:
	PushY						;save Y pointer
;
DHTcommand1:
;#if DHT_TEST	; for testing
;	STOREI16	RH_I,111,temp	;humidity, 10x
;	STOREI16	RT_I,0x9200,temp;temperature, Celsius, 10x
;#else

	GoSub	DHTRead				;fetch DHT data
;#endif
	LOAD	H,RH_I				;get integer RH into H
	LOAD	T,RT_I				;get integer temp into T
	LOAD	U,RT_D				;get decimal temp into U

	tst		H					;if T = 0 then checksum error
	brne	DHTcommand1b		;T != 0, branch for good reading
	clr		T					;zero T register
	rjmp	DHTcommand1c		;finshed, so exit

DHTcommand1b:	;good reading detected
	SKBS	GPIOR1,DHTUnit,temp	;skip next if deg in Celsius
	Goto	DHTcommand2			;desire deg in Fahrenheit

	cpi		U,6					;compensate for rounding
	brlo	DHTcommand1c		;lower, leave integer alone		;
	inc		T					;round up by adding 1 to integer

DHTcommand1c:	;check for Fahrenheit or Celsius
	SKBS	GPIOR1,DHTSign,temp	;skip next if recognize sign
	andi	T,0x7f				;strip sign bit

	mov		U,T					;get integer into U
	GoSub	pushU				;put value on the stack
	PopY						;restore Y pointer
	ret							;return to caller
;
;   F = (C* 9 +2)/5+32;  // The +2 takes care of the better rounding 
DHTcommand2:	;calculate degrees in Fahrenheit
	bst		T,Bit7				;save sign bit in T
	andi	T,0x7f				;strip sign bit
	ldi		temp,10				;need 10x multiplier
	mul		T,temp				;multiply, result in r1:r0	
	clr		temp				;zero upper byte
	ADD16	r1,r0,temp,U		;add in the decimal portion

	;multiplicand = YH:YL, multiplier = H:T
	Copy16	YH,YL,r1,r0			;move result into YH:YL
	clr		H					;clear upper multiplier
	ldi		T,9					;load lower multiplier

	;result 3:0 = inbyteh:inbytel:H:T
	GoSub	MPY16U				;calculate result
	Addi16	H,T,2				;add fudge factor for rounding

	; H:T / inchar:outchar = H:T, rem inbyte[h:l]
	clr		inchar				;zero upper byte
	ldi		outchar,50			;load divisor of 5 (10x for decimal shift)
	GoSub	DIV16U				;calculate result
	brts	DHTcommand2a		;reading was negative, branch
	Addi16	H,T,32				;add 32 for Fahrenheit
	Goto	DHTcommand2b		;save value

DHTcommand2a:	;calculate degrees in Fahrenheit. Subtract from
				; 32 for a negative reading 
	subi	T,low(32)
	sbci	H,high(32)
	neg		T

DHTcommand2b:
	mov		U,T					;save decimal result to U
	GoSub	pushU				;put value on the stack
	PopY						;restore Y pointer
	ret

DHTcommand9:
	STOREI	ERRCode,$0C			;error code "C"
	Goto	Error	
;
;********************************************************************
; DHHcommand: Read DHTxx humidity.
;********************************************************************
DHHcommand:
	PushY						;save Y pointer
	GoSub	DHTRead				;fetch DHT data
	LOAD	U,RH_I				;get integer humidity in U
	GoSub	pushU				;save on stack
	PopY						;restore Y pointer
	ret
;
;********************************************************************
; DHRcommand: Return sensor status.  "1" means busy, "0" means ready
;********************************************************************
DHRcommand:
	clr		U					;preset U to "0"
	SKBC	GPIOR1,DHTBusy,temp	;okay to take a new reading
	ldi		U,1

DHRcommand1:
	GoSub	pushU				;put value on the stack
	ret
;
;********************************************************************
; DHUcommand: Set or clear DHT units flag.  0 = Fahrenheit
;	1 = Celsius.  Default is "0", Fahrenheit.  Without [x] returns
;	the state of the DHTUnit bit.
;********************************************************************
DHUcommand:
	clr		U					;default "0"
	LOAD	T,GPIOR1			;get the current value of OSCCAL
	sbrc	T,DHTUnit			;skip next if DHTUnit bit clear
	ldi		U,1					;set to "1"
	GoSub	pushU				;save on stack
	rcall	Get1stParm			;get the last parameter
	GoSub	pushU				;save on stack
	ldi		temp,1				;only values of [0..1] allowed
	cp		temp,U
	brmi	DHUcommand9			;branch if allowable exceeded 
	sbrs	U,0					;skip next if bit 0 set
	Goto	DHUcommand1
	SETB	GPIOR1,DHTUnit,temp	;user desires deg in Celsius
	ret
;
DHUcommand1:
	CLRB	GPIOR1,DHTUnit,temp	;user desires deg in Fahrenheit
	ret

DHUcommand9:
	STOREI	ERRCode,$0C			;error code "C"
	Goto	Error	
;
;********************************************************************
; DHScommand: Set or clear DHT "ignore sign" flag.  0 = ignore sign
;	1 = allow sign.  Default is "0", ignore sign.  Without [x] 
;	returns the state of the DHTUnit bit.
;********************************************************************
DHScommand:
	clr		U					;default "0"
	LOAD	T,GPIOR1			;get the current value of GPIOR1
	sbrc	T,DHTSign			;skip next if DHTSign bit clear
	ldi		U,1					;set to "1"
	GoSub	pushU				;save on stack
	rcall	Get1stParm			;get the last parameter
	GoSub	pushU				;save on stack
	ldi		temp,1				;only values of [0..1] allowed
	cp		temp,U
	brmi	DHScommand9			;branch if allowable exceeded 
	sbrs	U,0					;skip next if bit 0 set
	Goto	DHScommand1
	SETB	GPIOR1,DHTSign,temp	;user desires deg in Celsius
	ret
;
DHScommand1:
	CLRB	GPIOR1,DHTSign,temp	;user desires deg in Fahrenheit
	ret

DHScommand9:
	STOREI	ERRCode,$0C			;error code "C"
	Goto	Error	
;
;***************************************************************
.if INTREG	;enabled only if internal variables
DHIcommand:
	ldi		U,LOW(RH_I)
	rjmp	DTcommand9
DHDcommand:
	ldi		U,LOW(RH_D)
	rjmp	DTcommand9
DTIcommand:
	ldi		U,LOW(RT_I)
	rjmp	DTcommand9
DTDcommand:
	ldi		U,LOW(RT_D)
DTcommand9:
	GoSub	pushU				;save on the stack
	GoSub	interpretlinev
	ret
.endif
;
;********************************************************************
; DHTRead:  Initiates and reads the data from the DHT sensor.  The 
;	watchdog timer (16mS timeout) is used to detect a non-response 
;	condition, which invokes an error.  If there is a data and 
;	checksum mismatch, then an error is invoked.
;	- The data is stored in RAM at RT_I (integer), RT_D (decimal), 
;	  RH_I (integer), RH_D (decimal) in its native format. 
;	- Since the RTC feature is enabled then the TIMR0 counter value is
;	  saved and later restored so TIMR0's TCNT0 register can be used 
;	  to time the duration of the DHT's data pulse widths.  The TIMR0 
;	  clock divider is untouched.
;	USES:	temp, H, T, U, inbytel, inbyteh, Y-Ptr
;********************************************************************
DHTRead:
	SKBC	GPIOR1,DHTBusy,temp	;okay to take a new reading
	ret							;no, exit

	LOAD 	H,RT_NRC			;get current no-resp. count
	tst		H					;is it zero
	brne	DHTReada			;no, continue
	STOREI	RT_NRC,HTNoRsp		;yes, reload no-response counter

DHTReada:
.if PWM
;!!!!!!!
; be sure to save timer registers in case PWM or ICP is being used
;!!!!!!!
	clt							;clear T-flag
	LOAD	temp,HT_TCCRB		;retrieve current clock source
	andi	temp,0x07			;strip all but clock source bits
	breq	DHTReadb			;branch if zero (TIMER not being used)
	LOAD	temp,HT_TCCRB		;retrieve TIMER registers
	push	temp				;and save on stack
	LOAD	temp,HT_TIMSK
	push	temp
	LOAD	temp,HT_TCCRC
	push	temp
	LOAD	temp,HT_TCCRA
	push	temp
	set							;set T-Flag as indicator to restore registers

DHTReadb:
	STOREI	HT_TCCRB,HTCK_0		;disable clock source
.else
	STOREI	HT_TCCRB,HTCK_0		;disable clock source
.endif


;Start of DHTxx device read
DHTRead1:
	rcall	DHTSetup			;setup timers and interrupts

;pulse the DHTPIN for N x 1mS to initiate data transfer
	ldi		temp,HTPulse		;load delay counter
	SETB	DHPORT-1,DHTPIN		;set DATA pin to output
	CLRB	DHPORT,DHTPIN		;set DATA pin low to initate xfer

DHTRead1a:
	Gosub	Delay1mS			;delay 1mS for pulse
	dec		temp				;decrement counter
	brne	DHTRead1a			;loop till zero

DHTRead1b:
;	rcall	DHTSetup			;setup timers and interrupts

	SetYPtr	RH_I				;set pointer to RAM
	clr		r0					;clear register
	STORE16	HT_TCNTL,r0,r0		;clear counter
	ldi		inbytel,HTBits+2	;load bit counter (2 start bits + 8 data bits)
	ldi		inbyteh,HTBytes		;load data byte counter
	clr		U					;clear temporary holding register
	clr		T					;clear summing register

	SETB	DHPORT,DHTPIN		;release DATA pin
	CLRB	DHPORT-1,DHTPIN		;set DATA pin to input

	STOREI	HT_TCCRB,HT_CLK		;enable clock source

DHTRead2:	;start of pin change checking
	sleep						;wait for IRQ
	nop							;benefit of the sleep instruction
	SKBS	GPIOR0,WDTIRQ		;skip next if WDTIRQ flag set (timeout)
	rjmp	DHTRead2a			;no watchdog timeout, continue

;no response from DHT sensor !!
	rcall	DHTCleanup			;cleanup timers, etc
	clr		r0					;zero humidity as flag
	STORE16	RH_I,r0,r0			;store RH = 0 

	LOAD 	temp,RT_NRC			;get current no-resp. count
	dec		temp				;is it zero
	STORE	RT_NRC,temp			;store new count value
	SKBS	SREG,SREG_Z,temp	;skip next if counter = 0 
	ret							;return to caller

	SetZPtr	(2*emessage18)		;Z = message start
	GoSub	sendlromline		;inform user of error
	ret							;return to caller

DHTRead2a:	;at this point we awoke from sleep mode
	SKBS	DHPORT-2,DHTPIN		;skip next if DATA pin high
	rjmp	DHTRead2b			;DATA pin went low, check high time
	STORE16	HT_TCNTL,r0,r0		;clear counter
	rjmp	DHTRead2

DHTRead2b:
	LOAD	H,HT_TCNTL			;get current TCNT count (low byte only)
	cpi		H,HT_OCRA_L			;compare to high-bit duration
	brsh	DHTRead2d			;Must be a "1"
	rjmp	DHTRead2c			;detected a "0" bit, jump
;	cpi		temp,HTZero			;compare to lo-bit duration
;	brcc	DHTRead2c			;Must be a "0"

DHTRead2c:		;detected a "0" bit
	clc							;clear carry bit
	rjmp	DHTRead2e

DHTRead2d:		;detected a "1" bit
	sec							;set carry flag

DHTRead2e:		;rotate carry flag into bit position 0
	dec		inchar				;decrement total bit counter
	rol		U					;rotate bit into temp register
	dec		inbytel				;decrement bit counter
	brne	DHTRead2
	dec		inbyteh				;decrement byte counter
	breq	DHTRead3			;finished fetching data, branch
	st		Y+,U				;save digit to RAM
	add		T,U					;add to summing register
	ldi		inbytel,HTBits		;re-load bit counter for 8 data bits
	rjmp	DHTRead2			;loop for more data

;At this point, all data has been captured from the device
DHTRead3:
	rcall	DHTCleanup			;cleanup timers, etc

;data captured, check for a checksum error
	cp		U,T					;good checksum?
	breq	DHTRead4			;no checksum error
	;checksum error detected, zero humidity as flag
	clr		r0					;zero register
	STORE16	RH_I,r0,r0			;store RH = 0 

	SetZPtr	(2*emessage19)		;Z = message start
	GoSub	sendlromline		;inform user of error
	ret							;exit

DHTRead4:
;Adjust RAM contents to show humidity as integer and decimal 
	SetYPtr	RH_D				;set pointer to RAM@RH_D
	LOAD16	inbytel,inbyteh,RH_I;set register to humidity value
	GoSub	DHTConvert			;convert to integer and decimal

;Adjust RAM contents to show temperature as integer and decimal
	SetYPtr	RT_D				;set pointer to RAM@RT_D
	LOAD16	inbytel,inbyteh,RT_I;set register to humidity value
	GoSub	DHTConvert			;convert to integer and decimal
	ret							;return to caller
;
DHTRead9:	;
.if USBSUPP
	GoSub	E_USBINT			;re-enable USB ints
.endif

.if RTC
	;re-enable Timer OCRA Interupts
	SETB	RTC_TIMSK,RTC_OCIEA,temp
.endif
	CLRB	GPIOR0,WDTIRQ		;clear WDTIRQ flag
	Goto	Error	
;
;*********************************************************************
;	DHTConvert:	Converts the 16-bit binary value in [inbyteh:inbytel]
;		to a separate integer and decimal.  Strips the sign bit before
;		conversion then adds it in after conversion.
;*********************************************************************
DHTConvert:
	bst		inbyteh,7			;save sign bit to T flag for later
	andi	inbyteh,0x7f		;strip off sign bit

	GoSub	BIN2BCD16			;convert to 6-digit BCD

	mov		temp,U				;copy decimal portion
	andi	temp,0x0F			;strip off upper digit
	st		Y,temp				;store humidity decimal to RAM@RH_D
	Subi16	YH,YL,1				;point to RAM@RH_I

	andi	U,0xF0				;strip low digit
	swap	U					;swap nibbles
	GoSub	decimaltobinary		;convert H:T:U to binary
	bld		U,7					;restore sign bit
	st		Y,U					;store humidity integer to RAM@RH_I
	ret							;return to caller

.endif
;
;*********************************************************************
; DHTSetup:	Sets up the timer and interrupt registers before
;	using them for the DHT sensor read.
;*********************************************************************
DHTSetup:
;enable watchdog timer for 16mS	
	STOREI	WDTCSR,(1<<WDCE|1<<WDE)
	STOREI	WDTCSR,((1<<WDIE)|(0<<WDP3)|(0<<WDP0)|(0<<WDE))
	wdr							;reset watchdog

	CLRB	GPIOR0,WDTIRQ		;insure WDTIRQ flag is clear

;enable pin-change interrupts for the DHTPIN
	STOREI	PCINT_I,(1<<PCINT_B);clear spurious PCINT's flag
	STOREI	PCINT_E,(1<<PCINT_B);enable pin-change IRQ's
	STOREI	PCINT_M,(1<<DHTPIN)	;enable PCINTx IRQ's

.if RTC
;disable Timer OCRA Interupts
	CLRB	RTC_TIMSK,RTC_OCIEA,temp;disable RTC interrupts
.endif

.if USBSUPP	;USB interrupts will screw us up!
	GoSub	D_USBINT			;disable USB ints
.else		;USART interrupts do too!
	CLRB	UCSRB,RXCIE,temp	;disable USART RX complete IRQ
.endif

	ret							;return to caller
;
;*********************************************************************
; DHTCleanup:	Cleans up the timer and interrupt registers after
;	using them for the DHT sensor read.
;*********************************************************************
DHTCleanup:
.if USBSUPP	;re-enable USB interrupts, USART are done in main loop
	GoSub	E_USBINT			;re-enable USB ints
.endif

	GoSub	WDDisable			;disable watchdog timer

.if PWM	;restore timer registers in case PWM was being used
	brts	DHTCleanup1			;branch if T-flag set, restore
	STOREI	HT_TCCRB,HTCK_0		;disable clock source
	Goto	DHTCleanup2			;continue

DHTCleanup1:
	pop		temp				
	STORE	HT_TCCRA,temp		;restore TIMER registers
	pop		temp				
	STORE	HT_TCCRC,temp		;restore TIMER registers
	pop		temp				
	STORE	HT_TIMSK,temp		;restore TIMER registers
	pop		temp				
	STORE	HT_TCCRB,temp		;restore TIMER registers
;
.else
	STOREI	HT_TCCRB,HTCK_0		;disable clock source
.endif

DHTCleanup2:
	SetYPtr	HTRdTmr 			;re-seed DHT read timeout
	STORE16	RT_TMR,YH,YL

.if RTC
	;re-enable Timer OCRA Interupts
	SETB	RTC_TIMSK,RTC_OCIEA,temp
.endif

	STOREI	PCINT_E,(0<<PCINT_B);disable pin-change IRQ's
	STOREI	PCINT_M,(0<<DHTPIN)	;disable PCINTx IRQ's
	STOREI	PCINT_I,(1<<PCINT_B);clear spurious PCINT's flag
	ret							;return to caller
;
