
.include "tn25def.inc"

;Define input/outputs:
ldi r17,(1<<DDB0)|(1<<DDB4) ;Set PB0, PB4 as output.
out DDRB,r17

startup: ;Startup routine for low input voltage.

ldi r17,(0<<PORTB0)|(1<<PORTB4) ;Close Q1 and Q2.
out PORTB,r17

;Make sure the PLL is disabled.
ldi r16,(0<<PLLE)|(0<<PCKE)
out PLLCSR,r16

;Initialize PWM A output:
ldi r16,(0<<PWM1B)
ldi r17,(1<<PWM1A)|(1<<COM1A0)|(1<<CS10)

;When given low input voltage, the internal oscillator on the tiny25
;will oscillate really slow. We therefore start with the fastest possible
;PWM frequency.
ldi r18,0x3 ;Count from 0 to 3. 
ldi r20,0x2 ;When at 2, toggle boost PWM output.

out OCR1A,r20
out OCR1B,r21
out OCR1C,r18
out GTCCR,r16
out TCCR1,r17

;Wait a little while to charge the capacitor:
ldi r16,0xff
vstab_loop1p:
	ldi r17,0xff
	vstab_loop2p:
		dec r17
		brne vstab_loop2p
	dec r16
	brne vstab_loop1p

;We now enter normal operation:

;Start up PLL:
ldi r16,(1<<LSM)|(1<<PLLE) ;Low-speed mode (32MHz)
out PLLCSR,r16
ldi r16,0x86
a:
	dec r16 ;Wait 100us for PLL to stabilize.
	brne a
b:
	in r16,PLLCSR ;Wait for PLL to lock.
	sbrs r16,PLOCK
	rjmp b

ldi r16,(1<<PWM1B)|(1<<COM1B1)
ldi r18,0xff ;8-bit resolution.
ldi r20,0x7f ;Continue with 50% duty-cycle on boost-converter
ldi r21,0xff ;and the buck-transistor closed.

out OCR1A,r20
out OCR1B,r21
out OCR1C,r18
out GTCCR,r16

;Use PLL as clock source.
ldi r16,(1<<LSM)|(1<<PLLE)|(1<<PCKE)
out PLLCSR,r16

;Initialize analog comparator:
sbi ACSR,ACBG

vcheck:
	;Update duty cycles:
	out OCR1A,r20 ;Boost-converter
	out OCR1B,r21 ;Buck-converter

	;Wait for voltage to stabilize:
	ldi r16,0x2
	wait_s1:
		ldi r17,0xff
		wait_s2:
			dec r17
			brne wait_s2
		dec r16
		brne wait_s1

	;Check VDDIN:
	sbis ACSR,ACO
	rjmp boost_dec ;Voltage too high.
	rjmp buck_dec  ;Voltage too low.

; Voltage too low:
buck_dec:
	cpi r21,0x00
	breq boost_inc ;Already at zero buck.
	dec r21 ;Decrease buck duty cycle.
	rjmp vcheck

boost_inc:
	cpi r20,0x20
	;If we're at maximum boost, it is assumed that the oscillator
	;is running to slow again, and we therefore jump to the
	;startup routine. This would probably be the right thing to do
	;in most cases, if not the battery is probably dead and the program
	;will halt soon anyway.
	breq startup
	dec r20 ;Increase boost duty cycle.
	rjmp vcheck

; Voltage too high:
boost_dec:
	cpi r20,0xff
	breq buck_inc ;Already at zero boost.
	inc r20 ;Decrease boost duty cycle.
	rjmp vcheck

buck_inc:
	cpi r21,0xff ;Check if buck-converter is at maximum (no current)
	breq vcheck  ;This should never happen unless the transistor is broken.
	inc r21 ;Increase buck duty cycle.
	rjmp vcheck

