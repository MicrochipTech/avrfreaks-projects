;************************************************************************/
;*																		*/
;*		       	Access Maxim 1-Wire Devices								*/
;*                                                                      */
;*              Author: Peter Dannegger,                                */
;*                      danni@specs.de                                  */
;*                                                                      */
;*              Will work up to 11MHz CPU-Clock                         */
;************************************************************************/
;-------------------------------------------------------------------------
;				Generate 1-Wire Reset
;-------------------------------------------------------------------------
;Input: none
;Output: C = 1: 1-wire device present
;Used:	a1
;
w1_reset:
	cli
	ldi	a1, 480 * xtal / 1000000 / 21	;max 671*xtal
_w1r1:
	cbi	w1_out, w1_pin					;2
	sbi	w1_ddr, w1_pin					;2	set Pin low
	rcall	_w1r3						;14
	subi	a1, 1						;1	C = 0
	brne	_w1r1						;2
	ldi	a1, 480 * xtal / 1000000 / 21
_w1r2:
	cbi	w1_ddr, w1_pin					;2	Data-Pin high_z
	sbis	w1_in, w1_pin				;1	presence detect
	cpi	a1, 465 * xtal / 1000000 / 21	;1	after 15us
	rcall	_w1r3						;14
	dec	a1								;1
	brne	_w1r2						;2
	sbis	w1_in, w1_pin				;check short circuit on 1Wire-Line
	clc
	reti								;int enable

_w1r3:
	rcall	_w1r4					;7	delay 14 cycle
_w1r4:
	ret								;4

;-------------------------------------------------------------------------
;				Read / Write Bit on 1-wire
;-------------------------------------------------------------------------
;Input	C = bit
;Output	C = bit
;Used	a1
;
w1_bit_rd:
	sec
w1_bit_wr:
w1_bit_io:
	cli
	sbi	w1_ddr, w1_pin				; low
	ldi	a1, 15 * xtal / 1000000 / 3
	brcc	_w1io1
	cbi	w1_ddr, w1_pin				; write 1 bit
_w1io1:
	dec	a1
	brne	_w1io1
	sbis	w1_in, w1_pin			; sample input
	clc
	ldi	a1, 45 * xtal / 1000000 / 3
_w1io2:
	dec	a1
	brne	_w1io2
	cbi	w1_ddr, w1_pin				; high
	reti

;-------------------------------------------------------------------------
;				Read / Write byte
;-------------------------------------------------------------------------
;Input	a0 = byte
;Output a0 = byte
;Used	a0, a1, a2
;
w1_byte_rd:
	ldi	a0, 0xFF
w1_byte_wr:
	ldi	a2, 8
_w1wr1:
	ror	a0			; lsb first
	rcall	w1_bit_io
	dec	a2
	brne	_w1wr1
	ror	a0			; last bit in
	ret
;-------------------------------------------------------------------------
;				ROM Search
;-------------------------------------------------------------------------
;Input	a0 = bit to resolve
;Output	a0 = next bit to resolve
;Used	a0, a1, a2, a3, zl. zh
;---------------------------------
w1_rom_search:
	mov	a3, a0
	rcall	w1_reset
	ldi	a0, presence_err
	brcc	_wrs7
	ldi	a0, search_rom
	rcall	w1_byte_wr
	ldi zh,high(romcode_ptr)
	ldi	zl,low(romcode_ptr)
	ldi	a0, last_device
	ldi	a2, 8 * 16 - 1		; 8 bytes a 8 bit
_wrs1:
	rcall	w1_bit_rd		; read bit
	sbc	a1, a1				; H = C
	rcall	w1_bit_rd		; read complement bit
	ld	a1, z
	brhc	_wrs2
	brcc	_wrs4			; bit = 1
	ldi	a0, data_err
	ret
_wrs2:
	brcs	_wrs5			; bit = 0
	cp	a2, a3
	breq	_wrs5			; bit = 0
	brcs	_wrs3			; bit = 1, resolve
	sbrs	a1, 0
	rjmp	_wrs5			; bit = 0
_wrs3:
	mov	a0, a2				; bit = 1
_wrs4:
	sec
	rjmp	_wrs6
_wrs5:
	clc
_wrs6:
	ror	a1
	st	z, a1
	rol	a1
	rcall	w1_bit_wr
	subi	a2, 2			; after 8 times: H = 1
	brhc	_wrs1
;*********** Original-Code modified *********
	in		Save,SREG	;save Carry-Bit
	adiw	zh:zl,1		;next byte.ADIW changes Carry-Bit!
	out		SREG,Save	;restore original Carry-Bit
;********************************************
	brcc	_wrs1
_wrs7:
	ret
;-------------------------------------------------------------------------

