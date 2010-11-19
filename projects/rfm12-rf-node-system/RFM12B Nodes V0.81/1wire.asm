
; reuse these while not in use elsewhere
.def	index		=	r19	; crc calc routines
.def	send_byte	=	r20	; crc calc routines
.def	loopcounter2	=	r24	; crc calc routines
.def	rChar		=	r10	; crc calc routines



; portd for data pin
.equ	databit		= 3

.dseg
Ppointer:	.byte	2	; mem print pointer
Pcount:		.byte	1	; chars printed to mem



tempdata:	.byte	10	; where to store

ticker:		.byte	1

.CSEG

; *****************************************************************************
; DESCR: resets the sensor and initiates a temp reading
; ENTRY: 
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
start_temp:
	lds	temp1,ticker			; see the bottom of this file
	inc	temp1				; for the ticker routine
	andi	temp1,1
	sts	ticker,temp1

; firstly we prepare some of the items in the TX buffer
	lds	temp1,config_node_address	; THIS nodes address (from config set)
	sts	TX_sourcenode,temp1		; write to the TX buffer as SOURCE addr

	lds	temp1,config_node_dest		; from the default destination in config set
	sts	TX_destnode,temp1		; write to TX buffer as DEST addr

	ldi	headerbyte,temppack		; packet type register. (types in RFM12.asm)


; now set up the destination in ram for the memory_stream output type
	ldx	ram,TX_data_buffer+2		; where to 'print' to. Payload area after source/dest addresses
	storeX	Ppointer			; save pointer

; adjust the 'print to ram' count to include the source/dest addresses
	ldi	temp1,2				; for 1st 2 bytes (source/dest)
	sts	Pcount,temp1			; save it

; the 1-wire routines need defined timings so disable interrupts 
	cli					; stop ALL interrupts

	sbi	portd,databit			; make sure port pins in
	sbi	ddrd,databit			; correct state to start

; reset the bus
	rcall	onewire_reset			; about 1ms , idealy 960uSec.

; initiate a conversion
;====================================================================
	ldi	send_byte,0xcc ; skip rom	; this section takes
	rcall	send_byte_1820			; about 1.2ms
						; 
	ldi	send_byte,0x44 ; do temp conv	; 
	rcall	send_byte_1820			; 
;====================================================================
	sei					; restart ints
	ret




;====================================================================
read_result:
	stream memory_stream			; set o/p stream to RAM

; stop all ints for 1-wire transactions
	cli
	rcall	onewire_reset			; all this section
						; takes about 7.4ms
	ldi	send_byte,0xcc			; skip rom cmd
	rcall	send_byte_1820			; 
						; 
	ldi	send_byte,0xbe			; read scratchpad
	rcall	send_byte_1820			; 
						; 
	ldZ	ram,tempdata
	ldi	loopcounter,9			; 8 data bytes + CRC
						; 
readloop:					; 
	rcall	inbyte_1820			; get conversion results
	st	z+,temp1			; 
	dec	loopcounter			; 
	brne	readloop			; 
;====================================================================
	sei				; re-enable ints

; produce local 1-wire CRC
	ldX	ram,tempdata
	ldi	loopcounter,8
	rcall	crc8			; returns value in temp2

; compare to received
	ld	temp1,x			; gets received CRC value
	cp	temp1,temp2		; compare to localy generated one
	brne	badCRC			; if not equal, error
	rjmp	showtem

badCRC:

	stringout "Bad CRC"		; this is 'printed' to ram
	rjmp	tmpex

; data is good so use it
showtem:
	rcall	showtemp		; value 'printed' to ram
	rcall	do_ticker		; followed by a changing char on alternate samples

tmpex:	stream	serial_stream		; o/p stream set back to serial
	ret



;**********************************************
;****************** DS18S20 Assembly routines *
;****************** atmega 8Mhz portd bit0 *
;***********************************************

inbyte_1820:
	ldi	index,0x07
inbyte_loop:
	rcall	master_read_slot
	dec	index
	cpi	index,0xff
	brne	inbyte_loop
	ret


send_byte_1820:
	ldi	index,0x07
send_byte_loop:
	ror	send_byte
	brcs	send_one
	brcc	send_zero


send_one:
	rcall	Master_write_1_slot
	rjmp	exit_send_byte


send_zero:
	rcall	Master_write_0_slot
exit_send_byte:
	dec	index
	cpi	index,0xff
	brne	send_byte_loop
	ret



Master_read_slot:
	cbi	portd,databit

	cydelay 46

	sbi	portd,databit

	cydelay 70

	clc				; clear carry
	sbis	pind,databit		; check pin value
	rjmp	Master_read_slot_exit
	sec				; set carry

Master_read_slot_exit:
	ror	temp1		; push carry into byte

	cydelay 436

	ret

Master_write_1_slot:
	cbi	portd,databit

	cydelay 46

	sbi	portd,databit

	cydelay	506

	ret


Master_write_0_slot:
	cbi	portd,databit

	cydelay 476

	sbi	portd,databit

	cydelay 78

	ret


