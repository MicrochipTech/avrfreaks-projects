; Very SMALL AVR code sqrt(num)
; adopted by Jack Tidwell
.def sqrt  = r0
.def numlo = r16
.def numhi = r17
.def subber = r24
.def subberh= r25

; Enter w/the 16bit number to take the sqrt of in r16,r17 (low/high)
Sqrt:
	clr	sqrt
	ldi	suber,1	; initialize the seed to 1
	clr	suberh
loop:
	sub	numlo,subber
	sbc	numhi,subberh
	brlo	exit
	inc	sqrt
	adiw	subber,2	; keep our "subber" ODD
	rjmp	loop
exit:
	ret             ; the sqrt(num) is in sqrt (r0), always an 8bit max
