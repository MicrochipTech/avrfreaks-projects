.include "2313def.inc" 



.dseg

.cseg


.org 0x000
	rjmp		RESET
.org 0x001 
	reti 						;rjmp EXT_INT0 ; IRQ0 Handler
.org 0x002 
	reti						;rjmp EXT_INT1 ; IRQ1 Handler
.org 0x003 
	reti						;rjmp TIM_CAPT1 ; Timer1 Capture Handler
.org 0x004 
	reti						;rjmp TIM_COMP1 ; Timer1 Compare Handler
.org 0x005 
	reti						;rjmp TIM_OVF1 ; Timer1 Overflow Handler
.org 0x006 
	reti						;rjmp TIM_OVF0 ; Timer0 Overflow Handler
.org 0x007 
	rjmp RecvByteIRQ			;rjmp UART_RXC ; UART RX Complete Handler
.org 0x008 
	reti						;rjmp UART_DRE ; UDR Empty Handler
.org 0x009 
	reti						;rjmp UART_TXC ; UART TX Complete Handler
.org 0x00a 
	reti						;rjmp ANA_COMP



.org 0x00b
.include "main.asm"
.include "serial.asm"



;---------------------------------
; OnReset
;---------------------------------
RESET:

	InitStack
	InitSerial

	ldi		R16, 1
	sts		liczba_kanalow, R16			; zapisanie do pamieci liczby kanalow = 1

	rjmp	main
;=================================

zapal0:
	sbi		portd, PD2
	sbi		portd, PD3
	sbi		portd, PD4
	sbi		portd, PD5
	rjmp	main

zapal1:
	cbi		portd, PD2
	sbi		portd, PD3
	sbi		portd, PD4
	sbi		portd, PD5
	rjmp	main

zapal2:
	cbi		portd, PD2
	cbi		portd, PD3
	sbi		portd, PD4
	sbi		portd, PD5
	rjmp	main

zapal3:
	cbi		portd, PD2
	cbi		portd, PD3
	cbi		portd, PD4
	sbi		portd, PD5
	rjmp	main

zapal4:
	cbi		portd, PD2
	cbi		portd, PD3
	cbi		portd, PD4
	cbi		portd, PD5
	rjmp	main
	

dodatnie:
		cpi uart_var, 132
		brcs zapal0
		cpi uart_var, 159
		brcs zapal1
		cpi uart_var, 191
		brcs zapal2
		cpi uart_var, 221
		brcs zapal3
		brcc zapal4
	ret

ujemne:
		cpi uart_var, 116
		brcc zapal0
		cpi uart_var, 96
		brcc zapal1
		cpi uart_var, 64
		brcc zapal2
		cpi uart_var, 32
		brcc zapal3
		brcs zapal4
	ret



main:
	ldi	R29, 0xff
	out ddrb, R29

	sbi ddrd, PD2
	sbi ddrd, PD3
	sbi ddrd, PD4
	sbi ddrd, PD5

	clc
main_loop:
	out portb, uart_var

	cpi uart_var, 127
	brcc dodatnie
	brcs ujemne
main_loop_end:
	rjmp	main_loop
;=================================


