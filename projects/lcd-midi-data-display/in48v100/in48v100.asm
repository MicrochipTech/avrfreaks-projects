;in48.asm   v1.00    Display raw MIDI data on character LCD 20x4   June 04, 2010 
;
;    	Use terminal font with tab size 8 to format code correctly.
;
;	***** Program Description ******
;
;	Received MIDI is placed in the SRAM input buffer.  MIDI clock (0xf8)and Active Sense (0xfe) are filtered out.
;	All MIDI input bytes are immediately re-transmitted to MIDI-Out without filtering.
;	New MIDI values displayed on fourth LCD line.  Each new byte has two hex-char display of the
;	buffer offset for the data, followed by colon and six data bytes (two hex chars and space).
;	After six bytes, lines 1-3 scroll up to lines 0-2 with same values.  Line 3 is cleared and
;	updated buffer offset displayed at left.
;	
;	When the potentiometer is moved to <+2.6v of +5v, the LCD screen is filled with previously received MIDI bytes. 
;	These bytes are displayed as two hex ASCII characters, six per LCD line, 24 bytes total, are displayed on the 
;	20x4 LCD.  The SRAM buffer is 240 bytes in size.  This is 40 rows of 6 bytes each.
;       The value of the ADC (less than 37) is the row number displayed at the LCD top left.  For example, ADC setting
;	of 0x1a (0x3f max value) would display 24 bytes from the SRAM MIDI input buffer starting at 156 (26 * 6). 
;
;         LCD display of received data                               LCD display of new data                          
;           _____________________                                    _____________________                               
;           |00:90 43 60 80 43 00; middle C key press/release        |                    ;
;           |06:b0 01 42 c0 10 f0; mod wheel; prg chng; sysex        |00:90 43 60 80 43 00;
;           |0c:43 20 00 10 7f f7; sysex msg                         |06:B0 01 42 C0 10 F0; 
;           |12:e0 45 00 -- -- --; pitchbend, unused MIDI buffer     |0c:43 20 00  	  ;  15 bytes total in buffer
;           =====================                                    =====================
;
;
; Target CPU >> Atmel AVR Mega48  4K Flash/ 512 SRAM/ 256 EEPROM
; Fuses: high:	11010111  EEPROM preserved through chip erase  [SP12 -wH10111]
;	 low:	11100100  clk/8=off, no clk out, 8MHz Int RC clock, full-swing, slow-rise power  [SP12 -wF11101111]
;
;           Atmel AVR Mega48 microcontroller
;                  ______
;           reset\ |1  28| ADC5 Pot- Row Select  10K-50K +5v to gnd
;        RxD   PD0 |2  27|
;	 TxD   PD1 |3  26|
;      LCD RS  PD2 |4  25| PushSw_C Reset the InputBuffer Pointer
;      LCD E   PD3 |5  24| PushSw_B Halt MIDI input
;    LCD data4 PD4 |6  23| PushSw_A Re-Start MIDI input
;              Vcc |7  22| Gnd
;              Gnd |8  21| Aref
;              PB6 |9  20| AVCC
;              PB7 |10 19| PB5  SCLK
;    LCD data5 PD5 |11 18| PB4  MISO
;    LCD data6 PD6 |12 17| PB3  MOSI
;    LCD data7 PD7 |13 16| PB2
;              PB0 |14 15| PB1
;                  -------
;
;


.nolist
.include "m48def.inc"
.list

; constants for specific AVR device type
.equ	clock		= 8000000     	; system clock Hz; internal R/C
.equ	RAMSTART	= 0x100         ; data memory space
.equ	MIDIbaud	= (clock/ (31250 *16)) - 1
.equ	T1sPerSecond	= 66
.equ	T1_reload	= (clock/1024) / T1sPerSecond
.equ	MSB		= 7
.equ	BUFFER_SIZE	= 240		; 40 LCD rows of 6 MIDI bytes displayed in each row
.equ	BYTES_PER_LINE	= 6
.equ	LONG_MAXCNT  	= 17
.equ	SwitchPINs	= PinC
.equ	SwitchPort	= PortC
.equ	SwitchDDR	= DDRC


;  data lines 4-7 hold four data bits.  Data goes into bits 0-3 of the buffer register
.equ	LCDport		= portd
.equ	LCDcontrol	= portd
.equ	LCDdata		= portd
.equ	LCDddr		= DDRd
.equ	LCD_RS		= 2		;Register Select
.equ	LCD_E		= 3		;Enable
.equ	LCDdata4	= 4
.equ	LCDdata5	= 5
.equ	LCDdata6	= 6
.equ	LCDdata7	= 7
.equ	LCD1stROW	= 0b10000000
.equ	LCD2ndROW	= 0b11000000
.equ	LCD3rdROW	= 0b10010100
.equ	LCD4thROW	= 0b11010100
;LCD instructions
.equ	clr_LCD		= 0b00000001	;clear display
.equ	home_LCD	= 0b00000010	;return home
.equ	entry_mode	= 0b00000110	;set Cursor
.equ	on_LCD		= 0b00001110	;LCD on


; General One-Bit Flags found in GPIOR0 register. SBI/CBI/SBIC/SBIS instructions OK.
.equ	LongPeriodFlag	= 7		; set 4 times a second
.equ	Timer1Flag	= 5		; Set by Timer1 Overflow IRQ routine.  66 times a second
.equ	InputLCDupdate	= 4		; set if updating the LCD with each new MIDI byte arrival
.equ	InputActive	= 3		; set if not ignoring all MIDI input


; Push Switch One-Bit Flags found in GPIOR1 register. Use IN/OUT instructions on entire register.
.equ	ResetPressed	= 2
.equ	HaltPressed	= 1
.equ	StartPressed	= 0


;-------register definitions    	; r0, r1 used for multiply instruction result
.def	SREGtemp	= r2
.def	ToggleReg	= r3
.def	DebounceFlags	= r4

;---------------high registers
.def	temp		= r16
.def	temp2		= r17
.def	temp3		= r18
.def	temp4		= r19
.def	irqtemp		= r20
.def	UARTin		= r21		; holds the latest MIDI byte


;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.DSEG 	; SRAM locations  0x0100 - 0x2ff  512 bytes, stack located at top of SRAM
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
InputBuffer:	.byte  BUFFER_SIZE  ; 0x100 - 0x1ef    40 LCD rows of 6 MIDI bytes per row
.equ	QmaxHi	= high (SRAM_START + BUFFER_SIZE)
.equ	QmaxLo	= low  (SRAM_START + BUFFER_SIZE)

LongPeriodCnt:	.byte 1		; 0x1f0   counts T1 interrupts to mark a 0.25 second period
ASCIIdigits:	.byte 5		; 0x1f1   results of 16-bit_to_ASCII_digits routine
ZeroLoc:	.byte 1		; 0x1f6   always set to zero
MIDIcountLow:	.byte 1		; 0x1f7   used by BufferReset (Yellow button),
MIDIcountHigh:	.byte 1		; 0x1f8
QoutHigh:	.byte 1		; 0x1f9   offset into InputBuffer where Main gets MIDIdata for processing
QoutLow:	.byte 1		; 0x1fa   offset into InputBuffer where Main gets MIDIdata for processing
QinHigh:	.byte 1		; 0x1fb   offset into InputBuffer where new MIDI byte placed
QinLow:		.byte 1		; 0x1fc   offset into InputBuffer where new MIDI byte placed
PrevADC:	.byte 1		; 0x1fd   ADC value before latest ADC conversion
LineNumber:	.byte 1		; 0x1fe   valid values (0-3)
ColumnNumber:	.byte 1		; 0x1ff   valid values (0-19)
LCDbyte:	.byte 1		; 0x200   valid values (0-255)
ByteASChigh:	.byte 1		; 0x201
ByteASClow:	.byte 1		; 0x202
RowByteCount:	.byte 1		; 0x203   valid values are 1 to 6.  Next MIDI input byte's display location ( * 3) on LCD line 4.

