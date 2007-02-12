; Project Notes
; Using interrupts could screw up the time sensitivity of the DMX side of things, so
; we'll use hardware handshaking (possibly) and polling for the incoming data side.
; Load 0x00 to 108 RAM locations (we need to leave some space for the stack - 20 bytes will be plenty)
; (Look into adding more memory using binary counters)
; I/O pins:
; PB0 - DMX Out
; PD0 - Serial RXD
; PD1 - Serial TXD
; 
.include	"2313def.inc"
.DEVICE		AT90S2313

.def	Temp		= r16
.def	Count		= r17
.def	FrameCount	= r18
.def	DMXByte		= r19
.def	BitCount	= r20
.def	RXBufferA	= r21
.def	RXBufferB	= r22
.def	RXBufferC	= r23

.org	0x0000
	rjmp	RESET

RESET:
	ldi		Temp, 0xFF
	out		DDRB, Temp ; Port B all outputs
	sbi		PORTB, 0
	 
	ldi		Temp, RAMEND
	out		SPL, Temp ; Init stack pointer

	sbi		UCR, RXEN ; Enable UART receiver
	sbi		UCR, TXEN ; and Transmitter
	ldi		Temp, 4000000/(19200*16)-1 
	out		UBRR, Temp ; Set baud rate (Using 4MHz Xtal)
; --- Clear 108 bytes in SRAM ---
	ldi		XH, 0x00
	ldi		XL, 0x61 ; Init RAM Pointer
	ldi		Temp, 0x00
	ldi		Count, 0x6C ; For 108 SRAM locations...
ZEROMEM:
	st		X+, Temp
	dec		Count
	brne	ZEROMEM ; ...Clear each one
	ldi		RXBufferA, 0x00
	ldi		RXBufferB, 0x00
	ldi		RXBufferC, 0x00 ; Also clear the receive buffers
MAINLOOP:
	rcall	SENDPACKETSTART
	ldi		DMXByte, 0x00
	rcall	SENDFRAME
	ldi		XH, 0x00
	ldi		XL, 0x61 ; Reset RAM pointer
	ldi		FrameCount, 0x6C
POLLNEXTFRAME:
	sbis	USR, RXC ; Do we have anything in the RX buffer?
	rjmp	LOADBYTE ; No; Load the next byte from SRAM
	rjmp	STOREBYTE ; Yes; Synchronise and store byte, we'll output the byte next time round...
SENDNEXTFRAME:
	rcall	SENDFRAME
	dec		FrameCount
	brne	POLLNEXTFRAME
	rjmp	MAINLOOP

; --- Load the next byte from SRAM into DMXByte ---
LOADBYTE:
	ld		DMXByte, X+
	rjmp	SENDNEXTFRAME ; Skip over STOREBYTE

; --- Sync, Interpret and store bytes from RX buffer ---
STOREBYTE:
	; Status codes to PC for each 3 bytes we receive:
	;	00 = Command OK (Data in sync and made sense)
	;	FF = Re-transmit command (data was out of sync, or didn't make sense)
	; Data From PC:
	; Byte 1 = FF (Start of command header)
	; Byte 2 = 01 - 6C = Channel Number (Channel Number of 0x00 is ignored, as it is interpreted as an empty
	; 		                     buffer)
	; Byte 3 = 00 - FF = Channel Value
	in		Temp, UDR
	cpi		RXBufferA, 0x00
	brne	CHECKBUFFERB ; Buffer A was full so check Buffer B
	mov		RXBufferA, Temp ; else Buffer A was empty, so Load Buffer A...
	ld		DMXByte, X+
	rjmp	SENDNEXTFRAME ; ...and send DMX and await the next byte
CHECKBUFFERB:
	cpi		RXBufferB, 0x00
	brne	SYNC ; Buffer B was full so this must be the last byte
	mov		RXBufferB, Temp ; else Buffer B was empty, so Load Buffer B...
	ld		DMXByte, X+
	rjmp	SENDNEXTFRAME ; ...and send DMX and await the next byte
SYNC:
; --- Check we're in Sync with the PC ---
	mov		RXBufferC, Temp ; Last byte, so load buffer C
	cpi		RXBufferA, 0xFF
	brne	RETRANSMIT ; First byte wasn't a command header or 100% channel value, so start again.
	cpi		RXBufferB, 0xFF
	breq	RETRANSMIT ; First byte was actually 100% channel value, second byte was command header, start again.
	cpi		RXBufferB, 0x6E
	brge	RETRANSMIT ; Channel number is greater than 108.
; --- We're in Sync after this point, so now we store data in SRAM ---
	push	XL ; Save memory pointer
	ldi		XL, 0x60 ; Zero pointer in SRAM
	add		XL, RXBufferB ; Add offset
	st		X, RXBufferC
	pop		XL ; Restore memory pointer
; --- Clear receive buffers and send a command OK ---
	ldi		RXBufferA, 0x00
	ldi		RXBufferB, 0x00
	ldi		RXBufferC, 0x00
	ldi		Temp, 0x00
	out		UDR, Temp ; Send a Command OK
CMDOKWAIT:
	sbis	USR, UDRE
	rjmp	CMDOKWAIT
	ld		DMXByte, X+
	rjmp	SENDNEXTFRAME
; --- Clear receive buffers and send PC a retransmit request ---
RETRANSMIT:
	ldi		RXBufferA, 0x00
	ldi		RXBufferB, 0x00
	ldi		RXBufferC, 0x00	
	ldi		Temp, 0xFF
	out		UDR, Temp ; Sent a ReTransmit ReQuest
RTRQWAIT:
	sbis	USR, UDRE
	rjmp	RTRQWAIT
	ld		DMXByte, X+
	rjmp	SENDNEXTFRAME

; --- Send Start of DMX Packet (Break, MAB etc.) ---
SENDPACKETSTART:
	cbi		PORTB, 0
	ldi		Count, 0x90
BRKDELAY:
	dec		Count
	brne	BRKDELAY
	sbi		PORTB, 0
	ldi		Count, 0x0b
MABDELAY:
	dec		Count
	brne	MABDELAY
	ret

; --- Send a single DMX frame consisting of the byte in DMXByte register ---
SENDFRAME:
	cbi		PORTB, 0
	ldi		BitCount, 0x08
	nop
	nop
NEXTBIT:
	ror		DMXByte
	brcs	SENDONE
	nop
	nop
SENDZERO:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	cbi		PORTB, 0
	dec		BitCount
	brne	NEXTBIT
	rjmp	STOPBITS ; Jump over SENDONE
SENDONE:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	sbi		PORTB, 0
	dec		BitCount
	brne	NEXTBIT
	nop
	nop
STOPBITS:
	ror		DMXByte ; Put it back in case we need to resend the same byte.
	nop ; Padding for MSB
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	sbi		PORTB, 0
	ldi		Count, 0x0b ; Gives >8uS (Doesn't matter if we go over that)
STOPDELAY:
	dec		Count
	brne	STOPDELAY
	ret