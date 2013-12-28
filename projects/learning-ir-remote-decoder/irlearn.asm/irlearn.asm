; 12 MHz xtal

; This program provides standalone IR remote learning and then decoding functionality.
; It works with common TSOP4838 or any other receiver.
; Program uses 1/16384 s time grain, to make it 'compatible' with LIRC databases when need.
; Software not stuck on any proprietary IR encoding system and should work with any IR protocol.
; However you should select proper receiver (38 / 56 kHz ... etc). Or even both in parallel.
; Code optimized for power consumption (uses sleep when awaiting for IR / button command).
; Ebay's USBASP v2.0 "LC Tech / LCSOFT"  blue 44x19 mm board used as 'eval board' because
;  it is cheapest possible ready-to-use board (cheaper than any other complete AVR board),
;  and can be programmed using another same board. Note, for use as IR transmitter,
;  you need to disconnect (cut) trace between pins 13 & 32, it is under IC body.
; Hardware also includes pushbutton (in parallel to TSOP4838, wired "OR" to save a pin 
; and an interrupt). Connect it all to pin 32.
; Hardware also uses some loads like LED/relay(s). Connect load0 to pin 25,
;  it connected to JP3 at LCSOFT USBASP board.
; Short pins 2 & 3 (short pin 2 to gnd) to set on/off mode switching of load0 by two different remote commands.
; Unconnect pin 2 to set toggle mode switching using one remote command.
; IR Transmitting also possible, and pin and HW Timer reserved and tested, but not done yet.
; The quantity of remembered commands is depend on EEPROM size and two commands can be used in ATmega8
; (512 bytes of EEPROM).

; To program board, short (permamently) JP2, and cut wire #4 in 10-pin cable. 
;  Cutting wire not damages programming cable, it will work as before on other devices.
; Connect board to same but unmodified programmer board and use:
;  sudo avrdude -p m8 -c usbasp -U flash:w:irlearn.hex 

; disambiguition: not to confuse with OSCaR IR raw RX/TX interface. This projest is for standalone use.

; Author: jopka1@gmail.com, www.bdyssh.ru. License: GPL V2+

.INCLUDE "m8def.inc"

; INVERTED
.equ    led0port = portc
.equ    led0ddr = ddrc
.equ    led0bit = 0		; pin 23 (pc0) INVERTED
; INVERTED
.equ    led1port = portc
.equ    led1ddr = ddrc
.equ    led1bit = 1		; pin 24 (pc1) INVERTED

; not use pin 14 (pb2) it is connected to reset

;  pin 32 (pd2) it is connected to pb1 (pin 13)
; BUT need unconnect it!

.equ    usb0port = portb
.equ    usb0ddr = ddrb
.equ    usb0bit = 0		; pin 12 (pb0)

;.equ    usb1port = portb
;.equ    usb1ddr = ddrb
;.equ    usb1bit = 1		; pin 13 (pb1)
; OR
.equ    usb1port = portd
.equ    usb1ddr = ddrd
.equ    usb1in = pind
.equ    usb1bit = 2		; pin 32 (pd2 / INT0)

; UART / GPIO
.equ    uart0port = portd	; RX
.equ    uart0ddr = ddrd
.equ    uart0in = pind
.equ    uart0bit = 0		; pin 30 (pd0)
.equ    uart1port = portd	; TX
.equ    uart1ddr = ddrd
.equ    uart1in = pind
.equ    uart1bit = 1		; pin 31 (pd1)

; HW counter/PWM 
;  pin 32 (pd2) connected to pb1 (pin 13)
; BUT i unconnect it.
.equ    timer1port = portb
.equ    timer1ddr = ddrb
.equ    timer1bit = 1		; pin 13 (pb1)
.equ    timer1in = pinb	; input of own pin - for test of end of pulse.

; mode jumpers
;.equ	modesw0port = portc
;.equ	modesw0ddr = ddrc
;.equ	modesw0bit = 2		; pin 25 (pc2) connected to JP3 at LCSOFT USBASP board
;.equ	modesw0in = pinc
; -OR-
.equ	modesw0port = portd
.equ	modesw0ddr = ddrd
.equ	modesw0bit = 4		; pin 2 (pd4)
.equ	modesw0in = pind

