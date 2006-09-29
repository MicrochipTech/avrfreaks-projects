
;RS232 defs (assuming 8MHz clock)
.equ usarta_ctl = 0x02	;set the 2x bit of control reg A
.equ usartb_ctl = 0x18	;enable rx and tx, no irqs
.equ usartc_ctl = 0x86	;async, 8 bit, no parity
.equ baud2x_HB = 0
.equ baud2x_LB = 105		;value for UBRR for 9600 baud in 2x mode


Init232:
			;This is the serial port initialization routine
			;
			;Currently, it just sets up the port for 9600 baud, 8 bit, no parity
			; assuming that the defs are correct
	ldi	Temp, usarta_ctl
	out UCSR0A, Temp

	ldi	Temp,usartb_ctl
	out UCSR0B, Temp
	ldi Temp, usartc_ctl
	ldi XL, UCSR0C
	st	X,Temp
	ldi	Temp, Baud2x_LB
	out UBRR0L, Temp
	ret

USART_Transmit:
	wdr						; reset the WD once since this should take 1ms, not longer
USART_Transmit_1:
	sbis UCSR0A,UDRE0		; Wait for empty transmit buffer
	rjmp USART_Transmit_1

	out UDR0,Temp		;	 Put data Temp into buffer, sends the data
	inc count
	ret


USART_Hex_Tx:			;send Temp out the serial port as ascii hex
	mov T2, Temp		;save a copy of Temp
	ldi T3, 7
	andi Temp, 0xF0
	lsr Temp
	lsr Temp
	lsr Temp
	lsr Temp
	cpi	Temp, 10
	brlt	U_H_T_1
	add Temp, T3
U_H_T_1:
	add Temp, ASCII_Offset
	call USART_Transmit
	mov Temp, T2
	andi Temp, 0x0F
	cpi Temp, 10
	brlt U_H_T_2
	add Temp, T3
U_H_T_2:
	add Temp, ASCII_Offset
	call USART_Transmit

	ret

USART_Dec_Tx:			;send Temp out the serial port as ascii decimal (unsigned, 0-255)
	mov T2, Temp		;This is crude, but it works.
	mov	T3, Temp
	cpi	Temp, 200
	brlo	UDT_1

	ldi	Temp, '2'
	rcall	USART_Transmit
	mov Temp, T2
	subi	Temp, 200
	mov	T2, Temp
	rjmp UDT_2
UDT_1:
	cpi	Temp, 100
	brlo	UDT_3

	ldi	Temp, '1'
	rcall	USART_Transmit
	mov Temp, T2
	subi	Temp, 100
	mov	T2, Temp
	rjmp	UDT_2
UDT_3:					;print a leading 0
;	ldi	Temp, '0'
;	rcall	USART_Transmit
;	mov Temp, T2

UDT_2:					;check for 10's
	cpi	Temp, 10
	brsh	UDT_4

	cp	Temp, T3		;if none, check to see if we already printed something, and if so, print 0
	breq	UDT_5
	ldi Temp, '0'
	rcall	USART_Transmit
	mov	Temp, T2
	rjmp	UDT_5
UDT_4:
	clr T3
UDT_4_1:
	subi	Temp, 10	;count the 10's
	inc	T3
	cpi	Temp, 10
	brsh	UDT_4_1

	mov T2, Temp
	mov Temp, T3
	add	Temp, ASCII_Offset
	rcall	USART_Transmit
	mov Temp, T2

UDT_5:
	add Temp, ASCII_Offset	;print the 1's
	rcall	USART_Transmit

	ret


USART_SDec_Tx:			;send Temp out the serial port as ascii decimal (signed, +-127) without leading 0's
	mov T2, Temp		;This is crude, but it works.

	sbrc	Temp, 7
	jmp	USDT_1_1
	jmp USDT_1
USDT_1_1:
	ldi	Temp, '-'
	rcall	USART_Transmit
	neg	T2
	mov	Temp, T2
USDT_1:	
	rcall USART_Dec_Tx

	ret


USART_Space:
	st	-Y, Temp
	ldi	Temp, 0x20	;ascii space
	rcall USART_Transmit
	ld	Temp, Y+
	ret

USART_CRLF:
	st	-Y, Temp

	ldi Temp, ASCII_CR
	rcall USART_Transmit
	ldi Temp, ASCII_LF
	rcall USART_Transmit

	ld	Temp, Y+
	ret

USART_PStr:				;output the null terminated string pointed to by Z
	lpm	Temp, Z+
	cpi	Temp, 0
	brne	UPS1
	ret
UPS1:
	rcall	USART_Transmit
	rjmp USART_PStr

Mon_Prompt:
	rcall USART_CRLF
	ldi	Temp, '>'
	rcall USART_Transmit
	rcall USART_Space
	ret

USART_Bin_Tx:	;send Temp out the serial port as ascii binary
						;look at the bits and output "0" or "1"
	ldi T2, 0x8			;bit counter
	mov T3, Temp		;too many Temps...

U_A_T_lp:
	lsl	T3
	brcs U_A_T_1
	ldi Temp, 0x30		;output a "0"
	rjmp U_A_T_nxt
U_A_T_1:
	ldi Temp, 0x31
U_A_T_nxt:
	call USART_Transmit

	dec T2
	brne U_A_T_lp

	ldi Temp, ASCII_CR
	call USART_Transmit
	ldi Temp, ASCII_LF
	call USART_Transmit

	ret


USART_Receive:
	wdr
	lds	Temp, Test_Chk_Flg
	cpi	Temp, 4
	brlo	UR_0
	rjmp	reset			;if we've been sitting here 4 or more Test check intervals, reset ourselves.
UR_0:
	sbis UCSR0A, RXC0		; Wait for data to be received
	rjmp USART_Receive

	in Temp, UDR0			; Get and return received data from buffer
	andi	Temp, 0x7F
	ret

