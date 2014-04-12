



	; Utilisation d'un ATmega168 pour lire le contenu d'un PALCE16V8H

	; Oscillateur interne 8MHz

	; Communication 9600 bauds




	; Lecture complète du composant


	.device	ATmega168

	.include "m168def.inc"


	.def	ZeroReg			=	R1
	.def	Temp			=	R16
	.def	Temp2			=	R17
	.def	Temp3			=	R18
	.def	Temp4			=	R19
	.def	Temp5			=	R20




	; oscillateur interne 8 MHz
	; transmission à 9600 Bauds
	.set	fOSC		=	8000000
	.set	BaudRate	=	9600
	.set	ValUBRR0	=	((fOSC / (16 * BaudRate)) - 1)

	.set	CR	=	0x0D
	.set	LF	=	0x0A





	.org	0


	cbi		PORTB, PORTB6		; couper cmde Alimentation du PALCE (pin 20)
	sbi		DDRB, PORTB6

	cbi		PORTB, PORTB7		; couper cmde Tension entrée en mode programmation du PALCE (pin 3)
	sbi		DDRB, PORTB7

	cbi		PORTB, PORTB0		; état '0' pin 1 du PALCE
	sbi		DDRB, PORTB0

	cbi		PORTB, PORTB1		; état '0' pin 2 du PALCE
	sbi		DDRB, PORTB1

	cbi		PORTC, PORTC2		; état '0' pin 4 du PALCE
	sbi		DDRC, PORTC2

	cbi		PORTC, PORTC3		; état '0' pin 5 du PALCE
	sbi		DDRC, PORTC3

	cbi		PORTC, PORTC4		; état '0' pin 6 du PALCE
	sbi		DDRC, PORTC4

	cbi		PORTC, PORTC5		; état '0' pin 7 du PALCE
	sbi		DDRC, PORTC5

	; note: les pins 8 à 11 du PALCE sont cablées à la masse

	clr		ZeroReg

	out		SREG, ZeroReg

	ldi		YL, low(RAMEND)
	ldi		YH, high(RAMEND)
	out		SPH, YH
	out		SPL, YL

	; effacer toute la RAM
	ldi		Temp, high(SRAM_START - 1)
	;<==
	st		Y, ZeroReg
	sbiw	Y, 1
	cpi		YL, low(SRAM_START - 1)
	brne	PC-3
	cpc		YH, Temp
	brne	PC-5


	; Initialiser UART à 9600 Bauds
	ldi		Temp, 1<<RXEN0 | 1<<TXEN0
	sts		UCSR0B, Temp

	ldi		Temp, ValUBRR0
	sts		UBRR0L, Temp


_Debut:

	; envoyer texte "Lecteur de PALCE16V8H (AMD ou LATTICE)" vers RS232
	ldi		ZL, low(_Texte1 * 2)
	ldi		ZH, high(_Texte1 * 2)
	rcall	_uartSendTexte

	; envoyer texte "Pret (pressez la touche <G> pour commencer SVP)" vers RS232
	ldi		ZL, low(_Texte2 * 2)
	ldi		ZH, high(_Texte2 * 2)
	rcall	_uartSendTexte




_AttenteClavier:	; attendre réception caractère 'G' avant de lancer lecture

	rcall	_uartRecv
	cpi		Temp, 'G'
	brne	_AttenteClavier

	ldi		Temp, CR
	rcall	_uartSend
	ldi		Temp, LF
	rcall	_uartSend

	ldi		Temp, CR
	rcall	_uartSend
	ldi		Temp, LF
	rcall	_uartSend

	sbi		PORTB, PORTB6		; Alimentation du PALCE (pin 20) ==> 5V3

	rcall	_Tempo_1s			; temporiser 1 seconde

	sbi		PORTB, PORTB7		; appliquer tension d'entrée en mode programmation sur pin 3 du PALCE ==> 9V8 - (tension jonction saturée) = 9V3 environ

	rcall	_Tempo_10ms			; temporiser 10ms

	rcall	_Lecture_Id_Code_PALCE16V8H	; aller lire Id Code du PALCE

	cpi		Temp, 2
	breq	PC+2
	rjmp	_ErreurIdCode		; si Id Code <> 2 ==> il ne s'agit pas d'un PALCE16V8H AMD ou LATTICE
	;<==



	rcall	_Tempo_10ms			; temporiser 10ms



	; LECTURE/MEMORISATION DE LA MATRICE DE FUSIBLES FONCTIONS "ET": ==> adresses 0 à 2047
	; -------------------------------------------------------------

	clr		Temp4				; nr de colonne de la matrice souhaité pour lecture/mémorisation
								; préparation pour valeur 0 à 31

