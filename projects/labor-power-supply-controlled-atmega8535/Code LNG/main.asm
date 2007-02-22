;***********************************************
; Labornetzgerät 2006:                      main
;
; uProzessor gesteuert: Atmega8535
;
; hirschmann vitalis
; 18.04.2006
;***********************************************
main:

	sbis	PINC, PGM1
	rcall	Prog_1

	sbis	PINC, PGM2
	rcall	Prog_2
	
	sbis	PINC, UP1
	rcall	UP_1
	
	sbis	PINC, DN1
	rcall	DN_1
	
	sbis	PINC, UP2
	rcall	UP_2
	
	sbis	PINC, DN2
	rcall	DN_2
	
	rjmp	main	
	
	

