;***********************************************
; Labornetzgerät 2006:               sousroutine
;
; uProzessor gesteuert: Atmega8535
;
; hirschmann vitalis
; 18.04.2006
;***********************************************

.include 	"macro_def.asm"						; propres macros

;-----------------------------------------------
; LCD définitions
;-----------------------------------------------
msg_5V:		.db	"5V, 1A ><", 0
msg_12V:	.db	"12V,1A ><", 0
msg_NC1:	.db	"NC1    ><", 0
msg_adc:			 .db "    ADC", 0
msg_pft:			 .db " Taster", 0
msg_pft1:	.db "Output = '1'    ", 0
msg_pft0:	.db "Output = '0'    ", 0
msg_svo:			 .db "  Servo", 0
msg_cr1:	.db "(c) v.hirschmann", 0
msg_cr2:	.db "    Renens, 2006", 0
msg_sw:		.db "- - - busy - - -", 0
msg_menu:	.db " v Menu", 0
msg_no:				.db "   No v ", 0
msg_ups:	.db "T=20ms,  pos=", 0
msg_adc2:	.db "Result ADC: ", 0
msg_lance:	.db "Grand Concours  " ,0


;-----------------------------------------------
; LCD refresh
;-----------------------------------------------

lcd_startup:
; ligne 1, part 1:
	lds		a0, program1
	
	cpi		a0, 0
	breq	lcdst_pg10

	cpi		a0, 1
	breq	lcdst_pg11

	cpi		a0, 2
	breq	lcdst_pg12

lcdst_pg10:
	TXT_LCD2	msg_5V,   0x00, 0x01
	rjmp	ligne1_part2
lcdst_pg11:
	TXT_LCD2	msg_12V,   0x00, 0x01
	rjmp	ligne1_part2
lcdst_pg12:
	TXT_LCD2	msg_NC1,   0x00, 0x01
	rjmp	ligne1_part2
;...............................................
; ligne 1, part 2:
ligne1_part2:
	lds		a0, program2
	
	cpi		a0, 0
	breq	lcdst_pg20

	cpi		a0, 1
	breq	lcdst_pg21

	cpi		a0, 2
	breq	lcdst_pg22

lcdst_pg20:
	TXT_LCD2	msg_adc, 0x09, 0x01
	rjmp	lcdst_ligne2
lcdst_pg21:
	TXT_LCD2	msg_pft, 0x09, 0x01
	rjmp	lcdst_ligne2
lcdst_pg22:
	TXT_LCD2	msg_svo, 0x09, 0x01
	rjmp	lcdst_ligne2
;...............................................
; ligne 2:
lcdst_ligne2:

	TXT_LCD2	msg_menu, 0x00, 0x02

	ldi		a0, '2'
	brts	stup_2
	ldi		a0, '1'
stup_2:
	rcall	LCD_putc
	TXT_LCD2	msg_no, 0x08, 0x02
	ret

;-----------------------------------------------
; LCD busy
;-----------------------------------------------
lcd_busy:
	TXT_LCD2	msg_sw,  0x00, 0x02
	ret



;-----------------------------------------------
; Bouton programme 1
;-----------------------------------------------
Prog_1:											; Menu
	brts	T_1									; -> sortie-2	

T_0:											; sortie 1 actuel
	lds		b0, program1
	inc		b0
	cpi		b0, anz_prog1
	
	brlo	p_depasse_1
	ldi		b0, 0
p_depasse_1:
	sts		program1, b0

lookup_1:
	J_TABLE	b0, menu1_table
	rjmp	fin_prog1

T_1:											; sortie 2 actuel
	lds		b0, program2
	inc		b0
	cpi		b0, anz_prog2
	
	brlo	p_depasse_2
	ldi		b0, 0
p_depasse_2:
	sts		program2, b0

lookup_2:
	J_TABLE	b0, menu2_table

fin_prog1:
	rcall	lcd_startup

	ret

;-----------------------------------------------
; Bouton programme 2
;-----------------------------------------------
Prog_2:
	in		_sreg, SREG							; invert T in SREG
	INVB	_sreg, T							;    T=0: sortie-1
	out		SREG, _sreg							;    T=1: sortie-2
	
	rcall	lcd_startup	
	WAIT_MS	1000

	ret

;-----------------------------------------------
; bouton up 1
;-----------------------------------------------
UP_1:
	TXT_LCD2	msg_cr1, 0x00, 0x01				; (c) v.hirschmann
	TXT_LCD2	msg_cr2, 0x00, 0x02

	WAIT_MS	3000
	rcall	lcd_startup

	ret

