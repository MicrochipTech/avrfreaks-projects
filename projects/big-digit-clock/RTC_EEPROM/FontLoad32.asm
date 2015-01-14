;FontLoad32.asm   load fonts into 24C32 EEPROM found on DS1307 RealTime Clock breakout boards
;  displays the contents of the serial EEPROM onto the Nokia 5110 graphics LCD.
;
;  Use Lucida Console font to see correctly. Recommend editor with syntax-coloring like Programmer's Notebook.
;
;  BigDigits font loads into EEPROM start address: 0x000
;  Standard ASCII font loads into EEPROM start address: 0x000 (decimal:576).
;  These are the normal 7x5_pixel characters.
;  BigDigits font loads into EEPROM start address: 0x300
;  Add the equate statements below to programs using EEPROM fonts:
;
; .equ	BD_EEADDRESS	= 0x300	; begining location in EEPROM where Big Digit 12x3rows font data is stored.
; .equ	STD_EEADDRESS	= 0x000	; begining location in EEPROM where standard 7x5 font data is stored.
;
;  See the Nokia5110 display routines for comment/uncomment instructions to activate EEPROM fonts.

;
; When writing both font sets to EEPROM, comment out calls to FillEEblock. Uncomment calls and inits to LoadEEPROM.
; If successful, the display will show the following screens:
;
;   00 00 00 00 00 beginning	1F 00 41 36 08 matches
;      00 AA AA AA data of	   00 AA AA AA data on
;      00 00 5F 00 standard	   02 01 02 04 lines:
;      00 AA AA AA 7x5 char	   02 AA AA AA  2288
;      00 07 00 07 font.	   3C 26 23 26  2289
;      00 AA AA AA space '!'	   3C AA AA AA
;
;   20 FC FE FE 0F		37 00 00 00 00
;      07 07 07 07		   AA AA AA AA
;      CF FC FC F8		   00 00 00 00
;      AA AA AA AA		   00 00 00 00
;      FF FF FF F0		   00 00 00 00
;      78 3C 1E 0F		   AA AA AA AA
;  matches data on	 	  matches data on
;  line# 2298. Begin              line# 2344. End
;  of BigDigit font.              of BigDigit font.
;


.cseg
.org 0x0013
;&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
;   14 x 6 =                           1111 	;&
;   84 total scr chars       01234567890123 	;&
Line0_OpenMsg:		.db "FontLoad32.asm"	;&
			.db "Dec 29, 2014  "	;&
			.db "build: e  v0.1"	;&
			.db "              "	;&
			.db "Alan Probandt "	;&
			.db "Portland,OR   ",0,0;&
;&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

;==================================================================================================
;
; Atmel AVR Mega8  [8K flash / 1K SRAM [address range:0x100 to 0x4ff]/ 512 EEPROM]
;                       _______                           ______
;   reset/  Nokia_reset |1  28| PC5 SCL               A0 |1   2| Vcc (nc)
;                   PD0 |2  27| PC4 SDA               A1 |2   4| WE\
;                   PD1 |3  26| ADC3                  A2 |3   6| SCL
;                   PD2 |4  25| ADC2                 GND |4   5| SDA
;                   PD3 |5  24| ADC1  pot                ------
;                   PD4 |6  23| ADC0                     EEPROM
;              +5V  VCC |7  22| GND
;                   GND |8  21| AREF  +5V                ______
;                   PB6 |9  20| AVCC  +5V           MISO |1  2| Vcc (nc)
;                   PB7 |10 19| PB5  SCK	    SCLK |3  4| MOSI
;                   PD5 |11 18| PB4  MOSI	    RST  |5  6| gnd
;                   PD6 |12 17| PB3  MISO	         ------
;                   PD7 |13 16| PB2  Nokia_SDIN           ISP
;       Nokia_DC    PB0 |14 15| PB1  Nokia_SCLK
;                       -------
;==================================================================================================

.nolist
.include "M8def.inc"
; Mega8 uses IN/OUT for all peripheral_control_register reading/writing. LDS/STS for SRAM variables.
; Other AVR devices use different peripheral addresses. Those above 0x1f use LDS/STS instructions.
; Also adjust interrupt vector table for addresses and 16-bit JMPs.

.list


;======================================
; TIMER equates  (for AVR Mega8)
;--------------------------------------
.equ	clock		=  8000000	; AVR internal system clock [in Hertz]
.equ	OPENING_SCR_TIME =  4	; seconds
.equ	Contrast_Value	=  53

.equ	HB_PRESCALE	=  256	; prescaler for 32 mS increment  using timer 1
.equ	HB_LOGIC	=  (1<<CS12) | (0<<CS11) | (0<<CS10)
.equ	HB_PER_SECOND	=  32	; Heartbeat periods per second. Each period is 31.25 milliSe
.equ	LongPeriod 	=  (clock / HB_PRESCALE) / HB_PER_SECOND   ; HBS=32 >> 977 counts up
.equ	baud38K		=  (clock/(38400*16)) - 1 ; 14  AVR USART baud rate low register value: baud high = 00

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
.equ	BIGSPACEOFFSET		= 11	; offset of beginning of space character data in BigDigits
.equ	MAXHEXPOSITION		= 6 * 4	; 6 rows of 4 hexpairs displayed on Nokia5110 LCD screen.

.equ	BIGDIGIT12X3_PAGECNT	= 18	; number of pages (32 bytes/page 24C32) for Big Digit font [12 x 3 pixels/char]
.equ	STDFONT_PAGECNT		= 24	; number of pages (32 bytes/page 24C32) for standard font  [ 7 x 5 pixels/char]
.equ	RTBRKBRD_EEPAGESIZE	= 32	; page size on the 24C32 serial EEPROM found on RealTime IC DS1307 breakout boards
.equ	BD_EEADDRESS		= 0x300	; begining location in EEPROM where Big Digit 12x3rows font data is to be stored
.equ	STD_EEADDRESS		= 0x000	; begining location in EEPROM where standard 7x5 font data is to be stored
.equ	EEFONT			= 0x000
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.equ	LED_DDR		= DDRD
.equ	LED_PORT	= PORTD
.equ	LED_DEBUG	= 5
.equ	SLA_RTC		= 0xd0  ; 8-bit slave address 1101 for RealTimeClock IC: device address 000
.equ	RTC_STARTADDR	= 0	; only used by SetRTC
.equ	RTCreadSIZE	= 8	; 0x08 for DS1307 RTC IC: 0x0e when using DS3231 RTC IC
.equ	RTCwriteSIZE	= 8	; 0x08 for DS1307 RTC IC: 0x0e when using DS3231 RTC IC
.equ	TWISTOPDELAY	= 45 	; 135 cycles at 0.125uSec/cycle   16.8uSec

.equ	SerEE_PageSize	= 32
.equ	DELAY8USEC	= 136
.equ	ADC_MUX1	= 1
.equ	ADC_MUX0	= 0

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
.def	counter		= r12	; found in ReadPage(TWI) routine
.def	SREGtemp 	= r14	; found in interrupt routines
.def	ZeroReg		= r15	; found in 16-bit add routines

;  upper registers   r16 - r31
.def	temp		= r16
.def	temp2		= r17
.def	temp3		= r18
.def	irqtemp		= r19
.def	FlagsReg	= r20
.equ	  WPOK		= 7
.equ	  RPOK		= 6

; upper register pairs:  r31:r30--ZH:ZL  r29:r28--YH:YL  r27:r26--XH:XL
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

;================================================================================================
;
;	SRAM Definitions
;
;================================================================================================
.dseg ; 1024 bytes SRAM in mega8  START = 0x0060  RAMEND= 0x045f
 ;TaskFlags
.equ	T1_HeartBeat	= 7
.equ	TWIer_flag	= 6
.equ	BlinkLED	= 5
.equ	SetGetFlag 	= 4 ; clr is set routine, set is get routine
.equ	ClearNow	= 3

Page_Buffer:		.byte   128	;0x060
HeartBeatCntr:		.byte	1       ;0x0e0
SecondCount:		.byte	1       ;0x0e1
HighSecCount:		.byte	1       ;0x0e2
PrevSecCount:		.byte	1       ;0x0e3
PrevHiSecCnt:		.byte	1       ;0x0e4
VarByteOne:		.byte	1       ;0x0e5
VarByteTwo:		.byte	1       ;0x0e6
VarByteThree:		.byte	1       ;0x0e7
PtrOneLow:		.byte	1       ;0x0e8
PtrOneHigh:		.byte	1       ;0x0e9
PtrTwoLow:		.byte	1       ;0x0ea
PtrTwoHigh:		.byte	1       ;0x0eb
DataByte:		.byte	1       ;0x0ec
EEPageSize:		.byte	1       ;0x0ed
PageCount:		.byte	1       ;0x0ee
TaskFlags:		.byte	1       ;0x0ef
ByteASChigh:		.byte	1       ;0x0f0
ByteASCLow:		.byte	1       ;0x0f1
ASCIIdigits:		.byte	5       ;0x0f2
ZeroLoc:		.byte	1       ;0x0f7
resultL:		.byte	1       ;0x0f8
resultH:		.byte	1       ;0x0f9
TWIstepValue:		.byte	1       ;0x0fa
ReceivedTWSR:		.byte	1       ;0x0fb
ExpectedTWSR:		.byte	1       ;0x0fc
SLA:			.byte	1       ;0x0fd
ReadBlockSize:		.byte	1       ;0x0fe
WriteBlockSize:		.byte	1       ;0x0ff
HighAddress:		.byte	1       ;0x100
LowAddress:		.byte	1       ;0x101
ADConeValLow:		.byte	1       ;0x102
ADConeValHigh:		.byte	1       ;0x103
ADCOnePrevLow:		.byte	1       ;0x104
ADCOnePrevHigh:		.byte	1       ;0x105
HexPosition:		.byte	1	;0x106
SCRpot:			.byte	1	;0x107


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

;===================================================================================================
.org 0x040  ; vector tables and opening screen occupy flash below 0x040
reset:
	ldi	temp, high (RAMEND)	; On Mega8, need to initialize stack pointer to end of SRAM.
	out	SPH, temp		; Not needed on other Mega AVRs, but good idea to do
	ldi	temp, low  (RAMEND)	; in case your embedded-system crash handler code jumps
	out	SPL, temp		; to reset (flash addr 000) instead of doing a hardware reset

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

;;----------------------------------------------------------
;; initialize USART   ; USCR0C default values OK for 8/N/1
;rst_UART:
;	ldi	temp, high (baud38K) ;Baud rate H
;	out	UBRRH, temp
;	ldi	temp, low  (baud38K) ;Baud rate L
;	out	UBRRL, temp
;	ldi	temp, (1<<RXEN) | (1<<TXEN) | (1<<RXCIE)
;	out	UCSRB, temp  ; turn on receive and transmit


