;**** MODIFIED   N O T E   A V R 3 0 2 ************************
;*
;* Title		: TWI Slave, received TWI data sent out hardware UART at 19200 8N1 
;* Version		: 0.001
;* Last updated		:
;*			 
;*
;* Interrupt Usage	: External Interrupt0,
;*			  Timer/Counter0 overflow interrupt
;*
;* DESCRIPTION
;* 	This application note shows how to implement an AVR device
;*	as an TWI slave peripheral. Sending TWI data out UART at 19200 8N1
;*	Received data is stored in r0 and r0 is sent to temp2 to be sent out UART
;*
;*	Some features :
;*	* Interrupt based (using INT0 and TIM0).
;*	* Easy insertion of "wait states".
;*	* Size and speed optimized.
;*	* Supports wake-up from idle mode.
;*
;*	Refers to the application note AVR302 and AVR Freaks DN004 documentation for more
;*	detailed description.
;*
;* USAGE
;*	Insert user code in the two locations marked "insert user code here".
;*
;* NOTES
;*	Minimum one instruction will be executed between each interrupt.
;*
;* STATISTICS
;*	Code Size	: 160
;*	Register Usage	: 5 High, 0 Low
;*	Interrupt Usage	: EXT_INT0 and TIM0_OVF
;*	Port Usage	: PD4(T0) and PD2(INT0)
;*	XTAL		: TWI Standard Mode : min  3.0MHz
;*
;***************************************************************************

;**** Includes ****

.include "8515def.inc"

;**** Global TWI Constants ****

.equ	pinmask = 0x14		; <=> (1<<PD4)+(1<<PD2)

;**** Global Register Variables ****

.def	temp	= r16		; Temporary variable
.def	etemp	= r17		; Extra temporary variable
.def	TWIdata	= r18		; TWI data register
.def	TWIadr	= r19		; TWI address and direction register
.def	TWIstat	= r20		; TWI temporary SREG storage
.def	temp2	= r21		; uart temp
;**** Interrupt Vectors ****

	rjmp	RESET		; Reset handle
	rjmp	EXT_INT0	; INT0 handle
	rjmp	TIM0_OVF	; Timer 0 overflow handle
;	( rjmp	ANA_COMP )	; ( Analog comparator handle )


;***************************************************************************
;*
;* INTERRUPT
;*	EXT_INT0 / TWI_wakeup
;*
;* DESCRIPTION
;*	Detects the start condition and handles the data transfer on 
;*	the TWI bus.
;*
;*	Received data is stored in the r0 register and r0 is put in the
;*      UDR register to be transmitted out 19200 8N1
;*
;* NOTE
;*	This interrupt code, in some cases, jumps to code parts in the 
;*	TIM0_OVF / TWI_skip section to reduce code size.
;*
;***************************************************************************

EXT_INT0:			; INT0 handle
	in	TWIstat,SREG	; store SREG


;*******************************
;* set up uart                 *
;*******************************

	ldi temp2,low(RAMEND)
	out SPL,temp2
	ldi temp2,high(RAMEND)
	out SPH,temp  ;init Stack Pointer
	rjump start   ;reset handler


; initialize UART

initialize:            ;baudrate in temp2

	out	UBRR,temp

;enable transmitter

	ldi temp2,(1<<TXEN)
	out UCR,temp
	ret

;*******************************
;* Receive Data (master write) *
;*******************************

TWI_master_write:

wlo_mw0:sbic	PIND,PIND4	; wait for SCL low
	rjmp	wlo_mw0
	cbi	DDRD,DDD2	; remove acknowledge from SDA

whi_mw:	sbis	PIND,PIND4	; wait for SCL high
	rjmp	whi_mw

	in	temp,PIND	; sample SDA (first bit) and SCL
	andi	temp,pinmask	; mask out SDA and SCL
do_mw:				; do
	in	etemp,PIND	;	new sample
	andi	etemp,pinmask	;	mask out SDA and SCL
	cp	etemp,temp
	breq	do_mw		; while no change

	sbrs	etemp,PIND4	; if SCL changed to low
	rjmp	receive_data	;	goto receive data

	sbrs	etemp,PIND2	; if SDA changed to low
	rjmp	TWI_get_adr	;	goto repeated start
				; else
	rjmp	TWI_stop	;	goto transfer stop

receive_data:
	ldi	TWIdata,2	; set TWIdata MSB to zero
	sbrc	temp,PIND2	; if SDA sample is one
	ldi	TWIdata,3	;	set TWIdata MSB to one

do_rd:				; do
wlo_rd:	sbic	PIND,PIND4	;	wait for SCL low
	rjmp	wlo_rd
	sec			; 	set carry
whi_rd:	sbis	PIND,PIND4	;	wait for SCL high
	rjmp	whi_rd

	sbis	PIND,PIND2	; 	if SDA low
	clc			;		clear carry
	rol	TWIdata		; 	shift carry into data register

	brcc	do_rd		; while register not full

	; insert wait states here if nessesary
;	sbi	DDRD,DDD4	; (start wait state)

;! INSERT USER CODE HERE !
;!NOTE!	If the user code is more than TWO instruction then wait
;	states MUST be inserted as shown in description.

	mov	r0,TWIdata	; Store data receved in "serial"




;*****MY CODE IS HERE  ************	
	mov	temp2,r0		; move data to UDR reg
