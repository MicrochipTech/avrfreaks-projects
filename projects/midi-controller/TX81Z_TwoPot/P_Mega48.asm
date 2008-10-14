; TX_TwoPot.asm    v1.3    October 2, 2008
;  alan_probandt@yahoo.com   Target CPU: Atmel AVR ATMega48
;
; ~~~ Bare-bones Real-Time controller for the Yamaha TQ5 4-operator FM tone-module audio synthesizer.
;     Works also with the popular Yamaha TX81-Z 4-op FM tone module.
;
;  -- There are a lot of features that can be added to make this program better.  But I want to get it in circulation now.
;  -- Send me your suggestions and improvements.
;
;  use Terminal or Lucinda font to format source code listing correctly
;
;
;  Program Operation Overview
;-----------------------------
;	The main code checks repeatedly for a keypress from the PS2 keyboard.  When a key is
;  pressed, the code checks if it is a keypad digit, an arrow key, or a key from the main
;  area.  The main keys are arranged like two halves of a piano; the ~12345 row is the black keys
;  (sharps and flats) of the QWERTY row of white keys.  The 'Z' key is middle C.  Pressing a
;  key sends Note ON to the synth, releasing the key turns the note off.  Up to eight keys
;  be sounded at once, but due to the PS2 internal wiring, some key combinations don't work.
;
;	The 'up' arrow shifts the keyboard notes down one octave and the 'down' key shifts the notes
;  played up one octave. The left and right keys change the voice number. Changing the voice
;  causes all the parameter values for that voice to be loaded into the processor.  The top line
;  of the LCD shows the new voice name and number.  The left and right keys roll over: pressing
;  left key at voice #0 goes to #127, pressing right at #127 goes to #0.  Pressing two keypad
;  digits changes the voice number.  From voice #23, pressing keypad '5' selects voice #25.
;  Then pressing '8' loads voice #58.
;
;	Pressing a function key (1 to 8) selects a new algorythm for the voice. The display on the top right
;  changes to show the new algorythm selection.
;
;	Every 1/10 of a second, the main code interrupts and does a scan of two variable resistors
;  (potentiometer knobs or sliders).  The new values for each 'pot' are compared to the values
;  from the last set of conversions. If they are the same, then the interrupt exits and nothing
;  happens.  If the user turns the Parameter pot, then the code puts a new text string on the LCD,
;  along with the parameter's current value loaded from the synth.  
;	The parameter pot is very sensitive -  a second pot of much lower value can be added in series 
;  to make it easier to make selections of params that are close together.  For example, a 5K ohm pot
;  in series with a 50,000 ohm pot.  The large value pot selects the operator range and the smaller
;  pot selects the individual parameter.
;
;               voicename  vx#   algorythm
;            --------------------------
;     LCD   :   GrandPiano  32    2    :
;     16x2  :   AttackRt_1  31   10    :
;            --------------------------
;           param name  init_val  adj_val
;
;	Changing the Value pot selects a new setting for the parameter.  This pot value is adjusted 
;   according to the maximum value of the selected parameter.  For example, a parameter that has only 
;   three settings (like Amp Mod Sensitivity) needs to have the knob turned (or slider pot moved) further
;   than a parameter with 32 settings (like Attack Rate) to effect any change.

;	All the parameters used by the Yamaha 4-op series of modules are listed.  Some parameters are 
;  not used by a particular module.  For example, the PEG (programmable envelope generator rate and level)
;  settings are used only by the DX-11.  Since the settings are in the VCED sys-ex structure, it's easier
;  to just leave them into the parameter list than it is to write code to nullify them.  Changes sent to 
;  unused parameters are ignored by the tone module.  
;
;	Only the single voice settings are changed, the multi voice settings are not implemented.  Nor are
;  TX81-Z effects or microscale tuning tables.  All these features can be set by pressing front panel buttons.
;  Switching Memory-Protect to OFF is done only for the TQ5 model.  For some unknown reason, the Yamaha 
;  designers used different sys-ex strings for each model to turn the Memory-Protect feature off.    
;
; 	If the LCD screen shows only "TwoPot_m48 v1.3" when powered-on, the MIDI-in and MIDI-out cables are not
;  both connected to the synth, or the synth is not set to send/receive on MIDI channel 1. 
;  
;
;
;~~~~~~~~~~~~~~   Hardware design notes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;                                                           |  Basic parts list                             DigiKey #
; Atmel AVR Mega48  microcontroller   4K Flash              |                                               
;                       _______                             |  2   DIN5 (180 degree) MIDI jacks             DIN-5500-5S
; +5V--10K----<  /reset |1  28|1  ADC5 SCL    Param  pot    |  1   MiniDIN6 jack for PS2 keyboard           CP-2460-ND
;     MIDI in   RxD PD0 |2  27|2  ADC4 SDA    Value  pot    |  1   MIDI opto-isolator (4n24..PC817)         
;     MIDI out  TxD PD1 |3  26|3  ADC3                      |  1   Power jack (2.1mm pin=ground)            PJ-102A
;    PS2 kbd clock INT0 |4  25|4  ADC2                      |  1   LM78L05 +5 volt regulator IC             LM78L05ACZFS-ND
;    PS2 kbd data   PD3 |5  24|5  ADC1                      |  1   Atmel AVR Mega48 microcontroller         ATMEGA48P-20PU-ND
;                   PD4 |6  23|6  ADC0                      |  1   16x2 LCD display (standard HD44780 style)
;          +5V <-   VCC |7  22|7  GND                       |  2   Slide potentiometers  (10K-100K ohm is OK)
;                   GND |8  21|8  AREF  -> +5V              |  1   small 'tach' switch for reset                     
;                   PB6 |9  20|9  AVCC  -> +5V              |  2   220 - 360 ohm resistors (1/4 watt) for MIDI 
;                   PB7 |10 19|10 PB5 SCK     LCD data7     |  1   470 to 1K ohm resistor for MIDI-In opto-isolator
;                   PD5 |11 18|11 PB4 MISO    LCD data6     |  1   Power adapter +9 (to +15 volts) DC ~ 50 milliAmps 
;                   PD6 |12 17|12 PB3 MOSI    LCD data5     |      - runs processor, LCD display, and PS2 keyboard
;                   PD7 |13 16|13 PB2 LCD data4             |  
;         LCD_RS    PB0 |14 15|14 PB1 LCD_E                 |  see file "Two_Pot_TX81.gif" for schematic drawing
;                       -------                                      
;
;  An AVR Mega8 works well also.  Change the interrupt vector table, the .org addresses in the SRAM init section,
;  and the peripheral (USART, timer, etc...) initializations.  Other AVRs should have UART, ADC, and eight I/O port pins.
;
;  Assemble this code with the free assembler available from the Atmel website: AVRASM2.exe  From the command line, use:
;      C:\AVR\avrasm2 TwoPot_m48.asm -fI -l TwoPot_m48.lst -o TwoPot_m48.hex
;
;  Load the code into the AVR using an Atmel STK500 dev system.  Or, use the free, high-quality programmer: "SP12.exe"
;      C:\AVR\SP12 -wpf TwoPot_m48.hex
;
; Mega48   high: unchanged from factory default                                                                                                                                                                                                                             
; Fuses:   low:	 clk/8=off, no clk out, internal RC 8MHz system clock, full-swing, slow-rise power                                                                                                                                                                                               

                                                                             
                                
