;*********************************************
;**** Constant declarations Data Rate for delay ****
.equ     Crystal = 1000          ; 1 Mhz
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
; 40us at 1 MHz
;
Delay40us:
	ldi	temp,13
	mov	dcount,temp

D40_uS:
	dec	dcount
	brne	D40_uS
	ret
;***************************************************************
; 100us at 1 MHz
;
Delay100us:
	ldi	temp,33
	mov	dcount,temp

D100_uS:
	dec	dcount
	brne	D100_uS
	ret
;***************************************************************
;debounce for keypress
; 50ms at 1 MHz
;
debounce:
    	ldi  temp, 50
	mov	dcount,temp
	rcall	DelaymS
	ret
;***************************************************************
; 250ms at 1 MHz
Delay250mS:
    	ldi  temp, 250
	mov	dcount,temp
	rcall	DelaymS
	ret
;***************************************************************
; 1s at 1 MHz
OneSecondDelay:
	rcall	Delay250mS
	rcall	Delay250mS
	rcall	Delay250mS
	rcall	Delay250mS
    	ret
;***************************************************************