_BcleLectureMatrice:

	rcall	_pins4_a_7_egal_1000	; état 1000 sur les pins 4 à 7 du PALCE

	mov		Temp, Temp4					; 00000000 à 00011111 (0 à 31) = 6 bits hauts des 12 bits de l'adresse
	rcall	_Shift6bitsVersPin2PALCE	; le nr de colonne matrice à lire

	rcall	_pins4_a_7_egal_0100	; état 0100 sur les pins 4 à 7 du PALCE

	rcall	_Shift64bitsVersPin2PALCE	; 00000001 00000001 00000001 00000001 00000001 00000001 00000001 00000001 

	rcall	_pins4_a_7_egal_0010	; état 0010 sur les pins 4 à 7 du PALCE

	mov		Temp, Temp4					; 00000000 à 00011111 (0 à 31)
	rcall	_Lire_MemoriserColonneMatricePALCE	; le nr de colonne matrice à lire

	inc		Temp4				; incrémenter nr de colonne matrice à lire
	cpi		Temp4, 32			; il y a 32 colonnes (0 à 31) à lire dans la matrice du PALCE16V8H
	brlo	_BcleLectureMatrice



	; LECTURE/MEMORISATION DE LA SIGNATURE: ==> adresses 2056 à 2119
	; ------------------------------------

	rcall	_pins4_a_7_egal_1000	; état 1000 sur les pins 4 à 7 du PALCE

	ldi		Temp, 0b00100000		; (32) = 6 bits hauts des 12 bits de l'adresse
	rcall	_Shift6bitsVersPin2PALCE

	rcall	_pins4_a_7_egal_0100	; état 0100 sur les pins 4 à 7 du PALCE

	rcall	_Shift64bitsVersPin2PALCE	; 00000001 00000001 00000001 00000001 00000001 00000001 00000001 00000001 

	rcall	_pins4_a_7_egal_0010	; état 0010 sur les pins 4 à 7 du PALCE

	rcall	_Lire_MemoriserSignaturePALCE



	; LECTURE/MEMORISATION DES ACTIVATIONS DE TERMES DES PRODUITS DE LA MATRICE: ==> adresses 2128 à 2191
	; -------------------------------------------------------------------------

	rcall	_pins4_a_7_egal_1000	; état 1000 sur les pins 4 à 7 du PALCE

	ldi		Temp, 0b00100001		; (33) = 6 bits hauts des 12 bits de l'adresse
	rcall	_Shift6bitsVersPin2PALCE

	rcall	_pins4_a_7_egal_0100	; état 0100 sur les pins 4 à 7 du PALCE

	rcall	_Shift64bitsVersPin2PALCE	; 00000001 00000001 00000001 00000001 00000001 00000001 00000001 00000001 

	rcall	_pins4_a_7_egal_0010	; état 0010 sur les pins 4 à 7 du PALCE

	rcall	_Lire_MemoriserActivTermesProduitsPALCE



	; LECTURE/MEMORISATION DES BITS XOR SUR LES SORTIES: ==> adresses 2048 à 2055
	; -------------------------------------------------

	rcall	_pins4_a_7_egal_1000	; état 1000 sur les pins 4 à 7 du PALCE

	ldi		Temp, 0b00111100		; (60)
	rcall	_Shift6bitsVersPin2PALCE

	rcall	_pins4_a_7_egal_0100	; état 0100 sur les pins 4 à 7 du PALCE

	rcall	_Shift64bitsVersPin2_PALCE	; 64 x pulse '1' sur pin 1 PALCE (avec pin 2 = '0')

	rcall	_pins4_a_7_egal_0010	; état 0010 sur les pins 4 à 7 du PALCE

	rcall	_Lire_MemoriserActivXORsurSorties



	; LECTURE/MEMORISATION DES BITS AC1 DES SORTIES: ==> adresses 2120 à 2127
	; ---------------------------------------------

	rcall	_pins4_a_7_egal_1000	; état 1000 sur les pins 4 à 7 du PALCE

	ldi		Temp, 0b00111101		; (61)
	rcall	_Shift6bitsVersPin2PALCE

	rcall	_pins4_a_7_egal_0100	; état 0100 sur les pins 4 à 7 du PALCE

	rcall	_Shift64bitsVersPin2_PALCE	; 64 x pulse '1' sur pin 1 PALCE (avec pin 2 = '0')

	rcall	_pins4_a_7_egal_0010	; état 0010 sur les pins 4 à 7 du PALCE

	rcall	_Lire_MemoriserBitsAC1desSorties



	; LECTURE/MEMORISATION DU BIT SYN: ==> adresse 2192
	; -------------------------------

	rcall	_pins4_a_7_egal_1000	; état 1000 sur les pins 4 à 7 du PALCE

	ldi		Temp, 0b00111110		; (62)
	rcall	_Shift6bitsVersPin2PALCE

	rcall	_pins4_a_7_egal_0100	; état 0100 sur les pins 4 à 7 du PALCE

	rcall	_Shift64bitsVersPin2_PALCE	; 64 x pulse '1' sur pin 1 PALCE (avec pin 2 = '0')

	rcall	_pins4_a_7_egal_0010	; état 0010 sur les pins 4 à 7 du PALCE

	rcall	_Lire8FoisPALCE

	; adresse de stockage:
	ldi		XH, high(0x0890)	; <== adresse = 0890h
	ldi		XL, low(0x0890)

	lsr		Temp				; shifter le byte qui vient d'être lu dans la matrice du PALCE
	set							; et recopier le bit sorti vers le bit 't'
	brcs	PC+2
	clt
	;<==

	rcall	_StockageDonneeSurRAM



	; LECTURE/MEMORISATION DU BIT AC0: ==> adresse 2193
	; -------------------------------

	rcall	_pins4_a_7_egal_1000	; état 1000 sur les pins 4 à 7 du PALCE

	ldi		Temp, 0b00111111		; (63)
	rcall	_Shift6bitsVersPin2PALCE

	rcall	_pins4_a_7_egal_0100	; état 0100 sur les pins 4 à 7 du PALCE

	rcall	_Shift64bitsVersPin2_PALCE	; 64 x pulse '1' sur pin 1 PALCE (avec pin 2 = '0')

	rcall	_pins4_a_7_egal_0010	; état 0010 sur les pins 4 à 7 du PALCE

	rcall	_Lire8FoisPALCE

	; adresse de stockage:
	ldi		XH, high(0x0891)	; <== adresse = 0891h
	ldi		XL, low(0x0891)

	lsr		Temp				; shifter le byte qui vient d'être lu dans la matrice du PALCE
	set							; et recopier le bit sorti vers le bit 't'
	brcs	PC+2
	clt
	;<==

	rcall	_StockageDonneeSurRAM




	; envoyer texte "Resultat de la lecture:" vers RS232
	ldi		ZL, low(_Texte4 * 2)
	ldi		ZH, high(_Texte4 * 2)
	rcall	_uartSendTexte


	; envoyer le contenu de la RAM (2194 bits, bit par bit) par paquets de 60 bits
	; 274 bytes + 2 bits
	ldi		XH, high(SRAM_START + 0)
	ldi		XL, low(SRAM_START + 0)
	clr		Temp2				; 60 caractères par ligne d'envoi RS232