; load on/off outputs
.equ	load0port = portc
.equ	load0ddr = ddrc
.equ	load0bit = 2		; pin 25 (pc2) connected to JP3 at LCSOFT USBASP board
.equ	load1port = portc
.equ	load1ddr = ddrc
.equ	load1bit = 3		; pin 26 (pc3) 

.equ	maxpktlen = 64		; multiplied by 4 bytes. So 64 uses 256 (max) bytes of eeprom. (and RAM, too.)

.equ	eeprom_size_bytes = EEPROMEND + 1
.equ	eeprom_size_packets = eeprom_size_bytes / maxpktlen / 4

.equ	cmd_qty = eeprom_size_packets	; 2 for atmega8, we hope.

; timeout - q'ty of 256 times of successive '1's treated as packet end. (minimum value = 1).
; big timeout eases debugging, but treat autorepeat command of some remotes (while button hold) as part of command.
; (use short remote button press to avoid).
; too short timeout will break packet before packet end on slow remotes (with low BPS).

;.equ	timeout = 16		; 64 = 1 s, 16 = 1/4 s.
.equ	timeout = 2		; to avoid 'repeat' commands concatenate.

.dseg
buf:	.byte	512
stack:	.byte	512

.cseg
	rjmp RESET ; Reset Handler
	rjmp EXT_INT0 ; IRQ0 Handler
	rjmp EXT_INT1 ; IRQ1 Handler
	rjmp TIM2_COMP ; Timer2 Compare Handler
	rjmp TIM2_OVF ; Timer2 Overflow Handler
	rjmp TIM1_CAPT ; Timer1 Capture Handler
	rjmp TIM1_COMPA ; Timer1 CompareA Handler
	rjmp TIM1_COMPB ; Timer1 CompareB Handler
	rjmp TIM1_OVF ; Timer1 Overflow Handler
	rjmp TIM0_OVF ; Timer0 Overflow Handler
	rjmp SPI_STC ; SPI Transfer Complete Handler
	rjmp USART_RXC ; USART RX Complete Handler
	rjmp USART_UDRE ; UDR Empty Handler
	rjmp USART_TXC ; USART TX Complete Handler
	rjmp ADC ; ADC Conversion Complete Handler
	rjmp EE_RDY ; EEPROM Ready Handler
	rjmp ANA_COMP ; Analog Comparator Handler
	rjmp TWSI ; Two-wire Serial Interface Handler
	rjmp SPM_RDY ; Store Program Memory Ready Handler

EXT_INT1:
TIM2_COMP:
TIM2_OVF:
TIM1_CAPT:
TIM1_COMPA:
TIM1_COMPB:
TIM1_OVF:
TIM0_OVF:
SPI_STC:
USART_RXC:
USART_UDRE:
USART_TXC:
ADC:
EE_RDY:
ANA_COMP:
TWSI:
SPM_RDY:
	rjmp	kernel_panic
	rjmp	kernel_panic
	rjmp	kernel_panic
;	reti

RESET:	ldi	r16, high(RAMEND); Main program start
	out	SPH, r16
	ldi	r16, low(RAMEND)
	out	SPL, r16

	ldi	r16, 0		; pullup
	out	ddrb, r16
	ldi	r16, 255
	out	portb, r16

	ldi	r16, 0		; pullup
	out	ddrc, r16
	ldi	r16, 255
	out	portc, r16

	ldi	r16, 0		; pullup
	out	ddrd, r16
	ldi	r16, 255
	out	portd, r16

; mode pin(s) init
	cbi	modesw0ddr, modesw0bit	; in
	sbi	modesw0port, modesw0bit	; pullup

; timer1 init
; dont forget, timer stops when sleep!
	sbi	timer1ddr, timer1bit
; r17=10, r16=0 - 2325 Hz (measured) : 12000000 / 2 / 2560 = 2343 Hz (theory)
; so 38 kHz is r17=0, r16=158
	ldi	r16, 158                ; (lo x)
	ldi	r17, 0                ; (hi x) 		; try to get 38 khz
	out	ocr1ah, r17
	out	ocr1al, r16

	rcall	timer1_off

; the difference of full power down and standby is only 0.1 mA.
; it is very small compared to TSOP4838 current (3.3 mA). 
; And after full power-down, decodind works bad for low (3,3V) power.
; Current draw in standby mode only 3,4 mA in total, for 3,3-3,6 V supply.
	ldi	r16, 0b11100000		; sleep enable, standby (xtal running)
