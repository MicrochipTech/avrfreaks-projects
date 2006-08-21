;Psu @ 9.6 Mhz
         
.include		"tn13def.inc"

;------------------------------------------------------------------------------------------------
.equ	PwmNon		= $83			; Fast Non Inverting PWM

.equ	Calib		= $63			; RC Calibration Byte

.equ	Delay		= $f8			; 104u Second Delay Time

.equ	AdcVal		= $86			; Init + 150Khz Sample Rate

.equ	MuxVal		= $02			; Adc2 + (AdLar=0) + VCC Analog Ref 

.equ	ExtInt		= $40			; Enable External Interrupt on Low Level


;----
.def	PreVal		= r4			; Previous A2D Value

.def	Status		= r5			; Holding Register for SREG

.def	ValLo		= r6			; 10-Bit Light Intensity Value
.def	ValHi		= r7

.def	Null		= r8			; Zero Constant

.def	FilLo		= r9			
.def	FilMd		= r10			; 20-Bit Filter Register	
.def	FilHi		= r11
	

.def	TmpLo		= r12			; 20-Bit Temporary Register
.def	TmpMd		= r13			
.def	TmpHi		= r14

.def	Status		= r15			; Holding Register for SREG
    
.def	Temp		= r16			; General Purpose Work Registers
.def	TempA		= r17			
.def	TempB		= r18	

.def	SerialA		= r19			; General Purpose Serial Registers
.def	SerialB		= r20			
.def	SerialC		= r21	

.def	SampLo		= r24			; 561 Times Sample Counter
.def	SampHi		= r25		

;------         
.Cseg
         
.Org	$0000

	rjmp	Reset				

;------         
GetByt:	
	in	Status,	SREG			; Save Status Register
		   								
	out	GimSk,	Null			; Disable External Interrupt	
 
	ldi	SerialA,$80			; Bit Counter = 1000 0000

	ldi	SerialB,Delay			; 104u Second Delay

	nop
	dec	SerialB
	brne	pc-2
 
 	in	SerialC,PinB			; Input PortB

      	lsr	SerialC				; Shift Pb1 into Carry
	lsr	SerialC				

      	ror	SerialA				; Rotate Carry into Bit Counter

      	brcc	pc-8				; If Not 8-Bits then Loop 

	out	Ocr0A,	SerialA			; Update PWM Duty Cycle with SerialA			; Store in Byte InPut

	out	SREG,	Status			; Restore Status Register

	reti
		
;----
Reset:			    						    						
      	ldi	Temp,	RamEnd		
     	out	Spl,	Temp			; Set Stack Pointer

	ldi	Temp,	Calib			; Calibrate Internal Oscillator
	out	OscCal,	Temp

	sbi	AcsR,	Acd			; Disable Comparator 

	ldi	Temp,	$01
	out	DdrB,	Temp			; Pb0 as Output
	out	PortB,	Temp	
		
	out	TccR0B,	Temp			; No Prescaler ;Pwm Freq = 37500 Hz	

	ldi	Temp,	PwmNon			; Fast PWM; Non Invering Mode 
	out	TccR0A,	Temp	

	sbi	DidR0,	Adc2d			; Disable Adc2d as Digital Input  
			           		
	ldi	Temp,	AdcVal			; Initial ADC Register
	out	AdCsrA,	Temp

	ldi	Temp,	MuxVal			; Initialise Mux Register
	out	AdMux,	Temp	

	ldi	Temp,	ExtInt			; Enable Interrupt External 
	out	GimSk,	Temp	

	clr	Null				; Set Null Constant to Zero

	sei					; General Interrupt Enable

;-----         
Main:	
	ldi	SampHi,	$02			; Set Sample count to 561			; 
	ldi	SampLo,	$31

;-----
Conv:		
	sbi	AdCsrA,	Adsc 			; En A/D Conv	
         
      	sbis	AdCsrA,	Adif			; Conv Comp ? 	
	rjmp	pc-1

	in	ValLo,	AdcL			; Read Lo order 8 Bits

      	in	ValHi,	AdcH			; Read Hi order 2 Bits

	sbi	AdCsrA,	Adif			; Clr Conversion Flag

	movw	TmpMd:TmpLo,FilHi:FilMd		; Filter Register shifted 8*Right

	sub	FilLo,	TmpLo			; Filter Register = Filter Register - Shift
	sbc	FilMd,	TmpMd
	sbc	FilHi,	Null

	add	FilLo,	ValLo			; Filter Register = Filter Register + Input
	adc	FilMd,	ValHi
	adc	FilHi,	Null

	sbiw	SampLo,	1			; Decrement Sample Loop Count
	brne	Conv		
	
	lsr	FilHi				; Scale Filter into 8-Bits
	ror	FilMd				
	lsr	FilHi	
	ror	FilMd

	cpse	FilMd,	PreVal			; Compare New and Old A2D Values ...
						; ...  and Skip Update If Equal

	out	Ocr0A,	FilMd			; Update PWM Duty Cycle with A2D value

	mov	PreVal,	FilMd			; Previous Value = New Value

	rjmp	Main				; Loop forever

	