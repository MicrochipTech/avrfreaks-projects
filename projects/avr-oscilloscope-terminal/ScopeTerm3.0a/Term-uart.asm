; Term-uart.asm	-- Included source for Dutchtronix Oscilloscope Terminal
;
;  Copyright © 2008 Jan P.M. de Rie
;
;  All Rights Reserved
;
;  This file is part of the Dutchtronix Oscilloscope Clock Distribution.
;  Use, modification, and re-distribution is permitted subject to the
;  terms in the file named "LICENSE.TXT", which contains the full text
;  of the legal notices and should always accompany this Distribution.
;
;  This software is provided "AS IS" with NO WARRANTY OF ANY KIND.
;
;  This notice (including the copyright and warranty disclaimer)
;  must be included in all copies or derivations of this software.
;
; Baud:		4800	19200
; 16Mhz		207		51
; 20 Mhz	259		64
;
; Postpone Receive enable
;
UART_Init:
#if Mega32	
    LDI     r17,51		;baud 19200 at 16MHz Xtal 
	OUT     UBRRL,r17
	OUT		UBRRH,_0
	ldi		r17,(1<<TXC)|(1<<TXEN)
	out		UCSRB,r17
; set default 8 bits, no parity, 1 stop
	ldi		r17,((1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0))
	out		UCSRC,r17
	in		r17,UBRRH
	in		r17,UCSRC
#elif Mega168
	ldi		r17,64		;baud 19200 at 20MHz Xtal 
	sts		UBRR0L,r17
	sts		UBRR0H,_0
	sts		UCSR0A,_0
; set rx interrupt, tx interrupt, clear data empty interrupt
	ldi		r17,(1<<TXCIE0)|(1<<TXEN0)
	sts		UCSR0B,r17
; set default 8 bits, no parity, 1 stop
	ldi		r17,((1<<UCSZ01)|(1<<UCSZ00))
	sts		UCSR0C,r17
#endif
	stsi	BaudRate,19
	sts		UART_Ready,_1
#if DEBUG
;
; all memory cleared in RELEASE mode
;
	sts		RXBufferHead,_0
	sts		RXBufferTail,_0
#endif
	rcall	UARTFlushPendingOutput
	ret
;
; Enable Serial In. Postponed to work around a level converter bug
;
UART_EnableIn:
#if Mega32	
	in		r17,UCSRB
	ori		r17,(1<<RXCIE)|(1<<RXEN)
	out		UCSRB,r17
#elif Mega168
	lds		r17,UCSR0B
	ori		r17,(1<<RXCIE0)|(1<<RXEN0)
	sts		UCSR0B,r17
#endif
	ret
;
; Change Baud Rate to 4,800 bps
;
UARTSet4800:
#if Mega32	
    LDI     r17,207		;baud 4800 at 16MHz Xtal 
	OUT     UBRRL,r17
	OUT		UBRRH,_0
#elif Mega168
	ldi		r17,(259 & 0xff)		;baud 4800 at 20MHz Xtal 
	sts		UBRR0L,r17
	ldi		r17,(259 >> 8)
	sts		UBRR0H,r17
#endif
	stsi	BaudRate,4
	ret
;
; Change Baud Rate to 19,200 bps
;
UARTSet19200:
#if Mega32	
    LDI     r17,51		;baud 19200 at 16MHz Xtal 
	OUT     UBRRL,r17
	OUT		UBRRH,_0
#elif Mega168
	ldi		r17,64		;baud 19200 at 20MHz Xtal 
	sts		UBRR0L,r17
	sts		UBRR0H,_0
#endif
	stsi	BaudRate,19
	ret
;
;
; UART Transmit Complete Interrupt
;
SIG_USART_TRANS:
	push	r16
	in		r16,SREG
	push	r16
	push	r17
	pushw	Z
;
; First check the Transmit Buffer
;
	lds		r16,TXBufferIndex
	cpi		r16,0xff
	breq	TX_L200
	inc		r16
	ldiw	Z,TXBuffer
	add		ZL,r16
	adc		ZH,_0
	ld		r17,Z
	tst		r17
	brne	TX_L100
	ldi		r16,0xff
	sts		UART_Ready,_1
	rjmp	TX_L150
TX_L100:
#if Mega32	
	out		UDR,r17
#elif Mega168
	sts		UDR0,r17
#endif
TX_L150:
	sts		TXBufferIndex,r16
	rjmp	TX_L400
TX_L200:
;
; Now check for a pointer to flash data
;
	ldsw	Z,pUART_Buffer
	mov		r16,ZL
	or		r16,ZH
	breq	TX_L250			;NULL pointer
	addiw	Z,1
	lpm		r17,Z
	tst		r17
	brne	TX_L300
;
; NULL character reached
;
	clrsw	pUART_Buffer
TX_L250:
	sts		UART_Ready,_1
	rjmp	TX_L400
TX_L300:
#if Mega32	
	out		UDR,r17
#elif Mega168
	sts		UDR0,r17
#endif
	stsw	pUART_Buffer,Z
TX_L400:
	popw	Z
	pop		r17
	pop		r16
	out		SREG,r16
	pop		r16
	reti
;
SIG_USART_RECV:
	push	r16
	in		r16,SREG
	push	r16
	push	r17
	push	r18
	push	r19
	pushw	Z
