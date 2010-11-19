
.dseg

rtc:		.byte	3	; RTC from t0 interrupt. h:m:s secs first in memory
time1:		.byte	3	; somewhere to save time when read
time2:		.byte	3	; somewhere to save time when read
time3:		.byte	3	; somewhere to save time when read

Tpointer1:	.byte	2	; memory pointer for where to store time
Tpointer2:	.byte	2	; memory pointer for where to store time
Tpointer3:	.byte	2	; memory pointer for where to store time
.cseg



; *****************************************************************************
; DESCR: adds 1 second to the RTC
; ENTRY: called from T0 interrupt once every second
; EXIT: RTC updated
; CHANGES: time
; USES: -
; *****************************************************************************
doRTC:	lds	temp1,rtc	; get seconds
	inc	temp1
	cpi	temp1,60
	brne	sve1
	clr	temp1
	sts	rtc,temp1
	rjmp	sve2

sve1:	sts	rtc,temp1
	ret

sve2:	lds	temp1,rtc+1	; get min
	inc	temp1
	cpi	temp1,60
	brne	sve3
	clr	temp1
	sts	rtc+1,temp1
	rjmp	sve4

sve3:	sts	rtc+1,temp1
	ret

sve4:	lds	temp1,rtc+2	; get hours
	inc	temp1
	cpi	temp1,24
	brne	sve5
	clr	temp1

sve5:	sts	rtc+2,temp1
	ret



; *****************************************************************************
; DESCR: adds 2 times. Wraps round in 24hr format. 23:30:00+00:31:00=00:01:00
; ENTRY: time1 and time2 contain times. HMS, secs 1st in increasing memory
; EXIT: time2=timr1+time2
; CHANGES: time2
; USES: 
; *****************************************************************************
add_time:
	push	temp1
	push	temp2
	push	xl
	push	xh

	ldX	ram,time2		; point at time2
	storeX	Tpointer2		; save pointer

	lds	temp1,time1		; get time1 seconds
	rcall	add_seconds

	loadX	Tpointer2
	adiw	xh:xl,1			; now point at min
	lds	temp1,time1+1		; get other min
	rcall	add_min

	loadX	Tpointer2
	adiw	xh:xl,2			; now point at hours
	lds	temp1,time1+2		; get other hours
	rcall	add_hour

	pop	xh
	pop	xl
	pop	temp2
	pop	temp1
	ret

add_seconds:
	ld	temp2,x			; x points at 
	add	temp2,temp1
	cpi	temp2,60
	brsh	adjustsec
; no rollover so just save and return
	st	x,temp2			; write back new seconds total
	ret

; seconds roll-over so do adjustment
adjustsec:
	subi	temp2,60		; adjust
	st	x+,temp2		; save and point to min
	ldi	temp1,1			; add a min
; 1 in temp1 due to rollover in sec
add_min:
	ld	temp2,x			; get min
	add	temp2,temp1
	cpi	temp2,60		; check for rollover of min
	brsh	adjustmin		; if yes, jmp
	st	x,temp2			; no, so just save and exit
	ret
adjustmin:
	subi	temp2,60		; adjust min
	st	x+,temp2		; save back and point to hours
	ldi	temp1,1			; add an hour
add_hour:
	ld	temp2,x			; get min
	add	temp2,temp1
	cpi	temp2,24		; check for rollover of hour
	brsh	adjusthr		; if yes, jmp
	st	x,temp2			; no, so just save and exit
	ret
adjusthr:
	subi	temp2,24		; adjust hour
	st	x,temp2			; save back
	ret







; *****************************************************************************
; DESCR: copies current time to ram @Tpointer1
; ENTRY:  Tpointer loaded with store location
; EXIT: time in ram
; CHANGES: ram
; USES: -
; Interrupts are NOT disabled, instead a 2nd read of seconds is performed
; If they match then time has not rolled-over during read. If changed
; another read is performed. This read will be correct.
; Interrupts are kept running because if this is used to wait for a time, the
; interrupts will be constantly off and on which will affect the clock accuracy
; *****************************************************************************
gettime:
	push	xl
	push	xh
	push	temp1
	push	temp2
	push	loopcounter

stor1:	loadX	Tpointer1	; get pointer to save location
	lds	temp1,RTC+2	; hours
	lds	temp2,RTC+1	; min
	lds	loopcounter,RTC	; sec

	st	x+,loopcounter	; sec 1st
	st	x+,temp2	; then min
	st	x,temp1		; then hours

	lds	temp1,RTC	; get seconds again
	cp	temp1,loopcounter ; have seconds changed?
	brne	stor1		; if changed (just rolled-over) then get again 
				; they will definitely match next read
	pop	loopcounter
	pop	temp2
	pop	temp1
	pop	xh
	pop	xl
	ret	



; *****************************************************************************
; DESCR: reads and then sends the time (in ascii) to the o/p stream
; ENTRY: -
; EXIT: time in ram and sent
; CHANGES: X, ram
; USES: gettime, decout
; *****************************************************************************
showtime:
	ldX	ram,time1	; get address to save to (pointer)
	storeX	Tpointer1	; save address at pointer location

	rcall	gettime		; read the time

tim:	adiw	xh:xl,3		; point at hours+1 (for PRE-decrement)

	ld	temp1,-x	; get hours
	call	decout
	send	':'
	ld	temp1,-x	; get min
	call	decout
	send	':'
	ld	temp1,-x	; get sec
	call	decout
	ret







