;White Noise Rain Sound Generator @ 9.6 Mhz
         
.include		"tn13def.inc"

;---- 
.def	Null		= r10			; Zero Constant Register 

.def	Reg1		= r12			; 32-Bit Random Registers
.def	Reg2		= r13			
.def	Reg3		= r14			
.def	Reg4		= r15
  
.def	Temp		= r16			; General Purpose Work Register

.def	Rand		= r17			; Random Number Constant Register	

.def	Hold		= r18			; Holds the RNG Carry Bit

.def	Mask		= r19			; Pb3, Pb4 Mask Register	

.def	TimLo		= r24			; Down Timer Registers
.def	TimHi		= r25

.def	SessLo		= r26			; Session Timer Registers
.def	SessHi		= r27

;------         
.Cseg
         
.Org	$0000

;------ 
Start:
	ldi	Temp,	$07			; Pb0, Pb1, Pb2 High
	out	DdrB,	Temp	

	ldi	Mask,	$18			; Set Bits Pb3, Pb4 High
	out	PortB,	Mask	

	sbi	AcsR,	Acd			; Disable Comparator

	ldi	SessHi,	$20			; 14 Min Session Duration
	ldi	SessLo,	$00

	in	Temp,	PinB			; Read Dil Switch

	and	Temp,	Mask			; Mark Off Pb3, Pb4

	lsr	Temp				; Shift Right 3 Times
	lsr	Temp
	lsr	Temp

	breq	pc+5				; If Temp = 0 then exit Loop	

	lsl	SessLo				; If not then Double Session Time
	rol	SessHi

	dec	Temp				; Decrement and Loop
	rjmp	pc-4

	rcall	BootUp				; Restore Previous RNG State

	clr	Null				; Zero Constant = 0

	ldi	Rand,	$f8			; RNG Constant Register = 1111 1000

;-----	
Main:   
	lsr 	Hold 				; Shift Bit 33 in RNG out
			
	rol 	Reg1				; 33 Bit RNG @ 533Khz Clock 		
	rol 	Reg2				
       	rol 	Reg3
       	rol 	Reg4 

	mov	Hold,	Reg3			; Fetch Reg3 into Hold

	andi	Hold,	$08			; Mask Off Bit 20 in RNG

	adc	Hold,	Rand			; Eor RNG Bits 20 and 33
	adc	Hold,	Null

	andi	Hold,	$01			; Mask Off Eor result in Hold
			
	mov	Temp,	Reg2			; Fetch Reg2 into Temp

	andi	Temp,	$03			; Mask Off Bits Pb0,Pb1

	or	Temp,	Mask			; Combine Bits

	out	PortB,	Temp			; Send out to Pins

	sbiw	TimLo,	1			; Decrement Down Counter
	brne	Main

	sbiw	SessLo,	1			; Decrement Session Counter
	brne	Main

;-----	       
Ende:
	out	PortB,	Null			; All Outputs Low	

	rcall	Store				; Save RNG State

	sei					; Enable Global Interrupts

	ldi 	Temp, 	$70			; Power Down + Disable Pull Ups
        out 	McuCr, 	Temp 

        sleep					; Deep Deep Sleep 

	rjmp	Start				; Do Again

;-----
BootUp:
	rcall 	EERead 				; Load 1 Random Register from EEProm
        mov 	Reg1,	Temp

        rcall 	EERead 				; Load 2 Random Register from EEProm
        mov 	Reg2,	Temp

        rcall 	EERead   			; Load 3 Random Register from EEProm      
        mov 	Reg3, 	Temp

        rcall 	EERead				; Load 4 Random Register from EEProm
        mov 	Reg4, 	Temp

	rcall 	EERead				; Load Hold Register from EEProm
        mov 	Hold, 	Temp
	ret

;-----
EERead:  
        sbic 	Eecr, 	Eewe			; EeProm Ready?
        rjmp 	EERead

        out 	Eear,  	Null			; Load Temp from EEProm
        sbi 	Eecr,  	Eere
       	in 	Temp,   Eedr

	inc 	Null				; Increment EEProm Index Register 
        ret

;-----
Store: 
	mov 	Temp, 	Reg1			; Store 1 Random Register in EEProm  
        rcall 	EEWrit

        mov 	Temp, 	Reg2			; Store 2 Random Register in EEProm  
        rcall 	EEWrit

       	mov 	Temp, 	Reg3			; Store 3 Random Register in EEProm  
       	rcall 	EEWrit

	mov 	Temp, 	Reg4			; Store 4 Random Register in EEProm  
        rcall 	EEWrit

        mov 	Temp, 	Hold			; Store Hold Register in EEProm  

;-----
EEWrit:  
        sbic 	Eecr,	Eewe			; EeProm Ready?
        rjmp 	EEWrit

        out 	Eear, 	Null			; Store Temp in EEProm
        out 	Eedr, 	Temp

        sbi 	Eecr, 	Eemwe
        sbi 	Eecr, 	Eewe
  
      	inc 	Null				; Increment EEProm Index Register 
        ret



