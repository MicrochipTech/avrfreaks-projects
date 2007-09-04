;  ************************************************************************************
;  *    KBASC36.asm  -  PC keyboard to ASCII RS232 for AVR tiny11  using 3.6864MHz crystal or clock
;  *	v1.5  Alan Probandt Portland Oregon USA     latest update Sept 4, 2007
;  *
;  * 	Code designed for the Atmel AVR Tiny11/12
;  *    Use a stand-alone PC keyboard and a Tiny11 to send an RS-232 ASCII char from each keypress
;  *
;  *   [ Use Terminal font in text editor to format this file correctly.  8 spaces per tab ]
;  *               
;  *    Original version finished May 27, 2005 v1.4
;  *
;  *    PC keyboard clock signal on AVR pin 6 (INT0)
;  *    PC keyboard data signal on AVR pin 5 (PB0)
;  *    RS-232 output on pin 7   connect to a MAX232 level-convert chip for true RS232 output voltage levels
;  *
;  *	  Every key stroke sends multiple scancode bytes to the AVR.  Keypress down sends one or
;  *	two scancodes.  Key release sends two or three scancodes. Each scancode byte takes
;  *	about 840 microseconds to transmit to the AVR.  When each data bit of the scancode
;  *	is stable on the Keyboard Data line, the Keyboard Clock line goes low.  This falling
;  *	edge generates an INT0 interrupt.  The INT0 routine reads and formats the data bit.
;  *	After each scancode transmission completes, the final received bit sets the
;  *	NewScanCode flag in the Flag Register.
;  *	  Each new scancode is tested to see whether it is an ASCII character to be sent out
;  *	the RS-232 or a format scancode.  The format scancodes are ignored.
;  *	Each RS232 byte transmission takes 261 microseconds at 38.4Kbaud.  The keypresses
;  *	are put into a queue (consecutive Tiny11 registers), but that's not necessary
;  *	since the fastest typist is still much slower than the slowest standard baud rate.
;
;  *	  The timer IRQ runs constantly at the transmit baud rate.  The routine tests whether any
;  *	character is being currently sent and which bit gets sent next.  When the char is
;  *	finished being transmitted, the routine clears the T flag which lets the main program know
;  *	that it can get the next char from the queue and transmit it.  The main program
;  *	puts the char into the transmit register and sets the T flag.  The next timer IRQ reloads
;  *	the timer's counter register and checks the T flag.  If T is set, the IRQ outputs the
;  *	bits.  Otherwise it exits. The Timer IRQ routine takes 11 cycles per baud period with no
;  *	char being transmitted and about 20-24 cycles per baud period when doing a transmission.
;
;
;    				Atmel AVR Tiny11
;	  RESET/ 	PIN 1     | VCC  PIN 8  +5V
;  3.68 MHz Xtal	PIN 2 PB3 | PB2  PIN 7 RS232 Out  to MAX232 level convert chip
;  3.68 MHz Xtal	PIN 3 PB4 | PB1  PIN 6 keyboard clock INT0
;	   	        PIN 4 Gnd | PB0  PIN 5 keyboard data
;				    -------
; --------------------------------
;  |     bottom of MiniDIN6 	 |
;  |            	         |
;  |   +5   nc   data  gnd       |
;  |    x    x    x    x         |
;  | nc x              x clk     |
; --------------------------------
;  ************************************************************


.nolist
.include "Tn11def.inc"
.list

.equ	Clock			= 3686400
.equ	baud			= 38400 ; RS232 baud rate
.equ	ReloadCycles		= 6 ; # of cycles from timer overflow until TCNT is reloaded
.equ	RS232txPin		= 2	 ; RS232 Transmit pin
.equ	Timer38K		= (256-(clock/(baud))) + ReloadCycles
.equ	INITIAL_BITCOUNT 	= 11   ;  used by Get_Scan
.equ	PARITY_BITCOUNT 	= 3    ;  used by Get_Scan
.equ  	KBPort			= Pinb
.equ	KBDataPin		= 0

; Flag register's bit values

;*****************************************************************************************
;  Register definitions
;
;  lower registers are used as a 15 byte buffer (r1 to r15)
.def	DecodeValue 	= r0   
.def	SREGtemp	= r1

