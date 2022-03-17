








	; Espion RS232 utilisant l'ATmega2560 d'une carte Arduino Mega2560

	; Mon choix s'est port� sur cette carte car ce �c poss�de plus de 2 UART (ici il en existe 4)
	; et le prix tr�s int�ressant pour une carte tr�s bien �quip�e (�c + oscillateur + translateur USB + ...)

	; quartz 16Mhz

	; La communication PC vers appareil est intercept�e par RX1
	; La       "       appareil vers PC  "       "       "  RX2
	; Le r�sultat de l'interception est retransmis sur TX0 � 115200 bauds
	; (et de TX0 la carte convertit en USB)

	; Le baudrate de la communication espionn�e est choisi sur les entr�es PA0...PA2

	; L'entr�e PA3 sert � choisir entre espionnage donn�es sur 7 bits ou sur 8 bits

	; Par d�faut, le programme envoie vers l'USB (via TX0) les donn�es intercept�es en HEXA , mais si
	; l'on envoie le carct�re 'A' vers la carte , les donn�es sont envoy�es en ASCII




	.include "m2560def.inc"


	.def	ZeroReg	=	R9

	.def	ProvenanceDerniereDonneeLue	=	R14

	.equ	CR	=	0x0D
	.equ	LF	=	0x0A


;////////////////////////////////////////////////////////////
	.dseg
	.org SRAM_START

	; les pointeurs de 8 bits pointent vers le buffer de 512 bytes car chaque d�p�t ou retrait se fait par paire
	; le buffer est circulaire (on d�pose du d�but � la fin puis au d�but de nouveau)
	; Evidement, idem pour le retrait
	; Les 2 sous-routines sur interruption d�posent dans le buffer
	; la boucle principale retire du buffer
PointeurDeposeBuffer:	.BYTE 1
PointeurRetraitBuffer:	.BYTE 1
Buffer:					.BYTE 512

;////////////////////////////////////////////////////////////
	.cseg
	.org 0

	rjmp	_Debut

	.org URXC1addr	; USART1 RX		==> interruption sur r�ception compl�te depuis RX1
	rjmp	_USART1_RxComplete

	.org URXC2addr	; USART2 RX		==> interruption sur r�ception compl�te depuis RX2
	rjmp	_USART2_RxComplete


;////////////////////////////////////////////////////////////
_BaudRates:	; valeurs d'UBRR pour assurer les Baudrates souhait�s (pour oscilllat 16Mhz)
	.db 207, 103		;   9600 bauds ,  19200 bauds
	.db  51,  25		;  38400 bauds ,  76800 bauds
	.db  16,   16		; 115200 bauds , 115200 bauds
	.db  16,   16		; 115200 bauds , 115200 bauds
;////////////////////////////////////////////////////////////
_ConvertHEXA:
	.db "0123456789ABCDEF"
