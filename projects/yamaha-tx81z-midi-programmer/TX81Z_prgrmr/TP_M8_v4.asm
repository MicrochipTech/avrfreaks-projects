;  TP_m8_v4.asm   March 11, 2009  version 4.6  build 2
;  alan_probandt@yahoo.com   Target CPU: Atmel AVR MEGA8
;
;  Real-Time programmer for the Yamaha TQ5 4-operator FM tone module audio music synthesizer.
;  Works also with the popular Yamaha TX81-Z 4-op FM tone module.
;  This version has only the most-often used TQ5/TX81Z parameters available for change in order to make the slider interface easier.
;  Operator Params are: Frequency Ratio, Attack Rate, Output Level, Oscillator Waveform,
;	Decay1Rate,Decay2Rate, Decay1Level, Algorythm, and Feedback
;  TQ5-specific parameters are: Effect Type, Effect Rate, Effect Balance
;
; ~~~ Weird behaviors and their workarounds ~~~~~
;=== Usually you have to press the reset button twice after power-up for the TQ5/TX81Z to respond correctly.
;
;--ACED and EFEDS always load zeros when changing the voice.
;  == This happens when using MIDIout2 (JDR board P5) instead of MIDIout1 (JDR board P3)
;   >> use MIDIin1 (P2) and MIDIout1 (P3) between the TopPot and the Yamaha TX81Z/TQ5



;  Program Operation Overview
;-----------------------------
;  The main code checks repeatedly for a keypress from the PS2 keyboard.
;
;  Every 1/15 of a second, the main code interrupts and does a scan of two potentiometers.
;  The new values for each pot are compared to the values from the last set of conversions.
;
;    16x2 LCD display     0123456789abcdef
;			--------------------
;    name, voice#, alg# | Voice_Name V#  A | First line: GetVCEDdata
;    param txt,new,org  | Parameter  n# o# | Second line: NewParameterSelected
;			--------------------
;
;    Frequency display    0123456789abcdef
;			--------------------
;   name, voice#, alg#  | Voice_Name V#  A | First line: GetVCEDdata
;   frq#, ratio  Cr Fn  | Frq_1 1.00 Cr Fn | Second line:
;			--------------------
;
; use Terminal or Lucinda font to format source code listing correctly.  Tab stop value = 8.
;
;	This application's pinout - Atmel AVR 8535   [8K flash/512 bytes SRAM/512 bytes EEPROM]
;
;
;        Atmel AVR Mega8
;            _______
;     reset/ |1  28| PC5 SCL
;   RxD  PD0 |2  27| PC4 SDA
;   TxD  PD1 |3  26| ADC3
;   PS2 clock|4  25| ADC2
;   PS2 data |5  24| ADC1
;        PD4 |6  23| ADC0
;   +5V  VCC |7  22| GND
;        GND |8  21| AREF  +5V
;	 PB6 |9  20| AVCC  +5V
;	 PB7 |10 19| PB5  LCD_d7 (SCK)
; 	 PD5 |11 18| PB4  LCD_d6 (MISO)
; RS out PD6 |12 17| PB3  LCD_D5 (MOSI)
; toggle PD7 |13 16| PB2  LCD_d4
; LCD_RS PB0 |14 15| PB1  LCD_E
;	     -------
;
;

.nolist
.include "M8def.inc"  ; Mega8 version
.list

; constants for specific AVR device type
.equ	SYSTEM_CLOCK	= 8000000
.equ	RAMSTART	= 0x60 ; for Mega8 and 90S8535
.equ	MIDIbaud	= ((SYSTEM_CLOCK/16) / 31250) - 1  ; 15  0.0% error for MIDI
.equ	UARTbaudrate	= 38400
.equ	T2prescaler	= 8
.equ	SoftBaudRate	= (system_clock/T2prescaler) / UARTbaudrate
.equ	UARTreload	= ((~ SoftBaudRate) + 2 );baud setting for timer
.equ	UARTDDR		= ddrd ; UARTOUTPORT, UARTOUTPIN
.equ	UARTOutPort	= portd
.equ	UARTOutPin	= 6
.equ	TogglePort	= portd
.equ	ToggleDDR	= ddrd
.equ	ToggleBit	= 7

; constants for Yamaha FM synthesizer
.equ	TX81Z_Store	= 0x41	; simulated front panel buttons  0x7f=pressed 00=released
.equ	TX81Z_DataPlus	= 0x48
.equ	TQ5_Store	= 91+30
.equ	TQ5_SecondPlus	= 91+15
.equ	TQ5_User	= 91+26
.equ	ButtonPressed	= 0x7f
.equ	ButtonReleased	= 0x00

;Definitions
;LCD equates
.equ	RS		= 0	 ; PB0 Register Select
.equ	E		= 1	 ; PB1 Enable  [LCDdata4-7 on PB2-5]
.equ	LCDport		= Portb  ; for 90S8535 : use portB for Mega8
.equ	LCDddr		= ddrb   ; 90S8535 : use portB for Mega8

