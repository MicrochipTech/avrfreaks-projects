;	Serial interface routines
;
;	66 bytes memory usage
;
;	Michal Strug 2004
;
;		include this file at .org 0x00b

.def uart_var = R19

;---------------------------------
; Initialize
;---------------------------------
; use only with RecvByteIRQ interrupt handle from this file
.MACRO InitSerial
		ldi		R16, 12			; 12 baudrate = 57600 for 12 Mhz
		out		UBRR, R16		; 5 baudrate = 115200 for 11,0592 Mhz
;		ldi		R16, 0b00001000	; enable reciving, transmitting and reciving irq
		ldi		R16, 0b10011000	; enable reciving, transmitting and reciving irq
		out		UCR, R16
		sei						; glonal interrupt enable
.ENDM
;=================================


;---------------------------------
; Send byte from uart_var
;---------------------------------
SendByte:
		sbis	USR, UDRE		;ready to send?
		rjmp	SendByte
		out		UDR, uart_var
	ret

; Macro send byte
.MACRO _SendByte
	ldi		uart_var, @0
	rcall	SendByte
.ENDM
.MACRO __SendByte
	MOV		uart_var, @0
	rcall	SendByte
.ENDM
;=================================

;---------------------------------
; Send string from program memory ZH:ZL, (uintil 0x00)
;---------------------------------
SendString_prog:
			lpm
			mov		uart_var, R0
			ldi		R17, 0x00
			cp		R0, R17
			breq	SendString_prog_end
			rcall	SendByte
			inc		ZL
			brne	SendString_prog
		SendString_prog_end:
	ret

; Macro send string from label
.MACRO SendStrP 
	ldi		ZH, high( @0 <<1)
	ldi		ZL, low( @0 <<1)
	rcall	SendString_prog
.ENDM
;=================================

;---------------------------------
; Send string from sram memory XH:XL, (uintil 0x00)
;---------------------------------
SendString_mem:
			ld		uart_var, X+
			ldi		R17, 0x00
			cp		uart_var, R17
			breq	SendString_mem_end
			rcall	SendByte
			brne	SendString_mem
		SendString_mem_end:
	ret

; Macro send string from label
.MACRO SendStrM
	ldi		XH, high( @0 <<1)
	ldi		XL, low( @0 <<1)
	rcall	SendString_mem
.ENDM
;=================================

;---------------------------------
; Recive byte to uart_var !interrupt handle!
;---------------------------------
RecvByteIRQ:
		in		uart_var, UDR
		sec
	reti
;usage:
;.org 0x007 
;	rjmp	RecvByteIRQ
;=================================

;---------------------------------
; Recive byte to uart_var
;---------------------------------
; use only with above RecvByteIRQ interrupt handle
RecvByte:
			clc
		RecvByte_test:
			brcc	RecvByte_test
	ret
;=================================

;---------------------------------
; Recive string to sram memory (until 0x00)
;---------------------------------
; use only with above RecvByteIRQ interrupt handle
; be careful with length of reciving string
RecvString_mem:
			clc
		RecvString_test:
			brcc	RecvString_test
			st		X+, uart_var
			ldi		R17, 0x00
			cp		uart_var, R17
			brne	RecvString_mem
		RecvString_end:
	ret

; Macro recv string from label
.MACRO RecvStrM
	ldi		XH, high( @0 <<1 )
	ldi		XL, low( @0 <<1 )
	rcall	RecvString_mem
.ENDM
;=================================

