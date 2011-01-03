;************************************************************************
;* Fichier : foam cutting.asm							                *
;* Titre: interface pour la commande d'un fil chaud cnc 			    *
;* Version: 1.0					                	                    *
;* Auteur: ludomercet                 				                	*
;* Date: 01/05/2010	de création				        	                *
;* Date: 24/05/2010 ok pour génération du fichier .hex					*														*
;************************************************************************


.include "m32def.inc"	; nom du fichiers de références des registres


;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Déclaration des Variables		                 	
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

	.def b = r15
	.def bouton =r26			; registre du canal midi
	.def a =R16				; registre équivalent à accumulateur (X51)
	.def sauvreg = R17		; registre de sauvegarde de SREG
    .def comptX = R18		; registres de flag
    .def comptY = R19			; valeurs des notes
	.def comptX1 =R20			; registre de note 
	.def comptY1 = R21			; registre d'usage
	.def etat =R22
	.def chauff= R23				; etat de la chauffe
	.def pwm = R24
	.def pwm_consigne =R25

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Déclaration des zones de variables dans la sram
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.DSEG
.ORG 0x0060
midrecin: 	.BYTE 3			; zone de 3 octets pour valeur uart midi in
midrecout:	.BYTE 3			; zone de 3 octets pour valeur uart pédalier
valeurnote:	.BYTE 4			; zone de 4 octets pour valeur de chaques notes

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Déclaration des Vecteurs d'interruptions
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.CSEG
.ORG 0x0000			;début zone mémoire programme

	rjmp RESET ; Reset Handler
 	rjmp EXT_INT0 ; IRQ0 Handler
 	rjmp EXT_INT1 ; IRQ1 Handler
 	rjmp EXT_INT2 ; IRQ2 Handler
 	rjmp TIM2_COMP ; Timer2 Compare Handler
 	rjmp TIM2_OVF ; Timer2 Overflow Handler
 	rjmp TIM1_CAPT ; Timer1 Capture Handler
 	rjmp TIM1_COMPA ; Timer1 CompareA Handler
 	rjmp TIM1_COMPB ; Timer1 CompareB Handler
 	rjmp TIM1_OVF ; Timer1 Overflow Handler
 	rjmp TIM0_COMP ; Timer0 Compare Handler
 	rjmp TIM0_OVF ; Timer0 Overflow Handler
 	rjmp SPI_STC ; SPI Transfer Complete Handler
 	rjmp USART_RXC ; USART RX Complete Handler
 	rjmp USART_UDRE ; UDR Empty Handler
	rjmp USART_TXC ; USART TX Complete Handler
	rjmp V_ADC ; ADC Conversion Complete Handler
	rjmp EE_RDY ; EEPROM Ready Handler
 	rjmp ANA_COMP ; Analog Comparator Handler
 	rjmp TWI ; Two-wire Serial Interface Handler
	rjmp SPM_RDY ; Store Program Memory Ready Handler
	
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Routines de traitement des interruptions
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx



	EXT_INT0: 	reti
	EXT_INT1: 	
	
;*************************
; PILOTAG DE LA CHAUFFE
;*************************

				
				in sauvreg,sreg
				push sauvreg
				push a
				
		; génération du timer à X khz 
		;----------------------------


				cpi chauff,1	;test niveau haut ou bas de l'horloge
				breq etat1
		
				sbi portb,5		; niveau haut horloge
				ldi chauff,1

				sbis pinb,4		; test chauff manuelle
				rjmp chauff_pc
				inc pwm			; incremente le compteur de passage
				cpi pwm,100
				brne plus
				ldi pwm,$00		; on est à 100, on remet le compteur à 0 et on met l'opto en marche.
				sbi portb,6		;

				
plus:			cp pwm,pwm_consigne		;  pwm = consigne
				brne plus1				; éteint la chauff
				cbi portb,6

				;gestion des boutons
				
plus1:			in a,pinb
				andi a,$03
				cp a,bouton 	; test si les bouton on la même valeur 
				breq enreg		; oui on enregitre et on sort

				cpi bouton,$03		; test si ancien etat haut
				brne enreg

				sbic pinb,0		; a t'on appuyer sur +
				rjmp moins
				
				cpi pwm_consigne,97	; test si pas superieur à la consigne
				breq moins			
				brpl moins
				inc pwm_consigne	; oui on incremente la consigne


