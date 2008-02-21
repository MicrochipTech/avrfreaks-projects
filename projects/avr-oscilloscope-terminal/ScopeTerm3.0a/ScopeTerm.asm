; ScopeTerm.asm	-- Main source for Dutchtronix Oscilloscope Terminal
;
;  Copyright © 2008 Jan P.M. de Rie
;
;  All Rights Reserved
;
;  This file is part of the Dutchtronix Oscilloscope Clock Distribution.
;  Use, modification, and re-distribution is permitted subject to the
;  terms in the file named "LICENSE.TXT", which contains the full text
;  of the legal notices and should always accompany this Distribution.
;
;  This software is provided "AS IS" with NO WARRANTY OF ANY KIND.
;
;  This notice (including the copyright and warranty disclaimer)
;  must be included in all copies or derivations of this software.
;
; Terminal program for Dutchtronix AVR Oscilloscope Clock hardware
;
; General design
;
; Main:
;	Initialize
;	loop forever calling ClockWorks and checking for
;		input such as the Push Button switch
;		or the Serial port
;
; ClockWorks:
;	Refresh
;	Check various bits set by interrupt or other functions
;
; The button switch controls terminal mode or command mode.
; Available commands are T(ime) and B(aud).
;
; Register conventions
;
;	r8, r9 used as Grid Offset values (global)
;	r14 and r15 used as literals (global)
;	r25 used as a status register (global)
;	r11, r12 used as X and Y offsets in Refresh
;
;	functions usally preserve registers, except top level functions
;
; Current Ports (for Atmega168, see terminalconfig.h)
;
;	PB0..PB3, PC0..PC3, PD3..PD4 (Parallel DAC)
;	PC4..PC5 RTC I2C
;   PD5: Intensity control
;	PD6: LED
;	PD7: Push Button Switch
;
; This program will only update the EEProm area to save the current baudrate (location E2END-2) when changed
; and will only update the Real Time Clock if the user gives explicit instructions in the form of the 'T' command.
;
; First Release: V3.0 (to stay in sync with the AVR Oscilloscope Clock)
; Minor Release: V3.0a Set the I2C lines to input when not in use. Save changed BaudRate in EEProm
;
; Suggestions:
;				Auto shut-off display if no data received for 5 minutes or so
;				Screen saver if no data received (analog clock?)
;				Terminal Emulation
;
#define Mega32 0
#define Mega168 1

#define DEBUG 0

#define	ZINVERTED	1			;Tek 400 Family convention
#define	DOTSPERCHARPIXEL 2
#define	PIXELDELAY 0

#if Mega32
#include <m32def.inc>
#elif Mega168
#include <m168def.inc>
#endif
;
; Pin Configuration. The Dutchtronix AVR Oscilloscope Clock uses the
; Mega168 configuration.
;
#if Mega168
#define	LED_DDR DDRD
#define	LED_PORT PORTD
#define	LED_PIN PIND
#define	LED_BIT 6
#define	SW_DDR DDRD
#define	SW_PORT PORTD
#define	SW_PIN PIND
#define	SW_BIT 7
#define	PPS_IN PORTD,2
;
; 8 bit parallel data (7..0) is mapped to PB3..PB0 PC3..PC0
; DACA/DACB select is mapped to PD3 (low is A)
; WR (active low) is mapped to PD4
;
#define DACHIDATA PORTB
#define DACLODATA PORTC
#define DACSELECT	PORTD,3
#define DACWR PORTD,4

#define DACZX PORTD,5

;#define DACZX PORTD,5
#define	DACHIDDR  DDRB
#define	DACLODDR  DDRC
#define DACCTLDDR DDRD
;
.EQU	PORT_RTC  =  PORTC
.EQU	DDR_RTC   =  DDRC
.EQU	PIN_RTC   =  PINC
.EQU	BIT_SDA   =  4
.EQU	BIT_SCL   =  5

#elif Mega32
#define	LED_DDR DDRC
#define	LED_PORT PORTC
#define	LED_PIN PINC
#define	LED_BIT 0
#define	LED2_BIT 1
;
; Note that the code relies on the fact that PORTD, pin 3 is the INT1 interrupt
;
#define	SW_DDR DDRD
#define	SW_PORT PORTD
#define	SW_PIN PIND
#define	SW_BIT 3
#define	PPS_IN PORTD,2

;
; 8 bit parallel data (7..0) is mapped to PA7..PA0
; DACA/DACB select is mapped to PD3 (low is A)
; WR (active low) is mapped to PD4
;
#define DACDATA PORTA
#define DACSELECT	PORTB,3
#define DACWR PORTB,4
#define DACZX PORTB,5
#define	DACDATADDR  DDRA
#define DACCTLDDR DDRB
;
.EQU	PORT_RTC  =  PORTB
.EQU	DDR_RTC   =  DDRB
.EQU	PIN_RTC   =  PINB
.EQU	BIT_SDA   =  0
.EQU	BIT_SCL   =  1
#endif



#if ZINVERTED
#define BEAMON	cbi DACZX			;low implies no Z-axis control
#define BEAMOFF	sbi	DACZX			;high means decreased intensity
#define SKIPIFBEAMON sbic DACZX		;skip if low (beam on)
#else
#define BEAMON	sbi DACZX			;Hi implies no Z-axis control
#define BEAMOFF	cbi	DACZX			;Lo means decreased intensity
#define SKIPIFBEAMON sbis DACZX		;skip if Hi (beam on)
#endif

#define RTC_YEAR	8
#define RTC_MONTH	7
#define RTC_WEEKDAY 6
#define RTC_DAY		5
#define RTC_HOUR	4
#define RTC_MIN		3
#define RTC_SEC		2
#define	RTCCS2		1
#define RTCCLK		0x0D
#define RTCTCR		0x0E
#define	RTCCNT		0x0F


#include ".\avr.inc"

.undef	BL		;r18
.undef	BH		;r19
.undef	CL		;r20
.undef	CH		;r21
.undef	DL		;r22
.undef	DH		;r23
.undef	EL		;r24
.undef	EH		;r25
;
; Permanenty assigned registers
;
.def	_0			= r15	;Zero register
.def	_1			= r14	;One register
.def	G_FLAGS 	= r25	;Global Flags
.def	GXOffset	= r8
.def	GYOffset	= r9

;
; Scope Terminal Constants
;
.equ	NULL = 0
.equ	BS	=	8
.equ	CR	=	13
.equ	LF	=	10
.equ	ESCAPE = 0x0b	;make sure this ESCAPE value is never used in the vector tables
.equ	SPACE =	32
.equ	DOT =	46
.equ	UNDERSCORE = 95
.equ	HALFSPACE =	128
.equ	TAB	= 9

.equ	FLASHPERSEC = 		4
.equ	FLASHTICKS	=		100/FLASHPERSEC
.equ	BURNINCOUNTDOWN =	5	;move grid every 5 minutes to prevent burn-in
.equ	INP_TIMEOUT = 		30	;must be <60
.equ	TIMER0TIMEOUT = 	125	;a little over 1 second

;
; Timer1 computations
;
; 16 Mhz:		60 Hz: -1042 = oxfbee
; 20 Mhz:		60 Hz: -1302 = 0xfaea
;
#if Mega168
.equ	Timer0CountUp	= 127		;timer0 set for 6.5536 mSec
.equ	Timer2CountUp = 195			;timer2 set for 9.98 mSec
#elif Mega32
.equ	Timer0CountUp	= 102		;timer0 set for 6.528 mSec
.equ	Timer2CountUp = 156			;timer2 set for 9.98 mSec
#endif

#define MAXTXBUFFERLEN	128
#define MAXRXBUFFERLEN	128
#define RXBUFFERMASK	0b01111111	;2^7-1
;
; G_FLAGS bits
;
#define		fRcvdChar	0
#define		fTimeOut	1
#define		fUpdSecond	2
#define		fTC0		3
#define		fMIN		4
#define		fUP			5
#define		fFLASH		6
#define		fLFPending	7
;
; G2_FLAG bits (infrequent)
;
#define		fHR			1
#define		fDOWN		2

#define		DEFCHARWIDTH		12
#define		HALFSPACECHARWIDTH	4
#define		COLONCHARWIDTH		7
#define		NUMERICVPOS			0

#define		MAXLINESIZE		20
#define		LINEVSIZE		18
#define		SCREENLINES		13
#define		TOPLINEVPOS		(((SCREENLINES)*LINEVSIZE)+3)


;
; Scope Terminal SRam Variables
;
	.dseg
	.org	SRAM_START
;
; Pointer(s) to SRam Space based Text buffers.
; Not all are currently used.
;
.equ SCANSSTRTABLEN =   14
ScanTblStr0:		.byte	4		;terminal line
ScanTblStr1:		.byte	4		;terminal line
ScanTblStr2:		.byte	4		;terminal line
ScanTblStr3:		.byte	4		;terminal line
ScanTblStr4:		.byte	4		;terminal line
ScanTblStr5:		.byte	4		;terminal line
ScanTblStr6:		.byte	4		;terminal line
ScanTblStr7:		.byte	4		;terminal line
ScanTblStr8:		.byte	4		;terminal line
ScanTblStr9:		.byte	4		;terminal line
ScanTblStr10:		.byte	4		;terminal line
ScanTblStr11:		.byte	4		;terminal line
ScanTblStr12:		.byte	4		;terminal line
ScanTblStrClk:		.byte	4		;Numeric Time Display
;
; Flash Control Table
;
; Each entry describes a ScanTbl entry that needs to flash/blink:
;
;	ScanTbl ptr, VectorTbl ptr, On/Off status, FlashCnt, Original VectorTbl ptr, Alternate VectorTbl ptr
;
; Original VectorTbl ptr table entry is to deal with errors
;
#define MaxFlashTblEntries 6
#define FlashTblEntrySize 10
FlashTbl:			.byte	FlashTblEntrySize * (MaxFlashTblEntries+1)

.equ	DISPBUFLEN =	12
NumDispBuf:			.byte	DISPBUFLEN	;Numeric display text