;MIDI equates
.equ	CONTROLLER_MSG  = 0xb0
.equ	PITCHBEND_MSG   = 0xe0
.equ	CHANGE_VOICE_MSG = 0xc0
.equ	ALLNOTESOFF_MSG  = 0x7f ; 0x7b normally, but 0x7f for TQ5
.equ	SUSTAIN_ON	= 0x7f
.equ	SUSTAIN_OFF	= 0x00
.equ	POSSIBLE_NOTES  = 8
.equ	OCTAVE_DEFAULT	= 34    ; 26 notes before middle C (MIDI Note #60) ~ lowest MIDI note number on QWERTY
.equ	SPACEBAR	= 0xc0
.equ	SUSTAINKEY	= 0xc1
.equ	REG_AVAILABLE	= 0xFF
.equ	VELOCITY  	= 0x50
.equ	NOTE_ON_MSG	= 0x90
.equ	NEW_PARAM_FLAG  = 7
.equ	QUEUESIZE	= 0x20
.equ	QUEUE_MASK	= QUEUESIZE - 1
.equ	MostSigBit	= 7
.equ	Bit6		= 6
.equ	ParamADCchannel = 2
.equ	ValueADCchannel = 3
.equ	EOX		= 0xf7

;LCD instructions
.equ	clr_LCD		= 0b00000001	;clear display
.equ	home_LCD	= 0b00000010	;return home
.equ	entry_mode	= 0b00000110	;set Cursor
.equ	on_LCD		= 0b00001110	;LCD on

;PS2 keyboard equates
.equ	KBport		= PinD
.equ	KBClockPin	= 2
.equ	KBdataPin	= 3
.equ	INITIAL_BITCOUNT = 11
.equ	PARITY_BITCOUNT  = 3

;* System Constants
.equ    ADCcount	= ((SYSTEM_CLOCK/1024) / 10 ) ;SYSTEM_CLOCK/1024=7812.5 Hz


;General lower registers   --  where used or referenced
.def	SREGtemp 	= r1
.def	scanbits	= r2	; shift register for holding PC keyboard data while being assembled
.def	Rx_Head		= r3
.def	Rx_Tail		= r4
.def 	Tx_Head		= r5
.def	Tx_Tail		= r6
.def	rBin1L		= r7
.def	rBin1H		= r8
.def	rBin2L		= r9
.def	rBin2H		= r10
.def	LowTemp1	= r11
.def	UARTout	= r12
.def	ToggleReg	= r13

;General upper registers   --  where used or referenced
.def	temp	 	= r16
.def	temp2	 	= r17
.def	temp3		= r18
.def	irqtemp	 	= r19
.def	KeyFlags 	= r20
; KeyFlags register's bit values
.equ	LR_arrow_flag	= 7
.equ	CapsStatus	= 6	; Caps Lock pressed on -- no NoteOff transmitted
.equ	Alt_flag	= 5	; left Alt key is being pressed
.equ	Break_flag	= 4	; the last scancode was an 0xf0 - the break code sent when a key is released
.equ	OnOff_flag	= 3	; set means turn on the note whose MIDI number was decoded from the most recent scancode
.equ	NewScanCode	= 2	; a byte has been received from the keyboard
.equ	Extended_flag	= 1	; the extended scancode 0xE0 was sent - use the E0table to decode scancode
.equ	KPdigit_flag	= 0	; set if expecting the second digit from the keypad
.def	DecodeValue	= r21 	; PC keyboard char decoded to NoteOn value, or AllNotesOff cmd
.def	GeneralFlags	= r22
.equ	Time4ADC	= 7
.equ	InputTimeOut	= 6
.equ	KeyboardOnly	= 5	; set when get_ACED_data returns InputTimeOut flag set
.equ	TX81_TQ5	= 4	; set if TX81-Z, clear if TQ5
.def	UARTcontrolReg	= r23
.equ	Out_Active	= 7		; Most Sig Bit set when transmitting RS232 through soft_UART
;  ZH:ZL r31:30    YH:YL r29:28    XH:XL r27:26


;****************************************
; SRAM variables
;****************************************
.dseg

 ; Current Voice Operator Parameter Values from synth
Current_Voice: ;	 dec hex  memloc offset	buffer  	Adjustable_Params list in order
AttackRate1:	.byte 1 ; 00 00   0x060	  0x2d  VCED		Frq_1      :0x00
Decay1Rate1:	.byte 1 ; 01 01   0x061	  0x2e  VCED    	Frq_2      :0x01
Decay2Rate1:	.byte 1 ; 02 02   0x062	  0x2f  VCED    	Frq_3      :0x02
Decay1Level1:	.byte 1 ; 03 03   0x063	  0x31  VCED    	Frq_4      :0x03
OutputLevel1:   .byte 1 ; 04 04   0x064	  0x37  VCED    	AttackRt_1 :0x04
CoarseFreq1:    .byte 1 ; 05 05   0x065	  0x38  VCED    	AttackRt_2 :0x05
AttackRate2:	.byte 1 ; 06 06   0x066	  0x13  VCED    	AttackRt_3 :0x06
Decay1Rate2:    .byte 1 ; 07 07   0x067	  0x14  VCED    	AttackRt_4 :0x07
Decay2Rate2:    .byte 1 ; 08 08   0x068	  0x15  VCED    	OutLevel_1 :0x08
Decay1Level2:   .byte 1 ; 09 09   0x069	  0x17  VCED    	OutLevel_2 :0x09
OutputLevel2:   .byte 1 ; 10 0a   0x06a	  0x1d  VCED    	OutLevel_3 :0x0a
CoarseFreq2:    .byte 1 ; 11 0b   0x06b	  0x1e  VCED    	OutLevel_4 :0x0b
AttackRate3:	.byte 1 ; 12 0c   0x06c	  0x20  VCED    	Osc_Wave_1 :0x0c
Decay1Rate3:    .byte 1 ; 13 0d   0x06d	  0x21  VCED    	Osc_Wave_2 :0x0d
Decay2Rate3:    .byte 1 ; 14 0e   0x06e	  0x22  VCED    	Osc_Wave_3 :0x0e
Decay1Level3:   .byte 1 ; 15 0f   0x06f	  0x24  VCED    	Osc_Wave_4 :0x0f
OutputLevel3:   .byte 1 ; 16 10   0x070	  0x2a  VCED    	Decay1Rt_1 :0x10
CoarseFreq3:    .byte 1 ; 17 11   0x071	  0x2b  VCED    	Decay1Rt_2 :0x11
AttackRate4:	.byte 1 ; 18 12   0x072	  0x06  VCED    	Decay1Rt_3 :0x12
Decay1Rate4:    .byte 1 ; 19 13   0x073	  0x07  VCED    	Decay1Rt_4 :0x13
Decay2Rate4:    .byte 1 ; 20 14   0x074	  0x08  VCED    	Decay2Rt_1 :0x14
Decay1Level4:   .byte 1 ; 21 15   0x075	  0x0a  VCED    	Decay2Rt_2 :0x15
OutputLevel4:   .byte 1 ; 22 16   0x076	  0x10  VCED    	Decay2Rt_3 :0x16
CoarseFreq4:    .byte 1 ; 23 17   0x077	  0x11  VCED    	Decay2Rt_4 :0x17
Algorythm: 	.byte 1 ; 24 18   0x078	  0x3a  VCED    	Decay1Lv_1 :0x18
Feedback:	.byte 1 ; 25 19   0x079	  0x3b  VCED    	Decay1Lv_2 :0x19
VoiceName_0:	.byte 1 ; 26 1a   0x07a	  0x53  VCED    	Decay1Lv_3 :0x1a
VoiceName_1:	.byte 1 ; 27 1b   0x07b	  0x54  VCED    	Decay1Lv_4 :0x1b
VoiceName_2:	.byte 1 ; 28 1c   0x07c	  0x55  VCED    	Feedback   :0x1c
VoiceName_3:	.byte 1 ; 29 1d   0x07d	  0x56  VCED    	EffectType :0x1d
VoiceName_4:	.byte 1 ; 30 1e   0x07e	  0x57  VCED    	Effect_Spd :0x1e
VoiceName_5:	.byte 1 ; 31 1f   0x07f	  0x58  VCED    	Effect_Bal :0x1f
VoiceName_6:	.byte 1 ; 32 20   0x080	  0x59  VCED
VoiceName_7:	.byte 1 ; 33 21   0x081	  0x5a  VCED		Fn1 to Fn8 selects new algorythm
VoiceName_8:	.byte 1 ; 34 22   0x082	  0x5b  VCED
VoiceName_9:	.byte 1 ; 35 23   0x083	  0x5c  VCED

ACEDvalues:	;ACED dump values
FineFreq1:	.byte 1 ; 36 24   0x084	  0x21  ACED
OscWave1: 	.byte 1 ; 37 25   0x085	  0x22  ACED
FineFreq2:	.byte 1 ; 38 26   0x086	  0x17  ACED
OscWave2: 	.byte 1 ; 39 27   0x087	  0x18  ACED
FineFreq3:	.byte 1 ; 40 28   0x088	  0x1c  ACED
OscWave3: 	.byte 1 ; 41 29   0x089	  0x1d  ACED
FineFreq4:	.byte 1 ; 42 2a   0x08a	  0x12  ACED
OscWave4: 	.byte 1 ; 43 2b   0x08b	  0x13  ACED

EFEDSvalues:	;EFEDS dump values
Effect_Type:	.byte 1 ; 44 2c   0x08c	  0x10  EFEDS
Effect_Speed:	.byte 1 ; 45 2d   0x08d	  0x11  EFEDS
Effect_Balance:	.byte 1 ; 46 2e   0x08e	  0x12  EFEDS

.org 0x090
Rx_Queue:	.byte QUEUESIZE ; 32 bytes ; 0x090 to 0x0af
TxQueue:	.byte QUEUESIZE ; 32 bytes ; 0x0b0 to 0x0cf

; assorted variables in SRAM
SoundingNotes:	.byte POSSIBLE_NOTES 	;0x0d0-0d7
Prev_Param_Pot: .byte 1		;0x0d8
Prev_Value_Pot:	.byte 1		;0x0d9
New_Param_Pot:	.byte 1		;0x0da
New_Value_Pot:	.byte 1		;0x0db
BitCount: 	.byte 1		;0x0dc used by PC keyboard INT0 routine
scancode: 	.byte 1		;0x0dd storage of data bits received from PC keyboard
MIDIout:	.byte 1		;0x0de
OctaveOffset:	.byte 1		;0x0df
ParamNumber:	.byte 1		;0x0e0
New_Single_Val:	.byte 1		;0x0e1
NewFreqCoarse:  .byte 1		;0x0e2
NewFreqFine:	.byte 1		;0x0e3
KPvalue:	.byte 1		;0x0e4 used to decode keypad digit presses-value stored between keypresses
ASCIIdigits:	.byte 5		;0x0e5-e9
ZeroLoc:	.byte 1		;0x0ea used by LCDdigits routine to display ASCII numbers
ADCstore:	.byte 1		;0x0eb
PrevPrmTx:	.byte 1		;0x0ec store the new value of the parameter being changed on the TX81Z/TQ5
Operator:	.byte 1		;0x0ed  bit 7 set if current parameter is a frequency value
FreqOffset:	.byte 1		;0x0ee
CurrentVoice:	.byte 1		;0x0ef
CurrentCoarse:	.byte 1		;0x0f0
CurrentFine:	.byte 1		;0x0f1
T1Counter:	.byte 1		;0x0f2   increments every Timer1 IRQ. Can be used for long period timings.
T2Counter:	.byte 1		;0x0f3
KP1st:		.byte 1		;0x0f4

.org 0x100
VCED_buffer:	.byte 0x70 	;112 bytes ; 0x100 to 0x16f
ACED_buffer:	.byte 0x30 	; 48 bytes ; 0x170 to 0x19f
EFEDS_buffer:	.byte 0x20 	; 32 bytes ; 0x1a0 to 0x1bf

;    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~


.cseg
.org 000
	rjmp	reset
; interrupt vectors for 90S8535 Atmel AVR
	rjmp	GetScan_IRQ	; 0x0001 External Interrupt 0
	reti			; 0x0002 External Interrupt 1
	reti			; 0x0003 Timer/Counter2 Compare Match
	rjmp 	T2OVF_IRQ	; 0x0004 Timer/Counter2 Overflow
	reti			; 0x0005 Timer/Counter1 Capture Event
	reti			; 0x0006 Timer/Counter1 Compare Match A
	reti			; 0x0007 Timer/Counter1 Compare Match B
	rjmp	T1OVF_IRQ 	; 0x0008 Timer/Counter1 Overflow
	reti			; 0x0009 Timer/Counter0 Overflow
	reti			; 0x000a SPI Serial Transfer Complete
	rjmp	RxC_IRQ		; 0x000b UART, RX Complete
	rjmp	UDRE_IRQ	; 0x000c UART Data Register Empty
	reti			; 0x000d UART, TX Complete
	reti			; 0x000e ADC Conversion Complete
	reti			; 0x000f EEPROM Ready
	reti			; 0x0010 Analog Comparator
	reti		; Mega8 ; 0x0011 2-wire Serial Interface
	reti		; Mega8 ; 0x0012 Store Program Memory Ready

reset:
; initialize stack pointer
	ldi	temp,low  (ramend)
	out	spl,temp
	ldi	temp,high (ramend)
	out	sph,temp

; initialize I/O ports
	ldi	Temp, 0b00111111 ; in7 in6 d7 d6 d5 d4 E RS
	out	LCDddr, Temp	; LCDdata output
	cbi	LCDport, E	; port b

	in	temp, UARTDDR	; ddrd
	ori	temp, (1 << UARTOutPin)
	out	UARTDDR, temp

	in	temp, UARTOutPort  ; port d
	ori	temp, (1 << UARTOutPin)
	out	UARTOutPort, temp  ; start with UART at logic high

	in	temp, ToggleDDR  ; ddr d
	ori	temp, (1 << ToggleBit) ; bit 0
	out	ToggleDDR, temp

	ldi	temp, (1 << ToggleBit) ; bit 0
	mov	ToggleReg, temp

; initialize LCD
	lds	temp, ZeroLoc
	cpi	temp, 0xaa
	breq	iLCDdone
	rcall	LCD_init
iLCDdone:

;--------------------------------------
;  initialize debugger memory view
;--------------------------------------
	ldi	ZH, high (RAMSTART)
	ldi	ZL, low (RAMSTART)
	ldi	temp2, 1
	ldi	temp, 255
	mov	temp3, temp
	ldi	temp, 0xaa
DMV_a: 	st	z+,temp
	dec	temp3
	brne	DMV_a
	tst	temp2
	breq	DMV_done
	dec	temp2
	rjmp	DMV_a
DMV_done:

; initialize external interrupts
	ldi	temp, (1<<ISC01)
	out	MCUCR, temp
	ldi	temp, (1<<INT0)
	out	GIMSK, temp  ; General Interrupt Control [register page 42]
	ldi	temp, INITIAL_BITCOUNT
	sts	bitcount, temp

; initialize timers.  Must adjust these I/O register names for different AVR devices
	clr	temp
	sts	T1counter, temp
;	sts	T2counter, temp

	ldi	temp, (1 << TOIE1)  | (1 << TOIE2) ; enable timer interrupts
	out	TIMSK, temp
	out	TIFR, temp	; clear interrupt flags

	ldi	temp, ~( high (ADCcount)) ; Timer1: write high first for 16-bit registers
	out	TCNT1H, temp
	ldi	temp, ~ ( low (ADCcount))
	out	TCNT1L, temp
	ldi	temp,  (1<<CS12) | (0<<CS11) | (1<<CS10) ; prescaler /1024
	out	TCCR1B, temp	; turn on timer 1  used to set the Slider_Update period

	ldi	temp, ~ ( UARTreload )  ; set timer 2 for 38,400 interrupts per second  - 26 microSeconds
	out	TCNT2, temp
	ldi	temp, (1<<CS21)	; turn on Timer 2 -- prescaler = /8
	out	TCCR2, temp


; initialize USART   ; USCR0C default values OK for 8/N/1
	ldi	temp, MIDIbaud ;Baud rate
	out	UBRRL, temp
	ldi	temp, (1<<TXEN) | (1<<RXEN) | (1<<RXCIE)
	out	UCR, temp  ; turn on receive and transmit

; enable global interrupts
	sei

; initialize application
	rcall	ClearNoteTable
	clr	Rx_Head
	clr	Rx_Tail
	clr	KeyFlags
	clr	GeneralFlags
	clr	DecodeValue
	sts	KP1st, DecodeValue
	sts	KPvalue,DecodeValue
	sts	ZeroLoc, DecodeValue
	sts	CurrentVoice, DecodeValue
	sts	Operator, DecodeValue
	ser	temp
	sts	PrevPrmTx, temp
	ldi	temp, OCTAVE_DEFAULT ; sets Middle C (MIDI note# 0x3c) to the 'Z' key
	sts	OctaveOffset, temp
	ldi	ZH, high (ProgID * 2)	;  LCDmsgFlash - subroutine to put message text from Flash on LCD
	ldi	ZL, low  (ProgID * 2)	;  ZH:ZL- holds address of message text (* 2) in Flash
	ldi	temp, 0			;  Temp Reg  -  holds LCD dynamic display address for first char
	rcall	LCDmsgFlash		;  Textstring ends with 0x00

; determine if synth is connected to MIDI in/out
	rcall	Get_ACED_data		; calls Do_MIDI_input
	sbrs	GeneralFlags, InputTimeOut  ; InputTimeOut flag is set if timeout on Do_MIDI_input
	rjmp	init0			; jump if Get_ACED did NOT time out
	sbr	GeneralFlags, (1 << KeyboardOnly);    Get_ACED did time out; no synth response
	ldi	ZH, high (NoSyn * 2)	; LCDmsgFlash - subroutine to put message text from Flash on LCD
	ldi	ZL, low  (NoSyn * 2)	;  ZH:ZL- holds address of message text (* 2) in Flash
	ldi	temp, 0x40		;  Temp Reg  -  holds LCD dynamic display address for first char
	rcall	LCDmsgFlash		;  Textstring ends with 0x00
	rjmp	Main


init0:	cbr	GeneralFlags, (1 << InputTimeOut)

; determine if synth is a Yamaha TX81-Z or TQ5
	rcall	Get_EFEDS_data
	sbrs	GeneralFlags, InputTimeOut ; if set then Get_EFEDS timed out and the synth is a TX81-Z
	rjmp	init1
	sbr	GeneralFlags, (1 << TX81_TQ5)  ; set = the synth is a TX81-Z
	ldi	ZH, high (TX81ID * 2)	; LCDmsgFlash - subroutine to put message text from Flash on LCD
	ldi	ZL, low  (TX81ID * 2)	;  ZH:ZL- holds address of message text (* 2) in Flash
	rjmp	init2

init1:	ldi	ZH, high (TQ5ID * 2)	; LCDmsgFlash - subroutine to put message text from Flash on LCD
	ldi	ZL, low  (TQ5ID * 2)	;  ZH:ZL- holds address of message text (* 2) in Flash
init2:	ldi	temp, 0x40		;  Temp Reg  -  holds LCD dynamic display address for first char
	rcall	LCDmsgFlash		;  Textstring ends with 0x00

	sbrc	GeneralFlags, TX81_TQ5  ; set = the synth is a TX81-Z
	rjmp	init2a  ; jump to TXprotectoff if TX81Z

	ldi	ZH, high (TQProtectOff * 2) ; turn off the Memory Protection for TQ5
	ldi	ZL, low  (TQProtectOff * 2)
	ldi	temp, 0x03  ; TQ5 protect-off sysex = F0 43 10 24 03 00 F7
	sts	ParamNumber, temp
	ldi	temp, 0x00
	rjmp	init3

init2a:	ldi	ZH, high (TXProtectOff * 2) ; turn off the Memory Protection for TX81-Z
	ldi	ZL, low  (TXProtectOff * 2)
	ldi	temp, 0x08  ; TX81-Z protect-off sysex = F0 43 10 10 7B 08 00 F7
	sts	ParamNumber, temp
	ldi	temp, 0x00
init3:	sts	New_Single_Val, temp
	rcall	Send_SysEx

; this is a one-second delay to show synth type on LCD second line
init4:	lds	temp, T1Counter
	mov	temp3, temp
	subi	temp3, -10 ; add 10 to present T1Counter value
init4a: lds	temp, T1Counter
	cpse	temp, temp3
	rjmp	init4a  ; test and loop for one second

	ldi	ZH, high (ClearLine * 2); clear 2nd line on LCD after short ID text msg
	ldi	ZL, low  (ClearLine * 2);  ZH:ZL- holds address of message text (* 2) in Flash
	ldi	temp, 0x40		;  Temp Reg  -  holds LCD dynamic display address for first char
	rcall	LCDmsgFlash		;  Textstring ends with 0x00


; initialize ADC values and display
	rcall	GetNewADCvalues ; prev ADC 0x0d8,0d9;  new ADC result: 0x0da,0db
	lds	temp, New_Param_Pot
	sts	Prev_Param_Pot, temp
	lds	temp, New_Value_Pot
	sts	Prev_Value_Pot, temp

	ldi	temp, 0 ; initial voice on power-up/reset 'LatelyBass'
	sts	CurrentVoice, temp
	rcall	DoProgramChange ; send C0-nn, getVCED(doLCD first line),ACED,EFEDS, NewParamSelected


;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;
;  Main Loop of the Program  99.9% of time spent here
;
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Main:
	sbrc	KeyFlags, NewScanCode
	rjmp	DoNewScanCode

	sbrc	GeneralFlags, KeyboardOnly
	rjmp	Main ; KeyboardOnly flag is set because Yamaha TX synth did not respond to dump request, so skip pot read

	sbrc	GeneralFlags, Time4ADC
	rcall	ADCtimeout

	rjmp	Main	; no keypress or ADC timeout



;********************************************************************************************
; DoNewScanCode   - A new byte from the PS2 keyboard has been fully received, so parse it.
;********************************************************************************************
DoNewScanCode:
	cbr	KeyFlags,(1<< NewScanCode);  prepare for next char
	lds	temp, scancode
	rcall	decode_data  ; convert the scancode from the keyboard into an ASCII character
		; send scancode and KeyFlags - return DecodeValue
	cpi	DecodeValue, 0xff    ; if DecodeValue =0xff, then ignore scancode - it's a shift character or break scancode
	brne	Valid_key
	rjmp	Main


;*****************************************************************************
;   Valid key  -   send the processed scan code to the correct code handler
;   first test for SPACE to turn all notes off
;   then check for a regular key Note On/Off
;   then check for keypad digit press
;   then check for function key
;*******************************************************************************
Valid_key:
	cpi	DecodeValue, SPACEBAR ; 0xc0 is decode value for All_Notes_Off
	brne	VK1
	rjmp	toAllNotesOff

VK1:	cpi	DecodeValue, SUSTAINKEY
	brne	VK2
	rjmp	toSustainToggle

VK2:	cpi	DecodeValue, 0x40
	brlo	PlayNote  ; note offsets are the lowest values in the scan table
	cpi	DecodeValue, 0x50
	brlo	toDoFunctionKey	; any DecodeValue between 0x40 and 0x4f is a function key press
	cpi	DecodeValue, 0x90
	brlo	toDoDigit ; any DecodeValue between 0x80 and 0x8f is a key pad digit
	cpi	DecodeValue, 0xa0
	brlo	toParseExtKey ; any DecodeValue between 0x90 and 0x9f has an extended key scancode
	rjmp	Main

PlayNote:
	sbrc	KeyFlags,OnOff_flag   ; if OnOff flag is set, then char is a MIDI note number to be played
	rjmp	toPlayMIDInote
	sbrs	KeyFlags,OnOff_flag  ; if OnOff flag is clear, then char is a playing MIDI note to be turned off
	rjmp	toTurnOffMIDInote
	rjmp	Main ; should never get here

toDoDigit:
	rcall	DoDigit
	rjmp	Main

toDoFunctionKey:
	rcall	DoFunctionKey
	rjmp	Main

toPlayMIDInote:
	rcall	PlayMIDInote
	rjmp	Main

toTurnOffMIDInote:
	rcall	TurnOffMIDInote
	rjmp	Main

toParseExtKey:
	rcall	ParseExtKey
	rjmp	Main

toAllNotesOff:
	rcall	AllNotesOff
	rjmp	Main

toSustainToggle:
	rcall	DoSustainMsg
	rjmp	Main


;====================================================
;
;	Subroutines
;
;====================================================



;******************************************************************************************************
; ADCtimeout - Time to check for any changes on the two pots
; Get new ADC values from ADC channels zero and one.
; Compare the new ADC values with the previous values.
;******************************************************************************************************
ADCtimeout:
	cbr	GeneralFlags, (1<<Time4ADC) ;turn off flag and store old pot values

	lds	temp, New_Param_Pot ; New_Param_Pot value is 0.1 second old at this point
	sts	Prev_Param_Pot, temp
	lds	temp, New_Value_Pot
	sts	Prev_Value_Pot, temp

	rcall	GetNewADCvalues
; happens about every 1/10 second. Returns updated NewPot0, NewPot1
	lds	temp, Prev_Param_Pot ; check for turn of the Parameter Type pot
	lds	temp2, New_Param_Pot
	cpse	temp, temp2
	rjmp	ADCT_0  ; new value in param pot
	rjmp	ADCT_1  ; NewP_Pot and Prev_PPot are equal, no change in Param pot

ADCT_0:
	rcall	NewParameterSelected   ; user changed Parameter pot; so display new param string and value on LCD
	rjmp	ADCT_exit

ADCT_1:   ; NewP_Pot and Prev_PPot are equal, no change in Param pot
	lds	temp, Prev_Value_Pot ; check value pot
	lds	temp2, New_Value_Pot
	cpse	temp, temp2
	rjmp	ADCT_2
	rjmp	ADCT_exit	; if Value pot is unchanged then exit

; DoNewValuePot is called when the new ADC (returned from GetNewADCValues) of the Value_Pot is different from the previous ADC of the Value_Pot.
;  Return new sys-ex value to send to the current parameter, or  0xff to mean that the ADC change was not large enough
;  to trigger a sys-ex message to change the parameter setting on the TX81Z/TQ5.
ADCT_2:
	rcall	DoNewValuePot

ADCT_exit:
	ret


;*****************************************************************************************************
; NewParameterSelected  -  display text of param on 2nd LCD line, get and display value from SRAM table
;
; called by: reset/initialization, ADCtimeout when New_Param_Pot <> Prev_Param_Pot
; send:  New_Param_Pot
; return:
; uses: temp, temp2, ZL:ZH
;*****************************************************************************************************
NewParameterSelected:
	rcall	DisplayParamText ; LCD display second row, columns 1-10
;-----------------------------------------------------------
; Parameter is either an operator, feedback, or an effect
; If op, then get the operator number into SRAM variable.
	ldi	temp, 0xff	; initialize operator [SRAM]
	sts	operator, temp
	lds	temp, New_Param_Pot ; 0x0da SRAM
	cpi	temp, 0x1c  ; highest operator type parameter in the Param_Pot list 0-31
	brsh	SP_checkfreq ; if Prm_Pot setting is => 0x1c then it is not an operator_type of param
	andi	temp, 0x03   ;  operator_type params are listed in order of op1 to op4. Op number is in least two bits.
	sts	operator, temp

;-----------------------------------------------------------
; check if the New Parameter Selected is a frequency.  If yes, then get the
; frequency text string for the current value in the coarse setting.
SP_checkfreq:
	lds	temp, New_Param_Pot ; latest ADC conversion used as offset into string table
	cpi	temp, 4
	brsh	SP_2
	rcall	Get_CV_FreqRatio
	rcall	DisplayFreqRatio
	rjmp	SP_exit

;--------------------------------------------------------------------------------------------------------
; Not a frequency parameter so display param value ASCII digits after param text
SP_2:	ldi	temp, 0b01001011  ;clear 2char display of previous parameter value
	ldi	ZL, low  (Clear2Spaces * 2)
	ldi	ZH, high (Clear2Spaces * 2)
	rcall	LCDmsgFlash ; LCD display second line column 12
	rcall	LCD1msWait
; get the current value of the new parameter from the C_V table
	ldi	ZL, low  (NewParamCVoffsets *2) ; beginning of current voice's parameters in SRAM
	ldi	ZH, high (NewParamCVoffsets *2) ; will always be 0
	lds	temp, New_Param_Pot ; pot value is the offset into the Param Value table in SRAM
	add	ZL, temp
	brcc	SP_3
	inc	ZH
SP_3:	lpm	; r0 has the offset into the CV table for the new Param_Pot value
	ldi	ZL, low  (Current_Voice)
	ldi	ZH, high (Current_Voice)
	add	ZL, r0
; LCD display of new param value
	ld	temp, z ; get param value from SRAM table
	ldi	temp2, 0b01001011 ; LCD displayRAM location start for digits
	ldi	temp3, 2 ; number of digits to display
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD

; erase the old adjusted value when the parameter is changed
	ldi	temp, 0b01001110  ;clear 2char display of previously adjusted value of the former parameter
	ldi	ZL, low  (Clear2Spaces * 2)
	ldi	ZH, high (Clear2Spaces * 2)
	rcall	LCDmsgFlash ; LCD display second line column 12
	rcall	LCD1msWait
SP_exit:
	ret


;*************************************************************************************************************
; DisplayParamText
;
;*************************************************************************************************************
DisplayParamText:
; display text for parameter on  LCD second line
	ldi	ZL, low  (ParamText*2)
	ldi	ZH, high (ParamText*2)
	lds	temp, New_Param_Pot ; latest ADC conversion used as offset into string table
	rcall	TimesTen ; send temp: return rBin1L:H
	add	ZL, rBin1L  ; point to new parameter string so that is can be displayed on LCD 2nd line
	adc	ZH, rBin1H
	ldi	temp, 0xc0 ; write parameter text at beginning of second line
	rcall	SendLCDcommand
	ldi	temp2, 10 ; each param text string is ten chars
SP_0:	lpm
	mov	temp, R0 ; get char from flash mem
	rcall	SendLCDdata  ; print to LCD 2nd line
	adiw	ZH:ZL,1
	dec	temp2
	brne	SP_0
	ret


;*************************************************************************************************************
Get_CV_FreqRatio:  ;determine Freq_Ratio number (0-255) from the Coarse/Fine values in Current_Voice table
; arrive here if new value in New_Param_Pot is a frequency (< 4)

;- display the frequency text value of the current setting of Frequency Coarse and Frequency Fine
; To get the frequency ratio string for the Freq Coarse:Fine pair:
;  - Search the FreqCoarse table from the beginning to find the first reference of the current voice's (cur_vx) FrqC value; save the offset [n].
;  - Check the FreqFine table at the offset to see if the cur_vx's FrqFine value matches. if yes then display the FreqRatio string for that offset.
;  if FreqFine[n]<>cur_vx freq fine, then find the next occurence of cur-vx frq Coarse in the FreqCrs table and check for freq fine match.
;  repeat until the offset is found where the values in the FreqCoarse:FreqFine tables match the cur-vx's crs:fine values.
;
;  send:      temp = NewParamPot (<4), operator
;  return:    CurrentCoarse, CurrentFine, FreqOffset
	lds	temp2, ZeroLoc
; get the coarse and fine values of the current voice
	ldi	ZH, high (OpValC*2) ; get the offset into the Current_Voice table of the coarse value
	ldi	ZL, low  (OpValC*2)
	add	ZL, temp  ; temp has the operator number (0-3)
	adc	ZH, temp2 ; temp2 holds 0
	lpm	;ZH:ZL points to offset of cur-vx's operator's coarse value in r0
	ldi	ZH, high (Current_Voice)
	ldi	ZL, low  (Current_Voice)
	add	ZL, r0
	adc	ZH, temp2 ; temp2 holds value 0
	ld	temp3, z ; ZH:ZL has SRAM address of cur-vx op's coarse freq value
	sts	CurrentCoarse, temp3
	ldi	ZH, high (OpValF*2) ; get the offset of the cur-vx's operator's coarse value
	ldi	ZL, low  (OpValF*2)
	add	ZL, temp  ; temp has the operator number (0-3)
	adc	ZH, temp2 ; temp2 holds 0
	lpm	; offset to cur-vx's operator's fine value in r0
	ldi	ZH, high (Current_Voice)
	ldi	ZL, low  (Current_Voice)
	add	ZL, r0
	adc	ZH, temp2 ; temp2 holds value 0
	ld	temp3, z
	sts	CurrentFine, temp3

	clr	temp2
	lds	temp3, ZeroLoc
	sts	FreqOffset, temp2

; at this point, we have the cur-vx op's Crs and Fine values.
; Now find the first instance of cur-vx op's Crs value in the FreqCoarse table
; Point to the beginning of the FreqCoarse table and check each value for the the
; first occurence of cur-vx op's Coarse value.
SP_ff0:	ldi	ZH, high (FreqCoarse*2)
	ldi	ZL, low  (FreqCoarse*2)
	lds	temp, CurrentCoarse

SP_ff1:	lpm
	cp	temp, r0
	breq	SP_ff2
	adiw	ZH:ZL,1
	inc	temp2
	sts	FreqOffset, temp2
	breq	SP_ffer0
	rjmp	SP_ff1

SP_ff2: ; found occurence of the current voice's operator's coarse freq value in FreqCoarse table
;  now check if the value in the FreqFine table at this offset is the same as the current voice's operator's fine freq value.

	ldi	ZH, high (FreqFine*2)
	ldi	ZL, low  (FreqFine*2)
	add	ZL, temp2 ;offset count under test
	adc	ZH, temp3 ; ZeroLoc value=0
	lds	temp, CurrentFine
	lpm
	cp	temp, r0
	breq	GFR_exit

; the value in the FineFreq table at the current Freq offset doesn't match the current-voice's operator's fine freq value.
; So look for the next occurence of the current-voice's operator's coarse freq value in the CoarseFreq table.
	inc	temp2
	breq	SP_ffer0
	ldi	ZH, high (FreqCoarse*2)
	ldi	ZL, low  (FreqCoarse*2)
	add	ZL, temp2
	adc	ZH, temp3
	lds	temp, CurrentCoarse
	rjmp	SP_ff1

GFR_exit:
	ret



;*************************************************************************************************************
;DisplayFreqRatio - found the offset value for both current coarse and fine frequencies, so print ratio string
;
; send:   FreqOffset
;*************************************************************************************************************
DisplayFreqRatio:
	ldi	ZL, low  (FreqText*2)
	ldi	ZH, high (FreqText*2)
	lds	rBin1L, FreqOffset ; Value Pot's ADC conversion (0-255) is the offset into the FreqText table
	clr	rBin1H ; multiply Value Pot's ADC conversion (0-255) by 4
	lsl	rBin1L ; and add result to beginning Flash address of FreqText table
	rol	rBin1H
	lsl	rBin1L
	rol	rBin1H
	add	ZL, rBin1L ; point at beginning of the correct FreqRatio string (addr of FreqText + (Freq Offset*4))
	adc	ZH, rBin1H
	ldi	temp, 0xc6 ; write parameter text at 7th char position of second line
	rcall	SendLCDcommand
	ldi	temp2, 4 ; each param text string is four chars
SP_ff3:	lpm
	mov	temp, R0 ; get char from flash mem
	rcall	SendLCDdata  ; print to LCD 2nd line one char at a time
	adiw	ZH:ZL,1
	dec	temp2
	brne	SP_ff3

	ldi	temp, 0b01001011  ;clear previous parameter value
	ldi	ZL, low  (Clear5Spaces * 2)
	ldi	ZH, high (Clear5Spaces * 2)
	rcall	LCDmsgFlash
	rcall	LCD1msWait
DFR_exit: ret

; The combination of current-voice's operator's coarse and fine freq valuess was not found in the Freq ratio tables
SP_ffer0:
;   so print '-.--' as the freq ratio string.  The string number for the c-v's op's coarse and fine is greater than 255.
	ldi	temp, 0b11000110; 0xc6 ; write parameter text at 7th char position of second line
	rcall	SendLCDcommand
	ldi	ZL, low  (NoFreqRatioStr*2) ; beginning of current voice's parameters in SRAM
	ldi	ZH, high (NoFreqRatioStr*2)
	ldi	temp2, 4 ; each param text string is four chars
SP_ffer2:
	lpm
	mov	temp, R0 ; get char from flash mem
	rcall	SendLCDdata  ; print to LCD 2nd line
	adiw	ZH:ZL,1
	dec	temp2
	brne	SP_ffer2

	ldi	temp, 0b01001011  ;clear previous parameter value
	ldi	ZL, low  (Clear5Spaces * 2)
	ldi	ZH, high (Clear5Spaces * 2)
	rcall	LCDmsgFlash
	rcall	LCD1msWait

	rjmp	DFR_exit



;***************************************************************************************************
;DoNewValuePot  - arrive here if the Value pot has changed since the last Timer interrupt
;  (as determined by ADCtimeout).  Get the current parameter and the MaxValue for the current
;  parameter.  Check if the new setting of the Value Pot is different enough from
;  the previous setting in order to justify sending a Parameter_Change sys-ex message.
;  > Write the digits for adjusted New_Value_Pot value to second line/right corner.
;
;  called by: 	ADCtimeout
;  send: 	(SRAM) Prev_Value_Pot, New_Value_Pot (different from Prev), New_Param_Pot
;  return:
;  uses:	ZH:ZL, temp, temp2, r0
;***************************************************************************************************
DoNewValuePot:
	lds	temp3, ZeroLoc
; determine if the parameter being changed is a frequency value
	lds	temp, New_Param_Pot
	cpi	temp, 4
	brsh	DNVP_0
	rcall	doNewFrequencyValue
	rjmp	DNVP_exit

;---- The new setting of the Value pot is NOT an adjustment of a frequency value
DNVP_0:
	ldi	ZH, high (Param_Max * 2)
	ldi	ZL, low  (Param_Max * 2)
	lds	temp, New_Param_Pot
	add	ZL, temp
	brcc	DNVP_1
	inc	ZH
DNVP_1:	lpm
	mov	temp2, r0 ; temp2 has the scaled Max Value of the selected parameter in New_Param_Pot
	lds	temp, New_Value_Pot ; 00-0x7f in N_V_P
; FindRange   send:   temp with the New_Value_Pot (00-0x7f), temp2 with maxParam of New_Param_Pot
;             return: XL with the size of each section
	rcall	FindRange
	lds	XH, PrevPrmTx  ; PrevPrmTx [Previous Parameter Transmitted]
	cpse	XL,XH
; if XL=XH (the latest N_V_P result is in the same range as the previous N_V_P result), then do nothing.
	rjmp	DNVP_2

DNVP_exit:
	ret
;----------------------------------------------------------------------------------------------------------
; the new setting of the Value Pot is beyond the range of the previous setting, so send a Single_param_change sys-ex message
; ----- First, get the parameter value needed for the Single_Parameter_Change sys-ex message from the adjusted N_P_Pot value
DNVP_2:	lds	temp2, New_Param_Pot ; 00-0x1f
	ldi	ZH, high (NewParamCVoffsets * 2)  ; 46 values in the Current_Voice table
	ldi	ZL, low  (NewParamCVoffsets * 2)
	add	ZL, temp2 ; add adjusted New_Param_Pot value to the CVoffset table beginning
	adc	ZH, temp3 ; temp3=0
	lpm	; r0 has the offset of the N_P_Pot setting into C_V table
	mov	temp2, r0 ; temp2 has the offset of N_P_Pot into C_V table
	mov	lowtemp1, r0 ; temp storage of the offset of N_P_Pot into C_V table
	cpi	temp2, 36 ;0x24 number of VCED params in the Current_Voice SRAM table plus 1
	brsh	DNVP_3  ; if branch taken then N_P_P value is not a VCED parameter
	ldi	YH, high (VCEDchngHdr * 2)
	ldi	YL, low  (VCEDchngHdr * 2)
	rjmp	DNVP_send

; new Oscillator Wave (in ACED setting).  The other ACED param (fine frequency) is handled in the frequency section.
DNVP_3:	cpi	temp2, 44  ; 0x2c number of VCED and ACED params in the Current_Voice SRAM table plus 1
	brsh	DNVP_4 ; if branch taken then N_P_P value is an EFEDS parameter
	ldi	YH, high (ACEDchngHdr*2)
	ldi	YL, low (ACEDchngHdr*2)
	rjmp	DNVP_send

; new effects (EFEDS) setting.
DNVP_4:	ldi	YH, high (EFEDSchngHdr*2)
	ldi	YL, low (EFEDSchngHdr*2)

;----- Second, get the adjusted new value from the Current_Voice table and send it to the synth in a Single_Param_Change sys-ex message
DNVP_send:
	ldi	ZH, high (CVtoParamNumber * 2) ; Current_Voice position to the parameter number that is used by Single_Param_Change sys-ex message
	ldi	ZL, low  (CVtoParamNumber * 2)
; Values in CVtoParamNumber table are offsets into the three input buffers for the selected parameter minus the header.  (and plus 3 for EFEDS)
	add	ZL, temp2
; temp2 has the offset of N_P_Pot into C_V table. If N_P_pot = 4 then param is AR1, temp2=0x00 (which is the location of AR1 is in C_V table)
	adc	ZH, temp3 ; temp3=0 (ZeroLoc in SRAM)
	lpm     ; r0 has the SingleParamSend msg ParamNumber
	sts	ParamNumber, r0
	sts	New_Single_Val, XL ; XL returned from FindRange subroutine
	sts	PrevPrmTx, XL
	mov	ZL, YL       ; send sys-ex message for Single_Parameter_Change
	mov	ZH, YH	; set up pointer to the header of the message string
	rcall	Send_SysEx

	mov	temp, XL ; display the synth's new parameter value on the LCD
	ldi	temp2, 0b01001101; LCD screen location for start for digit display
	ldi	temp3, 3 ;  number of digits to display on LCD
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD

; put the new value into the C_V table
	ldi	ZH, high (Current_Voice)
	ldi	ZL, low  (Current_Voice)
	add	ZL, Lowtemp1  ; previously saved in DNVP_2
	st	Z, XL

; put the new value into the correct location of the correct input buffer
; ParamNumber [SRAM] has offset (without header) of the parameter into the input buffer. Actual buffer not specified.
; New_Single_Val [SRAM] has the changed value.  Pot value has been adjusted for scale.
;  Sat March 7, 2009  build 5, placing new value into correct buffer and location works for VCED, but not ACED.
	ldi	ZH, high ( NewParamCVoffsets * 2)
	ldi	ZL, low  ( NewParamCVoffsets * 2)
	lds	temp2, New_Param_Pot ; has offset into C_V table of current param
	lds	temp3, ZeroLoc
	add	ZL, temp2
	adc	ZH, temp3
	lpm
	mov	temp, r0
	cpi	temp, 0x24  ; beginning of ACED buffer in C_V table
	brsh	DNVP_5
	ldi	ZH, high (VCED_Dump_Offset * 2)
	ldi	ZL, low  (VCED_Dump_Offset * 2)
	lds	temp3, ZeroLoc
	add	ZL, temp
	adc	ZH, temp3
	lpm

	ldi	ZH, high (VCED_buffer)
	ldi	ZL, low  (VCED_buffer)

	add	ZL, r0
	lds	temp, New_Single_Val
	st	Z, temp
	rjmp	DNVP_exit

DNVP_5:	cpi	temp, 0x2c
	brsh	DNVP_6
	subi	temp, -0x24
	ldi	ZH, high (VCED_Dump_Offset * 2)
	ldi	ZL, low  (VCED_Dump_Offset * 2)
	lds	temp3, ZeroLoc
	add	ZL, temp
	adc	ZH, temp3
	lpm
; put the value in New_Single_Val into the ACED buffer
	ldi	ZH, high (ACED_buffer)
	ldi	ZL, low  (ACED_buffer)
	mov	temp, r0
	subi	temp, -25
	add	ZL, temp
	lds	temp, New_Single_Val
	st	Z, temp
        rjmp	DNVP_exit

DNVP_6:	ldi	ZH, high (EFEDS_buffer)
        ldi	ZL, low  (EFEDS_buffer)
	lds	temp2, ParamNumber
	subi	temp2, -0x0c ; 0x1c == (offset into C_V) + (header size of EFEDS)
	add	ZL, temp2
        lds	temp, New_Single_Val
        st	Z, temp
        rjmp	DNVP_exit



;*********************************************************************************************************************
; FindRange     find number of sections for this param and which sections the Prev and New valuePots readings are in.
;
; send:   temp with the New_Value_Pot (00-0x7f), temp2 with maxParam of New_Param_Pot
; return: XL with the size of each section
;*********************************************************************************************************************
FindRange:     ;   						MaxParam	       #size of section  #sections
	cpi	temp2, 64 ; branch if max value >= 0x40 0b01nnnnnn > 0b01nnnnnn  127- - - - - - 1 - 0        127
	brsh	FR_exit
	lsr	temp
	cpi	temp2, 32 ; branch if max value >= 0x20 0b001nnnnn > 0b001nnnnn  127-126-124- - 2 - 0         64
	brsh	FR_exit
	lsr	temp
	cpi	temp2, 16 ; branch if max value >= 0x10 0b0001nnnn > 0b0001nnnn  127-124-120- - 4 - 0         32
	brsh	FR_exit
	lsr	temp
	cpi	temp2, 8  ; branch if max value >= 0x08 0b00001nnn > 0b00001nnn  127-120-112- - 8 - 0         16
	brsh	FR_exit
	lsr	temp
	cpi	temp2, 4  ; branch if max value >= 0x04 0b000001nn > 0b000001nn  127-112- 96- -16 - 0          8
	brsh	FR_exit
	lsr	temp
	cpi	temp2, 2  ; branch if max value >= 0x02 0b0000001n > 0b0000001n  127- 96- 64- -32 - 0          4
	brsh	FR_exit
	lds	temp, New_Value_Pot
	cpi	temp, 64  ; param is a switch		0b00000001 ? 0b01111111 : 0b00000000   64              2
	brsh	FR_1
	clr	temp
	rjmp	FR_exit
FR_1:	ldi	temp, 127
FR_exit:
	mov	XL, temp
	ret



;******************************************************************************************
; doNewFrequencyValue  - arrives here when New_Param is <4 and Value pot is changed.
;
;  Display on LCD the freq ratio string and C/F values.
;******************************************************************************************
doNewFrequencyValue:
	ldi	temp, ValueADCchannel ; re-read the Value pot in order to get its full range (0-255)
	rcall	doADC ; return from doADC with new value 0-0xff of Value pot in ADCstore
	lds	LowTemp1, ADCstore ; store this value in case a timer interrupt happens and ADCstore gets overwritten
	lds	temp2, ZeroLoc
; ----- use FreqOffset to get the coarse value of the adjusted frequency that is found in the FreqCoarse table
	ldi	ZH, high (FreqCoarse*2) ; get the value of the cur-vx's operator's coarse value
	ldi	ZL, low  (FreqCoarse*2)
	add	ZL, LowTemp1
	adc	ZH, temp2     ; add zero and carry bit to high byte for 16-bit addition
	lpm  	;  r0 has the value from the FreqCoarse table of the setting of the Value pot (0-255)
	sts	CurrentCoarse, r0
; ------ store the coarse value of the new frequency into the Current_Voice SRAM table
	ldi	ZH, high (OpValC * 2)
	ldi	ZL, low  (OpValC * 2)
	lds	temp, Operator
	add	ZL, temp
	adc	ZH, temp2
	lpm 	; r0 holds offset of operators' coarse value in Current_Voice table
	ldi	ZH, high (Current_Voice)
	ldi	ZL, low  (Current_Voice)
	add	ZL, r0
	lds	temp, CurrentCoarse
;------- store the fine value of the new frequency into the Current_Voice SRAM table
	st	z, temp
	ldi	ZH, high (OpValF * 2)
	ldi	ZL, low  (OpValF * 2)
	lds	temp, Operator
	add	ZL, temp
	adc	ZH, temp2
	lpm 	; r0 holds offset of operators' fine value in Current_Voice table
	ldi	ZH, high (Current_Voice)
	ldi	ZL, low  (Current_Voice)
	add	ZL, r0
	lds	temp, CurrentFine
	st	z, temp
;  ----- send the coarse value of the adjusted frequency to the synth
	lds	temp, Operator
	ldi	ZH, high (SingleMsgCrsNum * 2) ; get the offset of the cur-vx's operator's fine value
	ldi	ZL, low  (SingleMsgCrsNum * 2)
	add	ZL, temp
	adc	ZH, temp2 ; temp2  = 0; add zero and carry bit to high byte for 16-bit addition
	lpm	; r0 holds offset of operator's fine frequency value into Current_Voice table
	sts	ParamNumber, r0
	lds	temp, CurrentCoarse
	sts	New_Single_Val, temp
	ldi	ZH, high (VCEDchngHdr * 2) ; get the offset of the cur-vx's operator's fine value
	ldi	ZL, low  (VCEDchngHdr * 2)
	rcall	Send_SysEx
; ----- send the fine value of the adjusted frequency to the synth
	lds	temp, Operator
	ldi	ZH, high (SingleMsgFineNum * 2) ; get the offset of the cur-vx's operator's fine value
	ldi	ZL, low  (SingleMsgFineNum * 2)
	add	ZL, temp
	adc	ZH, temp2 ; temp2  = 0; add zero and carry bit to high byte for 16-bit addition
	lpm	; r0 holds offset of operator's fine frequency value into Current_Voice table
	sts	ParamNumber, r0
	lds	temp, CurrentFine
	sts	New_Single_Val, temp
	ldi	ZH, high (ACEDchngHdr * 2) ; get the offset of the cur-vx's operator's fine value
	ldi	ZL, low  (ACEDchngHdr * 2)
	rcall	Send_SysEx
; --- display on LCD the adjusted frequency ratio string
	ldi	ZL, low  (FreqText * 2) ; beginning of current voice's parameters in SRAM
	ldi	ZH, high (FreqText * 2)
	clr	temp2
	lsl	Lowtemp1 ; LowTemp1 value is the current setting of the Value pot (full range 0-255)
	rol	temp2
	lsl	Lowtemp1
	rol	temp2
	add	ZL, LowTemp1
	adc	ZH, temp2
	ldi	temp, 0b11000110; write parameter text at 6th char position of second line
	rcall	SendLCDcommand
	ldi	temp3, 4 ; each param text string has four chars so multiply offset by 4 to point at correct string in table.
dFV_0:	lpm
	mov	temp, R0 ; get char from flash mem
	rcall	SendLCDdata  ; print to LCD 2nd line
	adiw	ZH:ZL,1
	dec	temp3
	brne	dFV_0
; ---display on LCD the adjusted frequency's coarse value
	lds	temp, CurrentCoarse ; get param value (using pot0 ADC value as offset) from SRAM table
	ldi	temp2, 0b01001011 ; LCD displayRAM location start for digits
	ldi	temp3, 2 ; number of digits to display
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD
; --- display on LCD the adjusted frequency's fine value
	lds	temp, CurrentFine ; get param value (using pot0 ADC value as offset) from SRAM table
	ldi	temp2, 0b01001110 ; LCD displayRAM location start for digits
	ldi	temp3, 2 ; number of digits to display
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD
	ret


;*********************************************************************************
;  GetNewADCvalues   - do ADC on voltage channels zero and one
;   send:
;   return:   New_Param_Pot,New_Value_Pot
;   uses:     temp
;**********************************************************************************
GetNewADCvalues:
	ldi	temp, ParamADCchannel
	rcall	doADC	; return with new value 0-0xff in ADCstore register
	lds	temp, ADCstore
	lsr	temp	; 255/2 = 128
	lsr	temp	; 255/4 = 64
	lsr	temp	; 255/8 = 32 range 0x00-0x1f for new parameter
	sts	New_Param_Pot, temp
	ldi	temp, ValueADCchannel
	rcall	doADC	; return with new value 0-0xff in ADCstore register
	lds	temp, ADCstore
	lsr	temp	; divide by 2, range 0-0x7f for new value
	sts	New_Value_Pot, temp
	ret



;;*********************************************************************************
;;   doADC   - run analog-to-digital convertor, then return high bits (9-2)
;;   --- 8535 version has no ADLAR feature. So must shift the two highest ADC result
;; 	bits into the ADC_low_result register.
;;   send:	The ADC channel in Temp register
;;   return:	ADCstore register with the highest 8 bits of the conversion.
;;   uses:	temp2, temp
;;**********************************************************************************
;doADC:	out	ADMUX, temp
;; ADEN = ADC enable;  ADIE = irq enable; clock/32 for ADCclk (250KHz)
; 	ldi	temp, (1 << ADEN) | (1 << ADPS2) | (0 << ADPS1)  | (1 << ADPS0)
;	out	ADCSR, temp
;	sbi	ADCSR, ADSC 	; start conversion
;doADC0:	sbic	ADCSR, ADSC	; set means ADC conversion still in progress
;	rjmp	doADC0  	; conversion takes 13 ADC clocks  13*32 sys clocks==@52uS per conversion
;	in	temp,  ADCL	; >>> MUST READ ADCL first!!!!
;	in	temp2, ADCH	;  spent several hours finding the "ADCH_was_read_before_ADCL" bug
;	lsr	temp2		; LSB into carry, zero into MSB
;	ror	temp		; carry into MSB.  LSB rotates into carry, but gets discarded
;	lsr	temp2
;	ror	temp
;	sts	ADCstore, temp
;	ret

;********************************************************************************************
; doADC  Mega8   called by: GetNewADC  >>  ADC clock set at 125K (8MHz/64)
; send:		The ADC channel in Temp
; return:	Left aligned upper 8 bits of result in ADCstore register
; uses:  	temp
; if ADLAR=1 > bits 9-2 in ADCH ; ADC5 pin 28
;********************************************************************************************
doADC:	ori	temp, (1 << ADLAR); |(0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0)
	out	ADMUX, temp ; ADC result bits 9-2 in ADCH upon completion
	ldi	temp, (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1)  | (0 << ADPS0); sysclk 8MHz/64=125K
	out	ADCSRA, temp; enable ADC
	ori	temp, (1<<ADSC)
	out	ADCSRA, temp
ADCdly:
	in	temp, ADCSRA
	sbrc	temp, ADSC
	rjmp	ADCdly
	in	temp, ADCH
	sts	ADCstore, temp
	ret


;**********************************************************************************
; DoSustainMsg - toggle the synth's Sustain status when CapsLock key pressed
; send: KeyFlags
; return:
;**********************************************************************************
DoSustainMsg:   ; set up MIDI message for sustain controller change
	ldi	temp, (1<<CapsStatus)
	eor	KeyFlags, temp		; toggle the CapsStatus flag when CapsLock key is pressed
	ldi	temp, CONTROLLER_MSG	; then use this new status to change the Sustain setting on the synth.
	sts	MIDIOut, temp
	rcall	PutMIDIbyte
	ldi	temp, 0x40   ; sustain controller ID number
	sts	MIDIOut, temp
	rcall	PutMIDIbyte
	sbrc	KeyFlags, CapsStatus
	rjmp	SendSusON
	clr	temp
	sts	MIDIOut, temp
	rcall	PutMIDIbyte
	rjmp 	DoSus_exit
SendSusON:
	ldi	temp, SUSTAIN_ON ;0x7f
	sts	MIDIOut, temp
	rcall	PutMIDIbyte
DoSus_exit:
	ret



;*********************************************************************
; TimesTen - multiply 8-bit number by ten for devices without MUL instruction
; send: temp
; return: rBin1L, rBin1H
;*********************************************************************
TimesTen:
	mov	rBin1L, temp
	clr	temp2
	clr	rBin1H
	clc
	lsl	rBin1L
	rol	rBin1H
	lsl	rBin1L
	rol	rBin1H
	lsl	rBin1L
	rol	rBin1H
	add	rBin1L, temp
	adc	rBin1H, temp2
	add	rBin1L, temp
	adc	rBin1H, temp2
	ret



;***********************************************************************************************************
;  Get_VCED_Data - send/receive SysEx msgs. Put data into SRAM parameter
;  create LCD first line complete display with new text and data
;***********************************************************************************************************
Get_VCED_Data:
	ldi	ZH, high (VCEDreq * 2)
	ldi	ZL, low  (VCEDreq * 2)
	ldi	YH, high (VCED_Buffer) ; 0x170 to 0x19f  48 bytes
	ldi	YL, low  (VCED_Buffer)
	rcall	Do_MIDI_input ; VCED data in sysex buffer 0x100-0x16f
	ldi	ZH, high (VCED_dump_offset *2)	; *2 because referring to Flash location
	ldi	ZL, low  (VCED_dump_offset *2)
	ldi	XH, high (Current_Voice)	; 0x060 to 0x08f
	ldi	XL, low  (Current_Voice)	; where data is copied to

GVD_0:	lpm	; r0 has the offset into the SysEx buffer for the parameter being placed into the Current_Voice SRAM table
	tst	r0
	brmi	GVD_1
	mov	YL, r0
	ld	temp, y	; temp has the parameter's value from the synth
	st	x+, temp
	adiw	ZH:ZL, 1
	rjmp	GVD_0 ; load_VCED_values_into_SRAM_table loop


GVD_1:	ldi	temp, 0b00000000   ; remove the previous voice name and data from the first LCD display line
	ldi	ZL, low  (ClearLine * 2)
	ldi	ZH, high (ClearLine * 2)
	rcall	LCDmsgFlash
	rcall	LCD1msWait

	ldi	temp, 0x80  ; display voice name on LCD
	rcall	SendLCDcommand
	rcall	LCD1msWait
	ldi	temp2, 10
	ldi	YH, high (VoiceName_0)
	ldi	YL, low  (VoiceName_0)
GVD_2:	ld	temp, Y+
	rcall	SendLCDdata
	dec	temp2
	brne	GVD_2  ; display voice name loop

; display Program number at LCD first row, column 11, 3 digits
	ldi	temp, 0b00001101 ;D=1 display on; C=1 cur on; B=1 blink on
	rcall	SendLCDcommand
	rcall	LCD1msWait
	lds	temp, CurrentVoice  ; display current voice number on right side of top line of LCD
	ldi	temp2, 0b00001010 ; start display of digits at 11th char of first LCD line
	ldi	temp3, 3
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD

; display algorythm number at LCD first row, column 16, 1 digit
	ldi	ZH, high (Algorythm)
	ldi	ZL, low  (Algorythm)
	ld	temp, z  ; value of algorythm
	ldi	temp2, 0b00001111
	ldi	temp3, 1
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD
	ret



;***********************************************************************************************************
;  Get_ACED_Data -  Get ACED values from synth.  Put Freq Fine and Osc Wave into Current_Voice SRAM table
;
;***********************************************************************************************************
Get_ACED_Data:
	ldi	ZH, high (ACEDreq * 2)
	ldi	ZL, low  (ACEDreq * 2)
	ldi	YH, high (ACED_Buffer) ; 0x170 to 0x19f  48 bytes
	ldi	YL, low  (ACED_Buffer)
	rcall	Do_MIDI_input ; input buffer filled with data from TX/TQ5

	ldi	temp, 25 ; MUST have this long 25mSec delay
GAD_0:	rcall	LCD1msWait
	dec	temp
	brne	GAD_0

	ldi	ZH, high (ACED_dump_offset *2)
	ldi	ZL, low  (ACED_dump_offset *2)
	ldi	XH, high (ACEDvalues)
	ldi	XL, low  (ACEDvalues)

GAD_1:	ldi	YH, high (ACED_Buffer) ; 0x170 to 0x19f  48 bytes
	ldi	YL, low  (ACED_Buffer)
	lpm	; r0 has the offset into the SysEx buffer for the parameter being placed into the Current_Voice SRAM table
	tst	r0
	brmi	GAD_exit
	add	YL, r0  ; YH:YL has address of ACED_buffer beginning [0x170]
	ld	temp, y	; temp has the parameter's value from the synth
	st	x+, temp
	adiw	ZH:ZL, 1
	rjmp	GAD_1
GAD_exit:
	ret



;******************************************************************************************************************
;  Get_EFEDS_Data - Get values for Effects Type, speed, and balance from synth.  Put into Current_Voice SRAM table
;
;******************************************************************************************************************
Get_EFEDS_Data:
	ldi	ZH, high (EFEDSreq * 2)
	ldi	ZL, low  (EFEDSreq * 2)
	ldi	YH, high (EFEDS_Buffer)
	ldi	YL, low  (EFEDS_Buffer)
	rcall	Do_MIDI_input
	ldi	ZH, high (EFEDSvalues) ; beginning of effect values in the Current_voice SRAM table
	ldi	ZL, low  (EFEDSvalues)
	clr	temp2
	ldi	YH, high (EFEDS_Buffer + 16)
	ldi	YL, low  (EFEDS_Buffer + 16)

GED_4:	ld	temp, Y+
	st	Z+, temp
	inc	temp2
	cpi	temp2, 3
	brlo	GED_4
	ret



;*****************************************************************************************
; Do_MIDI_input - send request message, get bytes into SysEx buffer until 0xf7
;
; send:  ZH:ZL points to sys-ex request message
; return: GeneralFlags:InputTimeOut - set if no data arrived within timeout period
; uses:  YH:YL, temp, temp2
;*****************************************************************************************
Do_MIDI_input:
; send the dump-request sys-ex message to the synth
	lpm  	; output each byte of the MIDI sysex request string pointed to by ZH:ZL
	sts	MIDIout, r0
	tst	r0
	breq	DMI_0
	rcall	PutMIDIbyte
	adiw	ZH:ZL,1
	rjmp	Do_MIDI_input

DMI_0:	lds	temp, T1Counter	; If synth is a TX81z, then this routine will time-out when called from Get_EFEDS_Data
	mov	temp3, temp
	subi	temp3, -3

	clr	Rx_Head
	clr	Rx_Tail
	sbr	GeneralFlags, (1<<InputTimeOut)

DMI_1:	cpse	Rx_Head, Rx_Tail    ; wait for each byte to arrive from synth
	rjmp	DMI_2  ; RxH<>RxT means new byte in the input queue
; no new data on this loop, so check for timeout
	lds	temp, T1Counter
	cpse	temp, temp3  ; t=t2 means 200mS past with no data input; exit with timeout flag set
	rjmp	DMI_1
	rjmp	DMI_exit

DMI_2:	cbr	GeneralFlags, ( 1<<InputTimeOut)
; clear timeout flag if any data arrives within 200 mS of dump-request msg
	ldi	ZH, high (Rx_Queue)
	ldi	ZL, low  (Rx_Queue)
	lds	temp2, ZeroLoc
	add	ZL, Rx_Head
	adc	ZH, temp2
	ld	temp, Z
	st	Y+, temp
	mov	temp2, Rx_Head ; adjust pointer to next location in input queue
	inc	temp2
	andi	temp2, QUEUE_MASK
	mov	Rx_Head, temp2
	cpi	temp, EOX; 0xf7
	brne	DMI_1 ; wait for next input byte from UART
DMI_exit:
	ret



;***********************************************************************************************************
;  Function key was pressed - Change Algorythm
;
;***********************************************************************************************************
DoFunctionKey:
	mov 	temp, DecodeValue
	andi	temp, 0b00001111
	cpi	temp, 8
	brsh	DoFnExit
	sts	New_Single_Val, temp
	ldi	ZH, high (VCEDchngHdr * 2)
	ldi	ZL, low (VCEDchngHdr * 2)
	ldi	temp, 52 ;0x34
	sts	ParamNumber, temp
	rcall	Send_SysEx

	lds	temp, New_Single_Val
	ldi	ZH, high (VCED_buffer + 0x3a)
	ldi	ZL, low  (VCED_buffer + 0x3a)
	st	z, temp
	ldi	ZH, high (Algorythm)
	ldi	ZL, low  (Algorythm)
	st	z, temp ;new value of algorythm

	ldi	temp2, 0b00001111
	ldi	temp3, 1
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD

DoFnExit:
	ret



;********************************************************************************************
; Send_SysEx  TX81Z/TQ5 sys-ex message to change a single parameter
;  used to change single VCED,ACED, EFEDS parameter values
;
; send:	ZH,ZL (address of SingleParam msg), ParamNumber (SRAM), New_Single_Val  (SRAM)
;	ZH,ZL will have VCEDchngHdr, ACEDchngHdr, EFEDSchngHdr address
;	paramNumber is the param's dump position minus the header byte count
;	Example: Algorythm is VCED dump position 0x3a. PrmNm is 0x34
; uses: temp
;********************************************************************************************
Send_SysEx:
	lpm
	mov	temp, R0
	cpi	temp, 0xff
	breq	STX_1
	sts	MIDIout, temp
	rcall	PutMIDIbyte
	adiw	ZH:ZL,1
	rjmp	Send_SysEx
STX_1:	lds	temp, ParamNumber
	sts	MIDIout, temp
	rcall	PutMIDIbyte
	lds	temp, New_Single_Val
	sts	MIDIout, temp
	rcall	PutMIDIbyte
	ldi	temp, 0xf7
	sts	MIDIout, temp
	rcall	PutMIDIbyte
	ret



;*******************************************************************************************
; PrintParams  - Send ASCII of voice parameters to RS232 serial port soft_UART  38.4Kbaud
;  ver 4.5 build 0    bug --- RS232 transmits endlessly when PageDown button gets pressed.
;*******************************************************************************************
PrintParams:
	clr	temp3		; print Voice Name
	clr	ZH
	ldi	ZL, low  (Current_Voice + 26 )	;  ZH:ZL- holds address of VoiceName chars in SRAM
PrPm0:	ld	UARTout, z+
	rcall	RS_SendChar
	inc	temp3
	cpi	temp3, 10
	brlo	PrPm0

	ldi	temp, 0x0d
	mov	UARTout, temp	; print new line, carriage return
	rcall	RS_SendChar
	ldi	temp, 0x0a
	mov	UARTout, temp
	rcall	RS_SendChar

; print 26 operator parameters
	clr	temp3
	ldi	ZH, high (PrintText * 2) ;  LCDmsgFlash - subroutine to put message text from Flash on LCD
	ldi	ZL, low  (PrintText * 2) ;  ZH:ZL- holds address of message text (* 2) in Flash
	clr	YH
	ldi	YL, low (Current_Voice)
PrPm1:	rcall	PrOneParam
	inc	temp3
	cpi	temp3, 26
	brlo	PrPm1
	clr	temp3
	ldi	ZH, high (PrintText2 * 2) ;  LCDmsgFlash - subroutine to put message text from Flash on LCD
	ldi	ZL, low  (PrintText2 * 2) ;  ZH:ZL- holds address of message text (* 2) in Flash
	clr	YH
	ldi	YL, low (Current_Voice + 0x24)
PrPm2:	rcall	PrOneParam
	inc	temp3
	cpi	temp3, 11
	brlo	PrPm2
	ret


;**************************************************************************************************************
; PrOneParam - send one line of text to RS232  six chars of param name and five digits/spaces of param value
; send: ZH:ZL pointing to first char in param name string
; return:
; uses:  temp2, YH:YL (Current_Voice SRAM table pointer), rbin1L, rBin1H, XH:XL (pointer into ASCII string of digits)
; calls:  Bin2ToASC5
;**************************************************************************************************************

PrOneParam:   ; sends one line from PrintText to softUART. Then send 5 digits (2-3 spaces before digits) and CR/LF.
	ldi	temp2,6
Pr1P_1:	lpm
	mov	UARTOut, r0
	rcall	RS_SendChar
	adiw	ZH:ZL, 1
	dec	temp2
	brne	Pr1P_1  ; display voice name loop
	clr	rBin1H
	ld	rBin1L, Y+ ; value of the current parameter to be printed
	push	ZH
	push	ZL
	ldi	ZH, high (ASCIIdigits)
	ldi	ZL, low  (ASCIIdigits)
	rcall	Bin2ToASC5 ; In: 16-bit-binary in rBin1H:rBin1L, ZH:ZL points to 'ASCIIdigits'
	pop	ZL
	pop	ZH
	ldi	temp2, 5
	ldi	XH, high (ASCIIdigits)
	ldi	XL, low  (ASCIIdigits)
Pr1P_2:	ld	UARTOut, x+
	rcall	RS_SendChar
	dec	temp2
	brne	Pr1P_2
	ldi	temp, 0x0d
	mov	UARTOut, temp
	rcall	RS_SendChar
	ldi	temp, 0x0a
	mov	UARTOut, temp
	rcall	RS_SendChar
	ret

;*********************************************************************************
;  RS_SendChar  - set up soft UART, poll until softUART is finished sending char
;*********************************************************************************
RS_SendChar:
	ldi	UARTcontrolReg, (1 << Out_Active) + 10
RS_SC0:	sbrc	UARTcontrolReg, Out_active
	rjmp	RS_SC0
	ret



;**************************************************************************************************
; StoreVoiceToSynth  send the altered parameters to the Yamaha TX/TQ synth
;
;  Each changed parameter has its new value sent to the synth using SINGLE_CHANGE sysex message.
;  And the new value is placed into the table of parameter values that was originally downloaded.
;  When this routine gets called, the values in the SRAM parameter buffers are retransmitted to
;  the synth, along with correct checksums.  This will store the adjusted voice into the synth at
;  the current voice location.
;
;**************************************************************************************************
StoreVoiceToSynth:
; get undated checksums for the VCED, ACED, EFEDS buffers
	ldi	ZH, high (VCED_buffer + 6)
	ldi	ZL, low  (VCED_buffer + 6)
	ldi	temp, 0x5d ; 93
	rcall	GetChecksum
	ldi	ZH, high (VCED_buffer + 0x63) ; VCED checksum location
	ldi	ZL, low  (VCED_buffer + 0x63)
	st	Z, temp  ; store new checksum value
	ldi	ZH, high (ACED_buffer + 6)
	ldi	ZL, low  (ACED_buffer + 6)
	ldi	temp, 0x21
	rcall	GetChecksum
	ldi	ZH, high (ACED_buffer + 0x27)  ; ACED checksum location
	ldi	ZL, low  (ACED_buffer + 0x27)
	st	Z, temp ; store new checksum value
	ldi	ZH, high (EFEDS_buffer + 6) ; 6 is the header size
	ldi	ZL, low  (EFEDS_buffer + 6)
	ldi	temp, 0x0d
	rcall	GetChecksum
	ldi	ZH, high (EFEDS_buffer + 0x13)  ; EFEDS checksum location
	ldi	ZL, low  (EFEDS_buffer + 0x13)
	st	Z, temp  ; store new checksum value

; send buffers
	ldi	ZH, high (EFEDS_buffer)
	ldi	ZL, low  (EFEDS_buffer)
SVtx0:	ld	temp, z+
	out	UDR, temp
SVtx1:	sbis	USR, UDRE
	rjmp	SVtx1
	cpi	temp, 0xf7
	breq	SVtx_ACED2
	rjmp	SVtx0
SVtx_ACED2:
	rcall	ButtonPause
	ldi	ZH, high (ACED2block * 2)
	ldi	ZL, low  (ACED2block * 2)
SVtx2:	lpm
	mov	temp, r0
	out	UDR, temp
SVtx3:	sbis	USR, UDRE
	rjmp	SVtx3
	cpi	temp, 0xf7
	breq	SVtx_ACED
	adiw	ZH:ZL, 1
	rjmp	SVtx2
SVtx_ACED:
	rcall	ButtonPause
	ldi	ZH, high (ACED_buffer)
	ldi	ZL, low  (ACED_buffer)
SVtx4:	ld	temp, z+
	out	UDR, temp
SVtx5:	sbis	USR, UDRE
	rjmp	SVtx5
	cpi	temp, 0xf7
	breq	SVtx_VCED
	rjmp	SVtx4
SVtx_VCED:
	rcall	ButtonPause
	ldi	ZH, high (VCED_buffer)
	ldi	ZL, low  (VCED_buffer)
SVtx6:	ld	temp, z+
	out	UDR, temp
SVtx7:	sbis	USR, UDRE
	rjmp	SVtx7
	cpi	temp, 0xf7
	breq	SVsw
	rjmp	SVtx6

SVsw:	rcall	ButtonPause
	sbrc	GeneralFlags, TX81_TQ5 ; 1=TX81Z 0=TQ5
	rjmp	TXswitchpress

; virtual switch presses to store voice into RAM for TQ5
	ldi	ZH, high (TQSwitchChnHdr * 2)
	ldi	ZL, low  (TQSwitchChnHdr * 2)
	ldi	temp, TQ5_Store
	mov	rBin1L, temp	; TQ5 Store button on front panel pressed
	ldi	temp, ButtonPressed
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

	ldi	ZH, high (TQSwitchChnHdr * 2)
	ldi	ZL, low  (TQSwitchChnHdr * 2)
	ldi	temp, TQ5_Store
	mov	rBin1L, temp	; TQ5 Store button on front panel released
	ldi	temp, ButtonReleased
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

	ldi	ZH, high (TQSwitchChnHdr * 2)
	ldi	ZL, low  (TQSwitchChnHdr * 2)
	ldi	temp, TQ5_SecondPlus
	mov	rBin1L, temp	; TQ5 Left Center button pressed
	ldi	temp, ButtonPressed
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

	ldi	ZH, high (TQSwitchChnHdr * 2)
	ldi	ZL, low  (TQSwitchChnHdr * 2)
	ldi	temp, TQ5_SecondPlus
	mov	rBin1L, temp	; TQ5 Left Center button released
	ldi	temp, ButtonReleased
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

	ldi	ZH, high (TQSwitchChnHdr * 2)
	ldi	ZL, low  (TQSwitchChnHdr * 2)
	ldi	temp, TQ5_SecondPlus ; second press confirm "Sure?"
	mov	rBin1L, temp	; TQ5 Left Center button pressed
	ldi	temp, ButtonPressed
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

	ldi	ZH, high (TQSwitchChnHdr * 2)
	ldi	ZL, low  (TQSwitchChnHdr * 2)
	ldi	temp, TQ5_SecondPlus
	mov	rBin1L, temp	; TQ5 Left Center button released
	ldi	temp, ButtonReleased
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

	ldi	ZH, high (TQSwitchChnHdr * 2)
	ldi	ZL, low  (TQSwitchChnHdr * 2)
	ldi	temp, TQ5_User
	mov	rBin1L, temp	; TQ5 User button pressed
	ldi	temp, ButtonPressed
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

	ldi	ZH, high (TQSwitchChnHdr * 2)
	ldi	ZL, low  (TQSwitchChnHdr * 2)
	ldi	temp, TQ5_User
	mov	rBin1L, temp	; TQ5 User button released
	ldi	temp, ButtonReleased
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

SVtx_exit:
	ret

TXswitchpress: ; virtual switch presses to store voice into RAM for TX81Z
	ldi	ZH, high (TXswitchChnHdr * 2)
	ldi	ZL, low  (TXswitchChnHdr * 2)
	ldi	temp, TX81Z_Store
	mov	rBin1L, temp	; Tx81Z Store button pressed and held
	ldi	temp, ButtonPressed
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

	ldi	ZH, high (TXswitchChnHdr * 2)
	ldi	ZL, low  (TXswitchChnHdr * 2)
	ldi	temp, TX81Z_DataPlus
	mov	rBin1L, temp	; Tx81Z DataPlus button pressed
	ldi	temp, ButtonPressed
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

	ldi	ZH, high (TXswitchChnHdr * 2)
	ldi	ZL, low  (TXswitchChnHdr * 2)
	ldi	temp, TX81Z_DataPlus
	mov	rBin1L, temp	; Tx81Z DataPlus button released
	ldi	temp, ButtonReleased
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

	ldi	ZH, high (TXswitchChnHdr * 2)
	ldi	ZL, low  (TXswitchChnHdr * 2)
	ldi	temp, TX81Z_Store
	mov	rBin1L, temp	; Tx81Z Store button released
	ldi	temp, ButtonReleased
	mov	rBin1H, temp
	rcall	DoButton
	rcall	ButtonPause

DoButton: ; simulate button press/release on Yamaha FM Synth
; send Button number in rBin1L  on/off in rBin1H
DBn0:	lpm	; r0 has sysex message byte for button press header
	mov	temp3, r0
	cpi	temp3, 0xff
	breq	DBn1
	sts	MIDIout, temp3
	rcall	PutMIDIbyte
	adiw	ZH:ZL, 1
	rjmp	DBn0
DBn1:	sts	MIDIout, rBin1L
	rcall	PutMIDIbyte
	sts	MIDIout, rBin1H
	rcall	PutMIDIbyte
	ldi	temp, 0xf7
	sts	MIDIout, temp
	rcall	PutMIDIbyte
	ret

ButtonPause:
	lds	temp, T1counter
	subi	temp, -1
BPs0:	lds	temp2, T1counter
	cp	temp, temp2
	brne	BPs0
	ret


;**************************************************************************************************
; GetChecksum:   To calculate the checksum:
;	1> Add all data bytes SysEx buffer.  exclude header bytes
;	2> OR 16-bit sum  with 0x007f
;	3> invert bits
;	4> add one to bit inversion result
;	5> OR with 0x7f
;
;  send:	ZH:ZL = buffer start location; Temp = number of bytes in checksum range
;  return:	Temp = checksum of range
;  uses:	rBin1L/rBin1H as sum registers, temp2 as zero for 16-bit addition
;
;**************************************************************************************************
GetChecksum:
	clr	rBin1L
	clr	rBin1H
	lds	temp2, ZeroLoc
GCk0:	ld	temp3, z+	; get the byte from the SRAM buffer
	add	rBin1L, temp3	; add the byte value to the 16-bit sum
	adc	rBin1H, temp2
	dec	temp
	brne	GCk0
	mov	temp, rBin1L	; discard the high byte of the 16-bit sum
	andi	temp, 0x7f
	com	temp		; flip the bits  one's complement
	inc	temp		; two's complement
	andi	temp, 0x7f	; 7 bit checksum result
	ret



;**********************************************************************************************
;  playMIDInote    subroutine
;   receives:  DecodeValue (holds note number without Octave Offset)
;**********************************************************************************************
playMIDInote:
	ldi	ZH, high (SoundingNotes)
	ldi	ZL, low  (SoundingNotes)
	ldi	Temp2, POSSIBLE_NOTES
PMN1:	ld	temp, z
	cp	temp, DecodeValue
; Found identical note number, which means that the Note had been already playing.
; The key was held down long enough trigger typematic auto-repeat.  Do nothing and exit.
	breq	PMN_EXIT
	inc	zl  ; first check 8 storage registers for same DecodeValue to see if the note is already playing
	dec	Temp2
	breq	NewNote  ;DecodeValue not found in list of notes that are currently playing
	rjmp	PMN1
NewNote: 	; now check if there is space in the note-currently-playing list for a new note
	ldi	ZH, high (SoundingNotes)
	ldi	ZL, low  (SoundingNotes)
	ldi	Temp2, POSSIBLE_NOTES
PMN2:	ld	temp, z  ; load the value of the dereferenced slot in the currently-playing list
	cpi	temp, REG_AVAILABLE  ; check if a note value is occupying this space in the currently-playing list
	breq	PMN3 ; selected slot filled with a playing note, so check the next slot
	inc	zl
	dec	Temp2
	breq	PMN_EXIT ; no space for the new note was found in the note-currently-playing list
	rjmp	PMN2
PMN3:	st	z, DecodeValue ; now put the DecodeValue (note value minus octave offset) into the currently-playing list
;----------- set up Note-On MIDI message
	ldi	temp, NOTE_ON_MSG
	sts	MIDIout, temp
	rcall	PutMIDIbyte
; adding the OctaveOffset value after putting the note in the currently-playing list allows the note
; to stay on after changing the octave range with the up/down arrows.
	lds	temp, OctaveOffset
	add	DecodeValue, temp
	sts	MIDIOut, DecodeValue
	rcall	PutMIDIbyte
	ldi	temp, VELOCITY
	sts	MIDIout, temp
	rcall	PutMIDIbyte
PMN_EXIT:
	ret



;**********************************************************************************************
;. TurnOffMIDInote
;   receives: DecodeValue
;   registers used: Temp2
;**********************************************************************************************
TurnOffMIDInote:
	ldi	ZH, high (SoundingNotes)
	ldi	ZL, low  (SoundingNotes) ; look for note in the playing_note table
	ldi	Temp2, POSSIBLE_NOTES
ToMN1:	ld	temp, z
	cp	temp, DecodeValue
	breq	ToMN2
	inc	zl
	dec	Temp2
	brne	ToMN1
	rcall	AllNotesOff
	rjmp	ToMN_exit

ToMN2: ; found note number in register storage
	ldi	temp, REG_AVAILABLE  ; make this slot in the currently-playing list available for a new note
	st	z, temp  ; let program that this reg is available
;;----------- set up Note-Off  MIDI message
	ldi	temp, NOTE_ON_MSG
	sts	MIDIout, temp
	rcall	PutMIDIbyte
	lds	temp, OctaveOffset
	add	DecodeValue, temp
	mov	temp, DecodeValue
	sts	MIDIout, temp
	rcall	PutMIDIbyte
	clr	temp
	sts	MIDIout, temp  ; velocity set to zero in order to turn note off
	rcall	PutMIDIbyte
ToMN_exit:
	ret



;*********************************************************************************************
;	Keypad digits   	Decode Value = 0x80 to 0x8f
;
; if a keypad digit is pressed then check if it is the first or second digit pressed.
; the first digit will have KPValue register equal to zero. 2nd digit will have bit 7 set and
; first digit press (the ten's column value in bits 3 to 0).
; Code arrives here with digit value of keypad press (plus 0x80) in DecodeValue register.
;*********************************************************************************************
DoDigit:
	sbrc	KeyFlags,LR_arrow_flag
	rjmp	DD_kluge
	mov 	temp, DecodeValue	; DV will be 0x80 to 0x8f
	andi	temp, 0b00001111
;  check if keypress is a digit.  If yes, then check if 1st or 2nd of voice number in base10
	cpi	temp, 10
	brlo	DD_digit		; yes it's a digit (0 to 9)
	rjmp	DD_Exit			; no, keypad press is not digit, so do nothing and exit

;  get two digits; the ten's value comes first, then the one's value
DD_digit: ; temp holds latest digit from keypad press0-9
	sbrc	KeyFlags, KPdigit_flag		; KeyFlags bit 0 determines if latest keypad digit is 1st or 2nd digit
	rjmp	DD_2Digit
	sts	KP1st, temp
	sbr	KeyFlags, (1<<KPdigit_flag)	; KeyFlags bit 0 acts as a flag for 1st or 2nd digit
	rjmp	DD_Exit

DD_2Digit:
; received 2nd digit, so multiply 1st digit (temp2) by 10 and add 2nd (temp)
	cbr	KeyFlags, (1<<KPdigit_flag)	; KeyFlags bit 0 cleared removes 2nd digit flag
	mov	temp3, temp
	lds	temp2, KP1st
	mov	temp, temp2
	lsl	temp2			; multiply first digit by ten
	lsl	temp2			; by multiplying by 2 three times and adding original value twice
	lsl	temp2			; bit 7 to carry, bit 0 cleared
	add	temp2, temp
	add	temp2, temp
	add	temp2, temp3		;  max value 99 decimal in temp
	sts	CurrentVoice, temp2
	rcall	DoProgramChange
	cbr	KeyFlags, (1<<KPdigit_flag)
	nop
DD_Exit:
	ret
DD_kluge:
	cbr	KeyFlags, (1<<LR_arrow_flag)
	ret



;**********************************************************************************************
;  AllNotesOff  1) Put Note_Available sentinel into the register bank for all possible notes.
;			All possible note spaces are available because all notes are off.
;		2) Send the All_Notes_Off  MIDI control message to the synthesizer.
;		3) jump back to Main routine
;  When the Hold1 is enabled (CapsLock key), then AllNotesOff doesn't work.
;*********************************************************************************************
AllNotesOff:  ;  send AllNotesOff message - 0xB0 0x7B 0x00
	rcall	ClearNoteTable
	ldi	temp, 0xfe  ; turn on active sense and then don't send 0xfe again.
	sts	MIDIout, temp ; after 300mS, the sound generator turns off.
	rcall	PutMIDIbyte
	ret