moins :			sbic pinb,1
				brne enreg
				
				cpi pwm_consigne,3	; test si pas inferieur à la consigne
				breq enreg
				brmi enreg
				dec pwm_consigne	; on decremente la consigne				

enreg :			mov bouton ,a		; on enregistre quoi qu'il arrive le boutons
				rjmp sortie

etat1:			cbi portb,5		; niveau bas de l'horloge
				ldi chauff,0
				rjmp sortie

chauff_pc:		sbis pinb,3 			;lecture du bit 3 (pin 16 de la db25) pour lire l'etat du pwm fournit par le pc
				rjmp clear6
				sbi portb,6				; opto en marche
				rjmp sortie				; 
clear6:			cbi portb,6				; opto eteint
	

sortie:			pop a
				pop sauvreg
				out sreg,sauvreg
				
				reti
	EXT_INT2:	reti
 	TIM2_COMP: 	reti
	TIM2_OVF :
	
			reti

	TIM1_CAPT:	reti
	TIM1_COMPA: 	reti
	TIM1_COMPB :	reti
	TIM1_OVF :	reti
	TIM0_COMP:  

;******************
;ROUTINE DU BUZZER
;******************	
			

buzz1:		dec a
			tst a
			brne buzz2
			dec sauvreg
			tst sauvreg
			brne buzz2
			ldi a,(0<<toie2|1<<toie0)
			cbi portb,7
			out timsk,a
			rjmp buzz_out

buzz2:		sbis pinb,7
			rjmp set_buzz
			cbi portb,7
			rjmp buzz_out
set_buzz:	sbi portb,7

	
buzz_out:
			
				reti

	TIM0_OVF :	reti
	SPI_STC :	reti
	USART_RXC: 	reti
	USART_UDRE :	reti
	USART_TXC: 	reti
	V_ADC :		reti
	EE_RDY :		reti
	ANA_COMP :	reti
	TWI :		reti
	SPM_RDY :	reti

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Programme de RESET
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

; ATTENTION !!!
; si utilisation d'un quartz : vérifier les flags au chargement du 
; programme dans la puce !

RESET:		ldi a,high(RAMEND)
			out SPH,a
			ldi	a,low(RAMEND)
			out	SPL,a		; Initialisation de la pile à l'état bas

; initialisation des port 
;-------------------------   

		    ldi a,0b11111111    ; port d en in/out avec uart tx en out
								; d7 nc existe pas
								; d6 entrée des multiplexeurs
								; d5 entrée | haute 
								; d4 entrée | canal midi
								; d3 entrée |
								; d2 entrée | basse
								; d1 sortie emission des données midi
								; d0 entrée réception des données midi
		    out ddra,a			; 
		    ldi a,$cc			; port d resistence de rapel
		    out porta,a      	; 
			ldi a,$1f			; multiplexeur en off et addresse à 0
			out portb,a			; 
			ldi a,0b11100000	; port b en out 
								; b7 sortie sélection du 1er multiplexeur
								; b6 sortie sélection du 2eme multiplexeur
								; b5 sortie sélection du 3eme multiplexeur
								; b4 sortie sélection du 4eme multiplexeur
								; b3 nc n'est pas utilisé
								; b2 sortie addresse des entrée du multiplexeur
								; b1 sortie addresse des entrée du multiplexeur 
								; b0 sortie addresse des entrée du multiplexeur
			out ddrb,a			;
			ldi a,$1f			; multiplexeur en off et addresse à 0
			out portb,a			; 
		    ldi a,0b11111111    ; port d en in/out avec uart tx en out
								; d7 nc existe pas
								; d6 entrée des multiplexeurs
								; d5 entrée | haute 
								; d4 entrée | canal midi
								; d3 entrée |
								; d2 entrée | basse
								; d1 sortie emission des données midi
								; d0 entrée réception des données midi
		    out ddrc,a			; 
		    ldi a,$cc			; port d resistence de rapel
		    out portc,a      	; 
		    
		    ldi a,0b00000000    ; port d en in/out avec uart tx en out
								; d7 nc existe pas
								; d6 entrée des multiplexeurs
								; d5 entrée | haute 
								; d4 entrée | canal midi
								; d3 entrée |
								; d2 entrée | basse
								; d1 sortie emission des données midi
								; d0 entrée réception des données midi
		    out ddrd,a			; 
			ldi a,$00			; port d resistence de rapel
		    out portd,a      	; 
