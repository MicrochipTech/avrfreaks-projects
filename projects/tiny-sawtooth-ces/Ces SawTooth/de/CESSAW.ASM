;Psu @ 9.6 Mhz
         
.include		"tn13def.inc"

;----
.def	Null		= r11			; Zero Constant Register

.def	Reg1		= r12			; 32-Bit Random Registers
.def	Reg2		= r13			
.def	Reg3		= r14
.def	Reg4		= r15			
    
.def	Temp		= r16			; General Purpose Work Register

.def	PwmA		= r17			; Pulse Width Modulation Registers		
.def	PwmB		= r18	

.def	PwmOut		= r19			; Pulse Width Modulation Output	

.def	BuzCnt		= r20			; Buzzer Led Period Timer Register

.def	Hold		= r21			; RNG Holding Register

.def	Rand		= r22			; Random Number Constant Register	

.def	TimLo		= r24			; Session Timer Registers
.def	TimHi		= r25


;------         
.Cseg
         
.Org	$0000

;------ 
Start:
	
	sbi	AcsR,	Acd			; Disable Comparator
	
	ser	Temp				; All OutPuts High
	out	DdrB,	Temp		

	ldi	Temp,	$a3			; PwmA; Non Invering Mode 
	out	TccR0A,	Temp  

	rjmp	Reset	

.Org	$0006					; Timer/Counter0 Compare Match A 

	rjmp	PwmA
	
.Org	$0007					; Timer/Counter0 Compare Match B 	

;----
PwmB:	
	out	Ocr0B,	PwmB			; PwmB Duty Cycle @ 37k5 Hz
	
	inc	PwmB				; Step thru SawTooth B

	brne	A1				; 256 Iterations Completed ?

	cbi	PortB,	PortB2			; Yes then Buzzer Led Off @ 146.5 Hz

	dec	BuzCnt				; Decrement Led Buzzer Counter
	brne	A1

	sbiw	TimLo,	1			; Every 1.5 Seconds
	brne	pc+3

	set					; T Flag = 1
	rjmp	A1

	sbrs	TimLo,	0			; If Even goto Reti
	rjmp	A1	

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
			
	mov	PwmOut,	Reg2			; Fetch Reg2 into PwmOut
       
	andi	PwmOut,	$1f			; Mask Off Lower 5 Bits

	cpi	PwmOut,	$17			; Bracket Test
	brcs	pc+2

	andi	PwmOut,	$0f			; Mask Off Lower 4 Bits

	subi	PwmOut,	-$e9			; Adi 233
					
	sbi	PortB,	PortB2			; Buzzer Led On		

A1:	reti

;----
PwmA:
	out	Ocr0A,	PwmA			; PwmA Duty Cycle 

	inc	PwmA 				; Step thru SawTooth A
	
	cp	PwmA,	PwmOut			; = Random Value ?
	brne	pc+2

	clr	PwmA				; Yes then clear Register

	reti

;----
Reset:	
	ldi	Temp,	$01			; Prescaler = 1		 
	out	TccR0B,	Temp	
	
	ldi	Temp,	$0c			; Enable Both PWM Interrupts		 
	out	Timsk0,	Temp 

	ldi	TimHi,	$04			; Load and Randomise Session Duration
	mov	TimLo,	Reg2
	andi	TimLo,	$0f

	rcall	BootUp				; Hello World

	ldi	Rand,	$f8			; RNG Constant Register = 1111 1000

	sei					; Enable Global Interrupts

	ldi 	Temp, 	$20			; Idle Mode					
        out 	McucR, 	Temp 			

;-----	
Main:    
        sleep 					; Sleep + wait for Interrupts

        brtc	Main				; If T Flag = 0 then Main

;-----
Ende:  
        rcall 	Store				; Store Working State in EEProm

	out	PortB,	Null			; All Outputs Low		

	ldi 	Temp, 	$30			; Enable Power Down Mode
        out 	McuCr, 	Temp 

        sleep					; Deep Deep Sleep 

	rjmp	Start				; Do Again

;-----
Store: 
        mov 	Temp, 	Reg1			; Store 1st Random Register in EEProm  
        rcall 	EEWrit

       	mov 	Temp, 	Reg2			; Store 2nd Random Register in EEProm  
       	rcall 	EEWrit

        mov 	Temp, 	Reg3			; Store 3rd Random Register in EEProm  
	rcall 	EEWrit

	mov 	Temp, 	Reg4			; Store 4th Random Register in EEProm  
	rcall 	EEWrit

	mov 	Temp, 	Hold			; Store Hold Register in EEProm  
	rcall 	EEWrit

	rjmp	BuzLed

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
BootUp:
        rcall 	EERead 				; Load 1st Random Register from EEProm
        mov 	Reg1,	Temp

        rcall 	EERead   			; Load 2nd Random Register from EEProm      
        mov 	Reg2, 	Temp

        rcall 	EERead				; Load 3rd Random Register from EEProm
        mov 	Reg3, 	Temp

	rcall 	EERead				; Load 4th Random Register from EEProm
        mov 	Reg4, 	Temp

	rcall 	EERead				; Load Hold Register from EEProm
        mov 	Hold, 	Temp

;-----
BuzLed:
	ldi	BuzCnt,	78			; Set 1st Tone
	rcall	BuzOut

	ldi	BuzCnt,	138			; Set 2nd Tone
	rcall	BuzOut

	ldi	BuzCnt,	198			; Set 3rd Tone
	rcall	BuzOut
	
;-----	
BuzOut:
	sbi	PinB,	PinB2			; Toggle Buzzer+Led On

	inc	Null				; Increment and Loop
	brne	pc-1

	dec	BuzCnt				; Deccrement and Loop
	brne	BuzOut

	ret	


	
