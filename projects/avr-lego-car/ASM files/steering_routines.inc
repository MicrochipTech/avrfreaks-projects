TURN:
push temp
ldi turn_time_limit,40	;Epitrepomenos xronos peratwsis se 40*0.065sec
call START_TIME_LIMITER
call START_HD_ADC_0_BY_VCC
in temp,ADCH		;Prepei na paw deksia h aristera?
cp temp,steering
brsh trn_left


TRN_RIGHT:
sbi PORTB, 0x03		;Ksekina to moter
startconvr:
call START_HD_ADC_0_BY_VCC
in temp,ADCH		;Syggrisi trehousas thesis
cp temp,steering	;me tin epithimith
brsh pc +2
rjmp startconvr
cbi PORTB,0x03		;Stamata moter strepsis
call STOP_TIME_LIMITER	;Apenergopoihsh prostasias
pop temp
ret

TRN_LEFT:
sbi PORTB, 0x04		;Ksekina to moter
startconvl:
call START_HD_ADC_0_BY_VCC
in temp,ADCH		;Syggrisi trehousas thesis me
cp temp,steering	;tin epithimith
brlo pc +2
rjmp startconvl
cbi PORTB,0x04		;Stamata moter strepsis
call STOP_TIME_LIMITER	;Apenergopoihsh prostasias
pop temp
ret