;  upper registers
.def	temp		= r16	; scratch register
.def	irqtemp		= r17  	; interrupt's scratch register
.def	scanbits	= r18 	; shift register for holding PC keyboard data while being assembled
.def	scancode	= r19
.def	bitcount	= r20	; used by INT0 irq  [getscan]
.def	TXout		= r21
.def	TXcount 	= r22
.def	FlagReg		= r23
.equ	NewScanCode	=  0
.equ	Shift_flag	=  1
.equ	Break_flag	=  2
.equ	Alt_flag	=  3
.equ	Control_flag	=  4
.equ	E0_flag		=  5
.equ	TXActive	=  6


.cseg
.org   $0000
		rjmp 	reset  	; Reset handler $000

		rjmp	get_scan	; $001  External interrupt 0 handler

		reti		; $002 Pin Change interrupt

;******************************************************************************
;
;	Timer Overflow Interrupt Code
;
;   This interrupt is executed every (1/38400) 26microseconds (RS232 baud rate).
;  It reloads the timer count and checks the T flag.  If clear, then a byte
;  is not being transmitted out the RS232, and the routine exits.  If set,
;  then the bit count for the transmitting byte is tested for 0, 10, or else.
;  If 10, then begin transmission with the start bit (by bringing the
;  RS232 transmit line pin 7) low.  If bitcount is 0, the stop bit is
;  finished sending.  At this time clear the TXActive flag.
;    There is a always one baud period between the end of one byte transmission
;  and the beginning of the next transmission.
;******************************************************************************

.org  OVF0addr         	; $003  Timer 0 overflow handler
;  Since the analog comparator isn't used, it's OK to put the timer IRQ
;  routine here and save two clock cycles of IRQ latency by using no RJMP instruction.
OVT0:		in	SREGtemp, SREG
		ldi	irqtemp, Timer38K
		out	TCNT0, irqtemp	; reload the timer count to 38.4Kbaud period
		sbrs	FlagReg, TXActive	; TXActive set when processing data through SoftUART
		rjmp	exitOVT0 	

OUT1:		tst	TXcount
		breq	TXdone
		cpi	TXcount, 10; make a start bit?
		breq	DoStart
		cpi	TXcount,1  ; make a stop bit?
		breq	DoStop
		lsr	TXout
		brcc	OutA0
		sbi	PortB, RS232txPin
		rjmp	DoOutBit1
OutA0:		cbi	PortB, RS232txPin
DoOutBit1:	dec	TXcount
		rjmp	exitOVT0 

DoStart:	cbi	PortB, RS232txPin
		dec	TXcount
		rjmp	exitOVT0 

DoStop:		sbi	PortB, RS232txPin
		dec	TXcount
		rjmp	exitOVT0 

; indicate to main code that SoftUART is ready for new data by clearing the TXActive flag
TXdone:		cbr	FlagReg, (1<<TXActive)
		 
exitOVT0:	out	SREG, SREGtemp
		reti


;********************************************************
;
;	Main Code
;
;********************************************************
RESET:
; set-up ports for input/output and pull-up resistors
		ser	temp
		out	portb,temp	; port B pull-up resistors are all on
		clr	temp
		out	ddrb,temp	; port b is all inputs, except RS232 transmit pin
		ldi	temp,(1<<RS232txPin)
		out	ddrb,temp

		ldi	temp,  (1<<INT0)
		out	GIFR, temp	; clear INT0 interrupt flag
		out	GIMSK,temp	;  enable external INT0
		ldi	temp,(1<<ISC01) ; setup INT0 interrupt on falling edge
		out	MCUCR, temp

		ldi	temp, Timer38K
		out	TCNT0, temp
		ldi	temp, (1 << TOV0) ; clear interrupt flag
		out	TIMSK, temp
		ldi	temp, ( (0<<CS02) | (0<<CS01) | (1<<CS00) )
		out	TCCR0, temp	; start timer interrupt
		ldi	bitcount,INITIAL_BITCOUNT ; = 11
		wdr	; always reset watchdog timer before enabling it
		ldi	temp, (1<< wde)	; timeout watchdog in 15mSec if no WDR reset
		out	wdtcr, temp	; turn on watchdog timer, reset wdr every 32uSec
		clr	FlagReg
		sei	; switch on the general interrupt enabler

;*****************************************************************************
;
;  GetChar:  Main Loop of the Program  99.9% of time spent here
;
;*****************************************************************************

GetChar:  ; has a new char arrived from the keyboard?
		sbrs	FlagReg, NewScanCode
		rjmp	GetChar	; yes, so process it

; a new keypress or keypress release
		cbr	FlagReg, (1<< NewScanCode) ; prepare flag for next keypress
		rjmp	DecodeData

BackFromDecodeData:	; DecodeValue=0 do nothing; else put into the SoftUART
		tst	DecodeValue
		breq	GetChar	; if DecodeValue <> 0, then put DecodeValue out the RS232 softUART

