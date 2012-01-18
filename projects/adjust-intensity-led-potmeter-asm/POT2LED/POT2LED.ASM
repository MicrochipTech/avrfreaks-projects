;************************************************************************
;* POT2LED.ASM: Adjust the intensity of an LED with a potmeter
;************************************************************************
;*
;*
;* System information:
;*
;* 	Refresh rate	  	= approx. 56 Hz
;*	Resolution	  	= 4 bit (16 intensity levels)
;*	Pulse-Pause ratio	= 1:12000
;*	Duty-cycle		= T / 2 - P
;*	Range Pulse Width 	= 0 to T / 2 (0 = off, T/2 = bright)
;*
;* Connect a 10k - 50k potmeter to ADC0 pin PA0 and an LED + 75 Ohm to PA1
;*
;*
;*
;* Copyright ©2009 by RES®
;*
;*
;* RES can be reached at: http://www.attiny.com
;*
;*
;* Version	:1.00b
;* Date		:05-FEB-2009
;* Author	:Rob's ElectroSoft®
;* Target MCU	:ATtiny26-16PI @ 8 MHz int. RC
;* Fuses	:sp12 -wF01100100
;************************************************************************

.include "tn26def.inc"				;ATtiny26 definitions





.org	0x0000
	rjmp	RESET				;reset handler
.org	OVF0addr
	rjmp	OVF0				;timer0 ovf interrupt handler








;* Define registers and settings

	.equ	T		= 12000		;number of period cycles

;* Register variables

	.def	save		= r0		;save SREG
	.def	oldVal		= r1		;previous potVal
	.def	temp		= r16		;temporary data
	.def	potVal		= r17		;potmeter value
	.def	WL		= r24		;lower W pointer
	.def	WH		= r25		;upper W pointer












;* Timer0 overflow interrupt

;* Code

OVF0:	in	save, SREG			;save SREG
	in	potVal, ADCH			;fetch current potVal
	swap	potVal				;potVal / 16
	andi	potVal, 0x0f			;only lower nibble
	cp	potVal, oldVal			;if oldVal < potVal -> INC PWM
	brlo	decPWM				;if oldVal = potVal -> do nothing
	breq	exitOVF				;if oldVal > potVal -> DEC PWM
incPWM:	adiw	ZL, 2				;word up to next preset
	rjmp	loadPst				;& load preset
decPWM:	sbiw	ZL, 2				;word down to prev preset
loadPst:lpm	WL, Z+				;& load preset
	lpm	WH, Z
	sbiw	ZL, 1				;restore Z
exitOVF:mov	oldVal, potVal			;keep last potVal
	out	SREG, save			;restore SREG
	reti













;* Main program

;* Code

RESET:	ldi	temp, RAMEND			;stack space -> end SRAM
	out	SP, temp
	ser	temp
	out	DDRA, temp			;PORTA all outputs
	sbi	ACSR, ACD			;Analog Comparator off
	out	DDRB, temp			;PORTB all outputs
	clr	temp
	out	PORTB, temp			;PORTB all pins lo
	out	ADCSR, temp			;disable ADC*
	out	PORTA, temp			;PORTA all pins lo
	cbi	DDRA, PA0			;PA0 input ADC0
	ldi	temp, 0b00000101		;start timer0 @ ck/1024
	out	TCCR0, temp			;tmax = 32.768 msec
	ldi	temp, 0b00000010		;enable timer0 ovf interrrupt
	out	TIMSK, temp
	ldi	temp, 0b00100000		;ADC0, Vref = AVcc
	out	ADMUX, temp			; left adjusted
	ldi	temp, 0b11110110		;start ADC @ clk/64
	out	ADCSR, temp			; single mode
waitDum:sbis	ADCSR, ADIF			;wait for ADC dummy run end
	rjmp	waitDum				; (ADIF = 1)
	sbi	ADCSR, ADIF                     ;clear ADIF
waitADC:sbis	ADCSR, ADIF			;wait for ADC first run end
	rjmp	waitADC				; (ADIF = 1)
	ldi	ZH, high(INTENS*2)		;Z pointer ->
	ldi	ZL, low(INTENS*2)		; intensity levels bank
	in	potVal, ADCH			;fetch init potVal
	swap	potVal				;potVal / 16
	andi	potVal, 0x0f			;only lower nibble
	mov	oldVal, potVal			;keep initial potVal
	tst	potVal				;if potVal = 0,
	breq	notAdjZ				; do not adjust Z, else
adjustZ:adiw	ZL, 2				;right to next word,
	dec	potVal				; until potVal = 0
	brne	adjustZ
notAdjZ:lpm	WL, Z+				;load PW preset
	lpm	WH, Z
	sbiw	ZL, 1				;restore Z
	clr	XH				;zero period counter
	clr	XL
	out	TCNT0, XL			;zero timer0 counter
	sei					;enable interrupts

pot2LED:adiw	XL, 1				;count period time (T)
	ldi	temp, low(T/2)
	cp	XL, temp
	ldi	temp, high(T/2)
	cpc	XH, temp			;if T @ 50%,
	brlo	pulsPA1
pauseP:	cbi	PORTA, PA1			;begin pause period
	ldi	temp, low(T)
	cp	XL, temp
	ldi	temp, high(T)
	cpc	XH, temp			;if T @ 100%,
	brne	pot2LED
	clr	XH				;reset period counter
	clr	XL
	rjmp	pot2LED
pulsPA1:cp	XL, WL
	cpc	XH, WH				;if X > W,
	brlo	pot2LED
pulseP:	sbi	PORTA, PA1			;begin pulse period
	rjmp	pot2LED












;* Table with intensity level presets
;*
;* 0 = off, T/2 = bright
;* Presets stipulated by using the eye and interpolation

;* Data

INTENS:	.dw T/2-0,T/2-1,T/2-2,T/2-4,T/2-7,T/2-13,T/2-20,T/2-28
	.dw T/2-40,T/2-61,T/2-90,T/2-115,T/2-150,T/2-192,T/2-232,T/2-290