transmit:
	sbis 	USR,UDRE        ; ready to send?
	rjmp	transmit
	out	UDR,temp2
	ret
;*****END OF MY CODE   **************

;	cbi	DDRD,DDD4	; (end wait state)

	rjmp	TWI_master_write; Start on next transfer


;***************************************************************************
;*
;* INTERRUPT
;*	TIM0_OVF / TWI_skip
;*
;* DESCRIPTION
;*	This interrupt handles a "address miss". If the slave device is
;*	not addressed by a master, it will not acknowledge the address.
;*
;*	Instead of waiting for a new start condition in a "busy loop"
;*	the slave set up the counter to count 8 falling edges on SCL and 
;*	returns from the current interrupt. This "skipping" of data
;*	do not occupies any processor time. When 8 egdes are counted, a
;*	timer overflow interrupt handling routine (this one) will check
;*	the next condition that accure. If it's a stop condition
;*	the transfer ends, if it's a repeated start condition a jump
;*	to the TWI_wakeup interrupt will read the new address. If a new
;*	transfer is initiated the "skipping" process is repeated.
;*
;***************************************************************************

TIM0_OVF:			; ( Timer 0 overflow handle )
	in	TWIstat,SREG	; store SREG

TWI_adr_miss:
	
;**** Drop Acknowledge ****

whi_dac:sbis	PIND,PIND4	; wait for SCL high
	rjmp	whi_dac
wlo_dac:sbic	PIND,PIND4	; wait for SCL low
	rjmp	wlo_dac

	; disable timer 0 overflow interrupt
	ldi	temp,(0<<TOIE0)
	out	TIMSK,temp
	; enable external interrupt request 0
	ldi	temp,(1<<INT0)
	out	GIMSK,temp


;************************
;* Wait for a Condition *
;************************

TWI_wait_cond:
whi_wc:	sbis	PIND,PIND4	; wait for SCL high
	rjmp	whi_wc

	in	temp,PIND	; sample SDA (first bit) and SCL
	andi	temp,pinmask	; mask out SDA and SCL
do_wc:				; do
	in	etemp,PIND	;	new sample
	andi	etemp,pinmask	;	mask out SDA and SCL
	cp	etemp,temp
	breq	do_wc		; while no change

	sbrs	etemp,PIND4	; if SCL changed to low
	rjmp	TWI_skip_byte	;	goto skip byte

	sbrs	etemp,PIND2	; if SDA changed to low
	rjmp	TWI_get_adr	;	goto repeated start
				; else
				;	goto transfer stop


;*************************
;* Handle Stop Condition *
;*************************

TWI_stop:
	;! Set INT0 to generate an interrupt on low level,
	;! then set INT0 to generate an interrupt on falling edge.
	;! This will clear the EXT_INT0 request flag.
	ldi	temp,(0<<ISC01)+(0<<ISC00)
	out	MCUCR,temp
	ldi	temp,(1<<ISC01)+(0<<ISC00)
	out	MCUCR,temp

	out	SREG,TWIstat	; restore SREG
	reti			; return from interrupt


;****************
;* Skip byte(s) *
;****************

TWI_skip_byte:
	; set counter initial value
	ldi	temp,-7
	out	TCNT0,temp
	; enable timer 0 overflow interrupt
	ldi	temp,(1<<TOIE0)
	out	TIMSK,temp
	; disable external interrupt request 0
	ldi	temp,(0<<INT0)
	out	GIMSK,temp

	out	SREG,TWIstat	; restore SREG
	reti


;***************************************************************************
;*
;* FUNCTION
;*	TWI_init
;*
;* DESCRIPTION
;*	Initialization of interrupts and port used by the TWI interface
;*	and waits for the first start condition.
;*
;* USAGE
;*	Jump to this code directly after a reset.
;*
;* RETURN
;*	none
;*
;* NOTE
;*	Code size can be reduced by 12 instructions (words) if no transfer
;*	is started on the TWI bus (bus free), before the interrupt
;*	initialization is finished. If this can be ensured, remove the
;*	"Wait for TWI Start Condition" part and replace it with a "sei"
;*	instruction.
;*
;***************************************************************************

RESET:
TWI_init:

;**** PORT Initialization ****

	; Initialize PD2 (INT0) for open colector operation (SDA in/out)
	ldi	temp,(0<<DDD4)+(0<<DDD2)
	out	DDRD,temp

	; Initialize PD4 (T0) for open colector operation (SCL in/out)
	ldi	temp,(0<<PD4)+(0<<PD2)
	out	PORTD,temp

;**** Interrupt Initialization ****

	; Set INT0 to generate an interrupt on falling edge
	ldi	temp,(1<<ISC01)+(0<<ISC00)
	out	MCUCR,temp

	; Enable INT0
	ldi	temp,(1<<INT0)
	out	GIMSK,temp

	; Set clock to count on falling edge of T0
	ldi	temp,(1<<CS02)+(1<<CS01)+(0<<CS00)
	out	TCCR0,temp


;***************************************************
;* Wait for TWI Start Condition (13 intstructions) *
;***************************************************

	sei

;***************************************************************************
;*
;* PROGRAM
;*	main - Test of TWI slave implementation
;*
;***************************************************************************

main:	rjmp	main		; loop forever


;**** End of File ****

