;****************************************
; Delay subroutines
;****************************************
;200uS delay @ 4MHz
Del_200uS:
	ldi  r21, $02
D_200:
	ldi  r20, $83
D_200_1:
	dec  r20
	brne D_200_1
	dec  r21
	brne D_200
	ret
;****************************************
;1 mS delay @4MHz
Del_1mS:
	ldi  R20, $1F
Del_1mS1:
	ldi  R21, $2A
Del_1mS2:
	dec  R21
	brne Del_1mS2
	dec  R20
	brne Del_1mS1
	ret
;****************************************
;5.25uS @4MHz
Del_5uS:
	ldi  R21, 4
Del_5uS1:
	dec  R21
    	brne Del_5uS1
	ret
;****************************************
;10mS Delay @4MHz
Delay10mS:
	ldi	temp,10
Del_10mS:
	rcall	Del_1mS
	dec	temp
	brne	Del_10mS
	ret
;****************************************