_BcleEnvoiContenuRAM:
	ldi		Temp3, 8			; 8 shifts
	ld		Temp4, X+
	;<==
	lsr		Temp4
	ldi		Temp, '0'	; '0'
	brcc	PC+2
	inc		Temp		; '1'
	;<==
	rcall	_SendAsciiBit2RS232
	dec		Temp3
	brne	PC-6

	cpi		XH, high(SRAM_START + 274)
	brne	_BcleEnvoiContenuRAM
	cpi		XL, low(SRAM_START + 274)
	brne	_BcleEnvoiContenuRAM

	ldi		Temp3, 2			; 2 shifts
	ld		Temp4, X
	;<==
	lsr		Temp4
	ldi		Temp, '0'	; '0'
	brcc	PC+2
	inc		Temp		; '1'
	;<==
	rcall	_SendAsciiBit2RS232
	dec		Temp3
	brne	PC-6

	ldi		Temp, CR
	rcall	_uartSend
	ldi		Temp, LF
	rcall	_uartSend

	ldi		Temp, CR
	rcall	_uartSend
	ldi		Temp, LF
	rcall	_uartSend

	rjmp	_DebrancherLeComposant



_ErreurIdCode:

	push	Temp				; sauvegarder l'Id Code qui vient d'être lu

	; envoyer texte "Id Code non-conforme: " vers RS232
	ldi		ZL, low(_Texte3 * 2)
	ldi		ZH, high(_Texte3 * 2)
	rcall	_uartSendTexte

	pop		Temp				; récupérer l'Id Code précédemment lu dans le composant
	rcall	_uartSend_HEXA		; et envoyer cette valeur vers RS232

	ldi		Temp, CR
	rcall	_uartSend
	ldi		Temp, LF
	rcall	_uartSend

	ldi		Temp, CR
	rcall	_uartSend
	ldi		Temp, LF
	rcall	_uartSend