TXBuffer:			.byte	MAXTXBUFFERLEN
RXBuffer:			.byte	MAXRXBUFFERLEN
;
; Terminal Screen Data Storage
; add one positon for NULL terminator plus one for Debug Marker
;
Line0:				.byte	(MAXLINESIZE+2)
Line1:				.byte	(MAXLINESIZE+2)
Line2:				.byte	(MAXLINESIZE+2)
Line3:				.byte	(MAXLINESIZE+2)
Line4:				.byte	(MAXLINESIZE+2)
Line5:				.byte	(MAXLINESIZE+2)
Line6:				.byte	(MAXLINESIZE+2)
Line7:				.byte	(MAXLINESIZE+2)
Line8:				.byte	(MAXLINESIZE+2)
Line9:				.byte	(MAXLINESIZE+2)
Line10:				.byte	(MAXLINESIZE+2)
Line11:				.byte	(MAXLINESIZE+2)
Line12:				.byte	(MAXLINESIZE+2)
Line13:				.byte	(MAXLINESIZE+2)

FlashBuffer:		.byte	(MAXLINESIZE+2)

CurLine:			.byte	1
CurPosition:		.byte	1
InputMode:			.byte	1			;terminal or commands
BaudRate:			.byte	1
AvrSecs:			.byte	1
AvrMins:			.byte	1
AvrHrs:				.byte	1
FlashCount:			.byte	1			;countdown for flash fields
RotationIndex:		.byte	1			;current index in Grid Offset Rotation. 0xff means disabled.
RotationCountdown:	.byte	1			;Countdown for RotateGrid
UART_Ready:			.byte	1
pUART_Buffer:		.byte	2
TXBufferIndex:		.byte	1
RXBufferHead:		.byte	1
RXBufferTail:		.byte	1
Timer0Counter:		.byte	2				;TODO 1 byte now
Timer1Counter:		.byte	2
G2_FLAGS:			.byte	1
BtnPosition:		.byte	1
KeyState:			.byte	1
TimeTicks:			.byte	1
CTextFlashPtr:		.byte	2
#if Mega32
LedStatus:			.byte	1
#endif
fCRPending:			.byte	1
#if PIXELDELAY
CharCnt:			.byte	1
PixelDelayCnt:		.byte	1
#endif

;----------------------------------------------------------;
; Program code area

#if Mega32 | Mega168
#define	JMP jmp
#else
#define JMP rjmp
#endif

	.cseg
	JMP		Main				;0 Reset


#if DEBUG

#if Mega32
	JMP		INT0Handler			;1 External INT0
	JMP		INT1Handler			;2 External INT1
	JMP		BadVector3			;3 External INT2
	JMP		TC2MatchHandler		;4 Timer/Counter2 Compare Match
	JMP		BadVector5			;5 Timer/Counter2 Overflow
	JMP		BadVector6			;6 Timer/Counter1 Capture Event
	JMP		BadVector7			;7 Timer/Counter1 Compare Match A
	JMP		BadVector8			;8 Timer/Counter1 Compare Match B
	JMP		BadVector9			;9 TC1 overflow
	JMP		TC0MatchHandler		;10 TC0 compare Match
	JMP		BadVector11			;11 TC0 overflow
	JMP		BadVector12			;12 SPI Serial Transfer Complete
	JMP		SIG_USART_RECV		;13 USART Rx complete
	JMP		BadVector14			;14 USART Data Register empty
	JMP		SIG_USART_TRANS		;15 USART Tx complete
	JMP		BadVector16			;16 ADC conversion complete
	JMP		BadVector17			;17 EEProm Ready
	JMP		BadVector18			;18 Analog Comparator
	JMP		BadVector19			;19 TWI
	JMP		BadVector20			;20 SPM Ready
#elif Mega168
	JMP		INT0Handler			;1 External INT0
	JMP		BadVector2			;2 External INT1
	JMP		BadVector3			;3 Pin Change Interrupt Request 0
	JMP		BadVector4			;4 Pin Change Interrupt Request 1
	JMP		PinChangeInterrupt	;5 Pin Change Interrupt Request 2
	JMP		BadVector6			;6 Watchdog Time-out Interrupt
	JMP		TC2MatchHandler		;7 Timer/Counter2 Compare Match A
	JMP		BadVector8			;8 Timer/Counter2 Compare Match A ??? probably B
	JMP		BadVector9			;9 Timer/Counter2 Overflow
	JMP		BadVector10			;10 TC1 capture
	JMP		BadVector11			;11 TC1 compare Match A
	JMP		BadVector12			;12 TC1 compare Match B
	JMP		BadVector13			;13 TC1 overflow
	JMP		TC0MatchHandler		;14 TC0 compare Match A
	JMP		BadVector15			;15 TC0 compare Match B
	JMP		BadVector16			;16 TC0 overflow
	JMP		BadVector17			;17 SPI Serial Transfer Complete
	JMP		SIG_USART_RECV		;18 USART Rx ready
	JMP		BadVector19			;19 USART Tx sfr empty
	JMP		SIG_USART_TRANS		;20 USART Tx ready
	JMP		BadVector21			;21 ADC conversion complete
	JMP		BadVector22			;22 EEProm Ready
	JMP		BadVector23			;23 Analog Comparator
	JMP		BadVector24			;24 TWI
	JMP		BadVector25			;25 SPM Ready
#endif
	

BadVector1:
	rjmp	BadVector1
BadVector2:
	rjmp	BadVector2
BadVector3:
	rjmp	BadVector3
BadVector4:
	rjmp	BadVector4
BadVector5:
	rjmp	BadVector5
BadVector6:
	rjmp	BadVector6
BadVector7:
	rjmp	BadVector7
BadVector8:
	rjmp	BadVector8
BadVector9:
	rjmp	BadVector9
BadVector10:
	rjmp	BadVector10
BadVector11:
	rjmp	BadVector11
BadVector12:
	rjmp	BadVector12
BadVector13:
	rjmp	BadVector13
BadVector14:
	rjmp	BadVector14
BadVector15:
	rjmp	BadVector15
BadVector16:
	rjmp	BadVector16
BadVector17:
	rjmp	BadVector17
BadVector18:
	rjmp	BadVector18
BadVector19:
	rjmp	BadVector19
BadVector20:
	rjmp	BadVector20
BadVector21:
	rjmp	BadVector21
BadVector22:
	rjmp	BadVector22
BadVector23:
	rjmp	BadVector23
BadVector24:
	rjmp	BadVector24
BadVector25:
	rjmp	BadVector25
#else
	.org	INT0addr
	JMP		INT0Handler			;1 External INT0

#if Mega32
	.org	INT1addr
	JMP		INT1handler			;2 External INT1
#endif

#if Mega168
	.org	PCI2addr
	JMP		PinChangeInterrupt	;5 Pin Change Interrupt Request 2
#endif

#if Mega32
	.org	OC2addr				;TC2 Match
#elif Mega168
	.org	OC2Aaddr			;TC2 Match A
#endif
	JMP		TC2MatchHandler

#if Mega32
	.org	OC0addr				;TC0 Match
#elif Mega168
	.org	OC0Aaddr			;TC0 Match A
#endif
	JMP		TC0MatchHandler		;16 TC0 Match

	.org	URXCaddr			;18 Serial In Received
	JMP		SIG_USART_RECV

	.org	UTXCaddr			;20 Transmit Complete
	JMP		SIG_USART_TRANS

#endif		; DEBUG

#include "./Term-font8x5.asm"

ASC_TAB:	.db	"0123456789ABCDEF"
;
; map 24-hour hours to AM/PM hours
;
AMPMMapping:		.db	12,1,2,3,4,5,6,7,8,9,10,11,12,1,2,3,4,5,6,7,8,9,10,11
RotationTable:		.db	0,0, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1, 0,-1	;+1,-1


#include "./Term-rtc.asm"
#include "./Term-uart.asm"
#include "./Term-Flashing.asm"
;
; Initialize INT0
;
InitINT0:
;
; configure the INT0 pin (PPS input) with a pull-up resistor
; to prevent random interrupts if unconnected
;
	sbi		PPS_IN				;enable pull-up
	call	InitRTCInt			;setup for Interrupt
#if Mega32
	in		r16,MCUCR
	ori		r16,((1<<ISC01)|(0 <<ISC00)) ;set ISC00 and ISC01 to INT0 on falling edge
	out		MCUCR,r16
#elif Mega168
	ldi		r16,((1<<ISC01)|(0 <<ISC00)) ;set ISC00 and ISC01 to INT0 on falling edge
	sts		EICRA,r16
#endif
	rcall	EnableINT0
	ret

EnableINT0:
#if Mega32
	in		r16,GICR
	ori		r16,(1<<INT0)		;enable INT0
	out		GICR,r16
#elif Mega168
	sbi		EIMSK,INT0			;enable INT0
#endif
	ret

DisableINT0:
#if Mega32
	in		r16,GICR
	andi	r16,~(1<<INT0)		;disable INT0
	out		GICR,r16
#elif Mega168
	cbi		EIMSK,INT0			;disable INT0
#endif
	ret
;
; Handler for External Interrupt 0
;
; Triggered by RTC (or external source) once a second (1PPM)
; Sets flag to be acted upon in ClockWorks
;
INT0Handler:
	push	r16
	in		r16,SREG
	push	r16
	sbr		G_FLAGS,(1<<fUpdSecond)	;Tell ClockWorks we need to advance a second
INT0HandlerOut:
	pop		r16
	out		SREG,r16
	pop		r16
	reti
;
; TC0 is used to program the push button
;
; It tracks the duration of a down push and it does key debouncing
;
; Initialize TC0
;
InitTC0:
	push	r16
#if Mega168
; Set prescaler to clkio/1024. At 20 Mhz, this is 51.2 uSec
; Using a Timer0CountUp value of 127 yields an match every 6.502 mSec
	out		TCCR0A,_0
	outi	TCCR0B,(1<<CS02)|(0<<CS01)|(1<<CS00)
	outi	OCR0A,Timer0CountUp