;	ldi	r16, 0b10100000		; sleep enable, full power down
	out	MCUCR, r16

	ldi	r16, 0b01000000		; INT0 low level 
	out	GICR, r16

	rjmp 	cont

; test part of code for planned ir transmitter. do not use.

; test button
	rcall	led2on
btn:
	sbis	uart1in, uart1bit
	rjmp	pressed
unpressed:
	rcall	led2off
rcall timer1_off

	rjmp	btn_done
pressed:
	rcall	led2on

rcall timer1_on

btn_done:
	nop
	rcall	bounce_delay
	rjmp	btn

	rjmp	kernel_panic

timer1_on:
	ldi	r16, 0b01000000	
	out	TCCR1A, r16
	ldi	r16, 0b00001001	; div by 1
	out	TCCR1B, r16
	ret

timer1_off:
; wait for end of '1' pulse
;wt:
;	sbic	timer1pin, timer1bit	; sometimes hang...
;	rjmp	wt
;timer1_off_2:
;	ldi	r16, 0b00001000
;	out	TCCR1B, r16
	ldi	r16, 0b00000000	
	out	TCCR1A, r16
	ldi	r16, 0b00000000
	out	TCCR1B, r16
	cbi	timer1port, timer1bit	; not work. - work now.
	ret

cont:		
; try to share learning button pin and tsop4838 pin,
; due to it use interrupt and so IC can be sleep and don't poll button.
; r23 - mode. 
; r23=0 - sleep/idle - not used?
; r23=1 - normal packet receive process, do nothing, wait for end
; r23=2 - end of pkt receive, must process received data.
; r23=3 - short 0.5...5 s button press
; r23=4 - long >5s press

	ldi	r16, 0
	mov	r15, r16	; mode (receive / learn)

	rcall	led0off
	rcall	led1off

	ldi	r24, 0 ; cmd number for next remember/learn
main:
	ldi	r28, low(buf)	; RAM
	ldi	r29, high(buf)
	ldi	r22, 0	; pkt len.

rx_cont_pkt:
	ldi	r23, 1
	sei
	sleep	; waiting for interrupts...
	cli
	cli

	cpi	r23, 1
	breq	rx_cont_pkt
	cpi	r23, 2
	breq	rx_pkt_done
	cpi	r23, 3
	breq	btn_shortpress
	cpi	r23, 4
	breq	btn_longpress

	rjmp	main

btn_shortpress:
; short press acts as spare (emergency) control when remote is lost or not work.
; so put here action(s) same as IR decoded command reaction.
	rcall	load0_toggle
;	...
	rjmp	main

btn_longpress:
; long button press (hold down) is enter learning mode.
; Each new command starts at N*4*maxpktlen offset in EEPROM, where N - command number.

	ldi	r16, 1	; mode = learn - just write next received pkt to eeprom
	mov	r15, r16
	rjmp	main

; here is (we hope) packet received in RAM and what to do with it depends on current mode.
rx_pkt_done:
	rcall	led0off
	rcall	led1off

	mov	r16, r15
	cpi	r16, 0
	breq	cmd_search_2
	rjmp	pkt_write_eep

cmd_search_2:
	ldi	r21, 0	; number of decoded command, valid only if decoded correctly.
cmd_search:
	rcall	pkt_compare
	cpi	r16, 1	; match
	breq	cmd_found
	inc	r21
	cpi	r21, cmd_qty
	brne	cmd_search
	rjmp	cmd_search_done

cmd_found:
	cpi	r21, 0	; cmd number
	breq	cmd0
	cpi	r21, 1	; cmd number
	breq	cmd1
	rjmp	cmd_search_done

cmd0:
	sbic	modesw0in, modesw0bit
	rjmp	cmd_toggle
	rcall	load0_on
	rjmp	cmd_search_done

cmd_toggle:
	rcall	load0_toggle
	rjmp	cmd_search_done

cmd1:
	rcall	load0_off
	rjmp	cmd_search_done

cmd_search_done:
	rjmp	main

load0_toggle:
	mov	r16, r00
	cpi	r16, 1		; if it was 'on' then turn it off
	breq	load0_off
load0_on:
	ldi	r16, 1
	mov	r00, r16
	sbi	load0ddr, load0bit
	sbi	load0port, load0bit
; add your own functionality here:
 rcall	led2on
	ret

load0_off:
	ldi	r16, 0
	mov	r00, r16
	sbi	load0ddr, load0bit
	cbi	load0port, load0bit