_DebrancherLeComposant:

	cbi		PORTB, PORTB0		; état '0' pin 1 du PALCE
	cbi		PORTB, PORTB1		; état '0' pin 2 du PALCE
	cbi		PORTC, PORTC2		; état '0' pin 4 du PALCE
	cbi		PORTC, PORTC3		; état '0' pin 5 du PALCE
	cbi		PORTC, PORTC4		; état '0' pin 6 du PALCE
	cbi		PORTC, PORTC5		; état '0' pin 7 du PALCE

	cbi		PORTB, PORTB7		; couper cmde Tension entrée en mode programmation du PALCE (pin 3)

	rcall	_Tempo_10ms			; temporiser 10ms
	rcall	_Tempo_10ms			; temporiser 10ms

	cbi		PORTB, PORTB6		; couper cmde Alimentation du PALCE (pin 20)

	rjmp	_Debut				; boucler relire (éventuellemnt) ou simplement attendre mise hors tension carte

;=====================================

_Lecture_Id_Code_PALCE16V8H:

	rcall	_pins4_a_7_egal_1000	; état 1000 sur les pins 4 à 7 du PALCE

	; faire entrer 111010 sur pin 2 PALCE
	ldi		Temp, 0b00111010
	rcall	_Shift6bitsVersPin2PALCE

	rcall	_pins4_a_7_egal_0100	; état 0100 sur les pins 4 à 7 du PALCE

	; 64 x pulse '1' sur pin 1 PALCE
	ldi		Temp, 64
	;<==
	rcall	_Pulse1surPin1
	dec		Temp
	brne	PC-2

	rcall	_pins4_a_7_egal_0010	; état 0010 sur les pins 4 à 7 du PALCE

	rjmp	_LirePALCE			; lire état des pins 12 à 19 du PALCE ==> Temp contiendra valeur Id Code
;=====================================
	; Faire entrer les bits 5 à 0 de Temp sur la pin 2 du composant (en clockant pin 1)
_Shift6bitsVersPin2PALCE:

	lsl		Temp
	lsl		Temp

	ldi		Temp2, 6			; préparer 6 boucles
_LoopShift:
	lsl		Temp
	brcs	PC+3
	cbi		PORTB, PORTB1		; état '0' pin 2 du PALCE
	rjmp	PC+2
	;<==
	sbi		PORTB, PORTB1		; état '1' pin 2 du PALCE
	;<==
	rcall	_Pulse1surPin1
	dec		Temp2
	brne	_LoopShift


	cbi		PORTB, PORTB1		; état '0' pin 2 du PALCE

	rjmp	_Tempo_100us		; temporiser 100µs
