;
; Writes WORD0, WORD1 to A3992
;

#include <avr/io.h>

; Clock pulse
.macro CLOCK
	sbi (PORTA-32),PA4 		; CLOCK HIGH	
	cbi (PORTA-32),PA4 		; CLOCK LOW
.endm
	
.global send_WORD0
send_WORD0:

    	lds r21,SREG
	push r21
	push r1
	push r2
	push r14
	push r15
	push r16
	push r17
	push r18
	push ZL
	push ZH
	
	sbis (PINB-32),PB1		; check ENA is HIGH
	rjmp W0_exit				; return if not
	
    	lds r16, offset				; get index into sine table (0 - 255)
	lds r17, increment		; get increment (1,2,4,8,16,32,64)
	sbis (PINB-32),PB0		; check DIR
	neg r17					; change sign if negative
    	add r16, r17				; add increment
	sts offset, r16				; save index for next time
	
	mov r18, r16
	ldi r17, 64					; add pi/2 for cosine
	add r18, r17
	
;  For table in program memory
;	clr r17
;	ldi ZL, pm_lo8(sine_table)		; address of sine table
;	ldi ZH, pm_hi8(sine_table)
;	lsl ZL						; double because PM
;	rol ZH
;	movw r14, ZL				; save
;	add ZL, r16				; add index
;	adc ZH, r17			
;	lpm r1, Z 					; get bridge 1 current & sign	
	
;	movw ZL, r14	
;	add ZL, r18				; address of cosine
;	adc ZH, r17			
;	lpm r2, Z 					; get bridge 2 current & sign
;
; For table in data space
	clr r17
    	ldi ZL, lo8(DAC)
	ldi ZH, hi8(DAC)
	movw r14, ZL				; save
	add ZL, r16				; add index
	adc ZH, r17			
	ld r1, Z 					; get bridge 1 current & sign	
	
	movw ZL, r14	
	add ZL, r18				; address of cosine
	adc ZH, r17			
	ld r2, Z 					; get bridge 2 current & sign

	
    	cbi (PORTA-32),PA5 		; STROBE LOW 
	sbi (PORTA-32),PA3 		; D18 = 1 (range divider = 4)
	CLOCK
	sbi (PORTA-32),PA3 		; D17 = 1 (REF on REF pin)
	CLOCK
	cbi (PORTA-32),PA3 		; D16 = 0 (mixed decay mode)
	CLOCK
	cbi (PORTA-32),PA3 		; D15 = 0 (mixed decay mode)
	CLOCK
	cbi (PORTA-32),PA3 		; D14 (phase, Bridge 2)	
	sbrc r2,7
	sbi (PORTA-32),PA3 
	CLOCK
	cbi (PORTA-32),PA3 		; D13 (phase, Bridge 1)	
	sbrc r1,7
	sbi (PORTA-32),PA3 
	CLOCK
	cbi (PORTA-32),PA3 		; D12 (Bridge 2 DAC, MSB)
	sbrc r2,5
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D11	
	sbrc r2,4
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D10 
	sbrc r2,3
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D9	
	sbrc r2,2
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D8 
	sbrc r2,1
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D7 (Bridge 2 DAC, LSB)	
	sbrc r2,0
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D6 (Bridge 1 DAC, MSB)
	sbrc r1,5
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D5	
	sbrc r1,4
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D4 
	sbrc r1,3
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D3	
	sbrc r1,2
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D2 
	sbrc r1,1
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D1 (Bridge 1 DAC, LSB)	
	sbrc r1,0
	sbi (PORTA-32),PA3 	
	CLOCK
	cbi (PORTA-32),PA3 		; D0 = 0 (WORD0 identifier)
	CLOCK
	sbi (PORTA-32),PA5 		; STROBE HIGH
	
W0_exit:
	pop ZH
	pop ZL	
	pop r18
	pop r17
	pop r16
	pop r15
	pop r14
	pop r2
	pop r1
	pop r21
	sts SREG,r21
	
ret