#elif Mega32
; Set prescaler to clkio/1024. At 16 Mhz, this is 64 uSec
; Using a Timer0CountUp value of 102 yields an match every 6.528 mSec
	outi	TCCR0,(1<<CS02)|(0<<CS01)|(1<<CS00)
	outi	OCR0,Timer0CountUp
#endif
	pop		r16
	ret
;
; Only called from the TC0 interrupt handler itself
;
StartTC0Countdown:
	ldi		r16,TIMER0TIMEOUT
	sts		Timer0Counter,r16
	ret
;
; Enable TC0 timer for key debouncing
;
; USES r16
;
EnableTC0:
	sts		Timer0Counter,_0
#if Mega168
	outi	TIFR0,(1<<OCF0A)	;clear pending match
	stsi	TIMSK0,(1<<OCIE0A)	;Enable TC0.match
#elif Mega32
	outi	TIFR,(1<<OCF0)		;clear pending match
	outi	OCR0,Timer0CountUp
	in		r16,TIMSK
	ori		r16,(1<<OCIE0)		;Enable TC0.match
	out		TIMSK,r16
	rcall	TurnOnLed2			;for key debugging
#endif
	ret
;
; Disable TC0 timer for key debouncing
;
; USES r16
;
DisableTC0:
#if Mega168
	stsi	TIMSK0,(0<<OCIE0A)	;Disable TC0.match
#elif Mega32
	in		r16,TIMSK
	andi	r16,~(1<<OCIE0)		;Disable TC0.match
	out		TIMSK,r16
	rcall	TurnOffLed2			;for key debugging
#endif
	ret
;
; Timer0 Match Handler.
;
TC0MatchHandler:
	push	r16
	out		TCNT0,_0
	sei
	in		r16,SREG
	push	r16
	push	r17
;
; Decrement Timer0Counter iff != 0
;
	lds		r16,Timer0Counter
	tst		r16
	breq	TC0L100				;skip test
	dec		r16					;subtract 1
	sts		Timer0Counter,r16	;update value
	brne	TC0L100				;not zero yet
;
; the 8-bit Timer0Counter field is now 0
;
	sbr		G_FLAGS,(1<<fTC0)	;mark TC0 timeout in flags
	sts		BtnPosition,_0		;Pretend Button is up (ignore next fUP)
TC0L100:
;
; bool_t DebounceSwitch2() Jack Ganssle embedded.com
;    static uint16_t State = 0; ; Current debounce status
;    State=(State<<1) | !RawKeyPressed() | 0xe000;
;    if(State==0xf000)return TRUE;
;    return FALSE; 
; 
	lds		r16,KeyState
	lsl		r16				;shift left
	mov		r17,_1			;preset to UP	SPEED????
	sbis	SW_PIN,SW_BIT	;skip if HIGH (meaning UP)
	clr		r17				;set to DOWN
	or		r16,r17
	sts		KeyState,r16
	cpi		r16,0b10000000	;7 downs in a row
	brne	TC0L200
	lds		r16,BtnPosition	;Is Button already DOWN (1)
	cp		r16,_1
	breq	TC0L300			;yes, ignore this event
	lds		r16,G2_FLAGS
	sbr		r16,(1<<fDOWN)	;set DOWN in flags
	sts		G2_FLAGS,r16
	sts		BtnPosition,_1	;Button is down
	rcall	StartTC0Countdown
	rjmp	TC0L300
TC0L200:
	cpi		r16,0b01111111	;7 ups in a row
	breq	TC0L210			;yes, process
	cpi		r16,0b11111111	;button up for a while?
	brne	TC0L300
	rcall	StopKeyDebouncing ;stop debouncing
	rjmp	TC0L300			;out
TC0L210:
	lds		r16,BtnPosition	;Is Button already UP (0)
	tst		r16
	breq	TC0L300			;yes, ignore this event
	sbr		G_FLAGS,(1<<fUP);set UP in flags
	sts		BtnPosition,_0	;Button is up
	lds		r16,G2_FLAGS
	cbr		r16,(1<<fDOWN)	;clear DOWN in flags
	sts		G2_FLAGS,r16
TC0L300:
	pop		r17
	pop		r16
	out		SREG,r16
	pop		r16
	reti

StopKeyDebouncing:
	rcall	DisableTC0			;stop key debouncing timer
#if Mega168
	rcall	PCI2Enable			;turn on after key debouncing
#elif Mega32
	rcall	EnableINT1			;turn on after key debouncing
#endif
	ret

#if Mega168
;
; Initialize PCI2 interrupt from pin PCINT23
;
; USES r16
;
PCI2Enable:
	ldi		r16,(1<<PCINT23)	;enable pin23 change interrupt
	sts		PCMSK2,r16
	ldi		r16,(1<<PCIE2)
	sts		PCICR,r16
	ret
;
; Disable PCI2 interrupt from pin PCINT23
;
; USES r16
;
PCI2Disable:
	ldi		r16,(0<<PCINT23)	;disable pin23 change interrupt
	sts		PCMSK2,r16
	ldi		r16,(0<<PCIE2)
	sts		PCICR,r16
	ret
;
; Pin change handler 2.
; Used for push button pin PCINT23
;
; This interrupt appears to be triggered even when no pin change has
; occured. The TC0 based key debouncing will take care of such spurious
; interrupts.
;
; Key debouncing immediately turned off after fUP message. This means that a spurious
; pin change (without a resulting fUP message eventually) will keep the KeyDebouncing running.
; Same for partial keypresses that only result in an fUP message. Since this is not a change,
; the message is never sent and debouncing continues.
; Fixed: turn off debouncing in TC0MatchHandler if key has been up for a while
;
PinChangeInterrupt:
	push	r16
	in		r16,SREG
	push	r16
	rcall	PCI2Disable			;turn off while debouncing
	rcall	EnableTC0			;start key debouncing timer
	pop		r16
	out		SREG,r16
	pop		r16
	reti
#endif

#if Mega32
;
; Initialize INT1
;
InitINT1:
;
; configure the INT1 pin (PORTD,3) with a pull-up resistor
; to prevent random interrupts if unconnected
;
	ldi		r16,((1<<ISC11)|(0 <<ISC10)) ;set ISC10 and ISC11 to INT1 on falling edge
	out		MCUCR,r16
	rcall	EnableINT1
	ret
;
; USES r16
;
EnableINT1:
	in		r16,GICR
	ori		r16,(1<<INT1)		;enable INT1
	out		GICR,r16
	ret
;
; USES r16
;
DisableINT1:
	in		r16,GICR
	andi	r16,~(1<<INT1)		;disable INT1
	out		GICR,r16
	ret
;
; Handler for External Interrupt 1
;
; Triggered by tactile switch on Mega32
;
INT1Handler:
	push	r16
	in		r16,SREG
	push	r16
	rcall	DisableINT1			;turn off while debouncing
	rcall	EnableTC0			;start key debouncing timer
	pop		r16
	out		SREG,r16
	pop		r16
	reti
#endif
;
; TC2 is used to run a 100Hz clock
;
; Initialize and Enable TC2
#if Mega168
; Set prescaler to clkio/1024. At 20 Mhz, this is 51.2 uSec
; Using a Timer0CountDown value of 195 yields an overflow every 9.98 mSec (about 100 Hz)
#elif Mega32
; Set prescaler to clkio/1024. At 16 Mhz, this is 64 uSec
; Using a Timer0CountDown value of 156 yields an overflow every 9.98 mSec (about 100 Hz)
#endif
;
InitTC2:
	push	r16
#if Mega168
	sts		TCCR2A,_0
	stsi	TCCR2B,(1<<CS22)|(1<<CS21)|(1<<CS20)
	stsi	OCR2A,Timer2CountUp
#elif Mega32
	outi	TCCR2,(1<<CS22)|(1<<CS21)|(1<<CS20)
	outi	OCR2,Timer2CountUp
#endif
	rcall	EnableTC2
	pop		r16
	ret

EnableTC2:
#if Mega168
	outi	TIFR2,(1<<OCF2A)	;clear pending match
	stsi	TIMSK2,(1<<OCIE2A)	;Enable OC2.ov
#elif Mega32
	outi	TIFR,(1<<OCF2)		;clear pending match
	in		r16,TIMSK
	ori		r16,(1<<OCIE2)		;Enable OC2.ov
	out		TIMSK,r16
#endif
	ret

DisableTC2:
#if Mega168
	stsi	TIMSK2,(0<<OCIE2A)	;Disable TC2.ov
#elif Mega32
	in		r16,TIMSK
	andi	r16,~(1<<OCIE2)	;Disable OC2.ov
	out		TIMSK,r16
#endif
	ret
;
; TC2 is an 8-bit counter
;
TC2MatchHandler:
	push	r16
	in		r16,SREG
	push	r16
#if Mega32
	out		TCNT2,_0
#elif Mega168
	sts		TCNT2,_0
#endif
	lds		r16,TimeTicks
	tst		r16
	breq	TC2L100
	dec		r16
	sts		TimeTicks,r16
	brne	TC2L100
	sbr		G_FLAGS,(1<<fTimeOut)
TC2L100:
;
; Countdown for flashing fields
;
	lds		r16,FlashCount
	dec		r16
	brne	TC2L150
	sbr		G_FLAGS,(1<<fFLASH)		;set Flash-Bit
	ldi		r16,FLASHTICKS			;reload
TC2L150:
	sts		FlashCount,r16
	pop		r16
	out		SREG,r16
	pop		r16
	reti
;
; Disable Watchdog Timer Completely
;
#if Mega168
WDTimerOff:
	wdr
	in		r16,MCUSR
	andi	r16,~(1<<WDRF)
	out		MCUSR,r16
	lds		r16,WDTCSR
	ori		r16,(1<<WDCE)|(1<<WDE)
	sts		WDTCSR,r16
	clr		r16
	sts		WDTCSR,r16
	ret