; test if the softUART is finished transmiting the previous character
UARTwait:	sbrc	FlagReg, TXActive ; T flag clear=SoftUART ready to send
		rjmp	UARTwait ; if T is set, then poll it until it is cleared by Timer interrupt routine
UARTready:	mov	TXout, DecodeValue ;set up SoftUart data
		ldi	TXcount, 10	; set up tranmission bit count
		sbr	FlagReg, (1<< TXActive) ; set Active flag to begin transmission on next timer IRQ
		rjmp	GetChar



;*****************************************************************************************************************
;  DECODE_DATA  -- routine to convert the scancode from the keyboard into ASCII.
;	Not a subroutine. DECODE_DATA's single exit point jumps back to Main code.
;
;   	input:    scancode (register value) - the lastest byte received from the PS2 keyboard  
;       returns:  DecodeValue register (r0)
;
;   A key pressed down will send the one-byte scancode to the PC (for most keys).
;   When the key is released, the keyboard sends an 0xf0 byte followed by the scancode of the key being released.
;   This routine uses a flag called Break to determine if the scancode comes from a keypress down or release.
;	Break == clear means that scancode just received is a keypress down and should be decoded to ASCII.
;	Break == set means that the scancode just received was 0xf0 and the next scancode will be ignored.
;
;*****************************************************************************************************************
DecodeData:
;   Check for the one key on the PS2 keyboard (Function 7) whose scancode is outside of the table range 00-7f.
		cpi	scancode, $83
		brne	InRange
		rjmp	setZero  ; function keys are ignored in this program

;   first check that Break flag is clear. if yes, then keypress down received
InRange:
		sbrc	FlagReg, Break_flag   ; the previous scancode was $f0 when break_flag is set => key released
		rjmp	Break_set

;  Break is clear, so do check the special conditions: F0, 12, 14, 59, 58, E0,
		cpi	scancode,$f0  ; 	$F0 =Breakcode key-release identifier
		brne	ChkShift
		sbr	FlagReg, (1<<Break_flag)  ; set Break flag  so that the next scancode will not be seen a keypress down
		rjmp	setZero

ChkShift:
		sbrc	FlagReg, Shift_flag   ; the shift key is being held down and another key was just pressed
		rjmp	Shift_set
		sbrc	FlagReg, Control_flag   ; the control key is being held down and another key was just pressed
		rjmp	Control_set

notF0:
		cpi	scancode,$12  ; 12 =Left Shift
		brne	notLS
		sbr	FlagReg, (1<<Shift_flag)
		rjmp	setZero

notLS:		cpi	scancode,$59  ; Right Shift
		brne	notRS
		sbr	FlagReg, (1<<Shift_flag)
		rjmp	setZero

notRS:		cpi	scancode, $11 ; left Alt key is being pressed down. Alt_flag gets set.
		brne	notSft
		sbr	FlagReg, (1<<Alt_flag)
		rjmp	setZero

notSft:		cpi	scancode,$14  ; Right Control
		brne	isCapsLock
		sbr	FlagReg, (1<<Control_flag)
		rjmp	setZero

isCapsLock:	; CapsLock key pressed
		cpi	scancode, $58  ; caps lock key
		brne	isExtendedCode
		rjmp 	setZero

isExtendedCode: ; check for E0 flag
		cpi	scancode,$e0  ;  $e0  extended char table
		brne	isE0set  ; scancode is not one of the special cases
		sbr	FlagReg,(1<<E0_flag) ; set E0 flag
		rjmp	setZero

isE0set: ; if the E0_flag is set then E0 (extended char scancode) was the last previously sent
		sbrc	FlagReg,E0_flag 	; test extended char flag
		rjmp	get_E0char	; determine the action to take for the extended keypress

;not shift, $E0, or $F0, and no flags are set;  so do a table lookup
TableLookUp:
		ldi	ZL, low(2 * ScanTable)
		ldi	ZH, high(2 * ScanTable)
		add	ZL, scancode
		brcc	TL0
		inc	ZH
TL0:		lpm
		tst	R0
		brmi	setZero
		rjmp 	exit_decode

Shift_set:
		ldi	ZL, low(2 * ShiftTable)  ; do a table lookup from the ShiftTable
		ldi	ZH, high(2 * ShiftTable)
		add	ZL, scancode
		brcc	TL1
		inc	ZH
TL1:		lpm
		tst	R0
		brmi	setZero
		rjmp 	exit_decode