;----------------------------------------------------------------------------------------------
; initialize TwoWire Interface
	ldi 	temp, 2		; cpu clock / (16 + (2*twbr) * prescale
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
; control byte for 24LC512, 24LC256, 24LC128, 24LC64, -- 1010 control code, IC addr 3 bits (always 1
	ldi	temp,  0b10100000
	sts	SLA, temp

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

	ldi	temp, ADC_MUX1
	rcall	doADC10
	lds	temp, ADConeValHigh	; bits 7:0 hold ADC 9:2
	sts	ADCOnePrevHigh, temp

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
	cpi	temp, OPENING_SCR_TIME
	brne	initLoop0

  	rcall	LCD_clear


;********************************************************************************
; Font loading into EEPROM routines
;
;  Loading data into a serial EEPROM requires four parameters:
;  -PtrOneHigh:Low  Location of data in the flash of the host loader program
;  -PtrTwoHigh:Low  Address in the EEPROM where the data will be written
;  -EEPageSize        The size of the memory page for the EEPROM type
;  -PageCount       The number of pages to be written into the EEPROM
;
;  These parameters are stored in .equ and label statements before the
;  font loader routine is called. An example set of .equ lines is:
;
;	BigDigitEEFont:  flash code label of .db data
;	.equ	BD_EEADDRESS  = 0x200
;	.equ	BIGDIGIT12X3_PAGECNT = 18
;	.equ	RTBRKBRD_EEPAGESIZE  = 32  24C32 EEPROM page size
;
;  The font loader calls the routine that writes to EEPROM. This
;  routine (WritePage) receives several SRAM variables that need
;  to be initialized before the font loader code is called.
;
;	SLA		EEPROM device address
;	WriteBlockSize	number of bytes written to EEPROM
;	Page_Buffer	SRAM that holds data being written to EE
;	HighAddress:LowAddress   location in EE where data gets written
;
;  The font loader uses a register that is always value 0x00: ZeroReg.
;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


; Load standard font [7x5 pixels] into 24C32 EEPROM.  Initialize four parameters.
	ldi	temp, high (StandardEEFont << 1)
	ldi	temp2, low (StandardEEFont << 1)
	sts	PtrOneHigh, temp
	sts	PtrOneLow, temp2

	ldi	temp, high (STD_EEADDRESS)
	ldi	temp2, low (STD_EEADDRESS)	; 0x0000 first data block in EEPROM
	sts	PtrTwoHigh, temp
	sts	PtrTwoLow,  temp2

	ldi	temp, STDFONT_PAGECNT
	sts	PageCount, temp

	ldi	temp2, RTBRKBRD_EEPAGESIZE
	sts	EEPageSize, temp2

	rcall	LoadEEPROM

;  Load Big Digit font [12x3rows] into 24C32 EEPROM. Initialize four parameters.
	ldi	temp, high (BigDigitEEFont << 1)
	ldi	temp2, low (BigDigitEEFont << 1)
	sts	PtrOneHigh, temp
	sts	PtrOneLow,  temp2

	ldi	temp, high (BD_EEADDRESS)	 ; 0x0300 second data block in EEPROM
	ldi	temp2, low (BD_EEADDRESS)
	sts	PtrTwoHigh, temp
	sts	PtrTwoLow,  temp2

	ldi	temp,  BIGDIGIT12X3_PAGECNT
	sts	PageCount, temp

	ldi	temp2, RTBRKBRD_EEPAGESIZE
	sts	EEPageSize, temp2

	rcall	LoadEEPROM

;-----------


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



;***************************************************************;
; Load_EEPROM   Branch to here in Reset/Init section		;
;                                                       	;
; Load data from flash memory into serial EEPROM.          	;
; Data is stored at the end of this program.                    ;
; The data size should ALWAYS be a multiple of the page size.	;
;                                                               ;
; send: EEPageSize   serial EEPROM's page size in bytes		;
;	PageCount  number of pages transfered from flash to EE  ;
; 	PtrOneHigh:PtrOneLow  _ address in flash of EE data	;
;       PtrTwoHigh:PtrTwoLow  _ address in EE to write		;
;								;
; uses:   	SLA    initialized for EEPROM to 0xa0		;
; [SRAM]	HighAddress:LowAddress				;
;		WriteBlockSize					;
;		Page_Buffer					;
;		PtrOneHigh:PtrOneLow   EE data's flash address	;
;		PtrTwoHigh:PtrTwoLow   EE write address		;
;								;
; [registers]	ZeroPage, temp, temp2, temp3,			;
;		ZH:ZL, YH:YL, XH:XL, R0:R1			;
;								;
; calls:	WritePage 					;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Load_EEPROM
LoadEEPROM:  							; Load_EEPROM
	push	temp2						; Load_EEPROM
	push	ZH						; Load_EEPROM
	push	ZL						; Load_EEPROM
	push	YH						; Load_EEPROM
	push	YL						; Load_EEPROM
	push	XH 						; Load_EEPROM
	push	XL						; Load_EEPROM
								; Load_EEPROM
	lds	ZH, PtrOneHigh ; address in flash of EE data	; Load_EEPROM
	lds	ZL, PtrOneLow  					; Load_EEPROM
	  							; Load_EEPROM
	lds	temp,  PtrTwoHigh				; Load_EEPROM
 	sts	HighAddress, temp 	; address in EE where	; Load_EEPROM
 	lds	temp,  PtrTwoLow	; data will be written	; Load_EEPROM
	sts	LowAddress, temp 				; Load_EEPROM
	  							; Load_EEPROM
; calculate number of bytes to write to EEPROM from #pages	; Load_EEPROM
	lds	temp,  EEPageSize				; Load_EEPROM
	lds	temp2, PageCount				; Load_EEPROM
	mul	temp, temp2					; Load_EEPROM
	mov	YL, r0						; Load_EEPROM
	mov	YH, r1						; Load_EEPROM
								; Load_EEPROM
	ldi	XH, high (Page_Buffer)	; SRAM buffer for data	; Load_EEPROM
	ldi	XL, low  (Page_Buffer)	; to be written to EE	; Load_EEPROM
	lds	temp,  EEPageSize				; Load_EEPROM
	sts	WriteBlockSize, temp ; variable for WritePage	; Load_EEPROM
								; Load_EEPROM
;-------------------						; Load_EEPROM
; load page buffer with one page of font data			; Load_EEPROM
LdEE:	lds	temp2, EEPageSize				; Load_EEPROM
LdEE_a:	lpm	temp, z+					; Load_EEPROM
	st	x+, temp					; Load_EEPROM
	dec	temp2						; Load_EEPROM
	brne	LdEE_a						; Load_EEPROM
; send data block of one full page size to EEPROM 		; Load_EEPROM
	rcall	WritePage ; WritePage receives WriteBlockSize,	; Load_EEPROM
; 		    HighAddress, LowAddress, SLA, Page_Buffer 	; Load_EEPROM
								; Load_EEPROM
;-------------------						; Load_EEPROM
; Continue page writes until all data is written to EEPROM.	; Load_EEPROM
; adjust parameters for next page write				; Load_EEPROM
	lds	XL, LowAddress	; adjust page address for EE	; Load_EEPROM
	lds	XH, HighAddress					; Load_EEPROM
	lds	temp, EEPageSize				; Load_EEPROM
	add	XL, temp					; Load_EEPROM
	adc	XH, ZeroReg					; Load_EEPROM
	sts	LowAddress,  XL					; Load_EEPROM
	sts	HighAddress, XH					; Load_EEPROM
								; Load_EEPROM
	ldi	XH, high (Page_Buffer) ; repoint X to the start	; Load_EEPROM
	ldi	XL, low  (Page_Buffer) ; of the SRAM data buffer; Load_EEPROM
								; Load_EEPROM
	lds	temp, EEPageSize				; Load_EEPROM
	sub	YL, temp					; Load_EEPROM
	sbc	YH, ZeroReg					; Load_EEPROM
	cp	YL, ZeroReg					; Load_EEPROM
	cpc	YH, ZeroReg					; Load_EEPROM
	breq	LdEE_x						; Load_EEPROM
	rjmp	LdEE						; Load_EEPROM
LdEE_x:								; Load_EEPROM
			        				; Load_EEPROM
;-----------------------------					; Load_EEPROM
	pop	XL	        				; Load_EEPROM
	pop	XH	        				; Load_EEPROM
	pop	YL	        				; Load_EEPROM
	pop	YH	        				; Load_EEPROM
	pop	ZL	        				; Load_EEPROM
	pop	ZH	        				; Load_EEPROM
	pop	temp2          					; Load_EEPROM
	ret							; Load_EEPROM
								; Load_EEPROM
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%; Load_EEPROM






;;***************************************************************;
;; Fill_EEBlock  needs debugging! ######			;
;;                                                       	;
;; Load 0xff data from flash memory into serial EEPROM.        	;
;; The data size should ALWAYS be a multiple of the page size.	;
;;                                                              ;
;; send: EEPageSize   serial EEPROM's page size in bytes	;
;;	PageCount  number of pages transfered from flash to EE  ;
;;       PtrOneHigh:PtrOneLow  _ address in EE to write		;
;;								;
;; uses:   	SLA    initialized for EEPROM to 0xa0		;
;; [SRAM]	HighAddress:LowAddress				;
;;		WriteBlockSize					;
;;		Page_Buffer					;
;;		PtrOneHigh:PtrOneLow   EE write address		;
;;								;
;; [registers]	ZeroPage, temp, temp2, temp3,			;
;;		ZH:ZL, YH:YL, XH:XL, R0:R1			;
;;								;
;; calls:	WritePage 					;
;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;
;FillEEBlock:							; Fill_EE
;	push	temp2						; Fill_EE
;	push	temp3						; Fill_EE
;	push	ZH						; Fill_EE
;	push	ZL						; Fill_EE
;	push	YH						; Fill_EE
;	push	YL						; Fill_EE
;	push	XH 						; Fill_EE
;	push	XL						; Fill_EE
;								; Fill_EE
;	lds	temp,  PtrOneHigh				; Fill_EE
; 	sts	HighAddress, temp 	; address in EE where	; Fill_EE
; 	lds	temp,  PtrOneLow	; data will be written	; Fill_EE
;	sts	LowAddress, temp 				; Fill_EE
;	  							; Fill_EE
;	ldi	ZH, high (Page_Buffer)	; SRAM buffer for data	; Fill_EE
;	ldi	ZL, low  (Page_Buffer)	; to be written to EE	; Fill_EE
;	lds	temp2,  EEPageSize				; Fill_EE
;	sts	WriteBlockSize, temp2 ; variable for WritePage	; Fill_EE
;	ldi	temp, 0xff					; Fill_EE
;;-------------------						; Fill_EE
;; load page buffer with one page of font data			; Fill_EE
;FlEE:	  							; Fill_EE	
;	st	z+, temp					; Fill_EE
;	dec	temp2						; Fill_EE
;	brne	FlEE						; Fill_EE
;	  							; Fill_EE
;; send data block of one full page size to EEPROM 		; Fill_EE
;FlEE_b:							; Fill_EE
;	rcall	WritePage ; WritePage receives Write_BlockSize,	; Fill_EE
;; 		    HighAddress, LowAddress, SLA, Page_Buffer 	; Fill_EE
;;-------------------						; Fill_EE
;; Continue page writes until all data is written to EEPROM.	; Fill_EE
;; adjust parameters for next page write			; Fill_EE
;	lds	XL, LowAddress	; adjust page address for EE	; Fill_EE
;	lds	XH, HighAddress					; Fill_EE
;	lds	temp2, EEPageSize				; Fill_EE
;	add	XL, temp2					; Fill_EE
;	adc	XH, ZeroReg					; Fill_EE
;	sts	LowAddress,  XL					; Fill_EE
;	sts	HighAddress, XH					; Fill_EE
;	ldi	ZH, high (Page_Buffer)	; SRAM buffer for data	; Fill_EE
;	ldi	ZL, low  (Page_Buffer)	; to be written to EE	; Fill_EE
;								; Fill_EE
;	lds 	temp, PageCount					; Fill_EE
;	dec	temp						; Fill_EE
;	sts	PageCount, temp					; Fill_EE
;	tst	temp						; Fill_EE
;	brne	FlEE_b						; Fill_EE
;			        				; Fill_EE
;;-----------------------------					; Fill_EE
;	pop	XL	        				; Fill_EE
;	pop	XH	        				; Fill_EE
;	pop	YL	        				; Fill_EE
;	pop	YH	        				; Fill_EE
;	pop	ZL	        				; Fill_EE
;	pop	ZH	        				; Fill_EE
;	pop	temp3						; Fill_EE
;	pop	temp2          					; Fill_EE
;	ret							; Fill_EE
;								; Fill_EE
;;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%; Fill_EE
;









;***************************************************************;**********
;  HeartBeat   Branch to here HB_PER_SECOND 			; HeartBeat
;    Branches here from main when mainloop detects		; HeartBeat
;     TaskFlags:T1_Heartbeat is set.				; HeartBeat
; 								; HeartBeat
;  Checks for debug-indicator (quick flash of LED) is active.	; HeartBeat
;  LED will flash if the selected code point is run.		; HeartBeat
;  LED turns on until next time HBcntr mod 32=0.		; HeartBeat
;  To use: place these three instructions into code:		; HeartBeat
;								; HeartBeat
;          lds	temp, TaskFlags ; turn on T1_HB flag next HB	; HeartBeat
;          sbr	temp, ( 1 << T1_HeartBeat )			; HeartBeat
;          sts	TaskFlags, temp					; HeartBeat
;								; HeartBeat
;  Check for potentiometer change.  On any pot turn, convert	; HeartBeat
;  the new setting to an EEPROM address and display the 24-byte	; HeartBeat
;  block of data found at that EEPROM location (in Hex).	; HeartBeat
;								; HeartBeat
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
; Check for new secondCount value				; HeartBeat
	lds	temp,  SecondCount	; incremented by 	; HeartBeat
	lds	temp2, PrevSecCount	; the T1_overflow IRQ	; HeartBeat
	cp	temp, temp2					; HeartBeat
	breq	HBP_exit					; HeartBeat
								; HeartBeat
;--------------------						; HeartBeat
	sts	PrevSecCount, temp				; HeartBeat
								; HeartBeat
;===============================================================; HeartBeat
;;Simple display second count to indicate heartbeat activity	; HeartBeat
;	sts	DataByte, temp					; HeartBeat
;	ldi	temp, 3	  ; row 				; HeartBeat
;	sts	VarByteTwo, temp				; HeartBeat
;	ldi	temp, 0 * PIXELSperCHAR				; HeartBeat
;	sts	VarByteOne, temp				; HeartBeat
;	rcall	LCD_gotoXY					; HeartBeat
;	rcall	DisplayHexByte					; HeartBeat
;	rjmp	HBP_exit					; HeartBeat
;===============================================================; HeartBeat
;								; HeartBeat
; Check for new pot value. if yes, read EE, display 24 bytes	; HeartBeat
	ldi	temp, ADC_MUX1					; HeartBeat
	rcall	doADC10						; HeartBeat
	lds	temp,  ADConeValHigh				; HeartBeat
 	lds	temp2, ADCOnePrevHigh				; HeartBeat
 	cpse	temp, temp2					; HeartBeat
 	rjmp	HBP_0a						; HeartBeat
 	rjmp	HBP_0x						; HeartBeat
HBP_0a:								; HeartBeat
	sts	ADCOnePrevHigh, temp 				; HeartBeat
	lsr	temp		; 128*24=3072			; HeartBeat
	sts	SCRpot, temp	; screen pot value from ADCH/2	; HeartBeat
	ldi	temp2, 24					; HeartBeat
	mul	temp, temp2					; HeartBeat
	sts	LowAddress,  r0					; HeartBeat
	sts	HighAddress, r1					; HeartBeat
	ldi	temp, MAXHEXPOSITION	; 24			; HeartBeat
	sts	ReadBlockSize, temp 				; HeartBeat
	rcall	ReadPage					; HeartBeat
	rcall	WriteFullScr	 				; HeartBeat
HBP_0x:								; HeartBeat
								; HeartBeat
HBP_exit:							; HeartBeat
	pop	ZL						; HeartBeat
	pop	ZH						; HeartBeat
	pop	temp						; HeartBeat
	ret							; HeartBeat
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%%%%%%%%%%



;***************************************************************;***********
;Write_Byte    write a hex-pair stream				; Write_Byte
;	Takes about 1 millisecond to write a hex pair		; Write_Byte
;  Checks if Task_Flags:ClearNow flag set. if yes, then 	; Write_Byte
; clear LCD screen and write hex pair at (0,0) 			; Write_Byte
;    								; Write_Byte
;      If Task_Flags:ClearNow is not set, then checks		; Write_Byte
;  HexPosition range (0-23).  If out of range, moves		; Write_Byte
;  cursor to top-left corner. { gotoxy(0,0) }			; Write_Byte
;  if HexPosition mod 3 == 0, then reposition cursor		; Write_Byte
;  to leftmost of four write locations of next row.		; Write_Byte
;  Else, move to write location according to mod, 		; Write_Byte
;  writes two ASCII hex characters and a space, then		; Write_Byte
;  increments HexPosition SRAM variable, test for max,		; Write_Byte
;  If at max, set HPX to 0, and set TaskFlags:ClearNow		; Write_Byte
;								; Write_Byte
;  receives:	{SRAM} DataByte, HexPosition 			; Write_Byte
;		TaskFlags:ClearNow				; Write_Byte
;  calls:	LCD__gotoXY, Display_HexByte			; Write_Byte
;  called by:	Heartbeat, Do_NewMIDI, doNewscan_code		; Write_Byte
;---------------------------------------------------------------; Write_Byte
WriteByte:							; Write_Byte
	push	temp						; Write_Byte
	push	ZL						; Write_Byte
	push	ZH						; Write_Byte
;								; Write_Byte
	lds	temp, TaskFlags					; Write_Byte
	sbrs	temp, ClearNow					; Write_Byte
	rjmp	WB_a						; Write_Byte
;								; Write_Byte
	cbr	temp, (1 <<  ClearNow)				; Write_Byte
	sts	TaskFlags, temp					; Write_Byte
	rcall	LCD_clear					; Write_Byte
	clr	temp						; Write_Byte
	sts	HexPosition, temp				; Write_Byte
;								; Write_Byte
; determine which column to write hexpair			; Write_Byte
WB_a:	lds	temp, HexPosition				; Write_Byte
	andi	temp, 0b00000011				; Write_Byte
	ldi	ZL, low  ( WBcolumn << 1)			; Write_Byte
	ldi	ZH, high ( WBcolumn << 1)			; Write_Byte
	add	ZL, temp					; Write_Byte
	adc	ZH, ZeroReg					; Write_Byte
	lpm	temp, z						; Write_Byte
	sts	VarByteOne, temp	; x (column)		; Write_Byte
	lds	temp, HexPosition				; Write_Byte
	lsr	temp						; Write_Byte
	lsr	temp						; Write_Byte
	sts	VarByteTwo, temp	; y (row #)		; Write_Byte
	rcall	LCD_gotoXY					; Write_Byte
;								; Write_Byte
	rcall	DisplayHexByte	; receives DataByte		; Write_Byte
								; Write_Byte
	lds	temp, HexPosition				; Write_Byte
	inc	temp						; Write_Byte
	cpi	temp, MAXHEXPOSITION 				; Write_Byte
	brlo	WB_w						; Write_Byte
; check if screen if full, if yes don't erase screen		; Write_Byte
;	until next MIDI in					; Write_Byte
	lds	temp, TaskFlags					; Write_Byte
	sbr	temp, (1 << ClearNow)				; Write_Byte
	sts	TaskFlags, temp					; Write_Byte
;								; Write_Byte
WB_w:	sts	HexPosition, temp				; Write_Byte
								; Write_Byte
WB_exit:							; Write_Byte
	pop	ZH						; Write_Byte
	pop	ZL						; Write_Byte
	pop	temp						; Write_Byte
	ret							; Write_Byte
								; Write_Byte
WBcolumn: .db	18, 16+18, 32+18, 48+18 			; Write_Byte
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%%%%%%%%%%%




;***************************************************************;**************
; Display_HexByte 	; displays DataByte at current cursor.	;Display_HexByte
;								;Display_HexByte
; send:      	{SRAM} DataByte 				;Display_HexByte
; returns:   	---						;Display_HexByte
; calls:	LCD_character					;Display_HexByte
; SRAM:		Var_ByteOne					;Display_HexByte
; registers:	temp, ZH:ZL, ZeroReg				;Display_HexByte
; called by:	Write_Byte		 			;Display_HexByte
;   								;Display_HexByte
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~;Display_HexByte
DisplayHexByte:	; send {SRAM} DataByte				;Display_HexByte
	push	ZH						;Display_HexByte
	push	ZL						;Display_HexByte
	push	temp						;Display_HexByte
;   								;Display_HexByte
	lds	temp, DataByte					;Display_HexByte
	andi	temp, 0xf0					;Display_HexByte
	swap	temp						;Display_HexByte
 	ldi	ZH, high (HexChars << 1)			;Display_HexByte
 	ldi	ZL, low  (HexChars << 1) 			;Display_HexByte
 	add	ZL, temp					;Display_HexByte
 	adc	ZH, ZeroReg					;Display_HexByte
 	lpm	temp, z						;Display_HexByte
 	sts	VarByteOne, temp				;Display_HexByte
	rcall	LCD_character					;Display_HexByte
;								;Display_HexByte
 	lds	temp, DataByte					;Display_HexByte
 	andi	temp, 0x0f					;Display_HexByte
  	ldi	ZH, high (HexChars << 1)			;Display_HexByte
  	ldi	ZL, low  (HexChars << 1)			;Display_HexByte
  	add	ZL, temp					;Display_HexByte
	adc	ZH, ZeroReg					;Display_HexByte
  	lpm	temp, z						;Display_HexByte
  	sts	VarByteOne, temp				;Display_HexByte
 	rcall	LCD_character					;Display_HexByte
;								;Display_HexByte
	pop	temp						;Display_HexByte
	pop	ZL						;Display_HexByte
	pop	ZH						;Display_HexByte
	ret							;Display_HexByte
;								;Display_HexByte
HexChars: .db "0123456789ABCDEF ",0				;Display_HexByte
;%%%%%%%%%%%%  end of Hex-ASCII routines %%%%%%%%%%%%%%%%%%%%%%%;Display_HexByte




;***************************************************************;**************
;Write_FullScr   fill Nokia screen with 24 hex pairs		; Write_FullScr
								; Write_FullScr
;  receives:	[SRAM] 	ScrPot, Page_Buffer			; Write_FullScr
;								; Write_FullScr
;  calls:	LCD__gotoXY, Display_HexByte			; Write_FullScr
;  called by:	Heartbeat, 					; Write_FullScr
;---------------------------------------------------------------; Write_FullScr
WriteFullScr:							; Write_FullScr
	push	temp						; Write_FullScr
	push	temp2						; Write_FullScr
	push	temp3						; Write_FullScr
	push	ZL						; Write_FullScr
	push	ZH						; Write_FullScr
	push	YL						; Write_FullScr
	push	YH						; Write_FullScr
;								; Write_FullScr
	rcall	LCD_clear					; Write_FullScr
;  Display screen number (hex) in upper left of Nokia 5110	; Write_FullScr
	lds	temp, SCRpot	 				; Write_FullScr
	sts	DataByte, temp					; Write_FullScr
	ldi	temp, 0	  ; row 				; Write_FullScr
	sts	VarByteTwo, temp				; Write_FullScr
	ldi	temp, 0 * PIXELSperCHAR				; Write_FullScr
	sts	VarByteOne, temp				; Write_FullScr
	rcall	LCD_gotoXY					; Write_FullScr
	rcall	DisplayHexByte					; Write_FullScr
;								; Write_FullScr
	ldi	YH, high (Page_Buffer)	 			; Write_FullScr
	ldi	YL, low  (Page_Buffer)				; Write_FullScr
	clr	temp2						; Write_FullScr
	sts	HexPosition, temp2 				; Write_FullScr
;								; Write_FullScr
WrFS_write: 							; Write_FullScr
; determine which column to write hexpair. Go there		; Write_FullScr
 	lds	temp, HexPosition				; Write_FullScr
	andi	temp, 0b00000011				; Write_FullScr
	ldi	ZL, low  (WrFScolumn << 1)			; Write_FullScr
	ldi	ZH, high (WrFScolumn << 1)			; Write_FullScr
	add	ZL, temp					; Write_FullScr
	adc	ZH, ZeroReg					; Write_FullScr
	lpm	temp, z						; Write_FullScr
	sts	VarByteOne, temp	; x (column)		; Write_FullScr
	lds	temp2, HexPosition ; determine row#		; Write_FullScr
	lsr	temp2						; Write_FullScr
	lsr	temp2						; Write_FullScr
	sts	VarByteTwo, temp2	; y (row #)		; Write_FullScr
	rcall	LCD_gotoXY					; Write_FullScr
;								; Write_FullScr
	ld	temp, y+    					; Write_FullScr
	sts	DataByte, temp					; Write_FullScr
	rcall	DisplayHexByte					; Write_FullScr
	lds	temp, HexPosition				; Write_FullScr
	inc	temp						; Write_FullScr
	sts	HexPosition, temp				; Write_FullScr
	cpi	temp, MAXHEXPOSITION				; Write_FullScr
	brlo	WrFS_write					; Write_FullScr
								; Write_FullScr
WrFS_exit:							; Write_FullScr
	pop	YH						; Write_FullScr
	pop	YL						; Write_FullScr
	pop	ZH						; Write_FullScr
	pop	ZL						; Write_FullScr
	pop	temp3						; Write_FullScr
	pop	temp2						; Write_FullScr
	pop	temp						; Write_FullScr
								; Write_FullScr
	ret							; Write_FullScr
								; Write_FullScr
WrFScolumn: .db	18, 16+18, 32+18, 48+18				; Write_FullScr
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%%%%%%%%%%%%%%



;***************************************************************; Nokia_5110
;  Nokia 5110 graphics display routines:			; Nokia_5110
;	Source code @600 bytes					; Nokia_5110
;	plus font tables: ASCII chars 480 bytes			; Nokia_5110
;	total:    @1100 bytes 					; Nokia_5110
;								; Nokia_5110
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
;								; Nokia_5110
;; if using font in EEPROM, comment all from here to LCy:	; Nokia_5110
		 						; Nokia_5110
; multiply temp2 by 5 [number of bytes per bit-mapped char] 	; Nokia_5110
	mov	temp3, temp2					; Nokia_5110
	clr	temp						; Nokia_5110
	lsl	temp2 						; Nokia_5110
	rol	temp						; Nokia_5110
	lsl	temp2						; Nokia_5110
	rol	temp						; Nokia_5110
	add	temp2, temp3	;  low of offset into table	; Nokia_5110
	adc	temp, ZeroReg	; high of offset into table	; Nokia_5110
; 								; Nokia_5110
;Lchar0:							; Nokia_5110
	ldi	ZH, high (InLineFont << 1)			; Nokia_5110
	ldi	ZL, low  (InLineFont << 1) 			; Nokia_5110
	add	ZL, temp2 ; ZH:ZL points to first of five bytes ; Nokia_5110
	adc	ZH, temp					; Nokia_5110
	ldi	temp3, 5					; Nokia_5110
Lchar_loop:							; Nokia_5110
	lpm	temp, z+					; Nokia_5110
;; LCy:	_this line is always commented				; Nokia_5110 
;								; Nokia_5110
;; When using in-line (not EEPROM) font, comment lines to LCz.	; Nokia_5110
;; when font is stored in serial EEPROM, use lines to LCz:	; Nokia_5110
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
;
InLineFont:  ; 480 bytes; 96 character's font data;  5 col x 7 rows per char
; 2 chars/line as the assembler must have an even number of .db bytes on each line.	; Nokia 5110
.db	0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x5F, 0x00, 0x00 ;   space  21 !	; Nokia 5110
.db	0x00, 0x07, 0x00, 0x07, 0x00,  0x14, 0x7F, 0x14, 0x7F, 0x14 ;   22 "   23 #	; Nokia 5110
.db	0x24, 0x2A, 0x7F, 0x2A, 0x12,  0x23, 0x13, 0x08, 0x64, 0x62 ;   24 $   25 %	; Nokia 5110
.db	0x36, 0x49, 0x56, 0x20, 0x50,  0x00, 0x08, 0x07, 0x03, 0x00 ;   26 &   27 '	; Nokia 5110
.db	0x00, 0x1C, 0x22, 0x41, 0x00,  0x00, 0x41, 0x22, 0x1C, 0x00 ;   28 (   29 )	; Nokia 5110
.db	0x2A, 0x1C, 0x7F, 0x1C, 0x2A,  0x08, 0x08, 0x3E, 0x08, 0x08 ;   2a *   2b +	; Nokia 5110
.db	0x00, 0x80, 0x70, 0x30, 0x00,  0x08, 0x08, 0x08, 0x08, 0x08 ;   2c ,   2d -	; Nokia 5110
.db	0x00, 0x00, 0x60, 0x60, 0x00,  0x20, 0x10, 0x08, 0x04, 0x02 ;   2e .   2f ;	; Nokia 5110
.db	0x3E, 0x51, 0x49, 0x45, 0x3E,  0x00, 0x42, 0x7F, 0x40, 0x00 ;   30 0   31 1	; Nokia 5110
.db	0x72, 0x49, 0x49, 0x49, 0x46,  0x21, 0x41, 0x49, 0x4D, 0x33 ;   32 2   33 3	; Nokia 5110
.db	0x18, 0x14, 0x12, 0x7F, 0x10,  0x27, 0x45, 0x45, 0x45, 0x39 ;   34 4   35 5	; Nokia 5110
.db	0x3C, 0x4A, 0x49, 0x49, 0x31,  0x41, 0x21, 0x11, 0x09, 0x07 ;   36 6   37 7	; Nokia 5110
.db	0x36, 0x49, 0x49, 0x49, 0x36,  0x46, 0x49, 0x49, 0x29, 0x1E ;   38 8   39 9	; Nokia 5110
.db	0x00, 0x00, 0x14, 0x00, 0x00,  0x00, 0x40, 0x34, 0x00, 0x00 ;   3a :   3b ;	; Nokia 5110
.db	0x00, 0x08, 0x14, 0x22, 0x41,  0x14, 0x14, 0x14, 0x14, 0x14 ;   3c <   3d =	; Nokia 5110
.db	0x00, 0x41, 0x22, 0x14, 0x08,  0x02, 0x01, 0x59, 0x09, 0x06 ;   3e >   3f ?	; Nokia 5110
.db	0x3E, 0x41, 0x5D, 0x59, 0x4E,  0x7C, 0x12, 0x11, 0x12, 0x7C ;   40 @   41 A	; Nokia 5110
.db	0x7F, 0x49, 0x49, 0x49, 0x36,  0x3E, 0x41, 0x41, 0x41, 0x22 ;   42 B   43 C	; Nokia 5110
.db	0x7F, 0x41, 0x41, 0x41, 0x3E,  0x7F, 0x49, 0x49, 0x49, 0x41 ;   44 D   45 E	; Nokia 5110
.db	0x7F, 0x09, 0x09, 0x09, 0x01,  0x3E, 0x41, 0x41, 0x51, 0x73 ;   46 F   47 G	; Nokia 5110
.db	0x7F, 0x08, 0x08, 0x08, 0x7F,  0x00, 0x41, 0x7F, 0x41, 0x00 ;   48 H   49 I	; Nokia 5110
.db	0x20, 0x40, 0x41, 0x3F, 0x01,  0x7F, 0x08, 0x14, 0x22, 0x41 ;   4a J   4b K	; Nokia 5110
.db	0x7F, 0x40, 0x40, 0x40, 0x40,  0x7F, 0x02, 0x1C, 0x02, 0x7F ;   4c L   4d M	; Nokia 5110
.db	0x7F, 0x04, 0x08, 0x10, 0x7F,  0x3E, 0x41, 0x41, 0x41, 0x3E ;   4e N   4f O	; Nokia 5110
.db	0x7F, 0x09, 0x09, 0x09, 0x06,  0x3E, 0x41, 0x51, 0x21, 0x5E ;   50 P   51 Q	; Nokia 5110
.db	0x7F, 0x09, 0x19, 0x29, 0x46,  0x26, 0x49, 0x49, 0x49, 0x32 ;   52 R   53 S	; Nokia 5110
.db	0x03, 0x01, 0x7F, 0x01, 0x03,  0x3F, 0x40, 0x40, 0x40, 0x3F ;   54 T   55 U	; Nokia 5110
.db	0x1F, 0x20, 0x40, 0x20, 0x1F,  0x3F, 0x40, 0x38, 0x40, 0x3F ;   56 V   57 W	; Nokia 5110
.db	0x63, 0x14, 0x08, 0x14, 0x63,  0x03, 0x04, 0x78, 0x04, 0x03 ;   58 X   59 Y	; Nokia 5110
.db	0x61, 0x59, 0x49, 0x4D, 0x43,  0x00, 0x7F, 0x41, 0x41, 0x41 ;   5a Z   5b [	; Nokia 5110
.db	0x02, 0x04, 0x08, 0x10, 0x20,  0x00, 0x41, 0x41, 0x41, 0x7F ;   5c ¥   5d ]	; Nokia 5110
.db	0x04, 0x02, 0x01, 0x02, 0x04,  0x40, 0x40, 0x40, 0x40, 0x40 ;   5e ^   5f _	; Nokia 5110
.db	0x00, 0x03, 0x07, 0x08, 0x00,  0x20, 0x54, 0x54, 0x78, 0x40 ;   60 `   61 a	; Nokia 5110
.db	0x7F, 0x28, 0x44, 0x44, 0x38,  0x38, 0x44, 0x44, 0x44, 0x28 ;   62 b   63 c	; Nokia 5110
.db	0x38, 0x44, 0x44, 0x28, 0x7F,  0x38, 0x54, 0x54, 0x54, 0x18 ;   64 d   65 e	; Nokia 5110
.db	0x00, 0x08, 0x7E, 0x09, 0x02,  0x18, 0xA4, 0xA4, 0x9C, 0x78 ;   66 f   67 g	; Nokia 5110
.db	0x7F, 0x08, 0x04, 0x04, 0x78,  0x00, 0x44, 0x7D, 0x40, 0x00 ;   68 h   69 i	; Nokia 5110
.db	0x20, 0x40, 0x40, 0x3D, 0x00,  0x7F, 0x10, 0x28, 0x44, 0x00 ;   6a j   6b k	; Nokia 5110
.db	0x00, 0x41, 0x7F, 0x40, 0x00,  0x7C, 0x04, 0x78, 0x04, 0x78 ;   6c l   6d m	; Nokia 5110
.db	0x7C, 0x08, 0x04, 0x04, 0x78,  0x38, 0x44, 0x44, 0x44, 0x38 ;   6e n   6f o	; Nokia 5110
.db	0xFC, 0x18, 0x24, 0x24, 0x18,  0x18, 0x24, 0x24, 0x18, 0xFC ;   70 p   71 q	; Nokia 5110
.db	0x7C, 0x08, 0x04, 0x04, 0x08,  0x48, 0x54, 0x54, 0x54, 0x24 ;   72 r   73 s	; Nokia 5110
.db	0x04, 0x04, 0x3F, 0x44, 0x24,  0x3C, 0x40, 0x40, 0x20, 0x7C ;   74 t   75 u	; Nokia 5110
.db	0x1C, 0x20, 0x40, 0x20, 0x1C,  0x3C, 0x40, 0x30, 0x40, 0x3C ;   76 v   77 w	; Nokia 5110
.db	0x44, 0x28, 0x10, 0x28, 0x44,  0x4C, 0x90, 0x90, 0x90, 0x7C ;   78 x   79 y	; Nokia 5110
.db	0x44, 0x64, 0x54, 0x4C, 0x44,  0x00, 0x08, 0x36, 0x41, 0x00 ;   7a z   7b {	; Nokia 5110
.db	0x00, 0x00, 0x77, 0x00, 0x00,  0x00, 0x41, 0x36, 0x08, 0x00 ;   7c |   7d }	; Nokia 5110
.db	0x02, 0x01, 0x02, 0x04, 0x02,  0x3C, 0x26, 0x23, 0x26, 0x3C ;   7e ?   7f ?	; Nokia 5110
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; Nokia 5110



;***************************************************************************
;  Byte2Hex ASCII  Convert a byte to two hexidecimal ASCII characters.
;
; send:		DataByte
; return:	{SRAM} ByteASChigh, ByteASClow
; uses:		temp; ZH:ZL (non-destructive)
;***************************************************************************
Byte2HexASCII:							; Byte2hex
	push	ZH						; Byte2hex
	push	ZL						; Byte2hex
	push	temp						; Byte2hex
								; Byte2hex
	lds	temp, DataByte					; Byte2hex
	swap	temp						; Byte2hex
	andi	temp, 0x0f					; Byte2hex
	ldi	ZH, high (B2Hexchars << 1)			; Byte2hex
	ldi	ZL, low  (B2Hexchars << 1)			; Byte2hex
	add	ZL, temp					; Byte2hex
	adc	ZH, ZeroReg					; Byte2hex
	lpm	temp, z						; Byte2hex
	sts	ByteASChigh, temp				; Byte2hex
								; Byte2hex
	lds	temp, DataByte					; Byte2hex
	andi	temp, 0x0f					; Byte2hex
	ldi	ZH, high (B2Hexchars << 1)			; Byte2hex
	ldi	ZL, low  (B2Hexchars << 1)			; Byte2hex
	add	ZL, temp					; Byte2hex
	adc	ZH, ZeroReg					; Byte2hex
	lpm	temp, z						; Byte2hex
	sts	ByteASClow, temp				; Byte2hex
								; Byte2hex
	pop	temp						; Byte2hex
	pop	ZL						; Byte2hex
	pop	ZH						; Byte2hex
	ret							; Byte2hex
								; Byte2hex
B2Hexchars: .db "0123456789ABCDEF"				; Byte2hex
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



;***************************************************************
; do ADC10      Mega8 analog-to-digital 10-bit conversion
;  >>  ADC clock set at 125K (8MHz/64) 8uS*13=104uS/ADC
; called by: 	Heartbeat_Period
; send:		The ADC channel in temp
; return:	ADConeValH:L  ADCtwoValH:L
; uses:  	temp
;					size: 25 instructions
;***************************************************************; do ADC10
doADC10:							; do ADC10
	push	temp						; do ADC10
;								; do ADC10
	ldi	temp, ADC_MUX1	; ADMUX:76 are 0 0 for AREF +5V	; do ADC10
	ori	temp, 0b00100000 ; set ADLAR for left adjust	; do ADC10
	out	ADMUX, temp  ; ADMUX:5 is ADLAR. If 0,right adj	; do ADC10
	ldi	temp, (1 << ADEN) | (1<<ADPS2) | (1<<ADPS1)  	; do ADC10
	out	ADCSRA, temp	;ADPS = 1 1 0    8MHz/64=125K	; do ADC10
	sbi	ADCSRA, ADSC	; start ADC, takes 104 uSec	; do ADC10
;								; do ADC10
ADC10_loop:	; ADSC returns low when ADC done		; do ADC10
	sbic	ADCSRA, ADSC 					; do ADC10
	rjmp	ADC10_loop					; do ADC10
;								; do ADC10
	in	temp, ADCL					; do ADC10
	sts	ADConeValLow, temp				; do ADC10
	in	temp, ADCH					; do ADC10
	sts	ADConeValHigh, temp				; do ADC10
;								; do ADC10
	pop	temp						; do ADC10
;								; do ADC10
	ret							; do ADC10
								; do ADC10
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;%%%%%%%%%




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



;****************************************************************
; LongDelay	counts T1 periods { 80 per second: 12.5mS
;		for long do-nothing loops.
;  used by Reset/initApplication for LCD message display timing.
;  send:	temp    - 8MHz =0.125uS period
;  if timer1 prescaler=1024, then each timer1 interval is 128uS.
;  uses: 	temp2 (n_d) = number of 12.5mS periods to wait
;****************************************************************
LongDelay:
	push	temp2

	lds	temp2, HeartBeatCntr
	add	temp2, temp
LDly_1:	lds	temp, HeartBeatCntr
	cpse	temp, temp2
	rjmp	LDly_1

	pop	temp2
	ret
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%






;*******************************************************************************
; WritePage -  Write a page from Page_Buffer in SRAM to the EEPROM
;
; After writing START:SLA+1:addrH:addrL:data bytes,
; STOP condition sent by AVR master. EE is polled for
; completion of write interval by START:SLA+0.
; Loops until ACK returned from EE.
;
; send:		HighAddress, LowAddress, SLA, Page_Buffer, Write_BlockSize
; return: 	FlagsReg:WPOK
; uses: 	temp, temp2, temp3, ReceivedTWSR
; calls: 	tw_wait
;
;  TWSR (two-wire status register) message codes returned:
;  after ANDing the returned byte with 0b1111.1000
; ..........................................................
; 0000.1---  ==  received START_sent_OK
; 0001.1---  ==  Wrote_SLA+w__received_ACK
; 0100.0---  ==  Wrote_SLA+r__received_ACK
; 0010.1---  ==  Wrote Data - received ACK
; 0001.0---  ==  Repeated START sent OK
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; TWI_WritePage
WritePage:								; TWI_WritePage
 	push	temp							; TWI_WritePage
 	push	temp2							; TWI_WritePage
 	push	temp3							; TWI_WritePage
 	push	ZL							; TWI_WritePage
 	push	ZH							; TWI_WritePage
									; TWI_WritePage
	cbr	FlagsReg, (1 << WPOK)					; TWI_WritePage
									; TWI_WritePage
	ldi	temp, (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)		; TWI_WritePage
	out	TWCR, temp	 					; TWI_WritePage
	rcall	tw_wait							; TWI_WritePage
									; TWI_WritePage
	ldi	temp, 0							; TWI_WritePage
	sts	TWIstepValue, temp					; TWI_WritePage
									; TWI_WritePage
	in	temp, TWSR						; TWI_WritePage
	andi	temp, 0xF8						; TWI_WritePage
	sts	ReceivedTWSR, temp					; TWI_WritePage
									; TWI_WritePage
	ldi	temp2, 0x08						; TWI_WritePage
	sts	ExpectedTWSR, temp2					; TWI_WritePage
	cpse	temp, temp2	 					; TWI_WritePage
	rjmp	WP_ERROR						; TWI_WritePage
;----------------------------------------				; TWI_WritePage
; Send control byte  device code bits7-4 {1010},			; TWI_WritePage
; dev addr: (bits 3,2,1 =0  0 0), and R/_w bit0 (0=write)		; TWI_WritePage
	lds	temp, SLA						; TWI_WritePage
	andi	temp, 0b11111110	 				; TWI_WritePage
	out	TWDR, temp						; TWI_WritePage
	ldi	temp, (1<<TWINT) | (1<<TWEN)				; TWI_WritePage
	out	TWCR, temp						; TWI_WritePage
	rcall	tw_wait							; TWI_WritePage
									; TWI_WritePage
	ldi	temp, 1							; TWI_WritePage
	sts	TWIstepValue, temp					; TWI_WritePage
									; TWI_WritePage
	in	temp, TWSR						; TWI_WritePage
	andi	temp, 0xF8						; TWI_WritePage
	sts	ReceivedTWSR, temp					; TWI_WritePage
									; TWI_WritePage
	ldi	temp2, 0x18						; TWI_WritePage
	sts	ExpectedTWSR, temp2					; TWI_WritePage
	cpse	temp, temp2						; TWI_WritePage
	rjmp	WP_ERROR						; TWI_WritePage
;--------------------------						; TWI_WritePage
	lds	temp, HighAddress					; TWI_WritePage
	out	TWDR, temp						; TWI_WritePage
	ldi	temp, (1<<TWINT) | (1<<TWEN)				; TWI_WritePage
	out	TWCR, temp						; TWI_WritePage
	rcall	tw_wait							; TWI_WritePage
									; TWI_WritePage
	ldi	temp, 2							; TWI_WritePage
	sts	TWIstepValue, temp					; TWI_WritePage
									; TWI_WritePage
	in	temp, TWSR						; TWI_WritePage
	andi	temp, 0xF8						; TWI_WritePage
	sts	ReceivedTWSR, temp					; TWI_WritePage
									; TWI_WritePage
	ldi	temp2, 0x28						; TWI_WritePage
	sts	ExpectedTWSR, temp2					; TWI_WritePage
	cpse	temp, temp2						; TWI_WritePage
	rjmp	WP_ERROR						; TWI_WritePage
;-----------------------------------					; TWI_WritePage
	lds	temp, LowAddress					; TWI_WritePage
	out	TWDR, temp						; TWI_WritePage
	ldi	temp, (1<<TWINT) | (1<<TWEN)				; TWI_WritePage
	out	TWCR, temp						; TWI_WritePage
	rcall	tw_wait							; TWI_WritePage
									; TWI_WritePage
	ldi	temp, 3							; TWI_WritePage
	sts	TWIstepValue, temp					; TWI_WritePage
									; TWI_WritePage
	in	temp, TWSR						; TWI_WritePage
	andi	temp, 0xF8						; TWI_WritePage
	sts	ReceivedTWSR, temp					; TWI_WritePage
									; TWI_WritePage
	ldi	temp2, 0x28						; TWI_WritePage
	sts	ExpectedTWSR, temp2					; TWI_WritePage
	cpse	temp, temp2						; TWI_WritePage
	rjmp	WP_ERROR						; TWI_WritePage
;-----------------------------------					; TWI_WritePage
	ldi	ZH, high (Page_Buffer)					; TWI_WritePage
	ldi	ZL, low  (Page_Buffer)					; TWI_WritePage
	lds	temp3, WriteBlockSize					; TWI_WritePage
									; TWI_WritePage
WIP_0:	ld	temp, z+						; TWI_WritePage
	out	TWDR, temp						; TWI_WritePage
	ldi	temp, (1<<TWINT) | (1<<TWEN)				; TWI_WritePage
	out	TWCR, temp						; TWI_WritePage
	rcall	tw_wait							; TWI_WritePage
									; TWI_WritePage
	lds	temp, TWIstepValue					; TWI_WritePage
	inc	temp							; TWI_WritePage
	sts	TWIstepValue, temp					; TWI_WritePage
									; TWI_WritePage
	in	temp, TWSR						; TWI_WritePage
	andi	temp, 0xF8						; TWI_WritePage
	sts	ReceivedTWSR, temp					; TWI_WritePage
									; TWI_WritePage
	ldi	temp2, 0x28						; TWI_WritePage
	sts	ExpectedTWSR, temp2					; TWI_WritePage
	cpse	temp, temp2						; TWI_WritePage
	rjmp	WP_ERROR						; TWI_WritePage
									; TWI_WritePage
	dec	temp3							; TWI_WritePage
	brne	WIP_0							; TWI_WritePage
;;-----------------------------						; TWI_WritePage
	sbr	FlagsReg, (1 << WPOK)					; TWI_WritePage
									; TWI_WritePage
;===================================================			; TWI_WritePage
; test for EE Page Write interval timeout goes here			; TWI_WritePage
									; TWI_WritePage
									; TWI_WritePage
; Repeat sending STOP: START: SLA+0 until EE returns ACK		; TWI_WritePage
;---------------------------------------------------------		; TWI_WritePage
WP_WriteInterval:							; TWI_WritePage
	ldi	temp, (1<<TWINT) | (1<<TWSTO) | (1<<TWEN)		; TWI_WritePage
	out	TWCR, temp						; TWI_WritePage
	; Transmit stop cond to start writing page to EEPROM		; TWI_WritePage
	; wait 25 milli seconds						; TWI_WritePage
									; TWI_WritePage
; Wait 10 milliseconds for page-write operation				; TWI_WritePage
	clr	temp							; TWI_WritePage
	ldi	temp2, 0x80						; TWI_WritePage
WPdl:	dec	temp  	; 256 * 3 = 768 clocks				; TWI_WritePage
	brne	WPdl							; TWI_WritePage
	dec	temp2	; 128 * 768 = @12.5 milliSeconds		; TWI_WritePage
	brne	WPdl							; TWI_WritePage
									; TWI_WritePage
;	ldi	temp, DELAY8USEC					; TWI_WritePage
;WIP_STOP_loop:								; TWI_WritePage
;	dec	temp							; TWI_WritePage
;	brne	WIP_STOP_loop						; TWI_WritePage
;									; TWI_WritePage
;;---------------------------------------------------------		; TWI_WritePage
;;  After sending STOP [ which begins the EEPROM 			; TWI_WritePage
;;  write period @5 milliseconds] and waiting about			; TWI_WritePage
;;  16 uSec, send a START command and test for TWI 			; TWI_WritePage
;;  compliance.  Then send SLA+w.  If NACK, then 			; TWI_WritePage
;;  EEPROM is still writing previous data to memory.			; TWI_WritePage
;;  In that case, send STOP and wait about 16 uSec.			; TWI_WritePage
;;  Keep sending SLA+w until EEPROM returns ACK. 			; TWI_WritePage
;;  Write period is finished.						; TWI_WritePage
;;									; TWI_WritePage
;	ldi	temp, (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)		; TWI_WritePage
;	out	TWCR, temp		; Transmit start cond		; TWI_WritePage
;	rcall	tw_wait							; TWI_WritePage
;									; TWI_WritePage
;	in	temp, TWSR						; TWI_WritePage
;	andi	temp, 0xF8						; TWI_WritePage
;	sts	ReceivedTWSR, temp					; TWI_WritePage
;									; TWI_WritePage
;	ldi	temp2, 0x08						; TWI_WritePage
;	sts	ExpectedTWSR, temp2					; TWI_WritePage
;	cpse	temp, temp2		 				; TWI_WritePage
;	rjmp	WP_ERROR						; TWI_WritePage
;;----------------------------------------------------------		; TWI_WritePage
;									; TWI_WritePage
;	lds	temp, SLA						; TWI_WritePage
;	andi	temp, 0b11111110	 				; TWI_WritePage
;	out	TWDR, temp						; TWI_WritePage
;	ldi	temp, (1<<TWINT) | (1<<TWEN)				; TWI_WritePage
;	out	TWCR, temp						; TWI_WritePage
;	rcall	tw_wait							; TWI_WritePage
;									; TWI_WritePage
;	in	temp, TWSR						; TWI_WritePage
;	andi	temp, 0xF8						; TWI_WritePage
;	sts	ReceivedTWSR, temp					; TWI_WritePage
;									; TWI_WritePage
;	ldi	temp2, 0x18						; TWI_WritePage
;	cpse	temp, temp2						; TWI_WritePage
; ; skip polling when EE returns ACK  (TWSR & 0xf8 == 0x18)		; TWI_WritePage
;	rjmp	WP_WriteInterval					; TWI_WritePage
;									; TWI_WritePage
;; write final STOP condition after 					; TWI_WritePage
;; Page Write interval @5mS is completed				; TWI_WritePage
;									; TWI_WritePage
;	ldi	temp, (1<<TWINT) | (1<<TWSTO) | (1<<TWEN)		; TWI_WritePage
;	out	TWCR, temp						; TWI_WritePage
;; Transmit stop cond to start writing page to EEPROM			; TWI_WritePage
;									; TWI_WritePage
;	ldi	temp, DELAY8USEC					; TWI_WritePage
;WIP_STOP_loop2:							; TWI_WritePage
;	dec	temp							; TWI_WritePage
;	brne	WIP_STOP_loop2						; TWI_WritePage
									; TWI_WritePage
;~~~~~~~~~~~~~~~~~~							; TWI_WritePage
WIP_exit:								; TWI_WritePage
	pop	ZH							; TWI_WritePage
	pop	ZL							; TWI_WritePage
	pop	temp3							; TWI_WritePage
	pop	temp2							; TWI_WritePage
	pop	temp							; TWI_WritePage
	ret	; single exit point for WritePage subroutine		; TWI_WritePage
									; TWI_WritePage
									; TWI_WritePage
;***********************************************************************; TWI_WritePage
WP_ERROR:								; TWI_WritePage
	rcall	doTWIerror						; TWI_WritePage
	rjmp	WIP_exit						; TWI_WritePage
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%; TWI_WritePage



;************************************************************************
; ReadPage					  			;
;						 			;
; Do START condition, slave address + write, 				;
;  high address, and low address, do a REPEAT_START.			;
; Send slave address + READ. Then read block 				;
; size of data bytes, and send a STOP condition.			;
; Data goes into Page_Buffer.  Full Address 				;
; range of serial EEPROM available.					;
;									;
; send:		[SRAM] HighAddress, LowAddress,				;
;		SLA, ReadBlockSize, Page_Buffer				;
; return:	Page_Buffer (32 bytes in SRAM)				;
; uses:		temp, counter						;
; calls: tw_wait							;
;									;
;  TWSR (two-wire status register) message 				;
;  codes returned by the TWI unit:					;
; ......................................................................;
; 0000.1---  ==  START_sent_OK						; TWI_ReadPage
; 0001.1---  ==  Wrote_SLA+w__received_ACK  				; TWI_ReadPage
; 0100.0---  ==  Wrote_SLA+r__received_ACK				; TWI_ReadPage
; 0010.1---  ==  Wrote_Data___received ACK				; TWI_ReadPage
; 0001.0---  ==  Repeated START sent OK					; TWI_ReadPage
; 0101.0---  ==  Read_Data OK __master sent ACK				; TWI_ReadPage
; 0101.1---  ==  Master read Data OK__master sent NACK			; TWI_ReadPage
;                to indicate that slave should release SDA		; TWI_ReadPage
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~; TWI_ReadPage
ReadPage:								; TWI_ReadPage
	push	temp							; TWI_ReadPage
	push	temp2							; TWI_ReadPage
	push	temp3							; TWI_ReadPage
	push	ZL							; TWI_ReadPage
	push	ZH							; TWI_ReadPage
									; TWI_ReadPage
	cbr	FlagsReg, (1 << RPOK)					; TWI_ReadPage
;Transmit start condition						; TWI_ReadPage
	ldi	temp, (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)		; TWI_ReadPage
	out	TWCR, temp						; TWI_ReadPage
	rcall	tw_wait							; TWI_ReadPage
									; TWI_ReadPage
	ldi	temp, 0							; TWI_ReadPage
	sts	TWIstepValue, temp					; TWI_ReadPage
									; TWI_ReadPage
	in	temp, TWSR						; TWI_ReadPage
	andi	temp, 0xF8						; TWI_ReadPage
	sts	ReceivedTWSR, temp					; TWI_ReadPage
; Read SReg. 0x08 received (START sent)					; TWI_ReadPage
	ldi	temp2, 0x08						; TWI_ReadPage
	sts	ExpectedTWSR, temp2					; TWI_ReadPage
	cpse	temp, temp2						; TWI_ReadPage
	rcall	RP_ERROR						; TWI_ReadPage
									; TWI_ReadPage
;--------------------------------------------				; TWI_ReadPage
; Send SLA+0  [device code, address, and R/W bit low]			; TWI_ReadPage
	lds	temp, SLA						; TWI_ReadPage
	andi	temp, 0b11111110					; TWI_ReadPage
	out	TWDR, temp						; TWI_ReadPage
	ldi	temp, (1<<TWINT) | (1<<TWEN)				; TWI_ReadPage
	out	TWCR, temp						; TWI_ReadPage
	rcall	tw_wait							; TWI_ReadPage
									; TWI_ReadPage
	ldi	temp, 1							; TWI_ReadPage
	sts	TWIstepValue, temp					; TWI_ReadPage
									; TWI_ReadPage
	in	temp, TWSR						; TWI_ReadPage
	andi	temp, 0xF8						; TWI_ReadPage
	sts	ReceivedTWSR, temp					; TWI_ReadPage
; 0001.1---  ==  Wrote_SLA+w__received_ACK  msg from TWI 		; TWI_ReadPage
	ldi	temp2, 0x18						; TWI_ReadPage
	sts	ExpectedTWSR, temp2					; TWI_ReadPage
	cpse	temp, temp2						; TWI_ReadPage
	rcall	RP_ERROR						; TWI_ReadPage
									; TWI_ReadPage
;--------------------------------------------				; TWI_ReadPage
; TWDR should only be written when TWINT is high. 			; TWI_ReadPage
; TW_wait routine checks for this.					; TWI_ReadPage
	lds	temp, HighAddress					; TWI_ReadPage
	out	TWDR, temp						; TWI_ReadPage
									; TWI_ReadPage
	ldi	temp, (1<<TWINT) | (1<<TWEN)				; TWI_ReadPage
	out	TWCR, temp						; TWI_ReadPage
	rcall	tw_wait							; TWI_ReadPage
									; TWI_ReadPage
	ldi	temp, 2							; TWI_ReadPage
	sts	TWIstepValue, temp					; TWI_ReadPage
									; TWI_ReadPage
	in	temp, TWSR						; TWI_ReadPage
	andi	temp, 0xF8						; TWI_ReadPage
	sts	ReceivedTWSR, temp					; TWI_ReadPage
;  0x28 == data sent, ACK received					; TWI_ReadPage
	ldi	temp2, 0x28						; TWI_ReadPage
	sts	ExpectedTWSR, temp2					; TWI_ReadPage
	cpse	temp, temp2						; TWI_ReadPage
	rcall	RP_ERROR						; TWI_ReadPage
;-----------------------------------					; TWI_ReadPage
	lds	temp, LowAddress					; TWI_ReadPage
	out	TWDR, temp						; TWI_ReadPage
									; TWI_ReadPage
	ldi	temp, (1<<TWINT) | (1<<TWEN)				; TWI_ReadPage
	out	TWCR, temp						; TWI_ReadPage
	rcall	tw_wait							; TWI_ReadPage
									; TWI_ReadPage
	ldi	temp, 3							; TWI_ReadPage
	sts	TWIstepValue, temp					; TWI_ReadPage
									; TWI_ReadPage
	in	temp, TWSR						; TWI_ReadPage
	andi	temp, 0xF8						; TWI_ReadPage
	sts	ReceivedTWSR, temp					; TWI_ReadPage
;  0x28 == data sent, ACK received 					; TWI_ReadPage
        ldi	temp2, 0x28						; TWI_ReadPage
	sts	ExpectedTWSR, temp2					; TWI_ReadPage
	cpse	temp, temp2						; TWI_ReadPage
	rcall	RP_ERROR						; TWI_ReadPage
;-----------------------------------					; TWI_ReadPage
;  This is a REPEATED START and it tells				; TWI_ReadPage
;  the slave that the write operation is 				; TWI_ReadPage
;  aborted while keeping the address that was sent.			; TWI_ReadPage
	ldi	temp, (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)		; TWI_ReadPage
	out	TWCR, temp						; TWI_ReadPage
	rcall	tw_wait							; TWI_ReadPage
									; TWI_ReadPage
	ldi	temp, 4							; TWI_ReadPage
	sts	TWIstepValue, temp					; TWI_ReadPage
									; TWI_ReadPage
	in	temp, TWSR						; TWI_ReadPage
	andi	temp, 0xF8						; TWI_ReadPage
	sts	ReceivedTWSR, temp					; TWI_ReadPage
									; TWI_ReadPage
	ldi	temp2, 0x10	;  Repeated_START sent			; TWI_ReadPage
	sts	ExpectedTWSR, temp2					; TWI_ReadPage
	cpse	temp, temp2						; TWI_ReadPage
	rcall	RP_ERROR						; TWI_ReadPage
;----------------------------------------------				; TWI_ReadPage
; Send SLA+1 control byte [device code, address, and R/W bit]		; TWI_ReadPage
	lds	temp, SLA						; TWI_ReadPage
	ori	temp, 0b00000001 ; r~/w (bit 0) set (read data)		; TWI_ReadPage
	out	TWDR, temp						; TWI_ReadPage
									; TWI_ReadPage
	ldi	temp, (1<<TWINT) | (1<<TWEN)				; TWI_ReadPage
	out	TWCR, temp						; TWI_ReadPage
	rcall	tw_wait							; TWI_ReadPage
									; TWI_ReadPage
	ldi	temp, 5							; TWI_ReadPage
	sts	TWIstepValue, temp					; TWI_ReadPage
									; TWI_ReadPage
	in	temp, TWSR						; TWI_ReadPage
	andi	temp, 0xF8						; TWI_ReadPage
	sts	ReceivedTWSR, temp					; TWI_ReadPage
									; TWI_ReadPage
	ldi	temp2, 0x40	; ACK received after SLA+R		; TWI_ReadPage
	sts	ExpectedTWSR, temp2					; TWI_ReadPage
	cpse	temp, temp2						; TWI_ReadPage
	rcall	RP_ERROR						; TWI_ReadPage
;-----------------------------------------------------------------------; TWI_ReadPage
; read ReadBlockSize-1 bytes from EEPROM and send ACK after each byte	; TWI_ReadPage
	ldi	ZH, high (Page_Buffer)					; TWI_ReadPage
	ldi	ZL, low  (Page_Buffer)					; TWI_ReadPage
	lds	counter, ReadBlockSize 					; TWI_ReadPage
	dec 	counter							; TWI_ReadPage
; Read Data with Enable ACK						; TWI_ReadPage
RIP_1:	ldi	temp, (1<<TWINT) | (1<<TWEN) | (1<<TWEA)		; TWI_ReadPage
	out	TWCR, temp						; TWI_ReadPage
	rcall	tw_wait							; TWI_ReadPage
; store the data byte read from the EEPROM				; TWI_ReadPage
	in	temp3, TWDR 						; TWI_ReadPage
									; TWI_ReadPage
	lds	temp, TWIstepValue					; TWI_ReadPage
	inc	temp							; TWI_ReadPage
	sts	TWIstepValue, temp					; TWI_ReadPage
									; TWI_ReadPage
	in	temp, TWSR						; TWI_ReadPage
	andi	temp, 0xF8						; TWI_ReadPage
	sts	ReceivedTWSR, temp					; TWI_ReadPage
; 0x50 Master Read data received w/ Acknowledge				; TWI_ReadPage
	ldi	temp2, 0x50     					; TWI_ReadPage
	sts	ExpectedTWSR, temp2					; TWI_ReadPage
	cpse	temp, temp2						; TWI_ReadPage
	rcall	RP_ERROR						; TWI_ReadPage
									; TWI_ReadPage
	st	z+, temp3						; TWI_ReadPage
	dec	counter							; TWI_ReadPage
; loop to read each byte in the EEPROM page				; TWI_ReadPage
	brne	RIP_1  							; TWI_ReadPage
;-----------------------------------------------------------------------; TWI_ReadPage
;  read last byte from EEPROM, 						; TWI_ReadPage
;  then send NACK after receiving data bits				; TWI_ReadPage
	ldi	temp, (1<<TWINT) | (1<<TWEN)  				; TWI_ReadPage
	out	TWCR, temp ; Read Data with NACK			; TWI_ReadPage
	rcall	TW_Wait							; TWI_ReadPage
									; TWI_ReadPage
	lds	temp, TWIstepValue					; TWI_ReadPage
	inc	temp							; TWI_ReadPage
	sts	TWIstepValue, temp					; TWI_ReadPage
									; TWI_ReadPage
	in	temp, TWSR						; TWI_ReadPage
	andi	temp, 0xF8						; TWI_ReadPage
	sts	ReceivedTWSR, temp					; TWI_ReadPage
; 0x58 Master Read data received w/ No Acknowledge			; TWI_ReadPage
	ldi	temp2, 0x58						; TWI_ReadPage
	sts	ExpectedTWSR, temp2					; TWI_ReadPage
	cpse	temp, temp2						; TWI_ReadPage
	rcall	RP_ERROR						; TWI_ReadPage
									; TWI_ReadPage
; store the data byte read from the EEPROM				; TWI_ReadPage
	in	temp3, TWDR						; TWI_ReadPage
	st	z+, temp3						; TWI_ReadPage
									; TWI_ReadPage
;-----------------------------------------------------------------------; TWI_ReadPage
	ldi	temp, (1<<TWINT) | (1<<TWSTO) | (1<<TWEN)		; TWI_ReadPage
	out	TWCR, temp						; TWI_ReadPage
	ldi	temp, Delay8uSec					; TWI_ReadPage
RIP_2:	dec	temp							; TWI_ReadPage
	brne	RIP_2 							; TWI_ReadPage
; wait 8-16 uSec after STOP before turning off TWI			; TWI_ReadPage
	ldi	temp, 0	; TWI unit disabled				; TWI_ReadPage
	out	TWCR, temp						; TWI_ReadPage
									; TWI_ReadPage
	sbr	FlagsReg, ( 1 << RPOK)					; TWI_ReadPage
									; TWI_ReadPage
; single exit point for ReadPage subroutine  				; TWI_ReadPage
RIP_exit:								; TWI_ReadPage
       	pop	ZH							; TWI_ReadPage
	pop	ZL							; TWI_ReadPage
	pop	temp3							; TWI_ReadPage
	pop	temp2							; TWI_ReadPage
	pop	temp							; TWI_ReadPage
	ret								; TWI_ReadPage
;									; TWI_ReadPage
;									; TWI_ReadPage
;***********************************************************************; TWI_ReadPage
; ERROR  not a subroutine itself, but an extension			; TWI_ReadPage
; of the 'ReadPage' or 'WritePage' subroutine				; TWI_ReadPage
									; TWI_ReadPage
;***********************************************************************; TWI_ReadPage
RP_ERROR:								; TWI_ReadPage
	rcall	doTWIerror						; TWI_ReadPage
	cbr	FlagsReg, ( 1 << RPOK)					; TWI_ReadPage
	rjmp	RIP_exit						; TWI_ReadPage
									; TWI_ReadPage
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%; TWI_ReadPage




;***************************************************************************************;** TWI_err
; TWIerror  - a subroutine extension of TWI Read_Page/Write_page routines.		;** TWI_err
;  This routine called when the returned status code is not the expected code.		;** TWI_err
;  This code is specific to the display available on the hardware of the application.	;** TWI_err
;  With LCD, display text and TWI error-number for 2 sec.  Then return and continue.	;** TWI_err
;											;** TWI_err
;  send: [SRAM] ExpectedTWSR, ReceivedTWSR						;** TWI_err
;  error routine size: 318 flash bytes							;** TWI_err
;***************************************************************************************;** TWI_err
doTWIerror:		; called by TWIerror,GRTCerror in Get_RTC, Set_RTC		;** TWI_err
	lds	temp, ReceivedTWSR							;** TWI_err
	sts	DataByte, temp								;** TWI_err
	rcall	Byte2HexASCII		; send DataByte, rtn ASCII in ByteASChigh:low	;** TWI_err
;---------------------------------------------------------------------------------------;** TWI_err
;  display err msg, hex digits of actual TWSR value, and expected TWSR value on LCD.	;** TWI_err
	ldi	temp, 0x05 								;** TWI_err
	sts	VarByteOne, temp							;** TWI_err
	rcall	LCD_clearline								;** TWI_err
;											;** TWI_err
	ldi	temp, 0	* PIXELSperCHAR							;** TWI_err
	sts	VarByteOne, temp							;** TWI_err
	ldi	temp, 5									;** TWI_err
	sts	VarByteTwo, temp							;** TWI_err
	rcall	LCD_gotoXY								;** TWI_err
; 											;** TWI_err
	lds	temp, ByteASChigh	;display Received TWSR				;** TWI_err
	sts	VarByteOne, temp							;** TWI_err
	rcall	LCD_character								;** TWI_err
	lds	temp, ByteASClow							;** TWI_err
	sts	VarByteOne, temp							;** TWI_err
	rcall	LCD_character								;** TWI_err
;											;** TWI_err
	lds	temp, ExpectedTWSR	;display Expected TWSR				;** TWI_err
	sts	DataByte, temp								;** TWI_err
	rcall	Byte2HexASCII								;** TWI_err
	lds	temp, ByteASChigh							;** TWI_err
	sts	VarByteOne, temp							;** TWI_err
	rcall	LCD_character								;** TWI_err
	lds	temp, ByteASClow							;** TWI_err
	sts	VarByteOne, temp							;** TWI_err
	rcall	LCD_character								;** TWI_err
											;** TWI_err
	lds	temp, TWIstepValue	;display TWIstep				;** TWI_err
	sts	DataByte, temp								;** TWI_err
	rcall	Byte2HexASCII								;** TWI_err
	lds	temp, ByteASChigh							;** TWI_err
	sts	VarByteOne, temp							;** TWI_err
	rcall	LCD_character								;** TWI_err
	lds	temp, ByteASClow							;** TWI_err
	sts	VarByteOne, temp							;** TWI_err
	rcall	LCD_character								;** TWI_err
											;** TWI_err
	lds	temp, ExpectedTWSR							;** TWI_err
	cpi	temp, 0x08	; code for START OK					;** TWI_err
	brne	TWIertx1  								;** TWI_err
	ldi	ZH, high (TWx_BdStr << 1)	; Bad START -  will trigger if 0x10	;** TWI_err
	ldi	ZL, low  (TWx_BdStr << 1)	; REPEAT-START error is returned.	;** TWI_err
	rjmp	RIPDsptx								;** TWI_err
TWIertx1:										;** TWI_err
	cpi	temp, 0x18	; code for SLA+W Tx'ed, ACK received			;** TWI_err
	brne	TWIertx2								;** TWI_err
	ldi	ZH, high (TWx_SLAWEr << 1)	; Slave addr write - no ACK from slave	;** TWI_err
	ldi	ZL, low  (TWx_SLAWEr << 1)						;** TWI_err
	rjmp	RIPDsptx								;** TWI_err
TWIertx2:										;** TWI_err
	cpi	temp, 0x28	; code for Data byte Tx'ed, ACK received		;** TWI_err
	brne	TWIertx3								;** TWI_err
	ldi	ZH, high (TWx_DtACEr << 1)	; Data write - no ACK from slave	;** TWI_err
	ldi	ZL, low  (TWx_DtACEr << 1)						;** TWI_err
	rjmp	RIPDsptx  								;** TWI_err
TWIertx3:										;** TWI_err
	cpi	temp, 0x10	; code for REPEAT_START OK				;** TWI_err
	brne	TWIertx4								;** TWI_err
	ldi	ZH, high (TWx_RpStEr << 1)	; REPEAT-START error			;** TWI_err
	ldi	ZL, low  (TWx_RpStEr << 1)						;** TWI_err
	rjmp	RIPDsptx  								;** TWI_err
TWIertx4:										;** TWI_err
	cpi	temp, 0x40	; [Master Receive mode]  SLA+R Tx'ed, ACK received	;** TWI_err
	brne	TWIertx5								;** TWI_err
	ldi	ZH, high (TWx_CnRdEr << 1)						;** TWI_err
	ldi	ZL, low  (TWx_CnRdEr << 1)						;** TWI_err
	rjmp	RIPDsptx								;** TWI_err
TWIertx5:										;** TWI_err
	cpi	temp, 0x50	; [Master Receive mode]  Data Tx'ed, ACK received	;** TWI_err
	brne	TWIertx6								;** TWI_err
	ldi	ZH, high (TWx_RdByEr << 1)						;** TWI_err
	ldi	ZL, low  (TWx_RdByEr << 1)						;** TWI_err
	rjmp	RIPDsptx								;** TWI_err
TWIertx6:										;** TWI_err
	cpi	temp, 0x58	; [Master Receive mode]  Data Tx'ed, NACK received	;** TWI_err
	brne	RIPDsptx								;** TWI_err
	ldi	ZH, high (TWx_RdNAEr << 1)						;** TWI_err
	ldi	ZL, low  (TWx_RdNAEr << 1)						;** TWI_err
	rjmp	RIPDsptx								;** TWI_err
;											;** TWI_err
RIPDsptx:										;** TWI_err
	ldi	temp, 6 * PIXELSperCHAR 						;** TWI_err
	sts	VarByteOne, temp							;** TWI_err
	ldi	temp, 5	; row #								;** TWI_err
	sts	VarByteTwo, temp							;** TWI_err
	rcall	LCD_gotoXY								;** TWI_err
											;** TWI_err
	lds	temp, TaskFlags								;** TWI_err
	andi	temp, (1 << SetGetFlag)							;** TWI_err
	tst	temp									;** TWI_err
	brne	RIPDsptx1								;** TWI_err
	ldi	temp, 'S'	; SetGetFlag is clear					;** TWI_err
	sts	VarbyteOne, temp							;** TWI_err
	rcall	LCD_character								;** TWI_err
	rjmp	RIPDsptx2								;** TWI_err
RIPDsptx1:										;** TWI_err
	ldi	temp, 'G'       ; SetGetFlag is set					;** TWI_err
	sts	VarbyteOne, temp							;** TWI_err
	rcall	LCD_character								;** TWI_err
	rjmp	RIPDsptx2								;** TWI_err
											;** TWI_err
RIPDsptx2:										;** TWI_err
	sts	PtrOneHigh, ZH								;** TWI_err
	sts	PtrOneLow, ZL								;** TWI_err
	rcall	LCD_stringP								;** TWI_err
;											;** TWI_err
;---------------------------------------------------------------			;** TWI_err
;											;** TWI_err
	lds	temp, TaskFlags								;** TWI_err
	sbr	temp, (1 << TWIer_flag)							;** TWI_err
	sts	TaskFlags, temp								;** TWI_err
											;** TWI_err
	ldi	temp, 12		; 0.75 second display of error message		;** TWI_err
	rcall	LongDelay								;** TWI_err
;											;** TWI_err
;  display err msg, hex digits of actual TWSR value, and expected TWSR value on LCD.	;** TWI_err
	ldi	temp, 0x05 		; Nokia 5110 bottom row				;** TWI_err
	sts	VarByteOne, temp							;** TWI_err
	rcall	LCD_clearline								;** TWI_err
;											;** TWI_err
	ret				; return to GRTCerror, SRTC_error		;** TWI_err
											;** TWI_err
;		     01234567								;** TWI_err
TWx_BdStr:	.db "BdStart", 0 	; not 0x08  SDA ast with SCL rel, didn't happen	;** TWI_err
TWx_SLAWEr:	.db "SLAWnoA", 0 	; not 0x18  SLA and write sent, no ACK received	;** TWI_err
TWx_DtACEr:	.db "DtWrNoA", 0 	; not 0x28  Data write, no ACK received		;** TWI_err
TWx_RpStEr:	.db "RptStEr", 0 	; not 0x10  Repeat START error			;** TWI_err
TWx_CnRdEr:	.db "SLARnoA", 0 	; not 0x40  SLA and read sent, no ACK received	;** TWI_err
TWx_RdByEr:	.db "DtRdnoA", 0 	; not 0x50  Data read, no ACK			;** TWI_err
TWx_RdNAEr:	.db "EndNoNA", 0 	; not 0x50  ???					;** TWI_err
											;** TWI_err
;***************************************************************************************;** TWI_err
; TWI_Wait   wait for TWINT flag in TWCR to set						;** TWI_err
; TWDR should only be written when TWINT is high.					;** TWI_err
;											;** TWI_err
; send: -  										;** TWI_err
; return: -										;** TWI_err
; uses and changes: temp								;** TWI_err
;***************************************************************************************;** TWI_err
TW_Wait:										;** TWI_err
	in	temp, TWCR								;** TWI_err
	sbrs	temp, TWINT	; TWINT set means TWI is ready for new command		;** TWI_err
	rjmp	TW_Wait									;** TWI_err
	ret 			; TWINT is high now.  TWSR has a new valid value.	;** TWI_err
;											;** TWI_err
;***** end of Real-Time Clock routine  *************************************************;** TWI_err
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%;** TWI_err







;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;
;
;   Data to be stored into the EEPROM is found here
;
;
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

;.org 0x600   ; 0xaa is an unused spacer byte needed to align font data inside EEPROM pages
StandardEEFont:  ; 96 chars, 8 bytes per char 768 bytes  5 columns by 7 rows  24 pages
.db  0x00, 0x00, 0x00, 0x00, 0x00,   0xaa, 0xaa, 0xaa ; space
.db  0x00, 0x00, 0x5F, 0x00, 0x00,   0xaa, 0xaa, 0xaa ; 21 !
.db  0x00, 0x07, 0x00, 0x07, 0x00,   0xaa, 0xaa, 0xaa ; 22 "
.db  0x14, 0x7F, 0x14, 0x7F, 0x14,   0xaa, 0xaa, 0xaa ; 23 #
.db  0x24, 0x2A, 0x7F, 0x2A, 0x12,   0xaa, 0xaa, 0xaa ; 24 $
.db  0x23, 0x13, 0x08, 0x64, 0x62,   0xaa, 0xaa, 0xaa ; 25 %
.db  0x36, 0x49, 0x56, 0x20, 0x50,   0xaa, 0xaa, 0xaa ; 26 &
.db  0x00, 0x08, 0x07, 0x03, 0x00,   0xaa, 0xaa, 0xaa ; 27 '
.db  0x00, 0x1C, 0x22, 0x41, 0x00,   0xaa, 0xaa, 0xaa ; 28 (
.db  0x00, 0x41, 0x22, 0x1C, 0x00,   0xaa, 0xaa, 0xaa ; 29 )
.db  0x2A, 0x1C, 0x7F, 0x1C, 0x2A,   0xaa, 0xaa, 0xaa ; 2a *
.db  0x08, 0x08, 0x3E, 0x08, 0x08,   0xaa, 0xaa, 0xaa ; 2b +
.db  0x00, 0x80, 0x70, 0x30, 0x00,   0xaa, 0xaa, 0xaa ; 2c ,
.db  0x08, 0x08, 0x08, 0x08, 0x08,   0xaa, 0xaa, 0xaa ; 2d -
.db  0x00, 0x00, 0x60, 0x60, 0x00,   0xaa, 0xaa, 0xaa ; 2e .
.db  0x20, 0x10, 0x08, 0x04, 0x02,   0xaa, 0xaa, 0xaa ; 2f ;
.db  0x3E, 0x51, 0x49, 0x45, 0x3E,   0xaa, 0xaa, 0xaa ; 30 0
.db  0x00, 0x42, 0x7F, 0x40, 0x00,   0xaa, 0xaa, 0xaa ; 31 1
.db  0x72, 0x49, 0x49, 0x49, 0x46,   0xaa, 0xaa, 0xaa ; 32 2
.db  0x21, 0x41, 0x49, 0x4D, 0x33,   0xaa, 0xaa, 0xaa ; 33 3
.db  0x18, 0x14, 0x12, 0x7F, 0x10,   0xaa, 0xaa, 0xaa ; 34 4
.db  0x27, 0x45, 0x45, 0x45, 0x39,   0xaa, 0xaa, 0xaa ; 35 5
.db  0x3C, 0x4A, 0x49, 0x49, 0x31,   0xaa, 0xaa, 0xaa ; 36 6
.db  0x41, 0x21, 0x11, 0x09, 0x07,   0xaa, 0xaa, 0xaa ; 37 7
.db  0x36, 0x49, 0x49, 0x49, 0x36,   0xaa, 0xaa, 0xaa ; 38 8
.db  0x46, 0x49, 0x49, 0x29, 0x1E,   0xaa, 0xaa, 0xaa ; 39 9
.db  0x00, 0x00, 0x14, 0x00, 0x00,   0xaa, 0xaa, 0xaa ; 3a :
.db  0x00, 0x40, 0x34, 0x00, 0x00,   0xaa, 0xaa, 0xaa ; 3b ;
.db  0x00, 0x08, 0x14, 0x22, 0x41,   0xaa, 0xaa, 0xaa ; 3c <
.db  0x14, 0x14, 0x14, 0x14, 0x14,   0xaa, 0xaa, 0xaa ; 3d =
.db  0x00, 0x41, 0x22, 0x14, 0x08,   0xaa, 0xaa, 0xaa ; 3e >
.db  0x02, 0x01, 0x59, 0x09, 0x06,   0xaa, 0xaa, 0xaa ; 3f ?
.db  0x3E, 0x41, 0x5D, 0x59, 0x4E,   0xaa, 0xaa, 0xaa ; 40 @
.db  0x7C, 0x12, 0x11, 0x12, 0x7C,   0xaa, 0xaa, 0xaa ; 41 A
.db  0x7F, 0x49, 0x49, 0x49, 0x36,   0xaa, 0xaa, 0xaa ; 42 B
.db  0x3E, 0x41, 0x41, 0x41, 0x22,   0xaa, 0xaa, 0xaa ; 43 C
.db  0x7F, 0x41, 0x41, 0x41, 0x3E,   0xaa, 0xaa, 0xaa ; 44 D
.db  0x7F, 0x49, 0x49, 0x49, 0x41,   0xaa, 0xaa, 0xaa ; 45 E
.db  0x7F, 0x09, 0x09, 0x09, 0x01,   0xaa, 0xaa, 0xaa ; 46 F
.db  0x3E, 0x41, 0x41, 0x51, 0x73,   0xaa, 0xaa, 0xaa ; 47 G
.db  0x7F, 0x08, 0x08, 0x08, 0x7F,   0xaa, 0xaa, 0xaa ; 48 H
.db  0x00, 0x41, 0x7F, 0x41, 0x00,   0xaa, 0xaa, 0xaa ; 49 I
.db  0x20, 0x40, 0x41, 0x3F, 0x01,   0xaa, 0xaa, 0xaa ; 4a J
.db  0x7F, 0x08, 0x14, 0x22, 0x41,   0xaa, 0xaa, 0xaa ; 4b K
.db  0x7F, 0x40, 0x40, 0x40, 0x40,   0xaa, 0xaa, 0xaa ; 4c L
.db  0x7F, 0x02, 0x1C, 0x02, 0x7F,   0xaa, 0xaa, 0xaa ; 4d M
.db  0x7F, 0x04, 0x08, 0x10, 0x7F,   0xaa, 0xaa, 0xaa ; 4e N
.db  0x3E, 0x41, 0x41, 0x41, 0x3E,   0xaa, 0xaa, 0xaa ; 4f O
.db  0x7F, 0x09, 0x09, 0x09, 0x06,   0xaa, 0xaa, 0xaa ; 50 P
.db  0x3E, 0x41, 0x51, 0x21, 0x5E,   0xaa, 0xaa, 0xaa ; 51 Q
.db  0x7F, 0x09, 0x19, 0x29, 0x46,   0xaa, 0xaa, 0xaa ; 52 R
.db  0x26, 0x49, 0x49, 0x49, 0x32,   0xaa, 0xaa, 0xaa ; 53 S
.db  0x03, 0x01, 0x7F, 0x01, 0x03,   0xaa, 0xaa, 0xaa ; 54 T
.db  0x3F, 0x40, 0x40, 0x40, 0x3F,   0xaa, 0xaa, 0xaa ; 55 U
.db  0x1F, 0x20, 0x40, 0x20, 0x1F,   0xaa, 0xaa, 0xaa ; 56 V
.db  0x3F, 0x40, 0x38, 0x40, 0x3F,   0xaa, 0xaa, 0xaa ; 57 W
.db  0x63, 0x14, 0x08, 0x14, 0x63,   0xaa, 0xaa, 0xaa ; 58 X
.db  0x03, 0x04, 0x78, 0x04, 0x03,   0xaa, 0xaa, 0xaa ; 59 Y
.db  0x61, 0x59, 0x49, 0x4D, 0x43,   0xaa, 0xaa, 0xaa ; 5a Z
.db  0x00, 0x7F, 0x41, 0x41, 0x41,   0xaa, 0xaa, 0xaa ; 5b [
.db  0x02, 0x04, 0x08, 0x10, 0x20,   0xaa, 0xaa, 0xaa ; 5c ¥
.db  0x00, 0x41, 0x41, 0x41, 0x7F,   0xaa, 0xaa, 0xaa ; 5d ]
.db  0x04, 0x02, 0x01, 0x02, 0x04,   0xaa, 0xaa, 0xaa ; 5e ^
.db  0x40, 0x40, 0x40, 0x40, 0x40,   0xaa, 0xaa, 0xaa ; 5f _
.db  0x00, 0x03, 0x07, 0x08, 0x00,   0xaa, 0xaa, 0xaa ; 60 `
.db  0x20, 0x54, 0x54, 0x78, 0x40,   0xaa, 0xaa, 0xaa ; 61 a
.db  0x7F, 0x28, 0x44, 0x44, 0x38,   0xaa, 0xaa, 0xaa ; 62 b
.db  0x38, 0x44, 0x44, 0x44, 0x28,   0xaa, 0xaa, 0xaa ; 63 c
.db  0x38, 0x44, 0x44, 0x28, 0x7F,   0xaa, 0xaa, 0xaa ; 64 d
.db  0x38, 0x54, 0x54, 0x54, 0x18,   0xaa, 0xaa, 0xaa ; 65 e
.db  0x00, 0x08, 0x7E, 0x09, 0x02,   0xaa, 0xaa, 0xaa ; 66 f
.db  0x18, 0xA4, 0xA4, 0x9C, 0x78,   0xaa, 0xaa, 0xaa ; 67 g
.db  0x7F, 0x08, 0x04, 0x04, 0x78,   0xaa, 0xaa, 0xaa ; 68 h
.db  0x00, 0x44, 0x7D, 0x40, 0x00,   0xaa, 0xaa, 0xaa ; 69 i
.db  0x20, 0x40, 0x40, 0x3D, 0x00,   0xaa, 0xaa, 0xaa ; 6a j
.db  0x7F, 0x10, 0x28, 0x44, 0x00,   0xaa, 0xaa, 0xaa ; 6b k
.db  0x00, 0x41, 0x7F, 0x40, 0x00,   0xaa, 0xaa, 0xaa ; 6c l
.db  0x7C, 0x04, 0x78, 0x04, 0x78,   0xaa, 0xaa, 0xaa ; 6d m
.db  0x7C, 0x08, 0x04, 0x04, 0x78,   0xaa, 0xaa, 0xaa ; 6e n
.db  0x38, 0x44, 0x44, 0x44, 0x38,   0xaa, 0xaa, 0xaa ; 6f o
.db  0xFC, 0x18, 0x24, 0x24, 0x18,   0xaa, 0xaa, 0xaa ; 70 p
.db  0x18, 0x24, 0x24, 0x18, 0xFC,   0xaa, 0xaa, 0xaa ; 71 q
.db  0x7C, 0x08, 0x04, 0x04, 0x08,   0xaa, 0xaa, 0xaa ; 72 r
.db  0x48, 0x54, 0x54, 0x54, 0x24,   0xaa, 0xaa, 0xaa ; 73 s
.db  0x04, 0x04, 0x3F, 0x44, 0x24,   0xaa, 0xaa, 0xaa ; 74 t
.db  0x3C, 0x40, 0x40, 0x20, 0x7C,   0xaa, 0xaa, 0xaa ; 75 u
.db  0x1C, 0x20, 0x40, 0x20, 0x1C,   0xaa, 0xaa, 0xaa ; 76 v
.db  0x3C, 0x40, 0x30, 0x40, 0x3C,   0xaa, 0xaa, 0xaa ; 77 w
.db  0x44, 0x28, 0x10, 0x28, 0x44,   0xaa, 0xaa, 0xaa ; 78 x
.db  0x4C, 0x90, 0x90, 0x90, 0x7C,   0xaa, 0xaa, 0xaa ; 79 y
.db  0x44, 0x64, 0x54, 0x4C, 0x44,   0xaa, 0xaa, 0xaa ; 7a z
.db  0x00, 0x08, 0x36, 0x41, 0x00,   0xaa, 0xaa, 0xaa ; 7b {
.db  0x00, 0x00, 0x77, 0x00, 0x00,   0xaa, 0xaa, 0xaa ; 7c |
.db  0x00, 0x41, 0x36, 0x08, 0x00,   0xaa, 0xaa, 0xaa ; 7d }
.db  0x02, 0x01, 0x02, 0x04, 0x02,   0xaa, 0xaa, 0xaa ; 7e ?
.db  0x3C, 0x26, 0x23, 0x26, 0x3C,   0xaa, 0xaa, 0xaa ; 7f ?
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;
;
;
BigDigitEEFont:	 ; 0xaa is an unused spacer byte needed to align font data inside EEPROM pages
; 16 column bytes [12 pixels per columns] * 3 rows * 12 characters__576 bytes__18 pages
;          0    1    2    3    4    5    6    7    8    9    a    b       c    d    e    f
.db	0xfc,0xfe,0xfe,0x0f,0x07,0x07,0x07,0x07,0xcf,0xfc,0xfc,0xf8,   0xaa,0xaa,0xaa,0xaa ;character '0'     ;          0 1 2 3 4 5 6 7 8 9 a b       
.db	0xff,0xff,0xff,0xf0,0x78,0x3c,0x1e,0x0f,0x07,0xff,0xff,0xff,   0xaa,0xaa,0xaa,0xaa                    ; row 0  0       * * * * * *             
.db	0x3f,0x7f,0x7f,0xf8,0xf0,0xf0,0xf0,0xf0,0xf8,0x7f,0x7f,0x1f,   0xaa,0xaa,0xaa,0xaa                    ;        1   * * * * * * * * * *         
                                                                                                              ;        2 * * * * * * * * * * *         
.db     0x00,0x00,0x40,0x60,0x70,0x78,0xfc,0xfe,0xff,0xff,0x00,0x00,   0xaa,0xaa,0xaa,0xaa ;character '1'     ;        3 * * * *         * * * *       
.db     0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,   0xaa,0xaa,0xaa,0xaa                    ;        4 * * *             * * *       
.db     0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,   0xaa,0xaa,0xaa,0xaa                    ;        5 * * *             * * *       
                                                                                                              ;        6 * * *             * * *       
.db     0x38,0x3c,0x3e,0x0f,0x07,0x07,0x07,0x0f,0x1f,0xff,0xfe,0xfc,   0xaa,0xaa,0xaa,0xaa ;character '2'     ;        7 * * *           * * * *       
.db     0xc0,0xe0,0xe0,0xf0,0x78,0x78,0x3c,0x1e,0x0f,0x0f,0x07,0x03,   0xaa,0xaa,0xaa,0xaa                    ; row 1  0 * * *         * * * * *       
.db     0xff,0xff,0xff,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,   0xaa,0xaa,0xaa,0xaa                    ;        1 * * *       * * * * * *       
                                                                                                              ;        2 * * *     * * * * * * *       
.db	0x38,0x3c,0x1e,0x0f,0x0f,0x0f,0x0f,0x0f,0x1e,0xfc,0xf8,0xf0,   0xaa,0xaa,0xaa,0xaa ;character '3'     ;        3 * * *   * * * *   * * *       
.db	0x00,0x00,0x00,0x1e,0x1e,0x1e,0x1e,0x3e,0xff,0xff,0xe7,0x80,   0xaa,0xaa,0xaa,0xaa                    ;        4 * * * * * * *     * * *       
.db	0x78,0xf8,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf8,0xff,0x7f,0x1f,   0xaa,0xaa,0xaa,0xaa                    ;        5 * * * * * *       * * *       
                                                                                                              ;        6 * * * * *         * * *       
.db	0x00,0x00,0x00,0x00,0x00,0xc0,0xf0,0xf8,0x7e,0xff,0xff,0xff,   0xaa,0xaa,0xaa,0xaa ;character '4'     ;        7 * * * *           * * *       
.db	0xe0,0xf0,0xf8,0xbe,0x9f,0x8f,0x83,0x81,0x80,0xff,0xff,0xff,   0xaa,0xaa,0xaa,0xaa                    ; row 2  0 * * *             * * *       
.db	0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xff,   0xaa,0xaa,0xaa,0xaa                    ;        1 * * *             * * *       
                                                                                                              ;        2 * * *             * * *       
.db     0xff,0xff,0xff,0x87,0x87,0x87,0x87,0x87,0x87,0x07,0x07,0x07,   0xaa,0xaa,0xaa,0xaa ;character '5'     ;        3 * * * *         * * * *       
.db     0x07,0x07,0x03,0x03,0x03,0x03,0x03,0x03,0x07,0xff,0xff,0xfc,   0xaa,0xaa,0xaa,0xaa                    ;        4 * * * * * * * * * * * *       
.db     0x3c,0x7c,0x78,0xf0,0xf0,0xf0,0xf0,0xf0,0x78,0x7f,0x3f,0x1f,   0xaa,0xaa,0xaa,0xaa                    ;        5 * * * * * * * * * * *         
                                                                                                              ;        6   * * * * * * * * * *         
.db	0xf8,0xfc,0xfe,0x0f,0x07,0x07,0x07,0x07,0x07,0x0f,0x1e,0x1c,   0xaa,0xaa,0xaa,0xaa ;character '6'     ;        7       * * * * * *             
.db	0xff,0xff,0xff,0x1e,0x0e,0x0e,0x0e,0x0e,0x1e,0xfe,0xfc,0xf8,   0xaa,0xaa,0xaa,0xaa                                                          
.db	0x3f,0x7f,0x7f,0xf0,0xe0,0xe0,0xe0,0xe0,0xf8,0x7f,0x3f,0x1f,   0xaa,0xaa,0xaa,0xaa

.db	0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0xff,0xff,0xff,0xff,   0xaa,0xaa,0xaa,0xaa ;character '7'
.db	0x00,0x00,0x00,0x00,0x80,0xe0,0xf8,0xfe,0x7f,0x1f,0x0f,0x03,   0xaa,0xaa,0xaa,0xaa
.db	0x00,0xc0,0xf0,0xfe,0xff,0x1f,0x07,0x01,0x00,0x00,0x00,0x00,   0xaa,0xaa,0xaa,0xaa

.db	0xf8,0xfe,0xff,0x07,0x07,0x07,0x07,0x07,0x8f,0xff,0xfe,0x78,   0xaa,0xaa,0xaa,0xaa ;character '8'
.db	0xe0,0xff,0xff,0x1f,0x0e,0x0e,0x0e,0x1f,0x3f,0xff,0xf9,0xe0,   0xaa,0xaa,0xaa,0xaa
.db	0x1f,0x7f,0x7f,0xf0,0xe0,0xe0,0xe0,0xf0,0x78,0x7f,0x3f,0x1f,   0xaa,0xaa,0xaa,0xaa

.db	0xfc,0xfe,0xff,0x0f,0x07,0x07,0x07,0x07,0x0f,0xff,0xfc,0xf8,   0xaa,0xaa,0xaa,0xaa ;character '9'
.db	0x1f,0x3f,0x3f,0x7c,0x78,0x78,0x78,0x78,0x3c,0xff,0xff,0xff,   0xaa,0xaa,0xaa,0xaa
.db	0x00,0x70,0xf0,0xf0,0xf0,0xf0,0x70,0x78,0x7c,0x3f,0x3f,0x0f,   0xaa,0xaa,0xaa,0xaa

.db	0x00,0x00,0x00,0x60,0xf0,0xf8,0xf8,0xf0,0x60,0x00,0x00,0x00,   0xaa,0xaa,0xaa,0xaa ;character ':'
.db	0x00,0x00,0x00,0x00,0x80,0xc1,0xc1,0x80,0x00,0x00,0x00,0x00,   0xaa,0xaa,0xaa,0xaa
.db	0x00,0x00,0x00,0x03,0x07,0x0f,0x0f,0x07,0x03,0x00,0x00,0x00,   0xaa,0xaa,0xaa,0xaa

.db     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   0xaa,0xaa,0xaa,0xaa ;character ' '
.db     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   0xaa,0xaa,0xaa,0xaa
.db     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   0xaa,0xaa,0xaa,0xaa

                                                                                                                   