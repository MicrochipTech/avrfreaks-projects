; MIDIfoot.asm   Control a MIDI foot switch controller with Atmel AVR 90S2313
; version 1.6  June 25, 2005     Alan Probandt     Portland, Oregon USA
; assembler command line: D:\AVR\gavrasm.exe -s D:\AVR\0_source\Midifoot\Midifoot.asm
;
;  This circuit runs a foot switch board that has 12 momentary normal-open push switches.
;  These switches are labeled 0 - 9, 10's Up, and 10's Down.
;  There is a set of six DIP switches also.
;  DIP switches 1-4 set the MIDI channel.
;  DIP 5 selects between sending Program Change only or PrgChng and Song Position (beginning).
;  DIP 6 selects between displaying 1-128 or  0-127 for the program number on the LEDs.

; The switches are organized into banks of ten.  There are twelve banks that correspond to 
;  MIDI program numbers 1 to 128.
; When pedal0 - pedal9 is pressed, change the MIDI program number in the current bank.
; No MIDI program change message when 10sUp or 10sDown is pressed.   Put a dash in 
;  the 10's digit to let user know pedal was pressed.  When pedal0 - pedal9 is pressed,
;  change the bank and send the new MIDI program change message.
;  Selecting Program 0 sends the MIDI start command (0xFA), prg 129 sends MIDI stop (0xFC).

;  three functions:  multiplex 7 segment LEDs,  read three sets of switches, and output MIDI commands.
; The switches are read by a pair of 74HC147 priority encoders (reverse logic output).
;  	U2  74hc147  	    U3  74hc147	
;  out 	0 1 2           out 0 1 2
;	-------------------------------- 
;  	0 1 1 SW5           0 1 1 Pedal 3
;  	1 0 1 10's Up       1 0 1 Pedal 4
;  	0 0 1 10's Down     0 0 1 Pedal 5
;  	1 1 0 Pedal 0       1 1 0 Pedal 6
;  	0 1 0 Pedal 1       0 1 0 Pedal 7
;  	1 0 0 Pedal 2       1 0 0 Pedal 8
;                           0 0 0 Pedal 9
;
;
;            Atmel AVR 90s2313
;	 RESET\	|1          20| VCC +5V
;     1's digit	|2 pd0  pb7 19| seg 100 - DIP sw4
;      MIDI out |3 pd1  pb6 18| seg g - DIP sw3
;   	 8MHz   |4 xtl1 pb5 17| seg f - DIP sw2
;   	 8MHz   |5 xtl2 pb4 16| seg e - DIP sw1
; 	out0 U3 |6 pd2  pb3 15| seg d - DIP sw6
; 	out1 U3 |7 pd3  pb2 14| seg c - out2 U2
; 	out2 U3 |8 pd4  pb1 13| seg b - out1 U2
;		|9 pd5  pb0 12| seg a - out0 U2
;    	    GND |10     pd6 11| 10's digit


.nolist
.include "2313def.inc"
.list

.equ	clock	= 8000000
.equ	MIDIbaud = 31250
.equ	MIDIUBRR = ((clock/MIDIbaud)/16)-1
.equ	LED_TCNT_high   = 0xff; 16-bit timer with large prescale
.equ	LED_TCNT_low   = 200; 16-bit timer with large prescale
.equ	LEDport = portb
.equ	LEDselectPort = portd
.equ	LEDselect1pin = 0  ; portd0
.equ	LEDselect10pin = 6 ; portd6
.equ	LED100pin = 7 ;portb7
.equ	SW6pin	= 3 ; port b
.equ	SW5	= 4 ; bit in Switches register showing raw state of Switch 5 (PrgmChange vs. PrgmChng & SongPtr)
.equ	U3ddr   = ddrd	
.equ	U3Pin  = pind 
.equ	U2ddr   = ddrb  
.equ	U2Pin  = pinb

 

;************************
;  Register definitions
;************************
.def	Counter		= r1
.def	SevenSegValue	= r2
.def	MIDIchannel 	= r3
.def	PowerOn		= r4


