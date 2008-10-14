;  TP_mega88.asm  version 3.1   October 14, 2008
;  alan_probandt@yahoo.com   Target CPU: Atmel AVR Mega88
;
;  { Changes from previous ver: timeout for MIDI_input on Get_EFEDS_data ; 25mSec delay in Get_ACED_data }  
;
;  use Terminal or Lucinda font to format source code listing correctly
;
;  Real-Time controller for the popular Yamaha TX81-Z or TQ5 MIDI music synthesizer.
;  QWERTY PS2 keyboard plays notes; two potentiometers adjust 100+ sound parameters.
;  Information is displayed on a 16x2 LCD character display.
;  Cut-and-Paste assembler routines adaptable for other MIDI synths or programs.
;
;
;  Program Operation Overview
;-----------------------------
;  The main code checks repeatedly for a keypress from the PS2 keyboard. Most keys trigger
;  a MIDI Note-on message when pressed and Note-off when released.
;
;  Every 1/10 of a second, the main code interrupts and does a scan of two potentiometers.
;  The new values for each pot are compared to the values from the last set of conversions.
;  New values are sent to the synth using a MIDI sys-ex Change_Single_Parameter message.
;
;
;
;    Atmel AVR Mega88  [8K flash/512 bytes SRAM/512 bytes EEPROM]
;	       _______
;       reset/ |1  28| ADC5/SCL
;     MIDI RxD |2  27| ADC4/SDA
;     MIDI TxD |3  26| ADC3 Parameter_Select Pot
;    PS2 clock |4  25| ADC2 New_Value Pot
;    PS2  data |5  24| ADC1
;          PD4 |6  23| ADC0
;     +5V  VCC |7  22| GND
;          GND |8  21| AREF  +5V
;          PB6 |9  20| AVCC  +5V
;          PB7 |10 19| PB5  LCD_d7 (SCK)
;          PD5 |11 18| PB4  LCD_d6 (MISO)
;          PD6 |12 17| PB3  LCD_D5 (MOSI)
;          PD7 |13 16| PB2  LCD_d4
;  LCD_RS  PB0 |14 15| PB1  LCD_E
;              -------
;

.nolist
.include "m88def.inc"  ; ^^^^* Mega88 change
.list

; constants for specific AVR device type
.equ	SYSTEM_CLOCK	= 8000000
.equ	RAMSTART	= 0x100 ; for Mega8 and 90S8535     ; ^^^^* Mega88 change 
.equ	MIDIbaud	= ((SYSTEM_CLOCK/16) / 31250) - 1  ; 15  0.0% error for MIDI


;Definitions
;LCD equates
.equ	RS		= 0	 ; PB0 Register Select
.equ	E		= 1	 ; PB1 Enable  [LCDdata4-7 on PB2-5]
.equ	LCDport		= Portb	;  use PortC for 90S8535
.equ	LCDddr		= ddrb	;  use DDRC for 90S8535
;LCD instructions
.equ	clr_LCD		= 0b00000001	;clear display
.equ	home_LCD	= 0b00000010	;return home
.equ	entry_mode	= 0b00000110	;set Cursor
.equ	on_LCD		= 0b00001110	;LCD on