;**********************************************************************************************
;  Program Change MIDI Message
;
;  send:	CurrentVoice register, CurrentBank SRAM
;  return:	nothing
;**********************************************************************************************
DoProgramChange:
	rcall	ClearNoteTable
	ldi	temp, CHANGE_VOICE_MSG ; 0xc0
	sts	MIDIOut, temp
	rcall	PutMIDIbyte
	lds	temp, CurrentVoice
	sts	MIDIout, temp
	rcall	PutMIDIbyte
	rcall	LCD1msWait ; give TX81Z/TQ5 time to change voices internally
	rcall	Get_VCED_Data
	rcall	LCD1msWait
	rcall	Get_ACED_Data
	rcall	LCD1msWait
	rcall	Get_EFEDS_Data
	rcall	LCD1msWait
	rcall	LCD1msWait

	rcall	NewParameterSelected
	ret



;**********************************************************************************************
;  ClearNoteTable: set all values in note table to 0xff
;
;  send:	SoundingNotes buffer
;  return:	nothing
;  uses: ZH:ZL, Temp2, temp
;**********************************************************************************************
ClearNoteTable:
	ldi	ZH, high (SoundingNotes)
	ldi	ZL, low  (SoundingNotes)  ; clear the note storage regs because changing voice turns off all notes
	ldi	Temp2, POSSIBLE_NOTES
	ser	temp
