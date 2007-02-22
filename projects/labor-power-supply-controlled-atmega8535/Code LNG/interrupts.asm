;***********************************************
; Labornetzgerät 2006:   routines d'interruption
;
; uProzessor gesteuert: Atmega8535
;
; hirschmann vitalis
; 18.04.2006
;***********************************************

int_0:
	in		_sreg, SREG

	INVP	PORTA, Relon

	rcall	lcd_busy
	sbic	PORTA, Relon
	rjmp	out_on

	cbi		PORTC, LED1							; allumer LED
	WAIT_MS	700
	rjmp	ret_1

out_on:
	sbi		PORTC, LED1
	WAIT_MS	500

ret_1:
	rcall	lcd_startup
	OUTI	GIFR, (1<<INTF0)					; pas d'interruption durant la routine
	out		SREG, _sreg							;  -> prellfrei
	reti


int_1:
	rcall	lcd_busy

	INVP	PORTC, LED2

	lds		_w, program2
	cpi		_w, 2
	breq	int1_pg2
	cpi		_w, 0
	breq	int1_pg0

int1_pg0:
	rjmp	exit_int1

int1_pg2:
	in		_w, TCCR1A							; invert enable (servo on/off)
	INVB	_w, COM1A1							
	out		TCCR1A, _w							
	rjmp	exit_int1

exit_int1:
	WAIT_MS	500
	rcall	lcd_startup
	OUTI	GIFR, (1<<INTF1)					; pas d'interruption durant la routine
	reti										;  -> prellfrei
