;Random CES @ 9.6 Mhz
         
.include		"tn13def.inc"

;-----
.equ	Calib		= $63			; RC Calibration Byte

;-----	
.def	Null		= r8			; Zero Constant

.def	Reg1		= r9			; Random Registers
.def	Reg2		= r10			
.def	Reg3		= r11

.def	HoldLo		= r12			; Temporary Holding Registers
.def	HoldHi		= r13	

.def	BuzDly		= r14			; Buzzer + Led Duration Count
	
.def	Index		= r15			; EEprom Index Register	
    
.def	Temp		= r16			; General Purpose Work Register

.def	PulPos		= r17			; Pulse Position

.def	BuzCnt		= r18			; Buzzer + Led Frequency

.def	PulsLo		= r24			; Pulse Counter 
.def	PulsHi		= r25	

.def	SigLo		= r26			; Signal Counter
.def	SigHi		= r27	

.def	TimLo		= r28			; Time Duration Counter
.def	TimHi		= r29	

.def	DebLo		= r30			; Debounce Delay Counter
.def	DebHi		= r31	

;-----         
.Cseg
         
.Org	$0000	

;-----
Start:		
	rjmp	Reset				; Goto Reset

	nop
	
	rjmp	Inter				; External Interrupt0

;-----	
Timer0:					  
	sbiw	PulsLo,1			; Decrement Pulse Counter
	brne	T0 

	movw	PulsHi:PulsLo,HoldHi:HoldLo	; Reload Pulse Registers
	
	inc	PulPos				; Increment Pulse Position Counter

	andi	PulPos,$07			; Mask Off Bits 0 ... 2

	cpi	PulPos,$01			; PulPos = $01 ?
	brne	pc+4

	cbi	PortB,	Portb3			; Pb3 = On: Pb4 = Off
	sbi	PortB,	Portb4
	rjmp	T0	

	cpi	PulPos,$05			; PulPos = $05 ?	; 
	brne	pc+4

	sbi	PortB,	Portb3			; Pb3 = Off: Pb4 = On	
	cbi	PortB,	Portb4			
	rjmp	T0	

	sbi	PortB,	Portb3			; Pb3 = Off: Pb4 = Off
	sbi	PortB,	Portb4	

T0:	sbiw	SigLo,	1			; Decrement Signal Counter
	brne	T1

	sbrc	TimLo,	0			; If Odd Time then Skip

	rcall	Freq				; Else Get new Frequency
	
	sbiw	TimLo,	1			; If Session Duration expired?
	brne	T1

	set					; Then send Signal to Main Routine 
	
	reti

T1:	cpi	SigLo,	$00			; If 10 MilliSeconds elapsed?
	brne	pc+4
	cpi	SigHi,	$fe			; Else Skip
	brne	pc+2				

	out	Ocr0A,	Null			; Then Buzzer + Led Off

	reti

;-----
Inter:
	sbis	PinB,	 Pinb1			; Wait for Pb1 to return High
	rjmp	pc-1

	clr	DebHi				; 35 Milli Second Debounce Counter
	clr	DebLo	
			
	nop					; Decrement Debounce.Loop if Not Zero
	sbiw	DebLo,	1			
	brne	pc-2	
	
	set					; Sleep Signal to Main Routine
 
	reti

;-----         
Reset:	
	ldi	Temp,	RamEnd			; Set Stack Pointer
     	out	Spl,	Temp	
		    						
	ldi	Temp,	$19			; Pb0,Pb3,Pb4 as OutPuts
	out	DdrB,	Temp

	ldi	Temp,	$1b			; Set (Pb0,Pb3,Pb4) High + Pb1 Pullup 
	out	PortB,	Temp			

	sbi	AcsR,	Acd			; Disable Comparator 

	ldi	Temp,	$c3			; Fast PWM + Invering Mode 
	out	TccR0A,Temp	

	ldi	Temp,	$01			; No Prescaler 
	out	TccR0b,Temp	

	rcall	ReadEE				; Load Working State from EEProm

	ldi	TimLo,	Calib			; Calibrate Internal Oscillator
	out	OscCal,TimLo

	ldi	TimHi,	$04			; Set Time Counter to 30 Minutes ...
	eor	TimLo,	Reg2	
	andi	TimLo,	$0f			; ... + or - 1%	

	clt					; Clear T Bit

	ldi	Temp,	$02			; Enable Timer0 Interrupt 
	out	Timsk0,Temp	

	ldi	Temp,	$40			; Enable External Interrupt
	out	GimSk,	Temp	

	rcall	Howdy				; Hello World

	clr	Null				; Zero Constant

	sei					; Enable Global Interrupts

	ldi 	Temp, 	$20			; Idle + Low Level Interrupt on Pb1						
        out 	McucR, Temp 			

;-----	
Main:    
        sleep 					; Sleep + wait for Interrupts

        brtc	Main				; It T Flag = 0 then Loop

