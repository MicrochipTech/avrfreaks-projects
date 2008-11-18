START_HD_ADC_0_BY_VCC:
push temp
ldi temp,0b10001101		;Energopoihsh ADC
out ADCSR, temp
ldi temp,0b01100000
out ADMUX,temp
in temp,MCUCR
andi temp,0b00001111
ori temp,0b01000000
out MCUCR,temp
sleep
in temp,MCUCR
andi temp,0b00001111
out MCUCR,temp
pop temp
ret

START_HD_ADC_1_BY_VCC:
push temp
ldi temp,0b10001101		;Energopoihsh ADC
out ADCSR, temp
ldi temp,0b01100001
out ADMUX,temp
in temp,MCUCR
andi temp,0b00001111
ori temp,0b01000000
out MCUCR,temp
sleep
in temp,MCUCR
andi temp,0b00001111
out MCUCR,temp
pop temp
ret

START_FAST_ADC_1_BY_VCC:
push temp
ldi temp,0b01100001
out ADMUX,temp
ldi temp,0b11000101		;Energopoihsh ADC
out ADCSR, temp
wait_for_adc_complete:
sbis ADCSR,ADIF
rjmp wait_for_adc_complete
sbi ADCSR, ADIF
pop temp
ret

ADC_COMPLETE:
;Apla epistrephei apo sleep mode
reti
