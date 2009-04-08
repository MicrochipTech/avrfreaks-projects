; Toaster controller
;
; Written by Roger Storvik (roger.storvik@gmail.com)
; Updated 20090220
; Cleaned up code 20090405 for upload to AVRFreaks.net
;
; v1.0 - works in my toaster
;
; This is a modification on my noname toasters 4060 based controller.
;
; I did not like the design flaws of it having problems if I change the burn time after
; the toaster was started. Sometimes it never stopped, and burned the toast.
; I am also a big fan of indicators, and would like some feedback of how long time it is from
; releasing the toast. Ultimately I would like a 7-seg display for this, but the powersupply
; on the design will not work if we use too much current (It will not lock the el-magnet to hold
; and the springs down and start the heater). 
; So a single LED was used, and works pretty ok.
; I could make a separate PSU unit, but the space inside the toaster is limited and the Toaster
; itself did not cost that much, so I just used parts I already had.
;
; The modification is to remove the 4060 and put in the attiny15.
; Soldered in an lm7805 to stabilize power to 4-5volt. The 4060 gets 6v, but gets a voltage peak 
; in front and I'm not sure the attiny15 can handle that (or 6v).
; Disconnected the potmeter from the oscillator circuit on the original design and connected it
; to the attiny15. Connected the LED and the usual resistor.
;
; The reverseengineering/drawing of the original board I think is right, but I did not check it
; enirely. Do not build something based on this design and blame me if it does not work.
; I do not take any responability whatsoever.
; 
; How I programmed it with STK500 from my Mac Mini:
; avrdude -p t15 -c stk500v1 -P /dev/cu.PL2303-000033FD -e -U flash:w:toaster.hex
;
; PB1 - Status LED output
; PB2 - Potmeter input
; PB0 - Relay to cut power on toaster, this has to be "on" to apply power to the board itself

.include "..\include\tn15def.inc"

.def temp=r16
.def LEDdiff=r17
.def potmeter=r18
.def seconds=r19
.def LEDon=r20
.def c0overflow=r21
.def LEDoverflow=r22

.equ const_overflow=6

.org $00
	rjmp  RESET		; Reset Handle
	reti			; INT0, External IRQ0
	reti			; I/O, Pin change IRQ
	reti			; TIMER1, Compare match A
	rjmp tim1_overf	; TIMER1, Overflow
	rjmp tim0_overf	; TIMER0, Overflow
	reti			; EEPROM ready
	reti			; Analog Comparator
	rjmp ad_vector	; ADC Conversion complete
.cseg

;***********************************************
; ADC complete interrupt service
;
ad_vector:

	in potmeter, ADCH
	mov	LEDdiff, potmeter
	sub	LEDdiff, seconds
	reti

;About one second overflow
; About 6 + 24 per second (6 overflow + 0b00011000)
tim0_overf:
	dec	c0overflow
	brne	not_onesecond
	ldi	c0overflow, const_overflow; set constant to about one second
	inc	seconds
not_onesecond:
	reti

; Timer 1 overflow: used for status LED blinking
tim1_overf:
	cpi		LEDoverflow, 5	; Enough to make LED light up	
	brne	LEDunset    ; jump if led should not be switched on yet	
	cbi		PORTB, PB1     ; Set LED
LEDunset:	
	dec		LEDoverflow
	brne	LEDreturn      ; jump if not zero
	mov		LEDoverflow, LEDdiff
	sbi		PORTB, PB1		; Clear LED
LEDreturn:		
	reti

RESET:
; reset potmeter to max value, to have a known value to compare with until
; the ADC has been read for the first time
	ser	potmeter ;in 	potmeter, ADCH 	; maybe not
	ser	LEDdiff
	ser	LEDoverflow
	cli
;----------------------
; Configure ADC1
	ldi 	temp, (1<<ADFR) | (1<<ADEN) | (1<<ADIE) | (1<<ADPS2)   ;set ADC prescaler to 4, 1.6MHz / 4
	out 	ADCSR,temp   ; and enable end of conversion interrupt
	sbi 	ADMUX, ADLAR ; Get 8 most significant bits in ADCL
	sbi 	ADMUX, MUX0  ; ADC1/PB2 input

	sbi 	ADCSR, ADSC  ; Go free running and enable interrupts;

	clr	seconds
	ldi	c0overflow, const_overflow; set constant to about 1 second

;-----------------------
; Configure "seconds" irq
; Set timer 0 prescaler
; We want this to be close to one second
;	ldi	temp, (1<<CS02); ck/256
	ldi	temp, (1<<CS02)|(1<<CS00); ck/1024
	out	TCCR0,temp

;-----------------
;Configure blink irq
	clr 	temp

;	ldi	temp,(1<<CS13)|(1<<CS12)|(1<<CS11)|(1<<CS10) ; ck/1024 Really slow
;	ldi	temp,(1<<CS13)|(1<<CS12)                     ; ck/128
	ldi	temp,(1<<CS13)|(1<<CS11)                     ; ck/32
;	ldi	temp,(1<<CS13)                               ; ck/8
	out	TCCR1,temp
	clr	temp
	out	OCR1A,temp
	out	OCR1B,temp
	
;Enable timer 0 & 1 overflow irq
	ldi	temp, (1<<TOIE0)|(1<<TOIE1)
	out	TIMSK, temp


;-----------------
;Output
	ldi 	temp,0b00000011	; output PB0, PB1
	out	DDRB,temp
;-------------------
;Switch power on clearing bit 0: PB0
	cbi	PORTB,PB0	
	sei   ; enable interrupts
	
;Main loop ----------------------------------

main:
	; Check if potmeter position is greater that number of seconds
	cpse	seconds, potmeter
	rjmp	main ;Nah, still more seconds to wait
release:	
	sbi	PORTB,PB0 ;switch our power off, to release toast
	cli
	sbi	PORTB,PB1
lockup:	
	rjmp lockup ;Run forever, just in case

; End main loop -----------------------------





