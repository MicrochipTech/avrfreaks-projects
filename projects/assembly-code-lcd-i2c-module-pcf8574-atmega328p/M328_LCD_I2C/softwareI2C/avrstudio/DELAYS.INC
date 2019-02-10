;*********************************************
;**** Constant declarations Data Rate for delay ****
;.equ     Crystal = 7372          ;
;.equ     Crystal = 3686          ;
.equ     Crystal = 16000         ;
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
DelaymS:	
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
	brne	DelaymS
	ret
;***************************************************************
; 40us at 16 MHz
;
Delay40us:
	ldi	temp,211
	mov	dcount,temp

D40_uS:
	dec	dcount
	brne	D40_uS
	ret
;***************************************************************
; 100us at 16 MHz
;
Delay100us:
	ldi	temp,3
	mov	dcount,temp

	ldi	temp,16
	mov	dcount2,temp

D100_uS:
	dec	dcount2
	brne	D100_uS
	dec	dcount
	brne	D100_uS
	ret
;***************************************************************