;////////////////////////////////////////////////////////////
_Debut:

	clr		ZeroReg

	out		SREG, ZeroReg

	ldi		XL, low(RAMEND)
	ldi		XH, high(RAMEND)
	out		SPL, XL
	out		SPH, XH

	; effacer la RAM
	adiw	X, 1
	ldi		ZL, low(SRAM_START)
	ldi		ZH, high(SRAM_START)
	;<==
	st		Z+, ZeroReg
	cp		ZL, XL
	cpc		ZH, XH
	brne	PC-3



	; r�sistances pull-up sur les entr�es de s�lection du Baudrate
	; et sur l'entr�e de s�lection 7 bits ou 8 bits
	ldi		R16, 1<<PORTA3 | 1<<PORTA2 | 1<<PORTA1 | 1<<PORTA0
	out		PORTA, R16

	


	; Valider r�ception et transmission � 115200 bauds sur RX0 et TX0
	ldi		R16, 1<<U2X0
	sts		UCSR0A, R16

	ldi		R16, 16
	sts		UBRR0L, R16

	ldi		R16, 1<<RXEN0 | 1<<TXEN0
	sts		UCSR0B, R16





	; Valider r�ception sur RX1 et sur RX2 (+ d�clenchements interruptions)
	; et s�lectionner baudrate en fonction des entr�es PA0...PA2
	; + s�lection donn�es sur 7 ou 8 bits
	in		R17, PINA
	andi	R17, 1<<PORTA2 | 1<<PORTA1 | 1<<PORTA0
	ldi		R16, 1<<PORTA2 | 1<<PORTA1 | 1<<PORTA0	; inverser les bits car tirage vers la masse et pas vers le +5V
	eor		R17, R16
	ldi		ZL, low(2 * _BaudRates)
	ldi		ZH, high(2 * _BaudRates)
	add		ZL, R17
	adc		ZH, ZeroReg
	lpm		R17, Z

	ldi		R16, 1<<U2X1
	sts		UCSR1A, R16

	sts		UBRR1L, R17

	ldi		R16, 1<<RXCIE1 | 1<<RXEN1
	sts		UCSR1B, R16

	lds		R16, UCSR1C
	sbr		R16, 1<<UCSZ20	; 8 bits de donn�es si PINA3 = '1'
	sbis	PINA, PORTA3
	cbr		R16, 1<<UCSZ10	; 7 bits de donn�es si PINA3 = '0'
	sts		UCSR1C, R16

	ldi		R16, 1<<U2X2
	sts		UCSR2A, R16

	sts		UBRR2L, R17

	ldi		R16, 1<<RXCIE2 | 1<<RXEN2
	sts		UCSR2B, R16

	lds		R16, UCSR2C
	sbr		R16, 1<<UCSZ20	; 8 bits de donn�es si PINA3 = '1'
	sbis	PINA, PORTA3
	cbr		R16, 1<<UCSZ20	; 7 bits de donn�es si PINA3 = '0'
	sts		UCSR2C, R16


	cbi		GPIOR0, 0	; RESET bit g�n�ral


	sei	; autoriser les interruptions sur r�ception RX1 ou RX2


	ldi		ZL, low(2 * _Texte_EspionRS232)
	ldi		ZH, high(2 * _Texte_EspionRS232)
	rcall	_EnvoiTX0_Texte

	clr		ProvenanceDerniereDonneeLue



_Boucle:

	; Tester si qqch a �t� re�u depuis le PC (via convert USB) sur RX0
	lds		R16, UCSR0A
	sbrs	R16, RXC0	; qqch a �t� re�u
	rjmp	PC+7	; rien re�u
	cbi		GPIOR0, 0	; RESET bit g�n�ral	==> signaler r�ception souhait�e en HEXA
	lds		R16, UDR0
	cpi		R16, 'A'	; tester si re�u 'A'
	brne	PC+2
	sbi		GPIOR0, 0	; SET bit g�n�ral	==> signaler r�ception en pur ASCII (pas de conversion en hexa n�cessaire)
	;<==

	; Tester s'il y a qqch � prendre dans le tampon
	lds		R16, PointeurDeposeBuffer
	lds		R17, PointeurRetraitBuffer
	cp		R16, R17
	breq	_Boucle

	ldi		XL, low(Buffer)
	ldi		XH, high(Buffer)
	lds		R18, PointeurRetraitBuffer
	push	R18
	clr		R20
	lsl		R18
	rol		R20
	add		XL, R18
	adc		XH, R20
	ld		R19, X+		; valeur (1 ou 2) signalant provenance de la donn�e (RX1 ou RX2)
	ld		R24, X		; la donn�e proprement dite

	pop		R18
	inc		R18
	sts		PointeurRetraitBuffer, R18


	cp		ProvenanceDerniereDonneeLue, R19
	breq	_Pt1	; toujours m�me provenance ==> pas besoin de pr�ciser celle-ci

	; signaler la provenance de la donn�e:
	mov		ProvenanceDerniereDonneeLue, R19


	cpi		R19, 1
	brne	PC+5

	ldi		ZL, low(2 * _Texte_RX1)		; la donn�e provient du PC
	ldi		ZH, high(2 * _Texte_RX1)
	rcall	_EnvoiTX0_Texte
	rjmp	_Pt1

	;<==

	ldi		ZL, low(2 * _Texte_RX2)		; la donn�e provient de l'appareil
	ldi		ZH, high(2 * _Texte_RX2)
	rcall	_EnvoiTX0_Texte

_Pt1:

	sbic	GPIOR0, 0	; si bit g�n�ral = 0	==> conversion hexa n�cessaire
	rjmp	_Pt2

	rcall	_ConversionHEXA	; convertir et envoyer la donn�e contenue dans R24 vers TX0

	rjmp	_Boucle


