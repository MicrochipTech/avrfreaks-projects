;* DESCRIPTION
;* This application code is dedicated to transmit IR remote control signal to NIKON DSLR  
;* 
;*
;* The timing is adapted for 4,8 MHz internal clock
;*
; Author Peter Falat, www.falcom.sk
; ////////////////////////////////////////////////////////////////////////////////////////////////////////////
; Target MCU: ATtiny13
; Fuses: 
; 
.DEVICE ATtiny13
.INCLUDE "tn13def.inc"

.ORG 0
	rjmp	RESET			; Reset Handler
.ORG 1
	rjmp	EXT_INT0		; IRQ0 Handler
.ORG 2
;	rjmp	PCINTO			; PCIINT0 Handler
.ORG 3
	rjmp	TIM0_OVF		; Timer0 Overflow Handler
.ORG 4
;	rjmp	EE_RDY			; EEPROM Ready Handler
.ORG 5
;	rjmp	ANA_COMP		; Ananlog Comparator Handler
.ORG 6	
;	rjmp	TIM0_COMPA		; Timer0 CopmpareA Handler
.ORG 7
;	rjmp	TIM0_COMPB		; Timer0 CompareB Handler
.ORG 8
;	rjmp	WATCHDOG		; Watchdog Interrupt Hnadler
.ORG 9
;	rjmp	ADCCOMPL		; ADC Conversion Complete


.DSEG						; Data segment

.ESEG						; Eeprom segment

.CSEG						; Code segment

;***************************************************************************

.EQU	OUTPUT = PB0	; Data output to IR LED OC0A
.EQU 	INPUT = PB1 	; Data input from start switch
.EQU	TEST = PB2		; Testing output
.EQU	TYPE = PB3		; LOW = NIKON, HIGH = CANON
.EQU	RG = PB4 		; HIGH = NIKON, LOW = CANON
.DEF 	S = R0 			; Storage for the Status Register
.DEF	temp = R16		; temporaly variable
.DEF	timerL = R17	;
.DEF	timerH = R18	;
.DEF	period = R19	;
.DEF	i = R20			;	
.DEF	camera = R21	; 0 = NIKON, <>0 = CANON
.DEF	canon2s = R22

.MACRO LED_CANON        ; CANON LED ON 
	cbi PORTB, TYPE		; LED for CANON
	sbi PORTB, RG       ; 
.ENDMACRO  

.MACRO LED_NIKON        ; NIKON LED ON 
	sbi PORTB, TYPE		; LED for NIKON
	cbi PORTB, RG       ; 
.ENDMACRO 

.MACRO LED_OFF        	; LED OFF 
	cbi PORTB, TYPE		;
	cbi PORTB, RG       ; 
.ENDMACRO 

TIM0_OVF:
	in S,sreg 			; Store SREG
	out TCNT0, period
	inc timerL
	brne JMPT2 			; Updated every 26,25 us
	inc timerH
JMPT2:
	out sreg,S 			; Restore SREG
	reti

EXT_INT0:
	in S,sreg 			; Store SREG
	ldi temp, 0
	out MCUCR, temp
	out GIMSK, temp
	ldi temp, (1<<TOIE0)	; Enable Timer0 overflow interrupt
	out TIMSK0,temp
	out TCNT0, period
	out sreg,S 			; Restore SREG
	reti
	
RESET:
	ldi temp,low(RAMEND)	;Initialize stackpointer for parts with SW stack
	out SPL,temp
	ldi temp, (1<<COM0A1)|(0<<COM0A0)|(1<<WGM01)|(1<<WGM00)
	out TCCR0A, temp		; Fast PWM
	ldi temp, (1<<CS00)		; Timer/Counter 0 clocked at CLK/1 @4.8MHz
	out TCCR0B,temp
	ldi temp, 180
	out OCR0A, temp
	ldi temp, (1<<TOIE0)	; Enable Timer0 overflow interrupt
	out TIMSK0,temp
	ldi temp, 144			; for NIKON
	cpi camera, 0			; default NIKON
	breq JMPR1
	ldi temp, 123			; for CANON
JMPR1:
	mov period, temp
	out TCNT0, period
	ldi temp, 0b00011100 	; PORTB as --ioooii	
	out DDRB,temp			
	ldi temp, 0b00000010
	out PORTB, temp	
	nop	
	sei 					; Enable global interrupt
	clr i 
	cpi camera, 0
	breq NIKON				; camera=0 - jump to NIKON
	rjmp CANON	 			; camera>0 - jump to CANON 