;=====================================
	; 7 pulses sur pin 1 du PALCE (avec pin 2 = '0')
	; 1 pulse sur pin 1 du PALCE (avec pin 2 = '1')
	; 7 pulses sur pin 1 du PALCE (avec pin 2 = '0')
	; 1 pulse sur pin 1 du PALCE (avec pin 2 = '1')
	; 7 pulses sur pin 1 du PALCE (avec pin 2 = '0')
	; 1 pulse sur pin 1 du PALCE (avec pin 2 = '1')
	; 7 pulses sur pin 1 du PALCE (avec pin 2 = '0')
	; 1 pulse sur pin 1 du PALCE (avec pin 2 = '1')
	; 7 pulses sur pin 1 du PALCE (avec pin 2 = '0')
	; 1 pulse sur pin 1 du PALCE (avec pin 2 = '1')
	; 7 pulses sur pin 1 du PALCE (avec pin 2 = '0')
	; 1 pulse sur pin 1 du PALCE (avec pin 2 = '1')
	; 7 pulses sur pin 1 du PALCE (avec pin 2 = '0')
	; 1 pulse sur pin 1 du PALCE (avec pin 2 = '1')
	; 7 pulses sur pin 1 du PALCE (avec pin 2 = '0')
	; 1 pulse sur pin 1 du PALCE (avec pin 2 = '1')
	; pin 2 = '0'

_Shift64bitsVersPin2PALCE:

	ldi		Temp2, 8			; préparer 8 boucles
_BoucleShift:
	cbi		PORTB, PORTB1		; état '0' pin 2 du PALCE
	ldi		Temp3, 7			; préparer 7 boucles
	;<==
	rcall	_Pulse1surPin1
	dec		Temp3
	brne	PC-2
	sbi		PORTB, PORTB1		; état '1' pin 2 du PALCE
	rcall	_Pulse1surPin1
	dec		Temp2
	brne	_BoucleShift

	cbi		PORTB, PORTB1		; état '0' pin 2 du PALCE

	rjmp	_Tempo_100us		; temporiser 100µs
;=====================================
	; 64 x pulse '1' sur pin 1 PALCE (avec pin 2 = '0')
_Shift64bitsVersPin2_PALCE:

	cbi		PORTB, PORTB1		; état '0' pin 2 du PALCE
	ldi		Temp, 64
	;<==
	rcall	_Pulse1surPin1
	dec		Temp
	brne	PC-2

	ret
;=====================================
_Lire_MemoriserColonneMatricePALCE:

	mov		Temp5, Temp			; le nr de la colonne de la matrice à lire/mémoriser

	clr		Temp2				; préparer 8 lectures/stockages dans le PALCE

_Boucle1LectureColonneMatrice:

	rcall	_LirePALCE

	; calcul de l'adresse de stockage
	clr		XH					; <== 1ère adresse = 0
	clr		XL
	add		XL, Temp5			; ajouter à l'adresse (poids 1)
	adc		XH, ZeroReg
	mov		Temp3, Temp2
	lsl		Temp3
	lsl		Temp3
	lsl		Temp3
	lsl		Temp3
	lsl		Temp3
	add		XL, Temp3			; ajouter à l'adresse (poids 32)
	adc		XH, ZeroReg

	movw	Y, X				; sauvegarder le calcul d'adresse de base


	clr		Temp3				; préparer 8 boucles

_Boucle2LectureColonneMatrice:

	movw	X, Y				; récupérer le calcul d'adresse de base
	add		XH, Temp3			; ajouter à l'adresse (poids 256)

	lsr		Temp				; shifter le byte qui vient d'être lu dans la matrice du PALCE
	set							; et recopier le bit sorti vers le bit 't'
	brcs	PC+2
	clt
	;<==

	push	Temp
	push	Temp2
	push	Temp3
	rcall	_StockageDonneeSurRAM
	pop		Temp3
	pop		Temp2
	pop		Temp

	inc		Temp3
	cpi		Temp3, 8
	brlo	_Boucle2LectureColonneMatrice	; isoler et mémoriser 1 à 1 les 8 bits du byte lu dans le PALCE


	inc		Temp2
	cpi		Temp2, 8
	brlo	_Boucle1LectureColonneMatrice


	ret
;=====================================

_Lire_MemoriserSignaturePALCE:

	clr		Temp2				; préparer 8 lectures/stockages dans le PALCE

_Boucle1LectureSignature:

	rcall	_LirePALCE

	; calcul de l'adresse de stockage
	ldi		XH, high(0x0808)	; <== 1ère adresse = 0808h
	ldi		XL, low(0x0808)
	add		XL, Temp2			; ajouter à l'adresse (poids 1)
	adc		XH, ZeroReg

	movw	Y, X				; sauvegarder le calcul d'adresse de base


	clr		Temp3				; préparer 8 boucles