.org	0x0210
ScreenBuffer:	.byte 80 	; 0x210  (4 lines x 20 chars)
; last SRAM variable  		; 0x260



;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.CSEG 				; Flash address range  0x000 - 0x7ff
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.ORG		0x000
	rjmp	reset
;Mega48 interrupt vectors
	reti			;0x0001	External Interrupt Request 0
	reti			;0x0002	External Interrupt Request 1
	reti			;0x0003	Pin Change Interrupt Request 0
	reti			;0x0004	Pin Change Interrupt Request 0
	reti			;0x0005	Pin Change Interrupt Request 1
	reti			;0x0006	Watchdog Time-out Interrupt
	reti			;0x0007	Timer/Counter2 Compare Match A
	reti			;0x0008	Timer/Counter2 Compare Match A
	reti			;0x0009	Timer/Counter2 Overflow
	reti			;0x000a	Timer/Counter1 Capture Event
	reti			;0x000b	Timer/Counter1 Compare Match A
	reti			;0x000c	Timer/Counter1 Compare Match B
	rjmp T1_Overflow	;0x000d	Timer/Counter1 Overflow
	reti			;0x000e	Timer/Counter0 Compare Match A
	reti			;0x000f	Timer/Counter0 Compare Match B
	reti			;0x0010	Timer/Counter0 Overflow
	reti			;0x0011	SPI Serial Transfer Complete
	rjmp RxC_IRQ		;0x0012	USART Rx Complete
	reti			;0x0013	USART, Data Register Empty
	reti			;0x0014	USART Tx Complete
	reti			;0x0015	ADC Conversion Complete
	reti			;0x0016	EEPROM Ready
	reti			;0x0017	Analog Comparator
	reti			;0x0018	Two-wire Serial Interface


;**********************************************************************
;   RESET and Main Code
;**********************************************************************
reset:
; initialize stack
	ldi	temp, high (RAMEND) 	; Main program start
	out	SPH, temp		; Set Stack Pointer to top of RAM
	ldi	temp, low (RAMEND)
	out	SPL, temp

;  initialize the internal RC system clock for Mega48
; adjust the internal RC system clock so each MIDI bit is exactly 32 microseconds
	ldi	temp, low  (EEPROMEND)	; address where the OSCCAL value for precision MIDI is located
	out	EEARL, temp
	sbi	EECR, EERE		; strobe to start the read of the EEPROM data from the address in EEAR
	in	temp, EEDR
	ldi	temp2, 0xff
	cpse	temp, temp2		; if 0xff, then a OSCCAL precision value was not written for this individual IC.
	sts	OSCCAL, temp		; if not 0xff, then overwrite the factory OSCCAL with the precision value from EEPROM.

; initialize I/O ports
	ser	temp
   	out	LCDddr, temp
	cbi	LCDport, LCD_E
	in	temp, SwitchDDR		; make sure switches are inputs
	andi	temp, 0b11111000
	out	SwitchDDR, temp
	ldi	temp, 0b00000111	; enable internal pull-up resistors on switch pins
	out	SwitchPort, temp

; initialize USART   ; USCR0C default values OK for 8/N/1
	ldi	temp, high (MIDIbaud)	;Baud rate H
	sts	UBRR0H, temp
	ldi	temp, low (MIDIbaud)	;Baud rate L
	sts	UBRR0L, temp
	ldi	temp, (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0)
	sts	UCSR0B, temp

; LCD init
	rcall	LCD_init

; initialize timer1 for Mega48
	ldi	temp, ~ (high ( T1_reload ))	; long period 1/20th second  0xff
	sts	TCNT1H, temp 			; To do a 16-bit write, the high byte must be written before the low byte
	ldi	temp, ~ (low  ( T1_reload))	; long period 1/20th second  0x86
	sts	TCNT1L, temp
	ldi	temp, (1 << TOIE1)		; enable timer1 interrupt
	sts	TIMSK1, temp
	ldi	temp, ( (1<<CS12) | (0<<CS11) | (1<<CS10) ) ; clock/1024 prescaler on timer1
	sts	TCCR1B, temp

; enable interrupts
	sei	; permit any enabled interrupt to occur by setting Global_Interrupt_Flag in SREG.

; initialize application
	ldi	temp, LONG_MAXCNT
	sts	LongPeriodCnt, temp
	clr	temp
	out	GPIOR1, temp		; General Purpose flag Reg for push switches  flag set=switch being pressed
	out	GPIOR0, temp		; General Purpose flag register in I/O RAM
	sbi	GPIOR0, InputActive	; start device with MIDI_logging function activated
	sbi	GPIOR0, InputLCDupdate	; start with new_MIDI_input_display on
	ldi	temp, 0x20		; clear all SRAM variables
	clr	temp2
	ldi	ZL,  low  (LongPeriodCnt) ; beginning of SRAM variables
	ldi	ZH,  high (LongPeriodCnt)
iap_0:	st	Z+, temp2		
	dec	temp
	brne	iap_0
	ldi	temp, 0			; Next MIDI input byte's display location ( * 3) on LCD line 4.
	sts	RowByteCount, temp	; RBC incremented by DoMIDILCDupdate. Valid values are 1 to 6.
	rcall	Initialize_LCD_display
	rcall	ResetBuffer


	
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;						%
;		MAIN CODE			%
;						%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Main:
	lds	temp,  QoutLow		; check for new MIDI byte in buffer
	lds	temp2, QoutHigh
	lds	temp3, QinLow
	lds	temp4, QinHigh
	cp	temp, temp3
	brne	Main_0
	cp	temp2, temp4		; low bytes of queue in/out pointers are equal, so compare high bytes
	breq	Main_1			; if high bytes are equal, no new MIDI.   Check Main flags

Main_0:	rcall	NewMIDI			; new MIDI byte in buffer

Main_1:	sbic	GPIOR0, Timer1Flag	; 66 times a second
	rcall	DoTimer1

	sbic	GPIOR0, LongPeriodFlag	;  4 times a second
	rcall	Main_LongPeriod

	rjmp	Main




;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

NewMIDI:
; Get the latest MIDI byte arrival into UARTin register
	ldi	ZL, low (INPUTBUFFER)	; low and high address of the MIDI input buffer start
	ldi	ZH, high (INPUTBUFFER)
	lds	XL, QoutLow		; low and high values of the number of previously received MIDI bytes
	lds	XH, QoutHigh
	sts	MIDIcountLow, XL
	sts	MIDIcountHigh, XH
	add	ZL, XL			; get NewMIDI from SRAM location where the Rx interrupt put it
	adc	ZH, XH
	ld	UARTin, z
; adjust the QueueOut pointer to the next input buffer location
	adiw	XH:XL, 1		;  increment MIDI count (16-bit)
	cpi	XL, low (BUFFER_SIZE)	; check to see if next MIDI buffer_SRAM address is above top limit
	brne	NM_0			; if low value of new address is equal to the first address # above top, check high
	cpi	XH, high (BUFFER_SIZE)
	brne	NM_0			; if hi and lo values of new address is equal to the first address # above top,
	clr	XL			; reset count to zero.  Next MIDI input will go to beginning of the SRAM buffer.
	clr	XH
