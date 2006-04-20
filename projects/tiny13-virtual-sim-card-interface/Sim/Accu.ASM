;			Akku Voltage Logger @ 3.579545Mhz 
         
.include		"tn13def.inc"

;-----	
.def	Null		= r12
.def	Hold		= r13					
.def	Value		= r11	
.def	ValCnt  	= r15
       
.def	Temp		= r16		
.def	BitCnt		= r17				
.def	ChrIn		= r18	
.def	ChrOt		= r19

;------         
.Cseg
         
.Org	$0000

rjmp	Reset				

;------         
GetByt:			   						; Int0 Vector				
	out	GimSk,	Null		; Dis Ext Int	
 
	ldi	ChrIn,	$80		; Bit Counter = 1000 0000

Inp:	rcall	Baud			; Delay 1 Baud
 
 	in	Hold,	PinB		; Input PinB
      	lsr	Hold
	lsr	Hold			; Shift Pin1 into Carry

      	ror	ChrIn			; Rotate Carry into Bit Counter
      	brcc	Inp

	rcall	Baud			; Delay for Parity Bit

	rcall	Stop3			; 3 * Baud Slots 

	reti

;-----         
Reset:			    						; Reset Vector
      	ldi	Temp,	RamEnd		
     	out	Spl,	Temp		; Set Stack Pointer
	
	sbi	AcsR,	Acd		; Disable Comparator                     
      			 
   	sbi	DidR0,	Adc2d		; Disable Adc2d Digital Input

	ldi	Temp,	$62 		       
      	out	AdMux,	Temp		; Adc2 + (AdLar=1) + IntRef

	ldi	Temp,	$84		; Init + 222Khz Sample Rate
	out	AdCsrA,	Temp 

	clr	Null 

	ldi	ChrOt,	$3b			
	rcall	SndByt			; TS = Initial Sync Char

;-----         
Main:									; 256 Samples    
	sbi	AdCsrA,	Adsc		; En A/D Conv		
         
      	sbis	AdCsrA,	Adsc		; Conv Comp ? 	
	rjmp	pc-1

      	in	Temp,	AdcH		; Read Voltage

	add	Value,	Temp		; Accumulate
	ror	Value
	adc	Value,	Null

	dec	ValCnt			; Finished ?
	brne	Main

Int:	ldi	Temp,	$40		; Enable ExtInt
	out	GimSk,	Temp	
	
	ldi	Temp,	$30		; En Sleep + Lo Level
	out	McuCr,	Temp

	sei				; Enable Interrupt

	sleep				; PowerDown

	out	McuCr,	Null		; Wake Up

	inc	ChrIn			; ChrIn = $ff ?
	brne	Int

	mov	ChrOt,	Value		; Get Value

	subi	ChrOt,	187		; Minus Offset 4 to 6 Volt1
	rcall	SndByt

	rjmp	Main

;------
SndByt:									; ChrOt Out
	sbi	Ddrb,	Ddb1		; Set Bit1 Ddrb = Output

	ldi	BitCnt,	9		; BitCnt	

	clr	Hold			; Parity = 0	
	
	clc				; Start Bit
	
Out:	rcall	BitOut			; Delay 1 Bit Time	

	lsr	ChrOt			; Output ChrOt

	dec	BitCnt			; 8 Bits out ?
	brne	Out

	lsr	Hold			; Parity Bit
	rcall	BitOut

	cbi	Ddrb,	Ddb1		; Clr DdrB Bit1 = Input	

;-----
Stop3:									; 3 * Baud Time Delay
	rcall	Baud
	rcall	Baud
	rjmp	Baud

;------	
BitOut:									; Output Carry
	brcs	Hi

Lo:	cbi	PortB ,	1		; Clr Pin1 PortB  
	rjmp	Baud
				
Hi:	sbi	PortB ,	1		; Set Pin1 PortB 	

	inc	Hold			; Parity = Parity + 1

;-----
Baud:									; 104 uS Delay
	ldi	Temp,	$77		; Count
	
	dec	Temp			; Delay Loop 
	brne	pc-1	

	ret	