;-----------------------------------------------
; bouton Down 1
;-----------------------------------------------
DN_1:
	ret

;-----------------------------------------------
; bouton up 2
;-----------------------------------------------
UP_2:
	lds		a0, program2
	
	cpi		a0, 0
	breq	up2_pg0

	cpi		a0, 1
	breq	up2_pg1

	cpi		a0, 2
	_BREQ	up2_pg2
	ret											; (überflüssig)

; programme 0
up2_pg0: 
	TXT_LCD2	msg_adc2, 0x00, 0x02
	sbi		ADCSR, ADSC							; start ADC
	WP1		ADCSR, ADSC							; attendre fin ADC
;	in 		a1, ADCL
;	in 		a0, ADCH
	
;	rcall	puthex
;	mov		a0, a1
;	rcall	puthex

	rcall	calc_adc

	lds		a0, dig1
	rcall	puthex
	ldi		a0, '.'
	rcall	LCD_putc
	lds		a0, dig2
	rcall	puthex


	WAIT_MS	250

	sbic	PINC, DN2							; stop ADC si down2 appuyé
	rjmp	up2_pg0

	rcall	lcd_startup
	ret

; programme 1
up2_pg1: 
	sbi		PORTD, OUT2
	TXT_LCD2	msg_pft1, 0x00, 0x02
	ret
	
; programme 2
up2_pg2:
	AUGMT	1
	TXT_LCD2	msg_ups,  0x00, 0x02
	in		a0, OCR1AL
	rcall	putdec
	WAIT_MS	30
	ret


;-----------------------------------------------
; bouton Down 2
;-----------------------------------------------
DN_2:
	lds		a0, program2

	cpi		a0, 1
	breq	dn2_pg1

	cpi		a0, 2
	breq	dn2_pg2

	ret											; (überflüssig)

; programme 1
dn2_pg1: 
	cbi		PORTD, OUT2
	TXT_LCD2	msg_pft0, 0x00, 0x02
	ret

; programme 2
dn2_pg2:	
	DIMIN	1
	TXT_LCD2	msg_ups,  0x00, 0x02
	in		a0, OCR1AL
	rcall	putdec
	WAIT_MS	30
	ret


;-----------------------------------------------
; Initialisation des Programmes OUTPUT1
;-----------------------------------------------

;-----------------------------------------------
; OUTPUT1 = 5V
;-----------------------------------------------
prog10:
	TXT_LCD2	msg_sw,  0x00, 0x02				; busy
	cbi		PORTA, Relon						; sortie off!
	cbi		PORTC, LED1							; LED1 on

	WAIT_MS	700
	rcall	lcd_startup

	cbi		PORTA, Rel12
	cbi		PORTA, Rel05

	ret

;-----------------------------------------------
; OUTPUT1 = 12V
;-----------------------------------------------
prog11:
	TXT_LCD2	msg_sw,  0x00, 0x02				; busy					
	cbi		PORTA, Relon						; sortie off!
	cbi		PORTC, LED1							; LED1 on
	
	WAIT_MS	700
	rcall	lcd_startup
	
	sbi		PORTA, Rel12
	cbi		PORTA, Rel05

	ret

;-----------------------------------------------
; OUTPUT1 = not connected
;-----------------------------------------------
prog12:
	TXT_LCD2	msg_sw,  0x00, 0x02				; busy					
	cbi		PORTA, Relon						; sortie off!
	cbi		PORTC, LED1							; LED1 on

	WAIT_MS	700
	rcall	lcd_startup

	cbi		PORTA, Rel12
	sbi		PORTA, Rel05

	ret

;-----------------------------------------------
; Initialisation des Programmes OUTPUT2
;-----------------------------------------------

;-----------------------------------------------
; OUTPUT2 = ADC
;-----------------------------------------------
prog20:
	cbi		PORTD, OUT2							; output2 off! deux fois input sur la ligne
	cbi		PORTC, LED2							; LED2 on	

	WAIT_MS	700
	rcall	lcd_startup
	ret

;-----------------------------------------------
; OUTPUT2 = taster prellfrei
;-----------------------------------------------
prog21:
	cbi		PORTD, OUT2							; output2 off
	cbi		PORTC, LED2							; LED2 on	

	WAIT_MS	700
	rcall	lcd_startup
	ret

;-----------------------------------------------
; OUTPUT2 = servo-moteur
;-----------------------------------------------
prog22:
	sbi		PORTD, OUT2							; output2 on
	cbi		PORTC, LED2							; LED2 on	

	WAIT_MS	700
	rcall	lcd_startup
	ret