;-----
Ende:  
        rcall RiteEE				; Store Working State in EEProm

	rcall	Adieu				; Say Bye Bye

	ldi	Temp,	$1b			; Set Pb0,Pb3,Pb4 High + Pb1 Pullup 
	out	PortB,	Temp		

	ldi 	Temp, 	$30			; Enable Power Down Mode
       out 	McuCr, Temp 

       sleep					; Sleep + wait for External Interrupt

	rjmp	Start				; Do Again

;-----
Rand:
	mov 	Temp,	Reg1			; 23-Bit Random Number Generator
 	swap 	Temp
       sbrc 	Reg3, 	6 
       com 	Temp 
       lsr 	Temp
       rol 	Reg1
       rol 	Reg2
       rol 	Reg3 

	andi	Temp,	$07			; Mask Off Bits 0 ... 2

	ret

;-----
Freq:
	ser	Temp				; Buzzer + Led On
	out	Ocr0A,	Temp			
			
	rcall	Rand				; Get Random Number

	cpi	Temp,	$04			; = .5 Hz ?
	brcs	pc+4

	ldi	PulsHi,$24			; Pulse Count = 0.5 Hz:Weight of 4
	ldi	PulsLo,$9f
	rjmp	F0

	cpi	Temp,	$03			; = 100 Hz ?
	brne	pc+4

	ldi	PulsHi,$00			; Pulse Count = 100 Hz:Weight of 1 
	ldi	PulsLo,$2f
	rjmp	F0

	cpi	Temp,	$02			; = 10 Hz ?  
	brne	pc+4

	ldi	PulsHi,$01			; Pulse Count = 10 Hz:Weight of 1
	ldi	PulsLo,$d5
	rjmp	F0

	ldi	PulsHi,$0c			; Pulse Count = 1.5 Hz:Weight of 2 
	ldi	PulsLo,$35

F0:	movw	HoldHi:HoldLo,PulsHi:PulsLo	; BackUp Pulse Counter in Hold Register

	ret

;-----
ReadEE:
	clr	Index				; Clear EEProm Index Register 

        rcall EERead 			; Load 1st Random Register from EEProm
        mov 	Reg1,	Temp

        rcall EERead   			; Load 2nd Random Register from EEProm      
        mov 	Reg2, 	Temp

        rcall EERead				; Load 3rd Random Register from EEProm
        mov 	Reg3, 	Temp

	rcall 	EERead				; Load PulsHi Register from EEProm
       mov 	PulsHi,Temp

	rcall 	EERead				; Load PulsLo Register from EEProm
       mov 	PulsLo,Temp

	rcall 	EERead				; Load Pulse Position Register from EEProm
       mov 	PulPos,Temp
	
	ret
        
;-----
EERead:  
       sbic 	Eecr, 	Eewe			; EeProm Ready?
       rjmp 	EERead

       out 	Eear,  Index			; Load Temp from EEProm
       sbi 	Eecr,  Eere
       in 	Temp,  Eedr

	inc 	Index				; Increment EEProm Index Register 
       ret

;-----
RiteEE: 
	clr	Index				; Clear EEProm Index Register 
   
       mov 	Temp, 	Reg1			; Store 1st Random Register in EEProm  
       rcall 	EEWrit

       mov 	Temp, 	Reg2			; Store 2nd Random Register in EEProm  
       rcall 	EEWrit

       mov 	Temp, 	Reg3			; Store 3rd Random Register in EEProm  
	rcall 	EEWrit

	mov 	Temp, 	PulsHi			; Store PulsHi Register in EEProm  
	rcall 	EEWrit

	mov 	Temp, 	PulsLo			; Store PulsLo Register in EEProm
	rcall 	EEWrit

	mov 	Temp, 	PulPos			; Store Pulse Position Register in EEProm

;-----
EEWrit:  
       sbic 	Eecr,	Eewe			; EeProm Ready?
       rjmp 	EEWrit

       out 	Eear, 	Index			; Store Temp in EEProm
       out 	Eedr, 	Temp

       sbi 	Eecr, 	Eemwe
       sbi 	Eecr, 	Eewe
  
       inc 	Index				; Increment EEProm Index Register 
        
	ret

;-----
Adieu:	
	rcall	Howdy				; Fare Thee well !	

;-----
Howdy:
	ldi	BuzCnt,50			; Set 1st Tone
	rcall	BuzLed

	ldi	BuzCnt,100			; Set 2nd Tone
	rcall	BuzLed

	ldi	BuzCnt,150			; Set 3rd Tone
	rcall	BuzLed

	ldi	BuzCnt,200			; Set 4th Tone

;-----	
BuzLed:				
	out	Ocr0A,	BuzCnt			; Buzzer Led On

	dec	BuzDly				; Decrement Frequency Count and Loop
	brne	pc-1			

	out	Ocr0A,	Null			; Buzzer Led Off

	dec	BuzDly				; Decrement Frequency Count and Loop
	brne	pc-1

	dec	BuzCnt				; Decrement Duration Count and Loop
	brne	BuzLed

	ret

	