; *************** NIKON ******************************
NIKON:
;************			; 2000us log.1
	LED_NIKON
	clr timerL
	clr timerH
	sbi DDRB, OUTPUT	; 1 level is transmitted
JMP1:
	cpi timerL, 77
	brne JMP1
;************			; 28ms log.0
	clr timerL
	clr timerH
	cbi	DDRB, OUTPUT
JMP2:
	cpi timerH, 4
	brne JMP2
JMP3:
	cpi timerL, 44
	brne JMP3
;************			; 400us log.1
	clr timerL
	clr timerH
	sbi DDRB, OUTPUT	; 1 level is transmitted
JMP4:
	cpi timerL, 15
	brne JMP4
;************			; 1580us log.0
	clr timerL
	clr timerH
	cbi	DDRB, OUTPUT
JMP5:
	cpi timerL, 60
	brne JMP5
;************			; 400us log.1
	clr timerL
	clr timerH
	sbi DDRB, OUTPUT	; 1 level is transmitted
JMP6:
	cpi timerL, 15
	brne JMP6	
;************			; 3580us log.0
	clr timerL
	clr timerH
	cbi	DDRB, OUTPUT
JMP7:
	cpi timerL, 136
	brne JMP7
;************			; 400us log.1
	clr timerL
	clr timerH
	sbi DDRB, OUTPUT	; 1 level is transmitted
JMP8:
	cpi timerL, 15
	brne JMP8
	inc i
	cpi i, 2
	breq EXIT	
;************			; 63.2ms log.0
	clr timerL
	clr timerH
	cbi	DDRB, OUTPUT
JMP9:
	cpi timerH, 9
	brne JMP9
JMP10:
	cpi timerL, 113
	brne JMP10
	rjmp NIKON

; *************** NIKON ************ END********

; ****************CANON ************************
CANON:
	LED_CANON
	ldi canon2s, 175		; 5.36ms log.0 => 2s delay
	sbrs camera, 1		; camera=1 => no delay, camera=2 => 2s delay
	ldi canon2s, 239		; 7,33ms log.0 => no delay

;************			; 489,3us log.1
	clr timerL
	clr timerH
	sbi DDRB, OUTPUT	; 1 level is transmitted
JMPC1:
	cpi timerL, 16
	brne JMPC1
;************			; 7,33ms log.0
	clr timerL
	clr timerH
	cbi	DDRB, OUTPUT
JMPC3:
	cp timerL, canon2s
	brne JMPC3
;************			; 489,3us log.1
	clr timerL
	clr timerH
	sbi DDRB, OUTPUT	; 1 level is transmitted
JMPC4:
	cpi timerL, 16
	brne JMPC4

; *************** CANON ************ END********

EXIT:
	cbi DDRB, OUTPUT
	clr timerL
	clr timerH
	clr i

JMPE1:
	cpi timerH, 25
	brne JMPE1
	LED_OFF
	cpi camera, 2
	breq JMPE4

JMPE2:
	cpi timerH, 50
	brne JMPE2
	rjmp JMPE5

JMPE4:
	cpi timerH, 50
	brne JMPE4
	LED_CANON
JMPE6:
	cpi timerH, 80
	brne JMPE6
	LED_OFF

JMPE5:
	clr timerL
	clr timerH

Wait4Button:
	cpi timerH, 230
	brsh WAIT
	sbis PINB, INPUT
	rjmp Wait4Button
	rjmp END

WAIT:
	clr timerL
	clr timerH
	cpi camera, 0
	breq CANON1
	sbrs camera, 1
	rjmp CANON1
	clr camera
	LED_NIKON
	rjmp WAIT1

CANON1:
	inc camera
	LED_CANON

WAIT1:
	cpi timerH, 100
	brsh END
	sbis PINB, INPUT
	rjmp WAIT1
	rjmp END

END:
	clr i
	cli
	clr temp
	ldi temp, (1<<SE)|(0<<ISC01)|(0<<ISC00)|(1<<SM1)	;
	out MCUCR, temp
	ldi temp, (1<<INT0)
	out GIMSK, temp
	ldi temp, 0			; Disable Timer0 overflow interrupt
	out TIMSK0,temp
	sei
	LED_OFF
	sleep