; high registers
.def	temp		= r16
.def	irqtemp		= r17
.def	ProgramValue	= r18  ;Always 1-128 in source code. "SendMIDIprg" decrements
;	PV for MIDI data format {0x00 - 0x7f}. 
.def	DIPswitches 	= r19 ; raw negative logic when switches are read
.equ	PrgChngMode	= 4 ; switch 5 gnd = on>>Send song select and program change
.equ	Mode128		= 5 ; switch 6 gnd = on>>Use pgm 1-128, else use prgm 0-127
.def	Flags		= r20
.equ	UpdateSwitches	= 0
.equ	LEDselect	= 1
.equ	Digit100	= 2
.equ	SwitchDown	= 3
.equ	PrgChngModeFlag	= 4 ;  1=Send song select and program change  0=send program change msg only
.equ	Mode128Flag	= 5 ;  1=use pgm 1-128, 0=use prgm 0-127

.def	OnesValue	= r21
.def	TensValue	= r22
.def	HundsValue	= r23
.def	BankValue	= r24
.def	PedalState	= r25
.def	StorePortB	= r26



.dseg
.org	0x60
RAM100:
.byte	1
RAM10:
.byte	1
RAM1:
.byte	1
StorePortD:
.byte	1

.cseg
    
;***************************************************************
; Interrupt Vector table -  Code Starts Here on Reset/Power On
;***************************************************************
.org 0
    	rjmp	reset
.org INT0addr
	reti		; External interrupt 0 handler  $001 
.org INT1addr
	reti		; External interrupt 1 handler  $002
.org ICP1addr
	reti		; Timer1 capture event handler  $003
.org OC1addr
	reti		; Timer1 compare match     $004
.org OVF1addr
	rjmp TIM1_OVF	; Timer1 overflow handler  $005
.org OVF0addr
	reti		; Timer0 overflow handler  $006
.org URXCaddr
	reti		; UART Rx Complete $007
.org UDREaddr
	reti		; UART Data Register empty $008
.org UTXCaddr
	reti		; UART Tx Complete $009
.org ACIaddr
	reti		; Analog comparator handler $00a


;**************************************************
;  Code
;**************************************************

reset:
;****  initialize stack pointer before anything else
	ldi	temp, RAMEND 
	out	spl, temp
                              
;**** initialize I/O ports
	ldi	temp, 0b01000001 ; read U3 on 2,3,4  out 1's digit on 0; UART TxD on 1; out 10's digit on 6
	out	U3ddr, temp ; ddrd
		                            
;**** initialize timers
	ldi	temp, (1 << TOIE1) ; set TOIE1 - LED multiplex
	out	TIMSK,temp 	; TOIE enable interrupt on  overflow
	out	TIFR,temp	;  to clear T/C1 overflow flag

; --initialize timer1  interrupt to run continously at 1/60th second
	ldi	Temp, LED_TCNT_High ; load timer1 count register for 1/60 second
	out	TCNT1H, Temp
	ldi	Temp,LED_TCNT_Low ; load timer1 count register for 1/60 second
	out	TCNT1L, Temp
	ldi	Temp, ( (1<<CS12) | (0<<CS11) | (1<<CS10) ); start timer 1 at preset sysclk/1024 7812overflows/sec
	out	TCCR1B, Temp
	
;---initialize UART
	ldi	temp,MIDIUBRR ; 15; ((clock/31250)/16)-1 MIDI baud of 31250
	out	UBRR,temp
	sbr	temp, (1<<TXEN) ; UART transmit enabled
	out	UCR,temp	; setup UART control register
	
;---initialize global interrupt enable
	sei

;---initialize watchdog
	ldi	temp, (1<<WDE) | (1<<WDP2) | (0<<WDP1) | (0<<WDP0) ; 256ms
	out	WDTCR, temp

;**** initialize application
; --- PowerOn register will be 0xAA if reset was caused by Watchdog timeout.
; --- If WD reset, don't initialize all the variables or change the current program #. 
	mov	temp, PowerOn
	cpi	temp, 0xaa
	breq	WDtimeOut
	ldi	temp, 0xaa
	mov	PowerOn, temp
	clr	OnesValue
	clr	TensValue
	clr	HundsValue
	clr	BankValue
	clr	ProgramValue
	clr	temp
	ldi	temp, 0x01 ; AVR ProgramValue is 01, but MIDI program change is 00
	mov	SevenSegValue, temp
	mov	ProgramValue, temp
	clr	Flags
	clr	DIPswitches
	ldi	temp, 10
	mov	Counter, temp  ; test switches every 1/6 of a second; or after ten LED mux IRQs
	
