

#define		oneW_port	portb		; port for 1-wire
#define		oneW_pin	PINB		; pin reg
#define		oneW_dir	DDRB		; direction
.equ	databit		= 4			; bit 4

; pick one of the types below
	#define		DS18B20		; high res, 12-bit temp
;	#define		DS18S20		; low res, 9-bit temp


.dseg

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
	sbi	SSpin				; make sure RFM12 deselected

	cli					; stop ALL interrupts

	sbi	oneW_port,databit		; make sure port pins in
	sbi	oneW_dir,databit		; correct state to start

; set up to read data into ram
	rcall	onewire_reset			; about 1ms , idealy 960uSec.

;====================================================================
	ldi	temp1,0xcc	; skip rom	; this section takes
	rcall	send_byte_1820			; about 1.2ms
						; 
	ldi	temp1,0x44 ; do temp conv	; 
	rcall	send_byte_1820			; 
;====================================================================
	sbi	oneW_dir,databit		; make back to an output
	cbi	oneW_port,databit		; make sure port pin low

	sei					; restart ints
	ret




;====================================================================
read_result:				;			/\
	cli				;			|
					;			|
	sbi	oneW_dir,databit	; make an o/p again	|
	cbi	oneW_port,databit	; make sure port pins in|
					;			|
	rcall	onewire_reset		; all this section	|
					; takes about 7.4ms	|
	ldi	temp1,0xcc		; skip rom cmd		|
	rcall	send_byte_1820		;			|
					;			|
	ldi	temp1,0xbe		; read scratchpad	|
	rcall	send_byte_1820		;			|
					;			|
	ldZ	ram,tempdata		;			|
	ldi	loopcounter,9		; 8 data bytes + CRC	|
					;			|
readloop:				;			|
	rcall	inbyte_1820		; get device code	|
	st	z+,temp1		;			|
	dec	loopcounter		;			|
	brne	readloop		;			|
					;			\/	
;===================================================================
	sbi	oneW_dir,databit	; make an o/p again
	cbi	oneW_port,databit	; make sure pin low
	sei				; re-enable ints

; the 'ticker' is to o/p a char that alternates between two chars on successive reads
; This is to let you see something is happening if the temperature display appears
; to be static. It might just be at the same value but the ticker lets you know its
; a live reading.
	lds	temp1,ticker
	inc	temp1
	andi	temp1,1
	sts	ticker,temp1

; now prep the items for the RFM12 TX buffer prior to initiating a TX of the data
	lds	temp1,config_node_address	; THIS nodes address
	sts	TX_sourcenode,temp1	; as source of data

	lds	temp1,config_node_dest	; destination addr
	sts	TX_destnode,temp1	; where to send to

	ldi	headerbyte,temppack	; packet type

	ldx	ram,TX_data_buffer+2	; where to 'print' to ram (see RFM12.asm)
	storeX	Ppointer		; save pointer
	ldi	temp1,2			; for 1st 2 bytes (source/dest)
	sts	Pcount,temp1		; preload the count

	stream	memory_stream		; set o/p stream        |

	ldX	ram,tempdata		; point to data read from 1-wire sensor
	ldi	loopcounter,8		; byte count
	rcall	crc8			; calculate CRC of data, returns value in temp2

	ld	temp1,x			; gets received CRC value
	cp	temp1,temp2		; compare to localy generated one
	brne	badCRC			; if not equal, error
	rjmp	showtem

badCRC:
	stringout "Bad CRC "		; print error message
	rjmp	tg1			; then jump to ticker

showtem:
	rcall	showtemp		; all good so print temp

tg1:	rcall	do_ticker		; print ticker

tmpex:	stream	null_stream		; o/p stream set to nowhere

	ret



;**********************************************
;****************** DS18S20 Assembly routines *
;****************** atmega 8Mhz portd bit0 *
;***********************************************

inbyte_1820:
	ldi	temp2,0x07
inbyte_loop:
	rcall	master_read_slot
	dec	temp2
	cpi	temp2,0xff
	brne	inbyte_loop
	ret


send_byte_1820:
	ldi	temp2,0x07
send_byte_loop:
	ror	temp1
	brcs	send_one
	brcc	send_zero


send_one:
	rcall	Master_write_1_slot
	rjmp	exit_send_byte


send_zero:
	rcall	Master_write_0_slot
exit_send_byte:
	dec	temp2
	cpi	temp2,0xff
	brne	send_byte_loop
	ret