NM_0:	sts	QoutLow, XL
	sts	QoutHigh, XH

; place UARTin register's ASCII  in ScreenBuffer and LCD display if InputLCDupdate flag is set in GPIOR1
	in	temp, GPIOR0
	sbrs	temp, InputLCDupdate	; Flag is determined by setting and movement of the pot.
	; if 7-bit ADC value < 76, flag is clear and ADC value is row number of LCD line 1.
	; if 7-bit ADC value >= 76, flag is set and LCD displays new MIDI bytes as they arrive.
	ret	; InputLCDupdate flag is clear.  Exit without updating LCD display with new MIDI value.

; new MIDI bytes are written to the fourth LCD line only. After six bytes (variable RowByteCount values=1-6)
; the lines 2-4 are scrolled up to 1-3.  Line 4 is blanked and the 8-bit buffer offset is written on the
; left side as two ASCII digits and a colon.
	rcall	DoMIDILCDupdate 	; uses RowByteCount in SRAM
	ret



;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DoTimer1:
	cbi	GPIOR0,  Timer1Flag	;  GPIOR0:5  set by timer1 overflow every 0.015 second
; do 0.015 second (66 Hz) activities here   push-button debouncing
	ret



;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Main_LongPeriod:  ; check switches and pot for changes
	cbi	GPIOR0,  LongPeriodFlag	;  GPIOR0:7  set by timer1 overflow every 1/20th second
; test push switches
	in	temp, SwitchPINs
	andi	temp, 0b00000111
	cpi	temp, 0b00000111
	breq	MLP_0d			; no buttons pressed
	lsr	temp
	brcc	MLP_0a			; branch if Start btn pressed
	lsr	temp
	brcc	MLP_0b			; branch if Halt btn pressed
	lsr	temp
	brcc	MLP_0c			; branch if ResetBuffer btn pressed
	rjmp	MLP_1			; no buttons are being pressed
;==================
MLP_0a:	in	temp, GPIOR1		; check if button is held down more than one LongPeriod cycle
	sbrc	temp, StartPressed
	rjmp	MLP_1			; flag set means keypress held from previous Long cycle, so just exit

	sbr	temp, (1<< StartPressed) ; new keypress, so set flag
	out	GPIOR1, temp
	ldi	temp, LCD2ndROW
	ldi	ZL,LOW  (InRActMsg*2)
	ldi	ZH,HIGH (InRActMsg*2)
	rcall	LCDmsgFlash
	sbr	temp, StartPressed
	sbi	GPIOR0, InputActive
	rjmp	MLP_1
;==================
MLP_0b: in	temp, GPIOR1
	sbrc	temp, HaltPressed	; Halt: New USART bytes not placed into buffer
	rjmp	MLP_1			; flag set means keypress held from previous Long cycle, so just exit

	sbr	temp, (1<< HaltPressed) ; new keypress, so set flag
	out	GPIOR1, temp
	ldi	temp, LCD2ndROW
	ldi	ZL,LOW  (InDActMsg*2)
	ldi	ZH,HIGH (InDActMsg*2)
	rcall	LCDmsgFlash
	cbi	GPIOR0, InputActive
	rjmp	MLP_1
;=================
MLP_0c: in	temp, GPIOR1
	sbrc	temp, ResetPressed	; Buffer pointer Reset to 00
	rjmp	MLP_1			; flag set means keypress held from previous Long cycle, so just exit

	sbr	temp, (1<< ResetPressed) ; new keypress, so set flag
	out	GPIOR1, temp
	ldi	temp, LCD2ndROW
	ldi	ZL,LOW  (BufRstMsg*2)
	ldi	ZH,HIGH (BufRstMsg*2)
	rcall	LCDmsgFlash
	sbi	GPIOR0, ResetPressed
	sbi	GPIOR0, InputActive
	clr	ZL
	clr	ZH
	sts	MIDIcountLow, ZL
	sts	MIDIcountHigh, ZH
	sts	QoutHigh, ZL
	sts	QoutLow, ZL
	sts	QinHigh, ZL
	sts	QinLow, ZL
	rjmp	MLP_1
;==================
MLP_0d:	in	temp, GPIOR1		; all buttons released, so...
	andi	temp, 0b11111000	; turn off switch flags
	out	GPIOR1, temp		; and continue with Analog-to-digital conversion
;===================================================================================================
; check for pot change by doing ADC and comparing it to previous ADC
MLP_1:	ldi	temp, 5 		; ADC channel for the LINES pot
	rcall	DoADC
 	lds	temp, ADCH		; ADCH memory mapped
 	lsr	temp			; divide by 4 for 64 ADC values max
 	lsr	temp
 	lds	temp2, PrevADC		; if new ACD equals previous ADC then exit
 	cp	temp, temp2
 	breq	MLP_4			; both aDCs are the same, so save and exit
	cpi	temp, 36		; > 36, user is not examining old data,
	brsh	MLP_3			; so display MIDI bytes as they arrive
	cbi	GPIOR0, InputLCDupdate  ; <= 36  old data being examined.  Do not update LCD with new MIDI.
	sts	PrevADC, temp

; display on LCD 24 bytes of previous MIDI data that is in the input buffer
; The ADC value holds the starting row for the first LCD line.
	rcall	DisplayOldMIDIdata
	ret

;  display the latest MIDI byte on line 4. If new byte display exceeds the line length
;  then scroll up the entire display one line.
MLP_3:	sbi	GPIOR0, InputLCDupdate	; write each new MIDI byte to LCD

MLP_4:	sts	PrevADC, temp
MLP_exit:
	ret




;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
;		Subroutines
;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


;**********************************************************************************************************
; DisplayOldMIDIdata  - fills LCD with four lines of buffered data.  Since there is no scrolling in this mode,
;	the ScreenBuffer is not updated.
;
; status:  v0.22 doesn't work; entire display flashes
; send:  PrevADC
; return:  --
; uses:  ZH:ZL, temp, temp2, temp3, R0, R1, ByteASChigh, ByteASClow, RowByteCount, LineNumber, ColumnNumber
; calls:  SendLCDcommand, SendLCDdata, Byte2ASCII, DisplayByte, DisplayChar
; called by: MainLongPeriod
;
;  PrevADC actually holds the latest ADC conversion.  This will be the number of the top-most of the four
; rows that will be displayed.  Each row displays the buffer offset of the first byte, followed six byte
; values from the InputBuffer.
;**********************************************************************************************************
DisplayOldMIDIdata:
	ldi	ZH, high (InputBuffer)		; 0x01 buffer at start of SRAM
	ldi	ZL, low  (InputBuffer)		; 0x00

	ldi	temp, 0xff
	sts	LineNumber, temp
	lds	temp, PrevADC
	ldi	temp2, 6
	mul	temp, temp2			; low result in R0
	add	ZL, r0				; ZH:ZL points to byte in InputBuffer that is being displayed
	adc	ZH, r1

; display a single line on the LCD using pre-received MIDI data.  Loop four times.

