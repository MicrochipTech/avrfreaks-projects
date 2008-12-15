; Bounce.asm
;
; Flash Studio+ & In-System-Debugger (ISD) "Bounce" demo.
; Use with any ATMEL AVR CPU that has internal SRAM and an RS232
; port or with Flash 5in1 AVR Starter Kits for added features.

.nolist
.include "m103def.inc"					; Compiling for the ATmega103
.list

; ****** USER SETTINGS ******

; Clock frequency - set this to your clock frequency
.equ FOSC = 3686400						; 3.6864Mhz
 
; Serial comms Baud rate - set this to your desired baud rate
.equ BAUD = 115200

; ***** END USER SETTINGS *****

.equ SIZEX = 16							; Size of bounce area
.equ SIZEY = 8

.def ballX = r1
.def ballY = r2
.def oldBallX = r3
.def oldBallY = r4
.def ballDX = r5
.def ballDY = r6

.dseg
.org $60
title:
		.byte 2*SIZEX					; Title
bounceArea:
		.byte SIZEY*SIZEX				; Bounce area

.cseg
.org $0
		jmp main						; Reset Vector

.cseg
.org $4
		jmp main						; Unused Interrupt Vectors
		jmp main
		jmp main
		jmp main
		jmp main
		jmp main
		jmp main
		jmp main
		jmp main
		jmp main
		jmp main
		
.cseg
titleText:
		.db "   Flash Demo      AVR Bounce   "
message:
		.db "                  THE WORLD'S      SMALLEST,       FASTEST,         NO-ICE,        IN-SYSTEM       DEBUGGER!                    "

UART0_Init:

		clr r24							; Disable while setting baud rate
		out UCR,r24
		ldi r24, (FOSC/(16*BAUD))-1;
		out UBRR, r24					; Set baud rate
		ldi r24, (1<<RXEN)|(1<<TXEN)
		out UCR, r24					; Enable Tx & Rx
		ret

INT0_Init:
		sbi PORTD,0
		ret

MakeLedRed:								; Turn on red LED
		in r24, DDRD
		andi r24, $f7
		out DDRD, r24
		ret

MakeLedGreen:							; Turn off red LED
		sbi DDRD, 3
		ret

DrawBall:								; Draw the Ball
		ldi r17, 'O'
		sbrs r17, 0						; Skip next instruction
EraseBall:
		ldi r17, ' '
		ldi XL, low(bounceArea)
		ldi XH, high(bounceArea)
		mov r16, ballY
		tst r16
		breq DrawBall_MulX
DrawBall_Mul:
		adiw XL, SIZEX
		dec r16
		brne DrawBall_Mul
DrawBall_MulX:
		clr r16
		add XL, ballX
		adc XH, r16
		st X, r17
		ret

main:
		ldi r16,0x0F					; Put stack near top of SRAM (0x0FFF)
		out SPH,r16
		ldi r16,0xFF
		out SPL,r16

		rcall INT0_Init					; Init Interrupt 0
		rcall UART0_Init				; Init the UART

		in r24, EIMSK					; Enable INT0 (ATmega103)
		ori r24, 1
		out EIMSK, r24

		sei								; Enable interrupts

										; Initialise Title
		ldi ZL, low(titleText*2)
		ldi ZH, high(titleText*2)
		ldi XL, low(title)
		ldi XH, high(title)
		ldi r16, 2*SIZEX
WriteTitle:
		lpm
		st X+, r0
		adiw ZL, 1
		dec r16
		brne WriteTitle
										; Initialise Bounce area
		ldi ZL, low(message*2)
		ldi ZH, high(message*2)
		ldi XL, low(bounceArea)
		ldi XH, high(bounceArea)
		ldi r16, SIZEY*SIZEX
WriteBounceArea:
		lpm
		st X+, r0
		adiw ZL, 1
		dec r16
		brne WriteBounceArea

		rcall MakeLedRed				; Turn on Red LED

		clr ballX						; Starting position (0, 3)
		ldi r24, 3
		mov ballY, r24

		rcall DrawBall					; Draw Ball

		mov oldBallX, ballX
		mov oldBallY, ballY

		clr ballDX						; Ball is moving right & down
		inc ballDX
		clr ballDY
		inc ballDY

MainLoop:

		call SetBreakHere				; Set a Hard Breakpoint here

		ldi ZL, low(message*2)			; Restore message
		ldi ZH, high(message*2)
		ldi XL, low(bounceArea)
		ldi XH, high(bounceArea)
		mov r16, oldBallY
		tst r16
		breq Restore_MulX
Restore_Mul:
		adiw ZL, SIZEX
		adiw XL, SIZEX
		dec r16
		brne Restore_Mul
Restore_MulX:
		clr r16
		add ZL, oldBallX
		adc ZH, r16
		add XL, oldBallX
		adc XH, r16
		lpm
		st X, r0

		add ballX, ballDX				; Update ball's position
		add ballY, ballDY

		rcall DrawBall					; Draw the Ball

TestBallX:
		tst ballX						; If ball is at left or right edge
		breq BallOnXEdge
		mov r24, ballX
		cpi r24, SIZEX-1
		brne TestBallY
BallOnXEdge:
		neg ballDX						; Change direction
		rcall MakeLedGreen
		
TestBallY:
		tst ballY						; If ball is at top or bottom edge
		breq BallOnYEdge
		mov r24, ballY
		cpi r24, SIZEY-1
		brne SaveBallPos
BallOnYEdge:
		neg ballDY						; Change direction
		rcall MakeLedRed

SaveBallPos:
		mov oldBallX, ballX
		mov oldBallY, ballY

		rjmp MainLoop					; Loop forever

.include "ISD16_ATmega103.asm"			; ISD

.exit