/*
 DDRA  = 0xFF;		// sortie moteur x et y
 PORTA = 0xCC;		// moteurs position 1
 PORTB = 0X1F;		// pullup en entrée
 DDRB  = 0xE0;		// sortie en partie haute et entrée en partie basse
 PORTB = 0x1F;
 DDRC  = 0xFF;		// sortie moteur x1 et y1
 PORTC = 0xCC;		// moteurs position 1
 PORTD = 0x00;		// pull up off
 DDRD  = 0x00;		// entrée moteur x,x1,y,y1 */

; remise à 0 des compteurs
;-------------------------

		ldi comptX,$01
		ldi comptY,$01
		ldi comptX1,$01
		ldi comptY1,$01


; initialisation timer0
;----------------------

		ldi a,$01
		out timsk,a
		ldi a,$fa
		out tcnt0,a
		ldi a,$02
		out tccr0,a

; initialisation 
;----------------

		ldi pwm_consigne,50

		sei 					; interruption en marche


;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Programme principal
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


; Lecture de l'eeprom
;--------------------


;***************
; MODE JEDICUT *
;***************
; 
; mode avec gestion de la chauffe, timer pour control des moteurs.
; 3 timer possible : 2000, 4000, 10000, peut etre 20000?
; moteur en pas entier ou demi pas, peut être 1/4 de pas

jedicut:

;buzz 1 fois

		rcall buzzer

jedicut1:
		sbis pinb,2		; test si le pc à mit les moteur en marche
		rjmp jedicut1 	; non on boucle
;**************************************************************************
;axe X
;**************************************************************************	
		sbrc etat,0		; test etatX
		rjmp test_avanceX	;
		sbis pind,1		;test bit avance X à 1
		rjmp axe_Y		; non on passe au suivant
		ori etat,$01	; etatx = 1
		rjmp axe_Y		;

test_avanceX:
		sbic pind,1		;
		rjmp axe_Y		;
		andi etat,$fe	; etatx = 0
		sbis pind,0		; test bit de sens
		rjmp reverse_X	;

//----------------------------------------------------------------------------
//	avance moteur X
//----------------------------------------------------------------------------
		
		inc comptX
		cpi comptX,8		; test !!!!!!!!!!!!!!!!!!!!!!!!
		brne avanceX		; test !!!!!!!!!!!!!!!!!!!!!!!
		
		ldi comptX,$00

avanceX:
		cpi comptX,0
		breq X1
		cpi comptX,1
		breq X2
		cpi comptX,2
		breq X3
		cpi comptX,3
		breq X4
		cpi comptX,4
		breq X5
		cpi comptX,5
		breq X6
		cpi comptX,6
		breq X7
		cpi comptX,7
;		breq X8
;		ldi comptX,$00
		

;envoie sur le port	
		
X8:
		in a,porta
		andi a,$f0
		ori a,$01
		out porta,a
		rjmp axe_Y
X7:
		in a,porta
		andi a,$f0
		ori a,$03
		out porta,a
		rjmp axe_Y
X6:
		in a,porta
		andi a,$f0
		ori a,$02
		out porta,a
		rjmp axe_Y
X5:
		in a,porta
		andi a,$f0
		ori a,$06
		out porta,a
		rjmp axe_Y
X4:
		in a,porta
		andi a,$f0
		ori a,$04
		out porta,a
		rjmp axe_Y
X3:
		in a,porta
		andi a,$f0
		ori a,$0c
		out porta,a
		rjmp axe_Y
X2:
		in a,porta
		andi a,$f0
		ori a,$08
		out porta,a
		rjmp axe_Y
X1:
		in a,porta
		andi a,$f0
		ori a,$09
		out porta,a
		rjmp axe_Y

//----------------------------------------------------------------------------
//	recule moteur X
//----------------------------------------------------------------------------

reverse_X:

		cpi comptX,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
		brne decX			; test !!!!!!!!!!!!!!!!!!!!!!!
		ldi comptX,$08
		
