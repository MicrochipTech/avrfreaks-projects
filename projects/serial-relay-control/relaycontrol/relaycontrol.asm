; This program reads data from the serial port and sets output pins according to the input.
; It includes 4 functions, Help, Enable, Disable and Status.
; At the top of this file the text messages are defined in eeprom

;Code included from AVR304: Half Duplex Interrupt Driven Software UART

.include "tn26def.inc"			;Made for ATTiny26 @1MHz, should be easily portable to any processor with TIM0 and INT0

;***** Registry definitions
.def		tmp		= r21		;Temp. register
.def		buffer	= r22		;Recieved byte
.def		adr		= r23		;EEPROM Address

;***** Port pins
.equ	TX=PB4
.equ	RX=PB6 ;INT0


.eseg	; Special characters: 0 = End of string, 1 = get specific data
t_help:		.db "Ex = Enable output x (1-8)",0x0a,0x0d,"Dx = Disable output x",0x0a,0x0d,"S = Status",0x0a,0x0d,0
t_enable:	.db	"ENABLE ",1,0x0a,0x0d,0
t_disable:	.db "DISABLE ",1,0x0a,0x0d,0
t_status:	.db	"STATUS 1=",1, " 2=",1," 3=",1," 4=",1," 5=",1," 6=",1," 7=",1," 8=",1,0x0a,0x0d,0


.cseg
.org		0x0000
	rjmp	reset				;Reset vector

.org		INT0addr
	rjmp	ext_int0			;External interrupt handler

.org		OVF0addr
	rjmp	tim0_ovf			;Timer0 overflow handler

.include	"uart.asm"			;All the UART stuff

reset:							;Initialize
	ldi 	tmp, RAMEND
	out 	SP, tmp				;Setup stack pointer

	ser		tmp
	out		PORTB,tmp			;Set port B as input with pullups
	sbi		DDRB,TX				;    except TX pin
	out		DDRA,tmp			;Set port A as output

	rcall	uart_init			;Init UART
	sei							;Enable interrupts

main:
	sbrs	u_status,RDR		;Wait for Character
	rjmp	main
	mov		buffer,u_buffer		;Get recieved character
	ori		buffer, 0x20		;Force lowercase

	cpi		buffer, 'h'			;If input = h/H
	brne	PC+2
	rcall	help				;Print help text

	cpi		buffer, 'e'			;If input = e/E
	brne	PC+2
	rcall	enable				;Enable output

	cpi		buffer, 'd'			;If input = d/D
	brne	PC+2
	rcall	disable				;Disable output

	cpi		buffer, 's'			;If input = s/S
	brne	PC+2
	rcall	status				;Print status

	rjmp	main				;Infinite loop


help:
	ldi		adr,t_help			;Set pointer to EEPROM data
h_loop:
	out		EEAR,adr			;Set the EEPROM's address
	inc		adr					;Increase pointer
	sbi		EECR,EERE			;Send the Read strobe
	in		u_transmit, EEDR	;Put the data in the transmit register
	tst		u_transmit			;Check if zero
	breq	h_end				;End function if so
	rcall	uart_transmit		;Transmit the data
h_wait:
	sbrc	u_status,TD			;Wait until data is sent
	rjmp	h_wait
	rjmp	h_loop				;Continue sendig chars
h_end:
	ret							;Return to main loop


enable:
	ldi		adr, t_enable		;Set pointer to EEPROM data
e_loop:
	out		EEAR,adr			;Set the EEPROM's address
	inc		adr					;Increase pointer
	sbi		EECR,EERE			;Send the Read strobe
	in		u_transmit,EEDR		;Put the data in the transmit register
	tst		u_transmit			;Check if zero
	breq	e_end				;End function if so
	cpi		u_transmit, 1		;Check for a one
	breq	e_get				;Goto the 'get' part
	rcall	uart_transmit		;Transmit the data

e_wait:
	sbrc	u_status,TD			;Wait until data is sent
	rjmp	e_wait
	rjmp	e_loop				;Continue sendig chars

