;*** I2C Master for Thermometer *************************************
;
; Filename		:I2CM_T.asm
;
; Project		;Brick For Programs
; First Target	:Thermometer With I2C Bus (TH03_V1.asm)
; Device		:All
; Created		:2003.04.05
; Last Updated	:
; Version		:1.0
;
; DESCRIPTION: See application Note AVR300
; ATTENTION: Supports Only Function of Reading
;
;	Main I2C functions :
;	'i2c_start' -		Issues a start condition and sends address
;				and transfer direction.
;	'i2c_rep_start' -	Issues a repeated start condition and sends
;				address and transfer direction.
;	'i2c_do_transfer' -	Sends or receives data depending on
;				direction given in address/dir byte.
;	'i2c_stop' -		Terminates the data transfer by issue a
;				stop condition.
;***************************************************************************

;**** Global I2C Constants ****
;--- Port Definitions ---------
.equ Port_I2C	=PORTD
.equ DDR_I2C	=DDRD
.equ Pin_I2C	=PIND
.equ SCL		=3
.equ SDA		=2

.equ slave_address	=0b10010000;Set device address

.equ	b_dir	= 0			; transfer direction bit in i2cadr
.equ	i2crd	= 1
;.equ	i2cwr	= 0

;**** Global Register Variables ****

.def	i2cdelay= r17			; Delay loop variable
.def	i2cdata	= r18			; I2C data transfer register
.def	i2cadr	= r19			; I2C address and direction register
.def	i2cstat	= r20			; I2C bus status register

;***************************************************************************
;* FUNCTION
;*	i2c_hp_delay
;*	i2c_qp_delay
;***************************************************************************

i2c_hp_delay:
	ldi	i2cdelay,30
i2c_hp_delay_loop:
	dec	i2cdelay
	brne	i2c_hp_delay_loop
	ret

i2c_qp_delay:
	ldi	i2cdelay,15	
i2c_qp_delay_loop:
	dec	i2cdelay
	brne	i2c_qp_delay_loop
	ret

;***************************************************************************
;* FUNCTION
;*	i2c_start
;***************************************************************************

i2c_start:				
	mov	i2cdata,i2cadr	; copy address to transmitt register
	sbi	DDR_I2C,SDA		; force SDA low
	rcall	i2c_qp_delay; quarter period delay
	ret
;***************************************************************************
;* FUNCTION
;*	i2c_write
;***************************************************************************

i2c_write:
	sec					; set carry flag
	rol	i2cdata			; shift in carry and out bit one
	rjmp	i2c_write_first
i2c_write_bit:
	lsl	i2cdata			; if transmit register empty
i2c_write_first:
	;breq	i2c_get_ack		;	goto get acknowledge
	breq	write_end
	sbi	DDR_I2C,SCL		; force SCL low

	brcc	i2c_write_low; if bit high
	nop					;	(equalize number of cycles)
	cbi	DDR_I2C,SDA		;	release SDA
	rjmp	i2c_write_high
i2c_write_low:			; else
	sbi	DDR_I2C,SDA		; force SDA low
	rjmp	i2c_write_high;	(equalize number of cycles)
i2c_write_high:
	rcall	i2c_hp_delay; half period delay
	cbi	DDR_I2C,SCL		; release SCL
	rcall	i2c_hp_delay; half period delay

	rjmp	i2c_write_bit
	write_end:
	ret

;***************************************************************************
;* FUNCTION Acknowledge from slave to master
;*	i2c_get_ack
;***************************************************************************

i2c_get_ack:
	sbi	DDR_I2C,SCL		; force SCL low
	cbi	DDR_I2C,SDA		; release SDA
	rcall	i2c_hp_delay; half period delay
	cbi	DDR_I2C,SCL		; release SCL

i2c_get_ack_wait:
	sbic	Pin_I2C,SCL	; wait SCL high (In case wait states are inserted)
	rjmp	i2c_get_ack_wait

	clc					; clear carry flag
	sbic	Pin_I2C,SDA	; if SDA is high
	sec					;	set carry flag
	sbic	Pin_I2C,SDA	; if SDA is high
	rcall line_fault
	rcall	i2c_hp_delay; half period delay
	ret

;***************************************************************************
;* FUNCTION
;*	i2c_read
;***************************************************************************

i2c_read:
	rol	i2cstat			; store acknowledge (used by i2c_put_ack)
	ldi	i2cdata,0x01	; data = 0x01
i2c_read_bit:			; do
	sbi	DDR_I2C,SCL		; force SCL low
	rcall	i2c_hp_delay;	half period delay

	cbi	DDR_I2C,SCL		; release SCL
	rcall	i2c_hp_delay; half period delay

	clc					; clear carry flag
	sbic	Pin_I2C,SDA	; if SDA is high
	sec					; set carry flag

	rol	i2cdata			; store data bit
	brcc	i2c_read_bit; while receive register not full
	ret

;***************************************************************************
;* FUNCTION
;*	i2c_stop
;***************************************************************************

i2c_stop:
	sbi	DDR_I2C,SCL		; force SCL low
	sbi	DDR_I2C,SDA		; force SDA low
	rcall	i2c_hp_delay; half period delay
	cbi	DDR_I2C,SCL		; release SCL
	rcall	i2c_qp_delay; quarter period delay
	cbi	DDR_I2C,SDA		; release SDA
	rcall	i2c_hp_delay; half period delay
	ret

;***************************************************************************

;**** Read data ****
read_data:
	rcall set_mode_I2C
	ldi	i2cadr,slave_address+i2crd	;Set device address and read
	rcall i2c_start		; Send start condition and slave address

	rcall i2c_write
	rcall i2c_get_ack

	sec					; Set no acknowledge (read is followed by a stop condition)
	rcall i2c_read
	rcall	i2c_stop	; Send stop condition - releases bus
	ret

;***************************************************************************
;* FUNCTION    i2c_init
;* DESCRIPTION Initialization of the I2C bus interface.
;* USAGE       rcall this function once to initialize the I2C bus. No parameters
;*	       are required.
;* RETURN      None
;* NOTE        PORTD and DDRD pins not used by the I2C bus interface will be
;*	       set to Hi-Z (!).
;* COMMENT     This function can be combined with other PORTD initializations.
;***************************************************************************

	set_mode_I2C:
	cbr flags, 1<<lf

  	cbi	Port_I2C,SCL	; set I2C pins to open colector
  	cbi	DDR_I2C,SCL		; release SCL
  	cbi	Port_I2C,SDA
  	cbi	DDR_I2C,SDA		; release SDA

  	rcall i2c_hp_delay	; half period delay
  	rcall i2c_hp_delay	; half period delay
  	rcall i2c_hp_delay	; half period delay
  	rcall i2c_hp_delay	; half period delay
  	clr   i2cstat			; clear I2C status register
  	ret
;***************************************************************************
;--------------------------------------------------
line_fault:
	sbr flags, 1<<lf
	ret

;**** End of File ****