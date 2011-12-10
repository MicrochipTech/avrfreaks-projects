; *********************************************************************************************************************************
; programming	: pvar a.k.a. spir@l evolut10n
; started		: 17-09-2011
; completed		: 05-10-2011
;
; A cylon scanner on 12 LEDs (charliplexed) and a cylon sound imitation with PWM wave generation (on PB4)
; *********************************************************************************************************************************


; **************************************************
; * fundamental assembler directives
; **************************************************

; constants -----------------------------------------------------------------------------------------------------------------------

.include "tn25def.inc"

.equ scan_rate = 160					; directly proportional to scan rate: higher values lead to higher scan rates
.equ refresh_stp1 = 5					; reverse proportional to refresh rate (of pixels and sound volume): higher values lead to lower refresh rates
.equ refresh_stp2 = 2					; reverse proportional to refresh rate (of sound volume): higher values lead to lower refresh rates

.equ pwm_max_level = 255

.equ noled_portb = 0b00100000
.equ noled_ddrb =  0b00011111
.equ portb_mask =  0b00110000

.equ led01_portb = 0b00101000			; these values configure PortB pins (high or low) for LED control
.equ led02_portb = 0b00100100
.equ led03_portb = 0b00101000
.equ led04_portb = 0b00100010
.equ led05_portb = 0b00101000
.equ led06_portb = 0b00100001
.equ led07_portb = 0b00100010
.equ led08_portb = 0b00100001
.equ led09_portb = 0b00100100
.equ led10_portb = 0b00100001
.equ led11_portb = 0b00100100
.equ led12_portb = 0b00100010

.equ led01_ddrb =  0b00011100			; these values configure PortB pins (output or input) for LED control
.equ led02_ddrb =  0b00011100
.equ led03_ddrb =  0b00011010
.equ led04_ddrb =  0b00011010
.equ led05_ddrb =  0b00011001
.equ led06_ddrb =  0b00011001
.equ led07_ddrb =  0b00010011
.equ led08_ddrb =  0b00010011
.equ led09_ddrb =  0b00010101
.equ led10_ddrb =  0b00010101
.equ led11_ddrb =  0b00010110
.equ led12_ddrb =  0b00010110

; variables -----------------------------------------------------------------------------------------------------------------------

.def tmp = r16							; abritary values

.def roll_step = r17					; 6 less significant bits indicate rolling step / 2 most significant bits indicate direction (0000: left to right / 1000 right to left)
.def scan_counter = r18					; counts the times the LED row is scanned
.def led_ptr_val = r19					; pointer to led value in table (copy of ledxx_value)
.def lock = r20							; locking mechanism for loop reitteration

.def sample_ptr_l = r21					; wave table pointer for next sample
.def sample_ptr_h = r22					; wave table pointer for next sample
.def sample_part = r23					; used for substraction (volume attenuation)

.def vol_att = r24						; volume attenuation
.def vol_ptr = r25						; pointer to next sound level
.def vol_att_cnt = r14					; used in substraction loop (volume attenuation)

.def led01_value = r1					; points to intensity for each led (in gamma-values table)
.def led02_value = r2
.def led03_value = r3
.def led04_value = r4
.def led05_value = r5
.def led06_value = r6
.def led07_value = r7
.def led08_value = r8
.def led09_value = r9
.def led10_value = r10
.def led11_value = r11
.def led12_value = r12


; **************************************************
; * code segment initialization
; **************************************************

.cseg
.org 0

	rjmp mcu_init						; Power-on, Brown-out and Watchdog Reset
	reti								; External Interrupt Request 0
	reti								; Pin Change Interrupt Request 0:n
	reti								; Timer/Counter1 Compare Match A
	rjmp set_pwm_dc						; Timer/Counter1 Overflow
	rjmp scan							; Timer/Counter0 Overflow
	reti								; EEPROM Ready
	reti								; Analog Comparator
	reti								; ADC Conversion Complete
	reti								; Timer/Counter1 Compare Match B
	rjmp pwm_mid						; Timer/Counter0 Compare Match A
	rjmp pwm_end						; Timer/Counter0 Compare Match B
	reti								; Watchdog Time-out
	reti								; USI Start
	reti								; USI Overflow


