;Opto RGB Color Sensor @ 13 Mhz
         
.include		"tn13def.inc"

;-----
.equ	Red		= $01			; Red   Sensor @ 100%

.equ	Grn		= $15			; Green Sensor @ 100%

.equ	Blu		= $11			; Blue  Sensor @ 100%

;-----	
.def	BytOut		= r15			; Serial 8-Bit Output Register

.def	Temp		= r16			; General Purpose Scratch Registers
.def	TempA		= r17			
.def	TempB		= r18	

.def	CntLo		= r19			; 24 Bit Period Count Registers
.def	CntMd		= r20	
.def	CntHi		= r21	

.def	Null		= r22			; Zero Constant Register

.def	Eins		= r23			: Ones Constant Register	

.def	TimLo		= r24			; Bit Time = 104 uS
.def	TimHi		= r25	
     	
;------         
.Cseg
         
.Org	$0000
	
	rjmp	Reset

.Org	$0001					; External Interrupt0

;-----
ExtInt:	
	clt					; T Flag Bit to 0	
	reti

;-----         
Reset:			    						
      	ldi	Temp,	RamEnd			; Set Stack Pointer
     	out	Spl,	Temp	

	ldi	Temp,	$15			; Enable Pb1 + Pb3 as Inputs
	out	DdrB,	Temp

	sbi	AcsR,	Acd			; Disable Comparator 

	ldi	Temp,	$40			; Enable External Interrupt
	out	GimsK,	Temp
		
	clr	Null				; Null Constant = 0

	ldi	Eins,	1			; One  Constant = 1

;-----         
Main:
	ldi	Temp,	Red			; Select + Send Red   24-Bit Light Value
	rcall	ConVrt

	ldi	Temp,	Grn			; Select + Send Green 24-Bit Light Value
	rcall	ConVrt

	ldi	Temp,	Blu			; Select + Send Blue  24-Bit Light Value
	rcall	ConVrt
					
	rcall	CarRet				; Send Carriage Return Character
	
	rjmp	Main				; Go Back to Main

;----- 
ConVrt:
	out	PortB,	Temp			; Enable Color Selection

	clr	CntLo				; Clear Count Registers
	clr	CntMd
	clr	CntHi

	ldi	Temp,	$22			; En Idle + Falling Edge
	out	McucR,	Temp

	sei					; Enable Global Interrupts	

	sleep					; Idle Mode

	set					; T Flag Bit to 1

	add	CntLo,	Eins			; Increment Period Count 
	adc	CntMd,	Null
	adc	CntHi,	Null			; 5 Clock Cycle Loop

	brts	pc-3				; If T Flag Still Set then Loop

	ldi	Temp,	3
	add	CntLo,	Temp			; If Not then Adjust Period Count 	
	adc	CntMd,	Null
	adc	CntHi,	Null

	cli					; Disable Global Interrupts

;------
SerOut:
	mov	BytOut,	CntHi			; Send Hi  Order Byte
	rcall	PrByte
	
	mov	BytOut,	CntMd			; Send Mid Order Byte
	rcall	PrByte

	mov	BytOut,	CntLo			; Send Lo  Order Byte
	rcall	PrByte		
	
;-----
Space:
	ldi	Temp,	$20			; Send Space Character
	rjmp	PrChar

;-----
CarRet:
	ldi	Temp,	$0d			; Send Carriage Return Character
	rjmp	PrChar	
				
;------
PrByte:						
	mov	Temp,	BytOut			; Fetch	BytOut

	swap	Temp				; Reverse Nibbles
	rcall	NibAsc

	mov	Temp,	BytOut			; Fetch	BytOut once more

;------
NibAsc:						
	andi	Temp,	$0f			; Mask Off Low Nibble

	cpi	Temp,	$0a			; > 10 ?
	brcs	pc+2

	subi	Temp,	-$07			; Adi 7

	subi	Temp,	-$30			; Adi 48
	
;------
PrChar:
	ldi	TempA,	9			; Bit Counter
	
	clc					; Start Bit Out

	rcall	OutBit				; Send  Bit Out

	lsr	Temp				; Shift next bit out

	dec	TempA				; Decrement Bit Counter + Loop If Not Zero
	brne	pc-3

	sec					; Stop Bit Out

;-----
OutBit:	
	brcs	pc+3				

	cbi	PortB,	 PortB0			; Space Bit if Carry Set
	rjmp	Delay

	sbi	PortB,	 PortB0			; Mark Bit if Carry Clear	
	nop

;-----
Delay:			
	ldi  	TimHi,	$01			; Delay 104 uS Counter @ 13 Mhz
	ldi  	TimLo,	$51	

  	sbiw	TimLo,	1			; Decrement Delay Counter + Loop If Not Zero
        brne 	pc-1	

	ret


