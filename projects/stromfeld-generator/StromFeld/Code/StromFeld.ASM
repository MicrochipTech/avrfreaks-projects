;Push Button StromFeld Generator  @ 9.6 Mhz
         
.include		"tn13def.inc"

;-----	
.def	Null		= r9			; Zero Constant

.def	HoldLo		= r10			; Temporary Holding Registers
.def	HoldHi		= r11	

.def	Reg1		= r12			; Random Registers
.def	Reg2		= r13		
.def	Reg3		= r14			; Holds the Tap-20 Bit
.def	Reg4		= r15
.def	Reg5		= r16			; Holds the Tap-33 Bit

.def	PulPos		= r17			; Pulse Position

.def	BuzCnt		= r18			; Buzzer + Led Frequency

.def	Rand		= r19			; Random Number Constant Register	

.def	Temp		= r20			; General Purpose Work Register

.def	SessLo		= r24			; Session Time Counter 
.def	SessHi		= r25	

.def	PulsLo		= r26			; Pulse Period Counter 
.def	PulsHi		= r27	

.def	TimLo		= r28			; Time Duration Counter
.def	TimHi		= r29	

.def	PhaseA		= r30			; Phase Shift Registers 
.def	PhaseB		= r31	

;------         
.Cseg
      

;------
.Org	$0000

Start:	
		
	rjmp	Reset				; Goto Reset


	
;-----
.Org	$0001

Inter:
	set					; Sleep Signal to Main Routine 
	reti	
	
;-----	
.Org	$0003	

Timer0:
	sbiw	PulsLo,	1			; Decrement Pulse Counter @ 4688 Hz
	brne	T0

	lsr 	Reg5 				; Shift Tap 33 in RNG into Carry
			
	rol 	Reg1				; 33 Bit RNG 	
	rol 	Reg2				
       	rol 	Reg3
       	rol 	Reg4 
  	
	mov	Reg5,	Reg3			; Fetch Reg3 into Reg5

	andi	Reg5,	$08			; Mask Off Tap 20 in RNG

	adc	Reg5,	Rand			; Eor RNG Taps 20 and 33
	adc	Reg5,	Null

	andi	Reg5,	$01			; Carry Out = Tap 33 in Reg5

	movw	PulsHi:PulsLo,HoldHi:HoldLo	; Reload Pulse Registers

	inc	PulPos				; Increment Pulse Position Counter
	
	andi	PulPos,	$0f			; Mask Off Lower Nibble

	cp	PulPos,	PhaseA			; PulPos = PhaseA ?
	brne	pc+4

	cbi	PortB,	Portb3			; Pb3 = On: Pb4 = Off
	sbi	PortB,	Portb4
	rjmp	T0

	cp	PulPos,	PhaseB			; PulPos = PhaseB ?	; 
	brne	pc+4

	sbi	PortB,	Portb3			; Pb3 = Off: Pb4 = On	
	cbi	PortB,	Portb4			
	rjmp	T0

	sbi	PortB,	Portb3			; Pb3 = Off: Pb4 = Off
	sbi	PortB,	Portb4	

T0:	sbiw	TimLo,	1			; Decrement Time Counter @ 4688 Hz	
	brne	T1

	sbiw	SessLo,	1			; Is Session Duration expired?
	breq	Inter

	cbi	PortB,	PortB0			; On Buzzer Led every 3.5 Seconds

	ldi	TimHi,	$40			; Reload Time Counter
		
	mov	PulsHi,	Reg2			; Fetch Reg2 into PulsHi

	andi	PulsHi,	$03			; Mark Off Bandwidth
	
	mov	PulsLo,	Reg1			; Fetch Reg1 into PulsLo

	cpi	PulsLo,	$18			; Upper BandWith exceeded ?
	brcc	pc+2	
		
	ori	PulsLo,	$18			; Yes then Frequency = 12 Hz
	
	movw	HoldHi:HoldLo,PulsHi:PulsLo	; BackUp Pulse Counter in Hold Register

	mov	PhaseA,	Reg3			; Randomise PhaseA Pulse Position

	andi	PhaseA,	$0f			; Mask Off Lower Nibble

	mov	PhaseB,	Reg4			; Randomise PhaseB Pulse Position

	andi	PhaseB,	$0f			; Mask Off Lower Nibble

	rjmp	T2
	
T1:	cpi	TimHi,	$3f			; 80 Milli Seconds Elapsed ?
	brne	pc+4
	cpi	TimLo,	$00
	brne	pc+2
	
	sbi	PortB,	PortB0			; Off Buzzer Led 

T2:	reti

;-----         
Reset:	
	sbi	AcsR,	Acd			; Disable Comparator 

	ldi	Temp,	$19			; Pb0,Pb3,Pb4 as OutPuts	    						
	out	DdrB,	Temp

	ldi	Temp,	$1b			; Set (Pb0,Pb3,Pb4) High + Pb1 Pullup 
	out	PortB,	Temp	

	ldi	SessHi,	$02			; Session Counter = 30 Minutes ...

	mov	SessLo,	Reg1			; ... +/- 1%	
	andi	SessLo,	$07
				
	out	TccR0b,	SessHi	 		; 8 Prescaler 

	out	Timsk0,	SessHi			; Enable Timer0 Interrupt

	ldi	Temp,	$40			; Enable External Interrupt
	out	GimSk,	Temp	

	ldi	Rand,	$f8			; Random Number Constant = 1111 1000

	rcall	Bounce				; Howdy ?

	ldi 	Temp, 	$20			; Idle + Low Level Interrupt on Pb1						
        out 	McucR, 	Temp 			

;-----	
Main:  
	sleep 					; Take a little nap 

        brtc	Main				; It T Flag = 0 then Loop

;-----
Ende:  
	rcall	Bounce				; Debounce + Bye Bye 

	ldi	Temp,	$1b			; Set Pb0,Pb3,Pb4 High + Pb1 Pullup 
	out	PortB,	Temp	

	ldi 	Temp, 	$30			; Enable Power Down Mode
        out 	McuCr, 	Temp 

        sleep					; Deep,Deep Sleep 

	rcall	Bounce				; Debounce

	rjmp	Reset				; Do Again

;-----
Bounce:
	cli					; Disable Global Interrupts

	adiw	TimLo,	1			; Delay Debounce = 25 mSecs
	brne	pc-1

	sbis	PinB,	PinB1			; If still Lo go back ?
	rjmp	pc-3

	ldi	TimHi,	$40			; Timer Counter = 3.5 Secs

;-----
TonGen:
	ldi	BuzCnt,78			; Set 1st Tone
	rcall	BuzLed

	ldi	BuzCnt,138			; Set 2nd Tone
	rcall	BuzLed

	ldi	BuzCnt,198			; Set 3rd Tone
	rcall	BuzLed

;-----	
BuzLed:
	sbi	PinB,	PinB0			; Toggle Buzzer Led 

	inc	Null				; Increment Frequency Count and Loop
	brne	pc-1				

	dec	BuzCnt				; Decrement Duration Count and Loop
	brne	BuzLed

	clt					; Clear T Bit
	
	sei					; Enable Global Interrupts	

	ret	


	