Master_read_slot:
	cbi	oneW_port,databit

	cydelay 46

	sbi	oneW_port,databit

	cydelay 70

	clc				; clear carry
	sbis	oneW_pin,databit	; check pin value
	rjmp	Master_read_slot_exit
	sec				; set carry

Master_read_slot_exit:
	ror	temp1			; push carry into byte

	cydelay 436

	ret

Master_write_1_slot:
	cbi	oneW_port,databit

	cydelay 46

	sbi	oneW_port,databit

	cydelay	506

	ret


Master_write_0_slot:
	cbi	oneW_port,databit

	cydelay 476

	sbi	oneW_port,databit

	cydelay 78

	ret


; returns C=no device, NC=device
onewire_reset:
	sbi	oneW_dir,databit		; 2 change pin to o/p
	cbi	oneW_port,databit		; 2 pull bus low

	cydelay 3840

	sbi	oneW_port,databit		; 2 drive high
	cbi	oneW_dir,databit		; 2 change pin to i/p (pullup is on)


	cydelay	560

	clc			; 1
	sbic	oneW_pin,databit; 2
	sec			; 1

	in	temp1,sreg	; 1
	push	temp1

	cydelay	3280

	pop	temp1		; 2
	out	sreg,temp1	; 1
	sbi	oneW_dir,databit; 2 change pin to o/p
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
	push	accL		;save accL
	clr	temp2		;start with a zero CRC-8


	;begin loop to do each byte in the string
CRC8BYT:
	ld	temp3,X+	;fetch next string byte and bump pointer
	ldi	accL,8		;load the bit-counter for this byte

	;begin loop to do each bit in the byte
CRC8BIT:
	mov	temp1,temp3	;get a temporary copy of current data
	eor	temp1,temp2	;XOR the data byte with the current CRC
	lsr	temp2		;position to the new CRC
	lsr	temp3		;position to next bit of this byte	
	lsr	temp1		;get low bit of old result into c-bit
	brcc	CRC8NA		;br if low bit was clear (no adjustment)
	ldi	temp1,$8C	;magical value needed for CRC-8s
	eor	temp2,temp1	;fold in the magic CRC8 value
CRC8NA:	dec	accL		;count the previous bit done
	brne	CRC8BIT		;br if not done all bits in this byte
	;end loop to do each bit in the byte

	dec	loopcounter	;count this byte done
	brne	CRC8BYT		;br if not done all bytes in the string
	;end loop to do each byte in the string


        pop	accL		;restore accL
	ret			;return to caller






	#ifdef	DS18B20
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
	push	loopcounter
	ldi	loopcounter,1
	add	temp2,loopcounter
	clr	loopcounter
	adc	temp1,loopcounter
	pop	loopcounter
	rjmp	st1


pos:	push	temp1
	send	'+'
	pop	temp1

st1:	lsl	temp2
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
	send	' '
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

tp0:	send	'0'	;	"00"		;25"	 The last two decimal places are removed
	send	'0'	;
	ret					; as not required in my application
tp1:	send	'0'
	send	'6'
	ret
tp2:	send	'1'
	send	'2'
	ret
tp3:	send	'1'
	send	'8'
	ret
tp4:	send	'2'
	send	'5'
	ret
tp5:	send	'3'
	send	'1'
	ret
tp6:	send	'3'
	send	'7'
	ret
tp7:	send	'4'
	send	'3'
	ret
tp8:	send	'5'
	send	'0'
	ret
tp9:	send	'5'
	send	'6'
	ret
tp10:	send	'6'
	send	'2'
	ret
tp11:	send	'6'
	send	'8'
	ret
tp12:	send	'7'
	send	'5'
	ret
tp13:	send	'8'
	send	'1'
	ret
tp14:	send	'8'
	send	'7'
	ret
tp15:	send	'9'
	send	'3'
	ret


; ===================================================
	#elif	DS18S20

showtemp:
	lds	temp1,tempdata		; 1st byte of temp data
	push	temp1

	lsr	temp1
	rcall	dec_tx_mem
	pop	temp1
	sbrs	temp1,0
	rjmp	dohalf
	stringout ".5"
	ret

dohalf:
	stringout ".0"
	ret
; =====================================================
	#endif


do_ticker:
	lds	temp1,ticker
	jeq	temp1,0,pat1
	jeq	temp1,1,pat2
	ret

pat1:	ldi	temp1,'#'
	rcall	stream_out
	rjmp	tikend

pat2:	ldi	temp1,'*'
	rcall	stream_out
	

tikend:
	ret

