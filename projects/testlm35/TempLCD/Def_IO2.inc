;*******	D I R E T T I V E   D I   A S S E G N A Z I O N E	*******
.DEF		Ascii				= R0			;Carattere da spedire al Display
.DEF		Delay1			= R1			;Usato dalla routine di ritardo
.DEF		Delay2			= R2			;		"					"			
.DEF		Delay3			= R3			;		"					"			
.DEF		Lval				= R4			;Copia di ADCL (parte bassa conversione AD)
.DEF		Hval				= R5			;Copia di ADCH
.DEF		CopyLval		= R6			;Copia di Lval
.DEF		CopyHval		= R7			;Copia di Hval
.DEF		Count			= R8			;Contatore usato nelle operazioni per trovare i 4 caratteri  ASCII

.DEF		Temp			= R16		;Registro di uso generico
.DEF		Temp2			= R17		;	"			"			"
.DEF		Dig0				= R18		;Digit 0 (migliaia)
.DEF		Dig1				= R19		;Digit 1 (centinaia)
.DEF		Dig2				= R20		;Digit 2 (decine)
.DEF		Dig3				= R21		;Digit 3 (unità)

.EQU	LCDPORT		= PORTB	;Porta dove collegare LCD
.EQU	LCDDIR		= DDRB	;Settaggio porta LCD come I/O
.EQU	LCDING		= PINB		;Lettura porta LCD

.EQU	A_DPORT 	= PORTA	;Porta ingresso tensione da convertire
.EQU	A_DDIR		= DDRA	;Settaggio porta A/D come I/O
.EQU	A_DING		= PINA		;Lettura porta A/D

.EQU	LEDPORT 	= PORTD	;Porta uscita per LED Debug
.EQU	LEDDIR		= DDRD	;Settaggio porta LED come I/O
.EQU	LEDING		= PINB		;Lettura porta LED

;**** Porta LCD ****
.EQU		RS_LCD		= 0
.EQU		RW_LCD		= 1
.EQU		EN_LCD		= 2
;									= 3
.EQU		DB4_LCD		= 4
.EQU		DB5_LCD		= 5
.EQU		DB6_LCD		= 6
.EQU		DB7_LCD		= 7

;