; returns C=no device, NC=device
onewire_reset:
	sbi	ddrd,databit		; 2 change pin to o/p
	cbi	portd,databit		; 2 pull bus low

	cydelay 3840

	sbi	portd,databit		; 2 drive high
	cbi	ddrd,databit		; 2 change pin to i/p (pullup is on)


	cydelay	560

	clc			; 1
	sbic	pind,databit	; 2
	sec			; 1

	in	temp1,sreg	; 1
	push	temp1


	cydelay	3280

	pop	temp1		; 2
	out	sreg,temp1	; 1
	sbi	ddrd,databit		; 2 change pin to o/p
	ret


;Library subroutine CRC8
;Calculates CRC-8 of a string
;runs on any Atmel AVR microcontroller that has SRAM stack
;(result matches Dallas Semiconductor's One-Wire [DOW] CRC8 algorithm)
;(use this subroutine to check the CRC8 on serial numbers from Dallas iButtons)
;
;written by  E. Nicholas Cupery  Farba Research  29 April 2000
;
;on input:
;     X register must point to the string in SRAM
;     loopcounter register must hold the byte-count of the string  [0 means 256]
;on exit:
;     temp2 holds the CRC-8 result
;     all other registers are preserved
;     c-bit is indeterminate
;
CRC8:
	push	rChar			;save rChar
	push	loopcounter		;save loopcounter
	push	loopcounter2		;save loopcounter2
	push	temp1			;save temp1

	clr	temp2			;start with a zero CRC-8


	;begin loop to do each byte in the string
CRC8BYT:
	ld	rChar,X+		;fetch next string byte and bump pointer
	ldi	loopcounter2,8		;load the bit-counter for this byte

	;begin loop to do each bit in the byte
CRC8BIT:
	mov	temp1,rChar		;get a temporary copy of current data
	eor	temp1,temp2		;XOR the data byte with the current CRC
	lsr	temp2			;position to the new CRC
	lsr	rChar			;position to next bit of this byte	
	lsr	temp1			;get low bit of old result into c-bit
	brcc	CRC8NA			;br if low bit was clear (no adjustment)
	ldi	temp1,$8C		;magical value needed for CRC-8s
	eor	temp2,temp1		;fold in the magic CRC8 value
CRC8NA:	dec	loopcounter2		;count the previous bit done
	brne	CRC8BIT			;br if not done all bits in this byte
	;end loop to do each bit in the byte

	dec	loopcounter		;count this byte done
	brne	CRC8BYT			;br if not done all bytes in the string
	;end loop to do each byte in the string


        pop	temp1			;restore temp1
        pop	loopcounter2		;restore loopcounter2
	pop	loopcounter		;restore loopcounter
	pop	rChar			;restore rChar
	ret				;return to caller






; =====================================================

showtemp:
	ldZ	ram,tempdata

	ld	temp2,z+	; low byte
	ld	temp1,z+	; high byte
; check if + or -
	sbrs	temp1,7		; skip jump if NEG
	rjmp	pos

	send	'-'		; display for negetive temp

	com	temp1		; invert value
	com	temp2

	inc	temp2		; add 1
	brne	pos		; check for rollover
	inc	temp1

pos:	lsl	temp2
	rol	temp1

	lsl	temp2
	rol	temp1

	lsl	temp2
	rol	temp1

	lsl	temp2
	rol	temp1

	swap	temp2
	rcall	decout
	ldi	temp1,'.'
	rcall	stream_out
	mov	temp1,temp2
	rcall	showLDP
	stringout " deg"

	ret


showLDP:
	clc
	rol	temp1		; mul by 2 for table of words

	clr	temp2
	ldZ	rom,Ttable
	add	zl,temp1
	adc	zh,temp2
	lpm	r0,z+
	push	r0
	lpm	r0,z
	mov	zh,r0
	pop	zl
	ijmp			; jumps to address from table



Ttable:	.dw	tp0,tp1,tp2,tp3,tp4,tp5,tp6,tp7,tp8,tp9,tp10,tp11,tp12,tp13,tp14,tp15

tp0:	stringout	"00"		;25"	 The last two decimal places are removed
	ret					; as not required in my application
tp1:	stringout	"06"		;25"
	ret
tp2:	stringout	"12"		;50"
	ret
tp3:	stringout	"18"		;75"
	ret
tp4:	stringout	"25"		;00"
	ret
tp5:	stringout	"31"		;25"
	ret
tp6:	stringout	"37"		;50"
	ret
tp7:	stringout	"43"		;75"
	ret
tp8:	stringout	"50"		;00"
	ret
tp9:	stringout	"56"		;25"
	ret
tp10:	stringout	"62"		;50"
	ret
tp11:	stringout	"68"		;75"
	ret
tp12:	stringout	"75"		;00"
	ret
tp13:	stringout	"81"		;25"
	ret
tp14:	stringout	"87"		;50"
	ret
tp15:	stringout	"93"		;75"
	ret


; ===================================================

; this just 'prints' a different char on alternate samples so if the 
; temp is static, the char changes so you can see things are still happening 
do_ticker:
	lds	temp1,ticker
	jeq	temp1,0,pat1
	jeq	temp1,1,pat2
	ret

pat1:	ldi	temp1,'#'
	rcall	stream_out
	ret

pat2:	ldi	temp1,'*'
	rcall	stream_out
	ret


