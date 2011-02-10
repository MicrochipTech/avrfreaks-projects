;--------------------------------------
;100mS delay @ 4MHz (400000 cycles)
Wait100mS:
	ldi	temp, $97
	mov	dcount,temp

W100mS_0:
	ldi	temp, $06
	mov	dcount2, temp
W100mS_1:
	ldi	temp, $92
	mov	dcount3, temp
W100mS_2:
	dec	dcount3
	brne	W100mS_2
	dec	dcount2
	brne	W100mS_1
	dec	dcount
	brne	W100mS_0
	ret
;------------------------------------------------------
;885.75uS @ 4MHz clock (3543 cycles)
Del889uS:
	push	ZL	 	; 
	push	ZH	 	; 
	ldi	ZL, 0x70	; 
	ldi	ZH, 0x03	; 
	rcall	Del889uS_1	; 
	pop	ZH	 	; 
	pop	ZL	 	; 
	ret		   	; 
;------------------------------------------------------
Del889uS_1:
	sbiw	ZL, 0x01	; 
	brne	Del889uS_1	; 
	ret		   	;
;------------------------------------------------------