; **************************************************
; * flash constant table
; **************************************************

gamma_values:
.db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 60, 30, 16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

wave_table:								; samples: 512 - sinewave amplitude: [0..255]
.db 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 254, 254, 253, 253, 253
.db 253, 252, 252, 252, 251, 251, 250, 250, 250, 249, 249, 248, 248, 247, 246, 246
.db 245, 245, 244, 243, 243, 242, 241, 241, 240, 239, 238, 238, 237, 236, 235, 234
.db 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, 223, 222, 221, 220, 219
.db 218, 217, 215, 214, 213, 212, 211, 210, 208, 207, 206, 205, 203, 202, 201, 200
.db 198, 197, 196, 194, 193, 192, 190, 189, 188, 186, 185, 183, 182, 181, 179, 178
.db 176, 175, 173, 172, 170, 169, 167, 166, 165, 163, 162, 160, 158, 157, 155, 154
.db 152, 151, 149, 148, 146, 145, 143, 142, 140, 138, 137, 135, 134, 132, 131, 129
.db 128, 126, 124, 123, 121, 120, 118, 117, 115, 113, 112, 110, 109, 107, 106, 104
.db 103, 101, 100, 98, 97, 95, 93, 92, 90, 89, 88, 86, 85, 83, 82, 80
.db 79, 77, 76, 74, 73, 72, 70, 69, 67, 66, 65, 63, 62, 61, 59, 58
.db 57, 55, 54, 53, 52, 50, 49, 48, 47, 45, 44, 43, 42, 41, 40, 38
.db 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22
.db 21, 21, 20, 19, 18, 17, 17, 16, 15, 14, 14, 13, 12, 12, 11, 10
.db 10, 9, 9, 8, 7, 7, 6, 6, 5, 5, 5, 4, 4, 3, 3, 3
.db 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0
.db 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2
.db 2, 3, 3, 3, 4, 4, 5, 5, 5, 6, 6, 7, 7, 8, 9, 9
.db 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 20, 21
.db 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36
.db 37, 38, 40, 41, 42, 43, 44, 45, 47, 48, 49, 50, 52, 53, 54, 55
.db 57, 58, 59, 61, 62, 63, 65, 66, 67, 69, 70, 72, 73, 74, 76, 77
.db 79, 80, 82, 83, 85, 86, 88, 89, 90, 92, 93, 95, 97, 98, 100, 101
.db 103, 104, 106, 107, 109, 110, 112, 113, 115, 117, 118, 120, 121, 123, 124, 126
.db 127, 129, 131, 132, 134, 135, 137, 138, 140, 142, 143, 145, 146, 148, 149, 151
.db 152, 154, 155, 157, 158, 160, 162, 163, 165, 166, 167, 169, 170, 172, 173, 175
.db 176, 178, 179, 181, 182, 183, 185, 186, 188, 189, 190, 192, 193, 194, 196, 197
.db 198, 200, 201, 202, 203, 205, 206, 207, 208, 210, 211, 212, 213, 214, 215, 217
.db 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233
.db 234, 234, 235, 236, 237, 238, 238, 239, 240, 241, 241, 242, 243, 243, 244, 245
.db 245, 246, 246, 247, 248, 248, 249, 249, 250, 250, 250, 251, 251, 252, 252, 252
.db 253, 253, 253, 253, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255

vol_table:
.db 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 3, 4, 5, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8

; **************************************************
; * microcontroller initialization
; **************************************************

