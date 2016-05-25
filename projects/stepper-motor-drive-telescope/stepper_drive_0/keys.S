#include <avr/io.h>

.global READ_RA_keys

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Read buttons for manual movement in RA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

READ_RA_keys:

	in  		r16,(PINB-32)			; read all pins on port B
	andi 	r16,0b00000110		; get  RA keystate 
	sts 		RA_keys, r16			; save as new keystate
	lds  	r16,prev_RA_keys		; get prev RA keystate 
	lds  	r17,RA_keys			; get new RA keystate 
	cp		r16,r17			
	breq	RK9					; quit if no change
	eor  	r16,r17
	sbrc 	r16,1					; skip if PB1 has not changed
	rcall 	RAneg
	sbrc 	r16,2					; skip if PB2 has not changed
	rcall 	RApos
RK9:	
	sts 		prev_RA_keys,r17		;save new keystate as previous
	ret
	
RAneg:
	sbrc 	r17,1			; RA- button just pressed?
	rcall 	RAopened		; no - it was released
	sbrs	r17,1			; RA- button just released?
	rcall	RAclosed		; no - it was pressed
	ret
	
RApos:
	sbrc 	r17,2			; RA+ button just pressed?
	rcall 	RAopened		; no - it was released
	sbrs	r17,2			; RA+ button just released?
	rcall	RAclosed		; no - it was pressed
	ret

RAclosed:
	push	r18
	lds		r18,RA_accel
	tst 		r18
	brne	RA1				; return if already ac/decelerating (button pressed twice?)
	ser		r18
	sts RA_STEP_max, r18	; set max step count to 0000FFFFh
	sts RA_STEP_max+1, r18
	clr 		r18
	sts RA_STEP_max+2, r18
	sts RA_STEP_max+3, r18
	sbrc	r16, 2				; RA+ pressed?
	cbi (PORTB-32),PB4		; set DIR LO, positive RA
	sbrc	r16, 1				; RA- pressed?
	sbi (PORTB-32),PB4		; set DIR HI, negative RA
	rcall 	RA_SLEW			; initiate slew in RA (changes r16)
RA1:
	pop 	r18
	ret
	
RAopened:	
	push	r21
	push	r20
	push	r19
	push	r18	
	push 	r16
	push	r11
	push	r10
	push	r9
	push	r8
	lds 		r18,RA_accel
	tst		r18
	breq 	RA9					; return if not moving
	lds	r18, RA_STEP_count		; get current step count
	lds	r19, RA_STEP_count+1
	lds	r20, RA_STEP_count+2
	lds	r21, RA_STEP_count+3
	clr	r8							; no. of accel steps (16384) 
	ldi r16,64
	mov r9,r16
	clr	r10
	clr	r11
	cp  r8,r18						
	cpc r9,r19
	cpc r10,r20
	cpc r11,r21
	brpl RA2						; jump if current step count < accel_steps
	mov r18,r8
	mov r19,r9
	mov r20,r10
	mov r21,r11
	sts RA_STEP_count, r18		; save accel_steps as current step count
	sts RA_STEP_count+1, r19
	sts RA_STEP_count+2, r20
	sts RA_STEP_count+3, r21
RA2:
	sts RA_STEP_max, r18		; save as max step count
	sts RA_STEP_max+1, r19
	sts RA_STEP_max+2, r20
	sts RA_STEP_max+3, r21
	ser r18
	sts RA_accel,r18				; start decelerating
RA9:
	pop 	r8
	pop	r9
	pop 	r10
	pop	r11
	pop	r16
	pop 	r18
	pop	r19
	pop 	r20
	pop	r21
	ret
	
.global READ_DEC_keys

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Read buttons for manual movement in Dec
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

READ_DEC_keys:

	in  		r16,(PIND-32)			; read all pins on port D
	andi 	r16,0b10000100		; get  DEC keystate 
	sts 		DEC_keys, r16			; save as new keystate
	lds  	r16,prev_DEC_keys	; get prev DEC keystate 
	lds  	r17,DEC_keys			; get new DEC keystate 
	cp		r16,r17			
	breq	DK9					; quit if no change
	eor  	r16,r17
	sbrc 	r16,2					; skip if PD2 has not changed
	rcall 	DECneg
	sbrc 	r16,7					; skip if PD7 has not changed
	rcall 	DECpos
DK9:	
	sts 		prev_DEC_keys,r17	;save new keystate as previous
	ret
	
DECneg:
	sbrc 	r17,2			; DEC- button just pressed?
	rcall 	DECopened	; no - it was released
	sbrs	r17,2			; DEC- button just released?
	rcall	DECclosed		; no - it was pressed
	ret
	
DECpos:
	sbrc 	r17,7			; DEC+ button just pressed?
	rcall 	DECopened	; no - it was released
	sbrs	r17,7			; DEC+ button just released?
	rcall	DECclosed		; no - it was pressed
	ret

DECclosed:
	push	r18
	lds		r18,DEC_accel
	tst 		r18
	brne	DEC1				; return if already ac/decelerating (button pressed twice?)
	ser		r18
	sts DEC_STEP_max, r18	; set max step count to 0000FFFFh
	sts DEC_STEP_max+1, r18
	clr 		r18
	sts DEC_STEP_max+2, r18
	sts DEC_STEP_max+3, r18
	sbrc	r16, 7				; DEC+ pressed?
	cbi (PORTD-32),PB4		; set DIR LO, positive DEC
	sbrc	r16, 2				; DEC- pressed?
	sbi (PORTD-32),PB4		; set DIR HI, negative DEC
	rcall 	DEC_SLEW		; initiate slew in DEC (changes r16)
DEC1:
	pop 	r18
	ret
	
DECopened:	
	push	r21
	push	r20
	push	r19
	push	r18	
	push 	r16
	push	r11
	push	r10
	push	r9
	push	r8
	lds 		r18,DEC_accel
	tst		r18
	breq 	DEC9					; return if not moving
	lds	r18, DEC_STEP_count		; get current step count
	lds	r19, DEC_STEP_count+1
	lds	r20, DEC_STEP_count+2
	lds	r21, DEC_STEP_count+3
	clr	r8							; no. of accel steps (16384) 
	ldi r16,64
	mov r9,r16
	clr	r10
	clr	r11
	cp  r8,r18						
	cpc r9,r19
	cpc r10,r20
	cpc r11,r21
	brpl DEC2						; jump if current step count < accel_steps
	mov r18,r8
	mov r19,r9
	mov r20,r10
	mov r21,r11
	sts DEC_STEP_count, r18		; save accel_steps as current step count
	sts DEC_STEP_count+1, r19
	sts DEC_STEP_count+2, r20
	sts DEC_STEP_count+3, r21
DEC2:
	sts DEC_STEP_max, r18		; save as max step count
	sts DEC_STEP_max+1, r19
	sts DEC_STEP_max+2, r20
	sts DEC_STEP_max+3, r21
	ser r18
	sts DEC_accel,r18				; start decelerating
DEC9:
	pop 	r8
	pop	r9
	pop 	r10
	pop	r11
	pop	r16
	pop 	r18
	pop	r19
	pop 	r20
	pop	r21
	ret
	