decX:	dec comptX

		cpi comptX,0
		breq XR1
		cpi comptX,1
		breq XR2
		cpi comptX,2
		breq XR3
		cpi comptX,3
		breq XR4
		cpi comptX,4
		breq XR5
		cpi comptX,5
		breq XR6
		cpi comptx,6
		breq XR7
		cpi comptx,7
;		breq XR8
;		ldi comptX,$00
;		rjmp XR1

;envoie sur le port	
		
XR8:
		in a,porta
		andi a,$f0
		ori a,$01
		out porta,a
;		cpi comptX,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX,$08
		rjmp axe_Y
XR1:
		in a,porta
		andi a,$f0
		ori a,$09
		out porta,a
;		cpi comptX,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX,$08
		rjmp axe_Y
XR2:
		in a,porta
		andi a,$f0
		ori a,$08
		out porta,a
;		cpi comptX,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX,$08
		rjmp axe_Y
XR3:
		in a,porta
		andi a,$f0
		ori a,$0c
		out porta,a
;		cpi comptX,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX,$08
		rjmp axe_Y
XR4:
		in a,porta
		andi a,$f0
		ori a,$04
		out porta,a
;		cpi comptX,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX,$08
		rjmp axe_Y
XR5:
		in a,porta
		andi a,$f0
		ori a,$06
		out porta,a
;		cpi comptX,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX,$08
		rjmp axe_Y
XR6:
		in a,porta
		andi a,$f0
		ori a,$02
		out porta,a
;		cpi comptX,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX,$08
		rjmp axe_Y
XR7:
		in a,porta
		andi a,$f0
		ori a,$03
		out porta,a
;		cpi comptX,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX,$08
;		rjmp axe_Y


;**************************************************************************
;axe Y
;**************************************************************************

axe_Y:

		sbrc etat,1		; test etatX
		rjmp test_avanceY	;
		sbis pind,3		;test bit avance X à 1
		rjmp axe_X1		; non on passe au suivant
		ori etat,$02	; etatx = 1
		rjmp axe_X1		;

test_avanceY:
		sbic pind,3		;
		rjmp axe_X1		;
		andi etat,$fd	; etatx = 0
		sbis pind,2		; test bit de sens
		rjmp reverse_Y	;

//----------------------------------------------------------------------------
//	avance moteur Y
//----------------------------------------------------------------------------
		
		inc comptY
		cpi comptY,8		; test !!!!!!!!!!!!!!!!!!!!!!!!
		brne avanceY	; test !!!!!!!!!!!!!!!!!!!!!!!
		ldi comptY,$00

avanceY:
		cpi comptY,0
		breq Y1
		cpi comptY,1
		breq Y2
		cpi comptY,2
		breq Y3
		cpi comptY,3
		breq Y4
		cpi comptY,4
		breq Y5
		cpi comptY,5
		breq Y6
		cpi comptY,6
		breq Y7
		cpi comptY,7
;		breq Y8
;		ldi comptY,$00

;envoie sur le port	
		
Y8:
		in a,porta
		andi a,$0f
		ori a,$10
		out porta,a
		rjmp axe_X1
Y7:
		in a,porta
		andi a,$0f
		ori a,$30
		out porta,a
		rjmp axe_X1
Y6:
		in a,porta
		andi a,$0f
		ori a,$20
		out porta,a
		rjmp axe_X1
Y5:
		in a,porta
		andi a,$0f
		ori a,$60
		out porta,a
		rjmp axe_X1
Y4:
		in a,porta
		andi a,$0f
		ori a,$40
		out porta,a
		rjmp axe_X1
Y3:
		in a,porta
		andi a,$0f
		ori a,$c0
		out porta,a
		rjmp axe_X1
Y2:
		in a,porta
		andi a,$0f
		ori a,$80
		out porta,a
		rjmp axe_X1
Y1:
		in a,porta
		andi a,$0f
		ori a,$90
		out porta,a
		rjmp axe_X1

//----------------------------------------------------------------------------
//	recule moteur Y
//----------------------------------------------------------------------------

reverse_Y:

		cpi comptY,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
		brne decY		; test !!!!!!!!!!!!!!!!!!!!!!!
		ldi comptY,$08
		
