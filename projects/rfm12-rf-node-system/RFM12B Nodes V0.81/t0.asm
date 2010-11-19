
; timer-0 setup and int routine for general timebase requirements
; user counts of 1ms, 100ms, and 1sec. 

; these figures are approximate
.equ	XX = 31		; for 1ms @8Mhz 
;.equ	XX = 62		; for 1ms @16Mhz 
;.equ	XX = 78		; for 1ms @20Mhz




.cseg


; timer free runs at full count (256) so still useful to genarate PWM if req
init_t0:
	clr	temp1
	sts	RTC,temp1
	sts	RTC+1,temp1
	sts	RTC+2,temp1

	ldi	temp1,XX
	mov	ticks,temp1

	ldi	temp1,100
	mov	tocks,temp1
	ldi	temp1,10
	mov	tacks,temp1

	ldi 	temp1,0b00000000
	store	TCCR0A,temp1		; 
	ldi 	temp1,0b00000001 	; clk at full xtal rate
	store	TCCR0B,temp1		; 
	ldi	temp1,(1<<TOIE0)	; timer overflow interrupt enable 0
	store	TIMSK0,temp1
	ret





; timer 0 overflow int.
TIM0_OVF:
	push	temp1		; save reg and status
	load	temp1,SREG
	push	temp1
	push	temp2

	dec	ticks		; z-flag valid
	brbc	1,t0exit	; if NZ, exit
	ldi	temp1,XX	; get start value
	mov	ticks,temp1	; reload to count for next pass

; every 1ms, get here
F0:	tst	smalltime	; check if count=0
	breq	F1		; if z, skip next instruc
	dec	smalltime	; not Z so can dec some more

F1:	tst	timeout
	brne	Fdc
	sbr	bitflags,timeoutFlg
	rjmp	F10
Fdc:	dec	timeout	
		
F10:	dec	tocks		; this is 100ms counter
	brbc	1,t0exit
	ldi	temp1,100
	mov	tocks,temp1


;we get here every 100ms
	tst	bigtime		; check 100ms user count (centisec)
	brbs	1,F22
	dec	bigtime

F22:	dec	tacks		; now do 10ths of a sec
	brbc	1,t0exit	; if NZ, exit (untill=0 which is 1-secs worth)
	ldi	temp1,10	; 1 secs worth
	mov	tacks,temp1

	sbr	bitflags,newsec	; flag a new second A mask, not a bit-number

	call	doRTC		; update rtc. See 'timefunc.asm'

; now do user seconds count
	tst	seconds
	brbs	1,t0exit
	dec	seconds


t0exit:	pop	temp2
	pop	temp1
	store	SREG,temp1
	pop	temp1
	reti







; delay 1ms steps
delay1:	mov	smalltime,temp1
wait1:	tst	smalltime
	brbc	1,wait1
	ret

;delay 100ms steps
delay2:
	mov	bigtime,temp1
wait2:	tst	bigtime
	brbc	1,wait2
	ret

;delay 1-sec steps
delay3:	mov	seconds,temp1
wait3:	tst	seconds
	brbc	1,wait3
	ret