_Pt2:	; si bit g�n�ral = 1	==> conversion hexa n'est pas n�cessaire (pur ascii)

	rcall	_EnvoiTX0_

	rjmp	_Boucle

;////////////////////////////////////////////////////////////
_USART1_RxComplete:		; routine sur interruption d�clench�e par r�ception dans RX1 (port s�rie)

	push	R16
	push	R17
	push	R18

	in		R16, SREG

	ldi		YL, low(Buffer)
	ldi		YH, high(Buffer)
	lds		R18, PointeurDeposeBuffer
	push	R18
	clr		R20
	lsl		R18
	rol		R20
	add		YL, R18
	adc		YH, R20
	ldi		R17, 1		; le 1 sert � signaler donn�e provenant de RX1
	st		Y+, R17
	lds		R17, UDR1
	st		Y, R17		; stocker la donn�e proprement dite

	pop		R18
	inc		R18
	sts		PointeurDeposeBuffer, R18

	out		SREG, R16

	pop		R18
	pop		R17
	pop		R16

	reti

;////////////////////////////////////////////////////////////
_USART2_RxComplete:		; routine sur interruption d�clench�e par r�ception dans RX2 (port s�rie)

	push	R16
	push	R17
	push	R18

	in		R16, SREG

	ldi		YL, low(Buffer)
	ldi		YH, high(Buffer)
	lds		R18, PointeurDeposeBuffer
	push	R18
	clr		R20
	lsl		R18
	rol		R20
	add		YL, R18
	adc		YH, R20
	ldi		R17, 2		; le 2 sert � signaler donn�e provenant de RX2
	st		Y+, R17
	lds		R17, UDR2
	st		Y, R17		; stocker la donn�e proprement dite

	pop		R18
	inc		R18
	sts		PointeurDeposeBuffer, R18

	out		SREG, R16

	pop		R18
	pop		R17
	pop		R16

	reti

;////////////////////////////////////////////////////////////
_ConversionHEXA:	; la valeur contenue dans R24 est convertie en hexadecimal et est transmise vers TX0
					; (pr�c�d�e d'un espace)

	ldi		R25, ' '
	rcall	_EnvoiTX0
	ldi		ZL, low(2 * _ConvertHEXA)
	ldi		ZH, high(2 * _ConvertHEXA)
	push	R24
	swap	R24
	andi	R24, 0b00001111
	add		ZL, R24
	adc		ZH, ZeroReg
	lpm		R25, Z
	rcall	_EnvoiTX0
	pop		R24
	ldi		ZL, low(2 * _ConvertHEXA)
	ldi		ZH, high(2 * _ConvertHEXA)
	andi	R24, 0b00001111
	add		ZL, R24
	adc		ZH, ZeroReg
	lpm		R25, Z
;	rjmp	_EnvoiTX0
;////////////////////////////////////////////////////////////
_EnvoiTX0:			; la valeur contenue dans R25 est envoy�e vers TX0 (format RS232)

	;<==
	lds		R16, UCSR0A
	sbrs	R16, UDRE0
	rjmp	PC-3

	sts		UDR0, R25

	ret
;////////////////////////////////////////////////////////////
_EnvoiTX0_:			; la valeur contenue dans R24 est envoy�e vers TX0 (format RS232)

	;<==
	lds		R16, UCSR0A
	sbrs	R16, UDRE0
	rjmp	PC-3

	sts		UDR0, R24

	ret
;////////////////////////////////////////////////////////////
_Texte_EspionRS232:
	.db CR, LF, CR, LF, "Espion RS232", CR, LF, "------------", CR, LF, CR, LF, 0, 0
;////////////////////////////////////////////////////////////
_Texte_RX1:
	.db CR, LF, "RX1: ", 0
;////////////////////////////////////////////////////////////
_Texte_RX2:
	.db CR, LF, "RX2: ", 0
;////////////////////////////////////////////////////////////
_EnvoiTX0_Texte:	; Envoi vers TX0 (format RS232) d'un texte log� en FLASH
					; la 1�re adresse FLASH du texte doit �tre contenue dans le registre Z
					; la valeur 0 doit terminer la cha�ne � envoyer
	;<==
	lpm		R25, Z+
	tst		R25
	breq	PC+3
	rcall	_EnvoiTX0
	rjmp	PC-4
	;<==

	ret
;////////////////////////////////////////////////////////////








