;********************************************************
; Author : Nay Oo
; Email  : nayoo1979@yahoo.com.sg
;		   Please do contact me by this email if you have 
;		   any suggestions or questions.	
;
; DESCRIPTION :
;		  Please refer to AVR Freaks Design Note #043 for
;         more indept understanding of string manipulation
;         using stack. 
;         This is just the study program of mine and I hope 
;         that it could also be useful to you.    
; Version :
;		  2.0
;         tested with STK500
;         uController = AT90S8515
;         Software  = Hyper Terminal
;         Press reset for repitative display of "Hello World"
;         Try it out and enjoy. :)
; Date :
;		   17/11/03
;*********************************************************

.include "8515def.inc"
.def	temp=r16
.def	Delay =r18; Delay variable 1
.def	Delay2 =r19; Delay variable 

; ************** Main Program Register Variables *********


.org 0x0000
		rjmp	Reset			; Program Reset routine

; **********  Send the character to PC via uart *************
Uart2Pc:
		ldi		r17,23			; Set the baud register to 23
		out		UBRR,r17		; 9600 baud
		sbi		UCR,TXEN		; UART initialization

		out		UDR,r16

wait:
		sbis	USR,TXC
		rjmp	wait

		ldi		Delay2,10
DLY:
		dec		Delay
		brne	DLY
		dec		Delay2
		brne	DLY

		ret
; **********  Buffer routine to display character ************
DisplayChar:
		rcall	Uart2Pc
		ret


DisplayRom:
		pop		zh
		pop		zl				; move stack pointer to Z-register
		lsl		zl			
		rol		zh

DR1:	
		lpm						; byte character from rom
		adiw	zl,1			; inc Z-register
		mov		r16,r0				
		cpi		r16,0			; test for the end of string
		breq	Rdone			; jump when end of string
		rcall 	DisplayChar		; send data
		rjmp	DR1
Rdone:
		lsr		zh				; restore the stack by pointing
		ror		zl				; Z just past the rom-based string
		push	zl				; then push it on the stack so
		push	zh				; the return operation places it in
		ret						; the program counter

; **********************************************************
; Reset 
; **********************************************************

Reset:
		ldi		temp,low(RAMEND)	; stack pointer initialization
		out		SPL,temp
		ldi		temp,high(RAMEND)
		out		SPH,temp
	
				

		rcall	DisplayRom
		.db		"Hello World",0

forever:rjmp	forever			; infinite loop