mcu_init:
	ldi tmp, $df						; Stack Pointer Setup
	out SPL, tmp						; Stack Pointer Low Byte

	ldi tmp, noled_ddrb					;
	out DDRB, tmp						;
	ldi tmp, noled_portb				;
	out PORTB, tmp						;

	cbi ADCSRA, ADEN					; turn off analog to digital converter

	sbi ACSR, ACD						; turn off analog comparator
	cbi ACSR, ACBG						; disconnect analog comparator from internal voltage reference

	in tmp, WDTCR						;
	andi tmp, 0b10110111				; clear WDTIE and WDE
	out WDTCR, tmp						; stop Watchdog Timer

	ldi tmp, 0b00000001					; set PRADC
	out PRR, tmp						; shutdown analog to digital converter

	ldi tmp, (1<<SE)					; set only SE
	out MCUCR, tmp						; enable sleep instruction and select "idle" sleep mode

	ldi tmp, 0xff^(1<<PLLE)				; disable PLL
	out PLLCSR, tmp						;

; timer/counter0 initial setup...

	in tmp, GTCCR						;
	ori tmp, (1<<TSM)|(1<<PSR0)			; hold timer/counter0 in reset mode (for configuration)
	out GTCCR, tmp						;

	clr tmp								; OC0A/OC0B disconnected from port, normal mode of operation
	out TCCR0A, tmp						;
	ldi tmp, 0b00000100					; normal mode of operation, prescaller set to 1/256
	out TCCR0B, tmp						;

	in tmp, TIMSK						;
	ori tmp, (1<<TOIE0)					; enable overflow interrupt for timer/counter0 (set TOIE0)
	out TIMSK, tmp						;

; timer/counter1 initial setup...

	ldi tmp, 0b00000010					; disable PWM with compare match A / prescaller set to 2
	out TCCR1, tmp						;

	ldi tmp, pwm_max_level				;
	out OCR1B, tmp						; set initial dc for pwm (an arbitary sample)
	out OCR1C, tmp						; set counter top value (PWM resolution is set to 7 bits or 128 levels)

	in tmp, GTCCR						;
	ori tmp, (1<<PWM1B)|(1<<COM1B1) 	; enable PWM with compare match B / connect OC1B / disconnect ^OC1B
	out GTCCR, tmp						;

	in tmp, TIMSK						;
	ori tmp, (1<<TOIE1)					; enable overflow interrupt for timer/counter1 (set TOIE1)
	out TIMSK, tmp						;

	clr tmp								; clear timer/counter1
	out TCNT1, tmp						;



	sei									; enable all interrupts!


; **************************************************
; Program init and main function
; **************************************************
main_init:
	clr sample_ptr_l					;
	clr sample_ptr_h					; load initial values...
	ldi vol_att, 8						;
	clr vol_ptr							; 

	clr roll_step						; first step / left to right direction 
	clr scan_counter					; scan_counter is cleared (no scan has been completed yet)

	ldi tmp, 13
	mov led01_value, tmp
	dec tmp
	mov led02_value, tmp
	dec tmp
	mov led03_value, tmp
	dec tmp
	mov led04_value, tmp
	dec tmp
	mov led05_value, tmp
	dec tmp
	mov led06_value, tmp
	dec tmp
	mov led07_value, tmp
	dec tmp
	mov led08_value, tmp
	dec tmp
	mov led09_value, tmp
	dec tmp
	mov led10_value, tmp
	dec tmp
	mov led11_value, tmp
	dec tmp
	mov led12_value, tmp

	ldi tmp, scan_rate					; load initial value to timer/counter0
	out TCNT0, tmp						;

	in tmp, GTCCR						; release timer/coutner0
	andi tmp, 0xff^((1<<TSM)|(1<<PSR0))	;
	out GTCCR, tmp						;

main_loop:
	ser lock							; lock and wait for a full row scan to be completed...

	cpi scan_counter, refresh_stp2		; check if time to change sound volume
	brne not_yet1						; if not, proceed to next check...
	rcall swing							; if yes, change sound volume
