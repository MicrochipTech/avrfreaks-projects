;Led Opto Lo Pass Light Meter @ 9.6 Mhz
         
.include		"tn13def.inc"

;-----
.equ	BiTime		= $f8			; 104 uSec Delay Constant

.equ	BitCnt		= $09			; 9 Bit Counter

.equ	Calib		= $63			; RC Calibration Byte

;-----	
.def	ValLo		= r6			; 10-Bit Light Intensity Value
.def	ValHi		= r7

.def	Null		= r8			; Zero Constant
.def	FilLo		= r9			
.def	FilMd		= r10			; 20-Bit Filter Register	
.def	FilHi		= r11
	

.def	TmpLo		= r12			; 20-Bit Temporary Register
.def	TmpMd		= r13			
.def	TmpHi		= r14
.def	ChrOut		= r15			; 8 Bit Character Out	 
    
.def	Temp		= r16			; General Purpose Work Registers
.def	TempA		= r17			
.def	TempB		= r18	

.def	SampLo		= r24			; 561 Times Sample Counter
.def	SampHi		= r25		

;------         
.Cseg
         
.Org	$0000

;-----         
Reset:			    						
      	ldi	Temp,	RamEnd		
     	out	Spl,	Temp			; Set Stack Pointer

	ldi	Temp,	Calib			; Calibrate Internal Oscillator
	out	OscCal,	Temp
	
	sbi	DdrB,	DdB0			; Set Mark Bit High		
	sbi	PortB,	PortB0

	sbi	AcsR,	Acd			; Disable Comparator 

	sbi	DidR0,	Adc2d			; Disable Adc2d as Digital Input  
			           		
	ldi	Temp,	$86			; Init + 150Khz Sample Rate
	out	AdCsrA,	Temp

	ldi	Temp,	$02			; Adc2 + (AdLar=0) + Vcc Analog Ref 
	out	AdMux,	Temp	
		
	clr	Null				; Null = 0

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
	
	movw	TmpMd:TmpLo,FilHi:FilMd		; Filter Register shifted 8*Right

	rcall	PrWord				; Send 16 Bit Filtered Light Value

	rjmp	Main

;------
PrWord:						
	mov	ChrOut,	TmpMd
	rcall	PrByte				; Word 8 Bit High

	mov	ChrOut,	TmpLo			; Word 8 Bit Low
	rcall	PrByte	

	ldi	Temp,	' '			; Send Space
	rjmp	PrChar

;------
PrByte:						
	mov	Temp,	ChrOut			; Fetch	ChrOut

	swap	Temp				; Reverse Nibbles
	rcall	NibAsc

	mov	Temp,	ChrOut			; Fetch	ChrOut once more

;------
NibAsc:						
	andi	Temp,	$0f

	cpi	Temp,	$0a			; > 10 ?
	brcs	pc+2

	subi	Temp,	-$07			; Adi 7

	subi	Temp,	-$30			; Adi 48
	
;------
PrChar:
	ldi	TempA,	BitCnt			; Bit Counter
	
	clc					; Start Bit Out

	rcall	SetBit				; Out Bit

	lsr	Temp				; Shift next bit out

	dec	TempA				; Decrement BitCnt Counter and Loop If Not Zero
	brne	pc-3

	sec					; Stop Bit Out

;-----
SetBit:	
	brcs	pc+3				

	cbi	PortB,	 PortB0			; Space Bit if Carry Set
	rjmp	Delay

	sbi	PortB,	 PortB0			; Mark Bit if Carry Clear

;-----
Delay:			
	ldi  	TempB,	BiTime			; Delay 104 uS:

	nop
  	dec	TempB				; Decrement BiTime Counter and Loop If Not Zero
        brne 	pc-2	

	ret


