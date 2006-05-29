;			Tiny13 Mppt @ 9.6 Mhz

;-----         
.include		"tn13def.inc"


;------------------------------------------------------------------------------------------------
.equ	PwmNon		= $83			; Fast Non Inverting PWM


;------------------------------------------------------------------------------------------------
.def	Null		= r5			; Zero Constant

.def	ValLo		= r8			; 16 Bit A2D Value
.def	ValHi		= r9

.def	PowLo		= r10			; Previous Power Value
.def	PowHi		= r11

.def	DutLo		= r12			; 16-Bit Duty Cycle Register	
.def	DutHi		= r13

.def	ResLo		= r14			; 16 Bit Power Result
.def	ResHi		= r15
		
.def	Temp		= r16			; Scratch Register A
.def	Work		= r17			; Scratch Register B

.def	DutCyc		= r18			; Pwm Duty Cycle	

;----  
.Cseg
         
.Org	$0000


;------------------------------------------------------------------------------------------------      
Reset:			    						
      	ldi	Temp,	RamEnd		
     	out	Spl,	Temp			; Set Stack Pointer

	ldi	Temp,	$01			; Enable PBO as Output
	out	DdrB,	Temp	

	ldi	Temp,	$58			; Calibrate Internal Oscillator
	out	OscCal,Temp

	sbi	AcsR,	Acd			; Disable Comparator 

	sbi	DidR0,	Adc1d			; Disable Adc1d as Digital Input 	     
      		
	ldi	Temp,	$87			; Init + 75 Khz Sample Rate
	out	AdCsrA,Temp

	ldi	Temp,	$01			; Adc1 + (AdLar=0) + Vcc Analog Ref 
	out	AdMux,	Temp

	out	TccR0B,Temp			; No Prescaler ;Pwm Freq = 37500 Hz	

	ldi	Temp,	PwmNon			; Fast PWM; Non Invering Mode 
	out	TccR0A,Temp

	ldi	DutCyc,105			; Initialise Duty Cycle to 40%			

	clr	Null				; Set Null Constant to Zero

	
;------------------------------------------------------------------------------------------------
Main:
	rcall	ConVert

	cp	PowLo,	ValLo			; If Value >= Power then Dec Else Inc
	cpc	PowHi,	ValHi
	brcc	Dec				

Inc:							
	cpi	DutCyc,160			; If Duty Cycle = 62% then Exit	
	breq	Ex

	inc	DutCyc				; Else Increment Duty Cycle
	rjmp	Ex

Dec:	
	cpi	DutCyc,50			; If Duty Cycle = 20% then Exit
	breq	Ex

	dec	DutCyc				; Else Decrement Duty Cycle
	
Ex:
	movw	PowHi:PowLo,ValHi:ValLo	; Update Power State
	
	out	Ocr0A,	DutCyc			; Update PWM Duty Cycle

	rjmp	Main				; Loop back


;------------------------------------------------------------------------------------------------        
ConVert:
	
;* This routine gets an A2D,10-Bit sample. These samples are then stored in "ValHi:ValLo" 										
	clr	ValLo				; Clear Value Registers
	clr	ValHi
	
	sbi	AdCsrA,Adsc 			; En A/D Conv	
         
      	sbis	AdCsrA,Adif			; Conv Comp ? 	
	rjmp	pc-1

	in	Temp,	AdcL			; Read Lo order 8 Bits
	add	ValLo,	Temp
	
      	in	Temp,	AdcH			; Read Hi order 2 Bits
	adc	ValHi,	Temp
		
	sbi	AdCsrA,Adif			; Clr Conversion Flag

	ret 


	