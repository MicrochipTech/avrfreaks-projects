;************************************************
;* RC5 Code Transmitter
;*
;* Chip Number 		: U1
;* File Name 		: Program 019M4 RC5 sender.asm
;* Date				: 12.13.2005
;* Last Update		: 12.13.2005
;* Version			: 4.0
;* Support Telephone: withheld
;* Support Fax		: withheld
;* Support E-mail	: digitan000@yahoo.com
;* Target MCU		: AT90S2313-4PC
;*
;* DESCRIPTION
;*
;* This program has the "TRANSMIT_RC5" routine which is used to 
;* send RC5 frames at 37.037kHz and ~562 bits per second.  The AVR will 
;* have two outputs.  One is the Infrared signal with the IR
;* carrier; the other is a no-carrier signal that is useful for
;* debugging.  There is a 65.536ms minimum delay between RC5 frames
;* with this code.
;*
;* The "TRANSMIT_RC5" routine will operate without modification with
;* a 4.00MHz clock source.
;*
;* PROGRAM THEORY OF OPERATOIN
;* 
;* After setup, the AVR enters the power-down state.  A low-level
;* voltage on the IRQ0 pin will awaken the AVR, and the program
;* will capture the lower 4 bits on Port B after the IRQ0 port
;* returns to 5 Volts.  These 4 bits make up the IR command that
;* will be sent.  This code sets the IR group to zero, but this 
;* can be easily edited.
;*
;* The main routine is TRANSMIT_RC5, which provides the IR signal.
;*
;* PORT USAGE SUMMARY:
;* PB.0 	Keypad input bit 0 (LSB)
;* PB.1 	Keypad input bit 1
;* PB.2 	Keypad input bit 2
;* PB.3 	Keypad input bit 4 (MSB)
;* PB.4 	n/c
;* PB.5 	RC5 output to IR (carrier at 36kHz)
;* PB.6 	n/c
;* PB.7 	Debugging output (RC5 with no carrier)
;* PORT D
;* PD.0 	n/c
;* PD.1 	n/c
;* PD.2 	"Data Available" from key decoder <controls INT0>
;* PD.3 	n/c
;* PD.4 	n/c
;* PD.5 	n/c
;* PD.6 	n/c
;* PD.7 	does not exist
;*************************************************	
;.device AT90S2313

.nolist							; Stop listfile generation
.include "2313def.inc"			; Include definiation file
.list							; Resume listfiles

;- Register Aliases -----
;.def					= R16	; General-purpose register by tradition
.def	loop			= R17	; Loop/increment counter
.def	input			= R18	; PortB input states (4 bits)
.def	parity			= R19	; RC5/MONO PARITY
.def	group 			= R20	; RC5/MONO GROUP #ID
.def	data  			= R21	; RC5/MONO DATA or COMMAND
;- Define Constants -----
.equ	ir_pin			= 5			; Location of IR LED on port B
.equ	debug_pin		= 7			; Location of hardwired link on port B
;- Macros ---------------
.macro ir_high
	sbi PORTB,ir_pin	; Raise IR output to 5V
.endmacro

.macro ir_low
	cbi PORTB,ir_pin	; Lower IR output to ground/0V
.endmacro

.macro debug_high
	sbi PORTB,debug_pin	; Raise test output to 5V
.endmacro

.macro debug_low
	cbi PORTB,debug_pin	; Lower test output to ground/0V
.endmacro
;------------------------

;########################
;# Flash Memory			#
;########################
.cseg					; Code Segment ; Begin Flash
.org 0x00				; Reset vector

	rjmp main			;Jump over remaining interrupt vectors
;########################
;# Interrupt Vectors	#
;########################
	RETI		;Int0 Interrupt
	RETI		;Int1 Interrupt
	RETI		;TC1-Capture Event
	RETI		;TC1-Compare Match
	RETI		;TC1-Overflow
	RETI		;TC0-Overflow
	RETI		;UART Receive Transfer Complete
	RETI		;UART Data Register Empty
	RETI		;UART Transmit complete
	RETI		;Analog Comparator
