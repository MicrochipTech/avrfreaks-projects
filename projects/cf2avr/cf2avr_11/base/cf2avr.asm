;
;	Interfacing Compact Flash card to AT90S2313
;
;	Michal Strug 2005
;
;	tested with 32MB card and 12MHz clock, rs232 connection at 57600
;
;	file version: 1.0
;


.include "2313def.inc" 

.equ	cf_A00 			= PB0
.equ	cf_A01 			= PB1
.equ	cf_A02 			= PB2

.equ	cf_WE			= PD6
.equ	cf_OE			= PD5
.equ	cf_CE1 			= PD4
.equ	cf_READY		= PD3

.equ	latch			= PD2			; =0 -> memorize, =1 -> off


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
	rjmp RecvByteIRQ				;rjmp UART_RXC ; UART RX Complete Handler
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
; delays
;---------------------------------
delay_read:
		ldi		R31, 0x06
	delay_read_loop:
		nop
		dec		R31
		tst		R31
		brne	delay_read_loop
	ret
;=================================


;---------------------------------
; Misc
;---------------------------------
read_error:				; reading error register	(first set: sbi portd, cf_CE1 )
		ldi	R19, 01
		rcall 	read_atr
		rcall	SendByte
	ret

wait_ready:				; wait for high ready signal
	   	sbis pind, CF_READY
		rjmp wait_ready
	ret

wait_rdy:				; wait for RDY nit set in status register	(first set: sbi portd, cf_CE1 )
		ldi	R19, 07
		rcall 	read_atr
		andi	R19, 0x040
		cpi	R19, 0x040
		brne	wait_rdy
	ret

wait_drq:				; wait for DRQ bit set in status register	(first set: sbi portd, cf_CE1 )
		ldi	R19, 07
		rcall 	read_atr
		andi	R19, 0x08
		cpi	R19, 0x08
		brne	wait_drq
	ret

;=================================

strobe_rd:					; used for reading buffer memory from cf card (readed value in R19)
		sbi	portd, cf_OE
		rcall	wait_ready
 		cbi	portd, cf_OE

		nop
		nop
		nop
		nop

		in	R19, pinb

		sbi	portd, cf_OE
		nop

		__SendByte R19
	ret

strobe_wr:					; used in writing buffer memory in cf card (value to write in R20)
		sbi	portd, cf_WE
		rcall	wait_ready

		out	portb, R20
 		cbi	portd, cf_WE
		
		nop
		nop
		nop
		nop

		sbi	portd, cf_WE
	ret

;=================================

;---------------------------------
; Write to register in cf card
;	R19 = adres of register (0 to 7)
;	R20 = value to write
;---------------------------------
write_atr:
		ldi	R29, 0xff
		out 	ddrb, R29				; portb out

		sbi	portd, cf_WE
	
		sbi	portd, latch
		out	portb, R19
		cbi	portd, latch

		rcall	wait_ready

		out	portb, R20
 		cbi	portd, cf_WE

		nop
		nop
		nop

 		sbi		portd, cf_WE
	ret
;=================================




;---------------------------------
; Read from register in cf card
;	R19 = adres of register (0 to 7)
;---------------------------------
read_atr:
		rcall	wait_ready

		ldi	R29, 0xff
		out 	ddrb, R29				; portb out

		sbi	portd, latch
		out	portb, R19
		cbi	portd, latch

		sbi	portd, cf_WE
		sbi	portd, cf_OE

		ldi	R29, 0xff
		out 	portb, R29
		ldi	R29, 0x00		; portb input
		out 	ddrb, R29

		nop
		cbi	portd, cf_OE

		rcall	delay_read
		in	R19, pinb

		sbi	portd, cf_OE
		nop
	ret

read_atr_send:
		rcall	read_atr
		__SendByte R19
	ret

;=================================

;---------------------------------
; Sector Read (LBA mode)
;	R21 = Sec. count
;	R22 = LBA 7-0
;	R23 = LBA 15-8
;	R24 = LBA 23-16
;	R25 = 1110 LBA 27-24	(def. 0x0e0)
;---------------------------------
read_sector:
		cbi		portd, cf_CE1

		rcall	wait_rdy

		ldi	R19, 02
		mov	R20, R21
		rcall 	write_atr

		ldi	R19, 03
		mov	R20, R22
		rcall 	write_atr

		ldi	R19, 04
		mov	R20, R23
		rcall 	write_atr

		ldi	R19, 05
		mov	R20, R24
		rcall 	write_atr

		ldi	R19, 06
		mov	R20, R25
		rcall 	write_atr

		ldi	R19, 07
		ldi	R20, 0x020
		rcall 	write_atr

		rcall	wait_drq

		ldi	R29, 0xff
		out 	ddrb, R29				; portb out

		sbi	portd, latch
		cbi	portb, CF_A00
		cbi	portb, CF_A01
		cbi	portb, CF_A02
		cbi	portd, latch			; register adres 0

		ldi	R29, 0xff
		out 	portb, R29
		ldi	R29, 0x00		; portb input
		out 	ddrb, R29

		sbi	portd, cf_OE

		ldi	R17, 0x02		; sector size = 512 bytes
	_read_sector_loop1:
		ldi R16, 0x00
	read_sector_loop1:
			rcall	strobe_rd
		dec		R16
		brne	read_sector_loop1
		dec		R17
		brne	read_sector_loop1

		sbi	portd, cf_CE1
	ret