.nolist                         
.include "m48def.inc"
.list

; constants for specific AVR device type
.equ	SYSTEM_CLOCK	= 8000000
.equ	RAMSTART	= 0x100 ; for Mega48
.equ	MIDIbaud	= ((SYSTEM_CLOCK/16) / 31250) - 1  ; 15  0.0% error for MIDI


;Definitions
;LCD equates

.equ	RS		= 0	 ; PB0 Register Select
.equ	E		= 1	 ; PB1 Enable  [LCDdata4-7 on PB2-5]
.equ	LCDport		= PortB  ; Portc  ; for 90S8535
.equ	LCDddr		= ddrB   ; ddrc   ; 90S8535

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
.equ	REG_AVAILABLE	= 0xFF
.equ	VELOCITY  	= 0x50
.equ	NOTE_ON_MSG	= 0x90
.equ	NEW_PARAM_FLAG  = 7
.equ	QUEUESIZE	= 0x20
.equ	QUEUE_MASK	= QUEUESIZE - 1
.equ	MostSigBit	= 7
.equ	Bit6		= 6
.equ	EOX		= 0xf7

;LCD instructions
.equ	clr_LCD		= 0b00000001	;clear display
.equ	home_LCD	= 0b00000010	;return home
.equ	entry_mode	= 0b00000110	;set Cursor
.equ	on_LCD		= 0b00001110	;LCD on
.equ	Set_Cursor	= 0b10000000	;command to set cursor position on LCD display  (Set_Cursor  | LCD_2ndLine | 0b00001101 ) third from right end
.equ	LCD_1stLine	= 0b00000000    ; Display location for Column 0 on first line. (Set_Cursor  | LCD_2ndLine | 0b00001101 )
.equ	LCD_2ndLine	= 0b01000000	; Display location for Column 0 on second line.

;PS2 keyboard equates
.equ	KBport		= PinD
.equ	KBClockPin	= 2
.equ	KBdataPin	= 3
.equ	INITIAL_BITCOUNT = 11
.equ	PARITY_BITCOUNT  = 3

;* System Constants
.equ    ADCcount	= ((SYSTEM_CLOCK/1024) / 5) ;SYSTEM_CLOCK/1024=7812.5 Hz
.equ	ADC_Param	= 3 ; ADC multipler channel (Mega48 IC pin 28)
.equ	ADC_Value	= 2 ; ADC multipler channel (Mega48 IC pin 27)

;General lower registers   --  where used or referenced
.def	SREGtemp 	= r1
.def	scanbits	= r2	; shift register for holding PC keyboard data while being assembled
.def	MIDIOut		= r3	      
.def	Rx_Head		= r4             
.def	Rx_Tail		= r5             
.def 	TxQueueTail	= r6             
.def	TxQueueHead	= r7             
.def	rBin1L		= r8             
.def	rBin1H		= r9             
.def	rBin2L		= r10            
.def	rBin2H		= r11            
                               

;General upper registers   --  where used or referenced
.def	temp	 	= r16
.def	temp2	 	= r17
.def	irqtemp	 	= r18
.def	KeyFlags 	= r19
; KeyFlags register's bit values
.equ	LR_arrow_flag	= 7
.equ	Caps_flag	= 6 ; Caps Lock pressed on -- no NoteOff transmitted
.equ	Alt_flag	= 5 ; left Alt key is being pressed
.equ	Break_flag	= 4 ; the last scancode was an 0xf0 - the break code sent when a key is released
.equ	OnOff_flag	= 3 ; set means turn on the note whose MIDI number was decoded from the most recent scancode
.equ	NewScanCode	= 2 ; a byte has been received from the keyboard
.equ	Extended_flag	= 1 ; the extended scancode 0xE0 was sent - use the E0table to decode scancode
.equ	KPdigit_flag	= 0 ; set if expecting the second digit from the keypad
.def	DecodeValue	= r20 	; PC keyboard char decoded to NoteOn value, or AllNotesOff cmd
.def	Temp3		= r21
.def	KP1stDigit	= r22 ;.byte 1 ;7b
.def	KP2ndDigit	= r23 ;.byte 1 ;7c
.def	GeneralFlags	= r24
.equ	Time4ADC	= 7
.equ	InputTimeOut	= 6
;  ZH:ZL r31:30    YH:YL r29:28    XH:XL r27:26

