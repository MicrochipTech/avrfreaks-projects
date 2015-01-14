; BD_clock.asm    big digits for Nokia 5110    Column format, LSB top
;  __ 10 digits plus colon & space   _adapted chars are 12 columns by 3 bytes (24 pixels) high
;  _36 bytes per digit character     396 bytes total
;   12/25/2014 9:39PM  Merry Christmas!
;
; Use Lucida Console font to see listings correctly.
;
;command line to assemble this file:
;   C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2 -fI  BD_clock.asm  -l BD_clock.lst -o BD_clock.hex
; 
; To set the real-time clock IC, adjust the values in the RTCdata array (above the Get_RTC routine),    
;  uncomment the 'rcall SetRTC' line after initLoop0, assemble, and load.  Then re-assemble and load    
;  with the 'rcall SetRTC' line re-commented.  Worst kludge that I've ever done, but it works.          
;
;==================================================================================================
;
;	   Atmel AVR Mega8  [8K flash / 1K SRAM [address range:0x100 to 0x4ff]/ 512 EEPROM]
;                          _______
;      reset/  Nokia_reset |1  28| PC5 SCL  to real time clock board
;                      PD0 |2  27| PC4 SDA  to real time clock board 
;                      PD1 |3  26| ADC3
;                      PD2 |4  25| ADC2
;                      PD3 |5  24| ADC1
;                      PD4 |6  23| ADC0
;                 +5V  VCC |7  22| GND
;                      GND |8  21| AREF  +5V                ______
;                      PB6 |9  20| AVCC  +5V           MISO |1  2| Vcc (nc)
;                      PB7 |10 19| PB5  SCK	       SCLK |3  4| MOSI
;                      PD5 |11 18| PB4  MOSI	       RST  |5  6| gnd
;                      PD6 |12 17| PB3  MISO	            ------
;                      PD7 |13 16| PB2  Nokia_SDIN           ISP
;          Nokia_DC    PB0 |14 15| PB1  Nokia_SCLK
;                          -------
;==================================================================================================
;
; Differences between Mega8 and Mega328: IRQ vectors 16-bit, no GPIOR in Mega8, Peripheral reg locations.


.cseg
.org 0x0013
;&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
;   14 x 6 =                           1111 	;&
;   84 total scr chars       01234567890123 	;&
Line0_OpenMsg:		.db "BD_clock.asm  "	;&
			.db "Dec 26, 2014  "	;&
			.db "build: d  v1.0"	;&
			.db "              "	;&
			.db "Alan Probandt "	;&
			.db "Portland,OR   ",0,0;&
;&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



.nolist
.include "M8def.inc"
; Mega8 uses IN/OUT instructions for all peripheral_control_register reading/writing. LDS/STS for SRAM
.list
;
;======================================
; TIMER equates  (for AVR Mega8)
;--------------------------------------
.equ	clock			=  8000000	; AVR internal system clock [in Hertz]
.equ	OPENING_SCREEN_TIME 	=  3	; seconds
.equ	Contrast_Value		=  53

.equ	HB_PRESCALE		=  256	; prescaler for 32 mS increment  using timer 1
.equ	HB_LOGIC		=  (1<<CS12) | (0<<CS11) | (0<<CS10)
.equ	HB_PER_SECOND		=  32	; Heartbeat periods per second. Each period is 31.25 milliSecond
.equ	LongPeriod 		=  (clock / HB_PRESCALE) / HB_PER_SECOND   ; HBS=32 >> 977 counts upward


;##############################################################################################
;
;=======================================
; LCD display equates for Nokia 5110
;---------------------------------------
.equ	PIN_DC			=  0
.equ	PIN_SDIN		=  2
.equ	PIN_SCLK		=  1
.equ	PORT_DC			= PortB
.equ	PORT_SDIN		= PortB
.equ	PORT_SCLK		= PortB
.equ	DDR_DC			= DDRB
.equ	DDR_SDIN		= DDRB
.equ	DDR_SCLK		= DDRB
.equ	LCD_D			=  1		; Configuration for the LCD
.equ	LCD_CMD			=  0
.equ	LCD_X			= 84		; Size of the LCD
.equ	LCD_Y			= 48
.equ	NOKIA_STRING_SIZE	= 14
.equ	PIXELSperCHAR		=  6		; five dots and one space
.equ	NUM_ROWS_PER_CHAR	=  3
.equ	BIGCHARWIDTH            = 12
.equ	BIGSPACEOFFSET		= 11	; offset of beginning of space character data in BigDigits table
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.equ	LED_DDR		= DDRD
.equ	LED_PORT	= PORTD
.equ	LED_DEBUG	= 5
.equ	SLA_RTC		= 0xd0  ; 8-bit slave address 1101 for RealTimeClock IC: device address 000
.equ	RTC_STARTADDR	= 0	; only used by SetRTC
.equ	RTCreadSIZE	= 8	; 0x08 for DS1307 RTC IC: 0x0e when using DS3231 RTC IC
.equ	RTCwriteSIZE	= 8	; 0x08 for DS1307 RTC IC: 0x0e when using DS3231 RTC IC
 .equ	TWISTOPDELAY	= 45 	; 135 cycles at 0.125uSec/cycle   16.8uSec


;=====================================
;
;  	REGISTER Definitions
;
;=====================================
;
;; low register use   r0 - r15
.def	lowTemp0	= r2
.def	lowTemp1	= r3
.def	rBin1L		= r4
.def	rBin1H		= r5
.def	rBin2L		= r6
.def	rBin2H		= r7
.def	ColumnCounter	= r8
.def	wTemp0		= r10	; found in Nokia 5110 LCD routines
.def	wTemp1		= r11	; found in Nokia 5110 LCD routines
.def	counter		= r12	; found in PS2 keyboard routines
.def	SREGtemp 	= r14	; found in interrupt routines
.def	ZeroReg		= r15	; found in 16-bit add routines

;  upper registers   r16 - r31
.def	temp		= r16
.def	temp2		= r17
.def	temp3		= r18
.def	irqtemp		= r19


; upper register pairs:  r31:r30--ZH:ZL  r29:r28--YH:YL  r27:r26--XH:XL
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

;================================================================================================
;
;	SRAM Definitions
;
;================================================================================================
.dseg ; 1024 bytes SRAM in mega8  START = 0x0060  RAMEND= 0x045f
HeartBeatCntr:		.byte	1  ; 0x060
SecondCount:		.byte	1  ; 0x061
HighSecCount:		.byte	1  ; 0x062
PrevSecCount:		.byte	1  ; 0x063
PrevHiSecCnt:		.byte	1  ; 0x064
VarByteOne:		.byte	1  ; 0x065
VarByteTwo:		.byte	1  ; 0x066
VarByteThree:		.byte	1  ; 0x067
PtrOneLow:		.byte	1  ; 0x068
PtrOneHigh:		.byte	1  ; 0x069
DataByte:		.byte	1  ; 0x06a
Yposition:		.byte	1  ; 0x06b
Xposition:		.byte	1  ; 0x06c
TaskFlags:		.byte	1  ; 0x06d
  .equ	T1_HeartBeat	= 7
  .equ	TWIer_flag	= 6
  .equ	BlinkLED	= 5
  .equ	SetGetFlag 	= 4 ; clr is set routine, set is get routine

ByteASChigh:		.byte	1  ; 0x06e
ByteASCLow:		.byte	1  ; 0x06f
ExpectedTWSR:		.byte	1  ; 0x070
ReceivedTWSR:		.byte	1  ; 0x071
RTCBuffer:		.byte   18 ; 0x072
ASCIIdigits:		.byte	5  ; 0x084
ZeroLoc:		.byte	1  ; 0x089
resultL:		.byte	1  ; 0x08a
resultH:		.byte	1  ; 0x08b
TWIstep:		.byte	1  ; 0x08c
HighAddress:		.byte	1  ; 0x08c
LowAddress:		.byte	1  ; 0x08c
ReadBlockSize:		.byte	1  ; 0x08c

Page_Buffer:		.byte	128  ; 0x08c




;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


.cseg
.org 0x0000
;************************************************************************************************
; interrupt vectors for Mega8
	rjmp	reset		; rjmp reset is firmware-system-crash-handling code.
; ***** INTERRUPT VECTORS ***********************************************************
	rjmp	reset		; = 0x001  External Interrupt Request 0
	rjmp	reset		; = 0x002  External Interrupt Request 1
	rjmp	reset		; = 0x003  Timer/Counter2 Compare Match
	rjmp	reset		; = 0x004  Timer/Counter2 Overflow
	rjmp	reset		; = 0x005  Timer/Counter1 Capture Event
	rjmp	reset		; = 0x006  Timer/Counter1 Compare Match A
	rjmp	reset		; = 0x007  Timer/Counter1 Compare Match B
	rjmp	TIM1_OVF	; = 0x008  Timer/Counter1 Overflow
	rjmp	reset		; = 0x009  Timer/Counter0 Overflow
	rjmp	reset		; = 0x00a  Serial Transfer Complete
	rjmp	reset		; = 0x00b  USART, Rx Complete
	rjmp	reset		; = 0x00c  USART Data Register Empty
	rjmp	reset		; = 0x00d  USART, Tx Complete
	rjmp	reset		; = 0x00e  ADC Conversion Complete
	rjmp	reset		; = 0x00f  EEPROM Ready
	rjmp	reset		; = 0x010  Analog Comparator
	rjmp	reset		; = 0x011  2-wire Serial Interface
	rjmp	reset		; = 0x012  Store Program Memory Ready

;=======================================================================================================
.org 0x040
reset:
	ldi	temp, high (RAMEND)	; On Mega8, need to initialize stack pointer to end of SRAM.
	out	SPH, temp		; Not needed on other Mega AVRs, but good idea to do
	ldi	temp, low  (RAMEND)	; in case your embedded-system crash handler code jumps
	out	SPL, temp		; to reset (flash addr 000) instead of doing a hardware reset.

;----------------------------------------------------------------------------------------------
; initialize Timer 1  16bit  32mSecond increments; overflows 32 times a second  HB_PER_SECOND
	ldi	temp, (1 << TOV1)
	out	TIFR, temp              ; clear interrupt flag.  use TIFR1:0x16 for Mega328
	ldi	temp, (1 << TOIE1)
	out	TIMSK, temp		; enable interrupt.   use TIMSK1:0x6f for Mega328
	ldi	temp, high (~LongPeriod ) ; write high byte of 16-bit value first
	out	TCNT1H, temp		; TCNT1H [Mega328= 0x85]
	ldi	temp, low  (~LongPeriod )
	out	TCNT1L, temp 		; TCNT1L [Mega328= 0x84]
	ldi	temp, HB_LOGIC		; ( (1<<CS12) | (0<<CS11) | (0<<CS10) )
	out	TCCR1B, temp		; [Mega328:TCCR1B= 0x81] increment timer every 256 system clocks
	clr	temp
	sts	HeartBeatCntr, temp

