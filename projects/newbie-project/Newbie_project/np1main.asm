;***** Push button to light LED *****
.include "8515def.inc"

	rjmp	RESET

;***** Main *****

RESET:
	;***** Init the  stack pointer *****
	ldi	r16,high(RAMEND)
	out	SPH,r16
	ldi	r16,low(RAMEND)
	out	SPL,r16

	;***** Initiera ports, A = in, C = out *****
	clr	r16
	out	DDRA,r16
	out	PORTA,r16
	ser	r16
	out	DDRC,r16

	;***** Read prot A, Write to port C *****
LOOP:
	in	r15,PINA
	out	PORTC,r15
	rjmp	LOOP		;Loop forever