;****************************************
; SRAM variables
;****************************************
.dseg
SoundingNotes:	.byte POSSIBLE_NOTES ;0x 100-107
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
ASCIIdigits:    .byte 5 ;115-119
ZeroLoc:	.byte 1 ;11a used by LCDdigits routine to display ASCII numbers
ADCstore:	.byte 1 ;11b
CurrentMax:	.byte 1 ;11c  not used ;  referenced at DNVP_0
PrevPrmTx:	.byte 1 ;11d
CurrentVoice:	.byte 1 ;11e

.org 0x120
Rx_Queue:	.byte QUEUESIZE  ; 32 bytes ; 0x120 to 0x13f
SysEx_Buffer:	.byte 0x70 ; 0x0140 to 0x1af


 ; Current Voice Operator Parameter Values
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
OctaveOffset:	.byte 1 ;124 7c   0x22c                                                                                                                                                                                                           
T1Counter:	.byte 1 ;125 7d   0x22d  increments every Timer1 IRQ. Can be used for long period timings.                                                                                                                                        

.org 0x230
TxQueue:	.byte 32 ;121 79   0x230-0x24f

;    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~


.cseg
.org 000
	rjmp	reset		; 0x0000 Power_On/Reset vector
; interrupt vectors for Mega48
	rjmp	GetScan		;= 0x0001 External Interrupt Request 0
	reti			;= 0x0002 External Interrupt Request 1
	reti			;= 0x0003 Pin Change Interrupt Request 0
	reti			;= 0x0004 Pin Change Interrupt Request 0
	reti			;= 0x0005 Pin Change Interrupt Request 1
	reti			;= 0x0006 Watchdog Time-out Interrupt
	reti			;= 0x0007 Timer/Counter2 Compare Match A
	reti			;= 0x0008 Timer/Counter2 Compare Match A
	reti			;= 0x0009 Timer/Counter2 Overflow
	reti			;= 0x000a Timer/Counter1 Capture Event
	reti			;= 0x000b Timer/Counter1 Compare Match A
	reti			;= 0x000c Timer/Counter1 Compare Match B
	rjmp	TIM1_OVF	;= 0x000d Timer/Counter1 Overflow
	reti			;= 0x000e TimerCounter0 Compare Match A
	reti			;= 0x000f TimerCounter0 Compare Match B
	reti			;= 0x0010 Timer/Couner0 Overflow
	reti			;= 0x0011 SPI Serial Transfer Complete
	rjmp	RxC_IRQ		;= 0x0012 USART Rx Complete
	rjmp	UDRE_toMIDI	;= 0x0013 USART, Data Register Empty
	reti			;= 0x0014 USART Tx Complete
	reti			;= 0x0015 ADC Conversion Complete
	reti			;= 0x0016 EEPROM Ready
	reti			;= 0x0017 Analog Comparator
	reti			;= 0x0018 Two-wire Serial Interface
	reti			;= 0x0019 Store Program Memory Read

reset:
; initialize stack pointer
	ldi	temp,low  (ramend)
	sts	spl,temp
	ldi	temp,high (ramend)
	sts	sph,temp

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
	sts	OSCCAL, temp   ; if not 0xff, then overwrite the factory OSCCAL with the precision value from EEPROM.

; initialize I/O ports
	ldi	Temp, 0b00111111 ; in7 in6 d7 d6 d5 d4 E RS
	out	LCDddr, Temp	; LCDdata output

; initialize LCD
	lds	temp, ZeroLoc
	cpi	temp, 0xaa
	breq	iLCDdone     ; LCD display was previously initiated after power-on
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
	sts	EICRA, temp
	ldi	temp, (1<<INT0)
	sts	EIMSK, temp  ; General Interrupt Control [register page 42]
	ldi	temp, INITIAL_BITCOUNT
	sts	bitcount, temp

; initialize timers
	ldi	temp, ~(4);( high (ADCcount)) ; write high first for 16-bit registers
	sts	TCNT1H, temp
	ldi	temp, ~0;( low (ADCcount))
	sts	TCNT1L, temp
	ldi	temp, (1 << TOIE1)
	sts	TIMSK1, temp
	sts	TIFR1, temp
	ldi	temp,  (1<<CS12) | (0<<CS11) | (1<<CS10) ; prescaler /1024
	sts	TCCR1B, temp	; turn on timer 1  used to set the Slider_Update period
	clr	temp
	sts	T1counter, temp

; initialize USART   ; USCR0C default values OK for 8/N/1
	ldi	temp, MIDIbaud ;Baud rate
	sts	UBRR0L, temp
	ldi	temp, (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0)
	sts	UCSR0B, temp  ; turn on receive and transmit

; enable global interrupts
	sei

