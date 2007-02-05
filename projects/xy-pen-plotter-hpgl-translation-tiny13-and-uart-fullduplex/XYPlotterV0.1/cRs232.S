;***************************************************************************
;*
;* Part are form the Atmel AVR305 Application Note
;*
;***************************************************************************
#include <avr\io.h>

		.equ RXDBit, 1
		.equ RXDPORT, (PINB-0x20)	
		.equ TXDBit, 0
		.equ TXDPORT, (PORTB-0x20)	
		.equ BaudDelay,162
		
		 
		// 162 = 9600 @ 9.6 MC 
		// 81 = 19200 @ 9.6 MC 
		// 38 = 38400 @ 9.6 MC
		.data
DelayTimer:
		.space	1

		.text					; switches to the .text section (code and ROM constants)
		.global	PutChar			; Sends a charater
		.global GetChar
		.global UART_delay
		.global	Puts

Puts:
		movw 	R30,R24			; Copy the Pointer
Puts1:
		lpm		R24,Z+
		tst		R24
		breq	putcharend		; return is the char is 0;
		rcall	PutChar	
		rjmp 	Puts1

PutChar:
		ldi		R16,11			;1+8+sb (sb is # of stop bits)
		com		R24				;Inverte everything
		sec						;Start bit
putchar0:	
		brcs	putchar1		;If carry set
		cbi		TXDPORT,TXDBit	; send a '1'
		rjmp	putchar2		;else	
putchar1:	
		sbi		TXDPORT,TXDBit	; send a '0'
		nop
putchar2:	
		rcall 	UART_delay		;One bit delay
		rcall 	UART_delay

		lsr	r24					;Get next bit
		dec	r16					;If not all bit sent
		brne	putchar0		;   send next
putcharend:						;else
		ret						;   return

GetChar:
		ldi 	R16,9			;8 data bit + 1 stop bit
getchar1:	
		sbis 	RXDPORT,RXDBit	;Wait for start bit
		rjmp 	getchar1
		rcall	UART_delay		;0.5 bit delay
getchar2:	
		rcall	UART_delay		;1 bit delay
		rcall	UART_delay		
		sec						;clear carry
		sbic 	RXDPORT,RXDBit	;if RX pin high
		clc						;
		dec 	R16				;If bit is stop bit
		breq 	getchar3		;   return
								;else
		ror 	R24				;   shift bit into Rxbyte
		rjmp 	getchar2		;   go get next
getchar3:	
		ret


;***************************************************************************
;*
;* "UART_delay"
;*
;* This delay subroutine generates the required delay between the bits when
;* transmitting and receiving bytes. The total execution time is set by the
;* constant "b":
;*
;*	3·b + 7 cycles (including rcall and ret)
;*
;* Number of words	:4 including return
;* Low registers used	:None
;* High registers used	:1 (temp)
;* Pointers used	:None
;*
;***************************************************************************




UART_delay:	
		ldi	R17,BaudDelay
UART_delay1:	
		dec	R17
		brne	UART_delay1
		ret

