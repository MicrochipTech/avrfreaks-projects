TIMER0_OVERFLOW:
push temp			;Anoigma diakopti
in temp,PORTB
or temp,direction
out PORTB,temp
pop temp
reti

TIMER0_COMPARE_MATCH:
cbi PORTB,0x00		;Kleisimo diakopti
cbi PORTB,0x01
reti

START_MOVING:
push temp
ldi temp,0b00000010	;Energopoiw ta interrupts
out TCCR0, temp		;tou metriti wste na 
in temp,TIMSK		;kanw PWM
ori temp,0b00000011
out TIMSK,temp
mov temp,speed
dec temp
out OCR0, temp
pop temp
ret

STOP_MOVING:
push temp
ldi temp,0b00000000	;Apenergopoiw ta interrupts
out TCCR0, temp		;tou metriti wste na 
in temp,TIMSK		;stamatisw to PWM
andi temp,0b11111100
out TIMSK,temp
in temp,PORTB		;Katharizw ta shmata kinishs
andi temp,0b11111100
out PORTB,temp
pop temp
ret

MOVE_DISTANCE:
cpi distance,0			;Tha ksekinisei mono an <>0
brne pc+2
ret
push temp
push wait_x_10ms

call START_MOVING

mov temp,distance

wait_for_wheel_pulse:	;Metraw palmous
sbis PIND,0x02
rjmp wait_for_wheel_pulse

ldi wait_x_10ms,11		;Kathisterisi gia paush
call wait				;anaphdhsewn

wait_for_end_of_wheel_pulse:
sbic PIND,0x02
rjmp wait_for_end_of_wheel_pulse

dec temp				;Kai molis teleiwsw
cpi temp,0x01
brsh wait_for_wheel_pulse

call STOP_MOVING

pop wait_x_10ms
pop temp
ret

