EXT_INT2: ; External Interrupt 2 Handler

;Routines to deal with RECEIVED keyboard characters

	push	temp		
	in	temp,SREG
	push	temp
	push	ZL
	push	ZH	

	cli

	lds	temp,keyboard_bit		;bit count
	tst	temp
	brne	KEYTEST1			;if zero, is start bit
;note:  in the event of a timeout, keyboard_bit is zeroed by the T2 overflow interrupt - we don't need to do any further tests on it here
	sbic	PINB,KDT
	rjmp  DONE_KEY
	ldi	temp,3
	sts	keyboard_time,temp	;start timout timer
	ldi	temp,1			; set bitcount
	clr	key_char
	rjmp	KEYOUT
		
KEYTEST1:
	cpi	temp,9			;is this parity bit?
	breq	KEYPARITY
	
	cpi	temp,10			;is this stop bit?
	breq	KEYSTOP			;process char
	
	clc					;clear carry
	sbic	PINB,KDT
	sec					;set carry if data is high
	ror	key_char			;the easy bit - put new bit into keyboard character
	inc	temp				;for next bit
	rjmp	KEYOUT			;we won't process until we get stop bit


KEYPARITY:

	inc	temp				;not testing parity, just increment
	rjmp	KEYOUT

KEYSTOP:
;First check if we have a shift character

	mov	temp,key_char

	cpi	temp,0x3C
	breq	KEY_SHIFT
	cpi	temp,0x55
	breq	KEY_SHIFT

;now check for symbol

	cpi	temp,0x49
	breq	KEY_SYMBOL

;now check for key up received

	cpi	temp,0xF0
	breq	KEY_UP_RECEIVED

;now check if last one was key up
	sbrc	more_flags,key_up
	rjmp	LAST_ONE_UP			;we just ignore this

;now check if shift or symbol
	sbrc	more_flags,shift
	rjmp	GET_SHIFTED
	sbrc	more_flags,symbol
	rjmp	GET_SYMBOL
	rjmp	GET_NORMAL





	

;put received character in the buffer!

	rjmp	DONE_KEY
			

KEY_UP_RECEIVED:

	sbr	more_flags,(1<<key_up)	;so we know next char is a key release

	rjmp	DONE_KEY

LAST_ONE_UP:

	cbr	more_flags,(1<<key_up)	;cancels flag
						;we don't use at the moment

	rjmp	DONE_KEY


KEY_SYMBOL:
	
	sbrc	more_flags,key_up		;Do we need to clear symbol flag
	rjmp	CLEAR_SYMBOL_FLAG
	sbr	more_flags,(1<<symbol)
	sbi	PORTB,PB0			;flash status LED

	rjmp	DONE_KEY

CLEAR_SYMBOL_FLAG:

	cbr	more_flags,(1<<symbol)
	cbi	PORTB,PB0			;unflash status LED
	cbr	more_flags,(1<<key_up)

	rjmp	DONE_KEY

KEY_SHIFT:
	
	sbrc	more_flags,key_up		;Do we need to clear shift flag
	rjmp	CLEAR_SHIFT_FLAG
	sbr	more_flags,(1<<shift)
	sbi	PORTB,PB0			;flash status LED

	rjmp	DONE_KEY

CLEAR_SHIFT_FLAG:

	cbr	more_flags,(1<<shift)
	cbi	PORTB,PB0			;unflash status LED
	cbr	more_flags,(1<<key_up)

DONE_KEY:					;when a full character processed

	clr	temp				;ready for next character
KEYOUT:
	sts	keyboard_bit,temp		;store revised bitcount


	sei

pop	ZH
pop	ZL
pop temp
out	SREG,temp		
pop	temp


	
	reti

;Routine to take a character in buffer and use it.

;if an ascii character print it on screen

;If return print newline and send buffer via SPI

;if delete remove character from buffer and screen



INT2_INIT:

	ldi	temp,(0<<ISC2)		;INT2 1/0 gives triggered on rising/falling edge
	out	MCUCSR,temp

	in	temp,GICR
	ori	temp,(1<<INT2)		;enable INT2
	out	GICR,temp

	ret


GET_SHIFTED:

	ldi	ZL,LOW(SHIFTED_CODES<<1)		;two bytes per data space
	ldi	ZH,HIGH(SHIFTED_CODES<<1)	

	rjmp	FETCH_ASCII

GET_SYMBOL:

	ldi	ZL,LOW(SYMBOL_CODES<<1)		;two bytes per data space
	ldi	ZH,HIGH(SYMBOL_CODES<<1)	

	rjmp	FETCH_ASCII

GET_NORMAL:

	ldi	ZL,LOW(UNSHIFTED_CODES<<1)		;two bytes per data space
	ldi	ZH,HIGH(UNSHIFTED_CODES<<1)	
	
FETCH_ASCII:

	add	ZL,temp				;add offset
	clr	temp
	adc	ZH,temp

	lpm	temp,Z				;fetch ascii from lookup table

	out	UDR,temp

	sbr	more_flags,(1<<char_here) ;just so we know!

	rjmp	DONE_KEY




;put received character in the buffer!
	ldi	ZL,LOW(keyboard_buffer)
	ldi	ZH,HIGH(keyboard_buffer)	
	
	lds	key_char,keyboard_pointer
	add	ZL,key_char
	clr	key_char
	adc	ZH,key_char

	st	Z,temp