_Boucle2LectureSignature:

	movw	X, Y				; récupérer le calcul d'adresse de base
	mov		Temp4, Temp3
	lsl		Temp4
	lsl		Temp4
	lsl		Temp4
	add		XL, Temp4			; ajouter à l'adresse (poids 8)
	adc		XH, ZeroReg

	lsr		Temp				; shifter le byte qui vient d'être lu dans la matrice du PALCE
	set							; et recopier le bit sorti vers le bit 't'
	brcs	PC+2
	clt
	;<==


	push	Temp
	push	Temp2
	push	Temp3
	rcall	_StockageDonneeSurRAM
	pop		Temp3
	pop		Temp2
	pop		Temp

	inc		Temp3
	cpi		Temp3, 8
	brlo	_Boucle2LectureSignature	; isoler et mémoriser 1 à 1 les 8 bits du byte lu dans le PALCE


	inc		Temp2
	cpi		Temp2, 8
	brlo	_Boucle1LectureSignature


	ret
;=====================================

_Lire_MemoriserActivTermesProduitsPALCE:

	clr		Temp2				; préparer 8 lectures/stockages dans le PALCE

_Boucle1LectureActivTermesProduits:

	rcall	_LirePALCE

	; calcul de l'adresse de stockage
	ldi		XH, high(0x0850)	; <== 1ère adresse = 0850h
	ldi		XL, low(0x0850)
	add		XL, Temp2			; ajouter à l'adresse (poids 1)
	adc		XH, ZeroReg

	movw	Y, X				; sauvegarder le calcul d'adresse de base


	clr		Temp3				; préparer 8 boucles

_Boucle2LectureActivTermesProduits:

	movw	X, Y				; récupérer le calcul d'adresse de base
	mov		Temp4, Temp3
	lsl		Temp4
	lsl		Temp4
	lsl		Temp4
	add		XL, Temp4			; ajouter à l'adresse (poids 8)
	adc		XH, ZeroReg

	lsr		Temp				; shifter le byte qui vient d'être lu dans la matrice du PALCE
	set							; et recopier le bit sorti vers le bit 't'
	brcs	PC+2
	clt
	;<==


	push	Temp
	push	Temp2
	push	Temp3
	rcall	_StockageDonneeSurRAM
	pop		Temp3
	pop		Temp2
	pop		Temp

	inc		Temp3
	cpi		Temp3, 8
	brlo	_Boucle2LectureActivTermesProduits	; isoler et mémoriser 1 à 1 les 8 bits du byte lu dans le PALCE


	inc		Temp2
	cpi		Temp2, 8
	brlo	_Boucle1LectureActivTermesProduits


	ret
;=====================================

_Lire_MemoriserActivXORsurSorties:

	rcall	_Lire8FoisPALCE

	; 1ère adresse de stockage:
	ldi		XH, high(0x0800)	; <== 1ère adresse = 0800h
	ldi		XL, low(0x0800)

	movw	Y, X				; sauvegarder le calcul d'adresse de base

	clr		Temp2				; préparer 8 boucles

_BoucleLectureActivXORsurSorties:

	movw	X, Y				; récupérer le calcul d'adresse de base
	add		XL, Temp2
	adc		XH, ZeroReg

	lsr		Temp				; shifter le byte qui vient d'être lu dans la matrice du PALCE
	set							; et recopier le bit sorti vers le bit 't'
	brcs	PC+2
	clt
	;<==


	push	Temp
	push	Temp2
	rcall	_StockageDonneeSurRAM
	pop		Temp2
	pop		Temp

	inc		Temp2
	cpi		Temp2, 8
	brlo	_BoucleLectureActivXORsurSorties


	ret
;=====================================

_Lire_MemoriserBitsAC1desSorties:

	rcall	_Lire8FoisPALCE

	; 1ère adresse de stockage:
	ldi		XH, high(0x0848)	; <== 1ère adresse = 0848h
	ldi		XL, low(0x0848)

	movw	Y, X				; sauvegarder le calcul d'adresse de base

	clr		Temp2				; préparer 8 boucles

