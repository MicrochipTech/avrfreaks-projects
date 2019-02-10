;***************************************************************************
;*
;* FUNCTION
;*	i2c_init
;*
;* DESCRIPTION
;*	Initialization of the I2C bus interface.
;*
;* USAGE
;*	Call this function once to initialize the I2C bus. No parameters
;*	are required.
;*
;* RETURN
;*	None
;*
;* NOTE
;*	PORTD and I2CDDR pins not used by the I2C bus interface will be
;*	set to Hi-Z (!).
;*
;* COMMENT
;*	This function can be combined with other PORTD initializations.
;*
;***************************************************************************
i2c_init:
	clr	temp		; 
				; 
	out	PORTB,temp	; set I2C pins to open collector
	out	DDRB,temp
	ret
;
;***************************************************************************
;*
;* FUNCTION
;*	i2c_hp_delay
;*	i2c_qp_delay
;*
;* DESCRIPTION
;*	hp - half i2c clock period delay (normal: 5.0us / fast: 1.3us)
;*	qp - quarter i2c clock period delay (normal: 2.5us / fast: 0.6us)
;*
;*	Adjusted for arduino board with 16MHz clock
;*
;* USAGE
;*	no parameters
;*
;* RETURN
;*	none
;*
;***************************************************************************

i2c_hp_delay:
	ldi	i2cdelay,24		;5uS delay @ 16MHz

i2c_hp_delay_loop:
	dec	i2cdelay
	brne	i2c_hp_delay_loop
	ret

i2c_qp_delay:
	ldi	i2cdelay,11		;2.5uS delay @ 16MHz

i2c_qp_delay_loop:
	dec	i2cdelay
	brne	i2c_qp_delay_loop
	ret
;
;***************************************************************************
WriteI2C:
	rcall	i2c_start		;send start bit and address & get ack bit
;	brcs	errorloop		;not enabled in this version

	mov	i2cdata,LCD_Reg	;
	rcall	i2c_write		;send data byte, get ack bit and send stop bit
;	brcs	errorloop		;not enabled in this version

	rcall	i2c_stop		;
	ret

;errorloop				;resend X amount of times if you want
;	rjmp	errorloop
;***************************************************************************
;*
;* FUNCTION
;*	i2c_start
;*
;* DESCRIPTION
;*	Generates start condition and sends slave address.
;*
;* USAGE
;*	i2cadr - Contains the slave address and transfer direction.
;*
;* RETURN
;*	Carry flag - Cleared if a slave responds to the address.
;*
;* NOTE
;*	IMPORTANT! : This function must be directly followed by i2c_write.
;*
;***************************************************************************
;
i2c_start:	
	ldi	i2cdata,i2cadr	; copy address to transmitt register
	sbi	I2CDDR,SDAP		; data line low (start bit)
	rcall	i2c_qp_delay	; quarter period delay
;
;continue to i2c_write
;
;***************************************************************************
;*
;* FUNCTION
;*	i2c_write
;*
;* DESCRIPTION
;*	Writes data (one byte) to the I2C bus. Also used for sending
;*	the address.
;*
;* USAGE
;*	i2cdata - Contains data to be transmitted.
;*
;* RETURN
;*	Carry flag - Set if the slave respond transfer.
;*
;* NOTE
;*	IMPORTANT! : This function must be directly followed by i2c_get_ack.
;*
;***************************************************************************

i2c_write:
	sec				; set carry flag
	rol	i2cdata		; shift in carry and out bit one
	rjmp	i2c_write_first

i2c_write_bit:
	lsl	i2cdata		; if transmit register empty

i2c_write_first:
	breq	i2c_get_ack		; goto get acknowledge
	sbi	I2CDDR,SCLP		; force SCL low

	brcc	i2c_write_low	; if bit high
	nop				;(equalize number of cycles)
	cbi	I2CDDR,SDAP		;release SDA
	rjmp	i2c_write_high

i2c_write_low:			; else
	sbi	I2CDDR,SDAP		; force SDA low
	rjmp	i2c_write_high	; (equalize number of cycles)

i2c_write_high:
	rcall	i2c_hp_delay	; half period delay
	cbi	I2CDDR,SCLP		; release SCL
	rcall	i2c_hp_delay	; half period delay

	rjmp	i2c_write_bit
;***************************************************************************
;*
;* FUNCTION
;*	i2c_get_ack
;*
;* DESCRIPTION
;*	Get slave acknowledge response.
;*
;* USAGE
;*	(used only by i2c_write in this version)
;*
;* RETURN
;*	Carry flag - Cleared if a slave responds to a request.
;*
;***************************************************************************

i2c_get_ack:
	sbi	I2CDDR,SCLP		; force SCL low
	cbi	I2CDDR,SDAP		; release SDA
	rcall	i2c_hp_delay		; half period delay
	cbi	I2CDDR,SCLP		; release SCL

i2c_get_ack_wait:
	sbis	I2CPIN,SCLP		; wait SCL high 
					;(In case wait states are inserted)
	rjmp	i2c_get_ack_wait

	clc				; clear carry flag
	sbic	I2CPIN,SDAP		; if SDA is high
	sec				; set carry flag
	rcall	i2c_hp_delay	; half period delay
	ret
;
;***************************************************************************
;*
;* FUNCTION
;*	i2c_stop
;*
;* DESCRIPTION
;*	Assert stop condition.
;*
;* USAGE
;*	No parameters.
;*
;* RETURN
;*	None.
;*
;***************************************************************************
;
i2c_stop:
	sbi	I2CDDR,SCLP		; force SCL low
	sbi	I2CDDR,SDAP		; force SDA low
	rcall	i2c_hp_delay	; half period delay
	cbi	I2CDDR,SCLP		; release SCL
	rcall	i2c_qp_delay	; quarter period delay
	cbi	I2CDDR,SDAP		; release SDA
	rcall	i2c_hp_delay	; half period delay
	ret
;***********************************************************