; initialize application
	rcall	ClearNoteTable
	clr	Rx_Head
	clr	Rx_Tail
	clr	KeyFlags
	clr	temp  
	sts	CurrentVoice, temp	
	sts	KPvalue, temp
	sts	ZeroLoc, temp
	ser	temp
	sts	PrevPrmTx, temp
	clr	KP1stDigit
	clr	KP2ndDigit
	ldi	temp, OCTAVE_DEFAULT ; sets Middle C (MIDI note# 0x3c) to the 'Z' key
	sts	OctaveOffset, temp

	ldi	ZH, high (ID*2)  ;LCDmsgFlash - subroutine to put message text from Flash on LCD
	ldi	ZL, low  (ID*2)  ; send:	Temp Reg  -  holds LCD dynamic display address for first char
	ldi	temp, 0        ;       ZL,ZH 	- holds address of message text (* 2) in Flash
	rcall	LCDmsgFlash    ;	Textstring ends with 0x00

	ldi	temp, high (ProtectOff * 2) ; turn off the Memory Protection in the TQ5/TX81Z
	sts	SysExStrH, temp
	ldi	temp, low (ProtectOff * 2)
	sts	SysExStrL, temp
	ldi	temp, 0x03
	sts	ParamNumber, temp
	ldi	temp, 0x00
	sts	New_Value_toTQ, temp
	rcall	SendTX81SysEx
 
	rcall	DoProgramChange

; initialize ADC values and display
	rcall	GetNewADCvalues ; return results in Mega48 SRAM memory locations : prev 0x108,109 new 0x10a,10b
	rcall	SelectParameter ; show parameter name text string and initial value on second LCD line
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
	sbrc	KeyFlags, NewScanCode	; check for new keypress
	rjmp	DoNewScanCode

	sbrc	KeyFlags, Time4ADC	; check for new potentiometer change
	rcall	ADCtimeout

	rjmp	Main			; no keypress or potentiometer change



;********************************************************************************************
; DoNewScanCode   - A new byte from the PS2 keyboard has been fully received, so process it.
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
	rcall	AllNotesOff  ; sends  0xb 0x7b 0x00 (all notes off)
	rjmp	Main

VK1:	cpi	DecodeValue, 0xf2 ; escape
	brne	VK2
	rcall	Get_VCED_Data
	rjmp	Main

VK2:	cpi	DecodeValue, 0x3f
	brlo	PlayNote  ; note offsets are the lowest values in the scan table
	cpi	DecodeValue, 0x80
	brlo	toDoFunctionKey	; any DecodeValue between 0x40 and 0x7f is a function key press
	rjmp	toDoDigit

PlayNote:
	sbrc	KeyFlags,OnOff_flag   ; if OnOff flag is set, then char is a MIDI note number to be played
	rjmp	toPlayMIDInote
	sbrs	KeyFlags,OnOff_flag  ; if OnOff flag is clear, then char is a playing MIDI note to be turned off
	rjmp	toTurnOffMIDInote

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



;====================================================
;
;      Subroutines
;
;====================================================



;**********************************************************************
; ADCtimeout - Time to check for any changes on the two pots
; Get new ADC values from ADC channels zero and one.
; Compare the new ADC values with the previous values.
;**********************************************************************
ADCtimeout:
	cbr	KeyFlags, (1<<Time4ADC) ;turn off flag and store old pot values

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

ADCT_0:	rcall	SelectParameter   ; user changed Parameter pot; so display new param string and value on LCD
	rjmp	ADCT_exit

ADCT_1:   ; NewP_Pot and Prev_PPot are equal, no change in Param pot
	lds	temp, Prev_Value_Pot ; check value pot
	lds	temp2, New_Value_Pot
	cpse	temp, temp2
	rjmp	ADCT_2
	rjmp	ADCT_exit	; if Value pot is unchanged then exit

ADCT_2:
; DoNewValuePot is called when the new ADC (returned from GetNewADCValues) of the Value_Pot is different from the previous ADC of the Value_Pot.
;  Return new sys-ex value to send to the current parameter, or  0xff to mean that the ADC change was not large enough
;  to trigger a sys-ex message to change the parameter setting on the TX81Z/TQ5.
	rcall	DoNewValuePot

ADCT_exit:
	ret


;*****************************************************************************************************
; SelectParameter  -  display text of param on 2nd line, get and display value from SRAM table
;
; called by: ADCtimeout
; send:  New_Param_Pot
; return:
; uses: temp, temp2, ZL:ZH
;*****************************************************************************************************
SelectParameter:
; display text for parameter on second line
	ldi	ZL, low  (ParamText*2)
	ldi	ZH, high (ParamText*2)
	lds	temp, New_Param_Pot ; latest ADC conversion used as offset into string table
	rcall	TimesTen ; send temp: return rBin1L:H
	add	ZL, rBin1L  ; point to new parameter string so that is can be displayed on LCD 2nd line
	adc	ZH, rBin1H
	ldi	temp, 0xc0 ; write parameter text at beginning of second line
	rcall	SendLCDcommand  ; postion the LCD cursor
	ldi	temp2, 10 ; each param text string is ten chars
SP_0:	lpm
	mov	temp, R0 ; get char from flash mem
	rcall	SendLCDdata  ; print to LCD 2nd line
	adiw	ZH:ZL,1
	dec	temp2
	brne	SP_0

; display param value ASCII digits after param text (at second row right corner)
	ldi	ZL, low  (VCEDvalues) ; beginning of current voice's parameters in SRAM
	ldi	ZH, high (VCEDvalues)
	lds	temp, New_Param_Pot ; pot value is the offset into the Param Value table in SRAM
	add	ZL, temp
	brcc	SP_1
	inc	ZH
SP_1:   ld	temp, z ; get param value (using pot0 ADC value as offset) from SRAM table
	ldi	temp2, 0b01001011 ; LCD displayRAM location start for digits
	ldi	temp3, 2
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD

	ret


;*********************************************************************************
;DoNewValuePot  - arrive here if the Value pot has changed since the last
;  Timer1 interrupt.  Get the current parameter and the MaxValue for the current
;  parameter.  Check if the new setting of the Value Pot is different enough from
;  the previous setting in order to justify sending a Parameter_Change sys-ex message.
;  > Write the digits for adjusted New_Value_Pot value to second line/right corner.
;
;  called by: 	ADCtimeout
;  send: 	(SRAM) Prev_Value_Pot, New_Value_Pot (different from Prev), New_Param_Pot
;  return:
;  uses:	ZH:ZL, temp, temp2, r0,
;*********************************************************************************
DoNewValuePot:
	ldi	ZH, high (Param_Max * 2)
	ldi	ZL, low  (Param_Max * 2)
	lds	temp, New_Param_Pot
	add	ZL, temp
	brcc	DNVP_0
	inc	ZH
DNVP_0:
	lpm
	mov	temp2, r0 ; temp2 has the scaled Max Value of the selected parameter in New_Param_Pot
	sts	CurrentMax, r0

	lds	temp, New_Value_Pot
	mov	temp2, r0 ; temp2 has the scaled Max Value of the selected parameter in New_Param_P
	rcall	FindRange
	mov	XL, rBin1L ; adjusted new value (of parameter) adjusted to number of settings (as determined by ParamMax)
	lds	XH, PrevPrmTx
	cpse	XL,XH; if equal, then Value_pot was not turned enough to trigger a sys-ex message to change the parameter setting.
	rjmp	DNVP_1 ; should trigger when N_V_Pot crosses section boundary

DNVP_exit:
	ret

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

	mov	temp, XL; get param value (using pot0 ADC value as offset) from SRAM table
	ldi	temp2, 0b01001110; LCD displayRAM location start for digits
	ldi	temp3, 2
	rcall	LCDdisplayParam ; display adjusted number from New_Value_Pot that will be sent to synth

	rjmp	DNVP_exit


;***********************************************************************************************************************
; FindRange     find number of sections for this param and which sections the Prev and New valuePots readings are in.
;  Calculate an upper and lower range value.
;
; send: temp2 with maxParam of New_Param_Pot, temp with the New_Value_Pot
; return: rBin1L with the size of each section, rBin1H with SectionNumber of temp
;
;***********************************************************************************************************************
FindRange:
	mov	rBin1L, temp2
	clr	rBin1H
	cpi	temp2, 64 ; branch if max value 0-99  0x63
	brsh	FR_1
	cpi	temp2, 32 ; branch if max value 0-63  0x3f
	brsh	FR_2
	cpi	temp2, 16 ; branch if max value 0-31  0x1f
	brsh	FR_3
	cpi	temp2, 8  ; branch if max value 0-15  0x0f
	brsh	FR_4
	cpi	temp2, 4  ; branch if max value 0-7   0x07
	brsh	FR_5
	cpi	temp2, 2  ; branch if max value 0-3   0x03
	brsh	FR_6
	rjmp	FR_7    ; param is a switch (max is 1)

FR_1: ; param max value <= 64.  unchanged ADC value used,
	clr	rBin2L
	inc	rBin2L
	mov	rBin2H, temp2
	rjmp	FR_exit

FR_2: ; param max value between 32 and 63 [0x20-0x3F]. two sections; shift ADC value right once (div by 2)
	lsr	temp
	ldi	temp3, 2
	mov	rBin1H, temp3
	rjmp	FR_exit
;-------------------
FR_3:	lsr	temp
	lsr	temp
	ldi	temp3, 4
	mov	rBin1H, temp3
	rjmp	FR_exit
;------------------
FR_4:	lsr	temp
	lsr	temp
	lsr	temp
	ldi	temp3, 8
	mov	rBin1H, temp3
	rjmp	FR_exit
;------------------
FR_5:	lsr	temp
	lsr	temp
	lsr	temp
	lsr	temp
	ldi	temp3, 16
	mov	rBin1H, temp3
	rjmp	FR_exit
;--------------------
FR_6:	lsr	temp
	lsr	temp
	lsr	temp
	lsr	temp
	lsr	temp
	ldi	temp3, 32
	mov	rBin1H, temp3
	rjmp	FR_exit
;-----------------
FR_7:	cpi	temp, 64
	brsh	FR_7a
	clr	temp
	rjmp	FR_exit
FR_7a:	ldi	temp, 127
	ldi	temp3, 64
	mov	rBin1H, temp3

FR_exit:
	mov	rBin1L, temp
	ret


;*********************************************************************************
;  GetNewADCvalues   - do ADC on voltage channels zero and one
;   send:
;   return:   New_Param_Pot,New_Value_Pot
;   uses:     temp
;**********************************************************************************
GetNewADCvalues:
	ldi	temp, ADC_Param
	rcall	doADC ; return with new value 0-0xff in ADCstore register
	lds	temp, ADCstore
	lsr	temp ; divide by 2, range 0-0x7f for parameter type
	sts	New_Param_Pot, temp
	ldi	temp, ADC_Value
	rcall	doADC ; return with new value 0-0xff in ADCstore register
	lds	temp, ADCstore
	lsr	temp
	sts	New_Value_Pot, temp
	ret


;********************************************************************************************
; doADC  Mega48   called by: GetNewADC  >>  ADC clock set at 125K (8MHz/64)
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


;*******************************************************************************
; TimesTen - multiply 8-bit number by ten for devices without MUL instruction
; send: temp
; return: rBin1L, rBin1H
;*******************************************************************************
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
;
;***********************************************************************************************************
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

	ldi	temp, 0
	ldi	ZL, low  (ClearLine * 2)
	ldi	ZH, high (ClearLine * 2)
	rcall	LCDmsgFlash
	rcall	LCD1msWait

	ldi	temp, (Set_Cursor | LCD_1stLine); 0x80  ; display voice name on LCD
	rcall	SendLCDcommand
	rcall	LCD1msWait
	ldi	temp2, 10
	ldi	YH, high (VoiceName1)
	ldi	YL, low  (VoiceName1)
GVD_1:	ld	temp, Y+
	rcall	SendLCDdata
	dec	temp2
	brne	GVD_1

	ldi	temp, 0b00001101   ;  display on; C=1 cur on; B=1 blink on
	rcall	SendLCDcommand
	rcall	LCD1msWait
	lds	temp, CurrentVoice 
	ldi	temp2, 0b00001011 ; display the Parameter number returned from New_Param_Pot
	ldi	temp3, 2
	rcall	LCDdisplayParam  ; display current voice number on right side of top line of LCD
	ldi	ZH, high (Algorythm)
	ldi	ZL, low  (Algorythm)
	ld	temp, z  ; value of algorythm
	ldi	temp2, 0b00001111
	ldi	temp3, 1
	rcall	LCDdisplayParam ; convert binary number to ASCII and display digits on LCD

	ret


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
	cpi	temp, 8
	brsh	DoFnExit
	sts	New_Value_toTQ, temp
	ldi	temp, high (VCEDchngHdr * 2)
	sts	SysExStrH, temp
	ldi	temp, low (VCEDchngHdr * 2)
	sts	SysExStrL, temp
	ldi	temp, 52
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
;   receives: KeyFlags, DecodeValue (holds note number without Octave Offset)
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
;   receives: KeyFlags, DecodeValue
;   registers used:  KeyFlags, Temp2
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
	sbrc	KeyFlags, KPdigit_flag		; determine if latest keypad digit is 1st or 2nd digit
	rjmp	DD_2Digit
	mov	KP1stDigit, temp
	sbr	KeyFlags, (1<<KPdigit_flag)	; the highest bit acts as a flag for 1st or 2nd digit
	rjmp	DD_Exit

DD_2Digit:
; received 2nd digit, so multiply 1st digit (temp2) by 10 and add 2nd (temp)
	cbr	KeyFlags, (1<<KPdigit_flag)	; remove 2nd digit flag
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
;  AllNotesOff	1) Put Note_Available sentinel into the register bank for all possible notes.
;			All possible note spaces are available because all notes are off.
;		2) Send the ActiveSense MIDI control message to the synthesizer.
;		3) wait 300mS for the lack of susequent ActiveSense message to turn off sound.
;  When the Hold1 is enabled (CapsLock key), then AllNotesOff doesn't work.
;*********************************************************************************************
AllNotesOff:  ;  send AllNotesOff message - 0xB0 0x7B 0x00
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
	rcall	LCD1msWait
	rcall	Get_VCED_Data
	rcall	LCD1msWait
	rcall	Get_ACED_Data
	rcall	LCD1msWait
	rcall	Get_EFEDS_Data
	rcall	LCD1msWait
	rcall	SelectParameter  ; show parameter name text and initial value on 2nd LCD line
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
;;  This main code routine is half of the code needed to have a non-polling output UART.  This allows the main code
;;  to not have to continously read the Data_Ready bit (UDRE) of the transmitter in order to know when the UART is
;;  ready to send another byte.
;;
;;  Send one byte only out the MIDI TX
;;  Put output byte into queue at tail position,
;;    increment tail, and turn on UDRE interrupt
;;
;;  send:   MIDIout (data byte for transmission)
;;  return: nothing
;;  uses:   TxQueue, TXQueueTail, YH, YL, temp
;;
PutMIDIbyte:
	ldi	YH, high (TxQueue)
	ldi	YL, low (TxQueue)
	add	YL, TXQueueTail
	brcc	PMb_0
	inc	YH