_BoucleLectureBitsAC1desSorties:

	movw	X, Y				; récupérer le calcul d'adresse de base
	add		XL, Temp2
	adc		XH, ZeroReg

	lsr		Temp				; shifter le byte qui vient d'être lu dans la matrice du PALCE
	set							; et recopier le bit sorti vers le bit 't'
	brcs	PC+2
	clt
	;<==


	push	Temp
	push	Temp2
	rcall	_StockageDonneeSurRAM
	pop		Temp2
	pop		Temp

	inc		Temp2
	cpi		Temp2, 8
	brlo	_BoucleLectureBitsAC1desSorties


	ret
;=====================================
	; stocker valeur d'1 bit dans la RAM
	; adresse 0 à 2193 ==> utilisation RAM d'adresse (SRAM_START + 0) à (SRAM_START + 274)
	; adresse fournie par XL:XH
	; le bit à stocker doit être copié dans le fameux bit 't' du µc avant l'appel de cette routine
_StockageDonneeSurRAM:

	push	XL

	; diviser l'adresse par 8
	lsr		XH
	ror		XL
	lsr		XH
	ror		XL
	lsr		XH
	ror		XL
	ldi		Temp, low(SRAM_START)
	add		XL, Temp
	ldi		Temp, high(SRAM_START)
	adc		XH, Temp

	pop		Temp
	andi	Temp, 0b00000111	; 0 à 7

	; Temp2 = 2 ^ Temp
	; Temp3 = Temp XOR 11111111
	ldi		Temp2, 0b00000001
	tst		Temp
	breq	PC+4
	;<==
	lsl		Temp2
	dec		Temp
	brne	PC-2
	;<==
	mov		Temp3, Temp2
	com		Temp3

	ld		Temp, X
	brtc	PC+3				; sauter si bit = '0'
	or		Temp, Temp2			; bit à '1'
	rjmp	PC+2
	;<==
	and		Temp, Temp3			; bit à '0'
	;<==
	st		X, Temp

	ret
;=====================================
	; état des pins 4 à 7 du PALCE
_pins4_a_7_egal_1000:

	sbi		PORTC, PORTC2		; état '1' pin 4 du PALCE
	cbi		PORTC, PORTC3		; état '0' pin 5 du PALCE
	cbi		PORTC, PORTC4		; état '0' pin 6 du PALCE
	cbi		PORTC, PORTC5		; état '0' pin 7 du PALCE

	ret
;=====================================
	; état des pins 4 à 7 du PALCE
_pins4_a_7_egal_0100:

	cbi		PORTC, PORTC2		; état '0' pin 4 du PALCE
	sbi		PORTC, PORTC3		; état '1' pin 5 du PALCE
	cbi		PORTC, PORTC4		; état '0' pin 6 du PALCE
	cbi		PORTC, PORTC5		; état '0' pin 7 du PALCE

	ret
;=====================================
	; état des pins 4 à 7 du PALCE
_pins4_a_7_egal_0010:

	cbi		PORTC, PORTC2		; état '0' pin 4 du PALCE
	cbi		PORTC, PORTC3		; état '0' pin 5 du PALCE
	sbi		PORTC, PORTC4		; état '1' pin 6 du PALCE
	cbi		PORTC, PORTC5		; état '0' pin 7 du PALCE

	ret
;=====================================
	; pulse '1' sur pin 1 du PALCE
_Pulse1surPin1:

	sbi		PORTB, PORTB0		; état '1' pin 1 du PALCE
	nop
	cbi		PORTB, PORTB0		; état '0' pin 1 du PALCE

	ret
;=====================================
	; lire état des pins 12 à 19 du PALCE
	; + pulse sur pin 1
_LirePALCE:

	push	Temp2

	in		Temp, PINC
	in		Temp2, PIND
	andi	Temp, 0b00000011	; bits 0 et 1
	andi	Temp2, 0b11111100	; bits 2 à 7
	or		Temp2, Temp

	; remise en ordre des bits 0 à 7 ==> 7 à 0
	bst		Temp2, 7
	bld		Temp, 0
	bst		Temp2, 6
	bld		Temp, 1
	bst		Temp2, 5
	bld		Temp, 2
	bst		Temp2, 4
	bld		Temp, 3
	bst		Temp2, 3
	bld		Temp, 4
	bst		Temp2, 2
	bld		Temp, 5
	bst		Temp2, 1
	bld		Temp, 6
	bst		Temp2, 0
	bld		Temp, 7

	pop		Temp2

	rjmp	_Pulse1surPin1
