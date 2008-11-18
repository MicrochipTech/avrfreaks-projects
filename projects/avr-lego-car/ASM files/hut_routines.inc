MOVE_HUT:
push temp
push wait_x_10ms

ldi turn_time_limit,60	;Epitrepomenos xronos peratwsis se 60*0.065sec
call START_TIME_LIMITER

cp curent_angle,angle	;Prepei na strafei CW h ACW?
brlo move_clockwise
mov temp,angle
inc temp
cp curent_angle,temp
brsh move_anticlockwise
call STOP_TIME_LIMITER	;Apenergopoihsh prostasias
pop wait_x_10ms
pop temp
ret

MOVE_ANTICLOCKWISE:
;Entoli diorthwsis offset
move_anticlickwise_aux:
in temp,PORTB			;Ksekina to moter
ori temp,anticlock_ws
out PORTB, temp
wait_for_pulse_hut_acw:	;Perimenw enan palmo
sbis PIND,0x03
rjmp wait_for_pulse_hut_acw
wait_for_pulse_hut_release_acw:
sbic PIND,0x03
rjmp wait_for_pulse_hut_release_acw
in temp, PORTB			;Stamataei to moter
andi temp, 0b10011111
out PORTB,temp
ldi wait_x_10ms, 2		;Perimenw 20ms gia elatwsh ths taxytitas
call WAIT
call HUT_ROUTINE		;Ektelounte oi diergasies poy epithimw
dec curent_angle
mov temp,angle
inc temp
cp curent_angle,temp
brsh move_anticlickwise_aux
call STOP_TIME_LIMITER	;Apenergopoihsh prostasias
pop wait_x_10ms
pop temp
ret

MOVE_CLOCKWISE:

move_clockwise_aux:
in temp,PORTB			;Ksekina to moter
ori temp,clock_ws
out PORTB, temp
wait_for_pulse_hut_cw:	;Perimenw enan palmo
sbis PIND,0x03
rjmp wait_for_pulse_hut_cw
wait_for_pulse_hut_release_cw:
sbic PIND,0x03
rjmp wait_for_pulse_hut_release_cw
in temp, PORTB			;Stamataei to moter
andi temp, 0b10011111 
out PORTB,temp
ldi wait_x_10ms, 2		;Perimenw 20ms gia elatwsh ths taxytitas
call WAIT		
call HUT_ROUTINE		;Ektelounte oi diergasies poy epithimw
inc curent_angle
cp curent_angle,angle
brlo move_clockwise_aux
call STOP_TIME_LIMITER	;Apenergopoihsh prostasias
pop wait_x_10ms
pop temp
ret

INIT_HUT:				;Stoixisi toy kapelou deksia
sbis pinb,0x07
rjmp init_achived
init_hut_loop:
ldi curent_angle,25
ldi angle,26
call move_hut
sbic pinb,0x07
rjmp init_hut_loop
init_achived:
ldi curent_angle,48
ret

HUT_ROUTINE:
call START_HD_ADC_1_BY_VCC
mov usart_byte,curent_angle
call TRANSMITE_BYTE
in usart_byte,ADCH
call TRANSMITE_BYTE

cp usart_byte,max_light
brlo not_max
mov max_light,usart_byte
mov max_angle,curent_angle
not_max:
ret