PMb_0:	st	y, MIDIout
	mov	temp, TXQueueTail
	inc	temp
	andi	temp, QUEUE_MASK
	mov	TXQueueTail, temp

	lds	temp, UCSR0B ;  enable UDRE interrupt
	ori	temp, (1 << UDRIE0)
	sts	UCSR0B, temp
	ret


;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
;	Interrupt routines
;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



;;*******************************************************************************************************************************
;; UDRE_toMIDI:
;;
;;  This interrupt routine is half of the code needed to have a non-polling output UART.  This allows the main code
;;  to not have to continously read the Data_Ready bit (UDRE) of the transmitter in order to know when the UART is
;;  ready to send another byte.
;;
;;  Set-up:  Put the byte that is to be send out of the MIDI port into the output queue
;;  then enable the UDRE interrupt.  UDRE interrupt will immediately happen, unless the UART is
;;  currently transmitting a character sequence and is not empty.
;;  Head== the first byte in, becoming the first byte out.  Locates where bytes will be taken out of the queue.
;;  Tail == later bytes in.  Locates where bytes will be put into the queue
;;
;;reset:   HT
;;UDREoff  |
;; OutQ:   00  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f
;; ^^^ state of TxQueue when PutMIDIbyte gets called
;;
;;         H   T  put b1 into RAM that Qtail points to,then increment tail and enable UDREirq
;;UDREon   |   |
;; OutQ:   b1  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f
;; ^^^ state of TxQueue when UDRE_toMIDI is entered.  PutMIDIByte routine places tranmit byte into TxQueue.
;;
;;
;; Usually interrupt routines are enabled by the initialization code, and remain inactive until triggered.
;; But the Transmit Data Register Empty IRQ starts as soon as it is enabled and runs until it is specifically disabled.
;; When enabled, UDRE_toMIDI gets the byte pointed to by the queue head and sends it out of the USART by putting it
;; into UDR.   Next it increments the head pointer to the next location in the queue.  Then it checks if the head pointer
;; equals the tail pointer.  If yes, then there is no more new bytes from the PC to send to the MIDI, and it disables
;; itself by turning off the interrupt enable switch.  If head is still behind the tail, then it will trigger again
;; (as soon as the UDRE flag is set, and after one main code instruction) until the head equals the tail of the queue.
;;
;; send:    TxQueue
;; return:
;; uses:  ZH:ZL (saved), irqtemp, UCSRB, UDRIE {USART definitions}
;; calls:
;*******************************************************************************************************************************
UDRE_toMIDI:
	in	SREGtemp, SREG
	push	ZH
	push	ZL
	ldi	ZH, high (TxQueue)
	ldi	ZL, low (TxQueue)
	add	ZL, TxQueueHead ; Locates where the new byte will taken out of the queue.
	brcc	U2M0  ; this code doesn't check whether the Qhead equals Qtail, it assumes that QHead is not equal QTail.
	inc	ZH
