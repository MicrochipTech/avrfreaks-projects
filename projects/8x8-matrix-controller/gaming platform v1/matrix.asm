; *********************************************************************************************************************************
; programming	: pvar a.k.a. sp1r@l ev0lut10n
; started		: 06 - 03 - 2010
; completed on	: 03 - 10 - 2010
;
; The firmware for my first gaming platform!
; *********************************************************************************************************************************

	

; *********************************************************************************************************************************
; * fundamental assembler directives
; *********************************************************************************************************************************

; constants -----------------------------------------------------------------------------------------------------------------------

.include "tn2313.inc"

.equ SRAM_pri_fb_hi	= $00		; first byte of primary frame buffer (first byte of SRAM)
.equ SRAM_pri_fb_lo	= $60		;

.equ SRAM_sec_fb_hi	= $00		; first byte of secondary frame buffer
.equ SRAM_sec_fb_lo	= $80		;

.equ SRAM_ter_fb_hi	= $00		; first byte of tertiary frame buffer
.equ SRAM_ter_fb_lo	= $A0		;

.equ refresh_speed = 200		; determines matrix scan rate (lower value -> lower scan rate)
								; a matrix scan will commence on timer counter overflow
								; 180 -> scan rate = 83.3Hz
								; 200 -> scan rate = 105.9Hz
								; 220 -> scan rate = 143.1Hz
								; 232 -> scan rate = 183.4Hz
								; 235 -> scan rate = 197.3Hz
								; 239 -> scan rate = 219.5Hz

.equ btn_spd = 24				; determines button responsiveness (lower values => more responsive buttons)
								; compared with unlock_cnt (-> increased after each matrix scan)
								; 143 results in delay of approximately 1sec (scan rate = ~ 143Hz)

; variables -----------------------------------------------------------------------------------------------------------------------

.equ draw_spd = 0x14			; determines frame drawing rate (lower value -> higher fps)
								; compared with draw_cnt (-> increased after each matrix scan)
								; frame drawing rate = draw_cnt / draw_spd
								; 143 results in on frame per second
								; (address of GPIOR1 register)

.equ btn_flags  = 0x13			; bit 7: reserved for future use :-)
								; bit 6: reserved for future use :-)
								; bit 5: set if buttons are locked
								; bit 4: set if buttons are pressed
								; bits 3-0: indicate pressed buttons
								; (address of GPIOR0 register)

.equ mode  = 0x15				; mode of operation
								; 000 xxxxx: wait for selection
								;	>	000 xxxx0 : fade in keypad indicators
								;	>	000 xxxx1 : fade out keypad indicators
								; 001 xxxxx: alien morphing 
								;	>	001 xx### : morph aliens
								; 010 xxxxx: game of life
								;	>	010 xx000 : create starting population
								;	>	010 xx001 : proceed to next generation
								;	>	010 xx011 : next generation completed, waiting morphing
								; 011 xxxxx: need for speed
								;	>	011 0xxxx : create starting map
								;	>	011 1xxxx : game started
								;	>	011 11xx0 : game lost (fade vehicle out)
								;	>	011 11xx1 : game lost (fade vehicle in)
								; 100 xxxxx: cylon fader
								;	>	100 xxxx0 : initialize vibrator
								;	>	100 xx001 : let it roll to the right
								;	>	100 xx011 : let it roll to the left
								;	>	100 ###x1 : wait for a while...
								; (address of GPIOR2 register)

.def sq_tmp = r8
.def sq_alive = r9
.def sq_f_row = r10				; data byte4 - used to read from and write to buffers
.def sq_f_col = r11				; data byte4 - used to read from and write to buffers
.def sq_l_row = r12				; data byte4 - used to read from and write to buffers
.def sq_l_col = r13				; data byte4 - used to read from and write to buffers

.def seed_lo = r14				; seed value for random number generation
.def seed_hi = r15				; seed value for random number generation

.def temp = r16					; data register for loading abritary values

.def pa_data = r17				; data to be output on porta
.def pb_data = r18				; data to be output on portb
.def pd_data = r19				; data to be output on portd

.def data1 = r20				; data byte1 - used to read from and write to buffers
.def data2 = r21				; data byte2 - used to read from and write to buffers
.def data3 = r22				; data byte3 - used to read from and write to buffers
.def data4 = r23				; data byte4 - used to read from and write to buffers

.def draw_cnt = r24				; scan counter until new frame is to be presented
.def unlock_cnt = r25			; scan counter until buttons are unlocked


; *********************************************************************************************************************************
; * code segment initialization
; *********************************************************************************************************************************

.cseg
.org 0

	rjmp mcu_init				; Power-on, Brown-out and Watchdog Reset
	reti						; INT0 External Interrupt Request 0
	reti						; INT1 External Interrupt Request 1
	reti						; TIMER1 CAPT Timer/Counter1 Capture Event
	reti						; TIMER1 COMPA Timer/Counter1 Compare Match A
	reti						; TIMER1 OVF Timer/Counter1 Overflow
	rjmp scan_frame				; TIMER0 OVF Timer/Counter0 Overflow
	reti						; USART0, RX USART0, Rx Complete
	reti						; USART0, UDRE USART0 Data Register Empty
	reti						; USART0, TX USART0, Tx Complete
	reti						; ANALOG COMP Analog Comparator
	reti						; PCINT Pin Change Interrupt
	reti						; TIMER1 COMPB Timer/Counter1 Compare Match B
	rjmp bam_step				; TIMER0 COMPA Timer/Counter0 Compare Match A
	rjmp mab_step				; TIMER0 COMPB Timer/Counter0 Compare Match B