;----------------------------------------------------------------------------------------------
; initialize TwoWire Interface
	ldi 	temp, 35		; cpu clock / (16 + (2*twbr) * prescale
	out	twbr, temp		; 8000000/86 = 93 KHz TWI/I2C clock rate
	ldi 	temp, 0x00		; prescale bits: 0 0   prescaler value = 1
	out 	twsr, temp		; TW status Reg.
	clr 	temp
	out 	twar, temp		; TW address Reg.  only used when Mega8 is a I2C slave
	ldi 	temp, (1 << TWEN)	; 0b00000100
	out	twcr, temp 		; TW control Reg

;	(0<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC)|(1<<TWEN)|(0<<TWIE)
;	   bit 7	6	5	    4	       3	2	   0
; initial value all 0 at reset.
;
; control byte for 24LC512, 24LC256, 24LC128, 24LC64, -- 1010 control code, IC addr 3 bits (always 1 0 0), R/w bit
;	ldi	temp,  0b10101001
;	sts	ControlByte, temp

;  writing a logic 1 to TWINT is only way to clear this interrupt flag.
;  It gets set when TWI has finished its current job.
;  clearing TWINT starts the next operation of the TWI, so TWAR, TWSR,
;  and TWDR must be ready before clearing TWINT.
;
;  if TWEA is set, then an ACK will be written by the TWI
;   when a byte is received in Master Receiver mode.
;----------------------------------------------------------------------------------------------


; initialize global interrupt enable
	sei	; switch on the AVR general interrupt enabler
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; initialize application
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; initialize (usually clear) variables
	clr	ZeroReg
	sts	TaskFlags,    ZeroReg
	sts	ZeroLoc,      ZeroReg
	sts	secondCount,  ZeroReg
	sts	PrevSecCount, ZeroReg
	sts	HighSecCount, ZeroReg
	sts	PrevHiSecCnt, ZeroReg
	sts	XPosition,    ZeroReg
	sts	YPosition,    ZeroReg

 	rcall	LCD_Initialize		; write seven commands to Nokia 5110

	clr	temp			; set display cursor to upper left of screen
	sts	VarByteOne, temp
	sts	VarByteTwo, temp
	rcall	LCD_gotoXY

 ; display opening screen
	ldi	temp, high (Line0_OpenMsg << 1 )
	sts	PtrOneHigh, temp
	ldi	temp, low  (Line0_OpenMsg << 1)
	sts	PtrOneLow, temp
	rcall	LCD_stringP

initLoop0:
	lds	temp, secondCount
	cpi	temp, OPENING_SCREEN_TIME
	brne	initLoop0

 	rcall	LCD_clear

;       rcall	SetRTC
; to set the real-time clock IC, adjust the values in the RTCdata array (above the Get_RTC routine),            
;  uncomment the 'rcall SetRTC' line after initLoop0, assemble, and load.  Then re-assemble and load            
;  with the 'rcall SetRTC' line re-commented.  Worst kludge that I've ever done, but it works.


;##############################################################################################
;						;##############################################
;  Main Code loop				;##############################################
;						;##############################################
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;##############################################
main_loop:					;####					   ####
	lds	temp, TaskFlags			;####	 Heartbeat time?  32 a second	   ####
	sbrs	temp, T1_HeartBeat		;####	 flicker LED; get pot values.	   ####
	rjmp	ml_noNew_HeartBeat 		;####					   ####
	rcall	HeartBeat_Period		;##############################################
ml_noNew_HeartBeat:				;##############################################
						;##############################################
						;##############################################
	rjmp	main_loop			;##############################################
;						;##############################################
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




;***************************************************************;**********
;  HeartBeat   Branch to here HB_PER_SECOND 			; HeartBeat
;    Branches here from main when mainloop detects		; HeartBeat
;     TaskFlags:T1_Heartbeat is set.				; HeartBeat
; 								; HeartBeat
;  Checks for debug-indicator (quick flash of LED) is active.	; HeartBeat
;  if yes, then turn on LED until next time HBcntr mod 32=0.	; HeartBeat
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; HeartBeat
HeartBeat_Period:						; HeartBeat
	push	temp						; HeartBeat
	push	ZH						; HeartBeat
	push	ZL						; HeartBeat
;								; HeartBeat
	lds	temp, TaskFlags ; turn off T1_HB flag		; HeartBeat
	cbr	temp, ( 1 << T1_HeartBeat )			; HeartBeat
	sts	TaskFlags, temp					; HeartBeat
;								; HeartBeat
; Check for active LED-flicker debugging-indicator =============; HeartBeat
HBP_isLEDblinkon: 						; HeartBeat
	lds	temp, TaskFlags					; HeartBeat
	sbrs	temp, BlinkLED					; HeartBeat
	rjmp	HBP_blinkExit					; HeartBeat
;								; HeartBeat
;	turn on LED for less than 1 sec	[only for debugging]	; HeartBeat
	cbi	LED_PORT, LED_DEBUG 				; HeartBeat
	lds	temp, HeartBeatCntr  ; update 32/sec		; HeartBeat
	andi	temp, 0b00011111				; HeartBeat
	breq	HBP_LEDoff ; when HBcnt = 0			; HeartBeat
;								; HeartBeat
	rjmp	HBP_blinkExit					; HeartBeat
;;								; HeartBeat
HBP_LEDoff:	; turn off LED 					; HeartBeat
	sbi	LED_PORT, LED_DEBUG 				; HeartBeat
	lds	temp, TaskFlags					; HeartBeat
	cbr 	temp, ( 1 << BlinkLED)				; HeartBeat
	sts	TaskFlags, temp					; HeartBeat
;								; HeartBeat
HBP_blinkExit:  ;===============================================; HeartBeat
								; HeartBeat
								; HeartBeat
; Check for new secondCount value and display big digits	; HeartBeat
	lds	temp,  SecondCount	; incremented by 	; HeartBeat
	lds	temp2, PrevSecCount	; the T1_overflow IRQ	; HeartBeat
	cp	temp, temp2					; HeartBeat
	breq	HBP_exit					; HeartBeat
	sts	PrevSecCount, temp				; HeartBeat
	rcall	GetRTC 		 				; HeartBeat
	rcall	ShowRealTime 					; HeartBeat
								; HeartBeat
HBP_exit:							; HeartBeat
	pop	ZL						; HeartBeat
	pop	ZH						; HeartBeat
	pop	temp						; HeartBeat
	ret							; HeartBeat
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%%%%%%%%%%



;***************************************************************************************;
; Get RTC (no error checking)								;
; send:	-	SLA_RTC, TWISTOPDELAY							;
; return:	RTC_Buffer (18 bytes in SRAM) loaded with data from i2C device.		;
; 		Errors are written to the last line of the Nokia 5110 display.		;
; uses::	temp, temp2, temp3, TaskFlags:SetGetFlag, TWIstep			;
; calls: 	RTC_Wait, -error handling code calls display subroutines-		;
; called by:	HeartBeat								;
; size:		118 flash bytes								;
;											;
; Error triggers on START [display->'100800GBdStart'] when run after SetRTC routine.	;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;
GetRTC:											;** Get_RTC_ne
	ldi 	temp, (1 << TWEN)	; 0b00000100					;** Get_RTC_ne
	out	TWCR, temp 		;0x36   TW control Reg  turn on TWI unit	;** Get_RTC_ne
;											;** Get_RTC_ne
	ldi	temp, 0									;** Get_RTC_ne
	sts	TWIstep, temp								;** Get_RTC_ne
	lds	temp, TaskFlags								;** Get_RTC_ne
	ori	temp, (1 << SetGetFlag)							;** Get_RTC_ne
	sts	TaskFlags, temp								;** Get_RTC_ne
											;** Get_RTC_ne
	ldi	temp, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)	;Transmit start cond		;** Get_RTC_ne
	out	TWCR, temp	;  _________						;** Get_RTC_ne
	rcall	GRTC_Wait	; SCL......|_____     Start       master asserts SDA	;** Get_RTC_ne
;				;  ______	    Condition   while SCL is released	;** Get_RTC_ne
;				; SDA   |________					;** Get_RTC_ne
;---------------------------------------------------------------------------------------;** Get_RTC_ne
; Send control byte [8-bit device code and R/_w bit] always after start condition.	;** Get_RTC_ne
; RTC has device code 0xd0 and the read bit (bit0) is set.  : 0b11010001 [0xd1]		;** Get_RTC_ne
	ldi	temp, SLA_RTC	; slave address 1101 for RTC: device address 000	;** Get_RTC_ne
	out	TWDR, temp	;0x03 							;** Get_RTC_ne
	ldi	temp, (1<<TWINT) | (1<<TWEN) ; clearing TWINT (write a 1) starts TWI	;** Get_RTC_ne
	out	TWCR, temp	;0x36  							;** Get_RTC_ne
	rcall	GRTC_Wait								;** Get_RTC_ne
											;** Get_RTC_ne
;---------------------------------------------------------------------------------------;** Get_RTC_ne
;											;** Get_RTC_ne
; Write Address.  TWDR should only be written when TWINT is high (TWI finished)		;** Get_RTC_ne
  	ldi	temp, 0 	; first RTC Address, seconds BCD value			;** Get_RTC_ne
  	out	TWDR, temp	;0x03							;** Get_RTC_ne
  	ldi	temp, (1<<TWINT) | (1<<TWEN)						;** Get_RTC_ne
  	out	TWCR, temp	;0x36							;** Get_RTC_ne
  	rcall	GRTC_Wait								;** Get_RTC_ne
  											;** Get_RTC_ne
;----------------------------------------------						;** Get_RTC_ne
;  This is a REPEATED START and it tells the slave that the				;** Get_RTC_ne
;  write operation is aborted while keeping the address that was sent.			;** Get_RTC_ne
	ldi	temp, (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)				;** Get_RTC_ne
	out	TWCR, temp	;0x36							;** Get_RTC_ne
	rcall	GRTC_Wait								;** Get_RTC_ne
											;** Get_RTC_ne
;----------------------------------------------						;** Get_RTC_ne
; Send control byte [device code, address, and R/W bit=read]				;** Get_RTC_ne
; 8-bit RTC SlaveAddress=0xd0 with read bit0 = 1					;** Get_RTC_ne
	ldi	temp, SLA_RTC  ; slave address 1101 for EEPROM: device address 000	;** Get_RTC_ne
	ori	temp, 0b00000001	; set bit0 for reading from I2C device		;** Get_RTC_ne
	out	TWDR, temp	;0x03							;** Get_RTC_ne
	ldi	temp, (1<<TWINT) | (1<<TWEN)						;** Get_RTC_ne
	out	TWCR, temp	;0x36							;** Get_RTC_ne
	rcall	GRTC_Wait								;** Get_RTC_ne
; 											;** Get_RTC_ne
;----------------------------------------------						;** Get_RTC_ne
;  read RTC_readSIZE # of bytes [ 18 bytes for DS3231, 8 bytes for DS1307] from RTC	;** Get_RTC_ne
	ldi	ZH, high (RTCBuffer)							;** Get_RTC_ne
	ldi	ZL, low  (RTCBuffer)							;** Get_RTC_ne
	ldi	temp2, RTCreadSIZE - 1	; read 2 with ACK, then 1 with NACK		;** Get_RTC_ne
											;** Get_RTC_ne
GRTC_1:	ldi	temp, (1<<TWINT) | (1<<TWEN) | (1<<TWEA) ;Read Data with Enable ACK	;** Get_RTC_ne
	out	TWCR, temp								;** Get_RTC_ne
	rcall	GRTC_Wait								;** Get_RTC_ne
	in	temp3, TWDR	; store the data byte read from the EEPROM		;** Get_RTC_ne
	st	z+, temp3								;** Get_RTC_ne
											;** Get_RTC_ne
	dec	temp2									;** Get_RTC_ne
	brne	GRTC_1		; loop to read each byte in the EEPROM page		;** Get_RTC_ne
											;** Get_RTC_ne
;  last byte read from EEPROM, so send NACK after receiving data bits			;** Get_RTC_ne
	ldi	temp, (1<<TWINT) | (1<<TWEN) ; | (0<<TWEA) ; Read Data with NACK	;** Get_RTC_ne
	out	TWCR, temp	;0x36							;** Get_RTC_ne
	rcall	GRTC_Wait								;** Get_RTC_ne
											;** Get_RTC_ne
	in	temp3, TWDR	; 0x03 store the data byte read from the EEPROM		;** Get_RTC_ne
	st	z+, temp3								;** Get_RTC_ne
											;** Get_RTC_ne
GRTC_STOP:										;** Get_RTC_ne
	ldi	temp, (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) ; do STOP condition		;** Get_RTC_ne
	out	TWCR, temp	;0x36							;** Get_RTC_ne
	ldi	temp, TWISTOPDELAY	; @8 uSec delay					;** Get_RTC_ne
GRTC_2:	dec	temp									;** Get_RTC_ne
	brne	GRTC_2			; wait TWI period after STOP before turning off	;** Get_RTC_ne
	ldi	temp, 0 ;(0<<TWEN)							;** Get_RTC_ne
	out	TWCR, temp	;0x36	turn off TWI unit				;** Get_RTC_ne
											;** Get_RTC_ne
GRTC_exit:										;** Get_RTC_ne
	ret	;  exit point for Read_I2C_String subroutine				;** Get_RTC_ne
											;** Get_RTC_ne
											;** Get_RTC_ne
;***************************************************************************************;** Get_RTC_ne
GRTC_Wait:										;** Get_RTC_ne
	in	temp, TWCR								;** Get_RTC_ne
	sbrs	temp, TWINT	; TWINT set means TWI is ready for new command		;** Get_RTC_ne
	rjmp	GRTC_Wait								;** Get_RTC_ne
	ret 			; TWINT is high now.  TWSR has a new valid value	;** Get_RTC_ne
;											;** Get_RTC_ne
;***** end of GetEEString (no error checking) routine  *********************************;** Get_RTC_ne
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%%%%%%%%%