DBf_line:
	sts	LCDbyte, ZL			; InBuf offset <address> gets displayed on LCD columns 0,1
	lds	temp, LineNumber		; LineNumber unchanged for entire line display routine
	inc	temp				; valid values 0-3; pre-load with 0xff, first pass rolls-over to 0x00
	cpi	temp, 4
	breq	DBf_exit   			; finished updating screen with 24 data bytes.
	sts	LineNumber, temp		; store LN so it is valid for next loop
	clr	temp
	sts	ColumnNumber, temp
	rcall	DisplayByte			; LCDbyte, LineNumber, ColumnNumber.  Displays two ASCII chars for hex value of LCDbyte.
	ldi	temp, 2
	sts	ColumnNumber, temp
	ldi	temp, ':'
	sts	LCDbyte, temp
	rcall	DisplayChar 			; LCDbyte, LineNumber, ColumnNumber.  Displays one char, contents of LCDbyte.

	clr	temp				; set up display of six data bytes from InputBuffer
	sts	RowByteCount, temp
DBf_byte:					; Six interation loop. Display a byte pointed to by ZH:ZL on the LCD.
	lds	temp, RowByteCount
	inc	temp
	sts	RowByteCount, temp
	ldi	temp2, 3
	mul	temp, temp2
	sts	ColumnNumber, R0		; low result in R0. Valid values: 3,6,9,12,15,18

	ld	temp, Z+			; This is pre-received MIDI data in the InputBuffer
	sts	LCDbyte, temp
	rcall	DisplayByte			; LCDbyte, LineNumber, ColumnNumber.  Displays two ASCII chars for hex value of LCDbyte.

	lds	temp, RowByteCount		; check if end at LCD line
	cpi	temp, 6				; if yes, then prep next line
	breq	DBf_line			; if no, then display a space between the data bytes

	ldi	temp, ' '
	sts	LCDbyte, temp
	lds	temp, ColumnNumber
	subi	temp, -2			; space should be two positions right of the last data byte start
	sts	ColumnNumber, temp
	rcall	DisplayChar			; LCDbyte, LineNumber, ColumnNumber.  Displays one char, contents of LCDbyte.
	rjmp	DBf_byte			; six loop interations
	rjmp	DBf_line			; start next line

DBf_exit:
	ret



;********************************************************************************************************************
; DoMIDILCDupdate  - Get new MIDI byte's ASCII chars. Determine LCD column for chars and offset for ScreenBuffer.
;		Put chars (and space if less than six bytes on the LCD line) on LCD display and ScreenBuffer.
;		If six bytes are displayed on Line 4 when new MIDI arrives, scroll up, place new offset (2 chars)
;		and new MIDI chars on the LCD and ScreenBuffer.
;
; send:    UARTin,  {SRAM} RowByteCount, ScreenBuffer
; return:  --
; uses:    ZH:ZL, temp, temp2, temp3, R0:R1, {SRAM} ByteASChigh, ByteASClow, LineNumber, ColumnNumber, LCDbyte
; calls:   SendLCDcommand, SendLCDdata, Byte2ASCII, XYScrBufByte
;********************************************************************************************************************
DoMIDILCDupdate:
	lds	temp, RowByteCount
	inc	temp			; valid values are 1-6 after increment
	cpi	temp, 7
	breq	DMLC_1			; reached end of line 4 so scroll up display

; update Line 4 in the ScreenBuffer
	sts	RowByteCount, temp
	ldi	temp2, 3		; get column offset for new MIDI byte's digits
	mul	temp, temp2		; valid values in R0 after multiply: 3,6,9,12,15,18
	ldi	ZH, high (ScreenBuffer+60) ; ZH:ZL points to beginning of Line 4 in ScreenBuffer
	ldi	ZL, low  (ScreenBuffer+60)
	add	ZL, R0			; offset for two ASCII digits on fourth line in ScreenBuffer
	adc	ZH, R1
; place two hex ASCII digits into Line 4 of the ScreenBuffer starting at correct column
	mov	temp3, UARTin
	rcall	Byte2ASCII  		; send temp3, return ByteASChigh: ByteASClow (SRAM)
	lds	temp, ByteASChigh
	st	Z+, temp
	lds	temp, ByteASClow
	st	Z+, temp
	mov	temp, r0
	cpi	temp, 18
	breq	DMLC_0			; don't write space to ScreenBuffer
	ldi	temp, ' '
	st	Z, temp

; update line 4 of the LCD display.  write two hex ASCII digits and space (if not at end of line)
DMLC_0:	ldi	temp, LCD4thROW
	add	temp, R0
	rcall	sendLCDcommand		; reposition the LCD display cursor
	lds	temp, ByteASChigh
	rcall	sendLCDdata
	lds	temp, ByteASClow
	rcall	sendLCDdata
	mov	temp, r0
	cpi	temp, 18
	brne	DMLC_0a		; don't write space to LCD display if at end of line
	rjmp	DMLC_2
DMLC_0a:
	ldi	temp, ' '
	rcall	sendLCDdata
	rjmp	DMLC_2

; previous MIDI input filled LCD Line 4, so this byte requires a scroll up. 
;  Also a display of InputBuffer offset and the new byte.
DMLC_1:	ldi	temp, 1
	sts	RowByteCount, temp
	rcall	ScrollupLCD
; do new Line 4 in the ScreenBuffer
	lds	temp3, MIDIcountLow	; start LCD Line 4 with two chars (buffer offset for first of six bytes displayed)
	rcall	Byte2ASCII
	ldi	temp, 3
	sts	LineNumber, temp	; valid values (0-3)
	ldi	temp, 0			; valid values (0-19)
	sts	ColumnNumber, temp
	lds	temp, ByteASChigh
	sts	LCDbyte, temp
	rcall	XYScrBufByte
	ldi	temp, 1			; valid values (0-19)
	sts	ColumnNumber, temp
	lds	temp, ByteASCLow
	sts	LCDbyte, temp
	rcall	XYScrBufByte
	ldi	temp, 2			; valid values (0-19)
	sts	ColumnNumber, temp
	ldi	temp, ':'
	sts	LCDbyte, temp
	rcall	XYScrBufByte
;  new row setup finished, now put MIDI byte value into the ScreenBuffer
	mov	temp3, UARTin
	rcall	Byte2ASCII
	ldi	temp, 3
	sts	ColumnNumber, temp
	lds	temp, ByteASChigh
	sts	LCDbyte, temp
	rcall	XYScrBufByte

	ldi	temp, 4
	sts	ColumnNumber, temp
	lds	temp, ByteASCLow
	sts	LCDbyte, temp
	rcall	XYScrBufByte

 	ldi	temp, 5
 	sts	ColumnNumber, temp
	ldi	temp, ' '
 	sts	LCDbyte, temp
 	rcall	XYScrBufByte

;==================     finished ScreenBuffer update, now do LCD display
; do new Line 4 on the LCD display
	lds	temp, MIDIcountLow	; start LCD Line 4 with two ASCII digits (buffer offset for first of six bytes displayed)
	sts	LCDbyte, temp
	ldi	temp, 3
	sts	LineNumber, temp	; valid values (0-3)
	ldi	temp, 0			; valid values (0-19)
	sts	ColumnNumber, temp
	rcall	DisplayByte

	ldi	temp, LCD4thROW
	subi	temp, -2
	rcall	sendLCDcommand		; reposition the LCD display cursor
	ldi	temp, ':'
	rcall	sendLCDdata

;  new row setup finished, now put MIDI byte value into the ScreenBuffer
	sts	LCDbyte, UARTin
	ldi	temp, 3
	sts	ColumnNumber, temp
	rcall	DisplayByte

   	ldi	temp, LCD4thROW
	subi	temp, -5
	rcall	sendLCDcommand		; reposition the LCD display cursor
	ldi	temp, ' '
	rcall	sendLCDdata

DMLC_2:	ret



