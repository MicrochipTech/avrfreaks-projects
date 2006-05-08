;Led Opto Light Meter @ 9.6 Mhz
         
.include		"tn13def.inc"

;-----		
.def	ValHld		= r10			
.def	ValLo		= r11			; 24 Bit Light Intensity Value
.def	ValHi		= r12

.def	Null		= r13			; Always = Zero

.def	LupCnt		= r14			; A2D Sample Count

.def	ChrOut		= r15			; 8 Bit Character Out	 
    
.def	Temp		= r16			; General Purpose Work Register

.def	BitCnt		= r17			; Serial Out Bit Count

.def	BitDly		= r18			; 104 uS Delay Constant

;------         
.Cseg
         
.Org	$0000

;-----         
Reset:			    						
      	ldi	Temp,	RamEnd		
     	out	Spl,	Temp			; Set Stack Pointer

	ldi	Temp,	$02			; Set Mark Bit High
	out	DdrB,	Temp	
	out	PortB,	Temp			

	sbi	AcsR,	Acd			; Disable Comparator 

	sbi	DidR0,	Adc2d			; Disable Adc2d as Digital Input  
			           		
	ldi	Temp,	$86			; Init + 150 Khz Sample Rate
	out	AdCsrA,	Temp

	ldi	Temp,	$42			; Adc2 + (AdLar=0) + 1.1V Analog Ref 
	out	AdMux,	Temp	
		
	clr	Null				; Null = 0

;-----         
Main:	
	clr	ValHld				; Clear Value Registers
	clr	ValLo
	clr	ValHi

;-----
Conv:	
	sbi	AdCsrA,	Adsc 			; En A/D Conv	
         
      	sbis	AdCsrA,	Adif			; Conv Comp ? 	
	rjmp	pc-1

	sbi	AdCsrA,	Adif			; Clr Conversion Flag

	in	Temp,	AdcL			; Read Lo order 8 Bits
	add	ValHld,	Temp

      	in	Temp,	AdcH			; Read Hi order 2 Bits
	adc	ValLo,	Temp

	adc	ValHi,	Null			; Add With Carry + $00

	dec	LupCnt				; Decrement Loop Count
	brne	Conv

	rcall	PrWord				; Send 16 Bit Light Value

	rjmp	Main

;------
PrWord:						; Word to Ascii
	mov	ChrOut,	ValHi
	rcall	PrByt				; Word 8 Bit High

	mov	ChrOut,	ValLo			; Word 8 Bit Low

;------
PrByt:						; Byte to Ascii
	mov	Temp,	ChrOut

	swap	Temp				; Reverse Nibbles
	rcall	NibAsc

	mov	Temp,	ChrOut

;------
NibAsc:						; Nibble to Ascii
	andi	Temp,	$0f

	cpi	Temp,	$0a			; > 10 ?
	brcs	pc+2

	subi	Temp,	-$07			; Adi 7

	subi	Temp,	-$30			; Adi 48
	
;------
PrChr:
	ldi	BitCnt,	9			; 9 Bit Counter
	
	clc					; Start Bit Out

	rcall	BiTime				; Out Bit

	lsr	Temp				; Shift next bit out

	dec	Bitcnt				; Decrement Count and Loop If Not Zero
	brne	pc-3

	sec					; Stop Bit Out

;-----
BiTime:
	
	brcs	pc+3				; Goto Mark

	cbi	PortB,	 PortB1			; Space Bit
	rjmp	Delay

	sbi	PortB,	 PortB1			; Mark Bit
	nop

;-----
Delay:			
	ldi  	BitDly,	$f8			; Delay 104 uS: (May need to be tuned ?)

	nop
  	dec	BitDly				; Dec Cnt and Loop
        brne 	pc-2	

	ret



	