e_get:
	sbrs	u_status,RDR		;Wait for Character
	rjmp	e_get
	mov		buffer,u_buffer		;Get recieved character
	mov		u_transmit, buffer
	rcall	uart_transmit		;Print the received character
	cpi		buffer, '1'
	brne	PC+2
	sbi		PORTA, 0			;Enable output 1
	cpi		buffer, '2'
	brne	PC+2
	sbi		PORTA, 1			;Enable output 2
	cpi		buffer, '3'
	brne	PC+2
	sbi		PORTA, 2			;Enable output 3
	cpi		buffer, '4'
	brne	PC+2
	sbi		PORTA, 3			;Enable output 4
	cpi		buffer, '5'
	brne	PC+2
	sbi		PORTA, 4			;Enable output 5
	cpi		buffer, '6'
	brne	PC+2
	sbi		PORTA, 5			;Enable output 6
	cpi		buffer, '7'
	brne	PC+2
	sbi		PORTA, 6			;Enable output 7
	cpi		buffer, '8'
	brne	PC+2
	sbi		PORTA, 7			;Enable output 8

e_wait2:
	sbrc	u_status,TD			;Wait until data is sent
	rjmp	e_wait2
	rjmp	e_loop				;Continue sendig chars
e_end:
	ret							;Return to main loop



disable:
	ldi		adr, t_disable		;Set pointer to EEPROM data
d_loop:
	out		EEAR,adr			;Set the EEPROM's address
	inc		adr					;Increase pointer
	sbi		EECR,EERE			;Send the Read strobe
	in		u_transmit,EEDR		;Put the data in the transmit register
	tst		u_transmit			;Check if zero
	breq	d_end				;End function if so
	cpi		u_transmit, 1		;Check for a one
	breq	d_get				;Goto the 'get' part
	rcall	uart_transmit		;And transmit the data

d_wait:
	sbrc	u_status,TD			;Wait until data is sent
	rjmp	d_wait
	rjmp	d_loop				;Continue sendig chars

d_get:
	sbrs	u_status,RDR		;Wait for Character
	rjmp	d_get
	mov		buffer,u_buffer		;Get recieved character
	mov		u_transmit, buffer
	rcall	uart_transmit		;Print the received character
	cpi		buffer, '1'
	brne	PC+2
	cbi		PORTA, 0			;Disable output 1
	cpi		buffer, '2'
	brne	PC+2
	cbi		PORTA, 1			;Disable output 2
	cpi		buffer, '3'
	brne	PC+2
	cbi		PORTA, 2			;Disable output 3
	cpi		buffer, '4'
	brne	PC+2
	cbi		PORTA, 3			;Disable output 4
	cpi		buffer, '5'
	brne	PC+2
	cbi		PORTA, 4			;Disable output 5
	cpi		buffer, '6'
	brne	PC+2
	cbi		PORTA, 5			;Disable output 6
	cpi		buffer, '7'
	brne	PC+2
	cbi		PORTA, 6			;Disable output 7
	cpi		buffer, '8'
	brne	PC+2
	cbi		PORTA, 7			;Disable output 8

d_wait2:
	sbrc	u_status,TD			;Wait until data is sent
	rjmp	d_wait2
	rjmp	d_loop				;Continue sendig chars
d_end:
	ret							;Return to main loop



status:
	ldi		adr, t_status		;Set pointer to EEPROM data
	in		tmp, PORTA			;Get Port A status
s_loop:
	out		EEAR,adr			;Set the EEPROM's address
	inc		adr					;Increase pointer
	sbi		EECR,EERE			;Send the Read strobe
	in		u_transmit,EEDR		;Put the data in the transmit register
	tst		u_transmit			;Check if zero
	breq	s_end				;End function if so
	cpi		u_transmit, 1		;Check for a one
	breq	s_get				;Goto the 'get' part
	rcall	uart_transmit		;Transmit the data

s_wait:
	sbrc	u_status,TD			;Wait until data is sent
	rjmp	s_wait
	rjmp	s_loop				;Continue sendig chars

s_get:
	ldi		u_transmit, '0'		;Preload a 0
	lsr		tmp					;Shift the bits into carry
	brcc	PC+2				;Skip loading a 1 if carry = 0
	ldi		u_transmit, '1'
	rcall	uart_transmit		;Transmit the data

s_wait2:
	sbrc	u_status,TD			;Wait until data is sent
	rjmp	s_wait2
	rjmp	s_loop				;Continue sendig chars

s_end:
	ret							;Return to main loop