;########################
;# End Interrupt Vectors#
;########################
main:
;---Port Direction Setup-----
	ldi R16,0b11100000		; :: PORT B DIRECTION SETUP
	out DDRB,R16			; Send configuration

	clr R16					; ::: PORT D DIRECTION SETUP
	out DDRD,R16			; Send config ; All pins in input mode
;---Port Initial Level Setup-
	ldi R16,0b00101111		; ::: PORT B INITIALIZATION
	out PORTB,R16			; Inputs have pull-up resistors

	clr R16					; ::: PORT D INITIALIZATION
	out PORTD,R16			; Float all inputs
;---Stack Setup--------------
	ldi R16,(RAMEND)		; Begin stack at end-of-ram
	out SPL,R16				; RAMEND is address 0xDF for AVR2313
;---Set Transmitter's Group	;
	SEI						; Global interrupt enable
STANDBY:					; ::: LOW-POWER MODE
;---Reset Timer from last xmit
	clr R16					; R16 = 0
	out TCCR0,R16			; Stop Timer
	out TCNT0,R16			; Reset Timer
;---Enable power-down state--
	ldi R16,(1<<INT0)		; ::: EXTERNAL INTERRUPT 0 ENABLE
	out GIMSK,R16			; Int0 ISR code is enabled
	ldi R16,(1<<SE)+(1<<SM)	; Set "Sleep Enable" | Set "Sleep Mode"
	out MCUCR,R16			; Sleep on INT0 low level.  Power down mode is sleep mode
;---Entering power-down mode-
	sleep 					; Micro-power state. Awaken to IRQ0
;---Wakeup, disable sleep----
	clr R16					; ::: EXTERNAL INTERRUPT 0 ENABLE
	out GIMSK,R16			; Int0 ISR code is disabled
	clr R16					; R16 = 0
	out MCUCR,R16			; Disable sleep mode immidiatly
keyget:
	lds input,PINB + $20	; Capture Get pin binary status
	andi input,0b00001111	; Mask off non-keys
keystop:
	lds R16,PIND + $20		; Load PortD (Data Availible/Int0) status
	sbrs R16,2				; "RJMP" until the key is released
	rjmp keystop	
transmit:
	CLI						; No interrupts to affect timing
	mov data,input			; Setup RC5 Command/Data
	ldi group,0x00			; Example: Transmit to group 0x00
	rcall TRANSMIT_RC5		; Transmit one RC5 frame
	SEI						; Re-enable interrupts
timeout:
;---Idle for 65ms------------
	ldi R16,(1<<CS02)+(1<<CS00)	; Prescale: 4.00MHz/1024
	out TCCR0,R16				; Overflow: 65.536ms
	ldi R16,(1<<TOIE0)		; Timer0 Overflow Interrupt Enable
	out TIMSK,R16			; Send to timer int mask register
	ldi R16,(1<<SE)			; Sleep enabled (using "idle" mode)
	out MCUCR,R16			; Power-down mode is "idle" now, not "sleep"
;---Idle---------------------
	sleep					; Awaken to Timer0 after 65ms
	rjmp STANDBY			; Jump to power-down mode

;--------------------------------------------------------------------
TRANSMIT_RC5:							; 4.00MHz clock only ;		;
;										\--------------------/		;
; Transmits one RC5 frame.  The manchester-encoded bits				;
; have a 37.037kHz carrier except on the debugging pin."ON" periods ;
; are a ~889us-long square wave (~50% duty) containing 32 pulses	;
; of duration 27us.  Off periods consist of silence.	 			;
; This routine will also calculate the even-parity bit. 			; 
;																	;
; All frames are followed by 65.536ms of silence (minimum)			;
;																	;
; IN: group, data
; OUT: -nothing-													;
; DESTROYED: R16,R17,R18,R19,R20,R21								;
; NEEDS: Clock must be 4.00MHz										;
;--------------------------------------------------------------------
	lsl group				; Pre-shift group toward MSB of register...
	lsl group				; ...
	lsl group				; ...
	lsl data				; Pre-shift command toward MSB...
	lsl data				; ...
parity_calculation:
	mov R16,group			; Move group code into temp register
	mov R18,data			; Move data byte into register
	add R16,R18				; Binary add
	com R16					; Invert R16 to produce even parity
	mov parity,R16			; Bit 0 of R19 contains the parity
