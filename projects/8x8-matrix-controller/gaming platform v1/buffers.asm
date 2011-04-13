

; ----------------------------------------------
; BUFFER MANIPULATION SUBROUTINES
; ----------------------------------------------


; *********************************************************************************************************************************
; * clear all (primary and secondary) buffers
; *********************************************************************************************************************************

clr_bf:
	cli							; atomic transaction starts here: Interrupts are disabled

	ldi	ZH, SRAM_pri_fb_hi		; prepare X pointer for primary frame buffer
	ldi	ZL, SRAM_pri_fb_lo		;

	ldi data2, 96				; number of bytes to clear
	clr data1
clear_byte:
	st	Z+, data1
	dec data2
	brne clear_byte

	sei							; atomic transaction ends here: Interrupts are enabled
	ret



; *********************************************************************************************************************************
; * copy data from secondary to primary frame buffer
; *********************************************************************************************************************************

sec2pri:
	cli							; atomic transaction starts here: Interrupts are disabled

	ldi	YH, SRAM_sec_fb_hi		; prepare Y pointer for secondary frame buffer
	ldi	YL, SRAM_sec_fb_lo		;
	ldi	XH, SRAM_pri_fb_hi		; prepare X pointer for primary frame buffer
	ldi	XL, SRAM_pri_fb_lo		;

	ldi temp, 32				; number of bytes to copy
copy_byte1:
	ld data1, Y+				; read from secondary
	st X+, data1				; write to primary
	dec temp
	brne copy_byte1

	sei							; atomic transaction ends here: Interrupts are enabled
	ret



; *********************************************************************************************************************************
; * copy data from flash memory to ram (secondary or tertiary buffer)
; *********************************************************************************************************************************
; IN:
;	temp : address offset in flash table
;   data4 bit0: 0 : copy to secondary buffer
;   data4 bit0: 1 : copy to tertiary buffer

flash2ram:
	cli							; atomic transaction starts here: Interrupts are disabled

	sbrc data4, 0				; check bit0 of data4
	rjmp to_tertiary			; if set, prepare pointer for tertiary buffer

	ldi	XH, SRAM_sec_fb_hi		; prepare X pointer for secondary frame buffer
	ldi	XL, SRAM_sec_fb_lo		;
	rjmp tab_ptr				; proceed to table_pointer init

to_tertiary:
	ldi	XH, SRAM_ter_fb_hi		; prepare X pointer for tertiary frame buffer
	ldi	XL, SRAM_ter_fb_lo		;

tab_ptr:
	ldi ZH, high (aliens * 2)	; starting address of flash table
	ldi ZL, low (aliens * 2)	; starting address of flash table
	add ZL, temp				; add address offset
	clr temp					;
	adc ZH, temp					; add address offset

	ldi temp, 32				; counter for bytes to be transfered
copy_byte2:
	lpm							; load from flash memory to r0 (default register used by lpm)
	st X+, r0					; store in tertiary buffer
	adiw ZH:ZL, 1				;
	dec temp					; decrease byte counter
	brne copy_byte2				; repeat until counter=0

	sei							; atomic transaction ends here: Interrupts are enabled
	ret



; *********************************************************************************************************************************
; * create new buffer (secondary) from approximation of primary's to tertiary's data
; *********************************************************************************************************************************
; OUT:
;   data2: clear if all pixel couples were equal (no morphing happened)
;   data2: set if any pixel couple was different (morphing happened)

morphing_bf:
	ldi	XH, SRAM_pri_fb_hi
	ldi	XL, SRAM_pri_fb_lo
	ldi	ZH, SRAM_sec_fb_hi
	ldi	ZL, SRAM_sec_fb_lo
	ldi	YH, SRAM_ter_fb_hi
	ldi	YL, SRAM_ter_fb_lo

	clr data2
	ldi data1, 32
morph_next:
	ld data3, X+
	ld data4, Y+
	cp data3, data4
	breq dont_morph
	ser data2
	rcall morphing
dont_morph:
	st Z+, data3
	dec data1
	brne morph_next
	ret



; *********************************************************************************************************************************
; * shift secondary buffer contents one row downwards
; *********************************************************************************************************************************

shift_bf:
	ldi	XH, SRAM_ter_fb_hi		; prepare X pointer for tertiary frame buffer (one byte after the end of secondary)
	ldi	XL, SRAM_ter_fb_lo		;
	ldi	YH, SRAM_ter_fb_hi		; prepare Y pointer for tertiary frame buffer (one byte after the end of secondary)
	ldi	YL, SRAM_ter_fb_lo		;

	sbiw XH:XL, 8				;
	sbiw YH:YL, 4				;
	rcall copy_row				; transfer a row

	ldi data2, 6				; transfer six more rows
six_repetitions:
	sbiw XH:XL, 8				;
	sbiw YH:YL, 8				;
	rcall copy_row				; transfer a row
	dec data2
	brne six_repetitions
	ret

copy_row:
	ldi temp, 4					; number of bytes to transfer
transfer_byte:
	ld data1, X+
	st Y+, data1
	dec temp
	brne transfer_byte
	ret