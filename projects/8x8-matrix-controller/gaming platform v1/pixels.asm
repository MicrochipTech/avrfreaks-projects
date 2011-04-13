

; ----------------------------------------------
; PIXEL MANIPULATION SUBROUTINES
; ----------------------------------------------


; *********************************************************************************************************************************	
; * pixel morphing
; *********************************************************************************************************************************
; IN:
;   data3 : pixel data to be altered
;   data4 : pixel data to be approximated
; OUT:
;   data3 : altered pixel data

morphing:
	push temp					; save temp value
	push data3
	push data4

	andi data3, 0b00001111		; clear upper nibble (keep right pixel data)
	andi data4, 0b00001111		; clear upper nibble (keep right pixel data)

	ldi temp, 0b11110000		; temporary marking: this is the right pixel that is processed
morph:
	cp data3, data4				; check result of (data3 - data4)
	breq next_morph				; if equal, proceed to next pixel (no change)

	cp data3, data4				; check result of (data3 - data4)
	brlo morph_inc				; if result is negative jump to morph_inc, otherwise proceed to next instruction
	dec data3					; if data3 is higher than data4 (postitive result) it has to be decreased
	rjmp next_morph
morph_inc:
	inc data3					; if data3 is higher than data4 (negative result) it has to be increased
	rjmp next_morph

next_morph:
	cpi temp, 0b11110000		; if marking exists on temp, proceed to left pixel
	brne end_morph				; otherwise, both pixel values have been processed
	
	mov temp, data3				; save altered right pixel value in temp
								; marking will also be deleted! time for the left pixel

	pop data4
	pop data3

	swap data3					; shift left pixel data in place
	andi data3, 0b00001111		;
	swap data4					;
	andi data4, 0b00001111		;

	rjmp morph

end_morph:
	swap data3					; put left pixel data in high nibble
	andi data3, 0b11110000		;

	or data3, temp				; data3 now contains altered data
	pop temp					; restore temp value
	ret

; *********************************************************************************************************************************	
; * random pixel generator
; *********************************************************************************************************************************

rnd_pixel:
	rcall randomizer
	andi data3, 0b01110111
	ldi data4, 0b10001111
	rcall rw_pixel_data
	ret

; *********************************************************************************************************************************	
; * read/write pixel data from coordinates
; *********************************************************************************************************************************
; IN:
;	data3 [high nibble]: row number [0:upper matrix row  -->  7:lower matrix row]
;	data3 [low nibble]: col number [0:leftmost matrix col  -->  7:rightmost matrix col]
;   data4 [high nibble]: 0000 read from primary buffer
;   data4 [high nibble]: 0001 read from secondary buffer
;   data4 [high nibble]: 1000 write on secondary buffer
;   data4 [high nibble]: 1100 write on tertiary buffer
;   data4 [low nibble]: data to be written
; OUT:
;   data4 : read pixel data

rw_pixel_data:
	push temp
	push data2

	mov data2, data3

	andi data3, 0b11110000			; mask out col number / keep row number
	lsr data3						;
	lsr data3						; address offset for row (row_num * 4)

	andi data2, 0b00001111			; mask out row number / keep col number
	lsr data2						; address offset for colun (col_num / 2)

	in temp, SREG					; copy SREG for C flag testing

	add data3, data2				; data3 holds the whole address offset

	sbrc data4, 7					;
	rjmp write_pixel_data			; if set, write data

read_pixel_data:
	sbrc data4, 4					;
	rjmp from_secondary				; if set, read from secondary

	ldi	XH, SRAM_pri_fb_hi			; prepare pointer for primary buffer
	ldi	XL, SRAM_pri_fb_lo			;
	rjmp read_data
from_secondary:
	ldi	XH, SRAM_sec_fb_hi			; prepare pointer for secondary buffer
	ldi	XL, SRAM_sec_fb_lo			;

read_data:
	add XL, data3					; add pointer the address offset
	ld data4, X						; load pixel data byte
	andi temp, 0b00000001			; check if Carry bit (bit0) was set or not
	breq get_high					;
	rjmp rw_pixel_data_end
get_high:
	swap data4 						; get high nibble
	rjmp rw_pixel_data_end

write_pixel_data:
	sbrc data4, 6					; check destination for writting operation
	rjmp write_to_tertiary			;

write_to_secondary:
	ldi	XH, SRAM_sec_fb_hi			; prepare pointer for tertiary buffer
	ldi	XL, SRAM_sec_fb_lo			;
	rjmp write_pixel_do
write_to_tertiary:
	ldi	XH, SRAM_ter_fb_hi			; prepare pointer for tertiary buffer
	ldi	XL, SRAM_ter_fb_lo			;

write_pixel_do:
	andi data4, 0b00001111			; keep only pixel data to be written

	add XL, data3					; add pointer the address offset
	ld data3, X						; load pixel data byte
	andi temp, 0b00000001			; check if Carry bit (bit0) was set or not
	breq put_high					;
	andi data3, 0b11110000			; clear low nibble (make room for new data)
	rjmp write_data
put_high:
	andi data3, 0b00001111			; clear high nibble (make room for new data)
	swap data4						; put data in high nibble
write_data:	
	or data3, data4					; combine pixel values in one byte
	st X, data3						; write pixel data byte

rw_pixel_data_end:
	andi data4, 0b00001111			; keep only asked pixel data (inconsistent data when writting)
	pop data2
	pop temp
ret
