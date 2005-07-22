
.include "tn13def.inc"
.def	reg16 = R16
.def	count = R17
.def	peakoff = R18

.org	0x0000
	rjmp	RESET

.org	0x1F
RESET:
	rcall	INIT
	rcall	DISCHARGE
	rcall	CHARGE
	rcall	CALABRATE

	rjmp	RESET

INIT:;setup everything
	ldi	reg16,	0x22			;set ADC input do ADC2
	out	ADMUX,	reg16

	;enable and configure ADC
	ldi	reg16,	(1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)
	out	ADCSRA,	reg16

	;disable data pin for ADC use
	ldi	reg16,	(1<<ADC2D)
	out	DIDR0,	reg16
	ret

START_CON2:	;begin conversion on pin ADC2
	ldi	reg16,	(1<<ADSC) | (1<<ADEN) | (1<<ADPS2) | (1<< ADPS1) | (1<<ADPS0)
	out	ADCSRA,	reg16
	ret

GET_CON2:
	in	reg16,	ADCSRA	;loop untill conversion is complete
	bst	reg16,	4
	brtc	GET_CON2

	;get and store data from ADCH
	in	reg16,	ADCL
	in	reg16,	ADCH
	
	ret


DISCHARGE:	;Discharge the capacitor untill it is below 10/256
	cbi	PORTB,	PORTB0
	rcall	START_CON2
	rcall	GET_CON2
	CPI	reg16,	10
	BRSH	DISCHARGE
	
	RET


CHARGE:		;Charge the capacitor untill it is above 240/256
	sbi	PORTB,	PORTB0

	ldi	count,	0	;set counter to zero

CHARGELOOP:
	inc	count		;increment Z

	rcall	START_CON2
	;insert aditional overhead here
	rcall	GET_CON2
	cpi	reg16,	240
	brlo	CHARGELOOP

	cp	count,	peakoff

	brsh	SWITCHON
	brlo	SWITCHOFF
	
SWITCHON:
	sbi	PORTB,	PORTB1
	RET

SWITCHOFF:
	cbi	PORTB,	PORTB1
	RET

CALABRATE:
	in	reg16,	PINB
	bst	reg16,	PINB2
	brts	CALA2
	ret

CALA2:
	ldi	peakoff,	0

CALA3:
	in	reg16,	PINB
	bst	reg16,	PINB2
	brts	CALA4
	ret

CALA4:
	;beging charging
	ldi	count,	0
	rcall	DISCHARGE

CAL_LOOP:

	inc	count
	sbi	PORTB,	PORTB0
	rcall	START_CON2
	rcall	GET_CON2
	cpi	reg16,	240
	brlo	CAL_LOOP

	cp	count,	peakoff
	brlo	CALA3
	mov	peakoff,	count
	rjmp	CALA3

	
	