CNT1: 	st	z+, temp
	dec	Temp2
	brne	CNT1 ; make all storage registers available for new notes when voice changes
	ret



;*****************************************************************************************************************
;  DECODE_DATA  -- routine to convert the scancode from the keyboard.   W-Or algorythm #1
;	input:	 scancode  register    [valid when NewScan flag is set by GetScan IRQ]
;	output:  DecodeValue, Keyflags registers
;	uses:    temp, KeyFlags
;
;	DecodeValue  < 0x3f  keyboard note (octave offset added to get MIDI note value)
;	DecodeValue  = 0x40 - 0x4f   function key
;	DecodeValue  = 0x80 - 0x8f   keypad press
;	DecodeValue  = 0xc0	---  space (All Notes Off)
;	DecodeValue  = 0xff   filler value - scancode ignored
;
;   A key pressed down will send the one-byte scancode to the PC for most characters.
;   When the key is released, the keyboard sends an 0xf0 byte followed by the scancode of the key being released.
;   This routine uses a flag called Break to determine if the scancode comes from a keypress down or release.
;	Break == clear means that scancode just received is a keypress down and should be decoded.
;	Break == set means that the previous scancode byte received was 0xf0 and the next scancode is the key going up.
;
;	Shift keys are ignored
;
;    The arrow keys are the only extended keys (first byte 0xe0) decoded.  Each of their routines are short
;   and therefore they are included in-line in this DECODE_DATA subroutine.
;*****************************************************************************************************************
Decode_Data:
	sbrc	KeyFlags, Break_flag   ; the previous scancode was 0xf0 when break_flag is set => key released
	rjmp	Break_set

