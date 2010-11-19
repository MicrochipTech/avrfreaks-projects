
;.include	"m328pdef.inc"
.include	"m168def.inc"



.include	"macros.inc"
;==============================================================================
;
; ***************  Y-reg reserved for use in ext0 int routine******************
;
;==============================================================================

#define rxmode cbi portd,4	; for the RS485 TX/RX direction select
#define txmode sbi portd,4

#define SSpin  portb,2		; slave select pin for RFM12B

.equ	true		= 0xff
.equ	false		= 0

; output stream destinations
.equ	null_stream	= 0	; nowhere
.equ	serial_stream	= 1	; serial out
.equ	memory_stream	= 2	; RAM


.equ	preamble 	= 0xAA	; for the receiver to lock-up to
.equ	syncbyte	= 0x2D	; 1st sync-byte (fixed in receiver silicon)


.equ	ram = 1			; used for the ldX/ldY/ldZ macros
.equ	rom = 2


.equ	inbufflen	= 128	; for serial RX. MUST BE POWER OF 2 (ie 4/8/16/32...)
.equ	outbufflen	= 16	; serial TX. not used so far

.equ	newsec		= 1	; a bit-mask, not bit number
.equ	timeoutFlg	= 4	; bit mask

.equ	connect_timeout = 20	; for when connected to a remote node (in command mode)
 
