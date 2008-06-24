;-----------------------------------------------------------------------
;------------------- PWM LOOP ------------------------------------------
;-----------------------------------------------------------------------
;slot 0&1 	// 32,55µs pro step
;              1,042ms on-time bei 32 steps /LED(RGB)
;		   62,52ms on-time bei 32 steps * 60 fps
;		   Verhältnis: 1/16
;-----------------------------------------------------------------------
_PWM000runXX:
	rjmp	_PWM000runX
_PWM000run:
	;pwm istwert
	lds	r17,Ipwm		;ist-counter
	tst	r17
	brne	_PWM000runXX	 ;	bei 0 ports einschalten
	;lade counter 
	lds	r23,LEDcnt	;0...47
	cpi	r23,47
	brlo	_NXTledGO
	clr	r23
_NXTledGO:
	clr	zh
	mov	zl,r23
	;adressiere x/y - tabelle
	ldi	yl,low(matrixTBL*2)
	ldi	yh,high(matrixTBL*2)
	add	zl,yl
	adc	zh,yh
	lpm		;load x-position
	mov	r18,r0
	adiw	zh:zl,1
	lpm	;load y-position
	mov	r19,r0
	adiw	zh:zl,1
	lpm	;load rgb pointer
	mov	r20,r0
	;store pointer
	sts	RGBpointer,r20

	;kopiere nach ports
	out	porta,r19
	com	r18
	out	portc,r18
		
	;led counter +3
	inc	r23
	inc	r23
	inc	r23
	sts	LEDcnt,r23




;--------------------------------------
;neue RGB Werte aus Speicher...
;format:
;<PWMred0>,<PWMgreen0>,<PWMblue>,<PWMred1>,<PWMgreen1>.....
;
;WICHTIG: * PWM-Tabellenwerte können jederzeit geändert werden. 
;	    * Dynamische Variablen, wie: SpwmRED etc. dürfen außerhalb dieser
;		Routine nicht beeinflusst werden !

	;pointer in r20 !
_PWMnewVAL:
	;HIER AUCH GESAMTHELLIGKEIT REGELN
	lds	zl,rgbHELL
	cpi	zl,pwmMAX
	brsh	_PWMmaxVAL
	clr	yh
	mov	yl,r20	
	subi	yl,low(-RGBstorage)
	sbci	yh,high(-RGBstorage)

	;rot
	ld	r20,y+
	add	r20,zl
	cpi	r20,pwmMAX+1
	brlo	_PWMnewVALa
	ldi	r20,pwmMAX
_PWMnewVALa:
	sts	SpwmRED,r20


	;grün
	ld	r20,y+
	add	r20,zl
	cpi	r20,pwmMAX+1
	brlo	_PWMnewVALb
	ldi	r20,pwmMAX
_PWMnewVALb:
	sts	SpwmGREEN,r20


	;blau
	ld	r20,y
	add	r20,zl
	cpi	r20,pwmMAX+1
	brlo	_PWMnewVALc
	ldi	r20,pwmMAX
_PWMnewVALc:
	sts	SpwmBLUE,r20
	rjmp	_PWM000runX



_PWMmaxVAL:
	ldi	r20,pwmMAX
	sts	SpwmRED,r20
	sts	SpwmGREEN,r20
	sts	SpwmBLUE,r20

	;-------------------------------------------
_PWM000runX:
	;pwm sollwerte
	lds	r20,SpwmRED		;red soll-counter
	lds	r21,SpwmGREEN	;green soll-counter
	lds	r22,SpwmBLUE	;blue soll-counter
	;---------------------------------------
	;test red
	cp	r17,r20
	brsh	_redOFF
	sbi	portb,pinb0		;red high-aktiv
	rjmp	_TSTgreen
_redOFF:
	cbi	portb,pinb0		;red low-inaktiv
	;---------------------------------------
	;test green
_TSTgreen:
	cp	r17,r21
	brsh	_greenOFF
	sbi	portb,pinb1		;green high-aktiv
	rjmp	_TSTblue
_greenOFF:
	cbi	portb,pinb1		;green low-inaktiv
	;---------------------------------------
	;test blau
_TSTblue:
	cp	r17,r22
	brsh	_blueOFF
	sbi	portb,pinb2		;blue high-aktiv
	rjmp	_TSTsize
_blueOFF:
	cbi	portb,pinb2		;blue low-inaktiv
	;--------------------------------------
	;cycle größe testen // max.31
_TSTsize:
;	inc	r17
	cpi	r17,pwmMAX
	breq	_NXTled		;ports aus - warte auf nächste led
	inc	r17
	sts	Ipwm,r17	
	rjmp _PWM000back


	;--------------------------------------
	;ports aus - slot fertig
_NXTled:
	;+++++++neu+++++++++++++++++++++
	lds	zl,rgbHELLte
	sts	rgbHELL,zl
	;+++++++++++++++++++++++++++++++

	;pwm's sperren
	sbi	portb,pinb0		;red 
	sbi	portb,pinb1		;green 
	sbi	portb,pinb2		;blue 
	;ist counter =0 
	clr	r17
	sts	Ipwm,r17
	;ports abschalten
	ldi	r18,0xff
	out	portc,r18
	clr	r18
	out	porta,r18
	rjmp _PWM000back


;-------------------------------------------------------------------------------------------------
	;...back
_PWM000back:
	ret

;-------------------------------------------------------------------------------------------------