;MIDI equates
.equ	CONTROLLER_MSG  = 0xb0
.equ	PITCHBEND_MSG   = 0xe0
.equ	CHANGE_VOICE_MSG = 0xc0
.equ	ALLNOTESOFF_MSG	= 0x7f ; 0x7b normally, but 0x7f for TQ5
.equ	SUSTAIN_ON	= 0x7f
.equ	SUSTAIN_OFF	= 0x00
.equ	POSSIBLE_NOTES  = 8
.equ	OCTAVE_DEFAULT	= 34    ;Middle C is on 'Z' key. lowest QWERTY keynote is 26 steps below middle C [A#0]
.equ	SPACEBAR	= 0xc0
.equ	SUSTAINKEY	= 0xc1
.equ	REG_AVAILABLE	= 0xFF
.equ	VELOCITY  	= 0x50
.equ	NOTE_ON_MSG	= 0x90
.equ	NEW_PARAM_FLAG  = 7
.equ	QUEUESIZE	= 0x20
.equ	QUEUE_MASK	= QUEUESIZE - 1
.equ	MOSTSIGBIT	= 7
.equ	PARAMADCCHANNEL = 3
.equ	VALUEADCCHANNEL = 2
.equ	ALGVCEDPOSITION = 52 ; DoFunctionKey -  position of algorythm param in VCED dump after header removed
.equ	EOX		= 0xf7 ; end of MIDI sys-ex message or dump

;PS2 keyboard equates
.equ	KBport		= PinD
.equ	KBClockPin	= 2
.equ	KBdataPin	= 3
.equ	INITIAL_BITCOUNT = 11
.equ	PARITY_BITCOUNT  = 3

;* System Constants
.equ    ADCcount	= ((SYSTEM_CLOCK/1024) / 5) ;SYSTEM_CLOCK/1024=7812.5 Hz


;General lower registers   --  where used or referenced
.def	SREGtemp 	= r1
.def	scanbits	= r2	; shift register for holding PC keyboard data while being assembled
.def	MIDIOut		= r3
.def	Rx_Head		= r4
.def	Rx_Tail		= r5
.def 	Tx_Tail		= r6
.def	Tx_Head		= r7
.def	rBin1L		= r8
.def	rBin1H		= r9
.def	rBin2L		= r10
.def	rBin2H		= r11
.def	LowTemp1	= r12


;General upper registers   --  where used or referenced
.def	temp	 	= r16
.def	temp2	 	= r17
.def	temp3		= r18
.def	irqtemp	 	= r19
.def	KeyFlags 	= r20
; KeyFlags register's bit values
.equ	LR_arrow_flag	= 7
.equ	CapsStatus	= 6     ; Caps Lock pressed on -- no NoteOff transmitted
.equ	Alt_flag	= 5     ; left Alt key is being pressed
.equ	Break_flag	= 4     ; the last scancode was an 0xf0 - the break code sent when a key is released
.equ	OnOff_flag	= 3     ; set means turn on the note whose MIDI number was decoded from the most recent scancode
.equ	NewScanCode	= 2     ; a byte has been received from the keyboard
.equ	Extended_flag	= 1     ; the extended scancode 0xE0 was sent - use the E0table to decode scancode
.equ	KPdigit_flag	= 0     ; set if expecting the second digit from the keypad
.def	DecodeValue	= r21 	; PC keyboard char decoded to NoteOn value, or AllNotesOff cmd
.def	KP1stDigit	= r22
.def	KP2ndDigit	= r23
.def	GeneralFlags	= r24
.equ	Time4ADC	= 7
.equ	InputTimeOut	= 6

;  ZH:ZL r31:30    YH:YL r29:28    XH:XL r27:26



;****************************************
; SRAM variables
;****************************************
.dseg
SoundingNotes:	.byte POSSIBLE_NOTES ; 100~107
Prev_Param_Pot: .byte 1 ;108
Prev_Value_Pot:	.byte 1 ;109
New_Param_Pot:	.byte 1 ;10a
New_Value_Pot:	.byte 1 ;10b
BitCount: 	.byte 1 ;10c used by PC keyboard INT0 routine
scancode: 	.byte 1 ;10d storage of data bits received from PC keyboard
SysExStrH:	.byte 1 ;10e
SysExStrL:	.byte 1 ;10f
ParamNumber:	.byte 1 ;110
New_Value_toTQ:	.byte 1 ;111
NewFreqCoarse:  .byte 1 ;112
NewFreqFine:    .byte 1 ;113
KPvalue:	.byte 1 ;114 used to decode keypad digit presses-value stored between keypresses
ASCIIdigits:    .byte 5 ;115-79
ZeroLoc:	.byte 1 ;11a used by LCDdigits routine to display ASCII numbers
ADCstore:	.byte 1 ;11b
PrevPrmTx:	.byte 1 ;11c store the new value of the parameter being changed on the TX81Z/TQ5
Operator:	.byte 1 ;11d  bit 7 set if current parameter is a frequency value
FreqOffset:     .byte 1 ;11e
CurrentVoice:	.byte 1 ;11f

Rx_Queue:	.byte QUEUESIZE  ; 32 bytes ; 0x120 to 0x13f
SysEx_Buffer:	.byte 0x70 ; 0x140 to 0x1af


; Current Voice Operator Parameter Values
Current_Voice:
VCEDvalues: ;            dec hex  memloc
AttackRate4:	.byte 1 ; 00 00   0x1b0
Decay1Rate4:    .byte 1 ; 01 01   0x1b1
Decay2Rate4:    .byte 1 ; 02 02   0x1b2
ReleaseRate4:   .byte 1 ; 03 03   0x1b3
Decay1Level4:   .byte 1 ; 04 04   0x1b4
LevelSens4:     .byte 1 ; 05 05   0x1b5
RateSensv4:     .byte 1 ; 06 06   0x1b6
EnvBiasSens4:   .byte 1 ; 07 07   0x1b7
AmpModEnable4:  .byte 1 ; 08 08   0x1b8
KeyVelSens4:    .byte 1 ; 09 09   0x1b9
OutputLevel4:   .byte 1 ; 10 0a   0x1ba
CoarseFreq4:    .byte 1 ; 11 0b   0x1bb
Detune4:        .byte 1 ; 12 0c   0x1bc
AttackRate2:	.byte 1 ; 13 0d   0x1bd
Decay1Rate2: 	.byte 1 ; 14 0e   0x1be
Decay2Rate2: 	.byte 1 ; 15 0f   0x1bf
ReleaseRate2:	.byte 1 ; 16 10   0x1c0
Decay1Level2:	.byte 1 ; 17 11   0x1c1
LevelSens2:  	.byte 1 ; 18 12   0x1c2
RateSensv2:  	.byte 1 ; 19 13   0x1c3
EnvBiasSens2:	.byte 1 ; 20 14   0x1c4
AmpModEnable2:	.byte 1 ; 21 15   0x1c5
KeyVelSens2: 	.byte 1 ; 22 16   0x1c6
OutputLevel2:	.byte 1 ; 23 17   0x1c7
CoarseFreq2: 	.byte 1 ; 24 18   0x1c8
Detune2:     	.byte 1 ; 25 19   0x1c9
AttackRate3:	.byte 1 ; 26 1a   0x1ca
Decay1Rate3: 	.byte 1 ; 27 1b   0x1cb
Decay2Rate3: 	.byte 1 ; 28 1c   0x1cc
ReleaseRate3:	.byte 1 ; 29 1d   0x1cd
Decay1Level3:	.byte 1 ; 30 1e   0x1ce
LevelSens3:  	.byte 1 ; 31 1f   0x1cf
RateSensv3:  	.byte 1 ; 32 20   0x1d0
EnvBiasSens3:	.byte 1 ; 33 21   0x1d1
AmpModEnable3:  .byte 1 ; 34 22   0x1d2
KeyVelSens3:	.byte 1 ; 35 23   0x1d3
OutputLevel3:	.byte 1 ; 36 24   0x1d4
CoarseFreq3:	.byte 1 ; 37 25   0x1d5
Detune3:	.byte 1 ; 38 26   0x1d6
AttackRate1:	.byte 1 ; 39 27   0x1d7
Decay1Rate1: 	.byte 1 ; 40 28   0x1d8
Decay2Rate1: 	.byte 1 ; 41 29   0x1d9
ReleaseRate1:	.byte 1 ; 42 2a   0x1da
Decay1Level1:	.byte 1 ; 43 2b   0x1db
LevelSens1:  	.byte 1 ; 44 2c   0x1dc
RateSensv1:  	.byte 1 ; 45 2d   0x1dd
EnvBiasSens1: 	.byte 1 ; 46 2e   0x1de
AmpModEnable1:	.byte 1 ; 47 2f   0x1df
KeyVelSens1:  	.byte 1 ; 48 30   0x1e0
OutputLevel1: 	.byte 1 ; 49 31   0x1e1
CoarseFreq1:  	.byte 1 ; 50 32   0x1e2
Detune1:	.byte 1 ; 51 33   0x1e3

; current voice general parameters
Algorythm: 	.byte 1 ; 52 34   0x1e4
Feedback:	.byte 1 ; 53 35   0x1e5
LFOSpeed:	.byte 1 ; 54 36   0x1e6
LFODelay:	.byte 1 ; 55 37   0x1e7
PitchModDepth:  .byte 1 ; 56 38   0x1e8
AmpModDepth:    .byte 1 ; 57 39   0x1e9
Sync_On_off:    .byte 1 ; 58 3a   0x1ea
LFO_Wave:	.byte 1 ; 59 3b   0x1eb
PitchModSense:  .byte 1 ; 60 3c   0x1ec
AmpModSense:    .byte 1 ; 61 3d   0x1ed
Transpose:	.byte 1 ; 62 3e   0x1ee
Mode_Poly:	.byte 1 ; 63 3f   0x1ef
PitchBendRange: .byte 1 ; 64 40   0x1f0
PortamentoMode: .byte 1 ; 65 41   0x1f1
PortamentoTime: .byte 1 ; 66 42   0x1f2
FootVolume:     .byte 1 ; 67 43   0x1f3
DX11_SU:	.byte 1 ; 68 44   0x1f4
DX11_PO:	.byte 1 ; 69 45   0x1f5
DX11_CH:	.byte 1 ; 70 46   0x1f6
ModWheelPitch:  .byte 1 ; 71 47   0x1f7
ModWheelAmp:	.byte 1 ; 72 48   0x1f8
BreathPitch:	.byte 1 ; 73 49   0x1f9
BreathAmp:  	.byte 1 ; 74 4a   0x1fa
BrPitchBias:	.byte 1 ; 75 4b   0x1fb
BrAmpBias: 	.byte 1 ; 76 4c   0x1fc
VoiceName1:	.byte 1 ; 77 4d   0x1fd
VoiceName2:	.byte 1 ; 78 4e   0x1fe
VoiceName3:	.byte 1 ; 79 4f   0x1ff
VoiceName4:	.byte 1 ; 80 50   0x200
VoiceName5:	.byte 1 ; 81 51   0x201
VoiceName6:	.byte 1 ; 82 52   0x202
VoiceName7:	.byte 1 ; 83 53   0x203
VoiceName8:	.byte 1 ; 84 54   0x204
VoiceName9:	.byte 1 ; 85 55   0x205
VoiceName10:	.byte 1 ; 86 56   0x206
DX11_PR1:	.byte 1 ; 87 57   0x207
DX11_PR2:	.byte 1 ; 88 58   0x208
DX11_PR3:	.byte 1 ; 89 59   0x209
DX11_PL1:	.byte 1 ; 90 5a   0x20a
DX11_PL2:	.byte 1 ; 91 5b   0x20b
DX11_PL3:	.byte 1 ; 92 5c   0x20c
VCEDchksum:	.byte 1 ; 93 5d   0x20d
                                    
ACEDvalues:                         
FixRatio4:	.byte 1 ; 94 5e   0x20e
FixRange4:	.byte 1 ; 95 5f   0x20f
FineFreq4:	.byte 1 ; 96 60   0x210
OscWave4: 	.byte 1 ; 97 61   0x211
EGshift4: 	.byte 1 ; 98 62   0x212
FixRatio2:	.byte 1 ; 99 63   0x213
FixRange2:	.byte 1 ;100 64   0x214
FineFreq2:	.byte 1 ;101 65   0x215
OscWave2: 	.byte 1 ;102 66   0x216
EGshift2: 	.byte 1 ;103 67   0x217
FixRatio3:	.byte 1 ;104 68   0x218
FixRange3:	.byte 1 ;105 69   0x219
FineFreq3:	.byte 1 ;106 6a   0x21a
OscWave3: 	.byte 1 ;107 6b   0x21b
EGshift3: 	.byte 1 ;108 6c   0x21c
FixRatio1:	.byte 1 ;109 6d   0x21d
FixRange1:	.byte 1 ;110 6e   0x21e
FineFreq1:	.byte 1 ;111 6f   0x21f
OscWave1: 	.byte 1 ;112 70   0x220
EGshift1: 	.byte 1 ;113 71   0x221
Reverb:		.byte 1 ;114 72   0x222
FootPitch:	.byte 1 ;115 73   0x223
FootAmp:	.byte 1 ;116 74   0x224
ACEDchksum:	.byte 1 ;117 75   0x225
                    
                                   
EFEDSvalues:                        
Effect_Type:	.byte 1 ;118 76   0x226
Effect_Speed:	.byte 1 ;119 77   0x227
Effect_Balance:	.byte 1 ;120 78   0x228
EFEDSchksum:	.byte 1 ;121 79   0x229
; assorted variables in SRAM        
CurrentCoarse:	.byte 1	;122 7a   0x22a
CurrentFine:	.byte 1 ;123 7b   0x22b
OctaveOffset:	.byte 1 ;124 7c   0x22c
T1Counter:	.byte 1 ;125 7d   0x22d  increments every Timer1 IRQ. Can be used for long period timings.


.org 0x230
TxQueue:	.byte 32


;    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~


.cseg
.org 000
	rjmp	reset		; 0x0000 Power_On/Reset vector
;; interrupt vectors for Mega88
	rjmp  GetScan	;= 0x0001 External Interrupt Request 0     
	reti	;= 0x0002 External Interrupt Request 1     
	reti	;= 0x0003 Pin Change Interrupt Request 0   
	reti	;= 0x0004 Pin Change Interrupt Request 0   
	reti	;= 0x0005 Pin Change Interrupt Request 1   
	reti	;= 0x0006 Watchdog Time-out Interrupt              
	reti	;= 0x0007 Timer/Counter2 Compare Match A   
	reti	;= 0x0008 Timer/Counter2 Compare Match A   
	reti	;= 0x0009 Timer/Counter2 Overflow          
	reti	;= 0x000a Timer/Counter1 Capture Event     
	reti	;= 0x000b Timer/Counter1 Compare Match A   
	reti	;= 0x000c Timer/Counter1 Compare Match B   
	rjmp  TIM1_OVF	;= 0x000d Timer/Counter1 Overflow          
	reti	;= 0x000e TimerCounter0 Compare Match A    
	reti	;= 0x000f TimerCounter0 Compare Match B    
	reti	;= 0x0010 Timer/Couner0 Overflow           
	reti	;= 0x0011 SPI Serial Transfer Complete             
	rjmp  RxC_IRQ	;= 0x0012 USART Rx Complete                
	rjmp  UDRE_toMIDI 	;= 0x0013 USART, Data Register Empty       
	reti	;= 0x0014 USART Tx Complete                
	reti	;= 0x0015 ADC Conversion Complete          
	reti	;= 0x0016 EEPROM Ready                     
	reti	;= 0x0017 Analog Comparator                        
	reti	;= 0x0018 Two-wire Serial Interface                
	reti	;= 0x0019 Store Program Memory Read

reset:
; initialize stack pointer
	ldi	temp,low  (ramend)
	out	spl,temp		 
	ldi	temp,high (ramend)
	out	sph,temp		 

;  initialize the internal RC system clock for Mega8
; adjust the internal RC system clock so each MIDI bit is exactly 32 microseconds
	ldi	temp, high (EEPROMEND) ; address where the OSCCAL value for precision MIDI is located
	out	EEARH, temp
	ldi	temp, low (EEPROMEND) ; address where the OSCCAL value for precision MIDI is located
	out	EEARL, temp
	sbi	EECR, EERE   ; strobe to start the read of the EEPROM data from the address in EEAR
	in	temp, EEDR
	ldi	irqtemp, 0xff
	cpse	temp, irqtemp  ; if 0xff, then a OSCCAL precision value was not written for this individual Tiny24.
	sts	OSCCAL, temp   ; if not 0xff, then overwrite the factory OSCCAL with the precision value from EEPROM. ^^^^* Mega88 change      

; initialize I/O ports
	ldi	Temp, 0b00111111 ; in7 in6 d7 d6 d5 d4 E RS
	out	LCDddr, Temp	; LCDdata output
	cbi	LCDport, E


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
	ldi	temp, (1<<ISC01) | (0<<ISC00)
	sts	EICRA, temp ;     ^^^^* Mega88 change to EICRA [lds/sts]
	ldi	temp, (1<<INT0)
	sts	EIMSK, temp  ; General Interrupt Control [register page 42]    ^^^^* Mega88 change to EIMSK [lds/sts]
	ldi	temp, INITIAL_BITCOUNT
	sts	bitcount, temp

; initialize timers.  Must adjust these I/O register names for different AVR devices.
	ldi	temp, ~(4);( high (ADCcount)) ; write high first for 16-bit registers
	sts	TCNT1H, temp   ;  ^^^^* Mega88 change TCNT1H [lds/sts]
	ldi	temp, ~0;( low (ADCcount))
	sts	TCNT1L, temp       ;  ^^^^* Mega88 change TCNT1L [lds/sts]
	ldi	temp, (1 << TOIE1)
	sts	TIMSK1, temp ;  ^^^^* Mega88 change TIMSK1 [lds/sts]
	out	TIFR1, temp       ;  ^^^^* Mega88 change TIFR1
	ldi	temp,  (1<<CS12) | (0<<CS11) | (1<<CS10) ; prescaler /1024
	sts	TCCR1B, temp	; turn on timer 1  used to set the Slider_Update period   ;  ^^^^* Mega88 change TCCR1B [lds/sts]
	clr	temp
	sts	T1counter, temp

; initialize USART   ; USCR0C default values OK for 8/N/1
	ldi	temp, MIDIbaud ;Baud rate
	sts	UBRR0L, temp     ;  ^^^^* Mega88 change UBRR0L [lds/sts], add UBRR0H=0
	ldi	temp, (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0) ;  ^^^^* Mega88 change TXEN0, RXEN0, RXCIE0 in UCSR0B [lds/sts]
	sts	UCSR0B, temp  ; turn on receive and transmit ;  ^^^^* Mega88 change UCSR0B [lds/sts]

; enable global interrupts
	sei

; initialize application
	rcall	ClearNoteTable
	clr	Rx_Head
	clr	Rx_Tail
	clr	KeyFlags
	clr	GeneralFlags
	clr	KP1stDigit
	clr	KP2ndDigit
	clr	DecodeValue
	sts	KPvalue,DecodeValue
	sts	ZeroLoc, DecodeValue
	sts	CurrentVoice, DecodeValue
	sts	Operator, DecodeValue
	ser	temp
	sts	PrevPrmTx, temp

	ldi	temp, OCTAVE_DEFAULT ; sets Middle C (MIDI note# 0x3c) to the 'Z' key
	sts	OctaveOffset, temp

	ldi	ZH, high (ProgID*2)	; LCDmsgFlash - subroutine to put message text from Flash on LCD
	ldi	ZL, low  (ProgID*2)	;  ZH:ZL- holds address of message text (* 2) in Flash
	ldi	temp, 0            	;  Temp Reg  -  holds LCD dynamic display address for first char
	rcall	LCDmsgFlash    		;  Textstring ends with 0x00
	ldi	ZH, high (SynthID*2)	; LCDmsgFlash - subroutine to put message text from Flash on LCD
	ldi	ZL, low  (SynthID*2)	;  ZH:ZL- holds address of message text (* 2) in Flash
	ldi	temp, 0x40       	;  Temp Reg  -  holds LCD dynamic display address for first char
	rcall	LCDmsgFlash    		;  Textstring ends with 0x00

	ldi	temp, high (ProtectOff * 2) ; turn off the Memory Protection in the TQ5/TX81Z
	sts	SysExStrH, temp
	ldi	temp, low (ProtectOff * 2)
	sts	SysExStrL, temp
	ldi	temp, 0x03
	sts	ParamNumber, temp
	ldi	temp, 0x00
	sts	New_Value_toTQ, temp
	rcall	SendTX81SysEx

	ldi	temp, 0 ; initial voice on power-up/reset
	sts	CurrentVoice, temp
	rcall	DoProgramChange

; initialize ADC values and display
	rcall	GetNewADCvalues
	rcall	NewParameterSelected
	lds	temp, New_Param_Pot
	sts	Prev_Param_Pot, temp
	lds	temp, New_Value_Pot
	sts	Prev_Value_Pot, temp

  

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;
;  Main Loop of the Program  99.9% of time spent here
;
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Main:
	sbrc	KeyFlags, NewScanCode
	rjmp	DoNewScanCode

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
;      Subroutines
;
;====================================================



;******************************************************************************************************
; ADCtimeout - Time to check for any changes on the two pots
; Get new ADC values from ADC channels zero and one.
; Compare the new ADC values with the previous values.
;******************************************************************************************************
ADCtimeout:
	cbr	GeneralFlags, (1<<Time4ADC) ;turn off flag and store old pot values

	lds	temp, New_Param_Pot ; New_Param_Pot value is 0.2 second old at this point
	sts	Prev_Param_Pot, temp
	lds	temp, New_Value_Pot
	sts	Prev_Value_Pot, temp

	rcall	GetNewADCvalues
; happens about every 1/3 second. Returns updated NewPot0, NewPot1
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
; NewParameterSelected  -  display text of param on 2nd line, get and display value from SRAM table
;
; called by: reset/initialization, ADCtimeout
; send:  New_Param_Pot
; return:
; uses: temp, temp2, ZL:ZH
;*****************************************************************************************************
NewParameterSelected:
; display text for parameter on second line
	ldi	ZL, low  (ParamText*2)
	ldi	ZH, high (ParamText*2)
	lds	temp, New_Param_Pot ; latest ADC conversion used as offset into string table
	rcall	TimesTen ; send temp: return rBin1L:H
	add	ZL, rBin1L  ; point to new parameter string so that is can be displayed on LCD 2nd line
	adc	ZH, rBin1H
	ldi	temp, 0b11000000 ; write parameter text at beginning of second line
	rcall	SendLCDcommand
	ldi	temp2, 10 ; each param text string is ten chars
SP_0:	lpm
	mov	temp, R0 ; get char from flash mem
	rcall	SendLCDdata  ; print to LCD 2nd line
	adiw	ZH:ZL,1
	dec	temp2
	brne	SP_0
;-----------------------------------------------------------

; check if the New Parameter Selected is a frequency, either coarse or fine.  If yes, then get the
; frequency text string for the current value in the coarse setting.
SP_checkfreq:
	lds	temp, New_Param_Pot ; latest ADC conversion used as offset into string table
	ldi	ZL, low  (OpValC*2) ; beginning of current voice's parameters in SRAM
	ldi	ZH, high (OpValC*2)
	ldi	temp3, 0
SP_1:	lpm
	mov	temp2, r0
	cp	temp2, temp
	breq	SP_foundFreq
	adiw	ZH:ZL, 1
	inc	temp3
	cpi	temp3, 8
	brlo	SP_1

;--------------------------------------------------------------------------------------------------------
; Not a frequency parameter so display param value ASCII digits after param text
SP_2:	lds	temp, operator         ; differenciate frequency parameters from non-freq params
	cbr	temp, (1<<MostSigBit)  ; by setting MSB in SRAM variable 'operator' when param is a frequency
	sts	operator, temp
	ldi	temp, 0b01001010  ;clear previous parameter value
	ldi	ZL, low  (Clear5Spaces * 2)
	ldi	ZH, high (Clear5Spaces * 2)
	rcall	LCDmsgFlash
	rcall	LCD1msWait

	ldi	ZL, low  (VCEDvalues) ; beginning of current voice's parameters in SRAM
	ldi	ZH, high (VCEDvalues)
	lds	temp, New_Param_Pot ; pot value is the offset into the Param Value table in SRAM
	add	ZL, temp
	brcc	SP_3
	inc	ZH
SP_3:
	ld	temp, z ; get param value (using pot0 ADC value as offset) from SRAM table
	ldi	temp2, 0b01001011 ; LCD displayRAM location start for digits
	ldi	temp3, 2 ; number of digits to display
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD

SP_exit: ret

;-------------------------------------------------------------------------------------------------------
SP_foundFreq:  ; arrive here if value in New_Param_Pot is in the OpVals list

; ----- display the frequency text value of the current setting of Frequency Coarse and Frequency Fine
; To get the frequency ratio string for the Freq Coarse:Fine pair:
;  - Search the FreqCoarse table from the beginning to find the first reference of the current voice's (cur_vx) FrqC value; save the offset [n].
;  - Check the FreqFine table at the offset to see if the cur_vx's FrqFine value matches. if yes then display the FreqRatio string for that offset.
;  if FreqFine[n]<>cyr_vx freq fine, then find the next occurence of cur-vx frq Coarse in the FreqCrs table and check for freq fine match.
;  repeat until the offset is found where the values in the FreqCoarse:FreqFine tables match the cur-vx's crs:fine values.
;
;  send here: temp3 = offset into OpValC table where parameter was found

; Since the current Parameter is a frequency, get the operator number
	andi	temp3, 0b00000011
	mov	temp, temp3
	sbr	temp3, (1<<MostSigBit)
	sts	Operator, temp3
	lds	temp2, ZeroLoc

; get the coarse and fine values of the current voice
	ldi	ZH, high (OpValC*2) ; get the offset of the cur-vx's operator's coarse value
	ldi	ZL, low  (OpValC*2)
	add	ZL, temp
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
	add	ZL, temp
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
	breq	SP_foundOffset

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

SP_foundOffset: ; found the offset value for both current coarse and fine frequencies, so print ratio string
	ldi	ZL, low  (FreqText*2) ; beginning of current voice's parameters in SRAM
	ldi	ZH, high (FreqText*2)
	lds	rBin1L, FreqOffset ; pot value is the offset into the Param Value table in SRAM
	clr	rBin1H
	lsl	rBin1L
	rol	rBin1H
	lsl	rBin1L
	rol	rBin1H
	add	ZL, rBin1L
	adc	ZH, rBin1H
	ldi	temp, 0xc6 ; write parameter text at 10th char position of second line
	rcall	SendLCDcommand
	ldi	temp2, 4 ; each param text string is four chars
SP_ff3:	lpm
	mov	temp, R0 ; get char from flash mem
	rcall	SendLCDdata  ; print to LCD 2nd line
	adiw	ZH:ZL,1
	dec	temp2
	brne	SP_ff3

	ldi	temp, 0b01001011  ;clear previous parameter value
	ldi	ZL, low  (Clear5Spaces * 2)
	ldi	ZH, high (Clear5Spaces * 2)
	rcall	LCDmsgFlash
	rcall	LCD1msWait

	rjmp	SP_exit

;----- Freq Ratio String not found [Freq ratio number greater than 255 ]
SP_ffer0:  ; the combination of current-voice's operator's coarse and fine freq value was not found in the Freq ratio tables
;   so print '-.--' as the freq ratio string.  The string number for the c-v's op's coarse and fine is greater than 255.
	ldi	temp, 0b11000110; 0xc6 ; write parameter text at 10th char position of second line
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

	rjmp	SP_exit



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
; determine if the parameter being changed is a frequency value
	lds	temp, Operator
	tst	temp
	brmi	DNVP_frq

;---------------- The new setting of the Value pot is NOT an adjustment of a frequency value
	ldi	ZH, high (Param_Max * 2)
	ldi	ZL, low  (Param_Max * 2)
	lds	temp, New_Param_Pot
	add	ZL, temp
	brcc	DNVP_0
	inc	ZH
DNVP_0:
	lpm
	mov	temp2, r0 ; temp2 has the scaled Max Value of the selected parameter in New_Param_Pot
	lds	temp, New_Value_Pot

;; temp has New Value, temp2 has maxParam
	rcall	FindRange ; return with rBin1L with the size of each section
	lds	XH, PrevPrmTx
	cpse	XL,XH
	rjmp	DNVP_1 ; should trigger when N_V_Pot crosses section boundary

DNVP_exit:
	ret

;-------The new setting of the Value pot is an adjustment of a frequency value
;  so change freq ratio string LCD display, and send new FreqCoarse-FreqFine values to synth
DNVP_frq:
	ldi	temp, ValueADCchannel ; re-read the Value pot in order to get its full range (0-255)
	rcall	doADC ; return from doADC with new value 0-0xff of Value pot in ADCstore
	lds	LowTemp1, ADCstore ; store this value in case a timer interrupt happens and ADCstore gets overwritten

; ----- send the coarse value of the adjusted frequency to the synth
	lds	temp2, ZeroLoc    ; zero value needed for valid carry bit usage when doing 16-bit addition
	lds	temp, Operator; get the offset of the operator's coarse freq value from the Current_Voice table
	andi	temp, 0b00000011 ;seperate operator number from flag. Both are stored in SRAM variable 'Operator'
	ldi	ZH, high (OpValC * 2) ; get the offset of the cur-vx's operator's coarse value
	ldi	ZL, low  (OpValC * 2) ; do 16-bit addition in case OpValC crosses an 8-bit border
	add	ZL, temp
	adc	ZH, temp2  ; add zero and carry bit to high byte for 16-bit addition
	lpm	; r0 holds offset of operator's coarse frequency value into Current_Voice parameter SRAM table
	sts	ParamNumber, r0 ; r0 holds the param that will be changed by the sys-ex message
	ldi	ZH, high (FreqCoarse*2) ; get the value of the cur-vx's operator's coarse value
	ldi	ZL, low  (FreqCoarse*2)
	add	ZL, LowTemp1
	adc	ZH, temp2     ; add zero and carry bit to high byte for 16-bit addition
	lpm  	;  r0 has the value from the FreqCoarse table of the setting of the Value pot (0-255)
	sts	CurrentCoarse, r0
	sts	New_Value_toTQ, r0
	ldi	temp, high (VCEDchngHdr*2)
	sts	SysExStrH, temp
	ldi	temp, low (VCEDchngHdr*2)
	sts	SysExStrL, temp
	rcall	SendTX81SysEx

;  ----- send the fine value of the adjusted frequency to the synth
	lds	temp, Operator
	andi	temp, 0b00000011 ;seperate operator number from flag. Both are stored in SRAM variable 'Operator'
	ldi	ZH, high (OpValF * 2) ; get the offset of the cur-vx's operator's fine value
	ldi	ZL, low  (OpValF * 2)
	add	ZL, temp
	adc	ZH, temp2 ; temp2  = 0; add zero and carry bit to high byte for 16-bit addition
	lpm	; r0 holds offset of operator's fine frequency value into Current_Voice table
	mov	temp3, r0
	subi	temp3, 94 ; adjust the offset into Current_Voice table for fine frequency when sending ACED sys-ex message
	sts	ParamNumber, temp3
	ldi	ZH, high (FreqFine * 2) ; get the offset of the cur-vx's operator's fine value
	ldi	ZL, low  (FreqFine * 2)
	add	ZL, LowTemp1
	adc	ZH, temp2 ; add zero and carry bit to high byte for 16-bit addition
	lpm     ;  r0 has the value from the FreqFine table of the setting of the Value pot (0-255)
	sts	CurrentFine, r0
	sts	New_Value_toTQ, r0
	ldi	temp, high (ACEDchngHdr * 2)
	sts	SysExStrH, temp
	ldi	temp, low (ACEDchngHdr * 2)
	sts	SysExStrL, temp
	rcall	SendTX81SysEx

; --- print the adjusted frequency ratio string
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
DNVP_f0: lpm
	mov	temp, R0 ; get char from flash mem
	rcall	SendLCDdata  ; print to LCD 2nd line
	adiw	ZH:ZL,1
	dec	temp3
	brne	DNVP_f0

; --- print the adjusted frequency's coarse value
	lds	temp, CurrentCoarse ; get param value (using pot0 ADC value as offset) from SRAM table
	ldi	temp2, 0b01001011 ; LCD displayRAM location start for digits
	ldi	temp3, 2 ; number of digits to display
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD
; --- print the adjusted frequency's fine value
	lds	temp, CurrentFine ; get param value (using pot0 ADC value as offset) from SRAM table
	ldi	temp2, 0b01001110 ; LCD displayRAM location start for digits
	ldi	temp3, 2 ; number of digits to display
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD
	rjmp	DNVP_exit


;----- The new setting of the Value pot is NOT an adjustment of a frequency value.  Determine what type of parameter it is.
DNVP_1:
	lds	temp2, New_Param_Pot
	cpi	temp2, 94
	brsh	DNVP_2
	ldi	temp, high (VCEDchngHdr*2)
	sts	SysExStrH, temp
	ldi	temp, low (VCEDchngHdr*2)
	sts	SysExStrL, temp
	rjmp	DNVP_send

DNVP_2:	cpi	temp2, 118
	brsh	DNVP_3
	subi	temp2, 94
	ldi	temp, high (ACEDchngHdr*2)
	sts	SysExStrH, temp
	ldi	temp, low (ACEDchngHdr*2)
	sts	SysExStrL, temp
	rjmp	DNVP_send

DNVP_3: subi	temp2, 114
	ldi	temp, high (EFEDSchngHdr*2)
	sts	SysExStrH, temp
	ldi	temp, low (EFEDSchngHdr*2)
	sts	SysExStrL, temp
	rjmp	DNVP_send

DNVP_send:
	sts	ParamNumber, temp2
	sts	New_Value_toTQ, XL
	sts	PrevPrmTx, XL
	rcall	SendTX81SysEx

	mov	temp, XL ; display the synth's new parameter value on the LCD
	ldi	temp2, 0b01001101; LCD screen location for start for digit display
	ldi	temp3, 3 ; number of digits to display on LCD
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD
	rjmp	DNVP_exit




;*********************************************************************************************************************
; FindRange     find number of sections for this param and which sections the Prev and New valuePots readings are in.
;  >>> there has to be an easier algorythm for this procedure
;
; send:   temp with the New_Value_Pot (adjusted to range 0-127), temp2 with maxParam of New_Param_Pot
; return: XL with the size of each section
;*********************************************************************************************************************
FindRange:     ;   						MaxParam           	#size of section  #sections
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
	cpi	temp, 64  ; param is a switch           0b00000001 ? 0b01111111 : 0b00000000   64              2
	brsh	FR_1
	clr	temp
	rjmp	FR_exit
FR_1:	ldi	temp, 127

FR_exit:
	mov	XL, temp
	ret


;*********************************************************************************
;  GetNewADCvalues   - do ADC on two voltage channels
;   send:
;   return:   New_Param_Pot, New_Value_Pot
;   uses:     temp
;**********************************************************************************
GetNewADCvalues:
	ldi	temp, ParamADCchannel
	rcall	doADC ; return with new value 0-0xff in ADCstore register
	lds	temp, ADCstore
	lsr	temp ; divide by 2, range 0-0x7f for parameter type
	sts	New_Param_Pot, temp
	ldi	temp, ValueADCchannel
	rcall	doADC ; return with new value 0-0xff in ADCstore register
	lds	temp, ADCstore
	lsr	temp
	sts	New_Value_Pot, temp
	ret


;********************************************************************************************
; doADC  Mega8   called by: GetNewADC  >>  ADC clock set at 125K (8MHz/64)
; send:		The ADC channel in Temp
; return:	Left aligned upper 8 bits of result in ADCstore register
; uses:  	temp
; if ADLAR=1 > bits 9-2 in ADCH ; ADC5 pin 28
;********************************************************************************************
doADC:	ori	temp, (1 << ADLAR); |(0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0)
	sts	ADMUX, temp ; ADC result bits 9-2 in ADCH upon completion
	ldi	temp, (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1)  | (0 << ADPS0); sysclk 8MHz/64=125K
	sts	ADCSRA, temp; enable ADC
	ori	temp, (1<<ADSC)
	sts	ADCSRA, temp
ADCdly:
	lds	temp, ADCSRA
	sbrc	temp, ADSC
	rjmp	ADCdly
	lds	temp, ADCH
	sts	ADCstore, temp
	ret
    ;  ^^^^* Mega88    ADMUX, ADCSRA, ADCH [lds/sts]



;**********************************************************************************
; DoSustainMsg - toggle the synth's Sustain status when CapsLock key pressed
; send: KeyFlags
; return:
;**********************************************************************************
DoSustainMsg:   ; set up MIDI message for sustain controller change
	ldi	temp, (1<<CapsStatus)
	eor	KeyFlags, temp        ; toggle the CapsStatus flag when CapsLock key is pressed
	ldi	temp, CONTROLLER_MSG  ; then use this new status to change the Sustain setting on the synth.
	mov	MIDIOut, temp
	rcall	PutMIDIbyte
	ldi	temp, 0x40   ; sustain controller ID number
	mov	MIDIOut, temp
	rcall	PutMIDIbyte
	sbrc	KeyFlags, CapsStatus
	rjmp	SendSusON
	clr	MIDIOut
	rcall	PutMIDIbyte
	rjmp 	DoSus_exit
SendSusON:
	ldi	temp, SUSTAIN_ON ;0x7f
	mov	MIDIOut, temp
	rcall	PutMIDIbyte
DoSus_exit:
	ret


;**************************************************************************************************
; TimesTen - multiply 8-bit number by ten for devices without MUL instruction
; Although the Mega8/88 has a multiply instruction, the ICE200 model device 90S8535 doesn't.
; send: temp
; return: rBin1L, rBin1H
;**************************************************************************************************
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


;*************************************************************************************************
;  Get_VCED_Data - send/receive SysEx msgs. Put data into SRAM parameter
;  Display voicename, voice number, and algorythm on first line of the LCD.
;*************************************************************************************************
Get_VCED_Data:
	ldi	ZH, high (VCEDreq * 2)
	ldi	ZL, low  (VCEDreq * 2)
	rcall	Do_MIDI_input ; VCED data in sysex buffer 0xa0-0x10f
	ldi	YH, high (SysEx_Buffer + 6)
	ldi	YL, low  (SysEx_Buffer + 6)
	ldi	ZH, high (VCEDvalues)
	ldi	ZL, low  (VCEDvalues)
	clr	temp2
GVD_0:	ld	temp, Y+
	st	Z+, temp
	inc	temp2
	cpi	temp2, 94
	brlo	GVD_0

	ldi	temp, 0b00000000
	ldi	ZL, low  (ClearLine * 2)
	ldi	ZH, high (ClearLine * 2)
	rcall	LCDmsgFlash
	rcall	LCD1msWait
	
	ldi	temp, 0x80  ; display voice name on LCD
	rcall	SendLCDcommand
	rcall	LCD1msWait
	ldi	temp2, 10
	ldi	YH, high (VoiceName1)
	ldi	YL, low  (VoiceName1)
GVD_1:	ld	temp, Y+
	rcall	SendLCDdata
	dec	temp2
	brne	GVD_1

	ldi	temp, 0b00001101 ;D=1 display on; C=1 cur on; B=1 blink on
	rcall	SendLCDcommand
	rcall	LCD1msWait
	lds	temp, CurrentVoice  ; display current voice number on right side of top line of LCD
	ldi	temp2, 0b00001010 ; display the Parameter number returned from New_Param_Pot
	ldi	temp3, 3
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD

	ldi	ZH, high (Algorythm)
	ldi	ZL, low  (Algorythm)
	ld	temp, z  ; value of algorythm
	ldi	temp2, 0b00001111
	ldi	temp3, 1
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD

GVD_exit:	ret


;***********************************************************************************************************
;  Get_ACED_Data - send/receive SysEx msgs. Put data into SRAM parameter
;
;***********************************************************************************************************
Get_ACED_Data:
	ldi	ZH, high (ACEDreq * 2)
	ldi	ZL, low  (ACEDreq * 2)
	rcall	Do_MIDI_input ; input buffer filled with data from TX/TQ5 
	ldi	temp, 25 ; MUST have this long 25mSec delay
GAD_1:	rcall	LCD1msWait
	dec	temp
	brne	GAD_1             
	ldi	YH, high (SysEx_Buffer + 16)
	ldi	YL, low  (SysEx_Buffer + 16)
	ldi	ZH, high (ACEDvalues)
	ldi	ZL, low  (ACEDvalues)
	clr	temp2
GAD_2:	ld	temp, Y+
	st	Z+, temp
	inc	temp2
	cpi	temp2, 24
	brlo	GAD_2
	ret


;***********************************************************************************************************
;  Get_EFEDS_Data - send/receive SysEx msgs. Put data into SRAM parameter
;
;***********************************************************************************************************
Get_EFEDS_Data:
	ldi	ZH, high (EFEDSreq * 2)
	ldi	ZL, low  (EFEDSreq * 2)
	rcall	Do_MIDI_input
	ldi	YH, high (SysEx_Buffer + 16)
	ldi	YL, low  (SysEx_Buffer + 16)
	ldi	ZH, high (EFEDSvalues)
	ldi	ZL, low  (EFEDSvalues)
	clr	temp2
GED_4:	ld	temp, Y+
	st	Z+, temp
	inc	temp2
	cpi	temp2, 4
	brlo	GED_4
	ret


;*********************************************************************************
; Do_MIDI_input - send request message, get bytes into SysEx buffer until 0xf7
;
; send:  ZH:ZL points to sys-ex request message
; return: GeneralFlags:InputTimeOut - set if no data arrived within timeout period
; uses:  YH:YL, temp, temp2
;*********************************************************************************
Do_MIDI_input:
; send the dump-request sys-ex message to the synth
	lpm  	; output each byte of the MIDI sysex request string pointed to by ZH:ZL
	mov	MIDIout, r0
	tst	MIDIout
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
	ldi	YH, high (SysEx_Buffer)
	ldi	YL, low  (SysEx_Buffer)

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
	st	y, temp
	ret


;***********************************************************************************************************
;  Function key was pressed - Change Algorythm
;
;***********************************************************************************************************
DoFunctionKey:
	mov 	temp, DecodeValue
	andi	temp, 0b00001111
	cpi	temp, 8  ; only check function keys 1 to 8, ignore others
	brsh	DoFnExit
	sts	New_Value_toTQ, temp   ; send sys-ex message to change algorythm parameter
	ldi	temp, high (VCEDchngHdr * 2)
	sts	SysExStrH, temp
	ldi	temp, low (VCEDchngHdr * 2)
	sts	SysExStrL, temp
	ldi	temp, AlgVCEDposition ;52
	sts	ParamNumber, temp
	rcall	SendTX81SysEx
	ldi	ZH, high (Algorythm)
	ldi	ZL, low  (Algorythm)
	st	z, temp ;new value of algorythm
	lds	temp, New_Value_toTQ
	ldi	temp2, 0b00001111
	ldi	temp3, 1
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD
DoFnExit:
	ret


;********************************************************************************************
; SendTX81SysEx  TX81Z/TQ5 sys-ex message to change a single parameter
;
; send SysExStrH, SysExStrL (SRAM), ParamNumber (SRAM), New_Value_toTQ  (SRAM)
;********************************************************************************************
SendTX81SysEx:
	lds	ZH, SysExStrH ; pointer to parameter_change header in Flash
	lds	ZL, SysExStrL
STX_0:	lpm
	mov	temp, R0
	cpi	temp, 0xff
	breq	STX_1
	mov	MIDIout, temp
	rcall	PutMIDIbyte
	adiw	ZH:ZL,1
	rjmp	STX_0
STX_1:	lds	MIDIout, ParamNumber
	rcall	PutMIDIbyte
	lds	MIDIout, New_Value_toTQ
	rcall	PutMIDIbyte
	ldi	temp, 0xf7
	mov	MIDIout, temp
	rcall	PutMIDIbyte
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
	mov	MIDIout, temp
	rcall	PutMIDIbyte
; adding the OctaveOffset value after putting the note in the currently-playing list allows the note
; to stay on after changing the octave range with the up/down arrows.
	lds	temp, OctaveOffset
	add	DecodeValue, temp
	mov	MIDIOut, DecodeValue
	rcall	PutMIDIbyte
	ldi	temp, VELOCITY
	mov	MIDIout, temp
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
	mov	MIDIout, temp
	rcall	PutMIDIbyte
	lds	temp, OctaveOffset
	add	DecodeValue, temp
	mov	MIDIOut, DecodeValue
	rcall	PutMIDIbyte
	clr	MIDIout  ; velocity set to zero in order to turn note off
	rcall	PutMIDIbyte
ToMN_exit:
	ret


;*********************************************************************************************
;   	Keypad digits   	Decode Value = 0x80 to 0x8f
;
; if a keypad digit is pressed then check if it is the first or second digit pressed.
; the first digit will have KPValue register equal to zero. 2nd digit will have bit 7 set and
; first digit press (the ten's column value in bits 3 to 0).
; Code arrives here with digit value of keypad press (plus 0x80) in DecodeValue register.
;
; >>> This code has a weird bug. The Prg Change message gets sent with each keypad digit press.
; >>> I can't understand why.  Earlier versions didn't have this bug, but the code is the same.
; >>> I ignore it because it doesn't much affect the unit's operation.
;*********************************************************************************************
DoDigit:
	sbrc	KeyFlags,LR_arrow_flag
	rjmp	DD_kluge
	mov 	temp, DecodeValue		; DV will be 0x80 to 0x8f
	andi	temp, 0b00001111
;  check if keypress is a digit.  If yes, then check if 1st or 2nd of voice number in base10
	cpi	temp, 10
	brlo	DD_digit		        														; yes it's a digit (0 to 9)
	rjmp	DD_Exit															; no, keypad press is not digit, so do nothing and exit

;  get two digits; the ten's value comes first, then the one's value
DD_digit: ; temp holds latest digit from keypad press0-9
	sbrc	KeyFlags, KPdigit_flag		; KeyFlags bit 0 determines if latest keypad digit is 1st or 2nd digit
	rjmp	DD_2Digit
	mov	KP1stDigit, temp
	sbr	KeyFlags, (1<<KPdigit_flag)	; KeyFlags bit 0 acts as a flag for 1st or 2nd digit
	rjmp	DD_Exit

DD_2Digit:
; received 2nd digit, so multiply 1st digit (temp2) by 10 and add 2nd (temp)
	cbr	KeyFlags, (1<<KPdigit_flag)	; KeyFlags bit 0 cleared removes 2nd digit flag
	mov	KP2ndDigit, temp
	mov	temp, KP1stDigit
	lsl	KP1stDigit			; First digit multiply by ten
	lsl	KP1stDigit			; by multiplying by 2 three times and adding original value twice
	lsl	KP1stDigit			; bit 7 to carry, bit 0 cleared
	add	KP1stDigit, temp
	add	KP1stDigit, temp
	add	KP1stDigit, KP2ndDigit		;  max value 99 decimal in KPvalue
	sts	CurrentVoice, KP1stDigit
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
;		2) Send Active_Sense message to the synthesizer.
;		3) When another Active_Sense byte does not come in 300mS, sound turns off.
;*********************************************************************************************
AllNotesOff:  ; 
	rcall	ClearNoteTable
	ldi	temp, 0xfe  ; turn on active sense and then don't send 0xfe again.
	mov	MIDIout, temp ; after 300mS, the sound generator turns off.
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
	mov	MIDIOut, temp
	rcall	PutMIDIbyte
	lds	MIDIout, CurrentVoice
	rcall	PutMIDIbyte
	rcall	LCD1msWait ; give TX81Z/TQ5 time to change voices internally
	rcall	Get_VCED_Data
	rcall	Get_ACED_Data
	rcall	Get_EFEDS_Data
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

;*****************************************************************************************************************
decode_data:
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

OffNoteTableLookUp:      ;determine which note needs to be turned off. Arrives here from BrS_0
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
;
;   send:  DecodeValue between 0x90 and 0x9f
;;********************************************************************************
ParseExtKey:
	andi	DecodeValue, 0x0f
	cpi	DecodeValue, 0  ; 0x90  (scancode 0x75) Up arrow - LOWER the octave range
	brne	PEK1		; not the Up arrow key
	lds	temp, OctaveOffset
	cpi	temp, 12	; can't send a MIDI note on message note value that is less than zero
	brlo	PEK0		; branch is same or higher (unsigned)
	subi	temp, 12	; number of semitone notes in a octave
	sts	OctaveOffset, temp
PEK0:	rjmp	PEK_exit

;; 0x91  (scancode 0x72) Down arrow, RAISE the octave range
PEK1:	cpi	DecodeValue, 1	; 0x91  (scancode 0x72) Down arrow  - RAISE the octave range
	brne	PEK2
	lds	temp, OctaveOffset
	cpi	temp, 0x7f - 11	; 12 is number of semitones in an octave
	brlo	PEK1a
	rjmp	PEK_exit        ; already in the highest octave range, so exit
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

;; 0x94  (scancode 0x11) Right Alt Key - not implemented
PEK4:	cpi	DecodeValue, 4
	brne	PEK5
	rjmp	PEK_exit

;; 0x95   (scancode 0x14) right Control key  - not implemented
PEK5:	cpi	DecodeValue, 5
	brne	PEK6
	rjmp	PEK_exit

;; 0x96	scancode=0x70	; Insert key  - not implemented
PEK6:	cpi	DecodeValue, 6
	brne	PEK7
	rjmp	PEK_exit

;; 0x97	scancode=0x6c	; Home key - not implemented
PEK7:	cpi	DecodeValue, 7
	brne	PEK8
	rjmp	PEK_exit

;; 0x98	scancode=0x71	; Delete key - not implemented
PEK8:	cpi	DecodeValue, 8
	brne	PEK9
	rjmp	PEK_exit

;; 0x99	scancode=0x69	; End key - not implemented
PEK9:	cpi	DecodeValue, 9
	brne	PEKa
	rjmp	PEK_exit

;; 0x9a	scancode=0x7d	; PageUp key - not implemented
PEKa:	cpi	DecodeValue, 0x0a
	brne	PEKb
	rjmp	PEK_exit

;; 0x9b	scancode=0x7a	; PageDown key - not implemented
PEKb:	cpi	DecodeValue, 0x0b
	brne	PEKc
	rjmp	PEK_exit

;; 0x9c scancode=0x5a	; KeyPad Enter key  - not implemented
PEKc:	cpi	DecodeValue, 0x0c
	brne	PEKd
	rjmp	PEK_exit

;; 0x9d	scancode=0x5a	; KeyPad divide key  - not implemented
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
PMb_0:	st	z, MIDIout
	mov	temp, Tx_Tail
	inc	temp
	andi	temp, QUEUE_MASK   ; andi instruction needs high register (r16-r31)
	mov	Tx_Tail, temp

	lds	temp, UCSR0B ;  enable UDRE interrupt;  ^^^^* Mega88 change UCSR0B [lds/sts]
	ori	temp, (1 << UDRIE0)  ;  ^^^^* Mega88 change UDRIE0 in UCSR0B [lds/sts]
	sts	UCSR0B, temp ;  ^^^^* Mega88 change UCSR0B [lds/sts]
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
; UDRE_toMIDI:
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
;          H   T  put b1 into RAM that Qtail points to (00),then increment tail (to 01) and enable UDREirq
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
UDRE_toMIDI:
	in	SREGtemp, SREG
	push	ZH
	push	ZL
	ldi	ZH, high (TxQueue)
	ldi	ZL, low (TxQueue)
	add	ZL, Tx_Head ; Locates where the new byte will taken out of the queue.
	brcc	U2M0  ; this code doesn't check whether the Qhead equals Qtail, it assumes that QHead is not equal QTail.
	inc	ZH
U2M0:	ld	irqtemp, Z
	sts	UDR0, irqtemp ; send the byte, that is pointed to by QHead, to the USART ;  ^^^^* Mega88 change UDR0 [lds/sts]

	mov	irqtemp, Tx_Head ; tail and head pointers in low registers.  ANDI uses high regs only.
	inc	irqtemp
	andi	irqtemp, QUEUE_MASK ; when at end of queue, the Head pointer rolls over to beginning of the queue
	mov	Tx_Head, irqtemp

	cpse	Tx_Tail, Tx_Head
	rjmp	U2M_exit
	lds	irqtemp, UCSR0B ; turn off the transmitter to MIDI (this IRQ code)  ;  ^^^^* Mega88 change UCSR0B [lds/sts]
	andi	irqtemp,  ~ (1 << UDRIE0) ; if head equals tail ;  ^^^^* Mega88 change UDRE0 in UCSR0B [lds/sts]
	sts	UCSR0B, irqtemp  ; no bytes in output queue to be sent    ;  ^^^^* Mega88 change UCSR0B [lds/sts]
U2M_exit:
	pop	ZL
	pop	ZH
	out	SREG, SREGtemp
	reti



;***************************************************
;  Timer1 Overflow Interrupt Code
;  Do ADC conversion set @5 times a second
;***************************************************
TIM1_OVF:
	in	SREGtemp, SREG
	ldi	irqtemp, ~(5);3;( high (ADCcount)) ; write high first for 16-bit registers
	sts	TCNT1H, irqtemp    ;  ^^^^* Mega88 change TCNT1H [lds/sts]
	ldi	irqtemp, 0; ~13;( low (ADCcount))
	sts	TCNT1L, irqtemp  ;  ^^^^* Mega88 change TCNT1L [lds/sts]
	sbr	GeneralFlags, (1<<Time4ADC)
	lds	irqtemp, T1Counter  ; T1Counter can be used to create long intervals since its roll-over period is @25 seconds
	inc	irqtemp
	sts	T1Counter, irqtemp
	out	SREG, SREGtemp
	reti


;*******************************************************************************************************
; GETSCAN:  ; signal handler for external interrupt falling edge on INT0 or PinChange
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
GetScan: ; bitcount and scancode variables are in SRAM in order to save register usage
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
	lds	irqtemp, UDR0 ;  ^^^^* Mega88 change UDR0 [lds/sts]
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
;  Send data to LCD display controller        				;** LCD
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
	ldi	temp, 15 ; 8360 cycles loop total   			;** LCD
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
	ldi	ZL, low (ASCIIdigits) 					;** LCD
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
	mov	rBin1L, temp          					;** LCD
	clr	rBin1H                					;** LCD
	ldi	ZH, high (ASCIIdigits)					;** LCD
	ldi	ZL, low  (ASCIIdigits)					;** LCD
	rcall	Bin2ToAsc5 ; convert 16-bit binary to five digits	;** LCD
	; returns result in ASCIIdigits (five digits followed by 0x00)  ;** LCD
	mov	temp, temp2
	ldi	temp2, 5
	sub	temp2, temp3
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
; converts a 16-bit-binary to a 5 digit ASCII-coded decimal    		;%% Math
; In: 16-bit-binary in rBin1H:rBin1L, ZH:ZL points to 'ASCIIdigits'	;%% Math
;   in Static RAM, where the result goes to.      			;%% Math
; Out: Z points to the beginning of the ASCII string,			;%% Math
;   16-bit number value in five bytes of ASCIIdigit buffer on return    ;%% Math
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
	ld	temp, z      ; read a BCD digit     			;%% Math
	tst	temp         ; check if leading zero			;%% Math
	brne	Bin2ToAsc5b  ; No, found digit >0   			;%% Math
	ldi	temp,' '     ; overwrite with blank 			;%% Math
	st	z+, temp     ; store and set to next position  		;%% Math
	dec	rBin2L       ; decrement counter     			;%% Math
	brne	Bin2ToAsc5a  ; further leading blanks			;%% Math
	ld	temp, z      ; Read the last BCD    			;%% Math
Bin2ToAsc5b:								;%% Math
	inc	rBin2L       ; one more char				;%% Math
Bin2ToAsc5c:								;%% Math
	subi	temp,-'0'    ; Add ASCII-0  				;%% Math
	st	z+, temp     ; store and inc pointer			;%% Math
	ld	temp,z       ; read next char				;%% Math
	dec	rBin2L       ; more chars?				;%% Math
	brne	Bin2ToAsc5c  ; yes, go on				;%% Math
	sbiw	ZL, 5        ; Pointer to beginning of the BCD 		;%% Math
	ret 	             ; done					;%% Math
;									;%% Math
          								;%% Math
; Bin2ToBcd5								;%% Math
; ==========								;%% Math
; converts a 16-bit-binary to a 5-digit-BCD				;%% Math
; In: 16-bit-binary in rBin1H:L, Z points to first digit		;%% Math
;   where the result goes to						;%% Math
; Out: 5-digit-BCD, Z points to first BCD-digit				;%% Math
; Used registers: rBin1H:L (unchanged), rBin2H:L (changed), 		;%% Math
;   temp								;%% Math
; Called subroutines: Bin2ToDigit   					;%% Math
;									;%% Math
Bin2ToBcd5:								;%% Math
	push	rBin1H ; Save number					;%% Math
	push	rBin1L              					;%% Math
	ldi	temp, HIGH (10000) ; Start with ten thousands		;%% Math
	mov	rBin2H,temp       					;%% Math
	ldi	temp, LOW (10000) 					;%% Math
	mov	rBin2L,temp       					;%% Math
	rcall	Bin2ToDigit       ; Calculate digit			;%% Math
	ldi	temp, HIGH (1000) ; Next with thousands			;%% Math
	mov	rBin2H,temp     					;%% Math
	ldi	temp, LOW (1000)					;%% Math
	mov	rBin2L, temp    					;%% Math
	rcall	Bin2ToDigit       ; Calculate digit   			;%% Math
	ldi	temp, HIGH (100)  ; Next with hundreds			;%% Math
	mov	rBin2H, temp   						;%% Math
	ldi	temp, LOW (100)						;%% Math
	mov	rBin2L, temp	  					;%% Math
	rcall	Bin2ToDigit       ; Calculate digit			;%% Math
	ldi	temp, HIGH (10)   ; Next with tens 			;%% Math
	mov	rBin2H, temp  						;%% Math
	ldi	temp, LOW (10)						;%% Math
	mov	rBin2L, temp  						;%% Math
	rcall	Bin2ToDigit       ; Calculate digit			;%% Math
	st	z,rBin1L          ; Remainder are ones			;%% Math
	sbiw	ZL, 4             ; Put pointer to first BCD		;%% Math
	pop	rBin1L            ; Restore original binary 		;%% Math
	pop	rBin1H							;%% Math
	ret                       ; and return				;%% Math
									;%% Math
									;%% Math
; Bin2ToDigit								;%% Math
; ===========								;%% Math
; converts one decimal digit by continued subtraction of a  		;%% Math
;   binary coded decimal						;%% Math
; Used by: Bin2ToBcd5, Bin2ToAsc5, Bin2ToAsc				;%% Math
; In: 16-bit-binary in rBin1H:L, binary coded decimal in    		;%% Math
;   rBin2H:L, Z points to current BCD digit				;%% Math
; Out: Result in Z, Z incremented					;%% Math
; Used registers: rBin1H:L (holds remainder of the binary), 		;%% Math
;   rBin2H:L (unchanged), temp						;%% Math
; Called subroutines: -							;%% Math
;									;%% Math
Bin2ToDigit:								;%% Math
	clr	temp            ; digit count is zero			;%% Math
Bin2ToDigita:								;%% Math
	cp	rBin1H, rBin2H  ; Number bigger than decimal?		;%% Math
	brcs	Bin2ToDigitc    ; MSB smaller than decimal		;%% Math
	brne	Bin2ToDigitb    ; MSB bigger than decimal 		;%% Math
	cp	rBin1L, rBin2L  ; LSB bigger or equal decimal		;%% Math
	brcs	Bin2ToDigitc    ; LSB smaller than decimal		;%% Math
Bin2ToDigitb:								;%% Math
	sub	rBin1L, rBin2L  ; Subtract LSB decimal 			;%% Math
	sbc	rBin1H, rBin2H  ; Subtract MSB decimal 			;%% Math
	inc	temp            ; Increment digit count			;%% Math
	rjmp	Bin2ToDigita    ; Next loop				;%% Math
Bin2ToDigitc:								;%% Math
	st	z+, temp        ; Save digit and increment		;%% Math
	ret 	                ; done					;%% Math
									;%% Math



; ------------------------------------------------------------------------------------------------------------
;  Program Data, MIDI messages, and Text strings
; ------------------------------------------------------------------------------------------------------------

; LCD displays	   0123456789abcdef
ClearLine:    .db "                ",0,0
Clear5Spaces: .db "     ",0

; Headers for commands to the TX81Z/TQ5
ProtectOff:   .db 0xF0, 0x43, 0x10, 0x24, 0xff,0

; headers for single_parameter_change sys-ex message sent out by the TX81Z/TQ5
VCEDchngHdr:  .db 0xF0, 0x43, 0x10, 0b00010010, 0xff, 0
ACEDchngHdr:  .db 0xF0, 0x43, 0x10, 0b00010011, 0xff, 0
EFEDSchngHdr: .db 0xF0, 0x43, 0x10, 0b00100100, 0xff, 0  ; not used on TX81-Z

; SysEx messages that request the TX81Z/TQ5 to send bulk data
VCEDreq:      .db 0xf0, 0x43, 0x20, 0x03, 0xf7, 0x00 ; returns 101 bytes
ACEDreq:      .db 0xf0, 0x43, 0x20, 0x7e, 0x4c, 0x4d, 0x20, 0x20, 0x38, 0x39, 0x37, 0x36, 0x41, 0x45, 0xf7, 0
EFEDSreq:     .db 0xf0, 0x43, 0x20, 0x7e, 0x4c, 0x4d, 0x20, 0x20, 0x38, 0x30, 0x33, 0x36, 0x45, 0x46, 0xf7, 0

OpValC: ; operator 1,2,3,4 values for frequency
.db   50,  24,  37, 11 ; Freq Coarse VCED 0x32, 0x18, 0x25, 0x0b
OpValF:
.db  111, 101, 106, 96 ; offset of Freq Fine  0x6f,0x65,0x6a,0x60
;.db 17,  7, 12, 2   ;   Freq Fine ACED positions for 1,2,3,4 operator

NoFreqRatioStr: .db "-.--", 0, 0


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
;     90   91   92   93   94   95   96   97   98   99   9a   9b   9c   9d <----Decode value returned
.db 0x75,0x72,0x6b,0x74,0x11,0x14,0x70,0x6c,0x71,0x69,0x7d,0x7a,0x5a,0x4a,0,0; 0x90-0x9d
;     \______________________________________________________________________  down arrow
;          \_________________________________________________________________  up arrow
;               \____________________________________________________________  left  arrow
;                    \_______________________________________________________  right arrow
;                         \__________________________________________________  right Alt key
;                              \_____________________________________________  right Control key
;                                   \________________________________________  Insert key
;                                        \___________________________________  Home key
;                                             \______________________________  Delete key
;                                                  \_________________________  End key
;                                                       \____________________  PageUp key
;                                                            \_______________  PageDown key
;                                                                 \__________  KeyPad Enter key
;                                                                      \_____  KeyPad divide key



ParamText: ; operator voice parameter strings
.db "AttackRt_4" ;0x00
.db "Decay1Rt_4"
.db "Decay2Rt_4"
.db "Rel Rate_4"
.db "Decy1Lev_4"
.db "LevelSen_4" ;0x05
.db "RateSens_4"
.db "EnvBiaSn_4"
.db "AmpModEn_4"
.db "KeyVlSen_4"
.db "OscLevel_4" ;0x0a
.db "Frq_4     "
.db "Detune___4"
.db "AttackRt_2" ;0x0d
.db "Decay1Rt_2"
.db "Decay2Rt_2"
.db "Rel Rate_2" ;0x10
.db "Decy1Lev_2"
.db "LevelSen_2"
.db "RateSens_2"
.db "EnvBiaSn_2"
.db "AmpModEn_2" ;0x15
.db "KeyVlSen_2"
.db "OscLevel_2"
.db "Frq_2     "
.db "Detune___2"
.db "AttackRt_3" ;0x1a
.db "Decay1Rt_3"
.db "Decay2Rt_3"
.db "Rel Rate_3"
.db "Decy1Lev_3"
.db "LevelSen_3"
.db "RateSens_3" ;0x20
.db "EnvBiaSn_3"
.db "AmpModEn_3"
.db "KeyVlSen_3"
.db "OscLevel_3"
.db "Frq_3     "
.db "Detune___3"
.db "AttackRt_1" ;0x27
.db "Decay1Rt_1"
.db "Decay2Rt_1"
.db "Rel Rate_1" ;0x2a
.db "Decy1Lev_1"
.db "LevelSen_1"
.db "RateSens_1"
.db "EnvBiaSn_1"
.db "AmpModEn_1"
.db "KeyVlSen_1" ;0x30
.db "OscLevel_1"
.db "Frq_1     "
.db "Detune___1" ;0x33
.db "Algorythm " ;0x34 ; general voice param text strings
.db "Feedback  "
.db "LFO Speed "
.db "LFO Delay "
.db "P Mod Dpth"
.db "A Mod Dpth"
.db "Sync on/f " ;0x3a
.db "LFO_wave  "
.db "PitchModSn"
.db "Amp Mod Sn"
.db "Transpose "
.db "Mode poly "
.db "P Bend Rg " ;0x40
.db "Porta Mode" ; not implemented on TQ5
.db "Porta Time" ; not implemented on TQ5
.db "FootCnlVol"
.db "Sustain_Sw" ; not implemented on TQ5
.db "P_O_switch" ;0x45 ; not implemented on TQ5
.db "Chorus_Sw " ; not implemented on TQ5
.db "ModWhPitch"
.db "ModWhlAmp "
.db "BreathPtch"
.db "Breath_Amp";0x4a
.db "BrPitchBia"
.db "BrAmp_Bias"
.db "VoiceName1"
.db "VoiceName2"
.db "VoiceName3"
.db "VoiceName4";0x50
.db "VoiceName5"
.db "VoiceName6"
.db "VoiceName7"
.db "VoiceName8"
.db "VoiceName9"
.db "VoiceNm_10"
.db "PEG_PRt__1" ; not implemented on TQ5
.db "PEG_PRt__2" ;0x58; not implemented on TQ5
.db "PEG_PRt__3" ; not implemented on TQ5
.db "PEG_PLvl_1" ; not implemented on TQ5
.db "PEG_PLvl_2" ; not implemented on TQ5
.db "PEG_PLvl_3" ; not implemented on TQ5
.db "VCED chksm"
.db "FixRatio_4"; AVED values in order
.db "FixRange_4"
.db "Frq_4     " ;0x60
.db "Osc_Wave_4"
.db "EG_Shift_4"
.db "FixRatio_2"
.db "FixRange_2"
.db "Frq_2     "
.db "Osc_Wave_2"
.db "EG_Shift_2"
.db "FixRatio_3" ;0x68
.db "FixRange_3"
.db "Frq_3     "
.db "Osc_Wave_3"
.db "EG_Shift_3"
.db "FixRatio_1"
.db "FixRange_1"
.db "Frq_1     "
.db "Osc_Wave_1" ;0x70
.db "EG_Shift_1"
.db "Reverb 0-7"
.db "FootCnPtch"
.db "FootCntAmp"
.db "ACED chksm"
.db "EffectType" ;0x76 effects values
.db "Effect_Spd"
.db "Effect_Bal" ;0x78
.db "EFEDS_cksm"
.db " ADC 0x7a "
.db " ADC 0x7b "
.db " ADC 0x7c "
.db " ADC 0x7d "
.db " ADC 0x7e "
.db " ADC 0x7f "

Param_Max: ;  Parameter Maximum Value
.db 0x1f, 0x1f	;AttackRate4: 		;Decay1Rate4:
.db 0x1f, 0x0f	;Decay2Rate4: 		;ReleaseRate4:
.db 0x0f, 0x63	;Decay1Level4:		;LevelSens4:
.db 0x03, 0x07	;RateSensv4:  		;EnvBiasSens4:
.db 0x01, 0x07	;AmpModEnable4:		;KeyVelSens4:
.db 0x63, 0x2f	;OutputLevel4: 		;CoarseFreq4:
.db 0x06, 0x1f	;Detune4:     		;AttackRate2:
.db 0x1f, 0x1f	;Decay1Rate2: 		;Decay2Rate2:
.db 0x0f, 0x0f	;ReleaseRate2:		;Decay1Level2:
.db 0x63, 0x03	;LevelSens2:  		;RateSensv2:
.db 0x07, 0x01	;EnvBiasSens2:		;AmpModEnable2:
.db 0x07, 0x63	;KeyVelSens2: 		;OutputLevel2:
.db 0x3f, 0x06	;CoarseFreq2: 		;Detune2:
.db 0x1f, 0x1f	;AttackRate3:		;Decay1Rate3:
.db 0x1f, 0x0f	;Decay2Rate3: 		;ReleaseRate3:
.db 0x0f, 0x63	;Decay1Level3:		;LevelSens3:
.db 0x03, 0x07	;RateSensv3:  		;EnvBiasSens3:
.db 0x01, 0x07	;AmpModEnable3:		;KeyVelSens3:
.db 0x63, 0x3f	;OutputLevel3: 		;CoarseFreq3:
.db 0x06, 0x1f	;Detune3:      		;AttackRate1:
.db 0x1f, 0x1f	;Decay1Rate1:  		;Decay2Rate1:
.db 0x0f, 0x0f	;ReleaseRate1: 		;Decay1Level1:
.db 0x63, 0x03	;LevelSens1:   		;RateSensv1:
.db 0x07, 0x01	;EnvBiasSens1: 		;AmpModEnable1:
.db 0x07, 0x63	;KeyVelSens1:  		;OutputLevel1:
.db 0x3f, 0x06	;CoarseFreq1:  		;Detune1:
.db 0x07, 0x07	;Algorythm: 		;Feedback:
.db 0x63, 0x63	;LFOSpeed:     		;LFODelay:
.db 0x63, 0x63	;PitchModDepth:		;AmpModDepth:
.db 0x01, 0x03	;Sync_On_off:  		;LFO_Wave:
.db 0x07, 0x03	;PitchModSense:		;AmpModSense:
.db 0x30, 0x01	;Transpose:    		;Mode_Poly:
.db 0x0c, 0x01	;PitchBendRange:	;PortamentoMode:
.db 0x63, 0x63	;PortamentoTime:	;FootVolume:
.db 0x01, 0x01	;DX11_SU:		;DX11_PO:
.db 0x01, 0x63	;DX11_CH:		;ModWheelPitch:
.db 0x63, 0x63	;ModWheelAmp:		;BreathPitch:
.db 0x63, 0x63	;BreathAmp:  		;BrPitchBias:
.db 0x63, 0x7f	;BrAmpBias:  		;VoiceName1:
.db 0x7f, 0x7f	;VoiceName2: 		;VoiceName3:
.db 0x7f, 0x7f	;VoiceName4: 		;VoiceName5:
.db 0x7f, 0x7f	;VoiceName6: 		;VoiceName7:
.db 0x7f, 0x7f	;VoiceName8: 		;VoiceName9:
.db 0x7f, 0x63	;VoiceName10:		;DX11_PR1:
.db 0x63, 0x63	;DX11_PR2: 		;DX11_PR3:
.db 0x63, 0x63	;DX11_PL1: 		;DX11_PL2:
.db 0x63, 0x7f	;DX11_PL3: 		;VCEDchksum:
.db 0x01, 0x07	;FixRatio4:		;FixRange4:
.db 0x0f, 0x07	;FineFreq4:		;OscWave4:
.db 0x03, 0x01	;EGshift4: 		;FixRatio2:
.db 0x07, 0x0f	;FixRange2:		;FineFreq2:
.db 0x07, 0x03	;OscWave2: 		;EGshift2:
.db 0x01, 0x07	;FixRatio3:		;FixRange3:
.db 0x0f, 0x07  ;FineFreq3:		;OscWave3:
.db 0x03, 0x01  ;EGshift3: 		;FixRatio1:
.db 0x07, 0x0f  ;FixRange1:		;FineFreq1:
.db 0x07, 0x03  ;OscWave1: 		;EGshift1:
.db 0x07, 0x7f	;Reverb:		;FootPitch:
.db 0x7f, 0x7f	;FootAmp:		;ACEDchksum:
.db 0x0a, 0x3b	;Effect_Type:		;Effect_Speed:
.db 0x63, 0x7f	;Effect_Balance:	;EFEDSchksum:
.db 0, 0	; 0x7a, 0x7b
.db 0, 0	; 0x7c, 0x7d
.db 0, 0	; 0x7e, 0x7f



;;---------------------------------------------------------------------------------------------------------------------------
;
;
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
FreqCoarse:  ;     text :offset      text :offset  FreqFine
.db   0,0    ;   " 0.50":  0      " 0.56" ;  1    :  0,1
.db   0,0    ;   " 0.62":  2      " 0.68" ;  3    :  2,3
.db   1,0    ;   " 0.71":  4      " 0.75" ;  5    :  0,4
.db   2,1    ;   " 0.78":  6      " 0.79" ;  7    :  0,1
.db   0,3    ;   " 0.81":  8      " 0.87" ;  9    :  5,0
.db   2,0    ;   " 0.88": 10      " 0.93" ; 11    :  1,7
.db   1,3    ;   " 0.96": 12      " 0.97" ; 13    :  3,1
.db   2,4    ;   " 0.98": 14      " 1.00" ; 15    :  2,0
.db   1,4    ;   " 1.05": 16      " 1.06" ; 17    :  4,1
.db   2,3    ;   " 1.07": 18      " 1.08" ; 19    :  3,2
.db   4,1    ;   " 1.12": 20      " 1.14" ; 21    :  2,5
.db   2,3    ;   " 1.17": 22      " 1.18" ; 23    :  4,3
.db   1,4    ;   " 1.23": 24      " 1.25" ; 25    :  6,4
.db   2,3    ;   " 1.27": 26      " 1.29" ; 27    :  5,4
.db   4,1    ;   " 1.31": 28      " 1.32" ; 29    :  5,7
.db   2,3    ;   " 1.37": 30      " 1.40" ; 31    :  6,5
.db   5,4    ;   " 1.41": 32      " 1.43" ; 33    :  0,7
.db   2,5    ;   " 1.47": 34      " 1.49" ; 35    :  7,1
.db   4,3    ;   " 1.50": 36      " 1.51" ; 37    :  8,6
.db   4,6    ;   " 1.56": 38      " 1.57" ; 39    :  9,0
.db   5,3    ;   " 1.58": 40      " 1.62" ; 41    :  2,6
.db   6,5    ;   " 1.66": 42      " 1.67" ; 43    :  1,3
.db   4,7    ;   " 1.68": 44      " 1.73" ; 45    : 11,0
.db   4,6    ;   " 1.75": 46      " 1.76" ; 47    : 12,2
.db   4,7    ;   " 1.81": 48      " 1.83" ; 49    : 13,1
.db   5,6    ;   " 1.85": 50      " 1.86" ; 51    :  5,3
.db   4,5    ;   " 1.87": 52      " 1.93" ; 53    : 14,6
.db   7,6    ;   " 1.94": 54      " 1.96" ; 55    :  2,4
.db   8,5    ;   " 2.00": 56      " 2.02" ; 57    :  0,7
.db   7,8    ;   " 2.05": 58      " 2.06" ; 59    :  3,1
.db   5,8    ;   " 2.11": 60      " 2.12" ; 61    :  8,2
.db   6,7    ;   " 2.15": 62      " 2.16" ; 63    :  6,4
.db   8,5    ;   " 2.18": 64      " 2.20" ; 65    :  3,9
.db   8,7    ;   " 2.25": 66      " 2.27" ; 67    :  4,5
.db   5,8    ;   " 2.29": 68      " 2.31" ; 69    : 10,5
.db   6,7    ;   " 2.35": 70      " 2.37" ; 71    :  8,6
.db   8,6    ;   " 2.43": 72      " 2.45" ; 73    :  7,9
.db   5,7    ;   " 2.46": 74      " 2.48" ; 75    : 12,7
.db   8,6    ;   " 2.50": 76      " 2.55" ; 77    :  8,10
.db   8,7    ;   " 2.56": 78      " 2.59" ; 79    :  9,8
.db   8,6    ;   " 2.62": 80      " 2.64" ; 81    : 10,11
.db   8,7    ;   " 2.68": 82      " 2.70" ; 83    : 11,9
.db   5,6    ;   " 2.73": 84      " 2.74" ; 85    : 15,12
.db   8,7    ;   " 2.75": 86      " 2.81" ; 87    : 12,10
.db   9,6    ;   " 2.82": 88      " 2.84" ; 89    :  0,13
.db   8,9    ;   " 2.87": 90      " 2.90" ; 91    : 14,1
.db   7,8    ;   " 2.91": 92      " 2.93" ; 93    : 11,15
.db   6,9    ;   " 2.94": 94      " 2.99" ; 95    : 14,2
.db  10,7    ;   " 3.00": 96      " 3.02" ; 97    :  0,12
.db   6,10   ;   " 3.04": 98      " 3.06" ; 99    : 15,1
.db   9,10   ;   " 3.08":100      " 3.12" ;101    :  3,2
.db   7,11   ;   " 3.13":102      " 3.14" ;103    : 13,0
.db   9,10   ;   " 3.17":104      " 3.18" ;105    :  4,3
.db  11,7    ;   " 3.23":106      " 3.24" ;107    :  1,14
.db  10,9    ;   " 3.25":108      " 3.26" ;109    :  4,5
.db  10,11   ;   " 3.31":110      " 3.33" ;111    :  5,2
.db   9,7    ;   " 3.34":112      " 3.35" ;113    :  6,15
.db  10,11   ;   " 3.37":114      " 3.43" ;115    :  6,3
.db  12,10   ;   " 3.46":116      " 3.50" ;117    :  0,8
.db   9,11   ;   " 3.52":118      " 3.53" ;119    :  8,4
.db  12,9    ;   " 3.56":120      " 3.61" ;121    :  1,9
.db  10,11   ;   " 3.62":122      " 3.63" ;123    : 10,5
.db  12,10   ;   " 3.67":124      " 3.68" ;125    :  2,11
.db   9,11   ;   " 3.70":126      " 3.72" ;127    : 10,6
.db  10,12   ;   " 3.75":128      " 3.78" ;129    : 12,3
.db  10,11   ;   " 3.81":130      " 3.82" ;131    : 13,7
.db   9,12   ;   " 3.87":132      " 3.89" ;133    : 12,4
.db  11,10   ;   " 3.92":134      " 3.93" ;135    :  8,15
.db   9,13   ;   " 3.96":136      " 4.00" ;137    : 13,0
.db  11,9    ;   " 4.02":138      " 4.05" ;139    :  9,14
.db  13,12   ;   " 4.06":140      " 4.10" ;141    :  1,6
.db  13,9    ;   " 4.12":142      " 4.14" ;143    :  2,15
.db  13,12   ;   " 4.18":144      " 4.21" ;145    :  3,7
.db  14,13   ;   " 4.24":146      " 4.25" ;147    :  0,4
.db  14,12   ;   " 4.31":148      " 4.32" ;149    :  1,8
.db  13,14   ;   " 4.37":150      " 4.40" ;151    :  6,2
.db  11,13   ;   " 4.41":152      " 4.43" ;153    : 13,7
.db  14,13   ;   " 4.49":154      " 4.50" ;155    :  3,8
.db  11,12   ;   " 4.51":156      " 4.54" ;157    : 14,10
.db  13,14   ;   " 4.56":158      " 4.58" ;159    :  9,4
.db  11,13   ;   " 4.61":160      " 4.62" ;161    : 15,10
.db  12,14   ;   " 4.64":162      " 4.67" ;163    : 11,5
.db  13,15   ;   " 4.68":164      " 4.71" ;165    : 11,0
.db  14,15   ;   " 4.75":166      " 4.80" ;167    :  6,1
.db  13,14   ;   " 4.81":168      " 4.84" ;169    : 13,7
.db  12,13   ;   " 4.86":170      " 4.87" ;171    : 13,14
.db  15,14   ;   " 4.90":172      " 4.93" ;173    :  2,8
.db  12,16   ;   " 4.97":174      " 5.00" ;175    : 14,0
.db  14,16   ;   " 5.02":176      " 5.06" ;177    :  9,1
.db  12,15   ;   " 5.08":178      " 5.10" ;179    : 15,4
.db  14,16   ;   " 5.11":180      " 5.12" ;181    : 10,2
.db  16,17   ;   " 5.18":182      " 5.19" ;183    :  3,0
.db  15,16   ;   " 5.20":184      " 5.25" ;185    :  5,4
.db  14,17   ;   " 5.28":186      " 5.29" ;187    : 12,1
.db  16,16   ;   " 5.31":188      " 5.37" ;189    :  5,6
.db  15,17   ;   " 5.39":190      " 5.40" ;191    :  7,2
.db  16,14   ;   " 5.43":192      " 5.46" ;193    :  7,14
.db  15,16   ;   " 5.49":194      " 5.50" ;195    :  8,8
.db  17,14   ;   " 5.51":196      " 5.55" ;197    :  3,15
.db  16,15   ;   " 5.56":198      " 5.59" ;199    :  9,9
.db  17,18   ;   " 5.62":200      " 5.65" ;201    :  4,0
.db  16,15   ;   " 5.68":202      " 5.69" ;203    : 11,10
.db  18,17   ;   " 5.72":204      " 5.73" ;205    :  1,5
.db  16,15   ;   " 5.75":206      " 5.78" ;207    : 12,11
.db  18,17   ;   " 5.81":208      " 5.83" ;209    :  2,6
.db  16,15   ;   " 5.87":210      " 5.88" ;211    : 14,12
.db  18,6    ;   " 5.90":212      " 5.93" ;213    :  3,15
.db  17,15   ;   " 5.94":214      " 5.98" ;215    :  7,13
.db  18,19   ;   " 5.99":216      " 6.00" ;217    :  4,0
.db  17,19   ;   " 6.05":218      " 6.06" ;219    :  8,1
.db  18,19   ;   " 6.08":220      " 6.12" ;221    :  5,2
.db  18,19   ;   " 6.16":222      " 6.18" ;223    :  6,3
.db  19,17   ;   " 6.25":224      " 6.27" ;225    :  4,10
.db  20,19   ;   " 6.28":226      " 6.31" ;227    :  0,5
.db  18,20   ;   " 6.34":228      " 6.37" ;229    :  8,1
.db  19,20   ;   " 6.43":230      " 6.47" ;231    :  7,2
.db  17,19   ;   " 6.48":232      " 6.50" ;233    : 12,8
.db  18,19   ;   " 6.52":234      " 6.56" ;235    : 10,9
.db  20,17   ;   " 6.57":236      " 6.59" ;237    :  3,13
.db  18,19   ;   " 6.60":238      " 6.62" ;239    : 11,10
.db  20,19   ;   " 6.67":240      " 6.68" ;241    :  4,11
.db  18,17   ;   " 6.69":242      " 6.70" ;243    : 12,14
.db  19,20   ;   " 6.75":244      " 6.77" ;245    : 12,5
.db  18,19   ;   " 6.78":246      " 6.81" ;247    : 13,13
.db  20,19   ;   " 6.86":248      " 6.87" ;249    :  6,14
.db  21,19   ;   " 6.92":250      " 6.93" ;251    :  0,15
.db  20,22   ;   " 6.96":252      " 7.00" ;253    :  7,0
.db  21,22   ;   " 7.02":254      " 7.06" ;255    :  1,1

FreqFine:
.db   0,1
.db   2,3
.db   0,4
.db   0,1
.db   5,0
.db   1,7
.db   3,1
.db   2,0
.db   4,1
.db   3,2
.db   2,5
.db   4,3
.db   6,4
.db   5,4
.db   5,7
.db   6,5
.db   0,7
.db   7,1
.db   8,6
.db   9,0
.db   2,6
.db   1,3
.db  11,0
.db  12,2
.db  13,1
.db   5,3
.db  14,6
.db   2,4
.db   0,7
.db   3,1
.db   8,2
.db   6,4
.db   3,9
.db   4,5
.db  10,5
.db   8,6
.db   7,9
.db  12,7
.db   8,10
.db   9,8
.db  10,11
.db  11,9
.db  15,12
.db  12,10
.db   0,13
.db  14,1
.db  11,15
.db  14,2
.db   0,12
.db  15,1
.db   3,2
.db  13,0
.db   4,3
.db   1,14
.db   4,5
.db   5,2
.db   6,15
.db   6,3
.db   0,8
.db   8,4
.db   1,9
.db  10,5
.db   2,11
.db  10,6
.db  12,3
.db  13,7
.db  12,4
.db   8,15
.db  13,0
.db   9,14
.db   1,6
.db   2,15
.db   3,7
.db   0,4
.db   1,8
.db   6,2
.db  13,7
.db   3,8
.db  14,10
.db   9,4
.db  15,10
.db  11,5
.db  11,0
.db   6,1
.db  13,7
.db  13,14
.db   2,8
.db  14,0
.db   9,1
.db  15,4
.db  10,2
.db   3,0
.db   5,4
.db  12,1
.db   5,6
.db   7,2
.db   7,14
.db   8,8
.db   3,15
.db   9,9
.db   4,0
.db  11,10
.db   1,5
.db  12,11
.db   2,6
.db  14,12
.db   3,15
.db   7,13
.db   4,0
.db   8,1
.db   5,2
.db   6,3
.db   4,10
.db   0,5
.db   8,1
.db   7,2
.db  12,8
.db  10,9
.db   3,13
.db  11,10
.db   4,11
.db  12,14
.db  12,5
.db  13,13
.db   6,14
.db   0,15
.db   7,0
.db   1,1

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

;;  debug code
;doToggle_0:
;	push	temp
;	ldi	temp, ToggleBit0
;	mov	ToggleRegister, temp
;	in	temp, TogglePort     ; *** DEBUG ***  does this LED flash when the pot is not moved?
;	eor	temp, ToggleRegister ;  toggle PB0 to show that code actually gets here repeatedly
;	out	TogglePort, temp
;	pop	temp
;	rts
;
;doToggle1:
;	push	temp
;	ldi	temp, ToggleBit1
;	mov	ToggleRegister, temp
;	in	temp, TogglePort     ; *** DEBUG ***  does this LED flash when the pot is not moved?
;	eor	temp, ToggleRegister ;  toggle PB1 to show that code actually gets here repeatedly
;	out	TogglePort, temp
;	pop	temp
;	rts

; *** in-code program indentification
;             0123456789abcdef
ProgID: .db  "TwoPot Mega8 v31",0,0
SynthID: .db "Yamaha TQ5/TX81Z",0,0

;  This last ID tag shows the program file name in ASCII at the end of the internal flash
;  The character pairs are reversed to make it easy to read with the SP12 code load feature. Use:  "C\AVR\SP12 -rpfh WhatsThisCode.txt <enter>"
.db   "****  wTPotoM ge8av 13  lanap_orabdn@tayoh.ooc m"



;**********************************************************************************************************
;  HD44780  LCD instruction summary
; RS  RW ---  7   6   5   4   3   2   1   0
;------------------------------------------
;  0  0  ---  0   0   0   0   0   0   0   1	Clear Display; set DDRAM address to 0
;  0  0  ---  0   0   0   0   0   0   1   -	Return Home; set DDRAM to 0; 1.52mS
;  0  0  ---  0   0   0   0   0   1   D   S	Entry Mode Set; cursor move dir  display shift  37uS
;  0  0  ---  0   0   0   0   1   D   C   B	Display; D=1 display on; C=1 cursor on; B=1 cursor blink on  37uS
;  0  0  ---  0   0   0   1  s/c  r/l -   -	Move cursor; shift display no content change  37uS
;  0  0  ---  0   0   1   dl  n   f   -   -	datalen 1=8bits,0=4bits;n=#lines 0=oneline 1=twolines;f=char font 0=5x8 1=5x10  37uS
;  0  0  ---  0   1   a   a   a   a   a   a	CGRAM address set  37uS
;  0  0  ---  1   a   a   a   a   a   a   a	DDRAM address set  37uS
;  0  1  --- busy a   a   a   a   a   a   a	Read busy flag 1=busy; address counter
;  1  0  --- d7  d6  d5  d4   -   -   -   -	First data write in 4bit access mode
;  1  0  --- d3  d2  d1  d0   -   -   -   -	Second data write in 4bit access mode

;  RS  register select	1=data  0=command/status
;  RW  read/write	1=read	0=write
;  EN  enable   500nS min cycle len   230nS min high len (+5V)