U2M0:	ld	irqtemp, Z
	sts	UDR0, irqtemp ; send the byte, that is pointed to by QHead, to the USART

	mov	irqtemp, TxQueueHead ; tail and head pointers in low registers.  ANDI uses high regs only.
	inc	irqtemp
	andi	irqtemp, QUEUE_MASK ; when at end of queue, the Head pointer rolls over to beginning of the queue
	mov	TxQueueHead, irqtemp

	cpse	TxQueueTail, TxQueueHead
	rjmp	U2M_exit
	lds	irqtemp, UCSR0B ; turn off the transmitter to MIDI (this IRQ code)
	andi	irqtemp,  ~ (1 << UDRIE0) ; if head equals tail
	sts	UCSR0B, irqtemp  ; no bytes in output queue to be sent
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
	ldi	irqtemp, ~(10);3;( high (ADCcount)) ; write high first for 16-bit registers
	sts	TCNT1H, irqtemp
	ldi	irqtemp, 0; ~13;( low (ADCcount))
	sts	TCNT1L, irqtemp
	sbr	KeyFlags, (1<<Time4ADC)
	lds	irqtemp, T1Counter
	inc	irqtemp
	sts	T1Counter, irqtemp
	out	SREG, SREGtemp
	reti


;*******************************************************************************************************
; GETSCAN:  ; signal handler for external interrupt falling edge on INT0
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
	lds	irqtemp, UDR0
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
	mov	temp, temp2                                             ;** LCD
	ldi	temp2, 5                                                ;** LCD
	sub	temp2, temp3                                            ;** LCD
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

