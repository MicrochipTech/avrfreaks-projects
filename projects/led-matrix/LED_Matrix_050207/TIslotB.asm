;-------------------------------------------------------------------------------
;---------------------- SLOT MAKER B---------------------------------------------
;-------------------------------------------------------------------------------
;32,55µs call

_SLOT000make:
	;status-register auf den stack	
	push	r5
	in	r5,sreg		
	;timer 0 reload !
	ldi	r16,tiREL	;reload timer0 --> nach jedem overload folgt ein reload !!
	out	tcnt0,r16


	;--------------------------------------------------------------------------
	;check slot 
	lds	r16,slotCNT
	cpi	r16,8
	brsh	_SLOT008make
	;0...7
	rcall	_PWM000run		;bei slot 0...7 aufrufen
	;NEW
	rcall	_i2c000A		;i2c...
	inc	r16
	rjmp	_SLOTback
	;--------------------------------------------------------------------------
	;>7
_SLOT008make:
	;NEW
	rcall	_i2c000A		;i2c...
	rcall	_PWM000load		;bei jedem 8ten slot aufrufen....
	clr	r16

	;--------------------------------------------------------------------------
_SLOTback:
	;Rücksprung aus Interrupt - Routine 
	;	
	sts	slotCNT,r16
	out	sreg,r5
	pop	r5
	reti		
	