;=====================================
	; lire 8 bytes dans le PALCE (garder le 8ème uniquement)
_Lire8FoisPALCE:

	ldi		Temp2, 8			; préparer 8 boucles
	;<==
	rcall	_LirePALCE
	dec		Temp2
	brne	PC-2

	ret
;=====================================
	; sous-routine d'envoi d'un texte vers l'UART
	; l'adresse d'implantation mémoire du 1er caractère est contenue dans ZL:ZH
_uartSendTexte:

	lpm		Temp, Z+
	tst		Temp
	brne	PC+2
	ret
	rcall	_uartSend
	rjmp	_uartSendTexte
;=====================================
	; pour envoi ascii '0' ou '1' par lignes de 60 caractères
_SendAsciiBit2RS232:

	push	Temp2
	rcall	_uartSend
	pop		Temp2

	inc		Temp2				; 60 caractères par ligne
	cpi		Temp2, 60
	brne	PC+6
	ldi		Temp, CR
	rcall	_uartSend
	ldi		Temp, LF
	rcall	_uartSend
	clr		Temp2				; ré-initialiser compteur de caractères
	;<==

	ret
;=====================================
	; sous-routine d'envoi donnée vers l'UART
_uartSend:

	;<==
	lds		Temp2, UCSR0A
	sbrs	Temp2, UDRE0
	rjmp	PC-3

	sts		UDR0, Temp

	ret
;=====================================

	; sous-routine de réception de données depuis l'UART
_uartRecv:

	;<==
	lds		Temp, UCSR0A
	sbrs	Temp, RXC0
	rjmp	PC-3

	lds		Temp, UDR0

	rjmp	_uartSend		; envoyer écho du caractère reçu
;=====================================
		; convertir Temp en Hexa et envoyer UART
_uartSend_HEXA:

	push	Temp2
	push	Temp3

	mov		Temp3, Temp

	ldi		Temp2, (('A' - '0') - 10)		; écart de 17 entre "0" et "A"
	swap	Temp
	andi	Temp, 0b00001111
	cpi		Temp, 10
	brlo	PC+2
	add		Temp, Temp2
	;<==
	ldi		Temp2, '0'						; valeur 48
	add		Temp, Temp2
	rcall	_uartSend

	ldi		Temp2, (('A' - '0') - 10)		; écart de 17 entre "0" et "A"
	mov		Temp, Temp3
	andi	Temp, 0b00001111
	cpi		Temp, 10
	brlo	PC+2
	add		Temp, Temp2
	;<==
	ldi		Temp2, '0'						; valeur 48
	add		Temp, Temp2
	rcall	_uartSend

	pop		Temp3
	pop		Temp2

	ret
; ====================================
		; temporiser environ 1 seconde
_Tempo_1s:

	ldi		Temp, 72
	;<==
	ldi		Temp2, 188
	;<==
	ldi		Temp3, 196
	;<==
	dec		Temp3
	brne	PC-1
	dec		Temp2
	brne	PC-4
	dec		Temp
	brne	PC-7

	ret
; ====================================
		; temporiser environ 10ms
_Tempo_10ms:

	ldi		Temp, 134
	;<==
	ldi		Temp2, 198
	;<==
	dec		Temp2
	brne	PC-1
	dec		Temp
	brne	PC-4

	ret
; ====================================
		; temporiser environ 100µs
_Tempo_100us:

	ldi		Temp, 2
	;<==
	ldi		Temp2, 132
	;<==
	dec		Temp2
	brne	PC-1
	dec		Temp
	brne	PC-4

	ret
; ====================================

_Texte1:

	.db CR, LF, CR, LF, "Lecteur de PALCE16V8H (AMD ou LATTICE)", CR, LF, 0, 0

_Texte2:

	.db CR, LF, "Pret (pressez la touche <G> pour commencer SVP)", CR, LF, 0

_Texte3:

	.db CR, LF, "Id Code non-conforme: ", 0, 0

_Texte4:

	.db CR, LF, "Resultat de la lecture:", CR, LF, 0

; ====================================