;   Break is clear, so do check the four special conditions: 0xF0[break sentinel], 0x11[left ALT], 0x58[CapsLock], 0xe0[extended sentinel]

; if the Extended_flag is set then extended char scancode was the last previously sent
	sbrs	KeyFlags, Extended_flag
	rjmp	isSpecialKeys
	cpi	temp, 0xf0  ; is this an extended key release?
	brne	toDoExChr   ; no, so determine which extended key is pressed
	sbr	KeyFlags, (1<<Break_flag) ; yes, set break flag and exit with 0xff
	rjmp	set_Ignore

isSpecialKeys:
	cpi	temp, 0xf0  ; 	0xF0 =Breakcode  finger-off-the-key identifier
	brne	isLeftAltKey

	sbr	KeyFlags, (1<<Break_flag)  ; set Break flag  so that the next scancode will not be seen a keypress down
	rjmp	set_Ignore

isLeftAltKey:
	cpi	temp, 0x11  ; left Alt key is being pressed down. Alt_flag gets set.  Next key should be Fn1 to Fn12.
	brne	isExtKey

	sbr	KeyFlags, (1<<Alt_flag)
	rjmp	set_Ignore

isExtKey: ; check for E0 flag
	cpi	temp, 0xe0  ;  0xe0  extended char table
	brne	isFn7;	isExtFlagSet  ; scancode is not one of the special cases

	sbr	KeyFlags, (1<<Extended_flag)
	rjmp	set_Ignore

toDoExChr:
	rjmp	Do_Ext_Char  ; determine the action to take for the extended keypress

;not 0xE0, or 0xF0, and no flags are set;  so check for Fn7. If not, then do a table lookup (from MIDIScanTable) of the scancode
isFn7:	cpi	temp, 0x83 ;  check for Fn7, the one key that has a scancode outside the 00-0x7f range
	brne	TLU0
	ldi	DecodeValue, 0x46;  decode for Fn7
	rjmp	exit_decode

; do a table lookup of the scancode
TLU0:	ldi	ZL, low  (2 * MIDIScanTable)
	ldi	ZH, high (2 * MIDIScanTable)
	add	ZL, temp
	brcc	TLU1
	inc	ZH
TLU1:	lpm
	mov	DecodeValue, R0
	cpi	DecodeValue, 0x40 ; check if it is a note key
	brsh	exit_decode; if not, then exit with value in DV register
	sbr	KeyFlags, (1<<OnOff_flag)  ; set the note on/off flag
	rjmp 	exit_decode

;  Break flag is set - this means the previous scancode was 0xf0 (a key release)
;   This scancode is for either an extended key or the number of the key being
;	released. If the scancode is in the EEPROM table, and is less than 0x3f, then turn OFF the MIDI note.
;   If the released key has an extended code, then the clear the break and the
;	extended flags, set DecodeValue to DoNothing value (0xff) and exit.

Break_set:
	sbrs	KeyFlags, Extended_flag
	rjmp	BrS_0

	cbr	KeyFlags, ( (1<<Break_flag) | (1<<Extended_flag) ) ; clear Break and extended flags
	rjmp	set_Ignore

BrS_0:	cpi	temp, 0x11 ; left Alt flag is released
	brne	OffNoteTableLookUp

	cbr	KeyFlags, ( (1<<Break_flag) | (1<<Alt_flag) ) ; turn off Alt key flag
	rjmp	set_Ignore

OffNoteTableLookUp:	;determine which note needs to be turned off. Arrives here from BrS_0
	cbr	KeyFlags, ((1<<Break_flag) | (1<<Extended_flag))
	ldi	ZL, low  (2 * MIDIScanTable)
	ldi	ZH, high (2 * MIDIScanTable)
	add	ZL, temp
	brcc	BrS_1
	inc	ZH
BrS_1:	lpm
	mov	DecodeValue, r0
	cpi	DecodeValue, 0x3f
	brsh	TLU3
	cbr	KeyFlags, (1<<OnOff_flag)   ; cause a Note-Off message by clearing the on/off flag
	rjmp	exit_decode
TLU3:	cbr	KeyFlags, (1<<Alt_flag) ; when in doubt, clear the flag

set_Ignore:
	ser	DecodeValue	; When Decode sub returns 0xff in DecodeValue reg, the main program ignores the scancode.
exit_decode:		;  DecodeData routine's single exit point
	ret

;;-------------------------------------------------------------------------------------------------
;  previous scancode was 0xe0, so this is an extended character code.
;  Program will come here when an extended key is pressed, not released
;  If scancode is found on ExtTable, then Decode_Value returns 0x90+offset (0x90-0x9d)
;-------------------------------------------------------------------------------------------------
Do_Ext_Char:   ;  (50 bytes including table)
	ldi	DecodeValue, 0x90
	ldi	ZH, high (extendedTable * 2)
	ldi	ZL,  low (extendedTable * 2)
E0_a:	lpm
	adiw	ZH:ZL, 1
	mov	temp2, r0
	cpi	temp2, 0x00  ; if zero, then reached the end of the table without finding extended scancode. Not possible.
	breq	ExCharNotFnd
	cp	temp2, temp ; temp has scancode of the extended keypress
	breq	FndExChar
	inc	DecodeValue
	rjmp	E0_a
FndExChar:
	cbr	keyflags, (1<<Extended_Flag)
	rjmp	exit_decode ; return with DecodeValue = 0x9n
ExCharNotFnd:
	rjmp	set_Ignore
; ***** end of DecodeData subroutine *******************




;*********************************************************************************
; ParseExtKey  - subroutine  - A key with extended code was pressed
;  called by the Main code to handle any keypress with an extended scancode.
;  enter with DecodeValue between 0x90 and 0x9f
;
;;********************************************************************************
ParseExtKey:
; 0x90 Up arrow
	andi	DecodeValue, 0x0f
	cpi	DecodeValue, 0  ; 0x90  (scancode 0x72) Up arrow - LOWER the octave range
	brne	PEK1		; not the Up arrow key
	lds	temp, OctaveOffset
	cpi	temp, 12		; can't send a MIDI note on message note value that is less than zero
	brlo	PEK0	; branch is same or higher (unsigned)
	subi	temp, 12		; number of semitone notes in a octave
	sts	OctaveOffset, temp
PEK0:	rjmp	PEK_exit

;; 0x91  (scancode 0x72) Down arrow, RAISE the octave range
PEK1:	cpi	DecodeValue, 1 ; 0x90  (scancode 0x72) Down arrow  - RAISE the octave range
	brne	PEK2
	lds	temp, OctaveOffset
	cpi	temp, 0x7f - 11	; 12 is number of semitones in an octave
	brlo	PEK1a
	rjmp	PEK_exit	; already in the highest octave range, so exit
PEK1a:	subi	temp, -12	; add (by subtracting minus number) one octave (12 semitones)
	sts	OctaveOffset, temp
	rjmp	PEK_exit

;; 0x92  (scancode 0x6b) left arrow - change voice to previous (lower number in sequence)
PEK2:	cpi	DecodeValue, 2
	brne	PEK3
	lds	temp, CurrentVoice
	dec	temp
	andi	temp, 0b01111111
	sts	CurrentVoice, temp
	sbr	KeyFlags, (1<<LR_arrow_flag)
	rcall	DoProgramChange
	rjmp	PEK_exit

;; 0x93  (scancode 0x74) right arrow  - change voice to next (higher number in sequence)
PEK3:	cpi	DecodeValue, 3
	brne	PEK4
	lds	temp, CurrentVoice
	inc	temp
	andi	temp, 0b01111111
	sts	CurrentVoice, temp
	sbr	KeyFlags, (1<<LR_arrow_flag)
	rcall	DoProgramChange
	rjmp	PEK_exit

;; 0x94  (scancode 0x11) Right Alt Key
PEK4:	cpi	DecodeValue, 4
	brne	PEK5
	rjmp	PEK_exit

;; 0x95   (scancode 0x14) right Control key
PEK5:	cpi	DecodeValue, 5
	brne	PEK6
	rjmp	PEK_exit

;; 0x96	scancode=0x70	; Insert key
PEK6:	cpi	DecodeValue, 6
	brne	PEK7
	rcall	StoreVoiceToSynth
	rjmp	PEK_exit

;; 0x97	scancode=0x6c	; Home key
PEK7:	cpi	DecodeValue, 7
	brne	PEK8
	rjmp	PEK_exit

;; 0x98	scancode=0x71	; Delete key
PEK8:	cpi	DecodeValue, 8
	brne	PEK9
	rjmp	PEK_exit

;; 0x99	scancode=0x69	; End key
PEK9:	cpi	DecodeValue, 9
	brne	PEKa
	rjmp	PEK_exit

;; 0x9a	scancode=0x7d	; PageUp key
PEKa:	cpi	DecodeValue, 0x0a
	brne	PEKb
	rcall	PrintParams
	rjmp	PEK_exit

;; 0x9b	scancode=0x7a	; PageDown key
PEKb:	cpi	DecodeValue, 0x0b
	brne	PEKc
	rjmp	PEK_exit

;; 0x9c scancode=0x5a	; KeyPad Enter key
PEKc:	cpi	DecodeValue, 0x0c
	brne	PEKd
	rjmp	PEK_exit

;; 0x9d	scancode=0x5a	; KeyPad divide key
PEKd:	cpi	DecodeValue, 0x0d
	brne	PEK_exit
	nop

PEK_exit:
	ret
;************* end of ExtKeyParse subroutine *******************************************


;;**********************************************************************************************
;; putMIDIbyte  -  UART TX IRQ version{using TxQueue}
;;
;;  This main code routine is the first half of the code needed to have a non-polling output UART.
;;  Interrupt-based UART allows the main code to not be checking the Data_Ready bit (UDRE) of the
;;  transmitter in order to know when the UART is ready to send another byte.
;;
;;  Send one byte only out the MIDI TX
;;  Put output byte into queue at tail position,
;;    increment tail, and turn on UDRE interrupt
;;
;;  send:   MIDIout (data byte for transmission)
;;  return: nothing
;;  uses:   TxQueue, Tx_Tail, YH, YL, temp
;;
;; *** AVR UART/USART definitions -- 90S8535 **********************************
;; UDR	= 0x0c  ; UART I/O Data Register
;; USR	= 0x0b  ; UART Control/Status Reg A   [UDRE: bit 5] UART Data Register Empty
;; UCR	= 0x0a  ; UART Control/Status Reg B   [UDRIE: bit 5] UDR_Empty Interrupt Enable
;; UBRR	= 0x09  ; UART high baud rate Reg
;; *** AVR UART/USART definitions -- Mega8*************************************
;; UDR	 = 0x0c ; USART I/O Data Register
;; UCSRA = 0x0b ; USART Control/Status Reg A  [UDRE: bit 5] USART Data Register Empty
;; UCSRB = 0x0a ; USART Control/Status Reg B  [UDRIE: bit 5] UDR_Empty Interrupt Enable
;; UBRRH = 0x20 ; USART high baud rate reg
;; ***----  Mega48/88  must use lds/sts to access I/O registers >> No USART in Tiny48
;; UDR0	  = 0xc6 ; - USART0 data register-
;; UBRR0L = 0xc4 ; - USART0 high baud rate register-
;; UBRR0H = 0xc5 ; - USART0 low baud rate register-
;; UCSR0C = 0xc2 ; - USART0 C control register-
;; UCSR0B = 0xc1 ; - USART0 B control register- [UDRIE0:bit 5] UDR_Empty Interrupt Enable
;; UCSR0A = 0xc0 ; - USART0 A control register- [UDRE0:bit 5] USART Data Register Empty
;;*****************************************************************************
PutMIDIbyte:
	cli
	push	ZH
	push	ZL
	ldi	ZH, high (TxQueue)
	ldi	ZL, low (TxQueue)
	add	ZL, Tx_Tail
	brcc	PMb_0
	inc	ZH
PMb_0:	lds	temp, MIDIout
	st	z, temp
	mov	temp, Tx_Tail
	inc	temp
	andi	temp, QUEUE_MASK   ; andi instruction needs high register (r16-r31)
	mov	Tx_Tail, temp

; ICE_8535
;	in	temp, UCR ;  enable UDRE interrupt
;	ori	temp, (1 << UDRIE)
;	out	UCR, temp

; Mega8
	in	temp, UCSRB ;  enable UDRE interrupt
	ori	temp, (1 << UDRIE)
	out	UCSRB, temp

	pop	ZL
	pop	ZH
	sei
	ret



;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
;	Interrupt routines
;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



;*******************************************************************************************************************************
; UDRE_IRQ:
;
;  This interrupt routine is the second half of the code needed to have a non-polling output UART.
;  Interrupt-based UART allows the main code to not be checking the Data_Ready bit (UDRE) of the
;  transmitter in order to know when the UART is ready to send another byte.
;
;  Set-up:  Put the byte that is to be send out of the MIDI port into the output queue
;  then enable the UDRE interrupt.  UDRE interrupt will immediately happen, unless the UART is
;  currently transmitting a character sequence and is not empty.
;  Head== the first byte in, becoming the first byte out.  Locates where bytes will be taken out of the queue.
;  Tail == later bytes in.  Locates where bytes will be put into the queue
;
; reset:   HT
; UDREoff  |
;  OutQ:   00  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f
;  ^^^ state of TxQueue when PutMIDIbyte gets called
;
;	   H   T  put b1 into RAM that Qtail points to (00),then increment tail (to 01) and enable UDREirq
; UDREon   |   |
;  OutQ:   b1  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f
;  ^^^ state of TxQueue when UDRE_toMIDI is entered.  PutMIDIByte routine places tranmit byte into TxQueue.
;
;
; Usually interrupt routines are enabled by the initialization code, and remain inactive until triggered.
; But the Data_Register_Empty IRQ starts as soon as it is enabled and runs until it is specifically disabled.
; When enabled, UDRE_toMIDI gets the byte pointed to by the queue head and sends it out of the USART by putting it
; into UDR.   Next it increments the head pointer to the next location in the queue.  Then it checks if the head pointer
; equals the tail pointer.  If yes, then there is no more new bytes from the PC to send to the MIDI, and it disables
; itself by turning off the interrupt enable (clearing UDRE).  If head is still behind the tail, then it will trigger again
; (as soon as the UDRE flag is set, and after one main code instruction) until the head equals the tail of the queue.
;
; ***M8*************************************************************
; UDR	 = 0x0c ; UDR - USART I/O Data Register
; UCSRA = 0x0b ; UCSR0A - USART Control and Status Register A
; UCSRB = 0x0a ; UCSR0B - USART Control and Status Register B
; ***M48/88**********************************************************
; UDR0	  = 0xc6 ; - USART0 data register
; UBRR0L = 0xc4 ; - USART0 high baud rate register
; UBRR0H = 0xc5 ; - USART0 low baud rate register
; UCSR0C = 0xc2 ; - USART0 C control register
; UCSR0B = 0xc1 ; - USART0 B control register
; UCSR0A = 0xc0 ; - USART0 A control register
;
; send:    TxQueue, Tx_Head, QUEUESIZE
; return:
; uses: SREG, ZH:ZL (saved), irqtemp, UCR, UDRIE {USART definitions}
;*******************************************************************************************************************************
UDRE_IRQ:
	in	SREGtemp, SREG
	push	ZH
	push	ZL
	ldi	ZH, high (TxQueue)
	ldi	ZL, low (TxQueue)
	add	ZL, Tx_Head ; Locates where the new byte will taken out of the queue.
	brcc	U2M0  ; this code doesn't check whether the Qhead equals Qtail, it assumes that QHead is not equal QTail.
	inc	ZH
U2M0:	ld	irqtemp, Z
	out	UDR, irqtemp ; send the byte, that is pointed to by QHead, to the USART

	mov	irqtemp, Tx_Head ; tail and head pointers in low registers.  ANDI uses high regs only.
	inc	irqtemp
	andi	irqtemp, QUEUE_MASK ; when at end of queue, the Head pointer rolls over to beginning of the queue
	mov	Tx_Head, irqtemp

	cpse	Tx_Tail, Tx_Head
	rjmp	U2M_exit

; ICE_8535
;	in	irqtemp, UCR ; turn off the transmitter to MIDI (this IRQ code)
;	andi	irqtemp,  ~ (1 << UDRIE) ; if head equals tail
;	out	UCR, irqtemp  ; no bytes in output queue to be sent

;   Mega8
	in	irqtemp, UCSRB ; turn off the transmitter to MIDI (this IRQ code)
	andi	irqtemp,  ~ (1 << UDRIE) ; if head equals tail
	out	UCSRB, irqtemp  ; no bytes in output queue to be sent

U2M_exit:
	pop	ZL
	pop	ZH
	out	SREG, SREGtemp
	reti