.global send_WORD1
send_WORD1:

    	lds r21,SREG
	push r21
	
    	cbi (PORTA-32),PA5 		; STROBE LOW
	sbi (PORTA-32),PA3 		; D18 = 1 
	CLOCK
	cbi (PORTA-32),PA3 		; D17 = 0
	CLOCK
	cbi (PORTA-32),PA3 		; D16 = 0 
	CLOCK
	cbi (PORTA-32),PA3 		; D15 = 0 (synchronous rectifier)
	CLOCK
	cbi (PORTA-32),PA3 		; D14 = 0 (0:0 = active)
	CLOCK	
	cbi (PORTA-32),PA3 		; D13 = 0 (internal 4MHz clock)	
	CLOCK	
	cbi (PORTA-32),PA3 		; D12 = 0
	CLOCK
	cbi (PORTA-32),PA3 		; D11 = 0 (fast decay time)
	CLOCK
	cbi (PORTA-32),PA3 		; D10 = 0 
	CLOCK
	cbi (PORTA-32),PA3 		; D9 = 0	
	CLOCK
	cbi (PORTA-32),PA3 		; D8 = 0 	
	CLOCK
	sbi (PORTA-32),PA3 		; D7 = 1 (fixed off time)	
	CLOCK
	sbi (PORTA-32),PA3 		; D6 = 1
	CLOCK
	sbi (PORTA-32),PA3 		; D5 = 1	
	CLOCK
	sbi (PORTA-32),PA3 		; D4 = 1
	CLOCK
	sbi (PORTA-32),PA3 		; D3 = 1
	CLOCK
	sbi (PORTA-32),PA3 		; D2 = 1 (max blank time)
	CLOCK
	sbi (PORTA-32),PA3 		; D1 = 1	
	CLOCK
	sbi (PORTA-32),PA3 		; D0 = 1 (WORD1 identifier)
	CLOCK
	sbi (PORTA-32),PA5 		; STROBE HIGH
	
	pop r21
	sts SREG,r21
	
ret


;sine_table:			; Full cycle, 256 bytes, maximum = 63, bit 7 used as sign bit

;.byte 0,1,3,4,6,7,9,10,12,14,15,17,18,20,21,23,24,25,27,28,30,31,32,34,35,36,38,39,40,41,42,44
;.byte 45,46,47,48,49,50,51,52,53,54,54,55,56,57,57,58,59,59,60,60,61,61,62,62,62,63,63,63,63,63,63,63
;.byte 63,63,63,63,63,63,63,63,62,62,62,61,61,60,60,59,59,58,57,57,56,55,54,54,53,52,51,50,49,48,47,46
;.byte 45,44,42,41,40,39,38,36,35,34,32,31,30,28,27,25,24,23,21,20,18,17,15,14,12,10,9,7,6,4,3,1
;.byte 0,129,131,132,134,135,137,138,140,142,143,145,146,148,149,151,152,153,155,156,158,159,160,162,163,164,166,167,168,169,170,172
;.byte 173,174,175,176,177,178,179,180,181,182,182,183,184,185,185,186,187,187,188,188,189,189,190,190,190,191,191,191,191,191,191,191
;.byte 191,191,191,191,191,191,191,191,190,190,190,189,189,188,188,187,187,186,185,185,184,183,182,182,181,180,179,178,177,176,175,174
;.byte 173,172,170,169,168,167,166,164,163,162,160,159,158,156,155,153,152,151,149,148,146,145,143,142,140,138,137,135,134,132,131,129

;sine_table_1:		; sin(phi) + 0.05*sin(4*phi)
;.byte 0,1,3,5,7,9,10,12,14,16,17,19,21,22,24,25,27,28,29,31,32,33,34,35,36,37,38,39,40,41,42,43
;.byte 44,44,45,46,47,47,48,49,49,50,50,51,52,52,53,53,54,55,55,56,56,57,57,58,59,59,60,60,60,61,61,62
;.byte 62,62,62,63,63,63,63,63,63,63,63,62,62,62,61,61,60,60,59,58,57,57,56,55,54,53,51,50,49,48,46,45
;.byte 44,42,41,39,38,36,35,33,32,30,29,28,26,25,23,22,20,19,17,16,15,13,12,11,9,8,7,6,4,3,2,1
;.byte 0,129,130,131,132,134,135,136,137,139,140,141,143,144,145,147,148,150,151,153,154,156,157,158,160,161,163,164,166,167,169,170
;.byte 172,173,174,176,177,178,179,181,182,183,184,185,185,186,187,188,188,189,189,190,190,190,191,191,191,191,191,191,191,191,190,190
;.byte 190,190,189,189,188,188,188,187,187,186,185,185,184,184,183,183,182,181,181,180,180,179,178,178,177,177,176,175,175,174,173,172
;.byte 172,171,170,169,168,167,166,165,164,163,162,161,160,159,157,156,155,153,152,150,149,147,145,144,142,140,138,137,135,133,131,129

.end