;******************************************************************
; GetRowCount - Returns row number (starting at 00)
;
; send: 	MIDIcountHigh:Low
; return: 	temp= row number starting (6 bytes per row)
;	   	from current buffer beginning to current ptr
; uses: 	ZH:ZL(non-destructive)
;
;******************************************************************
GetRowCount:
	push	ZH
	push	ZL
	lds	ZL, MIDIcountLow
	lds	ZH, MIDIcountHigh
	ser	temp
GLC_0:	inc	temp
	sbiw	ZH:ZL,6
	brcc	GLC_0
	pop	ZL
	pop	ZH
	ret



;********************************************************************************************
;  Byte2ASCII  Convert a byte to two ASCII characters.
;
; send:		temp3
; return: 	{SRAM} ByteASChigh, ByteASClow
; uses:		temp2, temp
; calls:	Hex2ASCII
;********************************************************************************************
Byte2ASCII:
	push	temp2
	push	temp
	mov	temp2, temp3
	swap	temp2
	andi	temp2, 0x0f
	rcall	Hex2ASCII
	sts	ByteASChigh, temp
	mov	temp2, temp3
	andi	temp2, 0x0f
	rcall	Hex2ASCII
	sts	ByteASClow, temp
	pop	temp
	pop	temp2
	ret
;********************************************************************************************
; Hex2ASCII  Convert a binary value 0x00~0x0f to an ASCII character.
;
; send:	  temp2 (binary in bits 0-3)
; return: temp with ASCII value, letters are uppercase.  temp2 returns unchanged.
; uses registers: ZH, ZL (non-destructive), temp
;********************************************************************************************
Hex2ASCII:
	push	ZH
	push	ZL
	ldi	ZH, high (hexchars * 2)
	ldi	ZL, low  (hexchars * 2)
	add	ZL, temp2
H2A0:	lpm	temp, z
	pop	ZL
	pop	ZH
 	ret
hexchars: .db "0123456789ABCDEF"



;********************************************************************************************
;ResetBuffer  - Set queue pointers to zero. Fill InputBuffer and ScreenBuffer with 0xAA.
;
; send:	-
; return: -
; uses:   ZH:ZL, YH:YL, XH:XL, temp2, temp
;********************************************************************************************
ResetBuffer:
	clr	temp
	sts	QoutLow, temp
	sts	QoutHigh, temp
	sts	QinLow, temp
	sts	QinHigh, temp
	ldi	ZH, high (InputBuffer)
	ldi	ZL, low  (InputBuffer)
	ldi	XH, high (ScreenBuffer)
	ldi	XL, low  (ScreenBuffer)

	ldi	YH,  high (InputBuffer + BUFFER_SIZE)
	ldi	YL,  high (InputBuffer + BUFFER_SIZE)
	ldi	temp, 0xaa
RsBf_1:	st	Z+, temp  ; fill input_buffer and LCD screen_buffer with sentinal characters
	st	x+, temp
	cp	ZH, YH
	brne	RsBf_1
	cp	ZL, YL
	brne	RsBf_1
	ret



;************************************************************************************************
;   doADC   - set up, run analog-to-digital convertor, then return high bits (9-2) of result
;   AREF pin external reference (0 -- +5 volts); 250KHz ADC clock
;
;   send:	temp = The ADC channel
;   return:	ADCH = left-aligned upper 8 bits of conversion
;************************************************************************************************
doADC:	; Mega48 version
	andi	temp,  0b00001111 ; preserve MUX bits; reference is VCC
	ori	temp, (0<<REFS1) | (0<<REFS0) | (1<<ADLAR)
	sts	ADMUX, temp
; ADEN = ADC enable;  ADIE = irq enable; clock/32 for ADCclk
 	ldi	temp, (1 << ADEN) | (1 << ADPS2) | (0 << ADPS1)  | (1 << ADPS0)
	sts	ADCSRA, temp
	sbr	temp, (1<<ADSC)
	sts	ADCSRA,  temp ; start conversion
doADC0:	lds	temp, ADCSRA
	sbrc	temp, ADSC ; set means ADC conversion still in progress
	rjmp	doADC0  ; conversion takes 13 ADC clocks  13*16 sys clocks @20uS
	ret



;***********************************************************************************************
; Initialize_LCD_display   - power-up/reset display
;
; uses:		temp, ZH:ZL  {SRAM} LineNumber, ColumnNumber
; calls:	LCDmsgFlash; XYScrBufStr
;***********************************************************************************************
Initialize_LCD_display:
	ldi	temp, LCD1stROW 	; 0x80 position cursor to beginning of first line
	ldi	ZL,LOW  (LineOne*2)	; low pointer points to begin table
	ldi	ZH,HIGH (LineOne*2)	; high pointer pointing to begin table
	rcall	LCDmsgFlash

	ldi	ZL,LOW  (LineOne*2)	; low pointer points to begin table
	ldi	ZH,HIGH (LineOne*2)	; high pointer pointing to begin table
	clr	temp
	sts	LineNumber, temp
	sts	ColumnNumber, temp
	rcall	XYScrBufStr
;---
	ldi	temp, LCD2ndROW 	; 0xc0 position cursor to beginning of second line
	ldi	ZL,LOW  (LineTwo*2)	; low pointer points to begin table
	ldi	ZH,HIGH (LineTwo*2)	; high pointer pointing to begin table
	rcall	LCDmsgFlash

	ldi	ZL,LOW  (LineTwo*2)	; low pointer points to begin table
	ldi	ZH,HIGH (LineTwo*2)	; high pointer pointing to begin table
	ldi	temp, 1
	sts	LineNumber, temp
	ldi	temp, 0
	sts	ColumnNumber, temp
	rcall	XYScrBufStr
;---
	ldi	temp, LCD3rdROW 	; 0x94 position cursor to beginning of third line
	ldi	ZL,LOW  (LineThree*2)	; low pointer points to begin table
	ldi	ZH,HIGH (LineThree*2)	; high pointer pointing to begin table
	rcall	LCDmsgFlash

	ldi	ZL,LOW  (LineThree*2)	; low pointer points to begin table
	ldi	ZH,HIGH (LineThree*2)	; high pointer pointing to begin table
	ldi	temp, 2
	sts	LineNumber, temp
	ldi	temp, 0
	sts	ColumnNumber, temp
	rcall	XYScrBufStr
;---
	ldi	temp, LCD4thROW 	; 0xd4 position cursor to beginning of fourth line
	ldi	ZL,LOW  (Start00*2)	; low pointer points to begin table
	ldi	ZH,HIGH (Start00*2)	; high pointer pointing to begin table
	rcall	LCDmsgFlash

  	ldi	ZL,LOW  (Start00*2)	; low pointer points to begin table
 	ldi	ZH,HIGH (Start00*2)	; high pointer pointing to begin table
 	ldi	temp, 3
 	sts	LineNumber, temp
 	ldi	temp, 0
	sts	ColumnNumber, temp
	rcall	XYScrBufStr

	ret


