.if ICP
.include "Include/Defs_ICP.inc"			;include DEF's
;
;****************************************************************
;	ICP commands
;****************************************************************
;ICG [x] - set gate time for capture [0..7].  [x] is optional and
;			defaults to '0' to disable.
ICGcommand:
	clr 	U
	Gosub	pushU			;Put default on stack
	rcall	Get1stParm			;get the last parameter
	ldi		temp,7			;only values of [0..7] allowed
	cp		temp,U
	brmi	ICGcommand9		;branch if allowable exceeded 
	LOAD	temp,ICPCfg		;get ICPCfg
	andi	temp,$F8		;clear gate coefficient bits
	or		U,temp			;merge new gate coefficient
	STORE	ICPCfg,U		;save gate coefficient in RAM

	clr		temp
	STORE	ICP_TIMSK,temp	;no interrrupts
	STORE	ICP_TIFR,temp	;clear interrupt flags
	STORE	ICP_TCCRC,temp
	STORE	ICP_TCCRA,temp 	;CTS mode
#if TEST	;use system clock for testing
	ldi		temp,(	(0 << ICNC1) | \
					(0 << ICES1) | \
					(0 << WGM12) | \
					(1 << CS10) )
#endif
	STORE	ICP_TCCRB,temp 	; CTC, clock on rising edge
	ret

ICGcommand9:
	STOREI	ERRCode,$0A		;error code "A"
	rcall	error
;
;****************************************************************
;ICE - set input capture edge select to [0..1] where 0 = falling 
;		and 1 = rising edge.  [x] is optional and defaults rising
;		edge (1).
ICEcommand:	
	ldi 	U,1				;default '1' for rising edge
	Gosub	pushU			;Put default channel on stack
	rcall	Get1stParm			;get the last parameter
	ldi		temp,1			;only values of [0..1] allowed
	cp		temp,U
	brmi	ICEcommand9		;branch if allowable exceeded 
	LOAD 	temp,ICPCfg		;get ICPCfg from RAM
	andi	temp,$7F		;mask off out edge select bit
	sbrs	U,0				;skip next if bit 0 set
	rjmp	ICEcommand1
	ori		temp,$80		;rising edge select
ICEcommand1:
	STORE	ICPCfg,temp		;save ICPCfg in RAM
	ret

ICEcommand9:
	STOREI	ERRCode,$0C		;error code "C"
	rcall	error
;
;
;****************************************************************
;ICP - start capture with 'ICG' gatetime and return low-byte of 
;	ICP register, store high-byte in 'Z'
ICPcommand:	
	PUSHY						;save Y since it is the program
								; pointer when running a program
	ser		T					;set T
	clr		U					;zero U
	STORE	ICP_TIFR,T			;clear TIFR flags
	STORE	ICP_TCCRA,U			;clear TCCRx registers
	STORE	ICP_TCCRB,U
	STORE	ICP_TCCRC,U
	STORE16	ICP_TCNTL,U,U		;zero TCNT

	CLRB	ICP_PORT-1,ICP_PIN,temp	;insure port to input

.if USBSUPP	;USB interrupts will screw us up!
	GoSub	D_USBINT			;disable USB ints
.endif

.if DDS
	LOAD	temp,DDS_TIMSK		;always disable DDS ints
	push	temp
	STOREI	DDS_TIMSK,0,temp
.endif

	LOAD 	temp,ICPCfg			;get ICPCfg from RAM
	mov		H,temp				;copy in H
	andi	H,$80				;mask out edge select bit 
	clc							;clear carry flag
	rol		H					;rotate edge select bit into bit 0
	rol		H
	ori		H,$06				;merge in EXT clock bits

	andi	temp,$07			;mask out gate coefficient bits

	SetZPtr	ICPcommandTbl		;point to jump table
	ADD16	ZH,ZL,U,temp		;add gate index as offset
	ijmp						;indirect jump Z-Ptr

ICPcommandTbl:
	rjmp	ICPcommand8			;0 = ICP disabled
	rjmp	ICPcommand_0		;1 =   10mS gate time
	rjmp	ICPcommand_0		;2 =   25mS gate time
	rjmp	ICPcommand_0		;3 =   50mS gate time
	rjmp	ICPcommand_1		;4 =  100mS gate time
	rjmp	ICPcommand_1		;5 =  250mS gate time
	rjmp	ICPcommand_1		;6 =  500mS gate time
	rjmp	ICPcommand_2		;7 = 1000mS gate time

ICPcommand_0:	;10, 20 or 50 mS gate time
	cpi		temp,3				;need 50mS?
	brcs	ICPcommand_0a		;skip next if not 50mS
	ldi		temp,5				;adjust for 50mS
ICPcommand_0a:
	mov		r0,temp				;move counter into r0
	rjmp	ICPcommand_7		;execute delay

ICPcommand_1:	;100, 200 or 500 mS gate time
	ldi		T,10				;set up for multiply
	subi	temp,3				;get to 100's
	cpi		temp,3				;need 500mS?
	brcs	ICPcommand_1a		;skip next if not 500mS
	ldi		temp,5				;adjust for 500mS
ICPcommand_1a:
	mul		temp,T				;multiply 
	rjmp	ICPcommand_7		;execute delay

ICPcommand_2:	;10000mS gate time
	ldi		temp,100			;set up loop counter
	mov		r0,temp				;move counter into r0

ICPcommand_7:
	STORE16	ICP_TCNTL,U,U		;zero TCNT
	STORE	ICP_TCCRB,H			;enable counting
ICPcommand_7a:
	GoSub	Delay10mS			;wait 10mS
	dec		r0					;decrement counter
	brne	ICPcommand_7a		;loop till zero

ICPcommand8:	
.if DDS
	pop		temp				;restore DDS ints
	STORE	DDS_TIMSK,temp
.endif

.if USBSUPP
	GoSub	E_USBINT			;re-enable USB ints
.endif
	LOAD16	T,U,ICP_TCNTL		;get TCNT
	clr		temp				;stop ICP clock
	STORE	ICP_TCCRB,temp
	SKBC	ICP_TIFR,ICP_TOV	;skip next if no timer overflow
	rjmp	ICPcommand9
	SETYPTR	(VARSTOR+25)		;point Y to variable 'Z'
	st		Y,T					;save to variable 'Z'
	Gosub	pushU				;Put low-byte on stack
	POPY
	ret

ICPcommand9:
	POPY
	STOREI	ERRCode,$0E			;error code "E"
	rcall	error
;
.endif


