; First AVRmicrocontroller alarm clock program
; TARGET: MEGA8515L
; Copyright (C) 2007-2008 Bo Stout

; To-Do:

; 4)  add am/pm to set time and time update code, or switch to 24hr clock mode
; 6)  re-enamble alarm switch check in time keeping code


.INCLUDE	"m8515def.inc"
.DEVICE 	ATmega8515

; random trash registers; generally get pushed/poped
; NOT USED BY INTERRUPTS - safe local variables for mainline code
; R1, R2, R3, R4, R9, R13, R14, R15

; dirty registers - everything touches it but generally push/pop it
; R16

; extremely dangerous to use - do not use in mainline code
; Z: ZH:ZL - USART subs and interrupts use without saving state
; X: XH:XL - 7SEG interrupt uses without saving state
; R17, R18, R19, R20 - RTC interrupt variables
; R23, R24, R25 - software PWM variables
; R5 - alarm state RTC interrupt variable

.EQU		RTCconst	= 2048			; 32768 for a 32.768kHz RTC that is oscillating properly

; USART for RS232 transfers
.EQU		baud96		= 51			; 9600 baud constant for 8.0 MHz clock
.DEF		TXbusy		= R7			; transmitting character flag
.DEF		savSREG		= R8			; saved status register (interrupt thrashing) needed?
.DEF		RXchar		= R10			; a received character
.DEF		TXflash		= R11			; text to be sent is in flash if <> 0
.DEF		bytecount	= R12			; how many bytes to transmit; dec in interrupt
.DEF		usartdata	= R0			; get trashed by all USART fncs

; DISPLAY MODE - either alarm time or clock time depending on display mode
.DEF		hourone		= R17			; hour, 1's place
.DEF		hourten		= R18			; hour, 10's place
.DEF		minone		= R19			; minute, 1's place
.DEF		minten		= R20			; minute, 10's place

; read pins that are connected to external pushbuttons and switches here
.DEF		buttons		= R21

; software PWM - critial that these be in registers 100% of time during alarm LED ramp
.DEF		swclock		= R23			; white LED channel PWM software clock
.DEF		whitemax	= R24			; software PWM compare value
.DEF		redmax		= R25			; PWM compare value
.DEF		ramp1		= R5			; duration of PWM ramping routine - # of ramp changes to do
.DEF		rampcounter	= R9			; seconds counter, dec and check for 0, then update ramps
.DEF		alarmstate	= R6			; alarm mode on = 1

; R1-R4 mangled by delay macro

; 7-segment updates
.EQU		segdataport	= PORTA			; data output port (all bits)
.EQU		segcnt		= PORTC			; control bits for 7segs
.EQU		seg0pin		= PORTC7		; 7 segment common cathode power pins
.EQU		seg1pin		= PORTC6
.EQU		seg2pin		= PORTC5
.EQU		seg3pin		= PORTC4

.EQU		switchpins	= PINE			; input port for hardware switches
.EQU		timesetpin	= PE0			; pushbutton: toggle set time modes
.EQU		timeincpin	= PE1			; pushbutton: incriment (set time modes)
.EQU		timedecpin	= PE2			; pushbutton: decriment (set time modes)
.EQU		alarmpin	= PC0			; alarm toggle switch: on/off
.EQU		ledport		= PORTC			; status LED's output port
.EQU		setled		= PORTC3		; clock set mode LED
.EQU		pmled		= PORTC2		; AM/PM LED
.EQU		alarmled	= PORTC1		; alarm active/inactive LED
.EQU		colorport	= PORTB			; PWM color output port
.EQU		redpin		= PORTB0		; red LED's software PWM output channel
.EQU		whitepin	= PORTB2		; white LED's software PWM output channel
.EQU		rampcount	= 13			; # of ramp values to cycle thru (DEFAULT DEBUG VALUE)

.EQU		seg0		= 0b01111110	; 7 segment definitions for numerals 0 thru 9
.EQU		seg1		= 0b00001100
.EQU		seg2		= 0b10110110
.EQU		seg3		= 0b10011110
.EQU		seg4		= 0b11001100
.EQU		seg5		= 0b11011010
.EQU		seg6		= 0b11111000
.EQU		seg7		= 0b00001110
.EQU		seg8		= 0b11111110
.EQU		seg9		= 0b11001110
.EQU		dot			= 0b00000001	; ORI this with numeral data to turn dot on



; EEPROM SEGMENT
.ESEG
.ORG 0
; 7 segment bytes to draw numbers 0 thru 9
segs:		.DB	seg0, seg1, seg2, seg3, seg4, seg5, seg6, seg7, seg8, seg9, seg0, dot

; default ramp values
wramps:		.DB 0xFF, 0xD0, 0xC0, 0xA0, 0x90, 0x80, 0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x00, 0x00
rramps:		.DB	0xFF, 0xD0, 0xC0, 0xA0, 0x90, 0x80, 0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x00, 0x00

; rampupdate and other default variable values
defaults:	.DB	10



; SRAM SEGMENT
.DSEG
.ORG 0x60
segtable:	.BYTE	10		; 0x60 to 0x6A

.ORG 0x91
setmode:	.BYTE	1		; 0x91
ahourone:	.BYTE	1
ahourten:	.BYTE 	1
aminone:	.BYTE	1
aminten:	.BYTE	1		; 0x95
apm:		.BYTE	1
chourone:	.BYTE	1
chourten:	.BYTE	1
cminone:	.BYTE	1
cminten:	.BYTE	1
cpm:		.BYTE	1
segupdate:	.BYTE	1		; 0x9C
seconds:	.BYTE	1
pwmtoggle:	.BYTE	1
rampperiod:	.BYTE	1		; 0x9F