decY:	dec comptY

		cpi comptY,0
		breq YR1
		cpi comptY,1
		breq YR2
		cpi comptY,2
		breq YR3
		cpi comptY,3
		breq YR4
		cpi comptY,4
		breq YR5
		cpi comptY,5
		breq YR6
		cpi comptY,6
		breq YR7
		cpi comptY,7
;		breq YR8
;		ldi comptY,$00
;		rjmp YR1

;envoie sur le port	
		
YR8:
		in a,porta
		andi a,$0f
		ori a,$10
		out porta,a
;		cpi comptY,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_X1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY,$08
		rjmp axe_X1
YR1:
		in a,porta
		andi a,$0f
		ori a,$90
		out porta,a
;		cpi comptY,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_X1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY,$08
		rjmp axe_X1
YR2:
		in a,porta
		andi a,$0f
		ori a,$80
		out porta,a
;;		cpi comptY,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_X1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY,$08
		rjmp axe_X1
YR3:
		in a,porta
		andi a,$0f
		ori a,$c0
		out porta,a
;		cpi comptY,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_X1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY,$08
		rjmp axe_X1
YR4:
		in a,porta
		andi a,$0f
		ori a,$40
		out porta,a
;		cpi comptY,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_X1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY,$08
		rjmp axe_X1
YR5:
		in a,porta
		andi a,$0f
		ori a,$60
		out porta,a
;		cpi comptY,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_X1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY,$08
		rjmp axe_X1
YR6:
		in a,porta
		andi a,$0f
		ori a,$20
		out porta,a
;		cpi comptY,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_X1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY,$08
		rjmp axe_X1
YR7:
		in a,porta
		andi a,$0f
		ori a,$30
		out porta,a
;		cpi comptY,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_X1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY,$08
;		rjmp axe_X1

		
;**************************************************************************
;axe X1
;**************************************************************************	

axe_X1:

		sbrc etat,2		; test etatX
		rjmp test_avanceX1	;
		sbis pind,5		;test bit avance X à 1
		rjmp axe_Y1		; non on passe au suivant
		ori etat,$04	; etatx = 1
		rjmp axe_Y1		;

test_avanceX1:
		sbic pind,5 	;
		rjmp axe_Y1		;
		andi etat,$fb	; etatx = 0
		sbis pind,4		; test bit de sens
		rjmp reverse_X1	;

//----------------------------------------------------------------------------
//	avance moteur X1
//----------------------------------------------------------------------------
		
		inc comptX1
		cpi comptX1,8		; test !!!!!!!!!!!!!!!!!!!!!!!!
		brne avanceX1	; test !!!!!!!!!!!!!!!!!!!!!!!
		ldi comptX1,$00

avanceX1:
		cpi comptX1,0
		breq X11
		cpi comptX1,1
		breq X12
		cpi comptX1,2
		breq X13
		cpi comptX1,3
		breq X14
		cpi comptX1,4
		breq X15
		cpi comptX1,5
		breq X16
		cpi comptX1,6
		breq X17
		cpi comptX1,7
;		breq X18
;		ldi comptX1,$00

;envoie sur le port	
		
X18:
		in a,portc
		andi a,$0f
		ori a,$10
		out portc,a
		rjmp axe_Y1
X17:
		in a,portc
		andi a,$0f
		ori a,$30
		out portc,a
		rjmp axe_Y1
X16:
		in a,portc
		andi a,$0f
		ori a,$20
		out portc,a
		rjmp axe_Y1
X15:
		in a,portc
		andi a,$0f
		ori a,$60
		out portc,a
		rjmp axe_Y1
X14:
		in a,portc
		andi a,$0f
		ori a,$40
		out portc,a
		rjmp axe_Y1
X13:
		in a,portc
		andi a,$0f
		ori a,$c0
		out portc,a
		rjmp axe_Y1
X12:
		in a,portc
		andi a,$0f
		ori a,$80
		out portc,a
		rjmp axe_Y1
X11:
		in a,portc
		andi a,$0f
		ori a,$90
		out portc,a
		rjmp axe_Y1

//----------------------------------------------------------------------------
//	recule moteur X1
//----------------------------------------------------------------------------

reverse_X1:

		cpi comptX1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
		brne decX1			; test !!!!!!!!!!!!!!!!!!!!!!!
		ldi comptX1,$08
		