;***************************************************************************
; DisplayByte - LCDbyte as two ASCII chars at LineNumber, ColumnNumber
;
; send:  LCDbyte, LineNumber, ColumnNumber, ScreenBuffer
; uses:	 temp, temp2, temp3, ZH:ZL, ByteASChigh, ByteASClow
; calls: Byte2ASCII, SendLCDcommmand, SendLCDdata
;***************************************************************************
DisplayByte:
	push	ZH
	push	ZL
	ldi	ZL, low  (DBRS *2)   ; row start list
	ldi	ZH, high (DBRS *2)
	lds	temp, LineNumber
	clr	temp2
	add	ZL, temp
	adc	ZH, temp2
	lpm	temp, z
	lds	temp2, ColumnNumber
	add	temp, temp2
	rcall	SendLCDcommand
	rcall	LCD1msWait
	lds	temp3, LCDbyte
	rcall	Byte2ASCII
	lds	temp, ByteASChigh
	rcall	SendLCDdata
	lds	temp, ByteASClow
	rcall	SendLCDdata

	ldi	ZL, low  (ScreenBuffer)
	ldi	ZH, high (ScreenBuffer)
	lds	temp, LineNumber
	ldi	temp2, 20
	mul	temp, temp2
	lds	temp, ColumnNumber
	add	R0, temp
	add	ZL, R0
	adc	ZH, R1

	lds	temp, ByteASChigh
	st	Z+, temp
	lds	temp, byteASClow
	st	Z+, temp
	pop	ZL
	pop	ZH
	ret

 DBRS: .db     0b10000000,0b11000000, 0b10010100, 0b11010100



;***************************************************************************
; DisplayChar - LCDbyte put on LCD at LineNumber, ColumnNumber
;
; send:  LCDbyte, LineNumber, ColumnNumber, ScreenBuffer
; uses:	 temp, temp2, temp3, ZH:ZL, ByteASChigh, ByteASClow
; calls: Byte2ASCII, SendLCDcommmand, SendLCDdata
;***************************************************************************
DisplayChar: ; send:  LCDbyte, LineNumber, ColumnNumber, ScreenBuffer
	push	ZH
	push	ZL
	ldi	ZL, low  (DBRS<<1)   ; row start list
	ldi	ZH, high (DBRS<<1)
	lds	temp, LineNumber
	clr	temp2
	add	ZL, temp
	adc	ZH, temp2
	lpm	temp, z
	lds	temp2, ColumnNumber
	add	temp, temp2
	rcall	SendLCDcommand
	rcall	LCD1msWait
	lds	temp, LCDbyte
	rcall	SendLCDdata

	ldi	ZL, low  (ScreenBuffer)
	ldi	ZH, high (ScreenBuffer)
	lds	temp, LineNumber
	ldi	temp2, 20
	mul	temp, temp2
	lds	temp, ColumnNumber
	add	R0, temp
	add	ZL, R0
	adc	ZH, R1
	lds	temp, LCDbyte
	st	Z+, temp
	pop	ZL
	pop	ZH
	ret



;******************************************************************************
;     <<<  LCD Routines >>>>						;** LCD
;******************************************************************************
; LCD_init:								;** LCD
; LCDmsgFlash - subroutine to put message text from Flash on LCD	;** LCD
; LCD1msWait:								;** LCD
; SendLCDcommand: ; transmitting char is in Temp register		;** LCD
; SendLCDdata:								;** LCD
; LCDdigits - display numbers on LCD					;** LCD
; LCDdisplayParam: 							;** LCD
; XYScrBufStr:   x=LineNumber, y=ColumnNumber, ZH:ZL stringPtr (0 end)	;** LCD
; XYScrBufByte:                                                         ;** LCD
; ScrollUpLCD:                                                          ;** LCD
;									;** LCD
;***********************************************************************;** LCD
;  Initialize the LCD display controller				;** LCD
;***********************************************************************;** LCD
LCD_init:								;** LCD
	ldi	temp, 15						;** LCD
initWait0:								;** LCD
	rcall	LCD1msWait						;** LCD
	dec	temp							;** LCD
	brne	initWait0 						;** LCD
									;** LCD
	cbi	LCDport, LCD_RS  ; set to command/status register	;** LCD
	ldi	Temp, 0b00110000 ; first write of 0x03 to LCD_d7:d4	;** LCD
	rcall	Control_E_Blink						;** LCD
	ldi	temp, 10						;** LCD
initWait1:								;** LCD
	rcall	LCD1msWait						;** LCD
	dec	temp							;** LCD
	brne	initWait1						;** LCD
									;** LCD
	ldi	Temp, 0b00110000  					;** LCD
	rcall	Control_E_Blink	; second write of 0x03 to LCD_d7:d4	;** LCD
	ldi	temp, 10						;** LCD
initWait2:								;** LCD
	rcall	LCD1msWait						;** LCD
	dec	temp							;** LCD
	brne	initWait2						;** LCD
									;** LCD
	cbi	LCDport, LCD_E						;** LCD
	ldi	temp, 0b00110000  ; third write of 0x03 to LCD_ d7:d4	;** LCD
	out	LCDport, temp						;** LCD
	rcall	Control_E_Blink 					;** LCD
									;** LCD
;Bit 5 set:  data length=4bits (bit4 clear), #lines=2 (bit 3 set),	;** LCD
;	font type=5x8 (bit 2 clear) 					;** LCD
	ldi	temp, 0b00100000 ;0x20					;** LCD
	rcall	Control_E_Blink	; fourth write				;** LCD
									;** LCD
;Bit 2 set:  increment cursor (bit 1 set) when new char displayed	;** LCD
	ldi	temp, 0b00000110 ;  0x06				;** LCD
	rcall	SendLCDcommand						;** LCD
	rcall	Blink							;** LCD
									;** LCD
;Bit 3 set:  display on (bit 2 set),					;** LCD
;	cursor off (bit 1 clr),cursor blink off (bit 0 clr)		;** LCD
	ldi	temp, 0b00001100					;** LCD
	rcall	SendLCDcommand  					;** LCD
	rcall	Blink							;** LCD
									;** LCD
;Bit 1 set:  Return home  needed for display				;** LCD
	ldi	temp, home_LCD ; 0x02					;** LCD
	rcall	SendLCDcommand						;** LCD
	ldi	temp, 2							;** LCD
initWait3:								;** LCD
	rcall	LCD1msWait						;** LCD
	dec	temp							;** LCD
	brne	initWait3						;** LCD
									;** LCD
;Bit 0 set:  clear display						;** LCD
	ldi	Temp, clr_LCD ; 0x01 					;** LCD
	rcall	SendLCDcommand						;** LCD
	rcall	Blink							;** LCD
;*********								;** LCD
	ret								;** LCD
;----------------------- end of LCD_init				;** LCD
									;** LCD
Control_E_Blink:		; called by LCD_init			;** LCD
	out	LCDport, temp						;** LCD
	cbi	LCDport, LCD_E   					;** LCD
	rcall	Blink		; delay @70 microseconds		;** LCD
	sbi	LCDport, LCD_E   					;** LCD
	rcall	Blink							;** LCD
	cbi	LCDport, LCD_E   					;** LCD
	rcall	Blink							;** LCD
	ret								;** LCD
									;** LCD
;***********************************************************************;** LCD
;  Send data to LCD display controller					;** LCD
;***********************************************************************;** LCD
SendLCDcommand: ; transmitting char is in Temp register			;** LCD
	push	temp2							;** LCD
 	cbi	LCDport, LCD_RS	; LCD command   			;** LCD
	rjmp	LCD_write						;** LCD
SendLCDdata:								;** LCD
	push	temp2							;** LCD
	sbi	LCDport, LCD_RS	; LCD data				;** LCD
	nop	; both NOPs needed at 8MHz clock			;** LCD
	nop								;** LCD