not_yet1:
	cpi scan_counter, refresh_stp1		; check if time to roll pixel values and change sound volume
	brne not_yet2						; if not, go to sleep...
	rcall swing							; swing sound volume
	rcall roll							; roll leds
	clr scan_counter					; if yes, clear scan_counter, roll pixel values and change sound volume
not_yet2:
	sleep
	tst lock							; check if a completed row scan has occured
	brne not_yet2						; if not, stay in loop...
	rjmp main_loop


; **************************************************
; scan the whole row of LEDs
; **************************************************
scan:
	push tmp
	in tmp, SREG
	push tmp
	push ZL
	push ZH

	inc scan_counter

	in tmp, GTCCR						;
	ori tmp, (1<<TSM)|(1<<PSR0)			; hold timer/counter0 in reset mode (for configuration)
	out GTCCR, tmp						;

	ldi tmp, 0b00000011					; normal mode of operation, prescaller set to 1/64
	out TCCR0B, tmp						;

	in tmp, TIMSK						;
	andi tmp, 0xff^(1<<TOIE0)			; disable overflow interrupt for timer/counter0 (clear TOIE0)
	ori tmp, (1<<OCIE0A)|(1<<OCIE0B)	; enable compare match A/B interrupts for timer/counte0 (set OCIE0A and OCIE0B)
	out TIMSK, tmp						;

	ldi tmp, 101						; load maximum pixel value (top counter value)
	out OCR0B, tmp						;

	sei									; enable all intrrupts (needs to be done because we are already in an ISR)
; ------------------------- led 01 -------------------------
	mov led_ptr_val, led01_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led01_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led01_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led01_portb				;
	out PORTB, tmp						;

led01_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ------------------------- led 02 -------------------------
	mov led_ptr_val, led02_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led02_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led02_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led02_portb				;
	out PORTB, tmp						;

led02_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ------------------------- led 03 -------------------------
	mov led_ptr_val, led03_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led03_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led03_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led03_portb				;
	out PORTB, tmp						;

led03_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ------------------------- led 04 -------------------------
	mov led_ptr_val, led04_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led04_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led04_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led04_portb				;
	out PORTB, tmp						;

led04_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ------------------------- led 05 -------------------------
	mov led_ptr_val, led05_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led05_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led05_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led05_portb				;
	out PORTB, tmp						;

led05_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ------------------------- led 06 -------------------------
	mov led_ptr_val, led06_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led06_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led06_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led06_portb				;
	out PORTB, tmp						;

led06_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ------------------------- led 07 -------------------------
	mov led_ptr_val, led07_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led07_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led07_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led07_portb				;
	out PORTB, tmp						;

led07_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ------------------------- led 08 -------------------------
	mov led_ptr_val, led08_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led08_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led08_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led08_portb				;
	out PORTB, tmp						;

led08_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ------------------------- led 09 -------------------------
	mov led_ptr_val, led09_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led09_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led09_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led09_portb				;
	out PORTB, tmp						;

led09_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ------------------------- led 10 -------------------------
	mov led_ptr_val, led10_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led10_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led10_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led10_portb				;
	out PORTB, tmp						;

led10_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ------------------------- led 11 -------------------------
	mov led_ptr_val, led11_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led11_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led11_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led11_portb				;
	out PORTB, tmp						;

led11_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ------------------------- led 12 -------------------------
	mov led_ptr_val, led12_value		; move specific led's value-pointer to parameter for pwm preparation routine
	rcall prep_pwm_cycle				;

	tst tmp								; if pixel value is zero, don't bother to switch it on...
	breq led12_pwm_cycle				; ...just wait for a complete pwm cycle

	ldi tmp, led12_ddrb					;
	out DDRB, tmp						; switch-on the led
	in tmp, PORTB						;
	andi tmp, portb_mask				;
	ori tmp, led12_portb				;
	out PORTB, tmp						;

led12_pwm_cycle:
	rcall start_pwm_cycle				; start pwm cycle and return when finished
