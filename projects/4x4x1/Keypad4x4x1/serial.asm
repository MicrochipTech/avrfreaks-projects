; Serial port routines

SER_MSGSEND:
; Expects address of a serial message in ZH:ZL
; And that serial message terminated with $00
	; Beware - AVRs use 16-bit aligned addresses
	; And sometimes those compiler warnings mean a split
	; serial line might have an extra padded $00 where
	; you might not want it.

	lpm		TxByte, Z+
	tst		TxByte
	breq	SER_MSGSENDa
	rcall	PUT_CHAR
	rjmp 	SER_MSGSEND
SER_MSGSENDa:
ret

PUT_CHAR:
	push ZL
	push ZH

	ldi ZL, $C0			; Set Z reg address UART status
	clr ZH
PUT_CHARa:
	ld temp, Z				; Get data therefrom
	sbrs temp, 5			; Bit 5 of USR = 'ready for more data'
	rjmp PUT_CHARa			;

	ldi ZL, UDR0			; Set Z reg address
	st Z, TxByte				; Put to UART

	pop ZH
	pop ZL
ret

GET_CHAR:
	push ZL
	push ZH

	ldi ZL, $C0
	clr ZH
GET_CHARa:
	ld temp, Z				; Get data therefrom
	sbrs temp, 7			; Bit 7 - RX Complete
	rjmp GET_CHARa

	ldi ZL, UDR0				; Set Z reg address
	ld RxByte, Z				; Get data from it

	pop ZH
	pop ZL
ret
