TIMER2_OVERFLOW:
dec turn_time_limit
cpi turn_time_limit,0x00
breq MECHANICAL_PROBLEM
reti

MECHANICAL_PROBLEM:
clr temp
out TCCR0,temp
out TCCR2,temp
out portb,temp
out adcsr,temp
wait_for_reset:
rjmp wait_for_reset
reti

WAIT:
push temp
mov dly,wait_x_10ms
next_period:
in temp,TCNT2
subi temp,-39
out OCR2, temp
ldi temp,0b10000000	;Katharismos shmaias
out TIFR,temp
loopwait:
in temp,TIFR		;Anamonh gia COMPARE MATCH
sbrs temp,0x07
rjmp loopwait
dec dly
cpi dly,0
breq pc + 2
rjmp next_period
pop temp
ret

START_TIME_LIMITER:
push temp
in temp,TIMSK		;Energopoihsh interupts TOV2
ori temp,0b01000000	;tou TIMER2
out TIMSK,temp
pop temp
ret

STOP_TIME_LIMITER:
push temp
in temp,TIMSK		;Apenergopoihsh twn interupts TOV2
andi temp,0b10111111
out TIMSK,temp
pop temp
ret
