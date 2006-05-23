;		Smps Pid  @ 9.6Mhz 
         
.include		"tn13def.inc"

.equ	SetPoint	= $d1			; Desired Voltage, 5.25V

.equ	PwmNon		= $83			; Fast Non Inverting PWM

;-----	
.def	Null		= r4			; Zero Constant
.def	Volt		= r5			; A2D Voltage

.def	PrerrL		= r6			; Present Error Registers
.def	PrerrH		= r7
			
.def	PaerrL		= r8			; Past Error Registers	
.def	PaerrH		= r9				
		 
.def	IntegL		= r10			; Integral Component Registers
.def	IntegH		= r11			

.def	AccumL		= r12			; Sum Accumulation Registers
.def	AccumH		= r13			

.def	DeltaL		= r14			; Difference/Delta Component Registers
.def	DeltaH		= r15			
       
.def	Temp		= r16			; Scratch Registers	
.def	TempA		= r17	

;------         
.Cseg
         
.Org	$0000
			
;-----         
Reset:			    						
      	ldi	Temp,	RamEnd		
     	out	Spl,	Temp		; Set Stack Pointer
	
	ldi	Temp,	$5b		; Calibrate Internal Oscillator
	out	OscCal,	Temp

	sbi	DdrB,	Ddb0		; Enable PBO as Output

	sbi	AcsR,	Acd		; Disable Comparator                     
      			 
   	sbi	DidR0,	Adc2d		; Disable Adc2d Digital Input

	ldi	Temp,	$42		; Adc2 + (AdLar=0) + 1.1V Analog Ref 
	out	AdMux,	Temp	

	ldi	Temp,	$84		; Init + 222 Khz A2D Sample Rate
	out	AdCsrA,	Temp 

	ldi	Temp,	$01		; No Prescaler ;Pwm Freq = 37500 Hz	
	out	TccR0B,	Temp		

	ldi	Temp,	PwmNon		; Fast PWM; Non Invering Mode 
	out	TccR0A,	Temp	

	clr	Null			; Set Null Constant to Zero

;-----         
Main:									    
	sbi	AdCsrA,	Adsc		; En A/D Conv		
         
      	sbis	AdCsrA,	Adif		; Conv Comp ? 	
	rjmp	pc-1

	in	Volt,	AdcL		; Read Low byte

      	in	Temp,	AdcH		; Read Hi order 2 Bits

	sbi	AdCsrA,	Adif		; Clr Conversion Flag

	cpi	Temp,	$03		; If Hi Order 2 Bits = "11" then OK
	breq	Error

	clr	Volt			; Else Low Byte = Zero

;-----	
Error:

;* Finds Difference (Error) between the Reference Voltage in SetPoint and 
;* the Current Voltage in Volt

;* PrerrH:PrerrL = SetPoint - Volt

;* Input  Variable = SetPoint   desired Output Voltage

;* Output Variable = PrerrH:PrerrL    Signed Error.

	
	ldi	TempA,	SetPoint		; Get Reference Voltage

	clr	PrerrH				; Clear Error High

	sub	TempA,	Volt			; Subtract Actual from Reference Voltage

	brcc	pc+2				; Was there a Borrow ?	

	dec	PrerrH				; If Yes then PrerrH = $ff; Else EOH = $00

	mov	PrerrL,	TempA			; store in current Error

;----
Prop:

;* AccumH:AccumL = PrerrH:PrerrL * Kp

;* Kp = 8   produced by 3 Left Shifts 

;* Input  Variable = PrerrH:PrerrL = Error and  Kp is the Proportional Gain factor

;* Output Variable = AccumH:AccumL =  Proportional Component.


	movw	AccumH:AccumL,PrerrH:PrerrL	; Mov Error into Sum

	lsl	AccumL				; Multiply Sum by 8
	rol	AccumH
	lsl	AccumL				
	rol	AccumH
	lsl	AccumL				; Proportional result in Sum 
	rol	AccumH

;----
Diff:

;* Finds difference/Delta between Present Loop Error and Previous Loop Error

;* DeltaH:DeltaL =(PaerrH:PaerrL - PrerrH:PrerrL) * Kd: Kd = 8 (Three Left Shifts)

;* Input Variables:	
;* PaerrH:PaerrL Previous Loop Error: PrerrH:PrerrL Pesent Loop Error
;* Kd = Differential Gain factor

;* Output Variable = DeltaH:DeltaL =  Differentional Component.


	Movw	DeltaH:DeltaL,PrerrH:PrerrL	; Move Error into Delta
	
	sub	DeltaL,	PaerrL			; Subtract Previous from Current Error
	sbc	DeltaH,	PaerrH			; and store in Delta

	lsl	DeltaL				; Multiply Delta by 8
	rol	DeltaH
	lsl	DeltaL				
	rol	DeltaH
	lsl	DeltaL				
	rol	DeltaH

;-----
Integ:

;* Multiplies Present Error by Ki and adds result to IntegH:IntegL

;* IntegH:IntegL =IntegH:IntegL + PrerrH:PrerrL * Ki: Ki = 8

;* Input Variables:	
;* PrerrH:PrerrL				: Present Loop Error
;* IntegH:IntegL				: Running Error Total
;* Ki						: Integral Gain factor

;* Output Variable = IntegH:IntegL 		: Integral Component.

	movw	PaerrH:PaerrL,PrerrH:PrerrL	; Update Previous Error

	lsl	PrerrL				; Multiply Error by 8
	rol	PrerrH
	lsl	PrerrL				
	rol	PrerrH
	lsl	PrerrL				
	rol	PrerrH

	add	IntegL,	PrerrL			; Add Result to Integral
	adc	IntegH,	PrerrH
	
;-----
Total:

;* Sums Proportional. Integral and Differential components

;* AccumH:AccumL =IntegH:IntegL + DeltaH:DeltaL +ProH:ProL * Kd

;* Input Variables:	IntegH:IntegL, PwmH:PwmL, DeltaH:DeltaL 

;* Output Variable:	AccumH:AccumL ;   Sum of Components.


	add	AccumL,	IntegL			; Add in the Integral
	add	AccumH,	IntegH

	add	AccumL,	DeltaL			; Add in the Differential
	add	AccumH,	DeltaH

	brpl	pc+3				; Total in  Sum Negative ?
					
	clr	AccumH				; Yes then Clear Sum Register ...
	clr	AccumL
	
	lsr	AccumH				
	ror	AccumL
	lsr	AccumH				; Divide Sum by 8
	ror	AccumL
	lsr	AccumH
	ror	AccumL

	out	Ocr0A,	AccumL			; Update PWM Duty Cycle

	rjmp	Main