; ----------------------------------------------------------
	in tmp, GTCCR						; release timer/coutner0
	ori tmp, (1<<TSM)|(1<<PSR0)			; hold timer/counter0 in reset mode (for configuration)
	out GTCCR, tmp						;

	ldi tmp, 0b00000100					; normal mode of operation, prescaller set to 1/256
	out TCCR0B, tmp						;

	in tmp, TIMSK						;
	ori tmp, (1<<TOIE0)					; enable overflow interrupt for timer/counter0 (set TOIE0)
	andi tmp, 0xff^((1<<OCIE0A)|(1<<OCIE0B)); disable compare match A/B interrupts for timer/counte0 (clear OCIE0A and OCIE0B)
	out TIMSK, tmp						;

	ldi tmp, scan_rate					; load initial value to timer/counter0
	out TCNT0, tmp						;
	in tmp, GTCCR						; release timer/coutner0
	andi tmp, 0xff^((1<<TSM)|(1<<PSR0))	;
	out GTCCR, tmp						;

	clr lock							; a full row scan has been completed... main loop can proceed...

	pop ZH
	pop ZL
	pop tmp
	out SREG, tmp
	pop tmp
reti


; **************************************************
; **************************************************
prep_pwm_cycle:
	in tmp, GTCCR						;
	ori tmp, (1<<TSM)|(1<<PSR0)			; hold timer/counter0 in reset mode (for configuration)
	out GTCCR, tmp						;

	ldi ZH, high (gamma_values * 2)		; starting address of table
	ldi ZL, low (gamma_values * 2)		; starting address of table

	clr tmp								;
	add ZL, led_ptr_val					; add offset pointer for value of selected led
	adc ZH, tmp							;

	lpm tmp, Z							; put selected led intensity in timer/counter0 first copmare/match interrupt
	out OCR0A, tmp						;
ret


; **************************************************
; **************************************************
start_pwm_cycle:
	clr tmp								; clear timer/counter0 value
	out TCNT0, tmp						;
	in tmp, GTCCR						; release timer/coutner0
	andi tmp, 0xff^((1<<TSM)|(1<<PSR0))	;
	out GTCCR, tmp						;

	ser lock							;
pwm_wait_loop:							; wait until pwm cycle is completed
	sleep								;
	tst lock							;
	brne pwm_wait_loop					;
ret


; **************************************************
; middle of PWM cycle - LEDs will be switched-off
; **************************************************
pwm_mid:
	cbi DDRB, 0							;
	cbi DDRB, 1							;
	cbi DDRB, 2							;
	cbi DDRB, 3							; switch-off all LEDs
	cbi PORTB, 0						;
	cbi PORTB, 1						;
	cbi PORTB, 2						;
	cbi PORTB, 3						;
reti


; **************************************************
; end of PWM cycle - loop lock must be cleared
; **************************************************
pwm_end:
	clr lock
reti


; **************************************************
; pixel value rolling
; **************************************************
roll:
	mov tmp, roll_step					; copy roll_step to tmp and keep only step-counter value
	andi tmp, 0b00111111				;
	tst tmp								; check step-counter value
	brne start_rolling					; if not zero, roll one more step

	sbrc roll_step, 7					; if zero, check direction
	rjmp right_to_left_init				; if high nibble not equal to "1000" roll from right to left

; ---------------------------------------
left_to_right_init:						; led01 is the first to shine, the other will follow...
	ldi tmp, 13
	mov led01_value, tmp
	dec tmp
	mov led02_value, tmp
	dec tmp
	mov led03_value, tmp
	dec tmp
	mov led04_value, tmp
	dec tmp
	mov led05_value, tmp
	dec tmp
	mov led06_value, tmp
	dec tmp
	mov led07_value, tmp
	dec tmp
	mov led08_value, tmp
	dec tmp
	mov led09_value, tmp
	dec tmp
	mov led10_value, tmp
	dec tmp
	mov led11_value, tmp
	dec tmp
	mov led12_value, tmp

	rjmp start_rolling
