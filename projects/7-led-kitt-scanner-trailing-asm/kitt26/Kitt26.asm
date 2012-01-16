;************************************************************************
;* KITT26.ASM: 7 LED KITT scanner with trailing effect & speed adjust
;************************************************************************
;*
;*
;* System information:
;*
;* 	Refresh rate	  	= approx. 55 Hz
;*	Intensity depth  	= 22 levels
;*	Speed adjust depth	= 5 bit (32 speeds)
;*	Pulse-Pause ratio	= 1:12000 (duty-cycle=Preset/T*100%)
;*
;*
;*
;* Copyright ©2009 by RES®
;*
;*
;* RES can be reached at: http://www.attiny.com
;*
;*
;* Version	:1.00
;* Date		:15-FEB-2009
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
	.equ	SPD		= 13		;set KITT animation speed
	.equ	FRMS		= 12		;set number of frames

;* Register variables

	.def	save		= r0		;save SREG
	.def	oldVal		= r1		;previous potVal
	.def	potVal		= r2		;potmeter value
	.def	speed		= r3		;KITT animation speed
	.def	PWMLo0		= r10		;PWM lower register for LED0
	.def	PWMHi0		= r11		;PWM upper register for LED0
	.def	PWMLo1		= r12		;PWM lower register for LED1
	.def	PWMHi1		= r13		;PWM upper register for LED1
	.def	PWMLo2		= r14		;PWM lower register for LED2
	.def	PWMHi2		= r15		;PWM upper register for LED2
	.def	temp		= r16		;temporary data
	.def	frmCount	= r17		;frame counter
	.def	seqCount	= r18		;sequence counter













;* Timer0 interrupts every 32.768 msec

;* Code

OVF0:	in	save, SREG			;save SREG
	in	potVal, ADCH			;fetch current potVal
	lsr	potVal
	lsr	potVal				;potVal / 8
	lsr	potVal				;(bring back to 5 bit)
	cp	potVal, oldVal			;if oldVal < potVal -> INC speed
	brlo	decSpd				;if oldVal = potVal -> no speed adjust
	breq	noSpAdj				;if oldVal > potVal -> DEC speed
incSpd:	inc	speed				;INC speed by one
	rjmp	noSpAdj
decSpd:	dec	speed				;DEC speed by one
	brne	noSpAdj				;if speed = 0,
	inc	speed				; speed = speed + 1 (never zero)
noSpAdj:mov	oldVal, potVal			;keep last potVal
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
	out	ADCSR, temp			; free running (init)
waitDum:sbis	ADCSR, ADIF			;wait for ADC dummy run end
	rjmp	waitDum				; (ADIF = 1)
	sbi	ADCSR, ADIF                     ;clear ADIF
waitADC:sbis	ADCSR, ADIF			;wait for ADC first run end
	rjmp	waitADC				; (ADIF = 1)
	in	potVal, ADCH			;fetch initial potVal
	lsr	potVal				;potVal / 8
	lsr	potVal				;(bring back to 5 bit)
	lsr	potVal
	mov	oldVal, potVal			;keep initial potVal
	mov	speed, potVal			;set initial speed
	tst	speed
	brne	spdNot0				;if initial speed = 0,
	inc	speed				; -> speed = speed + 1
spdNot0:ldi	ZH, high(INTENS*2)		;Z pointer ->
	ldi	ZL, low(INTENS*2)		; intensity levels bank
	adiw	ZL, 10				;set overall brightness
	adiw	ZL, 6				;initial intensity level
	lpm	PWMLo0, Z
	adiw	ZL, 1				;load PWM0 preset LED1
	lpm	PWMHi0, Z
	adiw	ZL, 1
	adiw	ZL, 12				;up to intens level LED2
	lpm	PWMLo1, Z
	adiw	ZL, 1				;load PWM1 preset LED2
	lpm	PWMHi1, Z
	adiw	ZL, 1
	adiw 	ZL, 8				;up to intens level LED3
	lpm	PWMLo2, Z
	adiw	ZL, 1				;load PWM2 preset LED3
	lpm	PWMHi2, Z
	adiw	ZL, 1
	mov	frmCount, speed			;initial speed = SPD * T
	ldi	seqCount, FRMS			;load frame counter FRMS
	ldi	ZH, high(LEDSEQ*2)		;position Z to,
	ldi	ZL, low(LEDSEQ*2)		;  KITT sequence data
	clr	XH				;clear cycle counter
	clr	XL
	out	TCNT0, XL			;zero timer0 counter
	sei					;enable interrupts