; add your own functionality here:
 rcall	led2off
	ret

load1_toggle:
	mov	r16, r01
	cpi	r16, 1		; if it was 'on' then turn it off
	breq	load1_off
load1_on:
	ldi	r16, 1
	mov	r01, r16
	sbi	load1ddr, load1bit
	sbi	load1port, load1bit
; add your own functionality here:
	ret

load1_off:
	ldi	r16, 0
	mov	r01, r16
	sbi	load1ddr, load1bit
	cbi	load1port, load1bit
; add your own functionality here:
	ret

pkt_compare:
	ldi	r28, low(buf)	; RAM
	ldi	r29, high(buf)
	ldi	r30, 0	; eeprom pkt start address
	ldi	r31, 0
.if maxpktlen=64
	mov	r31, r21	; read offset in eeprom for command
.else
	raise_compiler_error_should_be_modify_here_to_correct_calculate_shift_for_r31.
.endif
	ldi	r22, 0 
eep_compare:
; '0' (active) pulse length check
; - and -
; '1' (inactive) pulse length check (now combined because almost same procedure)
	rcall	eep_read
	ld	r17, y+
	sub	r16, r17
	mov	r18, r16

	rcall	eep_read
	cpi	r16, 255	; eeprom probably empty - FF'ed
;	breq kernel_panic
	breq	not_like

; this should normally not occurs at '0' (active) pulse length check,
; maybe it should be added later to skip it at '0' check... *TODO*
	cpi	r16, timeout	; end of stored packet reached - so it is match
	breq	match

	ld	r17, y+
;	sbc	r16, r17	; should check both bytes! but now only one check. TODO!
; ...
	cpi	r18, 252
	breq	like
	cpi	r18, 253
	breq	like
	cpi	r18, 254
	breq	like
	cpi	r18, 255
	breq	like
	cpi	r18, 0
	breq	like
	cpi	r18, 1
	breq	like
	cpi	r18, 2
	breq	like
	cpi	r18, 3
	breq	like
	cpi	r18, 4
	breq	like
	rjmp	not_like

like:
	inc	r22	; failsafe behaviour
	cpi	r22, maxpktlen*2	; here '1's and '0's both counted in r22 so double size.
	brne	eep_compare

not_like:
	ldi	r16, 0
	ret

match:
	ldi	r16, 1
	ret

kernel_panic:
	rcall	led0off
	rcall	led1on
	rcall	led2off
	rcall	delay_025s
	rcall	delay_025s
	rcall	led0on
	rcall	led1off
	rcall	led2on
	rcall	delay_025s
	rcall	delay_025s
	nop
	rjmp	kernel_panic
	rjmp	kernel_panic

pkt_write_eep:
	ldi	r16, 0
	mov	r15, r16	; reset current mode

	cpi	r22, 0	; length
;	breq	main
	brne	eepw
	rjmp	main

eepw:

;	cli
; write received pkt from ram to eeprom.
	ldi	r28, low(buf)	; RAM
	ldi	r29, high(buf)
	ldi	r30, 0	; eeprom pkt start address
.if maxpktlen=64
	mov	r31, r24	; write offset in eeprom for command
.else
	raise_compiler_error_should_be_modify_here_to_correct_calculate_shift_for_r31.
.endif
eepwr:
	rcall	led0off
	rcall	led1on
	ld	r16, y+
	rcall	eep_write
	ld	r16, y+
	rcall	eep_write
	rcall	led0on
	rcall	led1off
	ld	r16, y+
	rcall	eep_write
	ld	r16, y+
	rcall	eep_write

	dec	r22
	brne	eepwr

;	ldi	r22, 0	; index

;	ldi	r28, low(buf)	; RAM
;	ldi	r29, high(buf)

; roll place at eeprom where to write next learned command (if any)
	inc	r24
	cpi	r24, cmd_qty
	brne	n2
	ldi	r24, 0

n2:

;rx_cont_pkt:
	rcall	led0off
	rcall	led1off

	rjmp	main
	rjmp	main

eep_read:
	sbic	EECR, EEWE
	rjmp	eep_read
	out	EEARH, r31
	out	EEARL, r30
	sbi	EECR, EERE
	in	r16, EEDR
	adiw	r30, 1
	ret

