; PWM fan Control with NTC
;
; Written by Roger Storvik (roger.storvik@gmail.com)
; 
; v0.1 - works on my breadboard
;
; Fan connected to +12V and Collector on 2n2219
; Base on 2n2219 connected to PB1 on tiny15 through 3,3K resistor
; Emitter connected to GND
; 
; NTC 10K connected to 5V and PB2
; 5K resistor connected to PB2 and GND

.include "tn15def.inc"
;.include "4433def.inc"

.def temp=r16
.def tempL=r17
.def tempH=r18
.def fanStopped=r19
.def dlyV1=r20
.def dlyV2=r21
.def dlyV3=r22

.cseg

ldi	temp,RAMEND
out	spl,temp

clr fanStopped	; We assume when reset, fan has been stopped
clr	temp
out	tccr1,temp
;out	tccr1a,temp
;out	tccr1b,temp
out	tcnt1,temp
;out	tcnt1h,temp
;out	tcnt1l,temp
;out	ocr1h,temp
;out	ocr1l,temp
out	ocr1a,temp

; Set PWM Output

;ldi	temp,0x81	; Non-Inverting PWM, 8-bit
ldi		temp, (1<<CS13)|(1<<PWM1)|(1<<COM1A1) ; Non, CK/8
out	tccr1,temp
;out	tccr1a,temp
;ldi	temp,0x02	; CK/8
;out	tccr1b,temp
ldi temp,0b00000010	; output PB1
out	ddrb,temp

;ser temp		; full speed
;out	ocr1l,temp
;out	ocr1al,temp

ldi temp, 0x01 ;Select ADC1/PB2
out ADMUX, temp

startconvert:
    ldi temp, (1<<ADSC) | (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0)   ;set ADC prescaler to , 1MHz / 8 = 125kHz
    out ADCSR, temp

convert:
	in	temp, ADCSR
    andi temp, (1<<ADIF)
    breq convert


    in tempL, ADCL  ;IT LOOKS LIKE YOU HAVE TO READ ADCL BEFORE ADCH
    in tempH, ADCH
	
	cpi tempH, 0x02	; Check if we have overtemperature
	brsh fullspeed

	cpi fanStopped, 0x00
	brne notstopped

	cpi tempL, 0x40	; Check if we have lowtemp, from stopped
	brlo shutfan
	rjmp middle
notstopped:
	cpi tempL, 0x36	; Check if we have lowtemp, not stopped
	brlo shutfan

middle:
	cpi tempL, 0x60	; Check if we have measured something in the middle
	brsh regtemp

	ldi temp, 0x60	; Set speed to this if we are between reg & lowtemp
	rjmp checkstop

regtemp:
	mov temp, tempL	; Use ADCL to regulate Fan
	rjmp checkstop

shutfan:
	clr temp		; Stop fan
	clr fanStopped
	rjmp checkstop

fullspeed:
	ser temp		; Full speed

checkstop:
	cpi fanStopped, 0x00; Do we have stop command?
	brne setspeed

	cpi temp, 0x00		; Was the fan already stopped?
	breq stoppit
						; No, the fan was stopped
						; Run it at full speed a short time
	ser temp		; Full speed
	out ocr1a, temp
;	out ocr1l, temp
	ser fanStopped
	rcall long_dly	; Run for a while
	rjmp startconvert

stoppit:
	rcall long_dly
setspeed:
	out ocr1a, temp
;	out ocr1l, temp
	rjmp startconvert

long_dly:
	ldi	dlyV3,0x20

long_dly1:
	rcall dly
	dec	dlyV3
	brne long_dly1
	ret

dly:
	ser	dlyV1

dly1:
	ser	dlyV2

dly2:
	dec	dlyV2
	brne dly2
	dec dlyV1
	brne dly1
	ret