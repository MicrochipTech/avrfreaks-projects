;***************************************************************
;500mS delay @ 4MHz (2000000 cycles)
HalfSecDelay:
	ldi	temp, $12
	mov	dcount3,temp

HSD0:
	ldi	temp, $BC
	mov  dcount2, temp

HSD1:
	ldi	temp, $C4
	mov	dcount, temp

HSD2:
	dec	dcount
	brne	HSD2
	dec	dcount2
	brne	HSD1
	dec	dcount3
	brne	HSD0
	ret
;***************************************************************
;999605uS (1 sec) delay @ 4MHz  (3998420 cycles)
OneSecDelay:
	ldi	temp, $16	
	mov	dcount,temp
OSD_0:
	ldi	temp, $F2	
	mov	dcount2,temp
OSD_1:
	ldi	temp, $F9	
	mov	dcount3,temp
OSD_2:
	dec	dcount3
	brne	OSD_2
	dec	dcount2
	brne	OSD_1
	dec	dcount
	brne	OSD_0
	ret
;*******************************************
FiveSecDelay:
	ldi	temp,5		;five times around
	mov	dcount4,temp
FSD:
	rcall	OneSecDelay
	dec	dcount4
	brne	FSD
	ret
;*******************************************