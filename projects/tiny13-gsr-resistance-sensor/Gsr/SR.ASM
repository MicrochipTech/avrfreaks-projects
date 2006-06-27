;Gsr Meter @ 9.6 Mhz
         
.include		"tn13def.inc"

;-----		
.def	ValHld		= r11			; 24 Bit Skin Resistance Value
.def	ValLo		= r12
.def	ValHi		= r13

.def	Null		= r14			; Zero Constant
.def	ChrOut		= r15			; 8 Bit Character Out	 
    
.def	Temp		= r16			; General Purpose Work Registers
.def	TempA		= r17			
.def	TempB		= r18			

;------         
.Cseg
         
.Org	$0000

;-----         
Reset:			    						
      	ldi	Temp,	RamEnd		
     	out	Spl,	Temp			; Set Stack Pointer

	ldi	Temp,	$63			; Calibrate Internal Oscillator
	out	OscCal,Temp

	sbi	AcsR,	Acd			; Disable Comparator 

	sbi	DidR0,	Adc2d			; Disable Adc2d as Digital Input  
			           		
	ldi	Temp,	$87			; Init + 75Khz Sample Rate
	out	AdCsrA,Temp

	ldi	Temp,	$02			; Adc2 + (AdLar=0) + Vcc Analog Ref 
	out	AdMux,	Temp	
		
	clr	Null				; Zero Constant

;-----         
Main:	
	ldi	Temp,	$07			; Set Mark Bit + Clr Leds 
	out	DdrB,	Temp	
	out	PortB,	Temp			

	clr	ValHld				; Clear Value Registers
	clr	ValLo
	clr	ValHi

;-----
Conv:	
	sbi	AdCsrA,Adsc 			; Enable A2D Conversion	
         
      	sbis	AdCsrA,Adif			; Conversion Complete ? 	
	rjmp	pc-1

	in	Temp,	AdcL			; Read Lo order 8 Bits
	add	ValHld,Temp

      	in	Temp,	AdcH			; Read Hi order 2 Bits
	adc	ValLo,	Temp

	adc	ValHi,	Null			; Add With Carry + $00

	sbi	AdCsrA,Adif			; Clr Conversion Flag

	dec	r24				; Decrement Loop Count
	brne	Conv

	ldi	TempA,	$ff			; Ref 1/2 Voltage Span
	ldi	TempB,	$01

	cp	ValLo,	TempA			; If Carry Clear then Green 
	cpc	ValHi,	TempB

	brcs	pc+3				; Else Red

	cbi	PortB,	 PortB1		; Green True Led On
	rjmp	pc+2

	cbi	PortB,	 PortB2		; Red False Led On

	rcall	PrWord				; Send 10 Bit Resistance Value

	rjmp	Main

;------
PrWord:						
	mov	ChrOut,ValHi
	rcall	PrByte				; Word 8 Bit High

	mov	ChrOut,ValLo			; Word 8 Bit Low
	rcall	PrByte	

	ldi	Temp,	' '			; Send Space
	rjmp	PrChar

;------
PrByte:						
	mov	Temp,	ChrOut			; Fetch ChrOut

	swap	Temp				; Reverse Nibbles
	rcall	NibAsc

	mov	Temp,	ChrOut			; Fetch ChrOut once more

;------
NibAsc:						
	andi	Temp,	$0f

	cpi	Temp,	$0a			; > 10 ?
	brcs	pc+2

	subi	Temp,	-$07			; Adi 7

	subi	Temp,	-$30			; Adi 48
	
;------
PrChar:
	ldi	TempA,	9			; 9 Bit Counter
	
	clc					; Start Bit Out

	rcall	BiTime				; Out Bit

	lsr	Temp				; Shift next bit out

	dec	TempA				; Decrement Count and Loop If Not Zero
	brne	pc-3

	sec					; Stop Bit Out

;-----
BiTime:
	
	brcs	pc+3				; Goto Mark

	cbi	PortB,	 PortB0		; Space Bit
	rjmp	Delay

	sbi	PortB,	 PortB0		; Mark Bit

;-----
Delay:			
	ldi  	TempB,	$f8			; Delay 104 uS:

	nop
  	dec	TempB				; Dec Cnt and Loop
       brne 	pc-2	

	ret




	

