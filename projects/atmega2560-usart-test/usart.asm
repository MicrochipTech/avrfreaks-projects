;
.nolist
.include "m2560def.inc"
.list
;
.def	temp		= r16	;
.def	temp1		= r17	; used for sending data to device/host
;
.cseg
.org	0
;
Reset:
	jmp   Start      ;
;****************************************************
Start:
	ldi	temp,LOW(RAMEND)	;set up stack pointer
	out	SPL,temp
	ldi	temp,HIGH(RAMEND)
	out	SPH,temp

	call	USART_Init
;----------------------------------------------------
MainLoop:
	RCALL	Usart_RX
	RCALL	Usart_TX
	rjmp	MainLoop
;-----------------------------------------------------
.equ   baud=9600      ;Baud rate
.equ   fosc=16000000   ;Crystal frequency
;-----------------------------------------------------
USART_Init:
	ldi temp1, high(fosc/(16*baud)-1)
	ldi temp, low(fosc/(16*baud)-1) 

	sts UBRR0H, temp1		; Set baud rate
	sts UBRR0L, temp

	ldi temp, (1<<RXEN0)|(1<<TXEN0)	; Enable receiver and transmitter
	sts UCSR0B,temp

	ldi temp, (2<<UMSEL00)|(3<<UCSZ00)	; Set frame format: 8data, 1stop bit
	sts UCSR0C,temp
	ret
;-----------------------------------------------------
Usart_TX:
	lds	temp,UCSR0A		;
	sbrs	temp,UDRE0		;Wait for empty transmit buffer
	rjmp	Usart_TX		;

	sts	UDR0,temp1		; Put data (temp1) into buffer, sends the data
	ret
;-----------------------------------------------------
Usart_RX:
	lds	temp, UCSR0A	; Wait for data to be received
	sbrs	temp, RXC0
	rjmp	Usart_RX

	lds temp1, UDR0		; Get and return received data from buffer
	ret
;-----------------------------------------------------
.exit