;RTCdata:	; for DS3231
;	.db  0,59,50, 1,12, 8		; 012345   secs    min  hours   day    date   month
;	.db 14, 0, 0, 0, 0, 0		; 6789ab   year  a1sec  a1min   a1hr   a1dy    a1mn
;	.db  0, 0, 0, 0, 0, 0		; cdef01  a2sec  a2min   cntr  status  tmptrM tmptrL
;
RTCdata: ;       |   0      |   1 ; for DS1307   in binary coded decimal
	.db 0b00000000,0b00000111 ;  secs  min   24-hour coded
;                |   2 	    |   3
	.db 0b00001001,0b00000101 ;  hours day  [ bit6 set=12hr mode  bit5 set=PM/clr=AM ]
	 ;       |   4      |   5
	.db 0b00100110,0b00001100 ;  date  month (NOT BCD)
	 ;       |   6     7
	.db 0b00010100 ,0x10	  ;  year  control


;;**************************************************************************************;
;; Set RTC	only used a few times to set time:data on RTC IC 			;
;;											;
;; send:	RTC_data								;
;; return:	-									;
;; uses regs:	temp, temp2, temp3							;
;; calls: 	SRTC_wait, -error handling code calls NOkia 5110 display subroutines-	;
;; called by:	initialApplication							;
;; size:	194 flash bytes    ( with RTCdata)
;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;
SetRTC:											;** Set_RTC
	ldi	temp, 0									;** Set_RTC
	sts	TWIstep, temp								;** Set_RTC
	lds	temp, TaskFlags								;** Set_RTC
	andi	temp, ~(1 << SetGetFlag)	; clear setgetflag for errors		;** Set_RTC
	sts	TaskFlags, temp								;** Set_RTC
;;											;** Set_RTC
;	ldi	temp, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)	;Transmit start cond		;** Set_RTC
;	out	TWCR, temp	;  ____________						;** Set_RTC
;	rcall	SRTC_Wait	; SCL..........     Start       master pulls SDA low	;** Set_RTC
;;				;  _______	    Condition   while SCL is hi (rel)	;** Set_RTC
;;				; SDA    |_____						;** Set_RTC
;	in	temp3, TWSR								;** Set_RTC
;	andi	temp3, 0xF8								;** Set_RTC
;	sts	ReceivedTWSR, temp3							;** Set_RTC
;	ldi	temp,  0x08	; START condition created OK				;** Set_RTC
;	sts	ExpectedTWSR, temp							;** Set_RTC
;	cpse	temp3, temp	; Read SReg. 0x08 received (START sent)			;** Set_RTC
;	rcall	SRTC_error	; TWIstep=0						;** Set_RTC
											;** Set_RTC
;-------------------------------------------- 						;** Set_RTC
; Send control byte [device code, address, and R/_w bit] always after start condition.	;** Set_RTC
; DS3231 has device code 0xd0 with Write bit (bit0) = low				;** Set_RTC
	ldi	temp, SLA_RTC	 ; ControlByte	bit0 clear for write			;** Set_RTC
	out	TWDR, temp	 ; in order to send memory address to serial EEPROM.	;** Set_RTC
	ldi	temp, (1<<TWINT) | (1<<TWEN) ; clearing TWINT (write a 1) starts TWI	;** Set_RTC
	out	TWCR, temp								;** Set_RTC
	rcall	SRTC_Wait								;** Set_RTC
											;** Set_RTC
;	ldi	temp, 1									;** Set_RTC
;	sts	TWIstep, temp								;** Set_RTC
;	in	temp3, TWSR								;** Set_RTC
;	andi	temp3, 0xF8								;** Set_RTC
;	sts	ReceivedTWSR, temp3							;** Set_RTC
;	ldi	temp,  0x18	; 0x18 >> SLA+W send, ACK received			;** Set_RTC
;	sts	ExpectedTWSR, temp							;** Set_RTC
;	cpse	temp3, temp								;** Set_RTC
;	rcall	SRTC_error	; ; TWIstep=1						;** Set_RTC
											;** Set_RTC
;--------------------------------------------						;** Set_RTC
; Write RTC Address.  									;** Set_RTC
	ldi	temp, RTC_STARTADDR							;** Set_RTC
	out	TWDR, temp								;** Set_RTC
	ldi	temp, (1<<TWINT) | (1<<TWEN)						;** Set_RTC
	out	TWCR, temp								;** Set_RTC
	rcall	SRTC_Wait								;** Set_RTC
											;** Set_RTC
;	ldi	temp, 2									;** Set_RTC
;	sts	TWIstep, temp								;** Set_RTC
;	in	temp3, TWSR								;** Set_RTC
;	andi	temp3, 0xF8								;** Set_RTC
;	sts	ReceivedTWSR, temp3							;** Set_RTC
;	ldi	temp,  0x28	; 0x28 >> data sent, ACK received OK			;** Set_RTC
;	sts	ExpectedTWSR, temp							;** Set_RTC
;	cpse	temp3, temp								;** Set_RTC
;	rcall	SRTC_error	; TWIstep=2						;** Set_RTC
	 										;** Set_RTC
;----------------------------------------------------------------			;** Set_RTC
;  write 8 bytes to DS1307 from RTCdata flash array					;** Set_RTC
	ldi	ZH, high (RTCdata << 1)							;** Set_RTC
	ldi	ZL, low  (RTCdata << 1)							;** Set_RTC
	ldi	temp2, RTCwriteSIZE 	; write 15 bytes. Receive ACK after each.	;** Set_RTC
											;** Set_RTC
SRTC_1:											;** Set_RTC
	lpm	temp, z+								;** Set_RTC
	out	TWDR, temp	 							;** Set_RTC
	ldi	temp, (1<<TWINT) | (1<<TWEN) | (1<<TWEA) ; write Data with Enable ACK	;** Set_RTC
	out	TWCR, temp								;** Set_RTC
	rcall	SRTC_Wait								;** Set_RTC
											;** Set_RTC
;	lds	temp, TWIstep								;** Set_RTC
;	inc	temp									;** Set_RTC
;	sts	TWIstep, temp								;** Set_RTC
;											;** Set_RTC
;	in	temp3, TWSR								;** Set_RTC
;	andi	temp3, 0xF8								;** Set_RTC
;	sts	ReceivedTWSR, temp3							;** Set_RTC
;	ldi	temp,  0x28	; 0x28 Master Write w/ Acknowledge OK			;** Set_RTC
;	sts	ExpectedTWSR, temp							;** Set_RTC
;	cpse	temp3, temp								;** Set_RTC
;	rcall	SRTC_error	;  TWIstep=3..21					;** Set_RTC
											;** Set_RTC
	dec	temp2									;** Set_RTC
	brne	SRTC_1		; loop to read each byte in the EEPROM page		;** Set_RTC
											;** Set_RTC
SRTC_STOP:										;** Set_RTC
	ldi	temp, (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) ; do STOP condition		;** Set_RTC
	sts	TWCR, temp								;** Set_RTC
	ldi	temp, TWISTOPDELAY	;  actually @16 uSec delay			;** Set_RTC
SRTC_2:	dec	temp									;** Set_RTC
	brne	SRTC_2		; wait TWI period after STOP before turning off 	;** Set_RTC
	ldi	temp, 0 	; TWI unit (0 << TWEN).					;** Set_RTC
	sts	TWCR, temp								;** Set_RTC
											;** Set_RTC
SRTC_exit:										;** Set_RTC
	ret	;  exit point for SetRTC routine					;** Set_RTC
											;** Set_RTC
;SRTC_error:										;** Set_RTC
;	rcall	doTWIerror								;** Set_RTC
;	ret										;** Set_RTC
											;** Set_RTC
											;** Set_RTC
;***************************************************************************************;** Set_RTC
SRTC_Wait:										;** Set_RTC
	in	temp, TWCR								;** Set_RTC
	sbrs	temp, TWINT	; TWINT set means TWI is ready for new command		;** Set_RTC
	rjmp	SRTC_Wait								;** Set_RTC
	ret 			; TWINT is high now.  TWSR has a new valid value.	;** Set_RTC
;											;** Set_RTC
;***************************************************************************************;** Set_RTC


;***************************************************************; Nokia_5110
;  Nokia 5110 graphics display routines:			; Nokia_5110
;	Source code @600 bytes					; Nokia_5110
;	plus font tables: ASCII chars 480 bytes			; Nokia_5110
;	total:    @1100 bytes 					; Nokia_5110
;  code unused by this application gets commented out		; Nokia_5110
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Nokia_5110
;								; Nokia_5110
;  In order to convert code from Arduino_C			; Nokia_5110
;  into AVR assembler, I am using variables			; Nokia_5110
;  in SRAM to store the function arguments			; Nokia_5110
;  from the Ard_C code.  These are pre-named			; Nokia_5110
;  as Var_ByteOne and Var_ByteTwo.  When main			; Nokia_5110
;  asm code calls a subroutine that models			; Nokia_5110
;  the actions of the Adr_C function, it first			; Nokia_5110
;  loads the values of the arguments into the			; Nokia_5110
;  SRAM variables set aside for this purpose.			; Nokia_5110
;  Var_ByteOne:	     size= 1 byte each				; Nokia_5110
;  Var_ByteTwo:							; Nokia_5110
;  Ptr_OneLow:							; Nokia_5110
;  Ptr_OneHigh:							; Nokia_5110
;  								; Nokia_5110
;  LCD__Initialize: 	; no arguments				; Nokia_5110
;  LCD__stringP:	; Ptr_OneLow:Ptr_OneHigh = adr of strg	; Nokia_5110
;  LCD__character:	; Var_ByteOne == char	   in flash	; Nokia_5110
;  LCD__clear:		; no arguments				; Nokia_5110
;  LCD__write:		; Var_ByteOne== 0=cmd;1=dt: Var_ByteTwo	; Nokia_5110
;  LCD__gotoXY:		; Var_ByteOne= x : Var_ByteTwo = y	; Nokia_5110
;  LCD__stringSRAM	; Ptr_OneLow:Ptr_OneHigh = adr str SRAM	; Nokia_5110
;								; Nokia_5110
; Hardware:							; Nokia_5110
;	DDR__DC; DDR__SDIN; DDR__SCLK  				; Nokia_5110
;	PORT__DC; PORT__SDIN; PORT__SCLK 			; Nokia_5110
;	PIN__DC; PIN__SDIN; PIN__SCLK  				; Nokia_5110
;_______________________________________________________________; Nokia_5110
;	; Example Code						; Nokia_5110
;								; Nokia_5110
;  	; clear bottom line of Nokia display, 			; Nokia_5110
;	ldi	temp, 0		; position cursor column 0-83	; Nokia_5110
;	sts	Var_ByteOne, temp				; Nokia_5110
;	ldi	temp, 5		; position Nokia cursor 0-5 	; Nokia_5110
;	sts	Var_ByteTwo, temp ; display error msg 		; Nokia_5110
;	rcall	LCD_gotoXY	;on Nokia 5110 bottom row	; Nokia_5110
;	ldi	temp, high (ClearLine << 1)			; Nokia_5110
;	sts	Ptr_OneHigh, temp ; erase bottom line		; Nokia_5110
;	ldi	temp, low  (ClearLine << 1)			; Nokia_5110
;	sts	Ptr_OneLow, temp				; Nokia_5110
;	rcall	LCD__stringP					; Nokia_5110
;								; Nokia_5110
;	ClearLine:	.db "              ", 0, 0		; Nokia_5110
;								; Nokia_5110
;***************************************************************; Nokia_5110
LCD_Initialize:							; Nokia_5110
	sbi	DDR_DC,   PIN_DC  ; Nokia5110 control signals	; Nokia_5110
	sbi	DDR_SDIN, PIN_SDIN	; can be on diff ports	; Nokia_5110
	sbi	DDR_SCLK, PIN_SCLK 	; DDRA  0x10 		; Nokia_5110
;								; Nokia_5110
;  LCD_write(LCD__CMD, 0x21);  	; LCD Extended Commands.	; Nokia_5110
	ldi	temp, LCD_CMD					; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	ldi	temp, 0x21					; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
;								; Nokia_5110
;.equ	CONTRAST_VALUE	=  40					; Nokia_5110
	ldi	temp, LCD_CMD	; Set LCD Vop (Contrast).   	; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	ldi	temp, 0x80 |  CONTRAST_VALUE			; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
;								; Nokia_5110
	ldi	temp, LCD_CMD	; Set Temp coefficent. ;;0x04	; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	ldi	temp, 0x04					; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
;								; Nokia_5110
	ldi	temp, LCD_CMD	; LCD bias mode 1:48. ;;0x14	; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	ldi	temp, 0x14					; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
;								; Nokia_5110
	ldi	temp, LCD_CMD	; LCD in normal mode.		; Nokia_5110
	sts	VarByteOne, temp ; 0x0d for inverse		; Nokia_5110
	ldi	temp, 0x0c					; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