.ORG 0xA8
whitedata:	.BYTE	128		; 0xA8 to

.ORG 0xC6
reddata:	.BYTE	128		; 0xC6 to 


; CODE SEGMENT (EEPROM)
; Start of interrupt vector list and reset code
.CSEG
.ORG 0

INTERRUPTS:
	rjmp		RESET				; Power-on and power-reset interrupt
	reti							; INT0
	reti							; INT1
	reti							; Timer1 capture
	rjmp		UPDATETIME			; Timer1 compare A
	rjmp		UPDATE7SEGS			; Timer1 compare B
	reti							; Timer1 overflow
	reti							; Timer0 overflow
	reti							; SPI/STC serial transfer complete
	rjmp		RXdone				; USART receive complete
	rjmp		TXempty				; USART data register is empty
	reti							; USART transfer complete
	reti							; analog comparator
	reti							; external interrupt #2 (connected to watch oscillator)
	rjmp		T0_CMP				; Timer0 compare
	reti							; EEPROM ready
	reti							; store program memory ready

.ORG 0x11
RESET:
	; setup the stack pointer (to end of SRAM)
	ldi		R16, LOW( RAMEND )
	out		SPL, R16
	ldi		R16, HIGH( RAMEND )
	out		SPH, R16

	; PortB: LED software PWM on pins 0 and 2, timer interrupt signal on pin 1, serial programming on 5, 6, 7
	ldi		R16, 0b11111111		; input pull-ups ON, output's set to high
	out		PORTB, R16
	ldi		R16, 0b00000101		; setup pins 0 and 2 on portB for output
	out		DDRB, R16

	; turn off power LED's
	sbi		colorport, redpin
	sbi		colorport, whitepin

	; enable external interrupt #2
	;ldi		R16, 0b00100000
	;out		GICR, R16

	; set external interrupt #2 to trigger on rising edge of waveform
	;ldi		R16, 0b00000001
	;out		EMCUCR, R16

	; configure timer #1: pre-scalar, mode, and compare value
	; intro pg.97, registers pg.119
	ldi		R16, LOW( RTCconst )
	ldi		R17, HIGH( RTCconst )
	out		OCR1AH, R17			; 16bit timer compare, load high first
	out		OCR1AL, R16
	ldi		R16, 128
	ldi		R17, 0
	out		OCR1BH, R17			; compare A controls 7 segment update rate
	out		OCR1BL, R16
	clr		R16
	out		TCCR1A, R16			; setup timer control registers (normal mode)
	ldi		R16, 0b00001111		; external clock source on pin 2 (T1), rising edge trigger
	out		TCCR1B, R16

	; enable timer 1 compare A and compare B interrupts
	ldi		R16, 0b01100000
	out		TIMSK, R16

	; configure timer 0 to CTC mode: reset clock on compare match
	; configure pre-scalar to 1024
	; configure timer compare to 160: this will give 160 steps of granularity in software PWM
	ldi		R16, 96
	out		OCR0, R16
	ldi		R16, 0b00001010
	out		TCCR0, R16

	; setup ports for output to 7 segment LED's
	ser		R16					; set R16 to all 1's
	out		DDRA, R16			; port output mode - 7 SEG DATA PORT

	; button input port (3 pushbuttons)
	ldi		R16, 0b00000111		; pullup resistors on input pins enabled
	out		PORTE, R16
	ldi		R16, 0b00000000
	out		DDRE, R16			; port E is button input port

	; CC power for 7 seg and 3 status LED's cathodes, plus 1 alarm mode switch on port C
	ldi		R16, 0b00000001
	out		PORTC, R16			; pull-up resistors on for alarm input switch (pin 0)
	ldi		R16, 0b11111110		; pins 0 intput, 1-7 outputs (status LED's and CC power for 7 seg)
	out		DDRC, R16

	; turn off all the status LED's
	sbi		ledport, pmled
	sbi		ledport, alarmled
	sbi		ledport, setled


	; reset SRAM to something...
	ldi		R16, 0x11
	ldi		R19, 255
	ldi		XH, 0
	ldi		XL, 0x60
RAMCLEAR:
	st		X+, R16
	dec		R19
	brbc	SREG_Z, RAMCLEAR
	ldi		R19, 255
RAMCLEAR2:
	st		X+, R16
	dec		R19
	brbc	SREG_Z, RAMCLEAR2

	st		X+, R16
	st		X+, R16



	; load EEPROM 7seg data into SRAM
	clr		R16
	clr		R18
	ldi		R19, 10
	ldi		XH, 0
	ldi		XL, 0x60

LOADSRAM:
	out		EEARH, R16
	out		EEARL, R18			; set the EEPROM address register to 0
	sbi		EECR, EERE			; set the read bit in the EEPROM control reg.
	in		R17, EEDR			; get the byte from EEPROM
	st		X+, R17				; store it into SRAM
	inc		R18
	dec		R19
	brbc	SREG_Z, LOADSRAM		; branch if not zero


	; load EEPROM ramp rates into SRAM - white data
	clr		R16
	ldi		R18, wramps
	ldi		R19, rampcount
	ldi		XH, HIGH( whitedata )
	ldi		XL, LOW( whitedata )

LOADSRAM2:
	out		EEARH, R16
	out		EEARL, R18			; set the EEPROM address register to 0
	sbi		EECR, EERE			; set the read bit in the EEPROM control reg.
	in		R17, EEDR			; get the byte from EEPROM
	st		X+, R17				; store it into SRAM
	inc		R18
	dec		R19
	brbc	SREG_Z, LOADSRAM2	; branch if not zero


	; load EEPROM ramp rates into SRAM - red data
	clr		R16
	ldi		R18, rramps
	ldi		R19, rampcount
	ldi		XH, HIGH( reddata )
	ldi		XL, LOW( reddata )

LOADSRAM3:
	out		EEARH, R16
	out		EEARL, R18			; set the EEPROM address register to 0
	sbi		EECR, EERE			; set the read bit in the EEPROM control reg.
	in		R17, EEDR			; get the byte from EEPROM
	st		X+, R17				; store it into SRAM
	inc		R18
	dec		R19
	brbc	SREG_Z, LOADSRAM3	; branch if not zero

	; clock software initialization
	clr		R16
	sts		segupdate, R16		; reset the 7 seg update counter

	cbi		segcnt, seg0pin		; enable the first 7seg power (common cathode)
	sbi		segcnt, seg1pin		; disable the other 3
	sbi		segcnt, seg2pin
	sbi		segcnt, seg3pin

	; load some init time into the clock
	ldi		minone, 5
	ldi		minten, 5
	ldi		hourone, 1
	ldi		hourten, 1
	ldi		R16, 0
	sts		seconds, R16

	; store clock into SRAM, also store some default alarm values into SRAM
	sts		chourone, hourone
	sts		chourten, hourten
	sts		cminone, minone
	sts		cminten, minten
	ldi		R16, 1
	sts		ahourten, R16
	ldi		R16, 2
	sts		ahourone, R16
	ldi		R16, 0
	sts		aminten, R16
	ldi		R16, 2
	sts		aminone, R16
	clr		R16
	sts		cpm, R16			; 0 = AM, 1 = PM in bit # 0, just setting them all for ease
	ser		R16
	sts		apm, R16

	; set time state: 0 to 4: 0 = not in set mode, 1 = set hour,
	; 2 = set minute, 3 = set alarm hour, 4 = set alarm minute
	clr		R16
	sts		setmode, R16			; set to 0, "not in set mode"
	ldi		R16, 1
	sts		rampperiod, R16			; seconds between ramp updates
									; should load this from EEPROM default value

	ldi		R16, 0xDD
	sts		0x1C0, R16


; USART INITIALIZATION
; ----------------------------------------------------------------------------------------------
	;initial conditions
	clr		TXbusy					; start out not busy on TX
	clr		R16
	mov		RXchar, R16

	; setup USART: RX complete interrupt enabled
	; TX data register is empty interrupt enabled
	; enable RX and TX modes (pins)
	ldi 	R16, 0b10111000
	out 	UCSRB, R16

	;set baud rate to 9600
	clr		R16
	out		UBRRH, R16
	ldi		R16, baud96
	out		UBRRL, R16

	; asynchronous mode, no parity bit, 1 stop bit, 8 data bits
	ldi		R16, 0b10000110
	out		UCSRC, R16

	; RESET causes the TX empty interrupt to be SET - clear it
	cbi		UCSRB, UDRIE
; ----------------------------------------------------------------------------------------------

	rjmp	INITSYS



; DEBOUNCE MACRO
; trashes R16
; @0: # of cycles to delay (0-255)
; @1: witch pin port to get input from (PINA, PINB, PINC,...)
; @2: which register to store results into (R17,R18,...)
.MACRO debounce
DEBOUNCE0:
	push	R16
	ldi		R16, @0
DEBOUNCE1:
	dec		R16
	cpi		R16, 0
	brne	DEBOUNCE1

	in		@2, @1			; get the button pins
	cpi		@2, 0xFF
	breq	DEBOUNCE3		; no pins went low

	com		R16
	com		@2
	and		@2, R16			; button must be held down for xxx delay clock cycles
	cpi		@2, 0
	breq	DEBOUNCE3		; no buttons remained down
	rjmp	DEBOUNCE4
DEBOUNCE3:
	clr		buttons
	pop		R16
	rjmp	MAIN
DEBOUNCE4:
	pop		R16
.ENDMACRO

; MILLISECOND DELAY MACRO
; @0: # of milliseconds to delay (approx., depends on clock speed)
; IMPROVEMENT: use a scalar to get it closer to real milliseconds
.MACRO delayms
DELAY0:
	push	R16

	ldi		R16, HIGH( @0 )
	mov		R1, R16

DELAY4:
	ldi		R16, LOW( @0 )
	mov		R2, R16

DELAY3:
	ldi		R16, 89
	mov		R3, R16

DELAY2:
	ldi		R16, 16
	mov		R4, R16

DELAY1:
	dec		R4
	mov		R16, R4
	cpi		R16, 0
	brne	DELAY1

	dec		R3
	mov		R16, R3
	cpi		R16, 0
	brne	DELAY2

	dec		R2
	mov		R16, R2
	cpi		R16, 0
	brne	DELAY3

	dec		R1
	mov		R16, R1
	cpi		R16, 0
	brne	DELAY4

	pop		R16
.ENDMACRO
	rjmp	MAIN



INITSYS:
	delayms 3000						; wait for RTC to settle down, takes cpl seconds
	clr		R16
	mov		alarmstate, R16				; reset the alarm state
	
	sei									; enable interrupts

	ldi		ZL, LOW( resetmsg << 1 )	; send welcome message
	ldi		ZH, HIGH( resetmsg << 1 )
	rcall	SEND_STRING

MAIN:
	; get switch status, with debounce delay of 255 clock cycles
	; output into buttons register, 1 = button down, 0 = button not pressed
	debounce 255, switchpins, buttons

	sbrc	buttons, timesetpin		; if set mode button pressed jump to set mode routine
	rjmp	SMODE
	sbrc	buttons, timeincpin		; if inc time button pressed jump to settime
	rjmp	SETTIME
	sbrc	buttons, timedecpin		; id dec time button pressed jump to settime
	rjmp	SETTIME

	sbis	switchpins, alarmpin	; if alarm switch is ON (LOW) light up LED
	cbi		ledport, alarmled
	sbic	switchpins, alarmpin	; if alarm switch is OFF (HIGH) turn off LED
	sbi		ledport, alarmled

	mov		R16, RXchar
	cpi 	R16, 'i'			; send intro text message with line feed
	breq	COMMAND_I
	cpi		R16, 'r'			; dump contents of SRAM in binary form
	breq	COMMAND_R
	cpi		R16, 'e'			; dump contents of EEPROM in binary form
	breq	COMMAND_E
	cpi		R16, 'k'			; write xx to location xx in SRAM (or general purpose register)
	breq	COMMAND_K
	cpi		R16, 'j'			; write xx to location xx in EEPROM
	breq	COMMAND_J
	cpi		R16, 'l'			; write xx to I/O port xx
	breq	COMMAND_L
	cpi		R16, 't'			; upload alarm time
	breq	COMMAND_T
	cpi		R16, 'y'			; upload clock time
	breq	COMMAND_Y
	cpi		R16, 'u'			; upload general purpose registers to PC
	breq	COMMAND_U
	cpi		R16, 'o'			; upload I/O registers to PC
	breq	COMMAND_O
	cpi		R16, 'p'			; toggle PWM interrupt (timer 0 compare)
	breq	COMMAND_P
	rjmp	MAIN

; EEPROM contents -> USART in binary form without using USART interrupt
COMMAND_E:
	rjmp	DUMP_EEPROM
	clr		RXchar
	rjmp	MAIN

; write X3 to EEPROM location X2:X1
COMMAND_J:
	rcall	SET_EEPROM_BYTE
	clr		RXchar
	rjmp	MAIN

; write a byte into SRAM from loc and data vars from USART
COMMAND_K:
	rcall	SET_SRAM_BYTE
	clr		RXchar
	rjmp	MAIN

COMMAND_L:
	rcall	SET_IO_BYTE
	clr		RXchar
	rjmp	MAIN

; contents of ram (512 bytes) to USART in binary form
COMMAND_R:
	rcall	DUMP_SRAM
	clr		RXchar
	rjmp	MAIN

; intro message text to USART
COMMAND_I:
	ldi		ZL, LOW( intromsg << 1 )
	ldi		ZH, HIGH( intromsg << 1 )
	rcall	SEND_STRING
	clr		RXchar
	rjmp	MAIN

; send clock alarm time
COMMAND_T:
	ldi		ZL, 0x92
	ldi		ZH, 0
	ldi		R16, 4
	mov		bytecount, R16
	rcall	SEND_RAM

	clr		RXchar
	rjmp	MAIN

; send clock time
COMMAND_Y:
	ldi		ZL, 17
	ldi		ZH, 0
	ldi		R16, 4
	mov		bytecount, R16
	rcall	SEND_RAM

	clr		RXchar
	rjmp	MAIN

; send general purpose registers
COMMAND_U:
	ldi		ZL, 0
	ldi		ZH, 0
	ldi		R16, 32
	mov		bytecount, R16
	rcall	SEND_RAM

	clr		RXchar
	rjmp	MAIN

; send I/O registers
COMMAND_O:
	ldi		ZL, 0x20
	ldi		ZH, 0
	ldi		R16, 64
	mov		bytecount, R16
	rcall	SEND_RAM

	clr		RXchar
	rjmp	MAIN

; toggle PWM interrupt
COMMAND_P:
	mov		R16, alarmstate
	cpi		R16, 1
	brne	TOGGLE
	rjmp	CMDP_EXIT			; if the alarm is running do not allow toggling
TOGGLE:
	lds		R16, pwmtoggle
	cpi		R16, 0
	breq	TOGGLEOFF
	clr		R16
	sts		pwmtoggle, R16
	in		R16, TIMSK
	sbr		R16, 0b00000001
	out		TIMSK, R16			; toggle the PWM interrupt on
	rjmp	CMDP_EXIT
TOGGLEOFF:
	ser		R16
	sts		pwmtoggle, R16
	in		R16, TIMSK
	cbr		R16, 0b00000001
	out		TIMSK, R16			; toggle the PWM interrupt off
	cbi		colorport, redpin	; turn the LED's off
	cbi		colorport, whitepin	
CMDP_EXIT:
	clr		RXchar
	rjmp	MAIN



; set time button pressed
SMODE:
	lds		R16, setmode			; load the set time state
	inc		R16
	cpi		R16, 5
	brne	SETMODE1
	clr		R16
SETMODE1:
	cbi		ledport, setled		; turn on the set-mode LED (active low wiring)
	delayms	750					; delay 1 second to allow user to remove finger from button without flashing through all states too fast
	sts		setmode, R16		; save the new time set mode back to memory
	cpi		R16, 3
	breq	COPYALARM
	cpi		R16, 0
	breq	SETTIMEEND
	rjmp	MAIN
COPYALARM:
	sts		chourone, hourone	; mode changed from set time to set alarm, save the registers back to the time memory slots
	sts		chourten, hourten
	sts		cminone, minone
	sts		cminten, minten
	lds		hourone, ahourone	; now load the alarm values from memory slots into registers for editing
	lds		hourten, ahourten
	lds		minone, aminone
	lds		minten, aminten
	rjmp	MAIN
SETTIMEEND:
	sts		ahourone, hourone	; changed from set alarm mode to exit set time mode (0), save the alarm data and re-enable interrupts
	sts		ahourten, hourten
	sts		aminone, minone
	sts 	aminten, minten
	lds		hourone, chourone
	lds		hourten, chourten
	lds		minone, cminten
	lds		minten, cminten
	sbi		ledport, setled
	rjmp	MAIN



; either INC or DEC button was pressed
SETTIME:
	delayms	750
	lds		R16, setmode		; get the set mode
	cpi		R16, 0
	breq	JMAIN0				; if set mode = 0 (not in set mode) back to main we go to poll some more buttons
	sbrc	buttons, timeincpin
	rjmp	INCTIME
	sbrc	buttons, timedecpin
	rjmp	DECTIME
JMAIN0:
	rjmp	MAIN
INCTIME:
	cpi		R16, 1
	breq	INCHOUR
	cpi		R16, 2
	breq	INCMINUTE
	cpi		R16, 3
	breq	INCHOUR
	cpi		R16, 4
	breq	INCMINUTE
	rjmp	MAIN				; shouldn't make it here...
DECTIME:
	cpi		R16, 1
	breq	DECHOUR
	cpi		R16, 2
	breq	DECMINUTE
	cpi		R16, 3
	breq	DECHOUR
	cpi		R16, 4
	breq	DECMINUTE
	rjmp	MAIN				; shouldn't make it here...
DECHOUR:
	cpi		hourone, 0
	breq	DECHOURTEN
	dec		hourone
	cpi		hourone, 0
	breq	DECHOURB
	rjmp	MAIN
DECHOURB:
	cpi		hourten, 0
	breq	DECHOURC
	rjmp	MAIN
DECHOURC:
	ldi		hourten, 1
	ldi		hourone, 2
	rjmp	MAIN
DECHOURTEN:
	cpi		hourten, 0
	breq	SETHOUR12
	clr		hourten
	ldi		hourone, 9
	rjmp	MAIN
SETHOUR12:
	ldi		hourten, 1
	ldi		hourone, 2
	rjmp	MAIN
INCHOUR:
	cpi		hourone, 2
	breq	CHECKHOURTEN
	cpi		hourone, 9
	breq	INCHOURTEN
	inc		hourone
	rjmp	MAIN
CHECKHOURTEN:
	cpi		hourten, 1
	breq	SETHOUR01
	inc		hourone
	rjmp	MAIN
SETHOUR01:
	ldi		hourten, 0
	ldi		hourone, 1
	rjmp	MAIN
INCHOURTEN:
	clr		hourone
	ldi		hourten, 1
	rjmp	MAIN
DECMINUTE:
	cpi		minone, 0
	breq	DECMINUTE10
	dec		minone
	rjmp	MAIN
DECMINUTE10:
	cpi		minten, 0
	breq	SETMINUTE59
	dec		minten
	ldi		minone, 9
	rjmp	MAIN
SETMINUTE59:
	ldi		minten, 5
	ldi		minone, 9
	rjmp	MAIN
INCMINUTE:
	cpi		minone, 9
	breq	INCMIN10
	inc		minone
	rjmp	MAIN
INCMIN10:
	cpi		minten, 5
	breq	SETMIN00
	inc		minten
	clr		minone
	rjmp	MAIN
SETMIN00:
	clr		minone
	clr		minten
	rjmp	MAIN



; ----------------------------------------------------------------------------------------------
; INTERRUPT
; update 7 segment displays
; update OCR1B (timer 1 compare B value) to create a flicker-free display update
UPDATE7SEGS:
	in		savSREG, SREG
	push	R16

	clr		XH
	ldi		XL, 0x60
	lds		R16, segupdate
	inc		R16
	cpi		R16, 4
	brne	SEG0X
	clr		R16
SEG0X:
	sts		segupdate, R16
	cpi		R16, 1
	breq	SEG1X
	cpi		R16, 2
	breq	SEG2X
	cpi		R16, 3
	breq	SEG3X
	add		XL, hourten
	ld		R16, X
	cbi		segcnt, seg0pin
	out		segdataport, R16
	sbi		segcnt, seg3pin
	rjmp	SEGTIMER
SEG1X:
	add		XL, hourone
	ld		R16, X
	cbi		segcnt, seg1pin
	out		segdataport, R16
	sbi		segcnt, seg0pin
	rjmp	SEGTIMER
SEG2X:
	add		XL, minten
	ld		R16, X
	cbi		segcnt, seg2pin
	out		segdataport, R16
	sbi		segcnt, seg1pin
	rjmp	SEGTIMER
SEG3X:
	add		XL, minone
	ld		R16, X
	cbi		segcnt, seg3pin		; remove power from segment 3
	out		segdataport, R16	; send the 7 segment data out to the display
	sbi		segcnt, seg2pin		; apply power to common cathode of segment 0
SEGTIMER:
	push	R13
	push	R14
	in		R13, OCR1BL
	in		R14, OCR1BH
	ldi		R16, 156
	add		R13, R16
	clr		R16
	adc		R14, R16
	out		OCR1BH, R14
	out		OCR1BL, R13
	pop		R14
	pop		R13
	pop		R16
	out		SREG, savSREG
	reti



; ----------------------------------------------------------------------------------------------
; INTERRUPT
; Update RTC variables
; Check alarm status - start ramps and enable software PWM interrupt
; Update PWM ramp values when alarm is active
UPDATETIME:
	push	R16

	; update AM/PM status LED state
	lds		R16, cpm
	sbrc	R16, 0				; skip if bit clear
	cbi		ledport, pmled		; PM LED ON
	sbrs	R16, 0				; skip if bit set
	sbi		ledport, pmled		; PM LED OFF


	clr		R16
	out		OCR1BH, R16
	ldi		R16, 96
	out		OCR1BL, R16

	lds		R16, setmode			; check to see if clock is being set - exit interrupt if so
	cpi		R16, 0
	breq	NOTSETMODE
	pop		R16
	reti
NOTSETMODE:
	lds		R16, seconds
	inc		R16
	cpi		R16, 60
	breq	MIN_UPDATE
	sts		seconds, R16
	pop		R16
	reti
MIN_UPDATE:
	clr		R16
	sts		seconds, R16
	inc		minone
;---------------------------------------------------------------------------------------------------------------
	;sbis	switchpins, alarmpin		; check to see if alarm switch is ON (LOW)
	;rjmp	NOALARM						; if it's not just jump over time checks
;---------------------------------------------------------------------------------------------------------------
	mov		R16, alarmstate
	cpi		R16, 1
	brne	CONTINUETIME				; if alarm flag is set do some PWM ramps, don't bother checking alarm against time
	rjmp	PWMRAMPS
CONTINUETIME:
	lds		R16, aminone				; bunch of checks for all 4 time variables
	cp		minone, R16					; against alarm values stored in SRAM
	brne	FINISHTIME
	lds		R16, aminten
	cp		minten, R16
	brne	FINISHTIME
	lds		R16, ahourone
	cp		hourone, R16
	brne	FINISHTIME
	lds		R16, ahourten
	cp		hourten, R16
	brne	FINISHTIME
	lds		R16, apm		; check for AM/PM match
	cpi		R16, 0
	breq	ALARMPM0
	lds		R16, cpm
	sbrc	R16, 0
	rjmp	FINISHTIME
	rjmp	ALARMACTIVE
ALARMPM0:
	lds		R16, cpm
	sbrs	R16,0
	rjmp	FINISHTIME
	rjmp	ALARMACTIVE
FINISHTIME:								; alarm OFF or time doesn't match alarm
	cpi		minone, 10
	breq	MINTENUPDATE
	pop		R16
	reti
MINTENUPDATE:
	clr		minone
	inc		minten
	cpi		minten, 6
	breq	HOUR_UPDATE
	pop		R16
	reti
HOUR_UPDATE:
	clr		minten
	inc		hourone
	cpi		hourten, 1
	breq	HOURONE_1
	cpi		hourone, 10			; roll ones place to 10's place
	breq	HOURONE_3
	pop		R16
	reti
HOURONE_1:
	cpi		hourone, 2			; we just rolled to 12:00; toggle the PM variable
	lds		R16, cpm
	cpi		R16, 0
	breq	SETPM
	clr		R16
	sts		cpm, R16
SETPM:
	ser		R16
	sts		cpm, R16
	cpi		hourone, 3			; if hour10 = 1 & hour1 = 3 roll time from 12 to 01
	breq	HOURONE_2
	pop		R16
	reti
HOURONE_2:
	ldi		hourone, 1
	clr		hourten
	pop		R16
	reti
HOURONE_3:
	clr		hourone
	inc		hourten
	cpi		hourten, 2			; hour10 0 -> 1 O.K., hour10 1 -> 2 NO! roll time to 01
	breq	HOURONE_4
	pop		R16
	reti
HOURONE_4:
	clr		hourten
	pop		R16
	reti



; ----------------------------------------------------------------------------------------------
; fragment code - part of INTERRUPT to update RTC data
; Update PWM ramps - called from time update interrupt
; Dec the countdown timers.  Shut off alarm mode when they reach 0
; Go back and finish the time update
PWMRAMPS:
	dec		ramp1
	mov		R16, ramp1
	cpi		R16, 0
	brne	PWMXX0
	in		R16, TIMSK
	cbr		R16, 0b00000001		; turn off timer 0 compare
	out		TIMSK, R16
	sbi		colorport, redpin	; turn off LED pins (high=off)
	sbi		colorport, whitepin
	clr		alarmstate
	rjmp	FINISHTIME
PWMXX0:
	ldi		XH, HIGH( whitedata )
	ldi		XL, LOW( whitedata )		; start of ramp memory for white
	add		XL, ramp1					; offset by ramp1 amount (0 <= ramp1 <= ~30)
	ld		R16, X
	mov		whitemax, R16
	ldi		XH, HIGH( reddata )
	ldi		XL, LOW( reddata )
	add		XL, ramp1
	ld		R16, X
	mov		redmax, R16
	rjmp	FINISHTIME



; ----------------------------------------------------------------------------------------------
; fragment code - part of INTERRUPT to update RTC data
; Alarm mode just activated from time compare
; Load initial ramps and enable PWM interrupt
; Load countdown counters with initial values
; Change the alarm state flag to 1
ALARMACTIVE:
	;load the count down counters for the alarm mode (duration of PWM ramping)
	ldi		R16, rampcount
 	mov		ramp1, R16

	; load the first LED ramp values
	lds		R16, whitedata
	mov		whitemax, R16
	lds		R16, reddata
	mov		redmax, R16

	; enable the PWM interrupt
	in		R16, TIMSK
	sbr		R16, 0b00000001
	out		TIMSK, R16

	; set the alarm state flag
	ldi		R16, 1
	mov		alarmstate, R16

	; bitbang a msg over the USART connection
	ldi		ZL, LOW( alarmtext << 1 )
	ldi		ZH, HIGH( alarmtext << 1 )
	lpm		bytecount, Z+
	lpm		R0, Z+
ALRMWAIT:
	sbis	UCSRA, UDRE				; skip the jump if UDRE bit is set (empty data register)
	rjmp	ALRMWAIT
	out		UDR, R0					; send it out the USART
	lpm		R0, Z+
	dec		bytecount
	brbc	SREG_Z, ALRMWAIT		; branch if not zero

GETOUT:
	pop		R16
	reti



; ----------------------------------------------------------------------------------------------
; INTERRUPT: timer 0, compare
; inc software PWM clocks, check for compares
; On software clock compare matches turn off LED channel (HIGH)
T0_CMP:
	in		savSREG, SREG
	inc		swclock
	cp		swclock, whitemax
	breq	WHITECOMPARED
	cp		swclock, redmax
	breq	REDCOMPARED
	cpi		swclock, 128
	breq	RESETSWCLOCK
	out		SREG, savSREG
	reti
WHITECOMPARED:
	cbi		colorport, whitepin
	cp		swclock, redmax
	breq	REDCOMPARED
	out		SREG, savSREG
	reti
REDCOMPARED:
	cbi		colorport, redpin
	out		SREG, savSREG
	reti
RESETSWCLOCK:
	clr		swclock
	sbi		colorport, redpin
	sbi		colorport, whitepin
	out		SREG, savSREG
	reti



; ----------------------------------------------------------------------------------------------
; INTERRUPT
; USART sends bytes pointed to by Z+ until register bytecount-- is 0
TXempty:
	push	R16
	in		savSREG, SREG	; save processor status
	tst		TXflash			; Is the string in flash memory?
	breq	TXram			; If not, it is in RAM
	lpm		R0, Z+			; and put it in R0
	rjmp	TXfls
TXram: 
	ld		R0,Z+			; get the next char from RAM
TXfls:
	out 	UDR, R0
	dec		bytecount
	brbs	SREG_Z, TXend
	rjmp	TXexit			; exit until next char
TXend:
	clr		R16
	mov		TXbusy, R16		; no more chars
	cbi		UCSRB, UDRIE	; clear the TXempty interrupt
TXexit:
	out		SREG, savSREG	; restore proc status
	pop		R16
	reti					; back to pgm



; ----------------------------------------------------------------------------------------------
; INTERRUPT
; USART read one byte and store into register
RXdone:
	in		savSREG, SREG	; save processor status	
	in		RXchar, UDR		; get the character
	out		SREG, savSREG	; restore proc status
	reti					; back to pgm



; ----------------------------------------------------------------------------------------------
; SUBROUTINE
; wait until the USART is finished transmitting a byte
TXwait:
	tst		TXbusy		;now wait for the tranmission to finish
	brne	TXwait
	ret



; ----------------------------------------------------------------------------------------------
; SUBROUTINE
; send a string from CSEG to USART
; string first byte is length of string in bytes
SEND_STRING:
	lpm		bytecount, Z+			; put the byte count into the proper register and move to first char of msg
	lpm		R0, Z+					; load first char into R0 and incriment address
	out		UDR, R0					; put the character in the USART buffer
	ser		R16
	mov		TXflash, R16			; string is in flash program memory
	mov		TXbusy, R16				; and set the TX busy flag
	sbi		UCSRB, UDRIE			; enable the TXempty interrupt
	rcall	TXwait					; chill until done
	
	;setup ptr for <crlf> string
	ldi		ZL, LOW( crlf << 1 )  	; shifted becuase pgm memory is words
	ldi		ZH, HIGH( crlf << 1 )
	lpm		bytecount, Z
	inc		ZL
	lpm
	out		UDR, R0					; trigger the USART TX
	dec		bytecount
	ser		R16
	mov		TXflash, R16			; text string in flash program memory
	mov		TXbusy, R16				; and set the TX busy flag
	sbi		UCSRB, UDRIE			; enable the TXempty interrupt
	rcall	TXwait
	ret



; ----------------------------------------------------------------------------------------------
; SUBROUTINE
; sends byte(s) from DSEG to USART.  # of bytes controlled by bytecount register
; Z = start of data to send, bytecount = # of bytes to send
SEND_RAM:
	clr		R16						; R16 = 0x00
	mov		TXflash, R16			; data is in SRAM data memory
	ser		R16
	mov		TXbusy, R16				; and set the TX busy flag
	sbi		UCSRB, UDRIE			; enable the TXempty interrupt
	rcall	TXwait					; chill until done
	ret



; ----------------------------------------------------------------------------------------------
; SUBROUTINE
; sends byte(s) from ESEG to USART.  # of bytes controlled by bytecount register
; R1:R2 = start of data pointer, bytecount = # of bytes to send
SEND_EEPROM:
	cli

SEND_EEPROM2:
	out		EEARH, R1
	out		EEARL, R2			; set the EEPROM address register to 0
	sbi		EECR, EERE			; set the read bit in the EEPROM control reg.
	in		R0, EEDR			; get the byte from EEPROM
WAIT_TX:
	sbis	UCSRA, UDRE			; skip the jump if UDRE bit is set (empty data register)
	rjmp	WAIT_TX
	out		UDR, R0				; send it out the USART
	inc		R2
	dec		bytecount
	brbc	SREG_Z, SEND_EEPROM2		; branch if not zero

	sei
	ret



; ----------------------------------------------------------------------------------------------
; SUBROUTINE
; write X3 to SRAM location X2:X1 where X1-X3 are the next 3 USART input bytes
SET_SRAM_BYTE:
	cli

SSBWAIT:
	sbis	UCSRA, RXC				; wait for the next byte
	rjmp	SSBWAIT
	in		ZL, UDR
SSB2:
	sbis	UCSRA, RXC
	rjmp	SSB2
	in		ZH, UDR
SSB3:
	sbis	UCSRA, RXC
	rjmp	SSB3
	in		R0, UDR
	st		Z, R0

	sei
	ret



; ----------------------------------------------------------------------------------------------
; SUBROUTINE
; write X3 to EEPROM location X2:X1 where X1-X3 are the next 3 USART input bytes
SET_EEPROM_BYTE:
	cli
	push	R1
	push	R2

SEBWAIT1:					; wait for any ongoing EEPROM writes
	sbic	EECR, EEWE
	rjmp	SEBWAIT1

;SEBWAIT2:
;	sbic	SPMCR, SPMEN	; wait for any ongoing SRAM writes
;	rjmp	SEBWAIT2

SEBWAIT3:					; get low byte of address
	sbis	UCSRA, RXC
	rjmp	SEBWAIT3
	in		R1, UDR
SEB2:						; get high byte of address
	sbis	UCSRA, RXC
	rjmp	SEB2
	in		R2, UDR
SEB3:						; get data
	sbis	UCSRA, RXC
	rjmp	SEB3
	in		R0, UDR
	
	out		EEARH, R2		; high byte part of address
	out		EEARL, R1		; low byte part of address
	out		EEDR, R0		; data to write
	sbi		EECR, EEMWE		; allow EEPROM to be written to
	sbi		EECR, EEWE		; start the write

	pop		R2
	pop		R1
	sei
	ret



; ----------------------------------------------------------------------------------------------
; SUBROUTINE
; write X2 to I/O REGISTER location X1 (0-63) where X1-X2 are the next 2 USART input bytes
; only a few IO registers are allowed for writting, using conditional branches
; (there is no way of indirectly addressing IO registers with variable pointers)
SET_IO_BYTE:
	cli
	push	R2
	
	sbis	UCSRA, RXC
	rjmp	SET_IO_BYTE
	in		R2, UDR
SIB2:
	sbis	UCSRA, RXC
	rjmp	SIB2
	in		R0, UDR

	;compare and branch or skip instructions to write out to specific ports
	ldi		R16, PORTB
	cpse	R16, R2			; skip next instruction if R2 = PORTB
	rjmp	IO2				; weren't equal, so check next
	out		PORTB, R0		; were equal so output and return
	rjmp	SIEXIT
IO2:
	ldi		R16, PORTA
	cpse	R16, R2
	rjmp	IO3
	out		PORTA, R0
	rjmp	SIEXIT
IO3:
	ldi		R16, PORTC
	cpse	R16, R2
	rjmp	IO4
	out		PORTC, R0
	rjmp	SIEXIT
IO4:
	ldi		R16, PORTD
	cpse	R16, R2
	rjmp	IO5
	out		PORTD, R0
	rjmp	SIEXIT
IO5:
	ldi		R16, PORTE
	cpse	R16, R2
	rjmp	IO6
	out		PORTE, R0
	rjmp	SIEXIT
IO6:

SIEXIT:
	pop		R2
	sei
	ret



; ----------------------------------------------------------------------------------------------
; SUBROUTINE
; copy contents of EEPROM to USART output
DUMP_EEPROM:
	push	R1
	push	R2

	clr		R1
	clr		R2
	ldi		R16, 255
	mov		bytecount, R16
	rcall	SEND_EEPROM

	inc		bytecount
	rcall	SEND_EEPROM

	ldi		R16, 1
	mov		R1, R16
	clr		R2
	ldi		R16, 255
	mov		bytecount, R16
	rcall	SEND_EEPROM

	inc		bytecount
	rcall	SEND_EEPROM

	pop		R2
	pop		R1
	ret



; ----------------------------------------------------------------------------------------------
; SUBROUTINE
; copy contents of SRAM to USART output
DUMP_SRAM:
	ldi		ZL, 0x60
	ldi		ZH, 0
	ldi		R16, 255
	mov		bytecount, R16
	rcall	SEND_RAM

	ldi		R16, 255
	mov		bytecount, R16
	rcall	SEND_RAM

	ldi		R16, 2
	mov		bytecount, R16
	rcall	SEND_RAM
	ret



; define fixed strings to be tranmitted from flash
; must pad to integral word (16bit) lengths
crlf: .db	2, 0x0d, 0x0a, 0x00							; carrage return/line feed, zero-terminated, padded to WORD
intromsg: .db 37, "connected to Sunrise Alarm Clock v0.300", 0x00, 0x00	; reset/pwr connection msg
resetmsg: .db 19, "Sunrise Clock reset00", 0x00, 0x00
debugflag:	.db 10, "debug flag00", 0x00
alarmtext:	.db 14, "alarm ACTIVE", 0x0d, 0x0a, 0x00