#elif Mega32
WDTimerOff:
	wdr
	in		r16,MCUSR
	andi	r16,~(1<<WDRF)
	out		MCUSR,r16
	in		r16,WDTCR
	ori		r16,(1<<WDTOE)|(1<<WDE)
	out		WDTCR,r16
	clr		r16
	out		WDTCR,r16
	ret
#endif
;
Main:
#if Mega168
	rcall	WDTimerOff				;if we got here from the bootloader
;
; SP set by hardware
;
#else
	outi	SPL,low(RAMEND)			;initialize SP
	outi	SPH,high(RAMEND)
#endif

#if DEBUG
	ser		r18
#else
	clr		r18
#endif
	ldiw	Z,SRAM_START			;Preset RAM
	ldi		r17,SRAM_SIZE/256		;256 Bytes blocks
MainL1:
	clr		r16						;start at 256
MainL5:
	st		Z+,r18
	dec		r16
	brne	MainL5
	dec		r17
	brne	MainL1

;
; Initialize Permanently Assigned Registers
;
	clr		_0						;permanently assigned 0
	clr		_1						;permanently assigned 1
	inc		_1
	clr		G_FLAGS
	clr		GXOffset
	clr		GYOffset

	ser		r16
	sts		KeyState,r16			;this prevents an initial UP

#if DEBUG
;
; These are regular initializations but unneeded in RELEASE mode
; since all memory is set to 0 at startup.
;
	sts		G2_FLAGS,_0
	sts		BtnPosition,_0			;button UP
	sts		RotationIndex,_0		;preset RotationIndex
	clrsw	CTextFlashPtr
	sts		CurLine,_0
	sts		InputMode,_0
	sts		BaudRate,_0
	sts		TimeTicks,_0
	sts		fCRPending,_0
#if PIXELDELAY
	sts		CharCnt,_0
	sts		PixelDelayCnt,_0
#endif
;
; Preset ScanTbl with default data
;
	ldi		r18,SCANSSTRTABLEN*4
	ldiw	X,ScanTblStr0
MainL22:
	st		X+,_0
	dec		r18
	brne	MainL22
#endif

	sts		CurPosition,_0			;beginning of line
	stsi	FlashCount,FLASHTICKS	;initialize tick counter for flashing
	stsi	RotationCountdown,BURNINCOUNTDOWN	;initialize countdown
;
; AVR initialization
;
	rcall	InitTC0					;does NOT run until enabled
	rcall	DACinit
	rcall	I2CInit

	rcall	InitLed
	rcall	InitSwitch
#if Mega168
	rcall	PCI2Enable				;push button switch
#elif Mega32
	rcall	InitINT1				;push button switch
#endif
	rcall	InitTC2
	rcall	InitFlashTbl
	rcall	SetTermScreen			;initialize ScanTbl
	rcall	ClearTermScreen
	rcall	SetWelcomeTermScreen
	rcall	UART_Init				;default baudrate = 19,200
	rcall	UsrGetBaudRate			;get saved baudrate
	lds		r16,BaudRate
	cpi		r16,4					;change if 4800 bps
	brne	MainL50
	rcall	UARTSet4800
MainL50:
	rcall	RTCReadTime
	rcall	UpdClockNum
	rcall	SetNumDisplayBuf
;
; Start the internal 1 PPS signal
;
	rcall	InitINT0
	sei
	call	SpecialUARTNL
	call	UsrShowStatus
;
; There is a problem with the RS-232 level conversion circuit.
; If the circuit is not connected to a terminal, the Serial Out signals
; are fed back to the Serial-In circuit, causing random input.
; Work around: delay enabling the Serial In circuit.
;
	call	UART_EnableIn
;
; Main Program Forever Loop.
; No registers need to be saved by the called functions
;
	rjmp	StartCursorTimer		;odd way to enter loop

MainLoop:
	rcall	ClockWorks				;Update the display and process events
;
; Did we receive a Button Up Message (mode change)?
;
	sbrs	G_FLAGS,fUP
	rjmp	MainL500				;not UP
	cbr		G_FLAGS,(1<<fUP)

	lds		r16,InputMode			;flip mode
	eor		r16,_1
	sts		InputMode,r16
	rcall	UsrShowStatus

MainL500:
;
; Was a char received on the UART
;
	sbrs	G_FLAGS,fRcvdChar  		;skip mainloop jump is set
	rjmp	TestCursorTimer
;
; UART char received. Check the current input mode:
;	Process User Commands or
;	Process Serial Port Data. Bunch this
;
	lds		r16,InputMode			;what mode are we in
	tst		r16						;0 means terminal mode
	breq	ProcessBunchedChars
	rcall	UsrCommand
	rjmp	MainLoop

ProcessBunchedChars:
	rcall	StopCursorFlashing
	ldi		r16,40					;max chars per bunch
ProcessInp:
	push	r16
	call	ProcessSerialInput
	pop		r16
	sbrs	G_FLAGS,fRcvdChar  		;goto mainloop if done
	rjmp	StartCursorTimer
	dec		r16
	brne	ProcessInp
;	rjmp	StartCursorTimer		;FALL THROUGH
;
; We want to show a blinking cursor at the current char position
; but only after no input was received for at least 1 second,
; to prevent wasting too much time setting up the flash line
;
StartCursorTimer:
	stsi	TimeTicks,110			;timeout will set a bit in G_FLAGS
	rjmp	MainLoop

TestCursorTimer:
	sbrs	G_FLAGS,fTimeOut  		;goto mainloop if not time yet
	rjmp	MainLoop	
	cbr		G_FLAGS,(1<<fTimeOut)
	rcall	StartCursorFlashing		;iff we're not flashing yet!
	rjmp	MainLoop	
;
; end of Main Program Loop
;

;
;	ProcessSerialInput
;
;	Accepts a buffered serial input char and update display memory
;
ProcessSerialInput:
#if DEBUG
	ldsw	Z,CTextFlashPtr			;current Flash Ptr
	or		ZH,ZL					;test for NULL
	breq	ProcessSIL050
	rjmp	PC
ProcessSIL050:
#endif
	rcall	UART_ReceiveByte		;get char in r24
	cpi		r24,0x1F				;If not a control char branch to displayable char routine.
	brpl	ProcessSIL400			;could still be >127
	cpi		r24,BS					;Backspace
	brne	ProcessSIL60
	lds		r16,CurPosition
	tst		r16
	breq	ProcessSIL900			;done
	dec		r16
	sts		CurPosition,r16
	rjmp	ProcessSIL900			;done
ProcessSIL60:
	cpi		r24,CR					;If this is a carriage return character, 
	brne	ProcessSIL100
	lds		r16,fCRPending			;is there one pending?
	tst		r16
	breq	ProcessSIL75			;no
	rcall	DoTermLF
ProcessSIL75:
	sts		fCRPending,_1
	rjmp	ProcessSIL900
ProcessSIL100:
	cpi		r24,TAB
	brne	ProcessSIL200
	ldi		r24,SPACE
	rjmp	ProcessSIL500
ProcessSIL200:
;
; Handle multiple LFs
; LF implies CR
; <CR><LF> is identical to <LF>
;
	cpi		r24,LF					;If its a linfeed char then set linefeed pending flag
	brne	ProcessSIL300
	sbrc	G_FLAGS,fLFPending		;check for multiple LFs
	rcall	DoTermLF
	sbr		G_Flags,(1<<fLFPending)
	sts		fCRPending,_0			;clear
	rjmp	ProcessSIL900
ProcessSIL300:
;
; other control char. Display as ^+letter
;
	push	r24						;save
	ldi		r24,'^'
	rcall	DisplayableChar			;recurse
	pop		r24						;restore
	addi	r24,64					;map
	jmp		DisplayableChar
ProcessSIL400:
	sbrc	r14,7					;must be <=127
	rjmp	ProcessSIL900			;if not, ignore
;
; Displayable char routine
;
DisplayableChar:
ProcessSIL500:
;
;	If not a control char then do line feed if pending
;
	sbrc	G_Flags,fLFPending		;If linefeed is pending, then do the LF
	rcall	DoTermLF				;will reset CurPosition
	lds		r16,fCRPending			;CR Pending?
	tst		r16
	breq	ProcessSIL600			;no
	sts		fCRPending,_0			;clear
	sts		CurPosition,_0			;to beginning of line
ProcessSIL600:
	rcall	GetTermLine				;get ptr to current line
	lds		r16,CurPosition			;positon within the line
	add		XL,r16
	adc		XH,_0
	st		X+,r24					;store char in Display Buffer
	inc		r16						;next
	sts		CurPosition,r16
	cpi		r16,MAXLINESIZE			;line overflow?
	brlt	ProcessSIL900			;no
	sbr		G_Flags,(1<<fLFPending)
ProcessSIL900:
	ret

DoTermLF:
	lds		r16,CurLine
	inc		r16						;next line
	cpi		r16,SCREENLINES
	brlt	DoTermLFL100
	rcall	ScrollTermScreen		;will clear last line
	ldi		r16,SCREENLINES-1			;last line
DoTermLFL100:
	sts		CurLine,r16				;save
	rcall	GetTermLine				;line ptr in X
	rcall	ClearTermLine			;clear new line
	sts		CurPosition,_0			;beginning of line
	cbr		G_FLAGS,(1<<fLFPending)
	sts		fCRPending,_0			;clear
	ret								;Done

;
; Clear Terminal Data Line - fill it with spaces
;
; IN:	X -- line to clear
;
; OUT:  X -- ptr to next line
;
ClearTermLine:
	push	r16
	ldi		r16,MAXLINESIZE
	ldi		r17,SPACE
ClearTLL100:
	st		X+,r17
	dec		r16
	brne	ClearTLL100
	st		X+,_0				;terminator
	adiw	X,1					;skip debug marker
	pop		r16
	ret

ClearTermScreen:
	ldi		r16,SCREENLINES
	ldiw	X,Line0
ClearTCL100:
	rcall	ClearTermLine
	dec		r16
	brne	ClearTCL100
	ret

FillTermLine:
	push	r16
	ldi		r16,MAXLINESIZE
