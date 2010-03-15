;**** A P P L I C A T I O N   N O T E   A V R 3 0 0 ************************
;*
;* Title		: TWI (Single) Master Implementation
;* Version		: 1.0
;* Last updated		: 2002.05.03
;* Target		: AT90Sxxxx (any AVR device)
;*
;* Support email	: avr@atmel.com
;*
;* DESCRIPTION
;* 	Basic routines for communicating with TWI slave devices. This
;*	"single" master implementation is limited to one bus master on the
;*	TWI bus. Most applications do not need the multimaster ability
;*	the TWI bus provides. A single master implementation uses, by far,
;*	less resources and is less XTAL frequency dependent.
;*
;*	Some features :
;*	* All interrupts are free, and can be used for other activities.
;*	* Supports normal and fast mode.
;*	* Supports both 7-bit and 10-bit addressing.
;*	* Supports the entire AVR microcontroller family.
;*
;*	Main TWI functions :
;*	'TWI_start' -		Issues a start condition and sends address
;*				and transfer direction.
;*	'TWI_rep_start' -	Issues a repeated start condition and sends
;*				address and transfer direction.
;*	'TWI_do_transfer' -	Sends or receives data depending on
;*				direction given in address/dir byte.
;*	'TWI_stop' -		Terminates the data transfer by issue a
;*				stop condition.
;*
;* USAGE
;*	Transfer formats is described in the AVR300 documentation.
;*	(An example is shown in the 'main' code).	
;*
;* NOTES
;*	The TWI routines can be called either from non-interrupt or
;*	interrupt routines, not both.
;*
;* STATISTICS
;*	Code Size	: 81 words (maximum)
;*	Register Usage	: 4 High, 0 Low
;*	Interrupt Usage	: None
;*	Other Usage	: Uses two I/O pins on port D
;*	XTAL Range	: N/A
;*
;***************************************************************************

;**** Includes ****

.include "1200def.inc"			; change if an other device is used

;**** Global TWI Constants ****

.equ	SCLP	= 1			; SCL Pin number (port D)
.equ	SDAP	= 0			; SDA Pin number (port D)

.equ	b_dir	= 0			; transfer direction bit in TWIadr

.equ	TWIrd	= 1
.equ	TWIwr	= 0

;**** Global Register Variables ****

.def	TWIdelay= r16			; Delay loop variable
.def	TWIdata	= r17			; TWI data transfer register
.def	TWIadr	= r18			; TWI address and direction register
.def	TWIstat	= r19			; TWI bus status register

;**** Interrupt Vectors ****

	rjmp	RESET			; Reset handle
;	( rjmp	EXT_INT0 )		; ( IRQ0 handle )
;	( rjmp	TIM0_OVF )		; ( Timer 0 overflow handle )
;	( rjmp	ANA_COMP )		; ( Analog comparator handle )


;***************************************************************************
;*
;* FUNCTION
;*	TWI_hp_delay
;*	TWI_qp_delay
;*
;* DESCRIPTION
;*	hp - half TWI clock period delay (normal: 5.0us / fast: 1.3us)
;*	qp - quarter TWI clock period delay (normal: 2.5us / fast: 0.6us)
;*
;*	SEE DOCUMENTATION !!!
;*
;* USAGE
;*	no parameters
;*
;* RETURN
;*	none
;*
;***************************************************************************

TWI_hp_delay:
	ldi	TWIdelay,2
TWI_hp_delay_loop:
	dec	TWIdelay
	brne	TWI_hp_delay_loop
	ret

TWI_qp_delay:
	ldi	TWIdelay,1	
TWI_qp_delay_loop:
	dec	TWIdelay
	brne	TWI_qp_delay_loop
	ret


;***************************************************************************
;*
;* FUNCTION
;*	TWI_rep_start
;*
;* DESCRIPTION
;*	Assert repeated start condition and sends slave address.
;*
;* USAGE
;*	TWIadr - Contains the slave address and transfer direction.
;*
;* RETURN
;*	Carry flag - Cleared if a slave responds to the address.
;*
;* NOTE
;*	IMPORTANT! : This funtion must be directly followed by TWI_start.
;*
;***************************************************************************

TWI_rep_start:
	sbi	DDRD,SCLP		; force SCL low
	cbi	DDRD,SDAP		; release SDA
	rcall	TWI_hp_delay		; half period delay
	cbi	DDRD,SCLP		; release SCL
	rcall	TWI_qp_delay		; quarter period delay