Control_set:	ldi	ZL, low(2 * ScanTable)  ; Control key is held while another key pressed.  Assume the 2nd key is alphabetical
		ldi	ZH, high(2 * ScanTable) ; Find the ASCII and subtract 0x60
		add	ZL, scancode
		brcc	TL2
		inc	ZH
TL2:		lpm
		tst	R0
		brmi	setZero
		mov	temp, DecodeValue
		subi	temp, $60   ;  'b' = 0x62 in ASCII --  Control-B = 0x02 in ASCII
		mov	DecodeValue, temp
		rjmp 	exit_decode

;  Break flag is set - this means the previous scancode was $f0 (a key release)
;   This scancode is either the Extended sentinel ($e0) or the number of the key being released.
Break_set:	cbr	FlagReg,(1<<Break_flag) ; clear Break flag
		cpi	scancode, $11	; left Alt flag is released
		brne	isShift
		cbr	FlagReg, (1<<Alt_flag)
		rjmp	setZero
isShift:
		cpi	scancode, $12
		brne	isSh0
		cbr	FlagReg, (1<<Shift_flag)
		rjmp	setZero
isSh0:
		cpi	scancode, $59
		brne	isCntrl
		cbr	FlagReg, (1<<Shift_flag)
		rjmp	setZero
isCntrl:
		cpi	scancode, $14
		brne	isE0
		cbr	FlagReg, (1<<Control_flag)
		rjmp	setZero

; If scancode is E0, then set the Break flag so that the next scancode is recognized
; as the last of an extended release sequence instead of a normal keypress down.
isE0:		cpi	scancode, $e0	; extended key press sentinel
		brne	isE0flagOn
		sbr	FlagReg,(1<<Break_flag)
		sbr	FlagReg,(1<<E0_flag) ; set extended flag
		rjmp	setZero

; if the E0flag is on then this scancode is the extended character number that is being released.  Just clear flags.
isE0flagOn:	sbrs	FlagReg, E0_flag   ; Extended flag on?
		rjmp	ReleaseKey  ;no, a non-extended key is being released.
		cbr	FlagReg,(1<<Break_flag) ; yes, clear Break flag
		cbr	FlagReg,(1<<E0_flag) ; clear extended flag
		cpi	scancode, $14 ; Right Control key
		brne	chkRAltUp
		cbr	FlagReg,(1<<Control_flag) ; clear control flag
		rjmp	setZero
chkRAltUp:	cpi	scancode, $11 ; Right Alt key
		brne	setZero
		cbr	FlagReg,(1<<Alt_flag) ; Right Alt key released
		rjmp	setZero
ReleaseKey:  	cbr	FlagReg,(1<<Break_flag) ; yes, clear Break flag

setZero:  	clr	DecodeValue	; When Decode sub returns zero in DecodeValue reg, the main program ignores the scancode.
exit_decode: 	rjmp	BackFromDecodeData ; subroutine's single exit point

;  the previous scancode was E0, and no extended keys are handled by the Tiny11 code.
get_E0char:	cbr	FlagReg,(1<<E0_flag)  ; clear the E0 flag

		rjmp	setZero ; change to RET if using this function with an AVR with a stack



;********************************************************************************
; GET_SCAN:  ; signal handler for external interrupt int0 (AVR pin 6)
;   called by falling transition of the keyboard clock signal
;   assemble the scancode from keyboard data signal- data valid when clock is low
;  22 cycles for mid-byte data; 20 start and parity bits; 25 for stop bit
;
; (  use Terminal font to see correctly )
;clk---|_|--|_|--|_|--|_|--|_|--|_|--|_|--|_|--|_|---|_|---|_|----------   (---- = logic high +5v)
;data-\___/-----\___/-----\___/-----\___/-----\___/--------------------
;    11   10    9    8    7    6    5    4    3     2     1 bitcount on falledge before dec
;   start    0    1    2    3    4    5    6    7     P   stop   data bit
;  Bitcnt 3 to 10 is data. start bit = 11, parity bit=bit 2;  stop bit = bit 1
;  IRQ including latency:  16 cycles for start bit, 20 mid-byte and stop
;*********************************************************************************
get_scan:
		cpi 	bitcount,INITIAL_BITCOUNT ; falling edge FIRST pulse from kbd -start bit so do nothing
		breq	GSD0
		cpi	bitcount,PARITY_BITCOUNT ; = 3  ; test for parity bit and stop bit
		brlo	GSD0  			;  must use bitcount 3 for compare because branch tests only for lower
		lsr	scanbits   			;  shift data right one bit - data bit 7 gets 0
		sbic	KBport,KBdataPin  		;  set scancode bit if keyboard data bit is set
		ori	scanbits,$80 		;  if data from kbrd is 1, then set bit 7 only and let other bits unchanged
