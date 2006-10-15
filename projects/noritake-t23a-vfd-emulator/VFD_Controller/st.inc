Start:
	ldi		temp, '*'
	rcall 	asciitoseg	
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, 'a'
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, 't'
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, 'm'
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, 'e'
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, 'l'
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, '-'
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, 't'
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, '2'
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, '3'
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, 'a'
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, '*'
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, $07
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, $06
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, $05
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, $04
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, $03
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, $02
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, $02
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition
	
	ldi		temp, $03
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition
	
	ldi		temp, $04
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, $05
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, $06
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	ldi		temp, $07
	rcall	asciitoseg
	rcall	WriteToRefresh
	inc		charposition

	rjmp 	pc
