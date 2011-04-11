;*********************************************
;**** Constant declarations Data Rate for delay ****
;.equ     Crystal = 7372          ;
.equ     Crystal = 3686          ;
;***************************************************************
;*
;* FUNCTION
;*	delay
;*
;* DESCRIPTION
;* Make delay 1mS (x dcount).
;* uses dcount, dcount2, and dcount3
;* come here with a value in dcount (1 = 1mS)
;***************************************************************
delay:	
	ldi	temp,40
	mov	dcount3,temp
dl2:
	ldi	temp,(Crystal/120)
	mov	dcount2,temp
dl1:
	dec	dcount2
	brne	dl1

	dec	dcount3
	brne	dl2

	dec	dcount
	brne	delay
	ret
;***************************************************************