;								; Nokia_5110
	ldi	temp, LCD_CMD	 ; LCD standard commands	; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	ldi	temp, 0x20					; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
;								; Nokia_5110
	ldi	temp, LCD_CMD					; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	ldi	temp, 0x0c					; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
;								; Nokia_5110
	ret							; Nokia_5110
;								; Nokia_5110
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Nokia_5110
 								; Nokia_5110
;***************************************************************; Nokia_5110
;void Lcd__StringP(char *characters)  // string in prog flash	; Nokia_5110
;{								; Nokia_5110
;  while (*characters)						; Nokia_5110
;  {								; Nokia_5110
;    LcdCharacter(*characters++);				; Nokia_5110
;  }								; Nokia_5110
;}								; Nokia_5110
;---------------------------------------------------------------; Nokia_5110
.equ	EEfont	= 0x000						; Nokia_5110
LCD_stringP:							; Nokia_5110
	push	zh						; Nokia_5110
	push	zl						; Nokia_5110
	push	temp						; Nokia_5110
;								; Nokia_5110
	lds	zh, PtrOneHigh					; Nokia_5110
	lds	zl, PtrOneLow 					; Nokia_5110
LstrP_0:							; Nokia_5110
	lpm	temp, z+	;  dereferenced pointer 	; Nokia_5110
	tst	temp		;  value is in flash		; Nokia_5110
	breq	LstrP_exit					; Nokia_5110
;								; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	rcall	LCD_character					; Nokia_5110
;								; Nokia_5110
	rjmp	LstrP_0						; Nokia_5110
;								; Nokia_5110
LstrP_exit:							; Nokia_5110
	pop	temp						; Nokia_5110
	pop	zl						; Nokia_5110
	pop	zh						; Nokia_5110
	ret							; Nokia_5110
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Nokia_5110
	 							; Nokia_5110
;***************************************************************; Nokia_5110
;void LcdCharacter			 			; Nokia_5110
; character in Var_ByteOne [range 0x20 to 0x7f]			; Nokia_5110
; subtract 0x20 and multiply by 5 to get table offset  		; Nokia_5110
;  offset range [from ASCII_font begin] 0x00 to 0x27b {635}	; Nokia_5110
;								; Nokia_5110
;---------------------------------------------------------------; Nokia_5110
LCD_character:	; character in VarByteOne			; Nokia_5110
	push	zh						; Nokia_5110
	push	zl						; Nokia_5110
	push	temp2						; Nokia_5110
	push	temp3						; Nokia_5110
	push	temp						; Nokia_5110
;								; Nokia_5110
	lds	temp2, VarByteOne				; Nokia_5110
	subi	temp2, 0x20					; Nokia_5110
								; Nokia_5110
;; When using in-line (not EEPROM) font, use lines from LCw	; Nokia_5110
;;   to LCy and comment lines from LCy to LCz.			; Nokia_5110
;; When font is stored in serial EEPROM, use lines from LCy	; Nokia_5110
;;   to LCz and comment lines from LCy to LCz. Also you can	; Nokia_5110
;;   comment out the ASCII_font data table.  But you must	; Nokia_5110
;;   add the ReadPage routine.					; Nokia_5110
;;								; Nokia_5110
;; LCw:	_this line is always commented				; Nokia_5110
;; multiply temp2 by 5 [number of bytes per bit-mapped char] 	; Nokia_5110
	mov	temp3, temp2	; shift-and-add for MUL		; Nokia_5110
	clr	temp						; Nokia_5110
	lsl	temp2 						; Nokia_5110
	rol	temp						; Nokia_5110
	lsl	temp2						; Nokia_5110
	rol	temp						; Nokia_5110
	add	temp2, temp3	;  low of offset into table	; Nokia_5110
	adc	temp, ZeroReg	; high of offset into table	; Nokia_5110
	ldi	ZH, high (ASCII_font << 1)			; Nokia_5110
	ldi	ZL, low  (ASCII_font << 1) 			; Nokia_5110
	add	ZL, temp2 ; ZH:ZL points to first of five bytes ; Nokia_5110
	adc	ZH, temp					; Nokia_5110
	ldi	temp3, 5					; Nokia_5110
Lchar_loop:							; Nokia_5110
	lpm	temp, z+					; Nokia_5110
;;								; Nokia_5110
;; LCy:	_this line is always commented				; Nokia_5110
;;								; Nokia_5110
;	clr	temp3 						; Nokia_5110
;	lsl temp2    rol temp3    ; times 2 16-bit		; Nokia_5110
;	lsl temp2    rol temp3    ; times 4 16-bit		; Nokia_5110
;	lsl temp2    rol temp3    ; times 8 16-bit		; Nokia_5110
;	ldi	ZH, high ( EEFont )				; Nokia_5110
;	ldi	ZL, low  ( EEFont ) 				; Nokia_5110
;	add	ZL, temp2 ; ZH:ZL points to first of 8 bytes 	; Nokia_5110
;	adc	ZH, temp3					; Nokia_5110
;	sts	HighAddress, ZH					; Nokia_5110
;	sts	LowAddress,  ZL					; Nokia_5110
;	ldi	temp, 8						; Nokia_5110
;	sts	ReadBlockSize, temp				; Nokia_5110
;	rcall	ReadPage  ; send High:lowAddress, ReadBlockSize	; Nokia_5110
;	ldi	temp3, 5					; Nokia_5110
;	ldi	ZH, high (Page_Buffer)				; Nokia_5110
;	ldi	ZL, low  (Page_Buffer)				; Nokia_5110
;Lchar_loop:							; Nokia_5110
;	ld	temp, z+					; Nokia_5110
;; LCz: _this line is always commented				; Nokia_5110
								; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	ldi	temp, LCD_D					; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
								; Nokia_5110
	dec	temp3						; Nokia_5110
	brne	Lchar_loop					; Nokia_5110
;								; Nokia_5110
	ldi	temp, LCD_D					; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	ldi	temp, 0x00	; blank column			; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
;								; Nokia_5110
	pop	temp						; Nokia_5110
	pop	temp3						; Nokia_5110
	pop	temp2						; Nokia_5110
	pop	zl						; Nokia_5110
	pop	zh						; Nokia_5110
	ret							; Nokia_5110
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Nokia_5110
								; Nokia_5110
;***************************************************************; Nokia_5110
;void LcdClear(void)						; Nokia_5110
;{								; Nokia_5110
;  for (int index = 0; index < LCD_X * LCD_Y ; index++)		; Nokia_5110
;  {								; Nokia_5110
      ;    LCD_write(LCD_D, 0x00);				; Nokia_5110
;  }								; Nokia_5110
;}								; Nokia_5110
;---------------------------------------------------------------; Nokia_5110
LCD_clear:							; Nokia_5110
	push	zh						; Nokia_5110
	push	zl						; Nokia_5110
	push	temp						; Nokia_5110
;								; Nokia_5110
	clr	temp		; position cursor to top 	; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_gotoXY					; Nokia_5110
;								; Nokia_5110
	ldi	zh, high  ( LCD_X * ( LCD_Y / 8) )		; Nokia_5110
	ldi	zl, low   ( LCD_X * ( LCD_Y / 8) )		; Nokia_5110
;								; Nokia_5110
L_clr0:								; Nokia_5110
	ldi	temp, LCD_D					; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	ldi	temp, 0x00					; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
;								; Nokia_5110
	dec	zl						; Nokia_5110
	brne	L_clr0						; Nokia_5110
	dec	zh						; Nokia_5110
	tst	zh						; Nokia_5110
	brpl	L_clr0						; Nokia_5110
;								; Nokia_5110
	pop	temp						; Nokia_5110
	pop	zl						; Nokia_5110
	pop	zh						; Nokia_5110
;								; Nokia_5110
	ret							; Nokia_5110
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Nokia_5110
								; Nokia_5110
								; Nokia_5110
;***************************************************************; Nokia_5110
LCD_write:; Var_ByteOne: dc {0=cmd;1=d}  :: Var_ByteTwo: data	; Nokia_5110
	push	temp3		; LOWEST LEVEL FUNCTION		; Nokia_5110
	push	temp2						; Nokia_5110
	push	temp						; Nokia_5110
;								; Nokia_5110
	lds	temp3, VarByteOne	; cmd=0; data=1		; Nokia_5110
	tst	temp3						; Nokia_5110
	breq	Lw1						; Nokia_5110
;								; Nokia_5110
	sbi	PORT_DC, PIN_DC					; Nokia_5110
	rjmp	Lw2						; Nokia_5110
;		; Nokia gLCD command  selected low on DC	; Nokia_5110
Lw1:	cbi	PORT_DC, PIN_DC					; Nokia_5110
;								; Nokia_5110
Lw2:	ldi	temp, 8						; Nokia_5110
	lds	temp2, VarByteTwo				; Nokia_5110
;								; Nokia_5110
Lw_loop:							; Nokia_5110
	lsl	temp2			; MSB first 		; Nokia_5110
	brcc	Lw4						; Nokia_5110
;	carry bit set						; Nokia_5110
	sbi	PORT_SDIN, PIN_SDIN				; Nokia_5110
	sbi	PORT_SCLK, PIN_SCLK				; Nokia_5110
	cbi	PORT_SCLK, PIN_SCLK				; Nokia_5110
	rjmp	Lw5						; Nokia_5110
Lw4:	; carry bit clear					; Nokia_5110
	cbi	PORT_SDIN, PIN_SDIN				; Nokia_5110
	sbi	PORT_SCLK, PIN_SCLK				; Nokia_5110
	cbi	PORT_SCLK, PIN_SCLK				; Nokia_5110
;								; Nokia_5110
Lw5:	dec	temp						; Nokia_5110
	breq	Lw_exit						; Nokia_5110
	rjmp	Lw_loop						; Nokia_5110
;--------------------------					; Nokia_5110
;								; Nokia_5110
Lw_exit:							; Nokia_5110
	pop	temp						; Nokia_5110
	pop	temp2						; Nokia_5110
	pop	temp3						; Nokia_5110
	ret							; Nokia_5110
;								; Nokia_5110
;								; Nokia_5110
;;**************************************************************; Nokia_5110
; * gotoXY routine to position cursor				; Nokia_5110
; * byte offset is column X {Var_ByteOne}, row(y) {Var_ByteTwo}	; Nokia_5110
; * x - range: 0 to 83		Display address=(y*84)+x	; Nokia_5110
; * y - range: 0 to 5  						; Nokia_5110
; *;								; Nokia_5110
;void gotoXY(int x, int y)					; Nokia_5110
;{								; Nokia_5110
;  LCD_write( 0, 0x80 | x);  ;; Column.				; Nokia_5110
;  LCD_write( 0, 0x40 | y);  ;; Row.				; Nokia_5110
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Nokia_5110
LCD_gotoXY:		; Var_ByteOne: x, Var_ByteTwo y)	; Nokia_5110
	push	temp						; Nokia_5110
	push	lowTemp0					; Nokia_5110
	push	lowTemp1					; Nokia_5110
								; Nokia_5110
	lds	lowTemp0, VarByteOne				; Nokia_5110
	lds	lowTemp1, VarByteTwo				; Nokia_5110
								; Nokia_5110
	ldi	temp,  0					; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	ldi	temp, 0x80					; Nokia_5110
	or	temp, lowTemp0	; x position offset 0-83	; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
								; Nokia_5110
	ldi	temp,  0					; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	ldi	temp, 0x40					; Nokia_5110
	or	temp, lowTemp1	; y position offset 0-5		; Nokia_5110
	sts	VarByteTwo, temp				; Nokia_5110
	rcall	LCD_write					; Nokia_5110
								; Nokia_5110
	pop	lowTemp1					; Nokia_5110
	pop	lowTemp0					; Nokia_5110
	pop	temp						; Nokia_5110
	ret							; Nokia_5110
								; Nokia_5110
