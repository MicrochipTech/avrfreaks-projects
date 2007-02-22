;***********************************************
; Labornetzgerät 2006:                     Reset
;
; uProzessor gesteuert: Atmega8535
;
; hirschmann vitalis
; 18.04.2006
;
; PRisme Monitor inclu
; Version 0.99
; (c) 2005 by Freddy
;***********************************************

;-----------------------------------------------
; includes I
;-----------------------------------------------

.include 	"m8535def.inc"						; port / bit informations
.include 	"lng_definitions.asm"					; registres / constantes
.include 	"macros.asm"						; macros
;-----------------------------------------------

.dseg
.org	0x0060
	program1:	.byte 	1
	program2:	.byte	2
	dig1:		.byte	1
	dig2:		.byte	1
.cseg
;-----------------------------------------------
; interruptions
;-----------------------------------------------

.org	0										; reset vector
	rjmp	reset		
.org	INT0addr								; interruption externe 0
	rjmp	int_0
.org	INT1addr								; interruption externe 1
	rjmp	int_1							
.org	URXCaddr								; USART receive complete
	rjmp	UART_rxc
.org	UDREaddr								; USART data registry empty
	rjmp	UART_dre
.org	0x15									; end of vector table

;...............................................
; routines d'interruption
;...............................................
.include "interrupts.asm"


;-----------------------------------------------
; reset
;-----------------------------------------------

reset:		
	
	LDSP	RAMEND								; Load Stack Pointer (SP)
	
	OUTI	DDRA, 0b00001111					; A: Relais / ADC
	OUTI	DDRB, 0xff							; B: LCD Port
	OUTI	DDRC, 0b00000011					; C: Boutons / LEDs
	OUTI	DDRD, 0x00							; D: Boutons Int0/1, ProgUSB (, OUTPUT2)
;...............................................
	cbi		PORTC, LED1							; LED I/O on
	cbi		PORTC, LED2
	cbi		PORTA, Relon						; initialiser Relais (output off)
	cbi		PORTA, Rel05
	cbi		PORTA, Rel12							
;...............................................
	rcall	monitor_init						; monitor initialisation
;...............................................
	ldi		w, 0								; initialiser le program
	sts		program1, w
	sts		program2, w
;................................................
	OUTI	MCUCR, (1<<ISC11)+(1<<ISC01)		; falling edge
	OUTI	GICR, (1<<INT0)+(1<<INT1)
;................................................
	OUTI	TCCR1B, (1<<WGM13)+(1<<CS11)+(1<<CS10)	; prescale 1/64
;	OUTI	TCCR1A, (1<<COM1A1)					; non inverted PWM
	OUTI	OCR1AH, high(98)
	OUTI	OCR1AL, low(98) 
	OUTI	ICR1H, high(1250)					; TOP PWM
	OUTI	ICR1L, low(1250)
;................................................
	OUTI	ADCSR, (1<<ADEN)+6					; Enable ADC, prescale
	OUTI	ADMUX, 6							; PINA3 ADC input
;................................................
	rcall	lcd_init							; reset LCD
	rcall	lcd_home
	rcall	lcd_cursor_off
;	rcall	lcd_startup0
	rcall	lcd_startup
;................................................
	sei
;................................................
	rjmp	main


;-----------------------------------------------
; includes II
;-----------------------------------------------
.include 	"monitor_lib.asm"					; include Monitor module
.include 	"LCDisme.asm"						; routines pour LCD
;.include "math.asm"

;-----------------------------------------------
; includes III
;-----------------------------------------------
.include	"main.asm"
.include	"sousroutine.asm"
;.include	"mesure.asm"

;-----------------------------------------------
; look-up tables
;-----------------------------------------------
menu1_table:
.dw		prog10, prog11, prog12

menu2_table:
.dw		prog20, prog21, prog22



