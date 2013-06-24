

;.include	"tn861def.inc"		; un-comment for AVR studio-4
.include	"macros.inc"

.equ	TX_node_address	= 4	; node address for TX & RX (RX not used in this app)


#define	led_on		sbi portb,5	; 
#define	led_off		cbi portb,5	; 

#define SSpin		portb,3		; slave select pin for RFM12B

;#define	RX_enable	; builds with or without receive functionanlity

;==============================================================================
;
; ***************  Y-reg reserved for use in ext0 int routine******************
;
;==============================================================================

.equ	true		= 0xff
.equ	false		= 0

; output stream destinations
.equ	null_stream	= 0	; nowhere
.equ	serial_stream	= 1	; serial out
.equ	memory_stream	= 2	; memory buffer


.equ	preamble 	= 0xAA	; for the receiver to lock-up to
.equ	syncbyte	= 0x2D	; 1st sync-byte (fixed in receiver silicon)
.equ	group		= 0xD4	; default group (definable in software)

.equ	ram = 1			; used for the ldX/ldY/ldZ/ldACC macros
.equ	rom = 2

; 'state' used in ext0_int for RX
.equ	state0		= 0	; idle
.equ	state1		= 1	; error condition. RF system set to idle
.equ	state2		= 2	; initiated RX. On 1st RX byte, start timeout timer.
.equ	state3		= 3	; waiting for count
.equ	state4		= 4	; receiving data
.equ	state5		= 5	; waiting CRC
.equ	state6		= 6	; message complete & CRC good!

; state for the TX functios
.equ	state10		= 10	; doing TX.
.equ	state11		= 11	; TX complete



;==============================================================
;.def	 =		r0	; 
;.def	 =		r1	; 
.def	zero	 =	r2	; always contains zero

;.def	 =		r3	; -
;.def	 =		r4	; -
;.def	 =		r5	; -
;.def	 =		r6	; - 
;.def	 =		r7	; -
;.def	 =		r8	; -
;.def	 =		r9	; -
;.def	 =		r10	; -
;.def	 =		r11	; -
;.def	 =		r12	; -
;.def	 =		r13	; -
;.def	 =		r14	; -
;.def	 =		r15	; -

.def	temp1	=	r16	; temp register
.def	temp2	=	r17	; temp register
.def	temp3	=	r18	; temp register
.def	state =		r19	; current 'state' for int routine
.def	datacount =	r20	; count of incoming/outgoing data
.def	headerbyte =	r21	; header info for data block (block-type)
.def	loopcounter =	r22	; counter for loops

.def	bitflags =	r23	; 
; .equ			= 0	; 
; .equ			= 1	; 
; .equ			= 2	; 
; .equ			= 3	; 
; .equ			= 4	; 
; .equ			= 5	; 
; .equ			= 6	; 
 .equ	sleepbit	= 7	; sleep indicator

; r24 & 25 can be used for 16-bit operations
.def	accL =		r24	; 
.def	accH =		r25

;==============================================================================

.dseg
; RF system uses to show what an error was (during RX)
errorcode:		.byte	1	; number representing an error type

stream_destination:	.byte	1	; where an output stream is sent
					; (null, serial, memory)
					; Only null or memory used here


;=============================================================================
; actual code starts here with the int-vector table
.cseg

.include	"vectors_861.asm"	; int vectors. Only ext0 int used.


;******************************************************************************
;			reset vector
;******************************************************************************
; 1st thing to do is clear internal ram to zero
RESET:	clr	zero			; location always set to zero

	ldZ	ram,SRAM_START		; point to start of ram
	ldi	temp1,SRAM_SIZE/256	; get pages of 256 bytes
	clr	loopcounter		; downcount of 256 

wploop:	st	z+,zero			; write 0's to ram
	dec	loopcounter		; inner loop
	brne	wploop
	dec	temp1
	brne	wploop			; outer loop


; set up stack
	ldi	temp1,high(RAMEND)	; Init SP
	out	SPH,temp1
	ldi	temp1,low(RAMEND)
	out	SPL,temp1

; select startup output stream
	stream	null_stream		; set o/p stream to null (nowhere)

;******************************************************************************
; port-A
; bits are assigned as....
; 0-DIN			(in)  0		; USI
; 1-DOUT		(out) 1		; USI
; 2-DSCK		(out) 1		; USI
; 3-AREF		(in)  0		; wired to VCC with capacitor.
; 4-AN3			(in)  0		; 
; 5-AN4			(in)  0		; 
; 6-AN5			(0ut  1		; 
; 7-AN6			(out) 1		;
	ldi	temp1,0b11000110
	out	DDRA,temp1
	ldi	temp1,0b00000100
	out	PORTA,temp1

;******************************************************************************
;******************************************************************************
; port-B
; bits are assigned as....
; 0-mosi		(out) 1		; mosi
; 1-miso		(in)  0		; miso
; 2-SCK			(out) 1		; sck
; 3-SS			(out) 1		; SS
; 4-1-wire		(out) 1		; 1-wire bus
; 5-LED			(out) 1		; user LED
; 6-int0		(in)  0		; ext int0
; 7-rst pin		(in)  0		; not used as IO pin

	ldi	temp1,0b00111101	;
	out	DDRB,temp1
	ldi	temp1,0b00001000
	out	PORTB,temp1
	led_off

;******************************************************************************
;	end of port setup
;******************************************************************************

	clr	bitflags		; make sure all zero
	clr	state			; idle for RFM12B state machine

	ldi	temp1,0b00010000	; assign sleep mode = power-down
	out	mcucr,temp1

; test if eeprom config for RFM12 exists, if not create one.
	rcall	test_config		; see if a eeprom config available
	brcs	yes_config
	rcall	default_config		; create if not exists
yes_config:
	rcall	set_config		; use the config data

; basic inits for modules
	rcall	init_spi		; sets up SPI
	rcall	init_ext0		; sets up ext0 int type (-ve edge)
	sei				; global ints on

	rcall	init_RFM12B		; configs RFM12, leaves in idle mode.
	
; =============================================================================
; =============================================================================
main:	rcall	start_temp		; start 1-wire temp conv. Takes 2.2ms
	led_off				; activity about to stop
	rcall	enter_sleep1		; 1 sec sleep. See end of RFM12.asm
; falls thru after 1 sec so now read the sensor. Timing critical so
; interrupts are turned OFF during the read then back on again.
	led_on				; indicates awake 
	rcall	read_result		; ints are off for about 7.4ms
; Temperature data now in TX buffer

	lds	datacount,Pcount	; save the count
	ldx	ram,TX_base		; start of TX ram block
	rcall	prepare_tx		; finalise packet in memory then send it
	rcall	wait_TXend		; wait till the ToX is completed
					; finished sending and RFM12 back in idle.
	rjmp	main			; and back to sleep again till next time

; =============================================================================
; =============================================================================


.include	"subs.asm"
.include	"ext0.asm"
.include	"1wire_28-06-10.asm"
.include	"RFM12B.asm"
.include	"SPI.asm"
.include	"crc.asm"
.include	"eeprom.asm"