;=================================


;---------------------------------
; Sector Write (whole sector filled the same value) (LBA mode)
;	R21 = Sec. count
;	R22 = LBA 7-0
;	R23 = LBA 15-8
;	R24 = LBA 23-16
;	R25 = 1110 LBA 27-24	(def. 0x0e0)
;	R26 = value to write fill sector
;---------------------------------
write_sector:
		cbi		portd, cf_CE1

		rcall	wait_rdy

		ldi	R19, 02
		mov	R20, R21
		rcall 	write_atr

		ldi	R19, 03
		mov	R20, R22
		rcall 	write_atr

		ldi	R19, 04
		mov	R20, R23
		rcall 	write_atr

		ldi	R19, 05
		mov	R20, R24
		rcall 	write_atr

		ldi	R19, 06
		mov	R20, R25
		rcall 	write_atr

		ldi	R19, 07
		ldi	R20, 0x030
		rcall 	write_atr

		rcall	wait_drq

		ldi	R29, 0xff
		out 	ddrb, R29				; portb out

		sbi	portd, latch
		cbi	portb, CF_A00
		cbi	portb, CF_A01
		cbi	portb, CF_A02
		cbi	portd, latch			; register adres 0

		sbi	portd, cf_WE

		ldi	R17, 0x02			; sector size = 512 bytes
	_write_sector_loop1:
		ldi 	R16, 0x00
	write_sector_loop1:
			mov	R20, R26
			rcall	strobe_wr
		dec		R16
		brne	write_sector_loop1
		dec		R17
		brne	write_sector_loop1

		sbi	portd, cf_CE1
	ret
;=================================



;---------------------------------
; read drvie info
;---------------------------------
read_info:
		cbi	portd, cf_CE1

		rcall	wait_rdy

		ldi	R19, 06
		ldi	R20, 0
		rcall 	write_atr

		ldi	R19, 07
		ldi	R20, 0x0ec
		rcall 	write_atr

		rcall	wait_drq

		ldi	R29, 0xff
		out 	ddrb, R29				; portb out

		sbi	portd, latch
		cbi	portb, CF_A00
		cbi	portb, CF_A01
		cbi	portb, CF_A02
		cbi	portd, latch

		ldi	R29, 0xff
		out	portb, R29
		ldi	R29, 0x00		; portb input
		out 	ddrb, R29

		ldi	R17, 0x02		; sector size = 512 bytes
	_read_info_loop1:
		ldi 	R16, 0x00
	read_info_loop1:
			rcall	strobe_rd
		dec		R16
		brne	read_info_loop1
		dec		R17
		brne	_read_info_loop1

		sbi		portd, cf_CE1
	ret
;=================================


;---------------------------------
; OnReset
;---------------------------------
RESET:

	InitStack
	InitSerial

	rjmp	main
;=================================



main:
	ldi R29, 0xff
	out ddrb, R29

	sbi ddrd, latch
	sbi ddrd, CF_CE1
	sbi ddrd, CF_OE
	sbi ddrd, CF_WE
	cbi ddrd, CF_READY
	cbi portd, CF_READY


	rcall	wait_ready

	clc
wait_c1:
	brcc	wait_c1		; wait until recive any byte form pc by rs232
	clc


main_loop:
	rcall	read_info	; send drive info to pc

	clc
wait_c3:
	brcc	wait_c3		; wait until recive any byte form pc by rs232
	clc

; reading whole sectors in card
	ldi	R26,00  	;		Sector number = 62720 (in my 32MB cf card) 
	adr_loop1:
	ldi	R27, 00	
	adr_loop2:
		ldi		R21, 01	
		mov		R22, R27
		mov		R23, R26
		ldi		R24, 0x00
		ldi		R25, 0x0e0

		rcall 	read_sector		; send sector to pc

		clc
wait_c:
		brcc	wait_c		; wait until recive any byte form pc by rs232
		clc

	inc		R27
	cpi		R27, 00
	brne	adr_loop2
	inc		R26
	cpi		R26, 0xF6
	brne	adr_loop1


; writing data to a sector:
;
;	ldi		R21, 01	
;	ldi		R22, 01
;	ldi		R23, 00
;	ldi		R24, 00
;	ldi		R25, 0x0e0
;	ldi		R26, 129
;	rcall 	write_sector		; fill sector 01 with 129


main_loop_end:
	rjmp	main_loop_end
;=================================