KITT26:	movw	YL, ZL				;save Z in Y
	cp	XL, PWMLo0
	cpc	XH, PWMHi0
	brlo	pulseA1				;if X > PWM0 preset,
pauseA1:clr	temp
	out	PORTA, temp			;begin pause period
	rjmp	exitA1				; if X < PWM0 preset,
pulseA1:lpm	temp, Z
	adiw	ZL, 1				;to next LED position
	out	PORTA, temp			;begin pulse next LED
exitA1:	cp	XL, PWMLo1
	cpc	XH, PWMHi1
	brlo	pulseA2				;if X > PWM1 preset,
pauseA2:clr	temp
	out	PORTA, temp			;begin pause period
	rjmp	exitA2				; if X < PWM1 preset,
pulseA2:lpm	temp, Z
	adiw	ZL, 1				;to next LED position
	out	PORTA, temp			;begin pulse next LED
exitA2:	cp	XL, PWMLo2
	cpc	XH, PWMHi2
	brlo	pulseA3				;if X > PWM2 preset,
pauseA3:clr	temp
	out	PORTA, temp			;begin pause period
	rjmp	exitA3				; if X < PWM2 preset,
pulseA3:lpm	temp, Z
	out	PORTA, temp			;begin pulse next LED
exitA3:	movw	ZL, YL				;restore Z from Y
	adiw	XL, 1				;count next cycle
	ldi	temp, low(T)
	cp	XL, temp
	ldi	temp, high(T)			;if X @ end period (T),
	cpc	XH, temp
	brne	KITT26
	clr	XH				;reset cycle counter
	clr	XL
	dec	frmCount			;count next frame
	brne	noEvent				;if frmCount = 0,
	mov	frmCount, speed			;reset event counter
	adiw	ZL, 3				;move Z to next frame
	dec	seqCount			;if seqCount = 0,
	brne	noEvent
	ldi	seqCount, FRMS			;reset frame counter
	ldi	ZH, high(LEDSEQ*2)		;reposition Z to,
	ldi	ZL, low(LEDSEQ*2)		;  KITT sequence data
noEvent:rjmp	KITT26				;loop KITT forever










;* Table with intensity level presets (0 - 520).
;* 0 = off, 805 = bright.
;* Presets stipulated by using the eye and interpolation.

;* Data

INTENS:	.dw 0,1,2,4,7,13,20,28,40,61,90,115,150,192,232,290
	.dw 360,438,530,610,700,805

;* KITT sequence data
;*
;* Complete sequence contains 12 frames.
;* Each frame using 3 bytes corresponding to the 3 LEDs.
;* Number of databytes is 12 * 3 = 36.

LEDSEQ:	.db 0b00001000,0b00000100
	.db 0b00000010,0b00010000
	.db 0b00001000,0b00000100
	.db 0b00100000,0b00010000
	.db 0b00001000,0b01000000
	.db 0b00100000,0b00010000
	.db 0b10000000,0b01000000
	.db 0b00100000,0b01000000
	.db 0b10000000,0b00000000
	.db 0b00100000,0b01000000
	.db 0b10000000,0b00010000
	.db 0b00100000,0b01000000
	.db 0b00001000,0b00010000
	.db 0b00100000,0b00000100
	.db 0b00001000,0b00010000
	.db 0b00000010,0b00000100
	.db 0b00001000,0b00000100
	.db 0b00000010,0b00000000