decX1:	dec comptX1

		cpi comptX1,0
		breq X1R1
		cpi comptX1,1
		breq X1R2
		cpi comptX1,2
		breq X1R3
		cpi comptX1,3
		breq X1R4
		cpi comptX1,4
		breq X1R5
		cpi comptX1,5
		breq X1R6
		cpi comptX1,6
		breq X1R7
		cpi comptX1,7
;		breq X1R8
;		ldi comptX1,$00
;		rjmp X1R1

;envoie sur le port	
		
X1R8:
		in a,portc
		andi a,$0f
		ori a,$10
		out portc,a
;		cpi comptX1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX1,$08
		rjmp axe_Y1
X1R1:
		in a,portc
		andi a,$0f
		ori a,$90
		out portc,a
;		cpi comptX1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX1,$08
		rjmp axe_Y1
X1R2:
		in a,portc
		andi a,$0f
		ori a,$80
		out portc,a
;		cpi comptX1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX1,$08
		rjmp axe_Y1
X1R3:
		in a,portc
		andi a,$0f
		ori a,$c0
		out portc,a
;		cpi comptX1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX1,$08
		rjmp axe_Y1
X1R4:
		in a,portc
		andi a,$0f
		ori a,$40
		out portc,a
;		cpi comptX1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX1,$08
		rjmp axe_Y1
X1R5:
		in a,portc
		andi a,$0f
		ori a,$60
		out portc,a
;		cpi comptX1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX1,$08
		rjmp axe_Y1
X1R6:
		in a,portc
		andi a,$0f
		ori a,$20
		out portc,a
;		cpi comptX1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX1,$08
		rjmp axe_Y1
X1R7:
		in a,portc
		andi a,$0f
		ori a,$30
		out portc,a
;		cpi comptX1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne axe_Y1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptX1,$08
;		rjmp axe_Y1

;
;**************************************************************************
;axe Y1
;**************************************************************************

axe_Y1:

		sbrc etat,3		; test etatX
		rjmp test_avanceY1	;
		sbis pind,7		;test bit avance X à 1
		rjmp suite1		; non on passe au suivant
		ori etat,$08	; etatx = 1
		rjmp suite1		;

test_avanceY1:
		sbic pind,7		;
		rjmp suite1		;
		andi etat,$f7	; etatx = 0
		sbis pind,6		; test bit de sens
		rjmp reverse_Y1	;

//----------------------------------------------------------------------------
//	avance moteur Y1
//----------------------------------------------------------------------------
		
		inc comptY1
		cpi comptY1,8		; test !!!!!!!!!!!!!!!!!!!!!!!!
		brne avanceY1	; test !!!!!!!!!!!!!!!!!!!!!!!
		ldi comptY1,$00

avanceY1:
		cpi comptY1,0
		breq Y11
		cpi comptY1,1
		breq Y12
		cpi comptY1,2
		breq Y13
		cpi comptY1,3
		breq Y14
		cpi comptY1,4
		breq Y15
		cpi comptY1,5
		breq Y16
		cpi comptY1,6
		breq Y17
		cpi comptY1,7
;		breq Y18
;		ldi comptY1,$00

;envoie sur le port	
		
Y18:
		in a,portc
		andi a,$f0
		ori a,$01
		out portc,a
		rjmp suite1
Y17:
		in a,portc
		andi a,$f0
		ori a,$03
		out portc,a
		rjmp suite1
Y16:
		in a,portc
		andi a,$f0
		ori a,$02
		out portc,a
		rjmp suite1
Y15:
		in a,portc
		andi a,$f0
		ori a,$06
		out portc,a
		rjmp suite1
Y14:
		in a,portc
		andi a,$f0
		ori a,$04
		out portc,a
		rjmp suite1
Y13:
		in a,portc
		andi a,$f0
		ori a,$0c
		out portc,a
		rjmp suite1
Y12:
		in a,portc
		andi a,$f0
		ori a,$08
		out portc,a
		rjmp suite1
Y11:
		in a,portc
		andi a,$f0
		ori a,$09
		out portc,a
		rjmp suite1

//----------------------------------------------------------------------------
//	recule moteur Y1
//----------------------------------------------------------------------------

reverse_Y1:

		cpi comptY1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
		brne decY1			; test !!!!!!!!!!!!!!!!!!!!!!!
		ldi comptY1,$08