; 'state' used in ext0_int for RX
.equ	state0		= 0	; idle
.equ	state1		= 1	; error condition. RF system set to idle
.equ	state2		= 2	; initiated RX. On 1st RX byte (after sync-bytes), timeout timer started
.equ	state3		= 3	; waiting for data count
.equ	state4		= 4	; receiving data
.equ	state5		= 5	; waiting CRC
.equ	state6		= 6	; message complete & CRC good! (received and local CRC's match)

; state for the TX functios
.equ	state10		= 10	; doing TX.
.equ	state11		= 11	; TX complete

; state for wakeup mode from the RFM12B
.equ	state20		= 20	; wake-up function from RFM12B


;==============================================================
;.def	 =		r0	; varied uses
;.def	 =		r1	;
.def	proc_count =	r2	; used in [proc.asm] for param counting & gen-purpose others

.def	tacks =		r3	;\
.def	ticks =		r4	; - used in T0 timebase
.def	tocks =		r5	;/
.def	smalltime =	r6	; user downcount (8-bit), 1ms steps
.def	bigtime =	r7	; user downcount (8-bit), 100ms steps
.def	seconds =	r8	; user downcount (8-bit), seconds steps
.def	crc_A	=	r9	; crc calc routines
.def	crc_B	=	r10	; crc calc routines
.def	crc_C	=	r11	; crc calc routines
.def	crc_D	=	r12	; crc calc routines
.def	crc_low =	r13	; crc result (LSB)
.def	crc_high =	r14	; crc result (MSB)
.def	timeout =	r15	; reception timeout-timer downcount

.def	temp1	=	r16	; temp register
.def	temp2	=	r17	; temp register
.def	loopcounter =	r18	; general use loopcounter
.def	state =		r19	; current 'state' for int routine (RF receive/transmit)
.def	datacount =	r20	; count of incoming/outgoing data
.def	headerbyte =	r21	; header info for data block, ie packet-type
.def	destaddr =	r22	; destination node addr for transmissions

.def	bitflags =	r23	; 
;	0	newsec
;	1	new tenth-of-a-sec
;	2	RX timeout
;	3	local/remote	; clr if local, set if remote
;	4	
;	5	
;	6	
;	7	sleep indicator, check when woken up to see if just exited sleep (power-down)

; r24 & 25 can be used for 16-bit operations
.def	accL =		r24	; 
.def	accH =		r25

;==============================================================================
;==============================================================================
.dseg                        ; Start data segment

inbuffer:		.byte	inbufflen	; rx buffer (128 bytes)
outbuffer:		.byte	outbufflen	; tx buffer, not currently in use

; serial RX buffer pointers/count
rxcount:		.byte	1	; rxcount for receive routine
rxtail:			.byte	1
rxhead:			.byte	1

; RF system uses to show what an error was (during RX)
errorcode:		.byte	1	; number representing an error type

stream_destination:	.byte	1	; where an output stream is sent
					; (null, serial, memory)
incoming_connect:	.byte	1	; coming from source node (THIS node is destination)
outgoing_connect:	.byte	1	; going to dest (THIS node is source)
;==============================================================================
;==============================================================================

.cseg
; ATmega328/168 vectors, 2 bytes per entry
	.org	0x0000
	jmp	RESET			; Reset Handler
	.org	0x0002
	jmp	EXT_INT0		; IRQ0 Handler
	.org	0x0004
;	jmp	EXT_INT1		; IRQ1 Handler
	.org	0x0006
;	jmp	PCINT0			; PCINT0 Handler
	.org	0x0008
;	jmp	PCINT1			; PCINT1 Handler
	.org	0x000A
;	jmp	PCINT2			; PCINT2 Handler
	.org	0x000C
;	jmp	WDT			; Watchdog Timer Handler
	.org	0x000E
;	jmp	TIM2_COMPA		; Timer2 Compare A Handler
	.org	0x0010
;	jmp	TIM2_COMPB		; Timer2 Compare B Handler
	.org	0x0012
;	jmp	TIM2_OVF		; Timer2 Overflow Handler
	.org	0x0014
;	jmp	TIM1_CAPT		; Timer1 Capture Handler
	.org	0x0016
;	jmp	TIM1_COMPA		; Timer1 Compare A Handler
	.org	0x0018
;	jmp	TIM1_COMPB		; Timer1 Compare B Handler
	.org	0x001A
;	jmp	TIM1_OVF		; Timer1 Overflow Handler
	.org	0x001C
;	jmp	TIM0_COMPA		; Timer0 Compare A Handler
	.org	0x001E
;	jmp	TIM0_COMPB		; Timer0 Compare B Handler
	.org	0x0020
	jmp	TIM0_OVF		; Timer0 Overflow Handler
	.org	0x0022
;	jmp	SPI_STC			; SPI Transfer Complete Handler
	.org	0x0024
	jmp	USART_RXC		; USART, RX Complete Handler
	.org	0x0026
;	jmp	USART_UDRE		; USART, UDR Empty Handler
	.org	0x0028
;	jmp	USART_TXC		; USART, TX Complete Handler
	.org	0x002A
;	jmp	ADC			; ADC Conversion Complete Handler
	.org	0x002C
;	jmp	EE_RDY			; EEPROM Ready Handler
	.org	0x002E
;	jmp	ANA_COMP		; Analog Comparator Handler
	.org	0x0030
;	jmp	TWI			; 2-wire Serial Interface Handler
	.org	0x0032
;	jmp	SPM_RDY			; Store Program Memory Ready Handler


	


;******************************************************************************
;			reset vector (code start)
;******************************************************************************
; 1st thing to do is clear ram to zero
RESET:
	ldZ	ram,SRAM_START
	ldi	loopcounter,0		; 256 bytes of ram
	ldi	temp2,4			; 8 pages of 256 bytes in a mega328

	clr	temp1			; =0
wploop:	st	z+,temp1		; write 0's to ram
	dec	loopcounter		; inner loop
	brne	wploop
	dec	temp2
	brne	wploop			; outer loop


; set up stack
	ldi	temp1,high(RAMEND)	; Init SP
	out	SPH,temp1
	ldi	temp1,low(RAMEND)
	out	SPL,temp1

	clr	bitflags		; start with flag bits all zero

; select output stream
	stream	serial_stream			; set o/p stream to serial

; set command-mode to no connections
	clr	temp1
	sts	incoming_connect,temp1		; start with no connect incoming
	sts	outgoing_connect,temp1		; start with no connect outgoing


;******************************************************************************

;I/O port pin assighnments

;******************************************************************************
; port-D
; bits are assigned as....
; 0-RXD		(in)  0		; RS485 receive
; 1-TXD		(out) 1		; RS485 transmit
; 2-INT0	(in)  0		; int from RFM12B
; 3-INT1	(out) 1		; not used
; 4-RS485 DIR	(out) 1		; RS485 low=RX high=TX
; 5-n/u		(out) 1		; test pin
; 6-n/u		(in)  0		; not used
; 7-n/u		(out) 1		; not used
	ldi	temp1,0b10111010
	out	DDRD,temp1
	ldi	temp1,0b11010111	; pullups and levels
	out	PORTD,temp1
;******************************************************************************
; port-C
; bits are assigned as....
; 0-n/u		(in)  0		; not used
; 1-n/u		(in)  0		; not used
; 2-n/u		(in)  0		; not used
; 3-n/u		(in)  0		; not used
; 4-n/u		(in)  0		; not connected
; 5-n/u		(in)  0		; not connected
; 6-RST pin	(in)  0		; reset
; 7-NO bit 7 implimented
	clr	temp1
	out	DDRC,temp1	; all inputs
	out	PORTC,temp1	; all pullups OFF
;******************************************************************************
; port-B
; bits are assigned as....
; 0-			(in)  0		; not used at this time
; 1-			(out) 1		; used for testing
; 2-SS			(out) 1		; SS for SPI port
; 3-MOSI		(out) 1		; MOSI
; 4-MISO		(in)  0		; MISO
; 5-SCK			(out) 1		; clock for SPI port
; 6-			(in)  0		; used for xtal
; 7-			(in)  0		; used for xtal

	ldi	temp1,0b00101110	;
	out	DDRB,temp1
	ser	temp1			; pullups all ON
	out	PORTB,temp1

;******************************************************************************
;	end of port setup
;******************************************************************************

; startup initialisation
	ldi	temp1,0b00000100	; set sleep mode to power-down
	out	smcr,temp1

	ldi	state,state0		; idle for RFM12B RX/TX condition

	rcall	init_spi		; speed is clk/4 (8/4=2Mhz)

; test if eeprom config, if not create one
	call	test_config		; test if an eeprom config-set available
	brcs	yes_config
	call	default_config		; create if no config set exists
yes_config:
	call	set_config		; use the config data

	rcall	init_t0			; start timebase
	rcall	init_ser0		; starts serial. Rate=57.6k
	rcall	init_ext0		; ext int0 for RX/TX RFM12B
	sei				; global ints on

; now do initial (power-on) config of the RFM12B. 
	rcall	init_RFM12B		; always start up RFM12B in idle

	rcall	begin_RX		; switch RFM12B to listening


; *****************************************************************************
;
; choose RX or TX for this device

; see the TX section below for low-power (power-down) or always live selection
	#define		TX_		; make this node a transmitter
;	#define		RX_		; make this node a receiver


; =============================================================================
; =============================================================================
	#ifdef	RX_

main:	call	check_command_timeout	; check command-mode timeout
	rcall	check_for_input		; check for (and action if appropriate) serial i/p
	jeq	state,state1,err	; loop till either error...(see ext0.asm)
	jne	state,state6,main	; or RX complete

; falls through when valid packet (structuraly correct) is received. RF system now in idle
	call	process_packet		; deal with the packet (connect.asm)

err:	rcall	begin_RX		; restart RX process
	rjmp	main			; and back to the top of main loop

check_command_timeout:
	tst	seconds			; if 'connected' check timeout
	breq	clear_connect		; if timed-out, jump
	ret				; or return
clear_connect:
	sts	outgoing_connect,seconds; set connect state to zero (none)
	ret
; =============================================================================
; =============================================================================




; =============================================================================
; =============================================================================
	#elif	TX_


; =============================================================================
;	#define	low_power	; un-comment for using power-down mode
; =============================================================================
	  #ifdef low_power	
; this is the low-power choice, uses power-down mode between actions
main:
;	rcall	enter_sleep3		; OFF for 59 sec. (See end of RFM12B.asm)
	rcall	enter_sleep1		; OFF for 4 sec
					; then wakeup.....
	rcall	start_temp		; start 1-wire temp conv. Ints OFF for about 2.2ms
	rcall	enter_sleep2		; OFF for 1 sec while temp conversion in progress

; temp conversion will now be complete so get the result. 
	call	read_result		; ints are off for about 7.4ms

; Got data from sensor so pack it up and send it
	lds	datacount,Pcount	; save the count
	ldx	ram,TX_base		; start of TX ram block
	rcall	prepare_tx		; build packet in memory then start sending

	rcall	wait_TXend		; wait till the TX is completed

	rcall	begin_RX		; to use sleep mode on RFM12B, must cycle thru mode change
	call	wait_for_ack		; waits up to 20ms for an ACK
	brcs	gotone

; put resend stuff here if required
	rjmp	main

; ack received within the timeout period
gotone:
	rjmp	main
; =============================================================================
; =============================================================================


	  #else
; this is the always-on choice. Allows command connection from a remote node

main:	rcall	start_temp	; start 1-wire temp conv. Ints OFF for about 2.2ms
	rcall	begin_RX	; start RF receive background-task process

;------------------------------------------------------------------------------ 
	csloop_for 10			; loop for 10 centi-secs (1 second)
rxlp:		rcall	check_for_input	; check for (and action if appropriate) serial i/p (proc.asm)
		cpi	state,state1	; is there a RX error? (ext0.asm)
		brne	noerr		; if not, jmp
		rcall	begin_RX	; error so restart RX process (rfm12b.asm)
noerr:		cpi	state,state6	; has a packet been received?
		brne	nopack
		call	process_packet	; packet arrived so action it (connect.asm)
		rcall	begin_RX	; then restart RX process
nopack:
	csloop_to rxlp
;------------------------------------------------------------------------------ 

; falls thru after 1 sec so now read the sensor. Timing critical so
; interrupts are turned OFF during the read then back on again.
	stopRF				; RF system to idle
	call	read_result		; ints are off for about 7.4ms (1wire.asm)

; Got data from sensor so pack it up and send it
	lds	datacount,Pcount	; save the count of chars 'printed' to ram. The payload count.
	ldx	ram,TX_base		; start of TX ram block
	rcall	prepare_tx		; build packet in memory then start sending (rfm12B.asm)

	rcall	wait_TXend		; wait till the TX is completed (rfm12B.asm)

	call	wait_for_ack		; waits up to 20ms for an ACK (connect.asm)
	brcs	gotone

; put resend stuff here if required
	rjmp	main

; ack received within the timeout period
gotone:
	rjmp	main

	  #endif	; end of (low-power/always-on) choice

	#endif		; end of RX/TX choice









.include	"subs.asm"
.include	"ser0.asm"
.include	"t0.asm"
.include	"ext0.asm"
.include	"RFM12B.asm"		; all the main stuff for the RF system
.include	"SPI.asm"		; short & simple
.include	"crc.asm"		; CRC routines
.include	"1wire.asm"		; dallas 1-wire stuff
.include	"proc.asm"		; serial input to linebuffer, Part of command interp
.include	"cmd_tables.asm"	; line parsing and command tables
.include	"config.asm"		; number extraction from command paramater + channel & data-rate display
.include	"eeprom.asm"		; eeprom config set
.include	"connect.asm"		; 
.include	"commandmode.asm"	; 
.include	"timefunc.asm"
;.include	"README.txt"