; ---------------------------------------
right_to_left_init:						; led12 is the first to shine, the other will follow...
	ldi tmp, 13
	mov led12_value, tmp
	dec tmp
	mov led11_value, tmp
	dec tmp
	mov led10_value, tmp
	dec tmp
	mov led09_value, tmp
	dec tmp
	mov led08_value, tmp
	dec tmp
	mov led07_value, tmp
	dec tmp
	mov led06_value, tmp
	dec tmp
	mov led05_value, tmp
	dec tmp
	mov led04_value, tmp
	dec tmp
	mov led03_value, tmp
	dec tmp
	mov led02_value, tmp
	dec tmp
	mov led01_value, tmp
; ---------------------------------------

start_rolling:
	inc led01_value
	inc led02_value
	inc led03_value
	inc led04_value
	inc led05_value
	inc led06_value
	inc led07_value
	inc led08_value
	inc led09_value
	inc led10_value
	inc led11_value
	inc led12_value

	inc roll_step

check_visual_edge:
	mov tmp, roll_step					; copy roll_step to tmp and keep only step-counter value
	andi tmp, 0b00111111				;
	cpi tmp, 12 						; check if dot reached the edge
	brlo check_real_edge

	ldi tmp, 14							;
	sbrc roll_step, 7					;
	mov led01_value, tmp				; update pixel on edge, according to direction
	sbrs roll_step, 7					;
	mov led12_value, tmp				;

check_real_edge:
	mov tmp, roll_step					; copy roll_step to tmp and keep only step-counter value
	andi tmp, 0b00111111				;
	cpi tmp, 18 						; check roll_step for top value
	brne end_rolling					; if not there, go to end of rolling routine

	ldi tmp, 0b10000000					;
	and roll_step, tmp					; reset step counter and toggle direction
	eor roll_step, tmp					;
end_rolling:
ret


; **************************************************
; volume swinging
; **************************************************
swing:
	ldi ZH, high (vol_table * 2)		; starting address of table
	ldi ZL, low (vol_table * 2)			; starting address of table

	add ZL, vol_ptr						;
	brcc get_volume						; add sample pointer offset
	inc ZH								;
get_volume:
	lpm vol_att, Z						; get volume-attenuation

	inc vol_ptr
	cpi vol_ptr, 36
	brlo end_swing
	clr vol_ptr
end_swing:
ret

; **************************************************
; PWM cycle end - update duty cycle with next sample
; **************************************************
set_pwm_dc:
	push ZL
	push ZH
	push tmp
	in tmp, SREG
	push tmp
; ---------------------------------------
	ldi ZH, high (wave_table * 2)		; starting address of table
	ldi ZL, low (wave_table * 2)		; starting address of table

	inc sample_ptr_l					; increase sample pointer offset
	brcc add_offset						;
	inc sample_ptr_h					;
	andi sample_ptr_h, 0b00000001		;

add_offset:
	add ZL, sample_ptr_l				; add offset to Z pointer
	adc ZH,	sample_ptr_l				;

	lpm tmp, Z							; get new sample

	mov sample_part, tmp				; copy sample value
	lsr sample_part						; divide by 2 (original value by 2)
	lsr sample_part						; divide by 2 (original value by 4)
	lsr sample_part						; divide by 2 (original value by 8)
	lsr sample_part						; divide by 2 (original value by 16)

	mov vol_att_cnt, vol_att			;
volume_att:								;
	tst vol_att_cnt						;
	breq stop_att						; substract devided value as many times, as volume_attenuation indicates
	sub tmp, sample_part				;
	brmi crossed_zero					;
	dec vol_att_cnt						;
	rjmp volume_att						;
crossed_zero:							;
	clr tmp								;
stop_att:
	out OCR1B, tmp
; ---------------------------------------
	pop tmp
	out SREG, tmp
	pop tmp
	pop ZH
	pop ZL
reti