GSD0:		dec 	bitcount
		brne	GSD1			;  All bits received?
		mov	scancode, scanbits
		ldi	bitcount,INITIAL_BITCOUNT
		sbr	FlagReg, (1<< NewScanCode)
GSD1:  		reti


ScanTable:  ; ASCII values that correspond to the PC keyboard's transmitted scancode
;    0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
.db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$09,$60,$ff ; 00-0f
.db $ff,$ff,$ff,$ff,$ff,$71,$31,$ff,$ff,$ff,$7a,$73,$61,$77,$32,$ff ; 10-1f
.db $ff,$63,$78,$64,$65,$34,$33,$ff,$ff,$20,$76,$66,$74,$72,$35,$ff ; 20-2f
.db $ff,$6e,$62,$68,$67,$79,$36,$ff,$ff,$ff,$6d,$6a,$75,$37,$38,$ff ; 30-3f
.db $ff,$2c,$6b,$69,$6f,$30,$39,$ff,$ff,$2e,$2f,$6c,$3b,$70,$2d,$ff ; 40-4f
.db $ff,$ff,$27,$ff,$5b,$3d,$ff,$ff,$ff,$ff,$0d,$5d,$ff,$ff,$ff,$ff ; 50-5f
.db $ff,$ff,$ff,$ff,$ff,$ff,$08,$ff,$ff,$31,$ff,$34,$37,$ff,$ff,$ff ; 60-6f
.db $30,$ff,$32,$35,$36,$38,$1b,$ff,$ff,$ff,$33,$ff,$ff,$39,$ff,$ff ; 70-7f

ShiftTable:     ; ASCII values that correspond to the PC keyboard's transmitted scancode when shift key held down
;    0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
.db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$09,$7e,$ff ; 00-0f
.db $ff,$ff,$ff,$ff,$ff,$51,$21,$ff,$ff,$ff,$5a,$53,$41,$57,$40,$ff ; 10-1f
.db $ff,$43,$58,$44,$45,$24,$23,$ff,$ff,$20,$56,$46,$54,$52,$25,$ff ; 20-2f
.db $ff,$4e,$42,$48,$47,$59,$5e,$ff,$ff,$ff,$4d,$4a,$55,$26,$2a,$ff ; 30-3f
.db $ff,$3c,$4b,$49,$4f,$29,$28,$ff,$ff,$3e,$3f,$4c,$3a,$50,$5f,$ff ; 40-4f
.db $ff,$ff,$22,$ff,$7b,$2b,$ff,$ff,$ff,$ff,$0d,$7d,$ff,$7c,$ff,$ff ; 50-5f
.db $ff,$ff,$ff,$ff,$ff,$ff,$08,$ff,$ff,$31,$ff,$34,$37,$ff,$ff,$ff ; 60-6f
.db $30,$ff,$32,$35,$36,$38,$1b,$ff,$ff,$ff,$33,$ff,$ff,$39,$ff,$ff ; 70-7f

ID:  ;"****  KBASC36.asm  v1.5  Alan Probandt"
.db   "****  BKSA3C.6sa mv .1 5A al nrPbonatd"


;;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
; transmit square wave to verify correct baud rate output
;Test:	; place this code before GetChar
;	ldi	TXout, 0x55
;	ldi	TXcount, 10	; set up tranmission bit count
;	set
;TST1:	brbc    TXActive, test ; clear=SoftUART ready to send
;	rjmp	TST1
;
;;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

;	    MAX232 voltage converter for RS232
;		        __________
;	 	Cap 1 + |1     16| VCC +5
;	  gnd -- Cap2 + |2     15| Ground
;		Cap 1 - |3     14| RS232 out 2
;		Cap 3 + |4     13| RS232 in 2
;	        Cap 3 - |5     12| TTL in 2
;	  gnd -- Cap4 - |6     11| TTL out 2
;  DB9male#3 RS232 out1	|7     10| TTL in 1  (pin 7 Tiny11)
;  DB9male#2 RS232 in1  |8      9| TTL out 1
;			----------
;
;	\ 5  4  3  2  1  /  DB9 	2 in to PC
;	 \		/  male         3 out (ASCII from Tiny11)
;	  \ 9  8  7  6 /  PC serial     4
;          \__________/    port  	5 ground
;
                                                                   