LCD_write: 								;** LCD
	push	temp	; 'temp' register holds char to be written 	;** LCD
	in	temp2, LCDport ; read output latch value, not pins 	;** LCD
	andi	temp2, (1<<LCD_RS) | (1<<LCD_E); 0b00001100		;** LCD
	andi	temp, 0b11110000 ; isolate upper nibble of data		;** LCD
	or	temp2, temp ;  LCD data bits 7-4 in Temp2 bits 5-2	;** LCD
	out	LCDport, temp2						;** LCD
	rcall	E_Blink	; strobe E high for 70 microseconds		;** LCD
	pop	temp							;** LCD
	andi	temp2, (1<<LCD_RS) | (1<<LCD_E) ; clear the old data 	;** LCD
	swap	temp	; isolate lower nibble of data			;** LCD
	andi	temp, 0b11110000  					;** LCD
	or	temp2, temp						;** LCD
	out	LCDport, temp2						;** LCD
	rcall	E_Blink		; strobe E high for 70 microseconds	;** LCD
	pop	temp2  							;** LCD
	ret								;** LCD
									;** LCD
E_Blink:								;** LCD
	sbi	LCDport, LCD_E						;** LCD
	rcall	Blink	; delay @70 microseconds			;** LCD
	cbi	LCDport, LCD_E						;** LCD
	rcall	Blink							;** LCD
	ret								;** LCD
									;** LCD
									;** LCD
;***********************************************************************;** LCD
;LCDmsgFlash - subroutine to put message text from Flash on LCD		;** LCD
;									;** LCD
; send:	Temp Reg  -  holds LCD dynamic display address for first char   ;** LCD
;	ZH,ZL     -  holds address of message text (* 2) in Flash	;** LCD
;	Textstring ends with 0x00					;** LCD
;***********************************************************************;** LCD
LCDmsgFlash:		 						;** LCD
	push	ZH							;** LCD
	push	ZL							;** LCD
	ori	temp, 0b10000000					;** LCD
	rcall	SendLCDcommand ; set LCD cursor position		;** LCD
	rcall	LCD1msWait						;** LCD
LCDmsgF0:								;** LCD
	lpm	temp, z	; LPM does not change any flags			;** LCD
	tst	temp							;** LCD
	breq	LCDmsgF_exit						;** LCD
	rcall	sendLCDdata						;** LCD
	inc	ZL							;** LCD
	brne	LCDmsgF0						;** LCD
	inc	ZH							;** LCD
	rjmp	LCDmsgF0						;** LCD
LCDmsgF_exit:								;** LCD
	pop	ZL   							;** LCD
	pop	ZH							;** LCD
	ret								;** LCD
									;** LCD
									;** LCD
;***********************************************************************;** LCD
; Subroutine wait loops							;** LCD
;***********************************************************************;** LCD
									;** LCD
LCD1msWait: ; 1.73 mS [8MHz]:13890 cycles: ((18*768)-1)+((18*3)-1)+14	;** LCD
	push	temp 	 ; 2 cycles					;** LCD
	push	temp2	 ; 2 cycles					;** LCD
	ldi	temp, 18 ; 1 cycle 					;** LCD
	clr	temp2	 ; 1 cycle					;** LCD
LIW0:	dec	temp2	 ; 767 cycles	(3*256)-1			;** LCD
	brne	LIW0 							;** LCD
	dec	temp 	 ; 1 cycle					;** LCD
	brne	LIW0 	 ; ((18*768)-1) + ((18*3)-1) 			;** LCD
	pop	temp2	 ; 2 cycles					;** LCD
	pop	temp     ; 2 cycles					;** LCD
	ret		 ; 4 cycles					;** LCD
									;** LCD
Blink:	push	Temp 							;** LCD
	ldi	temp, 180 ; 545 cycles					;** LCD
blink1:	dec	Temp	; @70 uSec delay (8MHz clock)			;** LCD
	brne	blink1							;** LCD
	pop	Temp  							;** LCD
	ret								;** LCD
									;** LCD
									;** LCD
