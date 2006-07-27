;Schumann @ 9.6 Mhz
         
.include		"tn13def.inc"

;-----	
.def	Reg1		= r8			; 32-Bit Random Registers
.def	Reg2		= r9			
.def	Reg3		= r10
.def	Reg4		= r11	

.def	HoldLo		= r12			; Temporary Holding Registers
.def	HoldHi		= r13	

.def	TimCnt		= r14			; Time Duration Counter

.def	Null		= r15			; Zero Constant
    
.def	Temp		= r16			; General Purpose Work Register

.def	BuzTim		= r17			; Buzzer + Led Period

.def	BuzCnt		= r18			; Buzzer + Led Frequency

.def	Hold		= r19			; RNG Holding Register

.def	Rand		= r20			; Random Number Constant Register	

.def	PulsLo		= r24			; Pulse Counter 
.def	PulsHi		= r25	

;-----         
.Cseg
         
.Org	$0000	

;-----
Start:	
	sbi	AcsR,	Acd			; Disable Comparator 

	ser	Temp				; Set to  1111 1111

	rjmp	Reset				; Goto Reset
	
;-----	
Timer0:
	sbiw	PulsLo,	1			; Dec Pulse Counter @ 4687.5 Hz
	brne	T1
				
	movw	PulsHi:PulsLo,HoldHi:HoldLo	; Reload Pulse Registers

	cbi	PortB,	PortB0			; Pb0 = 0; Buzzer Led Off

	sbi	PinB,	PinB1			; Flip Pb1 @ 15.65 Hz
	
	dec	BuzTim				; Ready to Buzz ?			
	brne	T1

	rcall	Update				; Reload every 7 Seconds

	dec	TimCnt  			; If Session Duration expired?
	brne	T1	
								  
	set					; Then Signal Main Routine 

T1:	reti	

;-----         
Reset:    								
	out	DdrB,	Temp			; All Pins as OutPuts
	out	PortB,	Temp

	ldi	Temp,	$02			; Divide by 8 Prescaler 
	out	TccR0b,	Temp	

	out	Timsk0,	Temp			; Enable Timer0 Interrupt
			
	rcall	BootUp				; Load Working State 

	ldi	Rand,	$f8			; RNG Constant Register = 1111 1000

	rcall	Update				; Initialise Constants

	sei					; Enable Global Interrupts

	ldi 	Temp, 	$20			; Enable Idle Mode						
        out 	McucR, 	Temp 	

;-----	
Main:    
        sleep 					; Idle + wait for Interrupts

        brtc	Main				; If T-Bit = 0 then Goto Main
	
;-----
Ende:  
	rcall 	Store				; Orderly Shut Down
		
	out	PortB,	Null			; Clear All OutPuts

	ldi 	Temp, 	$30			; Enable Power Down Mode
        out 	McuCr, 	Temp 

        sleep					; Go into a Deep,Deep Sleep 

	rjmp	Start				; Wake up + Start all over Again

;------
Update:
	sbi	PortB,	PortB0			; Buzzer Led On  

	ldi	BuzTim,	110			; Buzz Led 7 Second Period
	
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
			
	mov	PulsLo,	Reg2			; Fetch Reg2 into PulsLo

	ldi	PulsHi,	$01			; Reload and Randomise Pulse Counter

	cpi	PulsLo,	$0a			; If < than Bandwidth
	brcc	pc+3

	ldi	PulsLo,	$0a			; Then set Upper Limit
	rjmp	pc+4

	cpi	PulsLo,	$4f			; If > than Bandwidth
	brcs	pc+2

	ldi	PulsLo,	$4e			; Then set Lower Limit	
	
	movw	HoldHi:HoldLo,PulsHi:PulsLo	; BackUp Pulse Counter

	ret

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
	sbi	PinB,	PinB0			; Toggle Buzzer+Led On

	inc	Null				; Increment and Loop
	brne	pc-1

	dec	BuzCnt				; Decrement and Loop
	brne	BuzOut

	ret	