FillTLL100:
	st		X+,r17
	inc		r17
	sbrc	r17,7				;wrap at 128
	ldi		r17,' '
	dec		r16
	brne	FillTLL100
	st		X+,_0				;terminator
	adiw	X,1					;skip debug marker
	pop		r16
	ret

SetWelcomeTermScreen:
	ldiw	Z,MsgPszWelcome*2
	ldiw	X,Line0
SetWTCL100:
	lpm		r0,Z+
	tst		r0
	breq	SetWTCL200		;done
	st		X+,r0
	rjmp	SetWTCL100
SetWTCL200:
	sts		CurLine,_1				;move to second line
	ldi		r16,SCREENLINES-2
	ldiw	X,Line2
	ldi		r17,'1'
SetWTCL300:
	rcall	FillTermLine
	dec		r16
	brne	SetWTCL300
	ret

SetTermScreen:
	ldi		r16,SCREENLINES
	ldiw	X,Line0
	ldiw	Z,ScanTblStr0
	ldi		r17,TOPLINEVPOS
	ser		r18
	ldi		r19,7
SetTSL100:
	st		Z+,XL
	st		Z+,XH
	adiw	X,MAXLINESIZE+1			;ptr to debug marker
	st		X+,r18					;ptr to next line
	st		Z+,r19					;HPos
	st		Z+,r17					;VPos
	subi	r17,LINEVSIZE			;Line below
	dec		r16
	brne	SetTSL100
	ret
;
; Scroll One Line
;
ScrollTermScreen:
	ldiw	Z,ScanTblStr0			;start here
	ld		r0,Z+					;save this line ptr
	ld		r1,Z+
	sbiw	Z,2						;back
	ldi		r17,SCREENLINES-1	;total - clock - first
	ldiw	X,ScanTblStr1			;copy from here
ScrollTSL100:
	ld		r16,X+					;copy line ptr
	st		Z+,r16
	ld		r16,X+
	st		Z+,r16
	adiw	X,2						;both to next entry
	adiw	Z,2
	dec		r17						;ScanTbl counter
	brne	ScrollTSL100			;more
	st		Z+,r0					;store line ptr from 1st entry
	st		Z+,r1
	movw	XL,r0					;clear this line
	rcall	ClearTermLine
	ret
;
; GetTermLine:
;
;	get ptr to current char line
;
; OUT	X -- ptr to line
;		Z -- ScanTbl entry where line is used
;
GetTermLine:
	lds		r16,CurLine				;current line
	cpi		r16,SCREENLINES			;out of range?
	brlt	GetTLL100
	clr		r16						;reset
GetTLL100:
	lsl		r16						;times 4
	lsl		r16
	ldiw	Z,ScanTblStr0			;ptr to first entry
	add		ZL,r16					;ptr to desired entry
	adc		ZH,_0
	ld		XL,Z+					;ptr to desired line
	ld		XH,Z+
	sbiw	Z,2						;ScanTbl Ptr where line is currently used
	ret
;
; Display a flashing cursor. This is done by building an alternate
; buffer for the current Display Line and mark the current line
; for flashing
;
StartCursorFlashing:
	ldsw	Z,CTextFlashPtr			;current Flash Ptr
	mov		r16,ZL					;test for NULL
	or		r16,ZH
	brne	StartCFL800				;not NULL, already flashing
	rcall	GetTermLine				;get current line ptr in X
	ldiw	Z,FlashBuffer			;destination
StartCFL100:						;copy line
	ld		r16,X+					;get char
	st		Z+,r16					;copy, incl. NULL
	tst		r16						;test for NULL
	brne	StartCFL100
;
; compute current position in FlashBuffer
;
	ldiw	Z,FlashBuffer			;destination
	lds		r16,CurPosition			;current line ptr in X
	cpi		r16,MAXLINESIZE			;line overflow?
	brlt	StartCFL200				;no
	ldi		r16,MAXLINESIZE-1
	rjmp	StartCFL300
StartCFL200:
	sbrs	G_FLAGS,fLFPending		;if LF pending, cursor under last char
	rjmp	StartCFL300
	lds		r16,CurPosition			;don't go back too far
	tst		r16
	breq	StartCFL300
	dec		r16
	sts		CurPosition,r16
StartCFL300:
	add		ZL,r16
	adc		ZH,_0
	ldi		r16,UNDERSCORE
	st		Z,r16
	rcall	GetTermLine				;ScanTbl ptr in Z,current line ptr in X
	movw	r0,XL					;swap them
	movw	XL,ZL
	movw	ZL,r0
	ldiw	A,FlashBuffer			;interface
	movw	r0,r16
	ldi		r16,0xff				;forever
	rcall	AddFlashItem
	stsw	CTextFlashPtr,X			;save Flash Ptr
	ret
StartCFL800:
	ret

StopCursorFlashing:
	ldsw	X,CTextFlashPtr			;current Flash Ptr
	rcall	RemoveFlashItem
	clrsw	CTextFlashPtr
	ret
;
; Workhorse function. Does not need to preserve registers
; 	
ClockWorks:
	rcall	ScreenRefresh		;update the display. No need to preserve registers
	sbrs	G_FLAGS,fUpdSecond	;Do we need to advance a second
	rjmp	CommonCWL100		;no
;
; code executed once a second
;
	rcall	UpdateSecs
	cbr		G_FLAGS,(1<<fUpdSecond)	;clear Seconds flag

CommonCWL100:
	sbrs	G_FLAGS,fFLASH		;Do we need to flash any fields
	rjmp	CommonCWL300
;
; Table Based Flashing
;
	call	ProcessFlashTbl
	cbr		G_FLAGS,(1<<fFLASH)	;Turn off Flash bit
;
CommonCWL300:
	sbrs	G_FLAGS,fMIN
	rjmp	CommonCWL500		;Skip tests for hours (fHR) boundary since it's only set
								;if fMIN is set;
; Check rotate Grid Offset
;
	lds		r16,RotationCountdown
	dec		r16
	brne	CommonCWL305
	rcall	RotateXYOffset
	ldi		r16,BURNINCOUNTDOWN	;initialize again
CommonCWL305:
	sts		RotationCountdown,r16

	cbr		G_FLAGS,(1<<fMIN)	;turn off Minute Flag
CommonCWL500:
	ret
;
; Interface function to ClockWorks
;	Preserves r16,r24,Z

SpecialClockWorks:
	push	r16					;temp register
	push	r24
	pushw	Z
	rcall	ClockWorks
	popw	Z
	pop		r24
	pop		r16
	ret
;
; ScreenRefresh: draw one image on the CRT
;
; r18 used as the ScanTbl index, pointing to next entry to process
; Z: current ScanTbl entry pointer
;
; No registers saved by this function. This is the
; responsibility of the caller.
;
.def	XOffset			=	r11
.def	YOffset			=	r12
;
#if 0
; Reminder:
.def	_0			= r15	;Zero register
.def	_1			= r14	;One register
.def	G_FLAGS 	= r25	;Global Flags
.def	GXOffset	= r8
.def	GYOffset	= r9
#endif
;
ScreenRefresh:
#if PIXELDELAY
	lds		r18,CharCnt			;from previous refresh
	neg		r18					;inverse relationship
	lsr		r18
	lsr		r18
	lsr		r18
	lsr		r18
	inc		r18
	sts		PixelDelayCnt,r18
	sts		CharCnt,_0			;reset
#endif
;
; Process SRam based String Data
;
	ldiw	X,ScanTblStr0		;First Pointer
	ldi		r18,SCANSSTRTABLEN	;count
;
; DrawChar Preloads Expected: r20, r21
;
	ldi		r20,low(NewFonttbl*2)	;rebase was corrected for SPACE
	ldi		r21,high(NewFonttbl*2)
;
; IN: r18, cnt   X, Table Ptr
;
TextTableScanSRam:
	ld		ZL,X+				;get new coordinates pointer LO,HI
	ld		ZH,X+
	ld		XOffset,X+
	ld		YOffset,X+
	mov		r0,ZL				;skip NULL entry
	or		r0,ZH
	breq	DrawTSL500
DrawTS100:
	ld		r16,Z+				;get next Text Code from SRam
	tst		r16					;end of string
	breq	DrawTSL500			;done
	rcall	DrawChar
	rjmp	DrawTS100
DrawTSL500:
	dec		r18
	brne	TextTableScanSRam
	ret
;
; DrawChar
;
; IN:	r16	-- char to draw. Valid range is 32..127 + HALFSPACE
;
; Preloads Expected: r20, r21
;
; USES: r0,r1,r6,r22,r24
; CharPixel uses: r3, r4 + r2
;
; Free r5, r13,r23
;
DrawChar:
	movw	r0,ZL			;save
	sbrc	r16,7			;bit 1 high is out of range
	rjmp	NewDCL900		;includes HALFSPACE
	subi	r16,FIRST_CHAR+1	;rebase
	brcs	NewDCL900		;char was out of range, includes SPACE
	BEAMON
	mov		ZL,r16			;compute 8 * char index
	clr		ZH
	lsl		ZL
	rol		ZH
	lsl		ZL
	rol		ZH
	lsl		ZL
	rol		ZH
	add		ZL,r20			;r20,r21 FontTable Ptr
	adc		ZH,r21
	ldi		r22,CHAR_HEIGHT	;First Y pos, Also Row Counter
NewDCL200:
	lpm		r6,Z+			;bits for current row
	clr		r24				;start x pos, 6 pixels per row
	sbrc	r6,7
	rcall	CharPixel
	inc		r24
	sbrc	r6,6
	rcall	CharPixel
	inc		r24
	sbrc	r6,5
	rcall	CharPixel
	inc		r24
	sbrc	r6,4
	rcall	CharPixel
	inc		r24
	sbrc	r6,3
	rcall	CharPixel
	inc		r24
	sbrc	r6,2
	rcall	CharPixel
	dec		r22				;next Y pos, row counter
	brne	NewDCL200		;finish all rows
#if PIXELDELAY
	lds		r5,CharCnt		;update cnt
	add		r5,_1			;inc doesn't set Carry
	brcc	NewDCL800
	dec		r5				;max 255 (for Atmega32)
