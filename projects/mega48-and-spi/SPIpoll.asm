;----------------------------------------------------
;Description: Basic polling SPI routine using a 74ls91
;			  shift register.
;File Name: SPIpoll.asm
;Author: Luke Wardensky
;
;Notes: This is a blocking routine.
;----------------------------------------------------
.include "m48def.inc"

;----------------------------------------------------
;Constants
;----------------------------------------------------
;.equ	
;.equ	

.cseg
;-----------------------------------------------------
;Interrupt Vectors
;-----------------------------------------------------
.org	$0
	rjmp	RESET	; Reset vector
;.org	INT0addr	; External Interrupt Request 0
;.org	INT1addr	; External Interrupt Request 1
;.org	PCI0addr	; Pin Change Interrupt Request 0
;.org	PCI1addr	; Pin Change Interrupt Request 0
;.org	PCI2addr	; Pin Change Interrupt Request 1
;.org	WDTaddr		; Watchdog Time-out Interrupt
;.org	OC2Aaddr	; Timer/Counter2 Compare Match A
;.org	OC2Baddr	; Timer/Counter2 Compare Match A
;.org	OVF2addr	; Timer/Counter2 Overflow
;.org	ICP1addr	; Timer/Counter1 Capture Event
;.org	OC1Aaddr	; Timer/Counter1 Compare Match A
;.org	OC1Baddr	; Timer/Counter1 Compare Match B
;.org	OVF1addr	; Timer/Counter1 Overflow
;.org	OC0Aaddr	; TimerCounter0 Compare Match A
;.org	OC0Baddr	; TimerCounter0 Compare Match B
;.org	OVF0addr	; Timer/Couner0 Overflow
;.org	SPIaddr		; SPI Serial Transfer Complete
;.org	URXCaddr	; USART Rx Complete
;.org	UDREaddr	; USART, Data Register Empty
;.org	UTXCaddr	; USART Tx Complete
;.org	ADCCaddr	; ADC Conversion Complete
;.org	ERDYaddr	; EEPROM Ready
;.org	ACIaddr		; Analog Comparator
;.org	TWIaddr		; Two-wire Serial Interface
;.org	SPMRaddr	; Store Program Memory Read

;----------------------------------------------------
;General Purpose Register Defines
;----------------------------------------------------
;.def	=r0			;result byte for 'lpm' and low byte for 'mul' instructions
;.def	=r1			;result high byte for 'mul' instruction
;.def	=r2
;.def	=r3
;.def	=r4
;.def	=r5
;.def	=r6
;.def	=r7
;.def	=r8
;.def	=r9
;.def	=r10
;.def	=r11
;.def	=r12
;.def	=r13
;.def	=r14
;.def	=r15
.def	temp	=r16
.def	data	=r17
.def	delay	=r18
.def	delay1	=r19
;.def	=r20
;.def	=r21
;.def	=r22
;.def	=r23
;.def	=r24
;.def	=r25
;.def	=r26		;XL by default
;.def	=r27		;XH by default
;.def	=r28		;YL by default
;.def	=r29		;YH by default
;.def	=r30		;ZL by default
;.def	=r31		;ZH by default

;-------------------------------------------------------
;Variables
;-------------------------------------------------------
.dseg
count:	.db	$0	;one byte counter to display on LEDs

;-------------------------------------------------------
;Main Program
;-------------------------------------------------------
.cseg
RESET:
	ldi	temp, $EF
	out	DDRB, temp	;set MISO as input. /SS, MOSI, and SCLK as output
	ldi	temp, $FF
	out	DDRD, temp	;set PORTD as output for LEDs
	out	PORTD, temp	;turn off all LEDs
	ldi	temp, $51
	out	SPCR, temp	;enable SPI with clk/16
	clr	temp
	sts	count, temp	;clear the counter
again:	lds	temp, count
	dec	temp
	sts	count, temp	;decrement the counter
	lds	data, count	;load data with counter value
	rcall	TRANS	;do a transmission cycle
	com	data		;compliment data since logical 0 turns on LEDs
	out	PORTD, data	;output value shifted in from register to LEDs
	clr	delay
	clr	delay1
hold:	dec	delay
	brne	hold
	dec	delay1
	brne	hold	;delay here to make LED changes visible
	rjmp	again	;do it again

;-------------------------------------------------------
;SPI Transmission Routine
;load r17 with output before calling and read r17 for input after return
;-------------------------------------------------------
TRANS:	cbi	PORTB, 2	;/SS low for transmission
	out	SPDR, r17		;send out data byte
wait:	in	temp, SPSR
	sbrs temp, SPIF	
	rjmp	wait	;wait for the end of transmission
	sbi	PORTB, 2	;/SS high after transmission
	in	r17, SPDR	;load input data into r18
	ret