WDTimeOut: ; come here if wiring causes the watchdog to trigger.  Reset with changing variables

;--   read initial DIP switch state --  up position= open; reads logic high- switches read reverse logic 1111= channel 0

	clr	temp; all 8 pins on portb need to be read at program initialization
	out	DDRB, temp  ;  U2out on pb2-0, DIP6 on pb3, DIP 4-1 on pb7-4
	ser	temp
	out	PortB, temp;  turn on all port b pull-up resistors
	nop	; wait one cycle for instruction to go through pipeline before reading pin state
	in	PedalState, U2pin  ; read  U2 (switch 5 in U2 matrix) and switch 6 (bit 0)
	clr	temp
	out	portb, temp ; don't light display when setting portb as outputs
	ser	temp
	out	U2ddr, temp ; set port b to output LED display
	
; test Mode128 setting.  DIP#6 is on (grounded) by default. MIDI program LED display is 01-128.
; Mode128 setting is handled in the ProgramValue register.
	bst	PedalState, SW6pin  ; put switch6 into T flag  [Bit Store to T]
	cbr	DIPswitches, (1<<Mode128)
	sbr	Flags, (1<<Mode128Flag); bit 5
	brtc	TstSw10	; if T is clear, jump.  Switch 6  is active (connected to ground) -use prgm 1-128
	sbr	DIPswitches, (1<<Mode128); reverse settings if Switch 6  is off (pulled high)
	cbr	Flags, (1<<Mode128Flag)
	
TstSw10:
	mov	temp, PedalState
	andi	temp, 0b00000111  ; check status of U2 output for state of switch5
	cpi	temp, 0b00000110  ; switch 5 is on (U2 A0 is ground) if equal
	brne	TstSw30  ; branch if switch is off
	cbr	DIPswitches, (1<<SW5)
	sbr	Flags, (1<<PrgChngModeFlag); sw5 down= flag on. Output Song Select and Pgm.
	rjmp	TstSw40
TstSw30:
	sbr	DIPswitches,(1<< PrgChngMode); register holds raw logic from DIP switches
	cbr	Flags, (1<<PrgChngModeFlag)
	
TstSw40: ; get MIDI channel
	swap	PedalState
	andi	PedalState, 0x0f
	or	DIPswitches, PedalState  ; 00001111 is MIDI channel 0   - reverse logic
	com	PedalState
	andi	PedalState, 0x0f
	mov	MIDIchannel, PedalState
; MIDIchannel register initialized;  PrgChng Mode initialized, 127/128 mode initialized


;************************************************
;%%%%%%%%%%%%%%%%%%%%%%%%
;  Main Program
;%%%%%%%%%%%%%%%%%%%%%%%%
;************************************************

main:
	sbrs	Flags, UpdateSwitches
	rjmp	main

	cbr	Flags, (1<<UpdateSwitches)
	ldi	temp,10
	mov	Counter, temp
	rcall	TestPedals

	rjmp	main

;*********************
; test pedal switches
;*********************
TestPedals:
	cli
	in	temp, portd
	sts	StorePortD, temp
	in	StorePortB, portb
	
	ldi	temp,0b11111000
	out	ddrb, temp
	ldi	temp, 0x07
	out	portb, temp; turn on pull-up resistors
	nop
	sbrc	Flags, SwitchDown
	rjmp	WaitSwitchRelease

; test for any switch pressed on U2 ; sw5, 10up, 10dn, p0, p1, p2  U2 
	in	PedalState, pinb
	nop
	andi	PedalState, 0b00000111
	cpi	PedalState, 0b00000111 ; no switch pressed on U2
	breq	CheckU3 ; no U2 switch press, check on U3
	
  	sbr	Flags, (1<<SwitchDown)  ; set flag that says wait until key is released
; Update Pedal status for new U2 value
	cpi	PedalState, 0b00000101 ; check for 10s Up key
	breq	TP_incBankValue 
	cpi	PedalState, 0b00000100 ; check for 10s Down key
	brne	TP5
	rjmp	TP_decBankValue
	
TP5:	com	PedalState ; isolate the pedal number
	andi	PedalState, 0b00000011
	mov	onesValue, PedalState
	
	rcall	DigitsToProgramValue
	rcall	DigitsTo7SegValue
	cpi	ProgramValue, 129
	breq	TP_Stop
	cpi	ProgramValue, 0
	breq	TP_Start	
	rcall	SendMIDIprg