;;**************************************************************; Nokia_5110
;;LCDStringSRAM     Display string being stored in SRAM.  	; Nokia_5110
;;		String size fixed at 14 characters.		; Nokia_5110
;;	send: 	Ptr_OneHigh:Ptr_OneLow				; Nokia_5110
;;	uses:	Var_ByteOne					; Nokia_5110
;;    registers: temp, temp2, ZH:ZL				; Nokia_5110
;;	constants:  NOKIA_STRING_SIZE 				; Nokia_5110
;;	calls:	LCD_character					; Nokia_5110
;;  								; Nokia_5110
;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Nokia_5110
;LCD_stringSRAM: 						; Nokia_5110
;	push	ZH						; Nokia_5110
;	push	ZL						; Nokia_5110
;	push	temp2						; Nokia_5110
;	push	temp						; Nokia_5110
;								; Nokia_5110
;	lds	ZH, PtrOneHigh					; Nokia_5110
;	lds	ZL, PtrOneLow					; Nokia_5110
;	ldi	temp2, NOKIA_STRING_SIZE			; Nokia_5110
;								; Nokia_5110
;LstrS_0:; main difference from LcdStringP is the use of	; Nokia_5110
;	ld	temp, z+   ; instead lpm instruction. DeRef-ed	; Nokia_5110
;	sts	VarByteOne, temp; pointer value is in SRAM. 	; Nokia_5110
;	rcall	LCD_character					; Nokia_5110
;								; Nokia_5110
;	dec	temp2	 					; Nokia_5110
;	breq	LstrS_exit					; Nokia_5110
;	rjmp	LstrS_0						; Nokia_5110
;								; Nokia_5110
;LstrS_exit:							; Nokia_5110
;	pop	temp						; Nokia_5110
;	pop	temp2						; Nokia_5110
;	pop	ZL						; Nokia_5110
;	pop	ZH						; Nokia_5110
;	ret							; Nokia_5110
;								; Nokia_5110
;;**************************************************************; Nokia_5110
;; LCDdisplayDecimal	replaces LEDdisplayParam [16x2 LCD]	; Nokia_5110
;;   Display decimal number starting at LCD position		; Nokia_5110
;;								; Nokia_5110
;;   Call LCD_gotoXY before this routine			; Nokia_5110
;; send:  Temp with 0-255 value					; Nokia_5110
;;	 Temp3 with number of digits to display  		; Nokia_5110
;; calls: Bin2ToAsc5, LCD_character		  		; Nokia_5110
;;								; Nokia_5110
;;**************************************************************; Nokia_5110
;LCD_displayDecimal:						; Nokia_5110
;	push	temp 						; Nokia_5110
;	push	temp2						; Nokia_5110
;	push	temp3						; Nokia_5110
;	push	rBin1L						; Nokia_5110
;	push	rBin1H						; Nokia_5110
;	push	ZH						; Nokia_5110
;	push	ZL						; Nokia_5110
;								; Nokia_5110
;	mov	rBin1L, temp					; Nokia_5110
;	clr	rBin1H						; Nokia_5110
;	ldi	ZH, high (ASCIIdigits)				; Nokia_5110
;	ldi	ZL, low  (ASCIIdigits)				; Nokia_5110
;; convert 16-bit binary to five digits				; Nokia_5110
;	rcall	Bin2ToAsc5     					; Nokia_5110
;; returns result in ASCIIdigits (five digits followed by 0x00) ; Nokia_5110
;;	mov	temp, temp2					; Nokia_5110
;	ldi	temp2, 5					; Nokia_5110
;	sub	temp2, temp3					; Nokia_5110
;	tst	temp2		 				; Nokia_5110
;	breq	Ld1						; Nokia_5110
;Ld0:	inc	ZL						; Nokia_5110
;	dec	temp2						; Nokia_5110
;	brne	Ld0						; Nokia_5110
;Ld1:								; Nokia_5110
;LD2: 	ld	temp, z+					; Nokia_5110
;	tst	temp						; Nokia_5110
; 	breq	Ld_exit						; Nokia_5110
;	sts	VarByteOne, temp 				; Nokia_5110
; 	rcall	LCD_character	 				; Nokia_5110
; 	rjmp	Ld2						; Nokia_5110
;								; Nokia_5110
;Ld_exit:	 						; Nokia_5110
;	pop	ZL						; Nokia_5110
;	pop	ZH						; Nokia_5110
;	pop	rBin1H						; Nokia_5110
;	pop	rBin1L						; Nokia_5110
;	pop	temp3						; Nokia_5110
;	pop	temp2						; Nokia_5110
;	pop	temp						; Nokia_5110
;	ret							; Nokia_5110
;                                                               ; Nokia_5110
;***************************************************************; Nokia_5110
; LCD__clearline	 					; Nokia_5110
;   Display decimal number starting at LCD position		; Nokia_5110
;								; Nokia_5110
; send:  VarByteOne with line value (0-5)			; Nokia_5110
;	 							; Nokia_5110
;***************************************************************; Nokia_5110
LCD_clearline:							; Nokia_5110
	push	temp						; Nokia_5110
	push	temp2						; Nokia_5110
	push	ZH						; Nokia_5110
	push	ZL						; Nokia_5110
								; Nokia_5110
	lds	temp2, VarByteOne				; Nokia_5110
	ldi	ZH, high (ClearLine << 1)			; Nokia_5110
	ldi	ZL, low  (ClearLine << 1)			; Nokia_5110
								; Nokia_5110
	ldi	temp, 0						; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	sts	VarByteTwo, temp2				; Nokia_5110
	rcall	LCD_gotoXY					; Nokia_5110
Lclln_0:							; Nokia_5110
	lpm	temp, z+					; Nokia_5110
	tst	temp						; Nokia_5110
	breq	Lclln_1						; Nokia_5110
	sts	VarByteOne, temp				; Nokia_5110
	rcall	LCD_character					; Nokia_5110
	rjmp	Lclln_0						; Nokia_5110
Lclln_1:							; Nokia_5110
								; Nokia_5110
	pop	ZL						; Nokia_5110
	pop	ZH						; Nokia_5110
	pop	temp2						; Nokia_5110
	pop	temp						; Nokia_5110
	ret							; Nokia_5110
								; Nokia_5110
ClearLine:  .db "                ",0,0				; Nokia_5110
								; Nokia_5110
								; Nokia_5110