NewDCL800:
	sts		CharCnt,r5
#endif
NewDCL900:
	BEAMOFF
	movw	ZL,r0			;restore
	rcall	GetCharWidth0	;make sure r16 still char
	add		XOffset,r16
	ret
;
; GetCharWidth0. For use in DrawChar
;
; IN:	 r16 TextCode
; OUT:	 r16 width
;
GetCharWidth0:
	cpi		r16,':'-(FIRST_CHAR+1)
	brne	GetCWL100
	ldi		r16,COLONCHARWIDTH
	rjmp	GetCWL400
GetCWL100:
	cpi		r16,HALFSPACE
	brne	GetCWL200
	ldi		r16,HALFSPACECHARWIDTH
	rjmp	GetCWL400
GetCWL200:
GetCWL300:
	ldi		r16,DEFCHARWIDTH
GetCWL400:
	ret
;
; GetCharWidth
;
; IN:	 r16 TextCode
; OUT:	 r16 width
;
GetCharWidth1:
	cpi		r16,':'
	brne	GetCWL500
	ldi		r16,COLONCHARWIDTH
	rjmp	GetCWL800
GetCWL500:
	cpi		r16,HALFSPACE
	brne	GetCWL600
	ldi		r16,HALFSPACECHARWIDTH
	rjmp	GetCWL800
GetCWL600:
	ldi		r16,DEFCHARWIDTH
GetCWL800:
	ret
;
;
; TLC7528 Dual 8-bit DAC
;
; Protocol: DACSELECT, out 8 Data bits, pulse WR low
; 
DACinit:
#if Mega32
	ser		r16
	out		DACDATADDR,r16
#elif Mega168
	sbi		DACHIDDR,0		;high nibble
	sbi		DACHIDDR,1
	sbi		DACHIDDR,2
	sbi		DACHIDDR,3
	sbi		DACLODDR,0		;low nibble
	sbi		DACLODDR,1
	sbi		DACLODDR,2
	sbi		DACLODDR,3
#endif
	sbi		DACCTLDDR,3		;DAC Select
	sbi		DACCTLDDR,4		;WR 
	sbi		DACCTLDDR,5		;LDAC/Z-Axis control

	sbi		DACZX			;Z-Axis control

;	sbi		DACZX			;Z-Axis control
	sbi		DACWR			;hold
	ret
;
; Display a Char Pixel (n points in X direction, m points in Y direction)
; where n and m are both either 1 or 2
;
; When not many characters are drawn on the screen, specially when there is a large blank
; area, a highlighted dot will appear just before that blank area (beam pausing)
; One possible enhancement is to keep track of the average char density
; on the screen and slow down the char pixel drawing speed accordingly, to
; even out the beam pausing. See conditional PIXELDELAY
;
; IN:	r24 (X), r22 (Y)
;
; Uses: r3, r4 + r2
;
CharPixel:
	mov		r3,r24
	mov		r4,r22
;
; X first (DACA)
;
	lsl		r24			;times 2
	add		r24,GXOffset ;grid offset
	add		r24,XOffset ;Scan Table offset
;
; Y second (DACB)
;
	lsl		r22			;times 2
	add		r22,GYOffset ;grid offset
	add		r22,YOffset ;Scan Table offset
;
; Here is decided how to draw one char pixel. The
; current design draws 2 pixels. It doesn't matter
; which axis is used (x or y).
;
; Drawing 3 or 4 screen pixels per char pixel will
; give a marginally better picture at a significantly
; reduced refresh rate.
;
#if (DOTSPERCHARPIXEL==4)
	rcall	DoXY
	inc		r24
	rcall	DoXY
	dec		r24
	inc		r22
	rcall	DoXY
	inc		r24
	rcall	DoXY
#elif (DOTSPERCHARPIXEL==3)
	rcall	DoXY
	inc		r22
	rcall	DoXY
	inc		r24
	rcall	DoXY
#elif (DOTSPERCHARPIXEL==2)
	rcall	DoXY
	inc		r22
	rcall	DoXY
#else
unsupported
#endif
	mov		r22,r4
	mov		r24,r3
	ret
;
; IN: r24, r22
;
; USES: r2
;
DoXY:	
	cbi		DACSELECT	;select DACA (active low)
#if Mega32
	out		DACDATA,r24
#elif Mega168
	mov		r2,r24
	out		DACLODATA,r2
	swap	r2
	out		DACHIDATA,r2
#endif
;
; latch DACA data
;
	cbi		DACWR		;latch data
	sbi		DACWR		;hold

	sbi		DACSELECT	;select DACB (active high)
#if Mega32
	out		DACDATA,r22
#elif Mega168
	mov		r2,r22
	out		DACLODATA,r2
	swap	r2
	out		DACHIDATA,r2
#endif
;
; latch DACB data
;
	cbi		DACWR		;latch data
	sbi		DACWR		;hold

#if PIXELDELAY
	lds		r2,PixelDelayCnt
CharPixelL100:
	dec		r2
	brne	CharPixelL100
#endif
	ret


.undef	XOffset
.undef	YOffset
;
;
; a Second has passed. Make sure this function in ONLY called from ClockWorks
; Note:	this function may be called several mSecs after the interrupt occurred
;		since we need to finish the Refresh() cycle. Not doing so results in
;		screen flicker.
;
UpdateSecs:
	rcall	BlinkLed
;
;Update Time
;
	lds		r17,AvrSecs		;get Seconds
	inc		r17				;update
	sts		AvrSecs,r17
	cpi		r17,60			;check for overflow
	brlt	UpdateSL300	
;
; a Minute has passed.
; Set Flag and keep time
;
	sbr		G_FLAGS,(1<<fMIN)	;set minutes flag
	sts		AvrSecs,_0		;reset Seconds
	lds		r17,AvrMins		;update minutes
	inc		r17
	sts		AvrMins,r17
	cpi		r17,60			;check for overflow
	brlt	UpdateSL300
;
; an Hour has passed.
;
	lds		r17,G2_FLAGS
	sbr		r17,(1<<fHR)	;set Hours flag
	sts		G2_FLAGS,r17
	sts		AvrMins,_0		;reset minutes
	lds		r17,AvrHrs		;update Hours
	inc		r17
	sts		AvrHrs,r17
	cpi		r17,24			;check for overflow
	brlt	UpdateSL300
;
; 24 hour overflow.
;
	sts		AvrHrs,_0
	rjmp	UpdateSL300
;
; Time has been updated. Update Time Display
;
UpdateSL300:
	rcall	UpdClockNum
	ret

;
; Update internal Time Values from RTC and update display
;
UpdateTimeAndDisplay:
	rcall	RTCReadTime
	rcall	UpdClockNum
	ret
;
; Compute Numeric Display Buffer
;
UpdClockNum:
	ldiw	X,NumDispBuf
	lds		r16,AvrHrs		;Data for Hours Digits
;
; Convert to AM/PM hours
;
	ldiw	Z,AMPMMapping*2
	add		ZL,r16			;index
	adc		ZH,_0
	lpm		r16,Z			;get mapped value
	rcall	SetAsciiFromBin	;convert to (hexa)decimal
	ldi		r16,':'			;separator
	st		X+,r16
	lds		r16,AvrMins		;Data for Minutes Digits
	rcall	SetAsciiFromBin	;convert to (hexa)decimal
	ldi		r16,':'			;separator
	st		X+,r16
	lds		r16,AvrSecs		;Data for Seconds Digits
	rcall	SetAsciiFromBin	;convert to (hexa)decimal

	ldi		r16,HALFSPACE	;separator
	st		X+,r16
	ldi		r17,'A'			;Preset 'AM'
	lds		r16,AvrHrs		;Data for Hours
	cpi		r16,12			;under 12, display A
	brlt	UCNL300
	ldi		r17,'P'			;Set 'PM'
UCNL300:
	st		X+,r17
	ldi		r17,'M'
	st		X+,r17
	st		X+,_0
	ret
;
; convert number to decimal text
;
; IN: r16 number to convert
;	  X: buffer ptr to store text
;
SetAsciiFromBin:
	mov		r24,r16
	rcall	FormatDecimal
	ret

SetNumDisplayBuf:
	ldiw	X,ScanTblStrClk	;Update Scan Table
	ldiw	Z,NumDispBuf	;needed twice
	st		X+,ZL
	st		X+,ZH
	rcall	ComputeSRamHPos	;center
	st		X+,r16			;Horizontal Position
	ldi		r16,NUMERICVPOS	;Vertical Position
	st		X+,r16
	ret
;
; ComputeSRamHPos
;	Compute Horizontal Pos necessary to center text
;
;	In:	Z ptr to SRam based text
;
;	Returns HPos in r16, 0 if overflow
;
;	Uses:	r18
;
ComputeSRamHPos:
	clr		r18					;width
CompSRamHPosL100:
	ld		r16,Z+
	tst		r16
	breq	CompSRamHPosL200
	rcall	GetCharWidth1		;r16 is char, r16 return (0 if error)
	add		r18,r16
	brcc	CompSRamHPosL100	;no overflow
	clr		r16
	rjmp	CompSRamHposL300
CompSRamHPosL200:
	ser		r16					;(255-width)/2
	sub		r16,r18
	lsr		r16
CompSRamHposL300:
	ret
;
; InitLed
;
InitLed:
	sbi		LED_DDR,LED_BIT
	cbi		LED_PORT,LED_BIT	;turn off
#if Mega32
	sbi		LED_DDR,LED2_BIT
	cbi		LED_PORT,LED2_BIT	;turn off
	sts		LedStatus,_0
#endif
	ret
;
; BlinkLed.
;
BlinkLed:
#if Mega168
	sbi		LED_PIN,LED_BIT		;toggle the LED
#elif Mega32
	lds		r16,LedStatus
	tst		r16
	breq	BlinkLedOn
	cbi		LED_PORT,LED_BIT	;turn off
	sts		LedStatus,_0
	rjmp	BlinkLedOut