; *********************************************************************************************************************************
; * microcontroller initialization
; *********************************************************************************************************************************

mcu_init:
	ser temp					;
	out DDRA, temp				;
	out DDRB, temp				; configure all pins on all ports as outputs
	out DDRD, temp				;

	ldi temp, $df				; adrress of last byte of SRAM
	out SPL, temp				; stack pointer initialization

	sbi ACSR, ACD				; turn off analog comparator
	cbi	ACSR, ACBG				; disconnect analog comparator from internal voltage reference

	in temp, MCUSR				;
	andi temp, 0b11110111		; clear WDRF in MCUSR
	out MCUSR, temp				;
	in temp, WDTCR				; write logical one to WDCE and WDE
	ori temp, 0b00011000		; keep old prescaler setting to prevent unintentional time-out
	out WDTCR, temp				;
	clr	temp					;
	out WDTCR, temp				; turn off watchdog timer

	out TCCR0A, temp			; disconnected from ports, normal mode operation
	out TCCR0B, temp			; no clock input -> timer stopped
	out TCNT0, temp				; reset timer

	out TCCR1A, temp			; disconnected from ports
	ldi temp, 0b00000010		; normal mode operation
	out TCCR1B, temp			; prescaller set to 1/8

	in temp, TIMSK				;
	andi temp, 0b11111000		; clear three lower bits
	ori temp,  0b00000010		; set bit 1
	out TIMSK, temp				; disable compare match A and compare match B interrupts, enable overflow interrupt

	in temp, MCUCR				;
	andi temp, 0b10001111		; clear bits 6 and 4
	ori temp,  0b00100000		; set bit 5
	out MCUCR, temp				; set sleep mode to idle and enable sleep instrucion

	sei							; global interrupt enable


; *********************************************************************************************************************************
; * program init / main program loop
; *********************************************************************************************************************************

	clr	temp					;
	out mode, temp				; initial program mode

	rcall clr_bf				; clear buffers (primary and secondary)

	ldi temp, refresh_speed		; set matrix scan rate
	out TCNT0, temp				; next matrix scan will comence on timer counter overflow
	ldi temp, 0b00000101 		; set bit 1 and 2
	out TCCR0B, temp			; start timer counter clock (prescaller 1/1024)

loop4ever:
	rcall render				; render image in secondary buffer

update_buffer:
	in temp, draw_spd			;
	cp draw_cnt, temp			; check if time to update primary buffer
	brlo not_yet				;

	rcall sec2pri				; update primary buffer with contents of secondary
	clr draw_cnt				; reset scan counter
	rjmp loop4ever				; jump to the beginning of main loop

not_yet:
	sbic btn_flags, 4			; check if any button was pressed
	rcall button_act			; if pressed (flag was set), take action
	sleep						; go to sleep (be green babe ;-)
	rjmp update_buffer			; repeat when awaken...


; *********************************************************************************************************************************
; * random number generator (Fibonacci LFSR)
; *********************************************************************************************************************************
; IN:
;   seed_hi:seed_lo : 16bit seed
; OUT:
;   data4:data3 : 16bit random number

randomizer:
	push temp

	mov data3, seed_lo			; get seed for random number generation
	mov data4, seed_hi			;

	mov temp, data4				; get high byte of seed

	andi temp, 0b10000000		; keep 16th bit
	lsr temp					; roll bit in 15th position
	lsr temp					; roll bit in 14th position
	eor temp, data4				;

	andi temp,0b00100000		; keep only the result bit (14th bit)
	lsr temp					; roll bit in 13th position
	eor temp, data4				;

	andi temp,0b00010000		; keep only the result bit (13th bit)
	lsr temp					; roll bit in 12th position
	lsr temp					; roll bit in 11th position
	eor temp, data4				;
	andi temp,0b00000100		; keep only the result bit (11th bit)
	lsr temp					; roll bit in 10th position
	lsr temp					; roll bit in 09th position
	lsr temp					; roll bit in carry

	rol data3					; shift carry bit (result of previous operations) in random number low byte
	rol data4					; shift all bits of random number to the left

	mov seed_lo, data3			; random number will be the new seed
	mov seed_hi, data4			;

	pop temp
	ret


; *********************************************************************************************************************************
; *********************************************************************************************************************************

.include "lm_scan.asm"			; matrix scanning subroutines

.include "btn_ctrl.asm"			; button control subroutines

.include "renderer.asm"			; frame rendering machine (according to mode of operation)
.include "buffers.asm"			; buffer manipulation subroutines
.include "pixels.asm"			; pixel manipulation subroutines

.include "figures.asm"			; table of characters and aliens (in flash memory)
.include "maptiles.asm"			; table of terrain tiles(in flash memory)