eep_write:
	sbic	EECR, EEWE
	rjmp	eep_write
	out	EEARH, r31
	out	EEARL, r30
	out	EEDR, r16
	sbi	EECR, EEMWE
	sbi	EECR, EEWE
	adiw	r30, 1
	ret

EXT_INT0:
	rcall	led0on
	rcall	led1off
	ldi	r19, 0
	ldi	r18, 0
meas_zero:
	rcall	ir_delay	; 1 / 16384 s. standard LIRC grain
	sbic	usb1in, usb1bit
	rjmp	meas_zero_done
	inc	r18
	cpi	r18, 0
	brne	meas_zero
	inc	r19
	cpi	r19, timeout		; max. one second (63).
	brne	meas_zero

; if active (low) for more than timeout, it is button press which
; shared (wired-OR) with TSOP4838.
	ldi	r23, 3 ; at least, short button press detected.
	rcall	led0on
	rcall	led1on

	ldi	r18, 10	; 10 x 0,25 = 2,5 s long press
meas_zero_2:
	rcall	delay_025s
	sbic	usb1in, usb1bit
;	rjmp	meas_zero_2_done
	rjmp	short_press
	dec	r18
	brne	meas_zero_2

	ldi	r23, 4 ; long button press detected.
	rcall	led0off
	rcall	led1on
;	rjmp	int0_done
	rjmp	meas_wait_for_zero_gone

short_press:
	rcall	led0off
	rcall	led1off
	rjmp	int0_done

; here need still to wait for low level gone. Else next time INT0 false fires.
; So, we never return if continuous low level stuck.
meas_wait_for_zero_gone:
;	rcall	ir_delay	; this not need here really
	rcall	bounce_delay	; and this really need.
	nop
	sbis	usb1in, usb1bit
	rjmp	meas_wait_for_zero_gone

	rjmp	int0_done

meas_zero_done:
	st	y+, r18		; store active (0) length to ram
	st	y+, r19

;	rcall	led0off
;	rcall	led1on

	ldi	r19, 0
	ldi	r18, 0
meas_one:
	rcall	ir_delay	; 1 / 16384 s. standard LIRC grain
	sbis	usb1in, usb1bit
	rjmp	meas_one_done
	inc	r18
	cpi	r18, 0
	brne	meas_one
	inc	r19
	cpi	r19, timeout		; max. one second (63).
	brne	meas_one

; here is timeout of high level, it is normal at end of packet.
	ldi	r23, 2	; end of packet due to timeout

meas_one_done:
;	cpi	r23, 1
;	brne	int0_done
	st	y+, r18		; store inactive (1) length to ram
	st	y+, r19
	inc	r22
	cpi	r22, maxpktlen
	brne	int0_done
	ldi	r23, 2	; end of packet due to length

int0_done:
	reti

; need to 12M / 16384 = 732 tact delay (61 * 12).
; but real delay (time grain for digitize packet) is not measured in hardware so may be not
; accurate. It is not any problem with self (standalone) learning and then detecting,
; but may have not ideal match to LIRC database (if used in any way).
ir_delay:
	ldi	r16, 61
ir_d1:
	nop	; 1 tact  x  9
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	dec	r16	; 1 tact
	brne	ir_d1	; 2 tact when loop
	ret

bounce_delay:
	push	r17
	push	r16
	ldi	r17,0
bounce_d1:
	rcall	ir_delay
	dec	r17
	brne	bounce_d1
	pop	r16
	pop	r17
	ret

delay_025s:
	push	r18
	ldi	r18, 16
dly02:
	rcall	bounce_delay
	dec	r18
	brne	dly02
	pop	r18
	ret

led0on:
	sbi	led0ddr, led0bit
	cbi	led0port, led0bit
	ret
led1on:
	sbi	led1ddr, led1bit
	cbi	led1port, led1bit
	ret
led0off:
	sbi	led0ddr, led0bit
	sbi	led0port, led0bit
	ret
led1off:
	sbi	led1ddr, led1bit
	sbi	led1port, led1bit
	ret

led2off:
	sbi	usb0ddr, usb0bit
	cbi	usb0port, usb0bit
	ret
led2on:
	sbi	usb0ddr, usb0bit
	sbi	usb0port, usb0bit
	ret

; sudo avrdude -p m8 -c usbasp -U eeprom:r:eep.hex:i
; sudo avrdude -p m8 -c usbasp -U eeprom:r:eee.hex:r

;.stop
; .end
