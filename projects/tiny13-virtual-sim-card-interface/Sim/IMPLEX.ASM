

;			Software Sim Card @ 3.579545Mhz 

.include		"tn13def.inc"

;-----
.equ	Half		= $38		; Delay Bit Times	
.equ	Full		= $75

;-----	
.def	Null		= r14
.def	Hold		= r15					       
.def	Temp		= r16		
.def	Eins7		= r17				
.def	Eins8		= r18	
.def	ChrOt		= r19
.def	Fiche		= r20
         
;------
.Cseg
         
.Org	$0000


;------
Start:
	ldi	Temp,	RamEnd		
     	out	Spl,	Temp		; Set Stack Pointer
	
	sbi	AcsR,	Acd		; Disable Comparator                     
      			 
   	sbi	DidR0,	Adc2d		; Disable Adc2d Digital Input

	ldi	Temp,	$62 		       
      	out	AdMux,	Temp		; Adc2 + (AdLar=1) + 1.1 Volt Ref

	ldi	Temp,	$84		; Init + 222Khz ADC Sample Rate
	out	AdCsrA,	Temp  

	ldi	ChrOt,	$3b			
	rcall	SndByt			; TS = Initial Sync Char

	clr	Null

	ldi	Temp,	$00		; T0 = Format Char
	rcall	SndByt

;------         
Main:	
	rcall	GetByt
		
	rcall	SndByt

	rjmp	Main
	
;------         
GetByt:	
     	sbic	PinB,	PinB1		; Pin1 PinB Lo ?      
	rjmp	GetByt

	ldi	Eins8,	Half		; Delay Half EtuBit Time 52uS
	dec	Eins8			 	
	brne	pc-1			
 
	ldi	Temp,	$80		; Bit Counter = 1000 0000

Gbt:	rcall	EtuBit			; Delay 1 EtuBit Time
         
      	in	Hold,	PinB		; Input Pin1 PinB
      	lsr	Hold
	lsr	Hold			; Shift into Carry
         
      	ror	Temp			; Rotate Carry into Bit Counter
      	brcc	Gbt

	rjmp	Guard			; 16 Etu Guard Delay Time
			
;------
SndByt:
	sbi	DdrB,	DdB1		; Set Bit1 Ddrb = Output	
	
	clc
	rcall	BitOut			; Start Bit

	ldi	Eins7,	8		; BitCnt	

	clr	Hold			; Parity = 0

Sbt:	lsr	Temp			; Output Byte
	rcall	BitOut

	dec	Eins7			; 8 Bits out ?
	brne	Sbt

	lsr	Hold			; Parity Bit
	rcall	BitOut

	cbi	DdrB,	DdB1		; DdrB Bit1 = Input	

	rjmp	Guard			; 16 Etu Guard Delay Time
					
;------	
BitOut:
	brcs	Hi

Lo:	cbi	PortB,	PortB1		; Clr Pin1 PortB  
	rjmp	EtuBit
				
Hi:	sbi	PortB,	PortB1		; Set Pin1 PortB 	

	inc	Hold			; Parity = Parity + 1

	rjmp	EtuBit			; Delay 1 Etu Bit Time

;-----
Guard:					; 16 Etu Time Slots
	ldi	Fiche,	15

	rcall	EtuBit
	dec	r21
	brne	pc-2

;-----
EtuBit:
	ldi	Eins8,	Full		; 104 uS
	
	dec	Eins8			; Delay Routine
	brne	pc-1	

	ret	