;*********************************************************
;  Timer1 Overflow Interrupt Code
;  New ADC for ParamPot and ValuePot 10 times a second
;*********************************************************
T1OVF_IRQ:
	in	SREGtemp, SREG
	ldi	irqtemp, ~( high (ADCcount)) ; write high first for 16-bit registers
	out	TCNT1H, irqtemp
	ldi	irqtemp, ~( low (ADCcount))
	out	TCNT1L, irqtemp
	sbr	GeneralFlags, (1<<Time4ADC)
	lds	irqtemp, T1Counter  ; T1Counter can be used to create long intervals since its roll-over period is @25 seconds
	inc	irqtemp
	sts	T1Counter, irqtemp
	out	SREG, SREGtemp
	reti



;*******************************************************************************************************
; GETSCAN_IRQ:  ; signal handler for external interrupt falling edge on INT0 or PinChange
;   called by falling transition of the PS2 keyboard clock signal
;   assemble the scancode from PS2 keyboard data signal- data valid when clock is low
;  22 cycles for mid-byte data; 20 start and parity bits; 25 for stop bit
;
; (  use Terminal font to see correctly )
;clk---|_|--|_|--|_|--|_|--|_|--|_|--|_|--|_|--|_|---|_|---|_|----------   (---- = logic high +5v)
;data-\___/-----\___/-----\___/-----\___/-----\___/--------------------
;    11   10    9    8    7    6    5    4    3     2     1 bitcount on falledge before dec
;   start    0    1    2    3    4    5    6    7     P   stop   data bit
;  Bitcnt 3 to 10 is data. start bit = 11, parity bit=bit 2;  stop bit = bit 1
;
;  External equates:  KBport, KBClockPin, INITIAL_BITCOUNT, PARITY_BITCOUNT,
;  Registers: SREGtemp, irqtemp, scanbits, KeyFlags
;  SRAM: scancode, bitcount
;*********************************************************************************************************
GetScan_IRQ: ; bitcount and scancode variables are in SRAM in order to save register usage
	in	SREGtemp, SREG
	lds	irqtemp, bitcount
	cpi 	irqtemp, INITIAL_BITCOUNT ; falling edge of the FIRST pulse from the keyboard -start bit so do nothing
	breq	GS0
	cpi	irqtemp, PARITY_BITCOUNT ; = 3  ; test for parity bit and stop bit
	brlo	GS0			; must use bitcount 3 for compare because branch tests only for lower
	sec
	sbis	KBport, KBdataPin	; set shift register bit if keyboard data bit is set
	clc	; if data from kbrd is 0, then clear bit 7 only and leave other bits unchanged
	ror	scanbits		; shift data right one bit - data bit 7 gets latest bit from keyboard
GS0:	dec 	irqtemp
	brne	GS1			; All bits received?
	sts	scancode, scanbits	; scanbits is the shift register for arriving data bits
	sbr	KeyFlags, (1<< NewScanCode) ; set flag to let main program know a new byte is ready
	ldi	irqtemp, INITIAL_BITCOUNT
GS1:	sts	bitcount, irqtemp
	out	SREG, SREGtemp
	reti



;**********************************************************************************************
; RxC_IRQ  --  new byte from PC in MIDI UART. Filter out Active Sense and MIDI clock
;
;**********************************************************************************************
RxC_IRQ:
	in	SREGtemp, SREG  ; SREG is always 0x3f in I/O address table, so OK to use IN instruction
	push	ZH ; interrupt can happen while main code is using ZH:ZL registers, so save them.
	push	ZL
	in	irqtemp, UDR
	cpi	irqtemp, 0xfe  ; filter out MIDI active sense
	breq	RxC_exit
	cpi	irqtemp, 0xf5  ; filter out MIDI clock
	breq	RxC_exit
	cpi	irqtemp, 0xff  ; stray falling-edge spike on RS232 line
	breq	RxC_exit
	ldi	ZH, high (Rx_Queue)  ;point to the where new data byte is to be placed in Rx_Queue
	ldi	ZL, low (Rx_Queue)
	add	ZL, Rx_Tail
	st	Z, irqtemp
	mov	irqtemp, Rx_Tail
	inc	irqtemp
	andi	irqtemp, QUEUE_MASK
	mov	Rx_Tail, irqtemp

RxC_exit:
	pop	ZL
	pop	ZH
	out	SREG, SREGtemp ; SREG is always 0x3f in I/O address table, so OK to use OUT instruction
	reti