decY1:	dec comptY1

		cpi comptY1,0
		breq Y1R1
		cpi comptY1,1
		breq Y1R2
		cpi comptY1,2
		breq Y1R3
		cpi comptY1,3
		breq Y1R4
		cpi comptY1,4
		breq Y1R5
		cpi comptY1,5
		breq Y1R6
		cpi comptY1,6
		breq Y1R7
		cpi comptY1,7
;		breq Y1R8
;		ldi comptY1,$00
;		rjmp Y1R1

;envoie sur le port	
		
Y1R8:
		in a,portc
		andi a,$f0
		ori a,$01
		out portc,a
;		cpi comptY1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne suite1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY1,$08
		rjmp suite1
Y1R1:
		in a,portc
		andi a,$f0
		ori a,$09
		out portc,a
;		cpi comptY1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne suite1		; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY1,$08
		rjmp suite1
Y1R2:
		in a,portc
		andi a,$f0
		ori a,$08
		out portc,a
;		cpi comptY1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne suite1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY1,$08
		rjmp suite1
Y1R3:
		in a,portc
		andi a,$f0
		ori a,$0c
		out portc,a
;		cpi comptY1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne suite1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY1,$08
		rjmp suite1
Y1R4:
		in a,portc
		andi a,$f0
		ori a,$04
		out portc,a
;		cpi comptY1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne suite1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY1,$08
		rjmp suite1
Y1R5:
		in a,portc
		andi a,$f0
		ori a,$06
		out portc,a
;		cpi comptY1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne suite1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY1,$08
		rjmp suite1
Y1R6:
		in a,portc
		andi a,$f0
		ori a,$02
		out portc,a
;		cpi comptY1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne suite1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY1,$08
		rjmp suite1
Y1R7:
		in a,portc
		andi a,$f0
		ori a,$03
		out portc,a
;		cpi comptY1,0		; test !!!!!!!!!!!!!!!!!!!!!!!!
;		brne suite1			; test !!!!!!!!!!!!!!!!!!!!!!!
;		ldi comptY1,$08
;		rjmp suite1

		
		
suite1:		



		rjmp jedicut1

;*************
; MODE IPL5X *
;*************
;
; mode où la gestion des moteurs est optimisé.

ipl5x :

; buzz 2 fois

;		rcall buzzer03
;		rcall tempo
;		rcall buzzer03

		rjmp ipl5x

;*************
; MODE EMC2  *
;*************
; 
; mode test avec gestion de la chauffe en fonction de la vitesse des moteurs.

emc2 :

; buzz 3 fois

;		rcall buzzer03
;		rcall tempo
;		rcall buzzer03
;		rcall tempo
;		rcall buzzer03
		
		rjmp emc2

	
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Sous-Programmes
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

;-------
;buzzer
;-------

; buzzer pendant 1/8 de seconde 

buzzer:
		ldi a,$02
		out tccr2,a
		ldi a,(1<<toie2|0<<toie0)
		out timsk,a
		ldi sauvreg,$0a
		ldi a,$ff

buzz:	nop
		nop
		nop
		in b,timsk
		sbrc b,6
		rjmp buzz
		ret

;------------------
; LECTRURE EEPROM
;------------------

EEPROM_write: 

		sbic EECR,EEWE ; Test si l’écriture est terminé
		rjmp EEPROM_write ; Boucle si pas terminé
		out EEARH, r28 ; Déclare l’adresse (r18:r17)
		out EEARL, r29
		out EEDR,a ; Ecrire la donnée (r16) dans le registre de donnée
		sbi EECR,EEMWE ; Ecrire un 1 dans EEMWE
		sbi EECR,EEWE ; Départ de l’écriture dans EEPROM avec EEWE
		ret ; Fin de la procédure d’écriture




EEPROM_read: 
						
		Sbic EECR,EEWE ; Test si l’écriture est terminée
		Rjmp EEPROM_read ; Boucle si pas terminé
		Out EEARH, r28 ; Déclare l’adresse (r18:r17)
		Out EEARL, r29
		Sbi EECR,EERE ; Départ de la lecture dans EEPROM avec EERE
		in a,EEDR ; Lire la donnée dans le registre
		ret ; Fin de la procédure de lecture

		

