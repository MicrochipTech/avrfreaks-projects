; *********************************************************************************************************************************	
; * pixel morphing
; *********************************************************************************************************************************
; IN:
;   data3 pixel data to be altered
;   data4 pixel data to be approximated
; OUT:
;   data3 altered pixel data
;   data4 is set (255) if pixels had the same values
;   data4 is cleared (0) if alterations happened


morphing:
	push temp					; save temp value

	push data4
	push data3
	push data4

	andi data3, 0b00001111		; clear upper nibble (keep right pixel data)
	andi data4, 0b00001111		; clear upper nibble (keep right pixel data)
	ori data3, 0b10000000		; temporary marking: this is the right pixel that is processed
morph:
	cp data3, data4				; check result of (data3 - data4)
	breq next_morph				; if equal, proceed to next pixel (no change)

	brlo morph_inc				; if result is negative jump to morph_inc, otherwise proceed to next instruction
	dec data3					; if data3 is higher than data4 (postitive result) it has to be decreased
	rjmp next_morph
morph_inc:
	inc data3					; if data3 is higher than data4 (negative result) it has to be increased
	rjmp next_morph

next_morph:
	sbrs data3,7				; if marking exists on data3, proceed to left pixel
	rjmp end_morph				; otherwise, both pixels are edited
	
	andi data3, 0b00001111		; remove marking: this is the left pixel that is going to be processed
	mov temp, data3				; save altered right pixel value in temp

	pop data4
	pop data3

	lsr data4					;
	lsr data4					; shift lef pixel data in place
	lsr data4					;
	lsr data4					;
	lsr data3					;
	lsr data3					; shift lef pixel data in place
	lsr data3					;
	lsr data3					;
	rjmp morph

end_morph:
	pop data4					; data4 now contains data to be approximated
	cp data3, data4				; compare data3 with data4
	brne morph_happened			; if not equal, jump to morph_happened
	ser data4					; if equal, pixels had the same values (load 255 to data4)
	rjmp morphing_end
morph_happened:
	clr data4					; alterations happened (clear data4)
morphing_end:
	or data3, temp				; data3 now contains altered data
	pop temp					; restore temp value
	ret