;--------------------------------------
TP_exit: 
	lds	temp, StorePortD
	out	portd, temp
	ser	temp
	out	ddrb, temp
	out	portb, StorePortB
	sei
	ret
;--------------------------------------

TP_Stop:
	ldi	temp, 0x5c ; load 'SP' into SevenSeg for MIDI stop cmd (#129)
	mov	SevenSegValue, temp
	cbr	Flags, (1<<Digit100) ; display 'SP' instead of '1SP'
	ldi	temp, 0xfc ; MIDI system common message for Stop
	out	UDR, temp
TP_Stop0:
	sbis	USR, UDRE 
	rjmp	TP_Stop0
	rjmp	TP_exit

TP_Start:
	ldi	temp, 0x5b ; load 'Sr' into SevenSeg for MIDI start cmd (#00)
	mov	SevenSegValue, temp
	ldi	temp, 0xfa ; MIDI system common message for Start
	out	UDR, temp
TP_Start0:
	sbis	USR, UDRE 
	rjmp	TP_Start0
	rjmp	TP_exit

CheckU3:
	in	PedalState, U3pin
	lsr	PedalState  
	lsr	PedalState
	andi	PedalState, 0b00000111
	cpi	PedalState, 0b00000111 ; no switch pressed on U3
	breq	NoPedalPresses
; U3 is all pedal numbers in sequence - starting at 3
	sbr	Flags, (1<<SwitchDown)
	com	PedalState
	andi	PedalState, 0b00000111
	subi	PedalState, -2
	mov	OnesValue, PedalState
	rcall	DigitsToProgramValue
	rcall	DigitsTo7SegValue
	cpi	ProgramValue, 129
	breq	TP_Stop
	cpi	ProgramValue, 0
	breq	TP_Start	
	rcall	SendMIDIprg
	rjmp	TP_exit

NoPedalPresses:
	rjmp	TP_exit

; don't send any MIDI program change message when 10sUp or 10sDown is pressed.  
; Put a dash in the 10's digit to let user know pedal was pressed.
TP_incBankValue:
	inc	BankValue
	cpi	BankValue, 13
	brne	TPinc20
	clr	BankValue ; roll over to bank 0
	clr	TensValue
	clr	HundsValue
	rjmp	TPincExit
TPinc20:
	clr	HundsValue
	cpi	BankValue,10
	brlt	TPinc30
	ldi	HundsValue, 1
	mov	temp, BankValue ; don't change BankValue to get new TensValue
	subi	temp, 10
	mov	TensValue, temp
	rjmp	TPincExit
TPinc30: inc	TensValue
TPincExit:
	rcall	BankValueToDigits
	rcall	DigitsToProgramValue
	rcall	DigitsTo7SegValue
	mov	temp, SevenSegValue
	andi	temp, 0x0f  ; clear tens digit
	ori	temp, 0xa0 ; put dash into tens digit display
	mov	SevenSegValue, temp
	rjmp	TP_exit

;------------------------
TP_decBankValue:
	dec	BankValue ; original BankValue 12--00, new BankValue 11 or -1(255)
	tst	BankValue
	brmi	TPdec10 ; new BankValue = -1, change to 12
	rjmp	TPdec_exit

TPdec10:
	ldi	BankValue, 12
	ldi	TensValue, 2
	ldi	HundsValue, 1
TPdec_exit:
	rcall	BankValueToDigits
	rcall	DigitsToProgramValue
	rcall	DigitsTo7SegValue
	mov	temp, SevenSegValue
	andi	temp, 0x0f  ; clear tens digit
	ori	temp, 0xa0 ; put dash into tens digit display
	mov	SevenSegValue, temp
	rjmp	TP_exit

WaitSwitchRelease:
	in	PedalState, pinb
	nop
	andi	PedalState, 0b00000111; sw5, 10up, 10dn, P0, P1,  P2  on  U2
	cpi	PedalState, 0b00000111 ; no switch pressed on U2
	breq	WSR10
	rjmp	TP_exit
WSR10:	in	PedalState, U3pin
	nop
	lsr	PedalState
	lsr	PedalState
	andi	PedalState, 0b00000111; check for P3, P4, P5, P6, P7, P8, P9 on U3
	cpi	PedalState, 0b00000111 ; no switch pressed on U3
	breq	WSR20
	rjmp	TP_exit
WSR20:	cbr	Flags, (1<<SwitchDown)
	rjmp	TP_exit

;*************************************************************************
; end of TestPedals
;*************************************************************************


BankValueToDigits:
	cpi	BankValue, 10
	brge	BVD10
	clr	HundsValue
	cbr	Flags, (1<<Digit100)
	mov	TensValue, BankValue
	ret
BVD10:	ldi	HundsValue,1
	sbr	Flags, (1<<Digit100)
	mov	temp, BankValue
	subi	temp, 10
	mov	TensValue, temp
	ret

;***************************************************************
DigitsToProgramValue:
; three register variables: HundsValue, TensValue, OnesValue to binary value in register 'ProgramValue'
	sts	RAM100, HundsValue
	sts	RAM10, TensValue
	sts	RAM1, OnesValue
	clr	ProgramValue
	cbr	Flags, (1<<Digit100)
	tst	HundsValue
	breq	DPV10
	subi	ProgramValue, -100
	sbr	Flags, (1<<Digit100)
DPV10:	tst	TensValue
	breq	DPV20
	dec	TensValue
	subi	ProgramValue, -10
	rjmp	DPV10
DPV20:	tst	OnesValue
	breq	DPVexit
	dec	OnesValue
	subi	ProgramValue, -1
	rjmp	DPV20
DPVexit:
	lds	HundsValue, RAM100
	lds	TensValue, RAM10
	lds	OnesValue, RAM1
	ret

;********************************************************************************
; Mode128Adjust-   change digits if user wants program displayed as 00-127, instead of default 01-128.

Mode128Adjust: 
	mov	temp, ProgramValue
	dec	temp
	clr	HundsValue
	clr	TensValue
	clr	OnesValue
; convert adjusted copy of ProgramValue to digit values
MdAdj05: subi	temp, 100
	brcs	MdAdj10  ; value 100 or greater
	inc	HundsValue
	rjmp	MdAdj05
MdAdj10: subi	temp, -100 ; restore the value before the subtraction that caused carry flag to be set
MdAdj20: subi	temp, 10
	brcs	MdAdj30 
	inc	TensValue
	rjmp	MdAdj20
MdAdj30: subi	temp, -10 ; restore the value before the subtraction that caused carry flag to be set	
MdAdj40: subi	temp, 1
	brcs	MdAdj_exit
	inc	OnesValue
	rjmp	MdAdj40
MdAdj_exit: 
	subi	temp, -1 ; restore the value before the subtraction that caused carry flag to be set
	ret

;***************************************************************************
DigitsTo7SegValue:  ; enter with ProgramValue register formatted into the three Digit registers.
; test the Mode128Flag.  Set (default) = display Programs 01-128.  Clear = display 00-127
; If Mode128 is clear, then convert digits to binary, decrement bin value,  convert back to digits, and put digits into 7seg.
	sbrs	Flags, Mode128Flag
	rcall	Mode128adjust
	cbr	Flags, (1<<Digit100)
	tst	HundsValue
	breq	D7SV10
	sbr	Flags, (1<<Digit100)
D7SV10:	mov	SevenSegValue, TensValue
	swap	SevenSegValue
	or	SevenSegValue, OnesValue
	ret


;******************************************
;   MIDI routines
;******************************************
SendMIDIprg:
	mov	temp, MIDIchannel
	ori	temp, 0xc0
	out	UDR, temp
SMidi0:	sbis	USR, UDRE
	rjmp	SMidi0

	mov	temp, ProgramValue; PV register variable is always 128 - 01
	dec	temp
	out	UDR, temp
SMidi1:	sbis	USR, UDRE
	rjmp	SMidi1
	sbrs	Flags, PrgChngModeFlag
	ret
	; send MIDI system common song_select and song number
	
	ldi	temp, 0xf3
	out	UDR, temp
SMidi2:	sbis	USR, UDRE
	rjmp	SMidi2
	mov	temp, ProgramValue
	dec	temp
	out	UDR, temp
SMidi3:	sbis	USR, UDRE
	rjmp	SMidi3	
	ret


;*******************************************
;  LED display   Timer interrupt routine   
;  puts BCD digits in SevenSegValue onto LEDs
;*******************************************
TIM1_OVF:
	push	irqtemp
	wdr		; reset watchdog 
	
	ldi	irqtemp, LED_TCNT_high
	out	TCNT1H, irqtemp
	ldi	irqtemp,LED_TCNT_low
	out	TCNT1L, irqtemp	; 1/60th Second period  - 8MHz clock/1024 preset
	ser	irqtemp
	out	ddrb, irqtemp
	
	dec	Counter
	brne	T1_LED
	sbr	Flags, (1<<UpdateSwitches)
	
	
T1_LED:	sbrs	Flags, LEDselect ; if entered this IRQ with LEDselect hi ( MSD lighted), then skip to doLSD
	rjmp	doMSD ; if entered this IRQ with LEDselect low ( LSD lighted), then don't skip this opcode
	cbr	Flags, (1<<LEDselect)
	ldi	ZH, high (SevenSegTable*2)  ; load 16-bit Z register with the byte address of the LED pattern table
	ldi	ZL, low  (SevenSegTable*2)
	mov	irqtemp, SevenSegValue	  ;SevenSegValue is the hex digit pair to be displayed
	andi	irqtemp, 0x0f	; mask out the most sig digit
	add	ZL, irqtemp	; add digit value offset to Z low
	brcc	LED_0		; add one to Z high if result crosses a 256 byte boundary
	inc	ZH
LED_0:	lpm			; move LED 7-seg digit pattern for the digit to R0 from program memory table
	mov	irqtemp, r0	;   bits 76543210
	sbrc	Flags, Digit100 ; if Digit100 flag is set, then light the 100's digit
	sbr	irqtemp,(1 << LED100pin)
	nop
	out	LEDport, irqtemp
	cbi	LEDselectport, LEDselect10pin ; left (MSB) LED digit switched off 
   	sbi	LEDselectport, LEDselect1pin  ; right (MSB) LED digit gets lighted
	rjmp	LEDexit

doMSD:	;RightLEDdigit 10's put new value on most-sig digit
	sbr	Flags, (1<<LEDselect)
	ldi	ZH, high (SevenSegTable*2)   ; reload 16-bit Z register with the byte address of the LED pattern table
	ldi	ZL, low  (SevenSegTable*2)
	mov	irqtemp, SevenSegValue	;SevenSegValue is the hex digit pair to be displayed
	andi	irqtemp, 0xf0	; mask out the least sig digit
	swap	irqtemp     	; move most sig digit to bits 0-3
	add	ZL, irqtemp	; add digit value offset to Z low
	brcc	LED_1		; add one to Z high if result crosses a 256 byte boundary
	inc	ZH
LED_1:	lpm			; move LED 7-seg digit pattern for the digit to R0 from program memory table
	mov	irqtemp, r0	;  bits 76543210 
	sbrc	Flags, Digit100 ; if Digit100 flag is set, then light the 100's digit
	sbr	irqtemp,(1 << LED100pin)
	nop	 
	out	LEDport, irqtemp ;	Igfedcba
	sbi	LEDselectport, LEDselect10pin ; left (MSB) LED digit gets lighted
	cbi	LEDselectport, LEDselect1pin  ; right (MSB) LED digit switched off

LEDexit: pop	irqtemp
	reti


;; display on a dual LED 7-segment display - Common Cathode - segments wired in parallel
;;  - bit 7 100's - PD0 lights LeastSigDigit - PD6 lights MostSigDigit
SevenSegTable:		  
.db   0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x67,0x40,0x50,0x73
     
;AT2313 Igfe dcba  display board  1=segment on
;  bits 7654 3210  portB
;   0   0011 1111  0x3f
;   1   0000 0110  0x06
;   2   0101 1011  0x5b
;   3   0100 1111  0x4f
;   4   0110 0110  0x66
;   5   0110 1101  0x6d
;   6   0111 1101  0x7d
;   7   0000 0111  0x07
;   8   0111 1111  0x7f
;   9   0110 0111  0x67
;   -   0100 0000  0x40   dash  aa
;   t   0101 0000  0x50   looks like r for 'Start' 5b
;   P   0111 0011  0x73   'Stop' 5c


; shows program information when the code is read back from the AVR chip.  ASCII pairs must be reversed.
;      ****  MIDIfoot.asm v1.6  June 25, 2005  Alan Probandt
.db   "****  IMIDoftoa.msv .1 6J nu e52 ,0250  lAnaP orabdn t"