BlinkLedOn:
	sbi		LED_PORT,LED_BIT	;turn on
	sts		LedStatus,_1
BlinkLedOut:
#endif
	ret
;
; TurnOffLed
;
TurnOffLed:
	cbi		LED_PORT,LED_BIT
	ret

#if Mega32
;
; TurnOnLed2
;
TurnOnLed2:
	sbi		LED_PORT,LED2_BIT
	ret
;
; TurnOff Led2
;
TurnOffLed2:
	cbi		LED_PORT,LED2_BIT
	ret
#endif
;
; InitSwitch
;
InitSwitch:
	cbi		SW_DDR,SW_BIT		;set as input
	sbi		SW_PORT,SW_BIT		;enable pull-up resistor
	ret
;
; Rotate the X/Y offset values
; TODO: consider testing for BurnInDisabled here
;
; Uses: r16
;
RotateXYOffset:
	lds		r16,RotationIndex
	cpi		r16,0xff			;disabled flag
	breq	RotL100
	pushw	Z
	ldiw	Z,RotationTable*2
	inc		r16
	andi	r16,0b00000111		;index % 8
	sts		RotationIndex,r16
	lsl		r16					;Compute table ptr
	add		ZL,r16
	adc		ZH,_0
	lpm		GXOffset,Z+			;get new values
	lpm		GYOffset,Z
	popw	Z
RotL100:
	ret
;
; Update Time Variable AvrSecs, AvrMins, AvrHrs
;
RTCReadTime:
;
; get time units in r24 in BCD form
; uses r16, r17
;
	push	r24
	ldi     r17,RTC_SEC
	rcall   I2CRndRd
	andi	r24,0b01111111
	rcall	bcdtobin		;convert
	cpi		r24,60
	brlt	RTC_RTL100
	clr		r24
RTC_RTL100:
	sts		AvrSecs,r24
	ldi     r17,RTC_MIN
	rcall   I2CRndRd
	andi	r24,0b01111111
	rcall	bcdtobin		;convert
	cpi		r24,60
	brlt	RTC_RT_L200
	clr		r24
RTC_RT_L200:
	sts		AvrMins,r24
	ldi     r17,RTC_HOUR
	rcall   I2CRndRd
	andi	r24,0b00111111
	rcall	bcdtobin		;convert
	cpi		r24,24
	brlt	RTC_RT_L300
	clr		r24
RTC_RT_L300:
	sts		AvrHrs,r24
	pop		r24
	ret
;
; Set RTC Time. Binary input in r24
; Uses r16,r17
;
RTCSetSecs:
	push	r24
	mov		r16,r24
	cpi		r16,60				;sanity check
	brsh	RTC_S100
	sts		AvrSecs,r24
	rcall	bintobcd
	ldi		ah,RTC_SEC			;seconds
	rcall	I2CRndWrt
RTC_S100:
	pop		r24
	ret

RTCSetMins:
	mov		r16,r24
	cpi		r16,60				;sanity check
	brsh	RTC_Mi100
	sts		AvrMins,r24
	rcall	bintobcd
	ldi		ah,RTC_MIN			;minutes
	rcall	I2CRndWrt
RTC_Mi100:
	ret

RTCSetHours:
	mov		r16,r24
	cpi		r16,24				;sanity check
	brsh	RTC_H100
	sts		AvrHrs,r24
	rcall	bintobcd
	ldi		ah,RTC_HOUR			;hours
	rcall	I2CRndWrt
RTC_H100:
	ret


;-----------------------------------------------------------------------------:
; 8bit/8bit Unsigned Division
;
; Register Variables
;  Call:  r16 = dividend (0x00..0xff)
;         r17 = divisor (0x01..0x7f)
;         r18  = <don't care>
;         r18   = <don't care> (high register must be allocated)
;
;  Result:r16 = r16 / r17
;         r17 = <not changed>
;         r18  = r16 % r17
;         r19   = 0
;
; Size  = 11 words
; Clock = 66..74 cycles  (+ret)
; Stack = 0 byte
div08u:
		clr	r18		;initialize variables
		ldi	r19,8		;  mod = 0; lc = 8;
					;---- calculating loop
		lsl	r16		;var1 = var1 << 1;
		rol	r18		;mod = mod << 1 + carry;
		cp	r18,r17		;if (mod => var2) {
		brcs	PC+3		; mod -= var2; var1++;
		inc	r16		; }
		sub	r18,r17		;/
		dec	r19		;if (--lc > 0)
		brne	PC-7		; continue loop;
		ret
;
; convert BCD number in r24 to binary in r24
;
bcdtobin:
#if 0
	push	r0
	push	r1
	push	r16
#endif
	push	r24
	swap	r24			;get high nibble. 0..15 value
	andi	r24,0x0f
	ldi		r16,10		;times 10
	mul		r24,r16		;result in r0:r1
	mov		r16,r0
	pop		r24
	andi	r24,0x0f	;low nibble
	add		r24,r16		;result
#if 0
	pop		r16
	pop		r1
	pop		r0
#endif
	ret
;
; convert binary number in r24 to BCD.
; Result in r16 and r24.
; uses r16,r17,r18,r19
;
bintobcd:
	mov		r16,r24		;divisor
	rcall	binto2digits
	swap	r16			;set high nibble
	or		r16,r18		;merge in low nibble
	mov		r24,r16		;set result
	ret
;
; split an 8-bit binary value into 2 decimal digits
;
; argument: r16 binary value
;
; results:	r18 low decimal digit
;			r16 high decimal digit
;
; uses:		r17,r18,r19
;
binto2digits:
	ldi		r17,10		;dividend
	rcall	div08u		;divide 24 by 10. Result in r16, Remainder in r18
	andi	r16,0x07	;force range limitation (max digit is 6)
	ret
;
; convert nibble value in r24 into Hex Char in r24
;
Hex2Asc:
	pushw	Z
	ldiw	Z,ASC_TAB*2
    add     ZL,r24
    adc     ZH,_0
    lpm		r24,Z
	popw	Z
    ret
;
; convert char in r24 to upper case
;
toupper:
	cpi		r24,'a'			;is this a lower case char
	brlt	toupperL100		;no
	cpi		r24,'z'+1
	brsh	toupperL100
	ldi		r16,'a'-'A'		;rescale
	sub		r24,r16
toupperL100:
	ret
;
; Decimal Formatting of binary value in r24
; Store in X pointed buffer, increment X
; Only values in r24 < 100 supported (if Decimal)
;
FormatDecimal:
#if DEBUG
	cpi		r24,100
	brsh	DDL100
#endif
	rcall	bintobcd
	push	r24
	swap    r24
	andi    r24,0X0F
	rcall   Hex2Asc
	st		X+,r24
	pop     r24
	andi    r24,0X0F
	rcall   Hex2Asc
	st		X+,r24
DDL100:
	ret
;
;------------- Execute a read or write on the EEPROM (depending on R17) ---------
; 					 if R17 == 6 then Write, if R17 == 1 then Read
;
; TODO: No need to wait for EepromTalk to finish if we're only writing ONE byte
;		Use EEProm Write Complete Interrupt
;
EepromTalk: 					
	out EEARL,XL 					; EEARL = address low
	out EEARH,XH 					; EEARH = address high
	adiw R26,1 						; address++
	sbrc R17,1 						; skip if R17 == 1 (read Eeprom)
#if Mega168
	sbi EECR,EEMPE 					; EEMPE = 1 (write Eeprom)
#elif Mega32
	sbi EECR,EEMWE 					; EEMWE = 1 (write Eeprom)
#endif
	out EECR,R17 					; EECR = R17 (6 write, 1 read)
L90: 
#if Mega168
	sbic EECR,EEPE 					; wait until EEPE == 0
#elif Mega32
	sbic EECR,EEWE 					; wait until EEWE == 0
#endif
	rjmp L90
	in R16,EEDR 					; R16 = EEDR
	ret
;
; Allow some simple actions, like Set Time and Change Baud Rate.
; Use the push button to change modes from terminal mode 
; (input is displayed on scope screen) to command mode (input is processed)
;
; Special Versions of the output functions.
; These call the main pump (ClockWorks) while waiting for
; the serial port to be ready.
;
; SpecialUARTRB
;
; return char in r24. 'X' means timeout occurred
;
SpecialUARTRB:
	push	r16
	lds		r16,AvrSecs
	ldi		r24,INP_TIMEOUT		;timeout seconds 
	add		r16,r24
	cpi		r16,60				;mod 60
	brlt	SpecialRBL100
	subi	r16,60
SpecialRBL100:
	rcall	UART_HasChar		;result in r24
	tst		r24
	brne	SpecialRBL200		;Character present
	rcall	SpecialClockWorks	;preserves r16,r24
	lds		r24,AvrSecs
	cp		r16,r24				;if same, timeout
	breq	SpecialRBL300	
	rjmp	SpecialRBL100
SpecialRBL200:
	rcall	UART_ReceiveByte 	;get data in r24
	rcall	toupper
	pop		r16
	ret
SpecialRBL300:
	ldi		r24,'X'
	pop		r16
	ret
;
; char to print in r24
;
SpecialUARTSB:
	push	r24
SpecialL150:
	rcall	UART_Is_Ready
	tst		r24
	brne	SpecialL200
	rcall	SpecialClockWorks	;preserves r16,r24
	rjmp	SpecialL150
SpecialL200:
	pop		r24
	rcall	UART_SendByte
	ret
;
; char ptr to print in Z
;
SpecialUARTPS:
	rcall	UART_Is_Ready
	tst		r24
	brne	SpecialL250
	rcall	SpecialClockWorks	;preserves r16,r24,Z
	rjmp	SpecialUARTPS
SpecialL250:
	rcall	UART_PrintfProgStr
	ret
;
; Print NewLine
;
SpecialUARTNL:
	push	r24
SpecialL275:
	rcall	UART_Is_Ready
	tst		r24
	brne	SpecialL300
	rcall	SpecialClockWorks	;preserves r16,r24,Z
	rjmp	SpecialL275