;###############################################################; Nokia_5110
;								; Nokia_5110
;  	Nokia 5110 Graphic Character Fonts			; Nokia_5110
;								; Nokia_5110
;###############################################################; Nokia_5110
;								; Nokia_5110
ASCII_font:  ; 96 chars, 480 bytes of char data   5 columns by 7 rows per char   ; Nokia_5110
.db  0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x5F, 0x00, 0x00 ; space  21 !  ; Nokia_5110
.db  0x00, 0x07, 0x00, 0x07, 0x00,   0x14, 0x7F, 0x14, 0x7F, 0x14 ; 22 "   23 #  ; Nokia_5110
.db  0x24, 0x2A, 0x7F, 0x2A, 0x12,   0x23, 0x13, 0x08, 0x64, 0x62 ; 24 $   25 %  ; Nokia_5110
.db  0x36, 0x49, 0x56, 0x20, 0x50,   0x00, 0x08, 0x07, 0x03, 0x00 ; 26 &   27 '  ; Nokia_5110
.db  0x00, 0x1C, 0x22, 0x41, 0x00,   0x00, 0x41, 0x22, 0x1C, 0x00 ; 28 (   29 )  ; Nokia_5110
.db  0x2A, 0x1C, 0x7F, 0x1C, 0x2A,   0x08, 0x08, 0x3E, 0x08, 0x08 ; 2a *   2b +  ; Nokia_5110
.db  0x00, 0x80, 0x70, 0x30, 0x00,   0x08, 0x08, 0x08, 0x08, 0x08 ; 2c ,   2d -  ; Nokia_5110
.db  0x00, 0x00, 0x60, 0x60, 0x00,   0x20, 0x10, 0x08, 0x04, 0x02 ; 2e .   2f ;  ; Nokia_5110
.db  0x3E, 0x51, 0x49, 0x45, 0x3E,   0x00, 0x42, 0x7F, 0x40, 0x00 ; 30 0   31 1  ; Nokia_5110
.db  0x72, 0x49, 0x49, 0x49, 0x46,   0x21, 0x41, 0x49, 0x4D, 0x33 ; 32 2   33 3  ; Nokia_5110
.db  0x18, 0x14, 0x12, 0x7F, 0x10,   0x27, 0x45, 0x45, 0x45, 0x39 ; 34 4   35 5  ; Nokia_5110
.db  0x3C, 0x4A, 0x49, 0x49, 0x31,   0x41, 0x21, 0x11, 0x09, 0x07 ; 36 6   37 7  ; Nokia_5110
.db  0x36, 0x49, 0x49, 0x49, 0x36,   0x46, 0x49, 0x49, 0x29, 0x1E ; 38 8   39 9  ; Nokia_5110
.db  0x00, 0x00, 0x14, 0x00, 0x00,   0x00, 0x40, 0x34, 0x00, 0x00 ; 3a :   3b ;  ; Nokia_5110
.db  0x00, 0x08, 0x14, 0x22, 0x41,   0x14, 0x14, 0x14, 0x14, 0x14 ; 3c <   3d =  ; Nokia_5110
.db  0x00, 0x41, 0x22, 0x14, 0x08,   0x02, 0x01, 0x59, 0x09, 0x06 ; 3e >   3f ?  ; Nokia_5110
.db  0x3E, 0x41, 0x5D, 0x59, 0x4E,   0x7C, 0x12, 0x11, 0x12, 0x7C ; 40 @   41 A  ; Nokia_5110
.db  0x7F, 0x49, 0x49, 0x49, 0x36,   0x3E, 0x41, 0x41, 0x41, 0x22 ; 42 B   43 C  ; Nokia_5110
.db  0x7F, 0x41, 0x41, 0x41, 0x3E,   0x7F, 0x49, 0x49, 0x49, 0x41 ; 44 D   45 E  ; Nokia_5110
.db  0x7F, 0x09, 0x09, 0x09, 0x01,   0x3E, 0x41, 0x41, 0x51, 0x73 ; 46 F   47 G  ; Nokia_5110
.db  0x7F, 0x08, 0x08, 0x08, 0x7F,   0x00, 0x41, 0x7F, 0x41, 0x00 ; 48 H   49 I  ; Nokia_5110
.db  0x20, 0x40, 0x41, 0x3F, 0x01,   0x7F, 0x08, 0x14, 0x22, 0x41 ; 4a J   4b K  ; Nokia_5110
.db  0x7F, 0x40, 0x40, 0x40, 0x40,   0x7F, 0x02, 0x1C, 0x02, 0x7F ; 4c L   4d M  ; Nokia_5110
.db  0x7F, 0x04, 0x08, 0x10, 0x7F,   0x3E, 0x41, 0x41, 0x41, 0x3E ; 4e N   4f O  ; Nokia_5110
.db  0x7F, 0x09, 0x09, 0x09, 0x06,   0x3E, 0x41, 0x51, 0x21, 0x5E ; 50 P   51 Q  ; Nokia_5110
.db  0x7F, 0x09, 0x19, 0x29, 0x46,   0x26, 0x49, 0x49, 0x49, 0x32 ; 52 R   53 S  ; Nokia_5110
.db  0x03, 0x01, 0x7F, 0x01, 0x03,   0x3F, 0x40, 0x40, 0x40, 0x3F ; 54 T   55 U  ; Nokia_5110
.db  0x1F, 0x20, 0x40, 0x20, 0x1F,   0x3F, 0x40, 0x38, 0x40, 0x3F ; 56 V   57 W  ; Nokia_5110
.db  0x63, 0x14, 0x08, 0x14, 0x63,   0x03, 0x04, 0x78, 0x04, 0x03 ; 58 X   59 Y  ; Nokia_5110
.db  0x61, 0x59, 0x49, 0x4D, 0x43,   0x00, 0x7F, 0x41, 0x41, 0x41 ; 5a Z   5b [  ; Nokia_5110
.db  0x02, 0x04, 0x08, 0x10, 0x20,   0x00, 0x41, 0x41, 0x41, 0x7F ; 5c ¥   5d ]  ; Nokia_5110
.db  0x04, 0x02, 0x01, 0x02, 0x04,   0x40, 0x40, 0x40, 0x40, 0x40 ; 5e ^   5f _  ; Nokia_5110
.db  0x00, 0x03, 0x07, 0x08, 0x00,   0x20, 0x54, 0x54, 0x78, 0x40 ; 60 `   61 a  ; Nokia_5110
.db  0x7F, 0x28, 0x44, 0x44, 0x38,   0x38, 0x44, 0x44, 0x44, 0x28 ; 62 b   63 c  ; Nokia_5110
.db  0x38, 0x44, 0x44, 0x28, 0x7F,   0x38, 0x54, 0x54, 0x54, 0x18 ; 64 d   65 e  ; Nokia_5110
.db  0x00, 0x08, 0x7E, 0x09, 0x02,   0x18, 0xA4, 0xA4, 0x9C, 0x78 ; 66 f   67 g  ; Nokia_5110
.db  0x7F, 0x08, 0x04, 0x04, 0x78,   0x00, 0x44, 0x7D, 0x40, 0x00 ; 68 h   69 i  ; Nokia_5110
.db  0x20, 0x40, 0x40, 0x3D, 0x00,   0x7F, 0x10, 0x28, 0x44, 0x00 ; 6a j   6b k  ; Nokia_5110
.db  0x00, 0x41, 0x7F, 0x40, 0x00,   0x7C, 0x04, 0x78, 0x04, 0x78 ; 6c l   6d m	 ; Nokia_5110
.db  0x7C, 0x08, 0x04, 0x04, 0x78,   0x38, 0x44, 0x44, 0x44, 0x38 ; 6e n   6f o	 ; Nokia_5110
.db  0xFC, 0x18, 0x24, 0x24, 0x18,   0x18, 0x24, 0x24, 0x18, 0xFC ; 70 p   71 q	 ; Nokia_5110
.db  0x7C, 0x08, 0x04, 0x04, 0x08,   0x48, 0x54, 0x54, 0x54, 0x24 ; 72 r   73 s	 ; Nokia_5110
.db  0x04, 0x04, 0x3F, 0x44, 0x24,   0x3C, 0x40, 0x40, 0x20, 0x7C ; 74 t   75 u	 ; Nokia_5110
.db  0x1C, 0x20, 0x40, 0x20, 0x1C,   0x3C, 0x40, 0x30, 0x40, 0x3C ; 76 v   77 w	 ; Nokia_5110
.db  0x44, 0x28, 0x10, 0x28, 0x44,   0x4C, 0x90, 0x90, 0x90, 0x7C ; 78 x   79 y	 ; Nokia_5110
.db  0x44, 0x64, 0x54, 0x4C, 0x44,   0x00, 0x08, 0x36, 0x41, 0x00 ; 7a z   7b {	 ; Nokia_5110
.db  0x00, 0x00, 0x77, 0x00, 0x00,   0x00, 0x41, 0x36, 0x08, 0x00 ; 7c |   7d }	 ; Nokia_5110
.db  0x02, 0x01, 0x02, 0x04, 0x02,   0x3C, 0x26, 0x23, 0x26, 0x3C ; 7e ?   7f ?	 ; Nokia_5110
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Nokia_5110
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%; Nokia_5110


;********************************************************************************************
;  Byte2Hex ASCII  Convert a byte to two hexidecimal ASCII characters.
;
; send:		DataByte
; return:	{SRAM} ByteASChigh, ByteASClow
; uses:		temp; ZH:ZL (non-destructive)
;********************************************************************************************
Byte2HexASCII:
	push	ZH
	push	ZL
	push	temp

	lds	temp, DataByte
	swap	temp
	andi	temp, 0x0f
	ldi	ZH, high (hexchars << 1)
	ldi	ZL, low  (hexchars << 1)
	add	ZL, temp
	adc	ZH, ZeroReg
	lpm	temp, z
	sts	ByteASChigh, temp

	lds	temp, DataByte
	andi	temp, 0x0f
	ldi	ZH, high (hexchars << 1)
	ldi	ZL, low  (hexchars << 1)
	add	ZL, temp
	adc	ZH, ZeroReg
	lpm	temp, z
	sts	ByteASClow, temp

	pop	temp
	pop	ZL
	pop	ZH
	ret

hexchars: .db "0123456789ABCDEF"
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;




;***************************************************************;<> Interrupt_Routines
;  Timer 1 overflow   set to activate every 31.25 milliseconds	;<> Interrupt_Routines
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;<> Interrupt_Routines
								;<> Interrupt_Routines
TIM1_OVF:							;<> Interrupt_Routines
	in	SREGtemp, SREG	; 0x3f				;<> Interrupt_Routines
								;<> Interrupt_Routines
	ldi	irqtemp, high (~LongPeriod)			;<> Interrupt_Routines
	out	TCNT1H, irqtemp  	; 0x6f			;<> Interrupt_Routines
	ldi	irqtemp, low  (~LongPeriod)			;<> Interrupt_Routines
	out	TCNT1L,  irqtemp  	; 0x6e			;<> Interrupt_Routines
								;<> Interrupt_Routines
;  set a flag for the main code to know that T1 IRQ happened	;<> Interrupt_Routines
	lds	irqtemp, TaskFlags				;<> Interrupt_Routines
	sbr	irqtemp, (1 << T1_HeartBeat)			;<> Interrupt_Routines
	sts	TaskFlags, irqtemp				;<> Interrupt_Routines
								;<> Interrupt_Routines
	lds	irqtemp, HeartBeatCntr				;<> Interrupt_Routines
	inc	irqtemp						;<> Interrupt_Routines
	sts	HeartBeatCntr, irqtemp				;<> Interrupt_Routines
								;<> Interrupt_Routines
; check if HBcntr mod 32 == 0. If yes, inc SecondCount		;<> Interrupt_Routines
 	andi	irqtemp,  HB_PER_SECOND - 1    ; ### DEBUG	;<> Interrupt_Routines
	brne	T1ovf_exit					;<> Interrupt_Routines
								;<> Interrupt_Routines
	lds	irqtemp, SecondCount  ; init to 0 by reset	;<> Interrupt_Routines
	inc	irqtemp						;<> Interrupt_Routines
	sts	SecondCount, irqtemp				;<> Interrupt_Routines
	tst	irqtemp						;<> Interrupt_Routines
	brne	T1ovf_exit					;<> Interrupt_Routines
	lds	irqtemp, HighSecCount ; init to 0 by reset	;<> Interrupt_Routines
	inc	irqtemp						;<> Interrupt_Routines
	sts	HighSecCount, irqtemp				;<> Interrupt_Routines
								;<> Interrupt_Routines
T1ovf_exit:							;<> Interrupt_Routines
	out	SREG, SREGtemp		;0x3f			;<> Interrupt_Routines
 	reti 							;<> Interrupt_Routines
								;<> Interrupt_Routines
;***************************************************************;<> Interrupt_Routines




;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%% Math
;    Math Routines							;%% Math
;									;%% Math
; Bin2ToAsc5			58 instructions	/ 116 bytes		;%% Math
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
	rcall	Bin2ToBcd5	; convert binary to BCD			;%% Math
	ldi	temp, 4		; Counter is 4 leading digits		;%% Math
	mov	rBin2L, temp						;%% Math
Bin2ToAsc5a:								;%% Math
	ld	temp, z		; read a BCD digit			;%% Math
	tst	temp		; check if leading zero			;%% Math
	brne	Bin2ToAsc5b	; No, found digit >0			;%% Math
	ldi	temp,' '	; overwrite with blank			;%% Math
	st	z+, temp	; store and set to next position	;%% Math
	dec	rBin2L		; decrement counter			;%% Math
	brne	Bin2ToAsc5a	; further leading blanks		;%% Math
	ld	temp, z		; Read the last BCD			;%% Math
Bin2ToAsc5b:								;%% Math
	inc	rBin2L		; one more char				;%% Math
Bin2ToAsc5c:								;%% Math
	subi	temp,-'0'	; Add ASCII-0				;%% Math
	st	z+, temp	; store and inc pointer			;%% Math
	ld	temp,z		; read next char			;%% Math
	dec	rBin2L		; more chars?				;%% Math
	brne	Bin2ToAsc5c	; yes, go on				;%% Math
	sbiw	ZL, 5		; Pointer to beginning of the BCD	;%% Math
	ret 			; done					;%% Math
									;%% Math
									;%% Math
; Bin2ToBcd5								;%% Math
; ==========								;%% Math
; converts a 16-bit-binary to a 5-digit-BCD				;%% Math
; In: 16-bit-binary in rBin1H:L, Z points to first digit		;%% Math
;   where the result goes to						;%% Math
; Out: 5-digit-BCD, Z points to first BCD-digit				;%% Math
; Used registers: rBin1H:L (unchanged), rBin2H:L (changed),		;%% Math
;   temp								;%% Math
; Called subroutines: Bin2ToDigit					;%% Math
;									;%% Math
Bin2ToBcd5:								;%% Math
	push	rBin1H		; Save number				;%% Math
	push	rBin1L							;%% Math
	ldi	temp, HIGH (10000) ; Start with ten thousands		;%% Math
	mov	rBin2H,temp						;%% Math
	ldi	temp, LOW (10000)					;%% Math
	mov	rBin2L,temp						;%% Math
	rcall	Bin2ToDigit	; Calculate digit			;%% Math
	ldi	temp, HIGH (1000) ; Next with thousands			;%% Math
	mov	rBin2H,temp						;%% Math
	ldi	temp, LOW (1000)					;%% Math
	mov	rBin2L, temp						;%% Math
	rcall	Bin2ToDigit	; Calculate digit			;%% Math
	ldi	temp, HIGH (100); Next with hundreds			;%% Math
	mov	rBin2H, temp						;%% Math
	ldi	temp, LOW (100)						;%% Math
	mov	rBin2L, temp						;%% Math
	rcall	Bin2ToDigit	; Calculate digit			;%% Math
	ldi	temp, HIGH (10)	; Next with tens			;%% Math
	mov	rBin2H, temp						;%% Math
	ldi	temp, LOW (10)						;%% Math
	mov	rBin2L, temp						;%% Math
	rcall	Bin2ToDigit	; Calculate digit			;%% Math
	st	z,rBin1L	; Remainder are ones			;%% Math
	sbiw	ZL, 4		; Put pointer to first BCD		;%% Math
	pop	rBin1L		; Restore original binary		;%% Math
	pop	rBin1H							;%% Math
	ret			; and return				;%% Math
									;%% Math
									;%% Math
; Bin2ToDigit								;%% Math
; ===========								;%% Math
; converts one decimal digit by continued subtraction of a		;%% Math
;   binary coded decimal						;%% Math
; Used by: Bin2ToBcd5, Bin2ToAsc5, Bin2ToAsc				;%% Math
; In:	16-bit-binary in rBin1H:L, binary coded decimal in		;%% Math
;	rBin2H:L, Z points to current BCD digit				;%% Math
; Out:	ZH:ZL points to location that has # of times rbin2 is in rBin1	;%% Math
;	rBin1H:L (holds remainder of the binary),			;%% Math
; Used registers:   rBin2H:L (unchanged), temp, ZH:ZL			;%% Math
; Called subroutines: -							;%% Math
;									;%% Math
Bin2ToDigit:								;%% Math
	clr	temp		; digit count is zero			;%% Math
Bin2ToDigitA:								;%% Math
	cp	rBin1H, rBin2H	; Number bigger than decimal?		;%% Math
	brcs	Bin2ToDigitC	; MSB smaller than decimal		;%% Math
	brne	Bin2ToDigitB	; MSB bigger than decimal		;%% Math
	cp	rBin1L, rBin2L	; LSB bigger or equal decimal		;%% Math
	brcs	Bin2ToDigitC	; LSB smaller than decimal		;%% Math
Bin2ToDigitB:								;%% Math
	sub	rBin1L, rBin2L	; Subtract LSB decimal			;%% Math
	sbc	rBin1H, rBin2H	; Subtract MSB decimal			;%% Math
	inc	temp		; Increment digit count			;%% Math
	rjmp	Bin2ToDigitA	; Next loop				;%% Math
Bin2ToDigitC:								;%% Math
	st	z+, temp	; Save digit and increment		;%% Math
	ret 			; done					;%% Math
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;Math


;******************************************************************************************************
; LongDelay	counts T1 periods { 80 per second: 12.5mS period } for long do-nothing loops.
;  used by Reset/initApplication for LCD message display timing.
; send:	temp    --- 8MHz =0.125uS period  if timer1 prescaler=1024, then each timer1 interval is 128uS.
; uses: temp2 (n_d) = number of 12.5mS periods to wait
;******************************************************************************************************
LongDelay:
	push	temp2

	lds	temp2, HeartBeatCntr
	add	temp2, temp
LDly_1:	lds	temp, HeartBeatCntr
	cpse	temp, temp2
	rjmp	LDly_1

	pop	temp2
	ret
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


BigDigits:	; 12 columns * 3 rows * 12 characters  432 bytes
;       0    1    2    3    4    5    6    7    8    9    a    b
.db	0xfc,0xfe,0xfe,0x0f,0x07,0x07,0x07,0x07,0xcf,0xfe,0xfc,0xf8	;character '0'
.db	0xff,0xff,0xff,0xf0,0x78,0x3c,0x1e,0x0f,0x07,0xff,0xff,0xff
.db	0x3f,0x7f,0x7f,0xf8,0xf0,0xf0,0xf0,0xf0,0xf8,0x7f,0x7f,0x1f

.db     0x00,0x00,0x40,0x60,0x70,0x78,0xfc,0xfe,0xff,0xff,0x00,0x00	;character '1'
.db     0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00
.db     0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00

.db     0x38,0x3c,0x3e,0x0f,0x07,0x07,0x07,0x0f,0x1f,0xff,0xfe,0xfc	;character '2'
.db     0xc0,0xe0,0xe0,0xf0,0x78,0x78,0x3c,0x1e,0x0f,0x0f,0x07,0x03
.db     0xff,0xff,0xff,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0

.db	0x38,0x3c,0x1e,0x0f,0x0f,0x0f,0x0f,0x0f,0x1e,0xfc,0xf8,0xf0  	;character '3'
.db	0x00,0x00,0x00,0x1e,0x1e,0x1e,0x1e,0x3e,0xff,0xff,0xe7,0x80
.db	0x78,0xf8,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf8,0xff,0x7f,0x1f

.db	0x00,0x00,0x00,0x00,0x00,0xc0,0xf0,0xf8,0x7e,0xff,0xff,0xff	;character '4'
.db	0xe0,0xf0,0xf8,0xbe,0x9f,0x8f,0x83,0x81,0x80,0xff,0xff,0xff
.db	0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xff

.db     0xff,0xff,0xff,0x87,0x87,0x87,0x87,0x87,0x87,0x07,0x07,0x07 	;character '5'
.db     0x07,0x07,0x03,0x03,0x03,0x03,0x03,0x03,0x07,0xff,0xff,0xfc
.db     0x3c,0x7c,0x78,0xf0,0xf0,0xf0,0xf0,0xf0,0x78,0x7f,0x3f,0x1f

.db	0xf8,0xfc,0xfe,0x0f,0x07,0x07,0x07,0x07,0x07,0x0f,0x1e,0x1c	;character '6'
.db	0xff,0xff,0xff,0x1e,0x0e,0x0e,0x0e,0x0e,0x1e,0xfe,0xfc,0xf8
.db	0x3f,0x7f,0x7f,0xf0,0xe0,0xe0,0xe0,0xe0,0xf8,0x7f,0x3f,0x1f

.db	0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0xff,0xff,0xff,0xff	;character '7'
.db	0x00,0x00,0x00,0x00,0x80,0xe0,0xf8,0xfe,0x7f,0x1f,0x0f,0x03
.db	0x00,0xc0,0xf0,0xfe,0xff,0x1f,0x07,0x01,0x00,0x00,0x00,0x00

.db	0xf8,0xfe,0xff,0x07,0x07,0x07,0x07,0x07,0x8f,0xff,0xfe,0x78	;character '8'
.db	0xe0,0xff,0xff,0x1f,0x0e,0x0e,0x0e,0x1f,0x3f,0xff,0xf9,0xe0
.db	0x1f,0x7f,0x7f,0xf0,0xe0,0xe0,0xe0,0xf0,0x78,0x7f,0x3f,0x1f

.db	0xfc,0xfe,0xff,0x0f,0x07,0x07,0x07,0x07,0x0f,0xff,0xfc,0xf8	;character '9'
.db	0x1f,0x3f,0x3f,0x7c,0x78,0x78,0x78,0x78,0x3c,0xff,0xff,0xff
.db	0x00,0x70,0xf0,0xf0,0xf0,0xf0,0x70,0x78,0x7c,0x3f,0x3f,0x0f

.db	0x00,0x00,0x00,0x60,0xf0,0xf8,0xf8,0xf0,0x60,0x00,0x00,0x00	;character ':'
.db	0x00,0x00,0x00,0x00,0x80,0xc1,0xc1,0x80,0x00,0x00,0x00,0x00
.db	0x00,0x00,0x00,0x03,0x07,0x0f,0x0f,0x07,0x03,0x00,0x00,0x00

.db     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00	;character ' '
.db     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.db     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


;***************************************************************;
; Show_RealTime   Display hours and minutes			;
;								;
;  send:  	RTCBuffer					;
;  return:	-- 						;
;  calls:	ShowBigDigit					;
;			 					;
;  constants:	BIG_CHARWIDTH					;
;								;
;  bit 7   6   5    4    3   2   1   0				;
;00:  CH   10s seconds   -1s seconds-- ClockHalt (clear=osc on)	;
;01:   0   10s minutes   -1s minutes--				;
;02:             10 hr     1s hrs				;
;03:                         ---day--- 				;
;04:   0   0   10s date    1s date				;
;05:   0   0   0   10s mns   1s mns				;
;06      10s year           1s year				;
;07: out  0    0 SQW=1   0  0  RS1  RS0				;
;08-3F    RAM	 RAM   	 RAM	 RAM				;
;---------------------------------------------------------------; Show_RealTime
ShowRealTime:  							; Show_RealTime
	push	ZH						; Show_RealTime
	push	ZL						; Show_RealTime
	push	YH						; Show_RealTime
	push	YL						; Show_RealTime
	push	temp3						; Show_RealTime
	push	temp2						; Show_RealTime
	push	temp						; Show_RealTime
								; Show_RealTime
;---------------------------------------------------------------; Show_RealTime
; 								; Show_RealTime
; write 10s of hours	       					; Show_RealTime
        ldi	ZH, high (RTCBuffer+2)				; Show_RealTime
        ldi	ZL, low  (RTCBuffer+2)				; Show_RealTime
	ld	temp, z 					; Show_RealTime
	swap	temp 						; Show_RealTime
	andi	temp, 0x03       				; Show_RealTime
	cpi	temp, 0       					; Show_RealTime
	brne	srt0a       					; Show_RealTime
	ldi	temp, 11       					; Show_RealTime
srt0a:	sts	DataByte, temp 					; Show_RealTime
	ldi	temp, (0 * BIGCHARWIDTH) + 0			; Show_RealTime
	sts	Xposition, temp       				; Show_RealTime
	ldi	temp, 0       					; Show_RealTime
	sts	Yposition, temp       				; Show_RealTime
	rcall	ShowBigDigit  ;DataByte [BD offset] Xpos: Ypos	; Show_RealTime
; 								; Show_RealTime
; write 1s of hours	       					; Show_RealTime
        ldi	ZH, high (RTCBuffer+2)				; Show_RealTime
        ldi	ZL, low  (RTCBuffer+2)				; Show_RealTime
	ld	temp, z 					; Show_RealTime
	andi	temp, 0x0f       				; Show_RealTime
	sts	DataByte, temp 					; Show_RealTime
	ldi	temp, (1 * BIGCHARWIDTH) + 2			; Show_RealTime
	sts	Xposition, temp       				; Show_RealTime
	ldi	temp, 0       					; Show_RealTime
	sts	Yposition, temp       				; Show_RealTime
	rcall	ShowBigDigit  ;DataByte [BD offset] Xpos: Ypos	; Show_RealTime
; 								; Show_RealTime
; write 10s of minutes	       					; Show_RealTime
        ldi	ZH, high (RTCBuffer+1)				; Show_RealTime
        ldi	ZL, low  (RTCBuffer+1)				; Show_RealTime
	ld	temp, z 					; Show_RealTime
	swap	temp 						; Show_RealTime
	andi	temp, 0x0f       				; Show_RealTime
	sts	DataByte, temp 					; Show_RealTime
	ldi	temp, (3 * BIGCHARWIDTH) + 6			; Show_RealTime
	sts	Xposition, temp       				; Show_RealTime
	ldi	temp, 0       					; Show_RealTime
	sts	Yposition, temp       				; Show_RealTime
	rcall	ShowBigDigit  ;DataByte [BD offset] Xpos: Ypos	; Show_RealTime
; 								; Show_RealTime
; write 1s of minutes	       					; Show_RealTime
        ldi	ZH, high (RTCBuffer+1)				; Show_RealTime
        ldi	ZL, low  (RTCBuffer+1)				; Show_RealTime
	ld	temp, z 					; Show_RealTime
	andi	temp, 0x0f       				; Show_RealTime
	sts	DataByte, temp 					; Show_RealTime
	ldi	temp, (4 * BIGCHARWIDTH) + 8			; Show_RealTime
	sts	Xposition, temp       				; Show_RealTime
	ldi	temp, 0       					; Show_RealTime
	sts	Yposition, temp       				; Show_RealTime
	rcall	ShowBigDigit  ;DataByte [BD offset] Xpos: Ypos	; Show_RealTime
; 								; Show_RealTime
; write colon		       					; Show_RealTime
	ldi	temp, 10					; Show_RealTime
	sts	DataByte, temp 					; Show_RealTime
	ldi	temp, (2 * BIGCHARWIDTH) + 4			; Show_RealTime
	sts	Xposition, temp       				; Show_RealTime
	ldi	temp, 0       					; Show_RealTime
	sts	Yposition, temp       				; Show_RealTime
	rcall	ShowBigDigit  ;DataByte [BD offset] Xpos: Ypos	; Show_RealTime
								; Show_RealTime
; write 10s of seconds in small digits				; Show_RealTime
	ldi	temp, 72  ; NOkia 5110 pixel column		; Show_RealTime
	sts	VarByteOne, temp       				; Show_RealTime
	ldi	temp, 2   ; NOkia 5110 row number		; Show_RealTime
	sts	VarByteTwo, temp       				; Show_RealTime
	rcall	LCD_gotoXY       				; Show_RealTime
				       				; Show_RealTime
        ldi	ZH, high (RTCBuffer+0)				; Show_RealTime
        ldi	ZL, low  (RTCBuffer+0)				; Show_RealTime
	ld	temp, z 					; Show_RealTime
	swap	temp 						; Show_RealTime
	andi	temp, 0x0f       				; Show_RealTime
	ori	temp, 0x30  ; make it an ASCII digit		; Show_RealTime
	sts	VarByteOne, temp 				; Show_RealTime
	rcall	LCD_character	       				; Show_RealTime
				       				; Show_RealTime
; write 1s of seconds in small digits  				; Show_RealTime
	ldi	temp, 78	 ; Nokia 5110 pixel column	; Show_RealTime
	sts	VarByteOne, temp       				; Show_RealTime
	ldi	temp, 2    	 ; Nokia 5110 row number	; Show_RealTime
	sts	VarByteTwo, temp       				; Show_RealTime
	rcall	LCD_gotoXY       				; Show_RealTime
				       				; Show_RealTime
        ldi	ZH, high (RTCBuffer+0)				; Show_RealTime
        ldi	ZL, low  (RTCBuffer+0)				; Show_RealTime
	ld	temp, z 					; Show_RealTime
	andi	temp, 0x0f       				; Show_RealTime
	ori	temp, 0x30  ; make it an ASCII digit		; Show_RealTime
	sts	VarByteOne, temp 				; Show_RealTime
	rcall	LCD_character	       				; Show_RealTime
; 								; Show_RealTime
; write date -  month text     					; Show_RealTime
	ldi	temp, 0	* PIXELSperCHAR				; Show_RealTime
	sts	VarByteOne, temp       				; Show_RealTime
	ldi	temp, 4 ; Nokia 5110 row number			; Show_RealTime
	sts	VarByteTwo, temp       				; Show_RealTime
	rcall	LCD_gotoXY       				; Show_RealTime
				       				; Show_RealTime
        ldi	ZH, high (RTCBuffer+5)				; Show_RealTime
        ldi	ZL, low  (RTCBuffer+5)				; Show_RealTime
	ld	temp, z	; not BCD, datasheet is wrong!		; Show_RealTime
	dec	temp	; month 0-11 instead of 1-12		; Show_RealTime
	rcall	DisplayArbLenStr				; Show_RealTime
								; Show_RealTime
; write date -  date number					; Show_RealTime
	ldi	ZH, high (RTCBuffer+4)				; Show_RealTime
        ldi	ZL, low  (RTCBuffer+4)				; Show_RealTime
        ld	temp, z 					; Show_RealTime
        swap	temp						; Show_RealTime
        andi	temp, 0x03					; Show_RealTime
 	ori	temp, 0x30 ; make it an ASCII digit		; Show_RealTime
        sts	VarByteOne, temp				; Show_RealTime
        rcall	LCD_character					; Show_RealTime
	ldi	ZH, high (RTCBuffer+4)				; Show_RealTime
	ldi	ZL, low  (RTCBuffer+4)				; Show_RealTime
	ld	temp, z 					; Show_RealTime
	andi	temp, 0x0f					; Show_RealTime
	ori	temp, 0x30 ; make it an ASCII digit		; Show_RealTime
	sts	VarByteOne, temp				; Show_RealTime
	rcall	LCD_character					; Show_RealTime
 								; Show_RealTime
srt_exit:							; Show_RealTime
	pop	temp 						; Show_RealTime
	pop	temp2						; Show_RealTime
	pop	temp3						; Show_RealTime
	pop	YL						; Show_RealTime
	pop	YH 						; Show_RealTime
	pop	ZL						; Show_RealTime
	pop	ZH						; Show_RealTime
								; Show_RealTime
	ret							; Show_RealTime
								; Show_RealTime
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Show_RealTime


;***************************************************************;
; Show_BigDigit    						;
;								;
; called by:  ShowRealTime					;
;								;
; send:  DataByte [offset into BigDigit table]			;
;	 Xposition: Yposition  [upper-left corner pixel]	;
;								;
; return:  -							;
;								;
; calls: LCD__gotoXY	uses: VarByteOne{x},VarByteTwo{y}	;
;	 LCD__write	uses: VarByteOne{LCD_D}			;
;			VarByteTwo{DataByte}			;
;								;
; constants:	BIGCHARWIDTH, NUM_ROWS_PER_CHAR			;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;
ShowBigDigit:  ;DataByte [BD offset] Xpos: Ypos			; Show_BigDigit
	push	temp						; Show_BigDigit
	push	temp2						; Show_BigDigit
	push	Lowtemp0					; Show_BigDigit
	push	Lowtemp1					; Show_BigDigit
	push	ZH						; Show_BigDigit
	push	ZL						; Show_BigDigit
								; Show_BigDigit
; multiply digit value by 36 to get offset into BD table.	; Show_BigDigit
	lds	temp, DataByte					; Show_BigDigit
 ;								; Show_BigDigit
	ldi	temp2, 36	; comment these four lines	; Show_BigDigit
	mul	temp, temp2	; for AVRs without MUL instr	; Show_BigDigit
	mov	temp,  r0	; like the Tiny1634.		; Show_BigDigit
	mov	temp2, r1	;				; Show_BigDigit
; 	rcall	Multiply36	; uncomment these three lines	; Show_BigDigit
;  	lds	temp,   resultL	; for AVRs without MUL instr	; Show_BigDigit
; 	lds	temp2,  resultH		 			; Show_BigDigit
								; Show_BigDigit
	lds	rBin1L, Xposition				; Show_BigDigit
	sts	VarByteOne, rBin1L				; Show_BigDigit
	lds	rBin1L, Yposition				; Show_BigDigit
	sts	VarByteTwo, rBin1L				; Show_BigDigit
	rcall	LCD_gotoXY 	; VarByteOne{x},VarByteTwo{y}	; Show_BigDigit
								; Show_BigDigit
	ldi	ZH, high (BigDigits << 1)			; Show_BigDigit
	ldi	ZL, low  (BigDigits << 1)			; Show_BigDigit
	add	ZL, temp					; Show_BigDigit
	adc	ZH, temp2					; Show_BigDigit
;---------------------------					; Show_BigDigit
; write row one of three for 12-bytes of digit's bitmap data	; Show_BigDigit
	ldi	temp2, BIGCHARWIDTH				; Show_BigDigit
	ldi	temp, LCD_D 					; Show_BigDigit
	sts	VarByteOne, temp				; Show_BigDigit
sbd1:	lpm	temp, z+					; Show_BigDigit
	sts	VarByteTwo, temp				; Show_BigDigit
	rcall	LCD_write					; Show_BigDigit
	dec	temp2						; Show_BigDigit
	brne	sbd1						; Show_BigDigit
 	lds	temp, Xposition					; Show_BigDigit
	sts	VarByteOne, temp				; Show_BigDigit
	lds	temp, Yposition 	       			; Show_BigDigit
	subi	temp, - 1	; add 1 by subtracting -1	; Show_BigDigit
	sts	VarByteTwo, temp				; Show_BigDigit
	rcall	LCD_gotoXY 	 				; Show_BigDigit
;---------------------------					; Show_BigDigit
; write row two of three for 12-bytes of digit's bitmapped data	; Show_BigDigit
	ldi	temp2, BIGCHARWIDTH	;12			; Show_BigDigit
	ldi	temp, LCD_D 			      		; Show_BigDigit
	sts	VarByteOne, temp				; Show_BigDigit
sbd2:	lpm	temp, z+ ; z already points at correct data	; Show_BigDigit
	sts	VarByteTwo, temp				; Show_BigDigit
	rcall	LCD_write					; Show_BigDigit
	dec	temp2						; Show_BigDigit
	brne	sbd2						; Show_BigDigit
 	lds	temp, Xposition					; Show_BigDigit
	sts	VarByteOne, temp				; Show_BigDigit
	lds	temp, Yposition  				; Show_BigDigit
	subi	temp, - 2 	; add 2 by subtracting -2	; Show_BigDigit
	sts	VarByteTwo, temp				; Show_BigDigit
	rcall	LCD_gotoXY 					; Show_BigDigit
;---------------------------					; Show_BigDigit
; write row three of three for 12-bytes of digit's bitmap data	; Show_BigDigit
	ldi	temp2, BIGCHARWIDTH	;12			; Show_BigDigit
	ldi	temp, LCD_D 					; Show_BigDigit
	sts	VarByteOne, temp				; Show_BigDigit
sbd3:	lpm	temp, z+					; Show_BigDigit
	sts	VarByteTwo, temp				; Show_BigDigit
	rcall	LCD_write					; Show_BigDigit
	dec	temp2						; Show_BigDigit
	brne	sbd3						; Show_BigDigit
								; Show_BigDigit
	ldi	temp, LCD_D 					; Show_BigDigit
	sts	VarByteOne, temp				; Show_BigDigit
	ldi	temp, 0						; Show_BigDigit
	sts	VarByteTwo, temp				; Show_BigDigit
	rcall	LCD_write	; empty column			; Show_BigDigit
								; Show_BigDigit
   								; Show_BigDigit
	pop	ZL						; Show_BigDigit
	pop	ZH						; Show_BigDigit
	pop	Lowtemp1					; Show_BigDigit
	pop	Lowtemp0					; Show_BigDigit
	pop	temp2						; Show_BigDigit
	pop	temp						; Show_BigDigit
 	ret							; Show_BigDigit
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


;**************************************************************** 
; multiply digit value by 36 to get offset into BD table.	;
;  this routine is for AVRs that don't have the MUL instruction	;
;  for example, the Tiny1634					;
;								;
;   Each digit in the table has 36 bytes.			;
;	n * 36 = (n * 32) + (n * 4)				;
; send:		DataByte					;
; return:	resultH:resultL		 			;
;------------------------------------------------------------------------------
Multiply36:		 					; Multiply_36
	push	temp		 				; Multiply_36
	push	temp2		 				; Multiply_36
	push	rBin1L	 	 				; Multiply_36
	push	rBin1H	 	 				; Multiply_36
;					 			; Multiply_36
	lds	temp, DataByte		 			; Multiply_36
	clr	temp2					 	; Multiply_36
 	lsl	temp    rol temp2       ; * 2		 	; Multiply_36
	lsl	temp    rol temp2       ; * 4		 	; Multiply_36
	mov	rBin1L, temp 		; save *4 high:low	; Multiply_36
	mov	rBin1H, temp2					; Multiply_36
 	lsl	temp    rol temp2       ; * 8		 	; Multiply_36
	lsl	temp    rol temp2       ; * 16		 	; Multiply_36
	lsl	temp    rol temp2       ; * 32		 	; Multiply_36
	add	temp,   rBin1L					; Multiply_36
	adc	temp2,  rBin1H		; add *4 to *32		; Multiply_36
	sts	resultL, temp		 			; Multiply_36
	sts	resultH, temp2		 			; Multiply_36
					 			; Multiply_36
	pop	rBin1H	 	 				; Multiply_36
	pop	rBin1L	 	 				; Multiply_36
	pop	temp2	 		 			; Multiply_36
	pop	temp			 			; Multiply_36
	ret							; Multiply_36
					 			; Multiply_36
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



;****************************************************************************************
; Display_ArbLenStr  - select a message string from a set of multiple
; 	 output strings of different lengths using an address table.
;	 Display string at current Nokia 5110 X,Y position (upper-left of first char)
;
; initialize:  rcall LCD_gotoXY  VB1=X: VB2=Y position of Nokia 5110 cursor
; send:   temp with the number of the message string to be output
; return: -
; uses:   ZH:ZL, temp, UART peripheral
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DisplayArbLenStr:
	push	temp2
	push	ZH
	push	ZL

	ldi	ZL, low  (M_StrList << 1)
	ldi	ZH, high (M_StrList << 1)
	lsl	temp ; multiply by 2
	add	ZL,temp
	adc	ZH, zeroreg
	lpm	temp,  Z+ ; move to low addr byte after load
	lpm	temp2, Z
	mov	ZH, temp
	mov	ZL, temp2
DALSlp:
 	lpm	temp, Z+
 	tst	temp
 	breq	DM_exit
	sts	VarByteOne, temp
	rcall	LCD_character
	rjmp	DALSlp

DM_exit:
	pop	ZL
	pop	ZH
	pop	temp2
	ret


;         01234567
m1  : .db "January ",0,0
m2  : .db "February ",0
m3  : .db "March ",0 ,0
m4  : .db "April ",0,0
m5  : .db "May ",0,0
m6  : .db "June ",0
m7  : .db "July ",0
m8  : .db "August ",0
m9  : .db "September ",0,0
m10 : .db "October ",0,0
m11 : .db "November ",0
m12 : .db "December ",0

;; data below used for debugging
;m13 : .db "month 13 ",0
;m14 : .db "month 14 ",0
;m15 : .db "month 15 ",0
;m16 : .db "month 16 ",0
;m17 : .db "month 17 ",0
;m18 : .db "month 18 ",0
;m19 : .db "month 19 ",0
;m20 : .db "month 20 ",0
;m21 : .db "month 21 ",0
;m22 : .db "month 22 ",0
;m23 : .db "month 23 ",0
;m24 : .db "month 24 ",0
;m25 : .db "month 25 ",0
;m26 : .db "month 26 ",0
;m27 : .db "month 27 ",0
;m28 : .db "month 28 ",0
;m29 : .db "month 29 ",0
;m30 : .db "month 30 ",0
;m31 : .db "month 31 ",0
;m32 : .db "month 32 ",0
;m33 : .db "month 33 ",0
;m34 : .db "month 34 ",0
;m35 : .db "month 35 ",0
;m36 : .db "month 36 ",0

M_StrList:
.db high  (M1 << 1), low  (M1 << 1)
.db high  (M2 << 1), low  (M2 << 1)
.db high  (M3 << 1), low  (M3 << 1)
.db high  (M4 << 1), low  (M4 << 1)
.db high  (M5 << 1), low  (M5 << 1)
.db high  (M6 << 1), low  (M6 << 1)
.db high  (M7 << 1), low  (M7 << 1)
.db high  (M8 << 1), low  (M8 << 1)
.db high  (M9 << 1), low  (M9 << 1)
.db high (M10 << 1), low (M10 << 1)
.db high (M11 << 1), low (M11 << 1)
.db high (M12 << 1), low (M12 << 1)

;; data below used for debugging
;.db high (M13 << 1), low (M13 << 1)
;.db high (M14 << 1), low (M14 << 1)
;.db high (M15 << 1), low (M15 << 1)
;.db high (M16 << 1), low (M16 << 1)
;.db high (M17 << 1), low (M17 << 1)
;.db high (M18 << 1), low (M18 << 1)
;.db high (M19 << 1), low (M19 << 1)
;.db high (M20 << 1), low (M20 << 1)
;.db high (M21 << 1), low (M21 << 1)
;.db high (M22 << 1), low (M22 << 1)
;.db high (M23 << 1), low (M23 << 1)
;.db high (M24 << 1), low (M24 << 1)
;.db high (M25 << 1), low (M25 << 1)
;.db high (M26 << 1), low (M26 << 1)
;.db high (M27 << 1), low (M27 << 1)
;.db high (M28 << 1), low (M28 << 1)
;.db high (M29 << 1), low (M29 << 1)
;.db high (M30 << 1), low (M30 << 1)
;.db high (M31 << 1), low (M31 << 1)
;.db high (M32 << 1), low (M32 << 1)
;.db high (M33 << 1), low (M33 << 1)
;.db high (M34 << 1), low (M34 << 1)
;.db high (M35 << 1), low (M35 << 1)
;.db high (M36 << 1), low (M36 << 1)


inline_ID: .db "              BD_clock.asm  v1.0 alan_probandt@yahoo.com"