;***************************************************************************
;*
;* FUNCTION
;*	TWI_start
;*
;* DESCRIPTION
;*	Generates start condition and sends slave address.
;*
;* USAGE
;*	TWIadr - Contains the slave address and transfer direction.
;*
;* RETURN
;*	Carry flag - Cleared if a slave responds to the address.
;*
;* NOTE
;*	IMPORTANT! : This funtion must be directly followed by TWI_write.
;*
;***************************************************************************

TWI_start:				
	mov	TWIdata,TWIadr		; copy address to transmitt register
	sbi	DDRD,SDAP		; force SDA low
	rcall	TWI_qp_delay		; quarter period delay


;***************************************************************************
;*
;* FUNCTION
;*	TWI_write
;*
;* DESCRIPTION
;*	Writes data (one byte) to the TWI bus. Also used for sending
;*	the address.
;*
;* USAGE
;*	TWIdata - Contains data to be transmitted.
;*
;* RETURN
;*	Carry flag - Set if the slave respond transfer.
;*
;* NOTE
;*	IMPORTANT! : This funtion must be directly followed by TWI_get_ack.
;*
;***************************************************************************

TWI_write:
	sec				; set carry flag
	rol	TWIdata			; shift in carry and out bit one
	rjmp	TWI_write_first
TWI_write_bit:
	lsl	TWIdata			; if transmit register empty
TWI_write_first:
	breq	TWI_get_ack		;	goto get acknowledge
	sbi	DDRD,SCLP		; force SCL low

	brcc	TWI_write_low		; if bit high
	nop				;	(equalize number of cycles)
	cbi	DDRD,SDAP		;	release SDA
	rjmp	TWI_write_high
TWI_write_low:				; else
	sbi	DDRD,SDAP		;	force SDA low
	rjmp	TWI_write_high		;	(equalize number of cycles)
TWI_write_high:
	rcall	TWI_hp_delay		; half period delay
	cbi	DDRD,SCLP		; release SCL
	rcall	TWI_hp_delay		; half period delay

	rjmp	TWI_write_bit


;***************************************************************************
;*
;* FUNCTION
;*	TWI_get_ack
;*
;* DESCRIPTION
;*	Get slave acknowledge response.
;*
;* USAGE
;*	(used only by TWI_write in this version)
;*
;* RETURN
;*	Carry flag - Cleared if a slave responds to a request.
;*
;***************************************************************************

TWI_get_ack:
	sbi	DDRD,SCLP		; force SCL low
	cbi	DDRD,SDAP		; release SDA
	rcall	TWI_hp_delay		; half period delay
	cbi	DDRD,SCLP		; release SCL

TWI_get_ack_wait:
	sbis	PIND,SCLP		; wait SCL high 
					;(In case wait states are inserted)
	rjmp	TWI_get_ack_wait

	clc				; clear carry flag
	sbic	PIND,SDAP		; if SDA is high
	sec				;	set carry flag
	rcall	TWI_hp_delay		; half period delay
	ret


;***************************************************************************
;*
;* FUNCTION
;*	TWI_do_transfer
;*
;* DESCRIPTION
;*	Executes a transfer on bus. This is only a combination of TWI_read
;*	and TWI_write for convenience.
;*
;* USAGE
;*	TWIadr - Must have the same direction as when TWI_start was called.
;*	see TWI_read and TWI_write for more information.
;*
;* RETURN
;*	(depends on type of transfer, read or write)
;*
;* NOTE
;*	IMPORTANT! : This funtion must be directly followed by TWI_read.
;*
;***************************************************************************

TWI_do_transfer:
	sbrs	TWIadr,b_dir		; if dir = write
	rjmp	TWI_write		;	goto write data


;***************************************************************************
;*
;* FUNCTION
;*	TWI_read
;*
;* DESCRIPTION
;*	Reads data (one byte) from the TWI bus.
;*
;* USAGE
;*	Carry flag - 	If set no acknowledge is given to the slave
;*			indicating last read operation before a STOP.
;*			If cleared acknowledge is given to the slave
;*			indicating more data.
;*
;* RETURN
;*	TWIdata - Contains received data.
;*
;* NOTE
;*	IMPORTANT! : This funtion must be directly followed by TWI_put_ack.
;*
;***************************************************************************