SpecialL300:
	pop		r24
	ldiw	Z,2*UART_pszEndOfLine
	rjmp	UART_PrintfProgStr
	ret
;
; Print TX Buffer
;
SpecialUARTTX:
	rcall	UART_Is_Ready
	tst		r24
	brne	SpecialL400
	rcall	SpecialClockWorks	;preserves r16,r24,Z
	rjmp	SpecialUARTTX
SpecialL400:
	rcall	UART_PrintTXBuffer
	ret
;
; Serial (RS-232) User Interface
;
UsrCommand:
;
; no need to loop here since we know a char is waiting
;
	rcall	UART_ReceiveByte ;get data in r24
	rcall	toupper			;ignore case
	cpi		r24,'X'			;is it an 'X'
	breq	UsrL10			;yes, process, no echo
	rjmp	UsrL900			;else, exit
;
UsrL10:
	ldiw	Z,MsgPszCommand*2
	rcall	SpecialUARTPS
	rcall	SpecialUARTRB	;get command in r24
	rcall	SpecialUARTSB	;echo
	rcall	SpecialUARTNL
	cpi		r24,'T'			;set Time?
	breq	UsrSetTime
	cpi		r24,'C'			;Copright Message?
	breq	UsrShowCopyright
	cpi		r24,'B'
	breq	UsrChangeBaudRate
;
; more commands here
;
Usr_L20:
	cpi		r24,'X'				;quit?
	rjeq	Usr_Cancel			;yes, exit
	rjmp	UsrL10				;no, loop

UsrSetTime:
	ldiw	Z,2*MsgPszHourCommand
	rcall	UsrGetNum
	brcs	Usr_CancelandRefresh
	rcall	RTCSetHours
	rcall	SpecialUARTNL
	ldiw	Z,2*MsgPszMinCommand
	rcall	UsrGetNum
	brcs	Usr_CancelandRefresh
	rcall	RTCSetMins
	rcall	SpecialUARTNL
	ldiw	Z,2*MsgPszSecCommand
	rcall	UsrGetNum
	brcs	Usr_CancelandRefresh
	rcall	RTCSetSecs			;now set value
	lds		r16,AvrSecs			;UpdateSecs will increment this
	dec		r16
	sts		AvrSecs,r16
	call	UpdateSecs
	rcall	SpecialUARTNL
	rjmp	UsrL900
Usr_CancelandRefresh:
	call	UpdateTimeAndDisplay
Usr_Cancel:
	ldiw	Z,MsgPszCancel*2
	rcall	SpecialUARTPS
	rjmp	UsrL900

UsrShowCopyright:
	ldiw	Z,MsgPszCopyright*2
	rcall	SpecialUARTPS
	rjmp	UsrL900
;
; Possible enhancement:
;	- store the new baudrate in EEProm
;	- allow more baudrates (select with a numeric code)
;						
UsrChangeBaudRate:
	lds		r16,BaudRate
	cpi		r16,4				;short for 4800
	brne	UsrCBRL100
	ldiw	Z,MsgPszBaud19200*2
	ldi		r16,19				;new baudrate
	rjmp	UsrCBRL200
UsrCBRL100:
	cpi		r16,19
	brne	UsrCBRL600			;validity check
	ldiw	Z,MsgPszBaud4800*2
	ldi		r16,4				;new baudrate
UsrCBRL200:
	sts		BaudRate,r16		;save
	rcall	SpecialUARTPS
	lds		r16,AvrSecs
	ldi		r24,3				;wait 3 seconds
	add		r16,r24
	cpi		r16,60				;mod 60
	brlt	UsrCBRL300
	subi	r16,60
UsrCBRL300:
	rcall	SpecialClockWorks	;preserves r16,r24,Z
	lds		r24,AvrSecs
	cp		r16,r24				;if same, timeout
	brne	UsrCBRL300

	rcall	UsrSaveBaudRate		;save in EEProm
	lds		r16,BaudRate
	cpi		r16,4				;short for 4800
	breq	UsrCBRL500
	rcall	UARTSet19200
	rjmp	UsrCBRL600
UsrCBRL500:
	rcall	UARTSet4800
UsrCBRL600:
	rjmp	UsrL900
;
; Save current BaudRate variable (4 or 19) in EEProm
;
UsrSaveBaudRate:
	pushw	X
	lds		r16,BaudRate
	ldiw	X,E2END -2 			;BaudRate storage located at address E2END-2
	out		EEDR,r16			;EEDR = r16
	ldi		R17,6 				;write EEPROM
	rcall	EepromTalk
	popw	X
	ret
;
; Get Current Baudrate (4 or 19) from EEProm into BaudRate variable
;
UsrGetBaudRate:
	pushw	X
	ldiw	X,E2END -2 			;BaudRate storage located at address E2END-2
	ldi		r17,1 				;read EEPROM
	rcall	EepromTalk			;result in r16
	cpi		r16,4				;validity test
	breq	UsrSBRL300			;ok
	cpi		r16,19
	breq	UsrSBRL300			;ok
	ldi		r16,19				;defaults to 19,200
UsrSBRL300:
	sts		BaudRate,r16		;store
	popw	X
	ret
;
; exit
;
UsrL900:
	rcall	UsrShow				;SpecialUARTNL
	ret							;end Usr_Command
;
;	UsrGetNum
;
;	get a 2 digit number from the Serial Port
;
;	IN	Z -- string ptr to print
;
;	OUT	Carry if cancel or timeout
;		r24 - number
;
;	USED: A, B, r24, r0, r1
;
UsrGetNum:
	rcall	SpecialUARTPS
UsrGNL100:
	rcall	SpecialUARTRB	;get high value in r24
	rcall	SpecialUARTSB	;echo
	cpi		r24,'X'			;quit?
	brne	UsrGNL200
UsrGNL190:
	sec
	ret
UsrGNL200:
	rcall	UsrChkDigit	;check for valid digit
	brcs	UsrGNL100		;invalid, try again
	mov		r16,r24			;save high digit
UsrGNL250:
	rcall	SpecialUARTRB	;get low digit in r24
	rcall	SpecialUARTSB	;echo
	cpi		r24,'X'			;quit?
	breq	UsrGNL190
	rcall	UsrChkDigit	;check for valid digit
	brcs	UsrGNL250		;invalid, try again
	ldi		r17,10			;multiply high digit by 10
	mul		r16,r17			;result in r1:r0. r1 always 0
	add		r24,r0			;compose binary value
	clc
	ret
;
; check if the digit in r24 is a valid digit (0-9)
; return C if invalid, else return binary value in r24
;
UsrChkDigit:
	cpi		r24,'0'			;in range?
	brlo	UsrCDL300		;no
	cpi		r24,'9'+1		;in range?
	brsh	UsrCDL300		;no
	subi	r24,'0'			;convert to binary
	clc
	ret
UsrCDL300:
	sec
	ret

UsrShowStatus:
	ldiw	Z,MsgPszSignOn*2	;signon
	rcall	SpecialUARTPS
	clr		r16					;no CR/LF
	rcall	DisplayTime			;display on serial port
	ldiw	Z,MsgPszTermMode*2	;preset
	lds		r16,InputMode
	tst		r16
	breq	UsrSSL100
	ldiw	Z,MsgPszCmdMode*2
UsrSSL100:
	rcall	SpecialUARTPS
	rcall	SpecialUARTNL
	ret

UsrShow:
	ldiw	Z,MsgPszSignOn*2	;signon
	rcall	SpecialUARTPS
	clr		r16					;no CR/LF
	rcall	DisplayTime			;display on serial port
	rcall	SpecialUARTNL
	ret
;
; Format Current Time in Transmit Buffer (X)
;
; Buffer overflow checked at code time (fixed format)
;
; Duplicate code. See UpdClockNum
;
FormatTime:
	lds		r24,AvrHrs			;Display Hours
	rcall	FormatDecimal
	ldi     r24,':'
	st		X+,r24
	lds		r24,AvrMins			;Display Minutes
	rcall	FormatDecimal
	ldi     r24,':'
	st		X+,r24
	lds		r24,AvrSecs			;Display Seconds
	rcall	FormatDecimal
	ret
;
; Display Current Time based on Internal Variables
; Create the text in the Transmit Buffer, then send
;
; Add optional CR/LF based on r16
;
; Buffer overflow checked at code time (fixed format)
;
DisplayTime:
	rcall	UART_Is_Ready		;Currently Transmitting?
	tst		r24
	brne	DisplayTL100		;no continue
	rcall	SpecialClockWorks	;preserves r16,r24,Z
	rjmp	DisplayTime			;Yes, loop until ready
DisplayTL100:
	pushw	Z					;needed?
	pushw	X
	push	r16					;save CR/LF Flag
	ldiw	X,TXBuffer
	rcall	FormatTime
	pop		r16
	tst		r16
	breq	DisplayTL200
	ldi     r24,CR
	st		X+,r24
	ldi     r24,LF
	st		X+,r24
DisplayTL200:
	st		X+,_0				;Termination Char
	rcall	UART_PrintTXBuffer	;start Transmitting
	popw	X
	popw	Z					;needed?
	ret

MsgPszWelcome:			.db	"Dutchtronix Terminal",0,0
MsgPszSignOn:			.db	"AVR Oscilloscope Terminal V3.0a ",0,0
MsgPszTermMode:			.db	" Mode is Terminal",0
MsgPszCmdMode:			.db	" Mode is Command",0,0
MsgPszCommand:			.db "Command(T,B):",0
MsgPszHourCommand:		.db	"2 digit hours:   ",0
MsgPszMinCommand:		.db	"2 digit minutes: ",0
MsgPszSecCommand:		.db	"2 digit seconds: ",0
MsgPszCancel:			.db	"Cancel or Timeout ",0,0
UART_pszEndOfLine:		.db 13,10,0,0
MsgPszCopyright:		.db	"Copyright 2008  Jan P.M. de Rie",13,10,0
MsgPszBaud4800:			.db	"Switching to 4800 Baud ",13,10,0
MsgPszBaud19200:		.db	"Switching to 19200 Baud",13,10,0



