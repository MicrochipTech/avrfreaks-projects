;
;	Interfacing Compact Flash card to AT90S2313 and playing wave file from it
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

.equ	latch			= PD2			; =0 -> memorize, =1 -> off
.equ	latch_DAC		= PD3			; =0 -> memorize, =1 -> transparent


.dseg
	opoznienie:	.BYTE 1

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
.include "..\main.asm"
.include "..\serial.asm"



;---------------------------------
; delays
;---------------------------------
delay_read:
		ldi		R31, 0x06
	delay_read_loop:
		nop
		dec		R31
		brne	delay_read_loop
	ret
delay_ff:
		ldi		R31, 0xff
	delay_ff_loop:
		nop
		dec		R31
		brne	delay_ff_loop
	ret

delay_play:
;		ldi		R31, 0x77
		lds		R31, opoznienie
	delay_play_loop:
		nop
		dec		R31
		brne	delay_play_loop
	ret

wait_ready:				; insted of using READY pin form cf
		nop				; experimentaly chosed
;		nop
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

wait_bsy:				; wait for DRQ bit set in status register	(first set: sbi portd, cf_CE1 )
		ldi	R19, 07
		rcall 	read_atr
		andi	R19, 0x080
		cpi	R19, 0x080
		breq	wait_drq
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

;		in	R19, pinb

;		sbi	portd, cf_OE
;		nop

;		__SendByte R19
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

		sbi	portd, cf_OE
	sbi	portd, latch_DAC
			ldi	R29, 0xff
			out 	portb, R29
			ldi	R29, 0x00		; portb input
			out 	ddrb, R29

		ldi	R17, 0x02		; sector size = 512 bytes
	_read_sector_loop1:
		ldi R16, 0x00
	read_sector_loop1:
			ldi	R29, 0xff
			out 	portb, R29
			ldi	R29, 0x00		; portb input
			out 	ddrb, R29
			rcall	strobe_rd

;sbi	portd, cf_CE1
;sbi	portd, latch
;ldi	R29, 0xff
;out 	ddrb, R29				; portb out
;out	portb, R19

sbi	portd, latch_DAC
nop
nop
cbi	portd, latch_DAC
rcall	delay_play
sbi	portd, cf_OE

;cbi	portb, CF_A00
;cbi	portb, CF_A01
;cbi	portb, CF_A02
;cbi	portd, latch			; register adres 0
;cbi	portd, cf_CE1

		dec		R16
		brne	read_sector_loop1
		dec		R17
		brne	read_sector_loop1

		sbi	portd, cf_CE1

	cbi	portd, latch_DAC
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
clc
wait_wr:
	brcc wait_wr
clc
			mov	R20, R19
;			ldi	R20, 0
;rcall wait_bsy
			rcall	strobe_wr

;__SendByte R20
		dec		R16
		brne	write_sector_loop1
		dec		R17
		brne	write_sector_loop1

		sbi	portd, cf_CE1
	ret
;=================================



;---------------------------------
; Receiving file
;---------------------------------
rcv_file:
	ldi	R26,0x05f  	;		starting sector, set this values manually !!!
adr_loop1:
	ldi	R27, 00
adr_loop2:

		ldi	R21, 01	
		mov	R22, R27
		mov	R23, R26
		ldi	R24, 0x00
		ldi	R25, 0x0e0
		rcall 	write_sector		; fill sector 01 with 129

		clc
wait_c:
		brcc	wait_c		; wait until recive any byte form pc by rs232
		clc

_SendByte 'a'

		clc
wait_c2:
		brcc	wait_c2		; wait until recive any byte form pc by rs232
		clc

	inc	R27

adr_loop3:
	cpi	R27, 0x00	;		ending, set this values manually !!!
	brne	adr_loop2
	inc	R26
	cpi	R26, 0xc0	;		ending, set this values manually !!!
	brne	adr_loop1
adr_loop4:

	ret
;=================================

;---------------------------------
; play1
; 0x00c3 ~ 0x28c6
;---------------------------------
play1:
	ldi	R26, 0x77
	sts	opoznienie, R26
	ldi	R26,00