#if Mega32	
	in		r18,UDR				;get received char
#elif Mega168
	lds		r18,UDR0			;get received char
#endif
	lds		r16,RXBufferHead	;is there room for incoming char
	mov		r19,r16				;save current value 
	inc		r16
	andi	r16,RXBUFFERMASK	;mod
	lds		r17,RXBufferTail
	cp		r16,r17				;if equal, there is no room
	brne	RECV_L50			;ok, store
	rjmp	RECV_L100			;out
RECV_L50:
	sts		RXBufferHead,r16	;update
	ldiw	Z,RXBuffer			;store incoming char
	add		ZL,r19
	adc		ZH,_0
	st		Z,r18
	sbr		G_FLAGS,(1<<fRcvdChar)
RECV_L100:
	popw	Z
	pop		r19
	pop		r18
	pop		r17
	pop		r16
	out		SREG,r16
	pop		r16
	reti
;
; argument in r24
;
UART_SendByte:
	push	r16
SB_L300:
	lds		r16,UART_Ready
	tst		r16
	breq	SB_L300
	sts		UART_Ready,_0
#if Mega32
	out		UDR,r24
#elif Mega168
	sts		UDR0,r24
#endif
	pop		r16
	ret	
;
; UART_ReceiveByte: return value in r24
;
; saves all registers
;
UART_ReceiveByte:
	sbrs	G_FLAGS,fRcvdChar
	rjmp	UART_ReceiveByte
	push	r16
	push	r17
	pushw	Z
	lds		r17,RXBufferHead
	lds		r16,RXBufferTail
;
; sanity check
;
	clr		r24
	cp		r16,r17
	breq	RB_L100				;should not happen
	ldiw	Z,RXBuffer
	add		ZL,r16
	adc		ZH,_0
	ld		r24,Z
	inc		r16					;update RBufferTail
	andi	r16,RXBUFFERMASK	;mod
	sts		RXBufferTail,r16
	cp		r16,r17				;buffer now empty?
	brne	RB_L200				;no, leave fRcvdChar on
RB_L100:
	cbr		G_FLAGS,(1<<fRcvdChar)
RB_L200:
	popw	Z
	pop		r17
	pop		r16
	ret

UART_ClrRXBuffer:
	push	r16
	cbr		G_FLAGS,(1<<fRcvdChar)
	sts		RXBufferHead,_0	
	sts		RXBufferTail,_0	
	pop		r16
	ret
;
; Print flash based text
; argument pBuf in Z
;
UART_PrintfProgStr:
	push	r16
	pushw	Z
PPS_L300:
	lds		r16,UART_Ready
	tst		r16
	breq	PPS_L300
	sts		UART_Ready,_0		;TODO we justed tested it 0
	sts		pUART_Buffer,ZL
	sts		pUART_Buffer+1,ZH
	lpm		r16,Z
#if Mega32	
	out		UDR,r16
#elif Mega168
	sts		UDR0,r16
#endif
	popw	Z
	pop		r16
	ret
;
; Print the text in the TXBuffer
;
UART_PrintTXBuffer:
	push	r16
	pushw	Z
PTX_L300:
	lds		r16,UART_Ready
	tst		r16
	breq	PTX_L300
	sts		UART_Ready,_0		;TODO just tested to be 0
	sts		TXBufferIndex,_0	;initial index
	ldiw	Z,TXBuffer
	ld		r16,Z				;first char
	tst		r16					;just in case
	brne	PTX_L400
;
; this should never happen
;
	ser		r16
	sts		TXBufferIndex,r16
	sts		UART_Ready,_1
	rjmp	PTX_L500
PTX_L400:
#if Mega32	
	out		UDR,r16
#elif Mega168
	sts		UDR0,r16			;transmit
#endif
PTX_L500:
	popw	Z
	pop	r16
	ret

#if DEBUG
;
; print value in r24 as 2 hex chars
; r24 used
;
UART_Printfu08:
	push	r0
	push    r24
    swap    r24
    andi    r24,0X0F
    rcall   Hex2Asc
	rcall	UART_SendByte
    pop     r24
    andi    r24,0X0F
    rcall   Hex2Asc
	rcall	UART_SendByte
	pop		r0
	ret
;
; print value in r24 and r25 as 4 hex chars
; r24 used
;
UART_Printfu16:
	push	r24
	mov		r24,r25
	rcall	UART_Printfu08
	pop		r24
	rcall	UART_Printfu08
	ret
;
; print value in r24, r25, r26, r27 as 4 hex chars
; r24 used
;
UART_Printfu32:
	push	r24
	push	r25
	mov		r24,r26
	mov		r25,r27
	rcall	UART_Printfu16
	pop		r25
	pop		r24
	rcall	UART_Printfu16
	ret
#endif
;
; return value in r24
;
UART_HasChar:
	clr		r24
	sbrc	G_FLAGS,fRcvdChar
	inc		r24
	ret
;
; return value in r24
;
UART_Is_Ready:
	lds		r24,UART_Ready
	ret

UARTFlushPendingOutput:
	ser		r16
	sts		TXBufferIndex,r16
	sts		pUART_Buffer,_0
	sts		pUART_Buffer+1,_0
	ret