;***********************************************************************;** LCD
; LCDdigits - display numbers on LCD					;** LCD
; send - ASCIIdigits(5 max), followed by zero (initialized as ZeroLoc)	;** LCD
;	 temp = LCD location for first digit				;** LCD
;	 temp2 = offset into ASCIIdigits (5 - #_of_digits displayed)	;** LCD
; returns - nothing							;** LCD
; uses - ZH,ZL (non-destructive)					;** LCD
;***********************************************************************;** LCD
LCDdigits:								;** LCD
	push	ZH							;** LCD
	push	ZL							;** LCD
	ldi	ZH, high (ASCIIdigits)					;** LCD
	ldi	ZL, low (ASCIIdigits)					;** LCD
	tst	temp2		 					;** LCD
	breq	Ld1							;** LCD
Ld0:	inc	ZL							;** LCD
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
	pop	ZL							;** LCD
	pop	ZH							;** LCD
	ret								;** LCD
									;** LCD
;***********************************************************************;** LCD
; XYScrBufByte								;** LCD
; send:  LineNumber, ColumnNumber, LEDbyte				;** LCD
; return:								;** LCD
; uses:  XH:XL, temp, temp2						;** LCD
; calls:								;** LCD
;********************************************************************** ;** LCD
XYScrBufByte:								;** LCD
 	rcall	MoveLCDbufPtr						;** LCD
	lds	temp, LCDbyte						;** LCD
	st	x, temp							;** LCD
	ret								;** LCD
									;** LCD
MoveLCDbufPtr:   ; move LCD buffer pointer to Line#,Col#		;** LCD
	lds	temp, LineNumber					;** LCD
	ldi	temp2, 20  						;** LCD
	mul	temp, temp2						;** LCD
	ldi	XH, high (ScreenBuffer)					;** LCD
	ldi	XL, low  (ScreenBuffer)					;** LCD
	add	XL,R0							;** LCD
	adc     XH,R1							;** LCD
	lds	temp, ColumnNumber					;** LCD
	clr	temp2    						;** LCD
	add	XL, temp 						;** LCD
	adc	XH, temp2						;** LCD
	ret								;** LCD
;									;** LCD
;***********************************************************************;** LCD
; XYScrBufStr  -  load string from flash into LCD screen buffer         ;** LCD
; send:  LineNumber (0-3), ColumnNumber, ZH:ZL ptr to str (0 terminate)	;** LCD
; return:								;** LCD
; uses:  ZH:ZL, temp, temp2						;** LCD
;********************************************************************** ;** LCD
XYScrBufStr:								;** LCD
 	rcall	MoveLCDbufPtr						;** LCD
Sbs_0:	lpm	temp, Z+						;** LCD
	tst	temp    						;** LCD
	breq	Sbs_1   						;** LCD
	st	X+, temp						;** LCD
	rjmp	Sbs_0   						;** LCD
Sbs_1:	ret								;** LCD
									;** LCD
;***********************************************************************;** LCD
; ScrollUpLCD   LCD lines 2,3,4 moved to 1,2,3; then 4th line cleared   ;** LCD
; send:   								;** LCD
; uses:   ZH:ZL, YH:YL, temp, temp2, ScreenBuffer			;** LCD
;********************************************************************** ;** LCD
ScrollUpLCD:								;** LCD
	ldi	ZH, high (ScreenBuffer)					;** LCD
	ldi	ZL, low  (ScreenBuffer)					;** LCD
	mov	YH, ZH  						;** LCD
	mov	YL, ZL  						;** LCD
	clr	temp2   						;** LCD
	ldi	temp, 20						;** LCD
	add	ZL, temp						;** LCD
	adc	ZH, temp2						;** LCD
	ldi	temp2, 60						;** LCD
Scl_0:	ld	temp, Z+ 						;** LCD
	st	Y+, temp 						;** LCD
	dec	temp2    						;** LCD
	brne	Scl_0							;** LCD
; fill ScreenBuffer line 4 with spaces					;** LCD
	ldi	ZH, high (ScreenBuffer+60) ; ZH:ZL points to 4th line   ;** LCD
	ldi	ZL, low  (ScreenBuffer+60) ; in the screen buffer	;** LCD
	ldi	temp, ' '						;** LCD
	ldi	temp2, 20						;** LCD
Scl_0a:	st	z, temp  						;** LCD
	dec	temp2    						;** LCD
	brne	Scl_0a   						;** LCD
									;** LCD
; write LCD display's first three rows from screen buffer		;** LCD
	ldi	temp, LCD1stROW  ;0b10000000				;** LCD
	rcall	SendLCDcommand						;** LCD
	ldi	ZH, high (ScreenBuffer)					;** LCD
	ldi	ZL, low  (ScreenBuffer)					;** LCD
	ldi	temp2, 20						;** LCD
Scl_1:	ld	temp, Z+   						;** LCD
	rcall	SendLCDdata						;** LCD
	dec	temp2      						;** LCD
	brne	Scl_1      						;** LCD
; move line 2 from buffer to LCD display				;** LCD
	ldi	temp, LCD2ndROW   ;0b11000000				;** LCD
	rcall	SendLCDcommand						;** LCD
	ldi	temp2, 20  						;** LCD
Scl_2:	ld	temp, Z+   						;** LCD
	rcall	SendLCDdata						;** LCD
	dec	temp2 							;** LCD
	brne	Scl_2 							;** LCD
; move line 3 from buffer to LCD display				;** LCD
	ldi	temp, LCD3rdROW						;** LCD
	rcall	SendLCDcommand 						;** LCD
	ldi	temp2, 20						;** LCD
Scl_3:	ld	temp, Z+						;** LCD
	rcall	SendLCDdata						;** LCD
	dec	temp2							;** LCD
	brne	Scl_3							;** LCD
	ldi	ZH, high (ClearLine*2)					;** LCD
	ldi	ZL, low  (ClearLine*2)					;** LCD
	ldi	temp, LCD4thROW		; clear line 4 of LCD display	;** LCD
	rcall	LCDmsgFlash						;** LCD
	ret								;** LCD
									;** LCD
;***********************************************************************;** LCD
; End of Subroutines for the LCD display				;** LCD
;***********************************************************************;** LCD


 ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ;
 ;	Interrupt Routines
 ;
 ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 ;*************************************************************************************************
 ;   RxC_IRQ:  reads latest arriving byte from USART
 ; Puts byte into Qtail pointer location and then increments Qtail pointer.
 ;
 ; send: QinHigh, QinLow
 ; return: QinHigh, QinLow
 ; used: XH,XL,YH,YL (nondestructive)
 ;*************************************************************************************************
 RxC_IRQ:
 	in	SREGtemp, SREG
 	lds	irqtemp, UDR0		; retransmit all incoming MIDI bytes
 	sts	UDR0, irqtemp

 	sbis	GPIOR0, InputActive
 	rjmp	RxC_exit

 	push	XH
 	push	XL
 	push	YH
 	push	YL
 	cpi	irqtemp, 0xf8		; don't display MIDI clock byte
 	breq	RxC_1
 	cpi	irqtemp, 0xfe		; don't display MIDI Active Sense byte
 	breq	RxC_1

 	ldi	XH, high (InputBuffer)  ; place new MIDI byte into buffer for LCD display
 	ldi	XL, low  (InputBuffer)
 	lds	YH, QinHigh
 	lds	YL, QinLow
 	add	XL, YL
 	adc	XH, YH

	st	X, irqtemp
	adiw	YH:YL, 1
	cpi	YH, high (BUFFER_SIZE)
	brne	RxC_0
	cpi	YL, low (BUFFER_SIZE)
	brne	RxC_0
	clr	YH			; reset tail pointer if at end of buffer
	clr	YL

RxC_0:	sts	QinHigh, YH
	sts	QinLow, YL
RxC_1:	pop	YL
	pop	YH
	pop	XL
	pop	XH
RxC_exit:
	out	SREG, SREGtemp
	reti


;***************************************************************************************************************
; Timer 1  Interrupt Code - every 0.015 second period
;
;***************************************************************************************************************
T1_Overflow:
	in	SREGtemp, SREG
	ldi	irqtemp, ~ ( high (T1_reload) )
	sts	TCNT1H, irqtemp		; To do a 16-bit write, the high byte must be written before the low byte
	ldi	irqtemp, ~ ( low  (T1_reload) )
	sts	TCNT1L, irqtemp
	sbi	GPIOR0, Timer1Flag	; set with every timeout

	lds	irqtemp, LongPeriodCnt
	dec	irqtemp
	brne	T1_a
	ldi	irqtemp, LONG_MAXCNT
	sbi	GPIOR0, LongPeriodFlag	; set every 1/4 second
T1_a:	sts	LongPeriodCnt, irqtemp
	out	SREG, SREGtemp
	reti




;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
;	Flash Memory Data
;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


;************************************************
;                0123456789abcdef0123
LineOne: .DB	" * LCDMIDI48 v1.00 *",0,0
LineTwo: .DB	"Alan Probandt  2010 ",0,0
LineThree: .DB	"                    ",0,0
LineFour:  .DB	"                    ",0,0
ClearLine: .DB  "                    ",0,0
A_btnTxt:  .DB  "Start",0
B_btnTxt:  .DB  "Halt ",0
C_btnTxt:  .DB  "Reset",0
InDActMsg: .db  " MIDIin DeActivated ",0,0
InRActMsg: .db  " MIDIin ReActivated ",0,0
BufRstMsg: .db  "Buffer Ptr Set to 00",0,0
Start00:   .db  "00:                 ",0,0
;		 0123456789abcdef0123


;ID "**** end of code::LCDMIDIin48.asm v1.00 "
.db "****e dno  foced::CLMDDIiI4n.8sa m1v0. 0"


;; Toggle routine
;	ldi	irqtemp, (1<< T1toggle)
;	mov	ToggleReg, irqtemp
;	in	irqtemp, TogglePort
;	eor	irqtemp, ToggleReg	; port B6 pin 9
;	out	TogglePort, irqtemp


;
;
;TestBuffer: ; simulation of a Yamaha TX81Z MIDI sys-ex dump.  Used for testing LCD MIDI monitor display. 6 chars per line.
;;       0     1     2     3     4     5
;.db  0xF0, 0x43, 0x10, 0x24, 0x07, 0x01
;.db  0xF7, 0xF0, 0x43, 0x00, 0x04, 0x20 ; 0x00 - 0x0b 12
;.db  0x00, 0x10, 0x0C, 0x01, 0x03, 0x0B
;.db  0x00, 0x01, 0x50, 0x07, 0x06, 0x1F ; 0x0c - 0x17 24
;.db  0x07, 0x14, 0x03, 0x04, 0x00, 0x01
;.db  0x5C, 0x05, 0x00, 0x11, 0x0A, 0x19 ; 0x18 - 0x23 36
;.db  0x07, 0x03, 0x00, 0x02, 0x59, 0x03
;.db  0x00, 0x19, 0x01, 0x0D, 0x07, 0x0E ; 0x24 - 0x2f 48
;.db  0x00, 0x02, 0x63, 0x03, 0x06, 0x12
;.db  0x2B, 0x00, 0x63, 0x63, 0x0E, 0x05 ; 0x30 - 0x3b 60
;.db  0x07, 0x04, 0x00, 0x63, 0xff, 0xff
;