TWI_read:
	rol	TWIstat			; store acknowledge
					; (used by TWI_put_ack)
	ldi	TWIdata,0x01		; data = 0x01
TWI_read_bit:				; do
	sbi	DDRD,SCLP		; 	force SCL low
	rcall	TWI_hp_delay		;	half period delay

	cbi	DDRD,SCLP		;	release SCL
	rcall	TWI_hp_delay		;	half period delay

	clc				;	clear carry flag
	sbic	PIND,SDAP		;	if SDA is high
	sec				;		set carry flag

	rol	TWIdata			; 	store data bit
	brcc	TWI_read_bit		; while receive register not full


;***************************************************************************
;*
;* FUNCTION
;*	TWI_put_ack
;*
;* DESCRIPTION
;*	Put acknowledge.
;*
;* USAGE
;*	(used only by TWI_read in this version)
;*
;* RETURN
;*	none
;*
;***************************************************************************

TWI_put_ack:
	sbi	DDRD,SCLP		; force SCL low

	ror	TWIstat			; get status bit
	brcc	TWI_put_ack_low		; if bit low goto assert low
	cbi	DDRD,SDAP		;	release SDA
	rjmp	TWI_put_ack_high
TWI_put_ack_low:			; else
	sbi	DDRD,SDAP		;	force SDA low
TWI_put_ack_high:

	rcall	TWI_hp_delay		; half period delay
	cbi	DDRD,SCLP		; release SCL
TWI_put_ack_wait:
	sbis	PIND,SCLP		; wait SCL high
	rjmp	TWI_put_ack_wait
	rcall	TWI_hp_delay		; half period delay

	cbi	DDRD,SDAP		; release SDA 

	ret


;***************************************************************************
;*
;* FUNCTION
;*	TWI_stop
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

TWI_stop:
	sbi	DDRD,SCLP		; force SCL low
	sbi	DDRD,SDAP		; force SDA low
	rcall	TWI_hp_delay		; half period delay
	cbi	DDRD,SCLP		; release SCL
	rcall	TWI_qp_delay		; quarter period delay
	cbi	DDRD,SDAP		; release SDA
	rcall	TWI_hp_delay		; half period delay
	ret


;***************************************************************************
;*
;* FUNCTION
;*	TWI_init
;*
;* DESCRIPTION
;*	Initialization of the TWI bus interface.
;*
;* USAGE
;*	Call this function once to initialize the TWI bus. No parameters
;*	are required.
;*
;* RETURN
;*	None
;*
;* NOTE
;*	PORTD and DDRD pins not used by the TWI bus interface will be
;*	set to Hi-Z (!).
;*
;* COMMENT
;*	This function can be combined with other PORTD initializations.
;*
;***************************************************************************

TWI_init:
	clr	TWIstat			; clear TWI status register (used
					; as a temporary register)
	out	PORTD,TWIstat		; set TWI pins to open colector
	out	DDRD,TWIstat
	ret


;***************************************************************************
;*
;* PROGRAM
;*	main - Test of TWI master implementation
;*
;* DESCRIPTION
;*	Initializes TWI interface and shows an example of using it.
;*
;***************************************************************************

RESET:
	;***
	;Init stack pointer for AVR devices with SRAM
	;***

main:	rcall	TWI_init		; initialize TWI interface

;**** Write data => Adr(00) = 0x55 ****

	ldi	TWIadr,$A0+TWIwr	; Set device address and write
	rcall	TWI_start		; Send start condition and address

	ldi	TWIdata,$00		; Write word address (0x00)
	rcall	TWI_do_transfer		; Execute transfer

	ldi	TWIdata,$55		; Set write data to 01010101b
	rcall	TWI_do_transfer		; Execute transfer

	rcall	TWI_stop		; Send stop condition

;**** Read data => TWIdata = Adr(00) ****

	ldi	TWIadr,$A0+TWIwr	; Set device address and write
	rcall	TWI_start		; Send start condition and address

	ldi	TWIdata,$00		; Write word address
	rcall	TWI_do_transfer		; Execute transfer

	ldi	TWIadr,$A0+TWIrd	; Set device address and read
	rcall	TWI_rep_start		; Send repeated start condition and address

	sec				; Set no acknowledge (read is followed by a stop condition)
	rcall	TWI_do_transfer		; Execute transfer (read)

	rcall	TWI_stop		; Send stop condition - releases bus

	rjmp	main			; Loop forewer

;**** End of File ****