;;*****************************************************************************************************
;;  Timer2 Overflow Interrupt Code for soft-UART Output
;;
;;  This code happens every 32 microseconds, which is the interval for UART baud rate.
;;  If the soft-UART is currently sending a UART byte, then the Output_Active flag
;;  will be set.  If this flag is set, then shift out the current bit of the UART byte.
;;  After the stop bit of the byte being transmitted, clear the Output_Active flag.
;;  This will signal this interrupt routine to only reload the timer overflow value and then exit.
;;
;;  IRQ is 19 cycles [2.4 uSec every 32uS, including overhead] if there is no UART being transmitted.
;;  About 36 cycles [4.5 uSec every 32uS] if a byte is being transmitted.
;data---\___/-----\___/-----\___/-----\___/-----\___/--------------------
;      10   9     8   7     6   5     4   3     2   1           bitcount on T2 entry; decrement on exit
;      start   0    1    2    3    4    5    6     7   stop     UART position
;;*******************************************************************************************************
T2OVF_IRQ:
	in	SREGtemp, SREG
	ldi	irqtemp, UARTreload
	out	TCNT2, irqtemp		; restore the timer count to[UART baud period
;
	sbrc	UARTcontrolReg, Out_Active	; check if currently sending a UART output byte.
	rjmp	Do_Output_Bit		; yes, jump to output handler.  If not, then exit interrupt
Tovfexit:
	out	SREG, SREGtemp 		; restore the state of the flags to pre-interrupt
	reti                     	; leave timer interrupt

Do_Output_Bit:   ; sending UART byte
	mov	irqtemp, UARTControlReg
	andi	irqtemp, 0b00001111 ;  seperate the UART Out bitcount from the Active flag
	cpi	irqtemp, 1
	breq	UARToutByteFinished ; UART output is completely finished, so shut down
	cpi	irqtemp, 10  ; UART output is at beginning, so make a start bit
	breq	DoLowBitOut

	lsr	UARTOut ; least significant bit is transmitted first; LSB goes into carry flag
	brcc	DoLowBitOut  ; set or clear the UART tx output port pin according to the carry flag
DoHighBitOut:
	sbi	UARTOUTPORT, UARTOUTPIN
	rjmp	PrepareNextIrq
DoLowBitOut:
	cbi	UARTOUTPort, UARTOUTPIN  ; pull outport pin low
PrepareNextIrq:
	dec	irqtemp; next interrupt the bit count will be one lower
	andi	UARTControlReg, 0b11110000 ; clear the old value in the bit count
	or	UARTControlReg, irqtemp ; insert the new value of the bit count
	rjmp	Tovfexit              ; jump to single exit point for interrupt routine

; Stop bit period is complete; the UART byte is finished being output.
UARTOutByteFinished:
	sbi	UARTOUTPORT, UARTOUTPIN ; make stop bit
	cbr	UARTControlReg, (1<<Out_Active) ;turn off active flag so the main program can set up the next byte out
	rjmp	Tovfexit            ; jump to single exit point for interrupt routine






;******************************************************************************
;     <<<  LCD Routines >>>>
;******************************************************************************
; LCD_init:								;** LCD
; LCDmsgFlash - subroutine to put message text from Flash on LCD	;** LCD
; LCD1msWait:								;** LCD
; SendLCDcommand: ; transmitting char is in Temp register		;** LCD
; SendLCDdata:								;** LCD
; LCDdigits - display numbers on LCD					;** LCD
; LCDdisplayParam: 							;** LCD
;									;** LCD
;---------------------------------------				;** LCD
;  Initialize the LCD display controller				;** LCD
;---------------------------------------				;** LCD
LCD_init:								;** LCD
	ldi	temp, 15  						;** LCD
initWait0:								;** LCD
	rcall	LCD1msWait						;** LCD
	dec	temp      						;** LCD
	brne	initWait0 						;** LCD
									;** LCD
	cbi	LCDport, RS  ; set to command/status register		;** LCD
	ldi	Temp, 0x30     						;** LCD
	rcall	Control_E_Blink						;** LCD
	ldi	temp, 5							;** LCD
initWait1:								;** LCD
	rcall	LCD1msWait						;** LCD
	dec	temp							;** LCD
	brne	initWait1						;** LCD
									;** LCD
	ldi	temp,0x38						;** LCD
	rcall	SendLCDcommand 						;** LCD
	rcall	LCD1msWait						;** LCD
									;** LCD
	ldi	temp, 0x30     						;** LCD
	rcall	Control_E_Blink						;** LCD
	ldi	temp, 5							;** LCD
initWait2:								;** LCD
	rcall	LCD1msWait						;** LCD
	dec	temp							;** LCD
	brne	initWait2						;** LCD
									;** LCD
	cbi	LCDport, E						;** LCD
	ldi	Temp, 0x30						;** LCD
	out	LCDport, temp  						;** LCD
	rcall	Control_E_Blink						;** LCD
									;** LCD
;Bit 5 set:  data length=4bits (bit4 clear), #lines=2 (bit 3 set),	;** LCD
;         font type=5x8 (bit 2 clear) 					;** LCD
	ldi	Temp, 0b00101000 ;0x28					;** LCD
	rcall	Control_E_Blink						;** LCD
									;** LCD
;Bit 2 set:  increment cursor (bit 1 set) when new char displayed	;** LCD
	ldi	Temp, 0b00000110 ;  0x06				;** LCD
	rcall	SendLCDcommand						;** LCD
	rcall	Blink							;** LCD
									;** LCD
;Bit 3 set:  display on (bit 2 set),					;** LCD
;       cursor off (bit 1 clr),cursor blink off (bit 0 clr)		;** LCD
	ldi	Temp, 0b00001100					;** LCD
	rcall	SendLCDcommand  					;** LCD
	rcall	Blink							;** LCD
									;** LCD
;Bit 1 set:  Return home  needed for display				;** LCD
	ldi	Temp, home_LCD ; 0x02					;** LCD
	rcall	SendLCDcommand						;** LCD
	ldi	temp, 2							;** LCD
initWait3:								;** LCD
	rcall	LCD1msWait						;** LCD
	dec	temp							;** LCD
	brne	initWait3						;** LCD
									;** LCD
;Bit 0 set:  clear display    						;** LCD
	ldi	Temp, clr_LCD ; 0x01 					;** LCD
	rcall	SendLCDcommand						;** LCD
	rcall	Blink							;** LCD
;*********								;** LCD
	ret								;** LCD
;----------------------- end of LCD_init    				;** LCD
									;** LCD
Control_E_Blink:            ; called by LCD_init			;** LCD
	out	LCDport, temp						;** LCD
	cbi	LCDport, E   						;** LCD
	rcall	Blink							;** LCD
	sbi	LCDport, E   						;** LCD
	rcall	Blink							;** LCD
	cbi	LCDport, E   						;** LCD
	rcall	Blink							;** LCD
	ret								;** LCD
									;** LCD
;---------------------------------------------				;** LCD
;  Send data to LCD display controller					;** LCD
;---------------------------------------------				;** LCD
SendLCDcommand: ; transmitting char is in Temp register			;** LCD
	push	temp2							;** LCD
 	cbi	LCDport, RS	; LCD command   			;** LCD
	rjmp	LCD_write						;** LCD
SendLCDdata:								;** LCD
	push	temp2    						;** LCD
	sbi	LCDport, RS	; LCD data				;** LCD
	nop	; both NOPs needed at 8MHz clock			;** LCD
	nop								;** LCD
LCD_write: 								;** LCD
	push	temp  	; 'temp' register holds char to be written 	;** LCD
	in	temp2, LCDport ; read output latch value, not pins 	;** LCD
	andi	temp2, 0b00000011 ; clear the bits for the LCD data	;** LCD
	andi	temp,  0b11110000 ; send Most sig four bits first  	;** LCD
	lsr	temp							;** LCD
	lsr	temp							;** LCD
	or	temp2, temp;  LCD data bits 7-4 in Temp2 bits 5-2	;** LCD
	out	LCDport, temp2						;** LCD
	rcall	E_Blink							;** LCD
	pop	temp							;** LCD
	andi	temp2, 0b00000011 ; clear the old data 			;** LCD
	swap	temp							;** LCD
	andi	temp,  0b11110000 ; send least sig four bits second	;** LCD
	lsr	temp							;** LCD
	lsr	temp							;** LCD
	or	temp2, temp   						;** LCD
	out	LCDport, temp2						;** LCD
	rcall	E_Blink							;** LCD
	pop	temp2  							;** LCD
	ret								;** LCD
									;** LCD
E_Blink:   								;** LCD
	sbi	LCDport, E						;** LCD
	rcall	Blink							;** LCD
	cbi	LCDport, E						;** LCD
	rcall	Blink							;** LCD
	ret								;** LCD
									;** LCD
									;** LCD
;***********************************************************************;** LCD
;LCDmsgFlash - subroutine to put message text from Flash on LCD		;** LCD
;									;** LCD
; send:	Temp Reg  -  holds LCD dynamic display address for first char   ;** LCD
;       ZL,ZH 	- holds address of message text (* 2) in Flash		;** LCD
;	Textstring ends with 0x00					;** LCD
;***********************************************************************;** LCD
LCDmsgFlash:								;** LCD
	ori	temp, 0b10000000					;** LCD
	rcall	SendLCDcommand ; set LCD cursor position		;** LCD
	rcall	LCD1msWait  						;** LCD
LCDmsgF0:								;** LCD
	lpm								;** LCD
	tst	r0							;** LCD
	breq	LCDmsgF_exit						;** LCD
	mov	temp, r0    						;** LCD
	rcall	sendLCDdata 						;** LCD
	inc	ZL							;** LCD
	brne	LCDmsgF0    						;** LCD
	inc	ZH							;** LCD
	rjmp	LCDmsgF0    						;** LCD
LCDmsgF_exit:								;** LCD
	ret  								;** LCD
									;** LCD
									;** LCD
;**************************************************************		;** LCD
; Subroutine wait loops							;** LCD
;**************************************************************		;** LCD
									;** LCD
LCD1msWait:								;** LCD
	push	temp 							;** LCD
	push	temp2							;** LCD
	ldi	temp, 15 ; 8360 cycles loop total			;** LCD
	clr	temp2	 ; 1 millisecond at 8MHz clk			;** LCD
LIW0:	dec 	temp2							;** LCD
	brne	LIW0 							;** LCD
	dec	temp 							;** LCD
	brne	LIW0 							;** LCD
	pop	temp2							;** LCD
	pop	temp 							;** LCD
	ret								;** LCD
									;** LCD
Blink:	push	Temp 							;** LCD
	ldi	temp, 180 ; 360 cycles    				;** LCD
blink1:	dec	Temp      ; @45 uSec delay				;** LCD
	brne	blink1							;** LCD
	pop	Temp  							;** LCD
	ret								;** LCD
									;** LCD
									;** LCD
;***********************************************************************;** LCD
; LCDdigits - display numbers on LCD					;** LCD
; send - ASCIIdigits(5 max), followed by zero (initialized as ZeroLoc)	;** LCD
;	 temp = LCD location for first digit				;** LCD
;	 temp2 = offset into ASCIIdigits (5 - #ofdigits displayed)	;** LCD
; returns - nothing							;** LCD
; uses - ZH,ZL     							;** LCD
;***********************************************************************;** LCD
LCDdigits:								;** LCD
	ldi	ZH, high (ASCIIdigits)					;** LCD
	ldi	ZL, low (ASCIIdigits)					;** LCD
	tst	temp2		 					;** LCD
	breq	Ld1							;** LCD
Ld0:	inc     ZL							;** LCD
	dec	temp2							;** LCD
	brne	Ld0							;** LCD
Ld1:	ori	temp, 0x80						;** LCD
	rcall	SendLCDcommand ; position the LCD cursor		;** LCD
	rcall	LCD1msWait						;** LCD
LD2: 	ld	temp, z+						;** LCD
	tst	temp							;** LCD
 	breq	Ld_exit							;** LCD
 	rcall	SendLCDdata 						;** LCD
 	rjmp	Ld2							;** LCD
Ld_exit:								;** LCD
	ldi	temp, 0x0c  						;** LCD
	rcall	SendLCDcommand						;** LCD
	ret								;** LCD
									;** LCD
									;** LCD
;***************************************************			;** LCD
; LCDdisplayParam							;** LCD
; send:  Temp with 0-255 value						;** LCD
; 	 Temp2 with LCD display memory location.    			;** LCD
;            Second row has 0x40 plus char position 			;** LCD
;	 Temp3 with number of digits to display  			;** LCD
;***************************************************			;** LCD
LCDdisplayParam: 							;** LCD
	mov	rBin1L, temp						;** LCD
	clr	rBin1H							;** LCD
	ldi	ZH, high (ASCIIdigits)					;** LCD
	ldi	ZL, low  (ASCIIdigits)					;** LCD
	rcall	Bin2ToAsc5 ; convert 16-bit binary to five digits	;** LCD
	; returns result in ASCIIdigits (five digits followed by 0x00)  ;** LCD
	mov	temp, temp2						;** LCD
	ldi	temp2, 5						;** LCD
	sub	temp2, temp3						;** LCD
	rcall	LCDdigits						;** LCD
	ret								;** LCD
									;** LCD
									;** LCD
;**************************************************************		;** LCD
; End of Subroutines for the LCD display				;** LCD
;**************************************************************		;** LCD




;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%% Math
;    Math Routines							;%% Math
;									;%% Math
; Bin2ToAsc5								;%% Math
; ==========								;%% Math
; converts a 16-bit-binary to a 5 digit ASCII-coded decimal		;%% Math
; In: 16-bit-binary in rBin1H:rBin1L, ZH:ZL points to 'ASCIIdigits'	;%% Math
;   in Static RAM, where the result goes to.				;%% Math
; Out: Z points to the beginning of the ASCII string,			;%% Math
;   16-bit number value in five bytes of ASCIIdigit buffer on return	;%% Math
;   leading zeros are filled with blanks				;%% Math
; Used registers: rBin1H:rBin1L (content is not changed),		;%% Math
;   rBin2H:rBin2L (content is changed), temp{highreg}			;%% Math
; Called subroutines: Bin2ToBcd5					;%% Math
;									;%% Math
Bin2ToAsc5: ;								;%% Math
	rcall	Bin2ToBcd5   ; convert binary to BCD			;%% Math
	ldi	temp, 4      ; Counter is 4 leading digits		;%% Math
	mov	rBin2L, temp						;%% Math
Bin2ToAsc5a:								;%% Math
	ld	temp, z      ; read a BCD digit				;%% Math
	tst	temp         ; check if leading zero			;%% Math
	brne	Bin2ToAsc5b  ; No, found digit >0			;%% Math
	ldi	temp,' '     ; overwrite with blank			;%% Math
	st	z+, temp     ; store and set to next position		;%% Math
	dec	rBin2L       ; decrement counter			;%% Math
	brne	Bin2ToAsc5a  ; further leading blanks			;%% Math
	ld	temp, z      ; Read the last BCD			;%% Math
Bin2ToAsc5b:								;%% Math
	inc	rBin2L       ; one more char				;%% Math
Bin2ToAsc5c:								;%% Math
	subi	temp,-'0'	; Add ASCII-0				;%% Math
	st	z+, temp	; store and inc pointer			;%% Math
	ld	temp,z		; read next char			;%% Math
	dec	rBin2L		; more chars?				;%% Math
	brne	Bin2ToAsc5c  ; yes, go on				;%% Math
	sbiw	ZL, 5	; Pointer to beginning of the BCD		;%% Math
	ret 		; done						;%% Math
;									;%% Math
          								;%% Math
; Bin2ToBcd5								;%% Math
; ==========								;%% Math
; converts a 16-bit-binary to a 5-digit-BCD				;%% Math
; In: 16-bit-binary in rBin1H:L, Z points to first digit		;%% Math
;   where the result goes to						;%% Math
; Out: 5-digit-BCD, Z points to first BCD-digit				;%% Math
; Used registers: rBin1H:L (unchanged), rBin2H:L (changed),		;%% Math
;   temp								;%% Math
; Called subroutines: Bin2ToDigit   					;%% Math
;									;%% Math
Bin2ToBcd5:								;%% Math
	push	rBin1H ; Save number					;%% Math
	push	rBin1L							;%% Math
	ldi	temp, HIGH (10000) ; Start with ten thousands		;%% Math
	mov	rBin2H,temp						;%% Math
	ldi	temp, LOW (10000) 					;%% Math
	mov	rBin2L,temp						;%% Math
	rcall	Bin2ToDigit	; Calculate digit			;%% Math
	ldi	temp, HIGH (1000) ; Next with thousands			;%% Math
	mov	rBin2H,temp     					;%% Math
	ldi	temp, LOW (1000)					;%% Math
	mov	rBin2L, temp    					;%% Math
	rcall	Bin2ToDigit	; Calculate digit			;%% Math
	ldi	temp, HIGH (100)  ; Next with hundreds			;%% Math
	mov	rBin2H, temp   						;%% Math
	ldi	temp, LOW (100)						;%% Math
	mov	rBin2L, temp	  					;%% Math
	rcall	Bin2ToDigit       ; Calculate digit			;%% Math
	ldi	temp, HIGH (10)   ; Next with tens 			;%% Math
	mov	rBin2H, temp  						;%% Math
	ldi	temp, LOW (10)						;%% Math
	mov	rBin2L, temp  						;%% Math
	rcall	Bin2ToDigit	; Calculate digit			;%% Math
	st	z,rBin1L	; Remainder are ones			;%% Math
	sbiw	ZL, 4		; Put pointer to first BCD		;%% Math
	pop	rBin1L		; Restore original binary 		;%% Math
	pop	rBin1H							;%% Math
	ret			; and return				;%% Math
									;%% Math
									;%% Math
; Bin2ToDigit								;%% Math
; ===========								;%% Math
; converts one decimal digit by continued subtraction of a		;%% Math
;   binary coded decimal						;%% Math
; Used by: Bin2ToBcd5, Bin2ToAsc5, Bin2ToAsc				;%% Math
; In: 16-bit-binary in rBin1H:L, binary coded decimal in		;%% Math
;   rBin2H:L, Z points to current BCD digit				;%% Math
; Out: Result in Z, Z incremented					;%% Math
; Used registers: rBin1H:L (holds remainder of the binary),		;%% Math
;   rBin2H:L (unchanged), temp						;%% Math
; Called subroutines: -							;%% Math
;									;%% Math
Bin2ToDigit:								;%% Math
	clr	temp		; digit count is zero			;%% Math
Bin2ToDigita:								;%% Math
	cp	rBin1H, rBin2H  ; Number bigger than decimal?		;%% Math
	brcs	Bin2ToDigitc    ; MSB smaller than decimal		;%% Math
	brne	Bin2ToDigitb    ; MSB bigger than decimal		;%% Math
	cp	rBin1L, rBin2L  ; LSB bigger or equal decimal		;%% Math
	brcs	Bin2ToDigitc    ; LSB smaller than decimal		;%% Math
Bin2ToDigitb:								;%% Math
	sub	rBin1L, rBin2L  ; Subtract LSB decimal			;%% Math
	sbc	rBin1H, rBin2H  ; Subtract MSB decimal			;%% Math
	inc	temp		; Increment digit count			;%% Math
	rjmp	Bin2ToDigita    ; Next loop				;%% Math
Bin2ToDigitc:								;%% Math
	st	z+, temp	; Save digit and increment		;%% Math
	ret 			; done					;%% Math
									;%% Math



; ------------------------------------------------------------------------------------------------------------
;  Program Data, MIDI messages, and Text strings
; ------------------------------------------------------------------------------------------------------------
; LCD displays	   0123456789abcdef
ClearLine:    .db "                ",0,0
Clear2Spaces: .db "  ",0,0
Clear5Spaces: .db "     ",0

; Headers for commands to the TX81Z/TQ5
TQProtectOff:   .db 0xF0, 0x43, 0x10, 0x24, 0xff,0
TXProtectOff:   .db 0xF0, 0x43, 0x10, 0x10, 0x7b, 0xff
; headers for single_parameter_change sys-ex message sent out to the TX81Z/TQ5
VCEDchngHdr:  .db 0xF0, 0x43, 0x10, 0b00010010, 0xff, 0
ACEDchngHdr:  .db 0xF0, 0x43, 0x10, 0b00010011, 0xff, 0
EFEDSchngHdr: .db 0xF0, 0x43, 0x10, 0b00100100, 0xff, 0
TQSwitchChnHdr: .db 0xF0, 0x43, 0x10, 0x24, 0xff, 0
TXSwitchChnHdr: .db 0xF0, 0x43, 0x10, 0x13, 0xff, 0

; SysEx messages that request the TX81Z/TQ5 to send bulk data
VCEDreq:      .db 0xf0, 0x43, 0x20, 0x03, 0xf7, 0x00 ; returns 101 bytes
ACEDreq:      .db 0xf0, 0x43, 0x20, 0x7e, 0x4c, 0x4d, 0x20, 0x20, 0x38, 0x39, 0x37, 0x36, 0x41, 0x45, 0xf7, 0
EFEDSreq:     .db 0xf0, 0x43, 0x20, 0x7e, 0x4c, 0x4d, 0x20, 0x20, 0x38, 0x30, 0x33, 0x36, 0x45, 0x46, 0xf7, 0

; After receiving the VCED data from the synth, use this table to load the Current_Voice SRAM table.
; Number is offset of param from beginning of the buffer.
VCED_dump_offset: ; Get_VCED_Data
;      AR,  D1R,  D2R,  D1L,  out,  Crs
.db  0x2d, 0x2e, 0x2f, 0x31, 0x37, 0x38 ; AR1,D1R1,D2R1,D1L1,out1,Crs1
.db  0x13, 0x14, 0x15, 0x17, 0x1d, 0x1e ; AR2,D1R2,D2R2,D1L2,out2,Crs2
.db  0x20, 0x21, 0x22, 0x24, 0x2a, 0x2b ; AR3,D1R3,D2R3,D1L3,out3,Crs3
.db  0x06, 0x07, 0x08, 0x0a, 0x10, 0x11 ; AR4,D1R4,D2R4,D1L4,out4,Crs4
.db  0x3a, 0x3b, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c ; feedback, algorythm, voicename0-9
.db  0xff, 0xff

; after receiving the ACED data from the synth, use this table to load the Current_Voice SRAM table
ACED_dump_offset:   ; Get_ACED_Data
.db  0x21, 0x22, 0x17, 0x18, 0x1c, 0x1d, 0x12, 0x13, 0xff, 0xff

; Table points to where the adjusted New-Param_Pot setting finds its associated parameter in the C_V table.
; when user selects a new param by changing the Param pot, get the current value
; for the selected param from the Current_Voice table by using this table of 32 offsets into the C_V table.
NewParamCVoffsets: ; NewParameterSelect
.db 0xff,0xff,0xff,0xff ; dummy sentinel values for frequency
.db 0x00,0x06,0x0c,0x12 ; AttackRate ops 1-4
.db 0x04,0x0a,0x10,0x16 ; OutputLevel  ops 1-4
.db 0x25,0x27,0x29,0x2b ; Oscillator Wave ops 1-4 [ACED]
.db 0x01,0x07,0x0d,0x13 ; Decay1Rate  ops 1-4
.db 0x02,0x08,0x0e,0x14 ; Decay2Rate  ops 1-4
.db 0x03,0x09,0x0f,0x15 ; Decay1Level ops 1-4
.db 0x19,0x2c,0x2d,0x2e ; fdbk, effect type,speed,bal

OpValC: ; operator 1,2,3,4 values for frequency
; offset of operator's coarse frequency value into Current_Voice parameter SRAM table
.db 0x05,0x0b,0x11,0x17 ; Freq Coarse VCED 0x32, 0x18, 0x25, 0x0b
OpValF: ; offset of operator's fine frequency value into Current_Voice parameter SRAM table
.db 0x24,0x26,0x28,0x2a ; Freq Fine ACED positions for 1,2,3,4 operator

NoFreqRatioStr: .db "-.--", 0, 0

;                0123456789abcdef
ProgID: .db     "TwoPot_Mega8 v46",0,0
NoSyn:  .db     " Keyboard Only  ",0,0
TQ5ID:  .db     "   Yamaha TQ5   ",0,0
TX81ID: .db     "  Yamaha TX81-Z ",0,0

; map the C_V offset to the ParamNumber sent by the Single_Param_Value sys-ex msg.
; Values in this table are offsets into the three input buffers for the selected parameter minus the header.  (and plus 3 for EFEDS)
CVtoParamNumber:
.db 0x27,0x28,0x29,0x2b,0x31,0x32 ; AR1,D1R1,D2R1,D1L1,Out1,Crs1  C_V:0x00 to C_V:0x05
.db 0x0d,0x0e,0x0f,0x11,0x17,0x18 ; AR2,D1R2,D2R2,D1L2,Out2,Crs2  C_V:0x06 to C_V:0x0b
.db 0x1a,0x1b,0x1c,0x1e,0x24,0x25 ; AR3,D1R3,D2R3,D1L3,Out3,Crs3  C_V:0x0c to C_V:0x11
.db 0x00,0x01,0x02,0x04,0x0a,0x0b ; AR4,D1R4,D2R4,D1L4,Out4,Crs4  C_V:0x12 to C_V:0x17
.db 0x34,0x35,0x4d,0x4e,0x4f,0x50 ; alg,fdbk,VN0,VN1,VN2,VN3      C_V:0x18 to C_V:0x1d
.db 0x51,0x52,0x53,0x54,0x55,0x56 ;  VN4,VN5,VN6,VN7,VN8,VN9      C_V:0x1e to C_V:0x23
.db 0x11,0x12,0x07,0x08,0x0c,0x0d,0x02,0x0d ; Fn1,Owave1,Fn2,Owave2,Fn2,Owave2,Fn2,Owave2  C_V:0x24 to C_V:0x2b
.db 0x04,0x05,0x06,0xff ; effect type, speed, balance             C_V:0x2c to C_V:0x2e

; Freq operator -to- new-single-value sys-ex msg ParamNumber
SingleMsgCrsNum:   .db 0x32,0x18,0x25,0x0b  ; VCED
SingleMsgFineNum:  .db 0x11,0x07,0x0c,0x02  ; ACED

ACED2block:
.db 0xf0, 0x43, 0x00, 0x7e, 0x00, 0x14, 0x4c, 0x4d, 0x20, 0x20
.db 0x38, 0x30, 0x32, 0x33, 0x41, 0x45, 0x03, 0x00, 0x32, 0x00
.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf7

Param_Max: ;  Parameter Maximum Value
.db 0xff, 0xff	;FqTblOffset1 	FqTblOffset2
.db 0xff, 0xff	;FqTblOffset3 	FqTblOffset4
.db 0x1f, 0x1f	;AttackRate1	AttackRate2
.db 0x1f, 0x1f	;AttackRate3  	AttackRate4
.db 0x63, 0x63	;OutputLevel1	OutputLevel2
.db 0x63, 0x63	;OutputLevel3 	OutputLevel4
.db 0x07, 0x07	;OscWave1     	OscWave2
.db 0x07, 0x07	;OscWave3	OscWave4
.db 0x1f, 0x1f	;Decay1Rt_1	Decay1Rt_2
.db 0x1f, 0x1f	;Decay1Rt_3 	Decay1Rt_4
.db 0x1f, 0x1f	;Decay2Rt_1	Decay2Rt_2
.db 0x1f, 0x1f	;Decay2Rt_3	Decay2Rt_4
.db 0x0f, 0x0f	;Decay1Lv_1	Decay1Lv_2
.db 0x0f, 0x0f	;Decay1Lv_3	Decay1Lv_4
.db 0x07, 0x0a	;Feedback	EffectType
.db 0x28, 0x63	;Effect_Spd	Effect_Bal

ParamText: ; operator voice parameter strings
;    0123456789
.db "Frq_1     "	;0x00
.db "Frq_2     "	;0x01
.db "Frq_3     "	;0x02
.db "Frq_4     "	;0x03
.db "AttackRt_1"	;0x04
.db "AttackRt_2"	;0x05
.db "AttackRt_3"	;0x06
.db "AttackRt_4"	;0x07
.db "OutLevel_1"	;0x08
.db "OutLevel_2"	;0x09
.db "OutLevel_3"	;0x0a
.db "OutLevel_4"	;0x0b
.db "Osc_Wave_1"	;0x0c
.db "Osc_Wave_2"	;0x0d
.db "Osc_Wave_3"	;0x0e
.db "Osc_Wave_4"	;0x0f
.db "Decay1Rt_1"	;0x10
.db "Decay1Rt_2"	;0x11
.db "Decay1Rt_3"	;0x12
.db "Decay1Rt_4"	;0x13
.db "Decay2Rt_1"	;0x14
.db "Decay2Rt_2"	;0x15
.db "Decay2Rt_3"	;0x16
.db "Decay2Rt_4"	;0x17
.db "Decay1Lv_1"	;0x18
.db "Decay1Lv_2"	;0x19
.db "Decay1Lv_3"	;0x1a
.db "Decay1Lv_4"	;0x1b
.db "Feedback  "	;0x1c
.db "EffectType"	;0x1d
.db "Effect_Spd"	;0x1e
.db "Effect_Bal"	;0x1f


;;---------------------------------------------------------------------------------------------------------------------------
;;   Each frequency ratio is a combination of a coarse (0 -63) and a fine (0-15) value. These two values are stored in seperate areas of
;; the internal voice ROM database and are altered by seperate MIDI sys-ex command messages.  The actual frequency ratio is not stored
;; in the database and gets determined and displayed by the TQ5 (and TX81-Z) system program.
;;   Each coarse frequency value has 16 associated fine freq values that define a seperate frequency ratio.  These fine freq values are
;; not stored in the database in order. Nor does each fine freq value in the database directly relate to the next or previous value.
;;   The tables for the two dimensional array below show the correspondence between each coarse and fine value in frequency order.
;
;; ** Getting the correct frequency string from the Coarse and Fine parameter values **
;; Start from byte 0 of the FreqCoarse table and compare the table value to the Coarse parameter value.
;; When found, compare the Fine parameter value to the same offset in the FreqFine table. If the same,
;; then the string at that offset in the FreqText table is correct.  If not the same then continue looking
;; for the next instance of the Coarse parameter value in the FreqCoarse table and check again until the
;; coarse and fine parameters match at the same offset.
;
FreqCoarse:	;     text :offset   text :offset  FreqFine
.db   0, 0	;   " 0.50":  0	" 0.56" ;  1	:  0,1
.db   0, 0	;   " 0.62":  2	" 0.68" ;  3	:  2,3
.db   1, 0	;   " 0.71":  4	" 0.75" ;  5	:  0,4
.db   2, 1	;   " 0.78":  6	" 0.79" ;  7	:  0,1
.db   0, 3	;   " 0.81":  8	" 0.87" ;  9	:  5,0
.db   2, 0	;   " 0.88": 10	" 0.93" ; 11	:  1,7
.db   1, 3	;   " 0.96": 12	" 0.97" ; 13	:  3,1
.db   2, 4	;   " 0.98": 14	" 1.00" ; 15	:  2,0
.db   1, 4	;   " 1.05": 16	" 1.06" ; 17	:  4,1
.db   2, 3	;   " 1.07": 18	" 1.08" ; 19	:  3,2
.db   4, 1	;   " 1.12": 20	" 1.14" ; 21	:  2,5
.db   2, 3	;   " 1.17": 22	" 1.18" ; 23	:  4,3
.db   1, 4	;   " 1.23": 24	" 1.25" ; 25	:  6,4
.db   2, 3	;   " 1.27": 26	" 1.29" ; 27	:  5,4
.db   4, 1	;   " 1.31": 28	" 1.32" ; 29	:  5,7
.db   2, 3	;   " 1.37": 30	" 1.40" ; 31	:  6,5
.db   5, 4	;   " 1.41": 32	" 1.43" ; 33	:  0,7
.db   2, 5	;   " 1.47": 34	" 1.49" ; 35	:  7,1
.db   4, 3	;   " 1.50": 36	" 1.51" ; 37	:  8,6
.db   4, 6	;   " 1.56": 38	" 1.57" ; 39	:  9,0
.db   5, 3	;   " 1.58": 40	" 1.62" ; 41	:  2,6
.db   6, 5	;   " 1.66": 42	" 1.67" ; 43	:  1,3
.db   4, 7	;   " 1.68": 44	" 1.73" ; 45	: 11,0
.db   4, 6	;   " 1.75": 46	" 1.76" ; 47	: 12,2
.db   4, 7	;   " 1.81": 48	" 1.83" ; 49	: 13,1
.db   5, 6	;   " 1.85": 50	" 1.86" ; 51	:  5,3
.db   4, 5	;   " 1.87": 52	" 1.93" ; 53	: 14,6
.db   7, 6	;   " 1.94": 54	" 1.96" ; 55	:  2,4
.db   8, 5	;   " 2.00": 56	" 2.02" ; 57	:  0,7
.db   7, 8	;   " 2.05": 58	" 2.06" ; 59	:  3,1
.db   5, 8	;   " 2.11": 60	" 2.12" ; 61	:  8,2
.db   6, 7	;   " 2.15": 62	" 2.16" ; 63	:  6,4
.db   8, 5	;   " 2.18": 64	" 2.20" ; 65	:  3,9
.db   8, 7	;   " 2.25": 66	" 2.27" ; 67	:  4,5
.db   5, 8	;   " 2.29": 68	" 2.31" ; 69	: 10,5
.db   6, 7	;   " 2.35": 70	" 2.37" ; 71	:  8,6
.db   8, 6	;   " 2.43": 72	" 2.45" ; 73	:  7,9
.db   5, 7	;   " 2.46": 74	" 2.48" ; 75	: 12,7
.db   8, 6	;   " 2.50": 76	" 2.55" ; 77	:  8,10
.db   8, 7	;   " 2.56": 78	" 2.59" ; 79	:  9,8
.db   8, 6	;   " 2.62": 80	" 2.64" ; 81	: 10,11
.db   8, 7	;   " 2.68": 82	" 2.70" ; 83	: 11,9
.db   5, 6	;   " 2.73": 84	" 2.74" ; 85	: 15,12
.db   8, 7	;   " 2.75": 86	" 2.81" ; 87	: 12,10
.db   9, 6	;   " 2.82": 88	" 2.84" ; 89	:  0,13
.db   8, 9	;   " 2.87": 90	" 2.90" ; 91	: 14,1
.db   7, 8	;   " 2.91": 92	" 2.93" ; 93	: 11,15
.db   6, 9	;   " 2.94": 94	" 2.99" ; 95	: 14,2
.db  10, 7	;   " 3.00": 96	" 3.02" ; 97	:  0,12
.db   6,10	;   " 3.04": 98	" 3.06" ; 99	: 15,1
.db   9,10	;   " 3.08":100	" 3.12" ;101	:  3,2
.db   7,11	;   " 3.13":102	" 3.14" ;103	: 13,0
.db   9,10	;   " 3.17":104	" 3.18" ;105	:  4,3
.db  11, 7	;   " 3.23":106	" 3.24" ;107	:  1,14
.db  10, 9	;   " 3.25":108	" 3.26" ;109	:  4,5
.db  10,11	;   " 3.31":110	" 3.33" ;111	:  5,2
.db   9, 7	;   " 3.34":112	" 3.35" ;113	:  6,15
.db  10,11	;   " 3.37":114	" 3.43" ;115	:  6,3
.db  12,10	;   " 3.46":116	" 3.50" ;117	:  0,8
.db   9,11	;   " 3.52":118	" 3.53" ;119	:  8,4
.db  12, 9	;   " 3.56":120	" 3.61" ;121	:  1,9
.db  10,11	;   " 3.62":122	" 3.63" ;123	: 10,5
.db  12,10	;   " 3.67":124	" 3.68" ;125	:  2,11
.db   9,11	;   " 3.70":126	" 3.72" ;127	: 10,6
.db  10,12	;   " 3.75":128	" 3.78" ;129	: 12,3
.db  10,11	;   " 3.81":130	" 3.82" ;131	: 13,7
.db   9,12	;   " 3.87":132	" 3.89" ;133	: 12,4
.db  11,10	;   " 3.92":134	" 3.93" ;135	:  8,15
.db   9,13	;   " 3.96":136	" 4.00" ;137	: 13,0
.db  11, 9	;   " 4.02":138	" 4.05" ;139	:  9,14
.db  13,12	;   " 4.06":140	" 4.10" ;141	:  1,6
.db  13, 9	;   " 4.12":142	" 4.14" ;143	:  2,15
.db  13,12	;   " 4.18":144	" 4.21" ;145	:  3,7
.db  14,13	;   " 4.24":146	" 4.25" ;147	:  0,4
.db  14,12	;   " 4.31":148	" 4.32" ;149	:  1,8
.db  13,14	;   " 4.37":150	" 4.40" ;151	:  6,2
.db  11,13	;   " 4.41":152	" 4.43" ;153	: 13,7
.db  14,13	;   " 4.49":154	" 4.50" ;155	:  3,8
.db  11,12	;   " 4.51":156	" 4.54" ;157	: 14,10
.db  13,14	;   " 4.56":158	" 4.58" ;159	:  9,4
.db  11,13	;   " 4.61":160	" 4.62" ;161	: 15,10
.db  12,14	;   " 4.64":162	" 4.67" ;163	: 11,5
.db  13,15	;   " 4.68":164	" 4.71" ;165	: 11,0
.db  14,15	;   " 4.75":166	" 4.80" ;167	:  6,1
.db  13,14	;   " 4.81":168	" 4.84" ;169	: 13,7
.db  12,13	;   " 4.86":170	" 4.87" ;171	: 13,14
.db  15,14	;   " 4.90":172	" 4.93" ;173	:  2,8
.db  12,16	;   " 4.97":174	" 5.00" ;175	: 14,0
.db  14,16	;   " 5.02":176	" 5.06" ;177	:  9,1
.db  12,15	;   " 5.08":178	" 5.10" ;179	: 15,4
.db  14,16	;   " 5.11":180	" 5.12" ;181	: 10,2
.db  16,17	;   " 5.18":182	" 5.19" ;183	:  3,0
.db  15,16	;   " 5.20":184	" 5.25" ;185	:  5,4
.db  14,17	;   " 5.28":186	" 5.29" ;187	: 12,1
.db  16,16	;   " 5.31":188	" 5.37" ;189	:  5,6
.db  15,17	;   " 5.39":190	" 5.40" ;191	:  7,2
.db  16,14	;   " 5.43":192	" 5.46" ;193	:  7,14
.db  15,16	;   " 5.49":194	" 5.50" ;195	:  8,8
.db  17,14	;   " 5.51":196	" 5.55" ;197	:  3,15
.db  16,15	;   " 5.56":198	" 5.59" ;199	:  9,9
.db  17,18	;   " 5.62":200	" 5.65" ;201	:  4,0
.db  16,15	;   " 5.68":202	" 5.69" ;203	: 11,10
.db  18,17	;   " 5.72":204	" 5.73" ;205	:  1,5
.db  16,15	;   " 5.75":206	" 5.78" ;207	: 12,11
.db  18,17	;   " 5.81":208	" 5.83" ;209	:  2,6
.db  16,15	;   " 5.87":210	" 5.88" ;211	: 14,12
.db  18, 6	;   " 5.90":212	" 5.93" ;213	:  3,15
.db  17,15	;   " 5.94":214	" 5.98" ;215	:  7,13
.db  18,19	;   " 5.99":216	" 6.00" ;217	:  4,0
.db  17,19	;   " 6.05":218	" 6.06" ;219	:  8,1
.db  18,19	;   " 6.08":220	" 6.12" ;221	:  5,2
.db  18,19	;   " 6.16":222	" 6.18" ;223	:  6,3
.db  19,17	;   " 6.25":224	" 6.27" ;225	:  4,10
.db  20,19	;   " 6.28":226	" 6.31" ;227	:  0,5
.db  18,20	;   " 6.34":228	" 6.37" ;229	:  8,1
.db  19,20	;   " 6.43":230	" 6.47" ;231	:  7,2
.db  17,19	;   " 6.48":232	" 6.50" ;233	: 12,8
.db  18,19	;   " 6.52":234	" 6.56" ;235	: 10,9
.db  20,17	;   " 6.57":236	" 6.59" ;237	:  3,13
.db  18,19	;   " 6.60":238	" 6.62" ;239	: 11,10
.db  20,19	;   " 6.67":240	" 6.68" ;241	:  4,11
.db  18,17	;   " 6.69":242	" 6.70" ;243	: 12,14
.db  19,20	;   " 6.75":244	" 6.77" ;245	: 12,5
.db  18,19	;   " 6.78":246	" 6.81" ;247	: 13,13
.db  20,19	;   " 6.86":248	" 6.87" ;249	:  6,14
.db  21,19	;   " 6.92":250	" 6.93" ;251	:  0,15
.db  20,22	;   " 6.96":252	" 7.00" ;253	:  7,0
.db  21,22	;   " 7.02":254	" 7.06" ;255	:  1,1

FreqFine: 	;     text :offset  text :offset
.db   0, 1	;   " 0.50":  0	" 0.56" ;  1
.db   2, 3	;   " 0.62":  2	" 0.68" ;  3
.db   0, 4	;   " 0.71":  4	" 0.75" ;  5
.db   0, 1	;   " 0.78":  6	" 0.79" ;  7
.db   5, 0	;   " 0.81":  8	" 0.87" ;  9
.db   1, 7	;   " 0.88": 10	" 0.93" ; 11
.db   3, 1	;   " 0.96": 12	" 0.97" ; 13
.db   2, 0	;   " 0.98": 14	" 1.00" ; 15
.db   4, 1	;   " 1.05": 16	" 1.06" ; 17
.db   3, 2	;   " 1.07": 18	" 1.08" ; 19
.db   2, 5	;   " 1.12": 20	" 1.14" ; 21
.db   4, 3	;   " 1.17": 22	" 1.18" ; 23
.db   6, 4	;   " 1.23": 24	" 1.25" ; 25
.db   5, 4	;   " 1.27": 26	" 1.29" ; 27
.db   5, 7	;   " 1.31": 28	" 1.32" ; 29
.db   6, 5	;   " 1.37": 30	" 1.40" ; 31
.db   0, 7	;   " 1.41": 32	" 1.43" ; 33
.db   7, 1	;   " 1.47": 34	" 1.49" ; 35
.db   8, 6	;   " 1.50": 36	" 1.51" ; 37
.db   9, 0	;   " 1.56": 38	" 1.57" ; 39
.db   2, 6	;   " 1.58": 40	" 1.62" ; 41
.db   1, 3	;   " 1.66": 42	" 1.67" ; 43
.db  11, 0	;   " 1.68": 44	" 1.73" ; 45
.db  12, 2	;   " 1.75": 46	" 1.76" ; 47
.db  13, 1	;   " 1.81": 48	" 1.83" ; 49
.db   5, 3	;   " 1.85": 50	" 1.86" ; 51
.db  14, 6	;   " 1.87": 52	" 1.93" ; 53
.db   2, 4	;   " 1.94": 54	" 1.96" ; 55
.db   0, 7	;   " 2.00": 56	" 2.02" ; 57
.db   3, 1	;   " 2.05": 58	" 2.06" ; 59
.db   8, 2	;   " 2.11": 60	" 2.12" ; 61
.db   6, 4	;   " 2.15": 62	" 2.16" ; 63
.db   3, 9	;   " 2.18": 64	" 2.20" ; 65
.db   4, 5	;   " 2.25": 66	" 2.27" ; 67
.db  10, 5	;   " 2.29": 68	" 2.31" ; 69
.db   8, 6	;   " 2.35": 70	" 2.37" ; 71
.db   7, 9	;   " 2.43": 72	" 2.45" ; 73
.db  12, 7	;   " 2.46": 74	" 2.48" ; 75
.db   8,10	;   " 2.50": 76	" 2.55" ; 77
.db   9, 8	;   " 2.56": 78	" 2.59" ; 79
.db  10,11	;   " 2.62": 80	" 2.64" ; 81
.db  11, 9	;   " 2.68": 82	" 2.70" ; 83
.db  15,12	;   " 2.73": 84	" 2.74" ; 85
.db  12,10	;   " 2.75": 86	" 2.81" ; 87
.db   0,13	;   " 2.82": 88	" 2.84" ; 89
.db  14, 1	;   " 2.87": 90	" 2.90" ; 91
.db  11,15	;   " 2.91": 92	" 2.93" ; 93
.db  14, 2	;   " 2.94": 94	" 2.99" ; 95
.db   0,12	;   " 3.00": 96	" 3.02" ; 97
.db  15, 1	;   " 3.04": 98	" 3.06" ; 99
.db   3, 2	;   " 3.08":100	" 3.12" ;101
.db  13, 0	;   " 3.13":102	" 3.14" ;103
.db   4, 3	;   " 3.17":104	" 3.18" ;105
.db   1,14	;   " 3.23":106	" 3.24" ;107
.db   4, 5	;   " 3.25":108	" 3.26" ;109
.db   5, 2	;   " 3.31":110	" 3.33" ;111
.db   6,15	;   " 3.34":112	" 3.35" ;113
.db   6, 3	;   " 3.37":114	" 3.43" ;115
.db   0, 8	;   " 3.46":116	" 3.50" ;117
.db   8, 4	;   " 3.52":118	" 3.53" ;119
.db   1, 9	;   " 3.56":120	" 3.61" ;121
.db  10, 5	;   " 3.62":122	" 3.63" ;123
.db   2,11	;   " 3.67":124	" 3.68" ;125
.db  10, 6	;   " 3.70":126	" 3.72" ;127
.db  12, 3	;   " 3.75":128	" 3.78" ;129
.db  13, 7	;   " 3.81":130	" 3.82" ;131
.db  12, 4	;   " 3.87":132	" 3.89" ;133
.db   8,15	;   " 3.92":134	" 3.93" ;135
.db  13, 0	;   " 3.96":136	" 4.00" ;137
.db   9,14	;   " 4.02":138	" 4.05" ;139
.db   1, 6	;   " 4.06":140	" 4.10" ;141
.db   2,15	;   " 4.12":142	" 4.14" ;143
.db   3, 7	;   " 4.18":144	" 4.21" ;145
.db   0, 4	;   " 4.24":146	" 4.25" ;147
.db   1, 8	;   " 4.31":148	" 4.32" ;149
.db   6, 2	;   " 4.37":150	" 4.40" ;151
.db  13, 7	;   " 4.41":152	" 4.43" ;153
.db   3, 8	;   " 4.49":154	" 4.50" ;155
.db  14,10	;   " 4.51":156	" 4.54" ;157
.db   9, 4	;   " 4.56":158	" 4.58" ;159
.db  15,10	;   " 4.61":160	" 4.62" ;161
.db  11, 5	;   " 4.64":162	" 4.67" ;163
.db  11, 0	;   " 4.68":164	" 4.71" ;165
.db   6, 1	;   " 4.75":166	" 4.80" ;167
.db  13, 7	;   " 4.81":168	" 4.84" ;169
.db  13,14	;   " 4.86":170	" 4.87" ;171
.db   2, 8	;   " 4.90":172	" 4.93" ;173
.db  14, 0	;   " 4.97":174	" 5.00" ;175
.db   9, 1	;   " 5.02":176	" 5.06" ;177
.db  15, 4	;   " 5.08":178	" 5.10" ;179
.db  10, 2	;   " 5.11":180	" 5.12" ;181
.db   3, 0	;   " 5.18":182	" 5.19" ;183
.db   5, 4	;   " 5.20":184	" 5.25" ;185
.db  12, 1	;   " 5.28":186	" 5.29" ;187
.db   5, 6	;   " 5.31":188	" 5.37" ;189
.db   7, 2	;   " 5.39":190	" 5.40" ;191
.db   7,14	;   " 5.43":192	" 5.46" ;193
.db   8, 8	;   " 5.49":194	" 5.50" ;195
.db   3,15	;   " 5.51":196	" 5.55" ;197
.db   9, 9	;   " 5.56":198	" 5.59" ;199
.db   4, 0	;   " 5.62":200	" 5.65" ;201
.db  11,10	;   " 5.68":202	" 5.69" ;203
.db   1, 5	;   " 5.72":204	" 5.73" ;205
.db  12,11	;   " 5.75":206	" 5.78" ;207
.db   2, 6	;   " 5.81":208	" 5.83" ;209
.db  14,12	;   " 5.87":210	" 5.88" ;211
.db   3,15	;   " 5.90":212	" 5.93" ;213
.db   7,13	;   " 5.94":214	" 5.98" ;215
.db   4, 0	;   " 5.99":216	" 6.00" ;217
.db   8, 1	;   " 6.05":218	" 6.06" ;219
.db   5, 2	;   " 6.08":220	" 6.12" ;221
.db   6, 3	;   " 6.16":222	" 6.18" ;223
.db   4,10	;   " 6.25":224	" 6.27" ;225
.db   0, 5	;   " 6.28":226	" 6.31" ;227
.db   8, 1	;   " 6.34":228	" 6.37" ;229
.db   7, 2	;   " 6.43":230	" 6.47" ;231
.db  12, 8	;   " 6.48":232	" 6.50" ;233
.db  10, 9	;   " 6.52":234	" 6.56" ;235
.db   3,13	;   " 6.57":236	" 6.59" ;237
.db  11,10	;   " 6.60":238	" 6.62" ;239
.db   4,11	;   " 6.67":240	" 6.68" ;241
.db  12,14	;   " 6.69":242	" 6.70" ;243
.db  12, 5	;   " 6.75":244	" 6.77" ;245
.db  13,13	;   " 6.78":246	" 6.81" ;247
.db   6,14	;   " 6.86":248	" 6.87" ;249
.db   0,15	;   " 6.92":250	" 6.93" ;251
.db   7, 0	;   " 6.96":252	" 7.00" ;253
.db   1, 1	;   " 7.02":254	" 7.06" ;255

FreqText:
.db "0.50" ;   0
.db "0.56"  ;  1
.db "0.62" ;   2
.db "0.68"  ;  3
.db "0.71" ;   4
.db "0.75"  ;  5
.db "0.78" ;   6
.db "0.79"  ;  7
.db "0.81" ;   8
.db "0.87"  ;  9
.db "0.88" ;  10
.db "0.93"  ; 11
.db "0.96" ;  12
.db "0.97"  ; 13
.db "0.98" ;  14
.db "1.00"  ; 15
.db "1.05" ;  16
.db "1.06"  ; 17
.db "1.07" ;  18
.db "1.08"  ; 19
.db "1.12" ;  20
.db "1.14"  ; 21
.db "1.17" ;  22
.db "1.18"  ; 23
.db "1.23" ;  24
.db "1.25"  ; 25
.db "1.27" ;  26
.db "1.29"  ; 27
.db "1.31" ;  28
.db "1.32"  ; 29
.db "1.37" ;  30
.db "1.40"  ; 31
.db "1.41" ;  32
.db "1.43"  ; 33
.db "1.47" ;  34
.db "1.49"  ; 35
.db "1.50" ;  36
.db "1.51"  ; 37
.db "1.56" ;  38
.db "1.57"  ; 39
.db "1.58" ;  40
.db "1.62"  ; 41
.db "1.66" ;  42
.db "1.67"  ; 43
.db "1.68" ;  44
.db "1.73"  ; 45
.db "1.75" ;  46
.db "1.76"  ; 47
.db "1.81" ;  48
.db "1.83"  ; 49
.db "1.85" ;  50
.db "1.86"  ; 51
.db "1.87" ;  52
.db "1.93"  ; 53
.db "1.94" ;  54
.db "1.96"  ; 55
.db "2.00" ;  56
.db "2.02"  ; 57
.db "2.05" ;  58
.db "2.06"  ; 59
.db "2.11" ;  60
.db "2.12"  ; 61
.db "2.15" ;  62
.db "2.16"  ; 63
.db "2.18" ;  64
.db "2.20"  ; 65
.db "2.25" ;  66
.db "2.27"  ; 67
.db "2.29" ;  68
.db "2.31"  ; 69
.db "2.35" ;  70
.db "2.37"  ; 71
.db "2.43" ;  72
.db "2.45"  ; 73
.db "2.46" ;  74
.db "2.48"  ; 75
.db "2.50" ;  76
.db "2.55"  ; 77
.db "2.56" ;  78
.db "2.59"  ; 79
.db "2.62" ;  80
.db "2.64"  ; 81
.db "2.68" ;  82
.db "2.70"  ; 83
.db "2.73" ;  84
.db "2.74"  ; 85
.db "2.75" ;  86
.db "2.81"  ; 87
.db "2.82" ;  88
.db "2.84"  ; 89
.db "2.87" ;  90
.db "2.90"  ; 91
.db "2.91" ;  92
.db "2.93"  ; 93
.db "2.94" ;  94
.db "2.99"  ; 95
.db "3.00" ;  96
.db "3.02"  ; 97
.db "3.04" ;  98
.db "3.06"  ; 99
.db "3.08" ; 100
.db "3.12"  ;101
.db "3.13" ; 102
.db "3.14"  ;103
.db "3.17" ; 104
.db "3.18"  ;105
.db "3.23" ; 106
.db "3.24"  ;107
.db "3.25" ; 108
.db "3.26"  ;109
.db "3.31" ; 110
.db "3.33"  ;111
.db "3.34" ; 112
.db "3.35"  ;113
.db "3.37" ; 114
.db "3.43"  ;115
.db "3.46" ; 116
.db "3.50"  ;117
.db "3.52" ; 118
.db "3.53"  ;119
.db "3.56" ; 120
.db "3.61"  ;121
.db "3.62" ; 122
.db "3.63"  ;123
.db "3.67" ; 124
.db "3.68"  ;125
.db "3.70" ; 126
.db "3.72"  ;127
.db "3.75" ; 128
.db "3.78"  ;129
.db "3.81" ; 130
.db "3.82"  ;131
.db "3.87" ; 132
.db "3.89"  ;133
.db "3.92" ; 134
.db "3.93"  ;135
.db "3.96" ; 136
.db "4.00"  ;137
.db "4.02" ; 138
.db "4.05"  ;139
.db "4.06" ; 140
.db "4.10"  ;141
.db "4.12" ; 142
.db "4.14"  ;143
.db "4.18" ; 144
.db "4.21"  ;145
.db "4.24" ; 146
.db "4.25"  ;147
.db "4.31" ; 148
.db "4.32"  ;149
.db "4.37" ; 150
.db "4.40"  ;151
.db "4.41" ; 152
.db "4.43"  ;153
.db "4.49" ; 154
.db "4.50"  ;155
.db "4.51" ; 156
.db "4.54"  ;157
.db "4.56" ; 158
.db "4.58"  ;159
.db "4.61" ; 160
.db "4.62"  ;161
.db "4.64" ; 162
.db "4.67"  ;163
.db "4.68" ; 164
.db "4.71"  ;165
.db "4.75" ; 166
.db "4.80"  ;167
.db "4.81" ; 168
.db "4.84"  ;169
.db "4.86" ; 170
.db "4.87"  ;171
.db "4.90" ; 172
.db "4.93"  ;173
.db "4.97" ; 174
.db "5.00"  ;175
.db "5.02" ; 176
.db "5.06"  ;177
.db "5.08" ; 178
.db "5.10"  ;179
.db "5.11" ; 180
.db "5.12"  ;181
.db "5.18" ; 182
.db "5.19"  ;183
.db "5.20" ; 184
.db "5.25"  ;185
.db "5.28" ; 186
.db "5.29"  ;187
.db "5.31" ; 188
.db "5.37"  ;189
.db "5.39" ; 190
.db "5.40"  ;191
.db "5.43" ; 192
.db "5.46"  ;193
.db "5.49" ; 194
.db "5.50"  ;195
.db "5.51" ; 196
.db "5.55"  ;197
.db "5.56" ; 198
.db "5.59"  ;199
.db "5.62" ; 200
.db "5.65"  ;201
.db "5.68" ; 202
.db "5.69"  ;203
.db "5.72" ; 204
.db "5.73"  ;205
.db "5.75" ; 206
.db "5.78"  ;207
.db "5.81" ; 208
.db "5.83"  ;209
.db "5.87" ; 210
.db "5.88"  ;211
.db "5.90" ; 212
.db "5.93"  ;213
.db "5.94" ; 214
.db "5.98"  ;215
.db "5.99" ; 216
.db "6.00"  ;217
.db "6.05" ; 218
.db "6.06"  ;219
.db "6.08" ; 220
.db "6.12"  ;221
.db "6.16" ; 222
.db "6.18"  ;223
.db "6.25" ; 224
.db "6.27"  ;225
.db "6.28" ; 226
.db "6.31"  ;227
.db "6.34" ; 228
.db "6.37"  ;229
.db "6.43" ; 230
.db "6.47"  ;231
.db "6.48" ; 232
.db "6.50"  ;233
.db "6.52" ; 234
.db "6.56"  ;235
.db "6.57" ; 236
.db "6.59"  ;237
.db "6.60" ; 238
.db "6.62"  ;239
.db "6.67" ; 240
.db "6.68"  ;241
.db "6.69" ; 242
.db "6.70"  ;243
.db "6.75" ; 244
.db "6.77"  ;245
.db "6.78" ; 246
.db "6.81"  ;247
.db "6.86" ; 248
.db "6.87"  ;249
.db "6.92" ; 250
.db "6.93"  ;251
.db "6.96" ; 252
.db "7.00"  ;253
.db "7.02" ; 254
.db "7.06"  ;255

MIDIScanTable:
;    0      1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
.db 0xff,0x48,0xff,0x44,0x42,0x40,0x41,0x4b,0xff,0x49,0x47,0x45,0x43,   1,   0,0xff ; 00-0f
.db 0xff,0xff,  25,0xff,0xf1,   2,0xff,0xff,0xff,0xff,  26,  27,  24,   4,   3,0xff ; 10-1f
.db 0xff,  30,  28,  29,   6,0xff,   5,0xff,0xff,0xc0,  31,0xff,   9,   7,   8,0xff ; 20-2f
.db 0xff,  35,  33,  34,  32,  11,  10,0xff,0xff,0xff,  37,  36,  13,  12,0xff,0xff ; 30-3f
.db 0xff,  38,0xff,  14,  16,  17,  15,0xff,0xff,  40,  42,  39,  41,  18,0xff,0xff ; 40-4f
.db 0xff,0xff,  44,0xff,  19,  20,0xff,0xff,0xc1,  43,  45,  21,0xff,  23,0xff,0xff ; 50-5f
.db 0xff,0xff,0xff,0xff,0xff,0xff,  22,0xff,0xff,0x81,0xff,0x84,0x87,0xff,0xff,0xff ; 60-6f
.db 0x80,0x8e,0x82,0x85,0x86,0x88,0xf2,0x8a,0x4a,0x8d,0x83,0x8c,0x8b,0x89,0xf4,0xff ; 70-7f


ExtendedTable:
;     90     91       92      93      94      95      96      97      98       99      9a      9b      9c      9d <----Decode value returned
.db 0x75,   0x72,   0x6b,   0x74,   0x11,   0x14,   0x70,   0x6c,   0x71,    0x69,   0x7d,   0x7a,   0x5a,   0x4a,0,0; 0x90-0x9d
;	\________________________________________________________________________________________________________  down arrow
;		\________________________________________________________________________________________________  up arrow
;			\________________________________________________________________________________________  left  arrow
;				\________________________________________________________________________________  right arrow
;					\________________________________________________________________________  right Alt key
;						\________________________________________________________________  right Control key
;							\________________________________________________________  Insert key
;								\________________________________________________  Home key
;									\________________________________________  Delete key
;										\________________________________  End key
;											\________________________  PageUp key
;												\________________  PageDown key
;													\________  KeyPad Enter key
;														\  KeyPad divide


PrintText:
.db "AtR1: "
.db "D1R1: "
.db "D2R1: "
.db "D1L1: "
.db "Out1: "
.db "Crs1: "
.db "AtR2: "
.db "D1R2: "
.db "D2R2: "
.db "D1L2: "
.db "Out2: "
.db "Crs2: "
.db "AtR3: "
.db "D1R3: "
.db "D2R3: "
.db "D1L3: "
.db "Out3: "
.db "Crs3: "
.db "AtR4: "
.db "D1R4: "
.db "D2R4: "
.db "D1L4: "
.db "Out4: "
.db "Crs4: "
.db "Algr: "
.db "FbBk: "

PrintText2:
.db "Fin1: "   ; Current_Voice 0x24
.db "Wav1: "
.db "Fin2: "
.db "Wav2: "
.db "Fin3: "
.db "Wav3: "
.db "Fin4: "
.db "Wav4: "
.db "Etyp: "   ; Current_Voice 0x2c
.db "Espd: "   ; Current_Voice 0x2d
.db "Ebal: "   ; Current_Voice 0x2e


; *** in-code program indentification
;     "****  TP_M8_v4.asm  alan_probandt@yahoo.com "
.db   "****  PTM__84va.ms  lanap_orabdn@tayoh.ooc m"