play_adr_loop1:
	cpi	R26, 0x00
	brne	play_adr_loop01
	ldi	R27, 0xC3
	rjmp	play_adr_loop2
play_adr_loop01:
	ldi	R27, 00
play_adr_loop2:
		ldi	R21, 01	
		mov	R22, R27
		mov	R23, R26
		ldi	R24, 0x00
		ldi	R25, 0x0e0
		rcall 	read_sector		; send sector to pc

	inc	R27
	cpi	R27, 0xC7
	brne	play_adr_loop3
	cpi	R26, 0x28
	breq	play_adr_loop4

play_adr_loop3:
	cpi	R27, 0x00
	brne	play_adr_loop2
	inc	R26
	cpi	R26, 0x29
	brne	play_adr_loop2
play_adr_loop4:
	ret
;=================================


;---------------------------------
; play2
; 0x2b00 ~ 0x5e8a
;---------------------------------
play2:
	ldi	R26, 0x77
	sts	opoznienie, R26
	ldi	R26, 0x2b  	;		Sector number = 62720 (in my 32MB cf card) 
play2_adr_loop1:
	ldi	R27, 00
play2_adr_loop2:
	ldi	R21, 01	
	mov	R22, R27
	mov	R23, R26
	ldi	R24, 0x00
	ldi	R25, 0x0e0
	rcall 	read_sector		; send sector to pc

	inc	R27
	cpi	R27, 0x8B
	brne	play2_adr_loop3
	cpi	R26, 0x5E
	breq	play2_adr_loop4

play2_adr_loop3:
	cpi	R27, 0x00
	brne	play2_adr_loop2
	inc	R26
	cpi	R26, 0x5E
	brne	play2_adr_loop2
play2_adr_loop4:
	ret
;=================================



;---------------------------------
; play4
; 0x5f00 ~ 0x0b092	(0x5192)
;---------------------------------
play4:
	ldi	R26, 0x38
	sts	opoznienie, R26
	ldi	R26, 0x5F
play4_adr_loop1:
	ldi	R27, 00
play4_adr_loop2:
	ldi	R21, 01	
	mov	R22, R27
	mov	R23, R26
	ldi	R24, 0x00
	ldi	R25, 0x0e0
	rcall 	read_sector		; send sector to pc

	inc	R27
	cpi	R27, 0x92
	brne	play4_adr_loop3
	cpi	R26, 0x0b0
	breq	play4_adr_loop4

play4_adr_loop3:
	cpi	R27, 0x00
	brne	play4_adr_loop2
	inc	R26
	cpi	R26, 0xb1
	brne	play4_adr_loop2
play4_adr_loop4:
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

;---------------------------------
; MAIN
;---------------------------------
main:
	ldi R29, 0xff
	out ddrb, R29

	sbi ddrd, latch
	sbi ddrd, latch_DAC
	sbi ddrd, CF_CE1
	sbi ddrd, CF_OE
	sbi ddrd, CF_WE

	sbi	portd, latch
	sbi	portd, latch_DAC
	sbi	portd, CF_CE1
	sbi 	portd, CF_OE
	sbi 	portd, CF_WE

	sbi	portd, latch_DAC
	ldi	R29, 0x81
	out	portb, R29
	cbi	portd, latch_DAC

	rcall	wait_ready
;	breq	play

	clc
wait_c1:
	brcc	wait_c1		; wait until recive any byte form pc by rs232
	clc

main_loop0:
	_SendByte '?'
	clc
main_loop:
	brcc	main_loop_end
	clc
if1:	
	ldi		R16, '1'
	cp		uart_var, R16
	brne	if2
	rcall	rcv_file
	rjmp	main_loop0

if2:	
	ldi		R16, '2'
	cp		uart_var, R16
	brne	if3
	rcall	play1
	rjmp	main_loop0

if3:	
	ldi		R16, '3'
	cp		uart_var, R16
	brne	if4
	rcall	play2
	rjmp	main_loop0

if4:
	ldi		R16, '4'
	cp		uart_var, R16
	brne	main_loop0
	rcall	play4
	rjmp	main_loop0

main_loop_end:
	rjmp	main_loop
;=================================