; Headers for commands to the TX81Z/TQ5
ProtectOff:   .db 0xF0, 0x43, 0x10, 0x24, 0xff,0

; headers for bulk data dumps sent out by the TX81Z/TQ5
VCEDchngHdr:  .db 0xF0, 0x43, 0x10, 0b00010010, 0xff, 0
ACEDchngHdr:  .db 0xF0, 0x43, 0x10, 0b00010011, 0xff, 0
EFEDSchngHdr: .db 0xF0, 0x43, 0x10, 0b00100100, 0xff, 0

; SysEx messages that request the TX81Z/TQ5 to send bulk data
VCEDreq:      .db 0xf0, 0x43, 0x20, 0x03, 0xf7, 0x00 ; returns 101 bytes
ACEDreq:      .db 0xf0, 0x43, 0x20, 0x7e, 0x4c, 0x4d, 0x20, 0x20, 0x38, 0x39, 0x37, 0x36, 0x41, 0x45, 0xf7, 0
EFEDSreq:     .db 0xf0, 0x43, 0x20, 0x7e, 0x4c, 0x4d, 0x20, 0x20, 0x38, 0x30, 0x33, 0x36, 0x45, 0x46, 0xf7, 0 ; ignored by TX81-Z


;;************************************************************************************
;;  MIDI keypress table  --  scancode is offset into table
;
;  table value < 0x3f  keyboard note (octave offset added to get MIDI note value)
;  table value 0x40 - 0x4f   function key
;  table value 0x80 - 0x8f   keypad press
;  table value 0xc0	space (All Notes Off)
;  table value 0xf0 - 0xfe   special key
;  table value 0xff   filler value ignore
;;************************************************************************************
MIDIScanTable:
;    0      1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
.db 0xff,0x48,0xff,0x44,0x42,0x40,0x41,0x4b,0xff,0x49,0x47,0x45,0x43,   1,   0,0xff ; 00-0f
.db 0xff,0xff,  25,0xff,0xf1,   2,0xff,0xff,0xff,0xff,  26,  27,  24,   4,   3,0xff ; 10-1f
.db 0xff,  30,  28,  29,   6,0xff,   5,0xff,0xff,0xc0,  31,0xff,   9,   7,   8,0xff ; 20-2f
.db 0xff,  35,  33,  34,  32,  11,  10,0xff,0xff,0xff,  37,  36,  13,  12,0xff,0xff ; 30-3f
.db 0xff,  38,0xff,  14,  16,  17,  15,0xff,0xff,  40,  42,  39,  41,  18,0xff,0xff ; 40-4f
.db 0xff,0xff,  44,0xff,  19,  20,0xff,0xff,0xff,  43,  45,  21,0xff,  23,0xff,0xff ; 50-5f
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
.db "CoarsFrq_4"
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
.db "CoarsFrq_2"
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
.db "CoarsFrq_3"
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
.db "CoarsFrq_1"
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
.db "PEG_PRt__2" ; 0x58; not implemented on TQ5
.db "PEG_PRt__3" ; not implemented on TQ5
.db "PEG_PLvl_1" ; not implemented on TQ5
.db "PEG_PLvl_2" ; not implemented on TQ5
.db "PEG_PLvl_3" ; not implemented on TQ5
.db "VCED chksm"
.db "FixRatio_4"; AVED values in order
.db "FixRange_4"
.db "FineFreq_4" ;0x60
.db "Osc_Wave_4"
.db "EG_Shift_4"
.db "FixRatio_2"
.db "FixRange_2"
.db "FineFreq_2"
.db "Osc_Wave_2"
.db "EG_Shift_2"
.db "FixRatio_3" ;0x68
.db "FixRange_3"
.db "FineFreq_3"
.db "Osc_Wave_3"
.db "EG_Shift_3"
.db "FixRatio_1"
.db "FixRange_1"
.db "FineFreq_1"
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
.db 0x1f, 0x1f	;AttackRate4: 	0x00	;Decay1Rate4:
.db 0x1f, 0x0f	;Decay2Rate4: 	    	;ReleaseRate4:
.db 0x0f, 0x63	;Decay1Level4:	    	;LevelSens4:
.db 0x03, 0x07	;RateSensv4:  	    	;EnvBiasSens4:
.db 0x01, 0x07	;AmpModEnable4:	0x08  	;KeyVelSens4:
.db 0x63, 0x2f	;OutputLevel4: 		;CoarseFreq4:
.db 0x06, 0x1f	;Detune4:     	    	;AttackRate2:
.db 0x1f, 0x1f	;Decay1Rate2: 	    	;Decay2Rate2:
.db 0x0f, 0x0f	;ReleaseRate2:	0x10  	;Decay1Level2:
.db 0x63, 0x03	;LevelSens2:  	    	;RateSensv2:
.db 0x07, 0x01	;EnvBiasSens2:		;AmpModEnable2:
.db 0x07, 0x63	;KeyVelSens2: 	    	;OutputLevel2:
.db 0x3f, 0x06	;CoarseFreq2: 	0x18  	;Detune2:
.db 0x1f, 0x1f	;AttackRate3:		;Decay1Rate3:
.db 0x1f, 0x0f	;Decay2Rate3: 	    	;ReleaseRate3:
.db 0x0f, 0x63	;Decay1Level3:	    	;LevelSens3:
.db 0x03, 0x07	;RateSensv3:  	0x20	;EnvBiasSens3:
.db 0x01, 0x07	;AmpModEnable3:	    	;KeyVelSens3:
.db 0x63, 0x3f	;OutputLevel3: 	    	;CoarseFreq3:
.db 0x06, 0x1f	;Detune3:      	    	;AttackRate1:
.db 0x1f, 0x1f	;Decay1Rate1:  	0x28  	;Decay2Rate1:
.db 0x0f, 0x0f	;ReleaseRate1: 		;Decay1Level1:
.db 0x63, 0x03	;LevelSens1:   	    	;RateSensv1:
.db 0x07, 0x01	;EnvBiasSens1: 	    	;AmpModEnable1:
.db 0x07, 0x63	;KeyVelSens1:   0x30  	;OutputLevel1:
.db 0x3f, 0x06	;CoarseFreq1:         	;Detune1:
.db 0x07, 0x07	;Algorythm: 	        ;Feedback:
.db 0x63, 0x63	;LFOSpeed:             	;LFODelay:
.db 0x63, 0x63	;PitchModDepth: 0x38   	;AmpModDepth:
.db 0x01, 0x03	;Sync_On_off:  		;LFO_Wave:
.db 0x07, 0x03	;PitchModSense:        	;AmpModSense:
.db 0x30, 0x01	;Transpose:            	;Mode_Poly:
.db 0x0c, 0x01	;PitchBendRange 0x40    ;PortamentoMode:
.db 0x63, 0x63	;PortamentoTime        	;FootVolume:
.db 0x01, 0x01	;DX11_SU:		;DX11_PO:
.db 0x01, 0x63	;DX11_CH:	       	;ModWheelPitch:
.db 0x63, 0x63	;ModWheelAmp:	0x48   	;BreathPitch:
.db 0x63, 0x63	;BreathAmp:  	 	;BrPitchBias:
.db 0x63, 0x7f	;BrAmpBias:  	        ;VoiceName1:
.db 0x7f, 0x7f	;VoiceName2: 	        ;VoiceName3:
.db 0x7f, 0x7f	;VoiceName4: 	0x50	;VoiceName5:
.db 0x7f, 0x7f	;VoiceName6: 	        ;VoiceName7:
.db 0x7f, 0x7f	;VoiceName8: 	        ;VoiceName9:
.db 0x7f, 0x63	;VoiceName10:	        ;DX11_PR1:
.db 0x63, 0x63	;DX11_PR2: 	0x58	;DX11_PR3:
.db 0x63, 0x63	;DX11_PL1: 		;DX11_PL2:
.db 0x63, 0x7f	;DX11_PL3: 	        ;VCEDchksum:
.db 0x01, 0x07	;FixRatio4:	        ;FixRange4:
.db 0x0f, 0x07	;FineFreq4:	0x60	;OscWave4:
.db 0x03, 0x01	;EGshift4: 	     	;FixRatio2:
.db 0x01, 0x07	;FixRange2:	     	;FineFreq2:
.db 0x07, 0x07	;OscWave2: 		;EGshift2:
.db 0x03, 0x01	;FixRatio3:	0x68	;FixRange3:
.db 0x0f, 0x07  ;FineFreq3:	     	;OscWave3:
.db 0x03, 0x01  ;EGshift3: 	     	;FixRatio1:
.db 0x01, 0x0f  ;FixRange1:	     	;FineFreq1:
.db 0x07, 0x03  ;OscWave1: 	0x70   	;EGshift1:
.db 0x07, 0x7f	;Reverb:	     	;FootPitch:
.db 0x7f, 0x7f	;FootAmp:		;ACEDchksum:
.db 0x0f, 0x3f	;Effect_Type:	     	;Effect_Speed:
.db 0x7f, 0x7f	;Effect_Balance: 0x78  	;EFEDSchksum:
.db 0, 0	; 0x7a, 0x7b
.db 0, 0	; 0x7c, 0x7d
.db 0, 0	; 0x7e, 0x7f


; *** in-code program indentification
;         0123456789abcdef
ID: .db  "TX_TwoPot v1.3",0,0
.db      "XTT_owoP t1v3."     ; ID tag viewable as ASCII chars in binary dump from Mega48 memory