;---BIT 01: START BIT--------
	rcall low_side
	ldi loop,8
	rcall delay
	rcall high_side
	ldi loop,7	
	rcall delay
;---BIT 02: START BIT--------
	rcall low_side
	ldi loop,8
	rcall delay
	rcall high_side
	ldi loop,7				
	rcall delay
;---BIT 03: PARITY-----------
	lsl parity				; [0.25us]
	rcall manchester		; [1.778ms approx]
;---BIT 04: GROUP BIT 4------
	lsl group				; [0.25us]
	rcall manchester		; [1.778ms approx]
;---BIT 05: GROUP BIT 3------
	lsl group				; [0.25us]
	rcall manchester		; [1.778ms approx]
;---BIT 06: GROUP BIT 2------
	lsl group				; [0.25us]
	rcall manchester		; [1.778ms approx]
;---BIT 07: GROUP BIT 1------
	lsl group				; [0.25us]
	rcall manchester		; [1.778ms approx]
;---BIT 08: GROUP BIT 0------
	lsl group				; [0.25us]
	rcall manchester		; [1.778ms approx]
;---BIT 09: DATA 5-----------
	lsl data				; [0.25us]
	rcall manchester		; [1.778ms approx]
;---BIT 10: DATA 4-----------
	lsl data				; [0.25us]
	rcall manchester		; [1.778ms approx]
;---BIT 11: DATA 3-----------
	lsl data				; [0.25us]
	rcall manchester		; [1.778ms approx]
;---BIT 12: DATA 2-----------
	lsl data				; [0.25us]
	rcall manchester		; [1.778ms approx]
;---BIT 13: DATA 1-----------
	lsl data				; [0.25us]
	rcall manchester		; [1.778ms approx]
;---BIT 14: DATA 0-----------
	lsl data				; [0.25us]
	rcall manchester		; [1.778ms approx]
	ret						; [1.00us]

manchester:
	ldi loop,14
	rcall delay
	brcc low_to_high		; [0.25us/0.50us]
	nop 					; [0.25us]
high_to_low:
	rcall high_side			; [0.50us] + routine
	rcall low_side			; [0.50us] + routine
	ret						; [1.00us]
low_to_high:
	rcall low_side			; [0.50us] + routine
	rcall high_side			; [0.50us] + routine
	ret						; [1.00us]


low_side:
	ldi loop,0			; [0.25us] ; Maximum delay!  
	rcall delay			; [0.50us] + [256us] = [256.50us]
	rcall delay			; [0.50us] + [256us] = [256.50us]
	rcall delay			; [0.50us] + [256us] = [256.50us]
	ldi loop,102		; [0.25us]
	rcall delay			; [0.50us] + [103us] = [103.50us]
	nop					; [0.25us]
	ret					; [1.00us]

high_side:
	debug_low			; Lower debug wire for inverted output logic
	ldi R18,32			; [0.25us] ; Generate 32 pulses per half-bit
square_wave:
	ir_high				; Activate IR signal
;---Latch for 13.1us-----
	ldi loop,11			; [0.25us]
	rcall delay			; [0.50us] + [27us] = [27.50us]
	ir_low				; Disassert outputs
	ldi loop,10			; [0.25us] Hold 27us
	nop					; [0.25us]	
	rcall delay			; [0.50us] + [27us] = [27.50us]
	dec R18				; [0.25us] ; Decrement outer loop counter
	brne square_wave	; [0.25us/0.50us]
;---Anti-Ailias delay----
	ldi loop,11
	rcall delay
	nop
    debug_high			; Return debug wire to normal "high" state
	ret					; [1.00us]


;----------------------------------------
delay:									;
; Inserts a waiting delay.				;
; Miniumum Delay = 2.00us				;
; Maximum Delay = 256us					;
; IN: loop (R17) = delay(us) - 1		;
; OUT: -nothing-						;
; DESTROYED: loop (R17)					;
;---------------------------------------/
	nop				; [0.25us]
	dec loop		; [0.25us]
	brne delay		; [0.50us/0.25us]
	nop				; [0.25us]
	ret				; [1.00us]