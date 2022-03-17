;************************************************************************
;* Fichier : p�dalier_orgue.asm							                *
;* Titre: emition de donn�es midi pour pedalier d'orgue, 32 notes	    *
;* Version: 1.0					                	                    *
;* Auteur: ludomercet                 				                	*
;* Date: 11/11/2007	de cr�ation				        	                *
;* Date: 24/03/2008 ok pour g�n�ration du fichier .hex					*
;* Date: 09/11/2008 fichier en theorie ok								*
;* date: 18/12/2008 la partie multiplexage marche apr�s 1 semaine de	*
;* 					tests et mise au point								*
;* date: 26/12/2008 tout est ok emmission midi							*
;*								                                    	*	
;* utilisation de l'uart pour emition midi ; avec multiplexeur			*
;* pour le balayage des notes du p�dalier, choix du canal avec roue	    *
;* codeuses, midi in avec uart renvoyer directement en midi out 	    *
;*									                                    *
;************************************************************************


.include "tn2313def.inc"	; nom du fichiers de r�f�rences des registres


;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;D�claration des Variables		                 	
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

	.def canal =r10			; registre du canal midi
	.def a =R16				; registre �quivalent � accumulateur (X51)
	.def sauvreg = R17		; registre de sauvegarde de SREG
    .def flags = R18		; registres de flag
    .def rtm = R19			; valeurs des notes
	.def note =R20			; registre de note 
	.def b = R21			; registre d'usage

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;D�claration des zones de variables dans la sram
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.DSEG
.ORG 0x0060
midrecin: 	.BYTE 3			; zone de 3 octets pour valeur uart midi in
midrecout:	.BYTE 3			; zone de 3 octets pour valeur uart p�dalier
valeurnote:	.BYTE 4			; zone de 4 octets pour valeur de chaques notes

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;D�claration des Vecteurs d'interruptions
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.CSEG
.ORG 0x0000			;d�but zone m�moire programme
	rjmp	RESET		;RESET
	rjmp	EXT_INT0	;Interruption externe INT0
	rjmp 	EXT_INT1	;Interruption externe INT1
    rjmp 	TIM_CAPT1	;Interrution entr�e capture TIMER1
	rjmp	TIM_COMP1A	;Interrution comparateurA TIMER1
	rjmp	TIM_OVF1	;Interrution �bordement compteur TIMER1
	rjmp 	TIM_OVF0	;Interrution �bordement compteur TIMER0
	rjmp 	UART_RXC	;Interrution r�ception UART termin� RX
	rjmp 	UART_DRE	;Interrution UART vide
	rjmp 	UART_TXC	;Interrution �mission UART termin� TX
	rjmp 	ANA_COMP	;Interrution comparaison analogique effectu�e
	rjmp	PCINT		;Interruption sur  pin 
	rjmp 	TIM_COMP1B	;Interruption comparateurB Timer 1
	rjmp	TIM_COMP0A	;Interruption comparateurA timer0
	rjmp 	TIM_COMP0B	;Interruption comparateurB timer0
	rjmp	USI_START	;Interruption usi start condition
	rjmp 	USI_OVER	;Interruption debordement usi
	rjmp 	EE_READY	;Interruption eeprom pr�te
	rjmp	WDT_OVER	;Interruption watchdog debordement timer
	
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Routines de traitement des interruptions
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


EXT_INT0:   reti			; fin de l'interruption			
EXT_INT1:	reti			
TIM_CAPT1:	reti	
TIM_COMP1A:	reti
TIM_OVF1:	reti

;----------------------------
; interruption �mission midi 
;----------------------------

	
TIM_OVF0:	in sauvreg,sreg		; sauvegarde de sreg
		    push sauvreg		; sreg est pouss� dans la pile
			push note			; note est sur la pile
			push b			; b est pouss� sur la pile
		    wdr
            

			;sbi portb,3								; ���!!!!! essai!!!!!
			out udr,a			; emmetre valeur canal + note on
at1:		sbis ucsra,udre		; test si transmission r�alis�
			rjmp at1			; non on attend 
			out udr,note		; emmettre la note 
at2:		sbis ucsra,udre		; test si valeur emise
			rjmp at2			; non on attend
			ldi b,$7f			;
			out udr,b			; emmetre velocit�
at3:		sbis ucsra,udre		; test si valeur �mise
			rjmp at3			; non on attend

transmidiin:
			sbrs flags,4		; test si il faut envoyer une donn�e midi in 
			rjmp sortirtx		; non on sort
			ldi r26,midrecin 	; chargement addresse variable !!!!!ATTENTION !!! c'est pas bon 

			rcall transmission	; premiere valeur envoie
			rcall transmission	; deuxieme valeur envoie
			rcall transmission 	; troisieme valeur envoie
			andi flags,$f8		; remise � 0 des flags de passages
	

sortirtx :             
            ldi b,$00
			out timsk,b		 ;eteint la transmition
			sbis portb,3;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			rjmp allum
			cbi portb,3		; etteint la led !!!!!!$$$$$$$!!!!!!!
			rjmp sortirled
allum :		sbi portb,3;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

sortirled :
			pop b 			; b est restitu�
			pop note		; note est restitu�
		    pop sauvreg		; sauvreg est restitu�
		    out sreg,sauvreg 	; sreg est remis dans son contexte (avant interruption)

			reti			; fin de l'interruption

;----------------------------
; interruption r�ception midi
;----------------------------
			
UART_RXC:	in sauvreg,sreg		; sauvegarde de sreg
		    push sauvreg		; sreg est pouss� dans la pile
		    push a				; a est pouss� sur la pile
								; le registre temporaire midi est sauv�
		    wdr
            
            in a,udr			; chargement de la valeur contenu dans le registre
            sbrc flags,1		; test si 1ere valeur que je doit recevoir 
            rjmp deuemevaleur	; non on passe � la 2eme valeur
			tst	a				; oui test si la valeur est = � 0
			breq sortirrx		; oui : on sort
			ldi r26,midrecin	; non : chargement de l'addresse ram de sauvegarde des donn�es midi r�ception
            st x+,a			  	; charge la valeur dans registre temporaire 1ere valeur
            sbr flags,1			; met le flag 1ere valeur � 1
            rjmp sortirrx		; on sort

deuemevaleur:

            sbrc flags,2			; test si 2eme valeur que je doit recevoir si non saute � 3eme valeur
			rjmp troisiemevaleur	; non on passe � la troisieme valeur
			st x+,a					; rec 
            sbr flags,2				; met le flag 2eme valeur � 1
			rjmp sortirrx			; on sort

troisiemevaleur: 
			
			st x,a				; rec de la troisieme valeur

			sbis ucsrb,txen		; test si transmision en cour
			rjmp trans			; non on l'�ffectue de suite
			sbr flags,4			; oui on met qu'il faut l'�ffectuer le plus vite possible
			rjmp sortirrx		; on sort
trans: 
			ldi r26,midrecin	; chargement addresse variable
			sbi ucsrb,txen		; mise en route de la transmission

			rcall transmission	; premiere valeur envoie
			rcall transmission	; deuxieme valeur envoie
			rcall transmission 	; troisieme valeur envoie

			cbi ucsrb,txen		; oui on etteind la transmission
			andi flags,$f8		; remise � 0 des flags de passage

sortirrx:              
								; le registre temporaire est restitu�
            pop a				; a est restitu�
		    pop sauvreg			; sauvreg est restitu�
		    out sreg,sauvreg 	; sreg est remis dans son contexte (avant interruption)
            reti

UART_DRE:   reti				; fin d'interruption 		
UART_TXC:	reti
ANA_COMP:	reti
PCINT:		reti
TIM_COMP1B:	reti 
TIM_COMP0A:	reti
TIM_COMP0B:	reti
USI_START:	reti
USI_OVER:	reti
EE_READY:	reti
WDT_OVER:	reti				; fin d'interrution

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Programme de RESET
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

; ATTENTION !!!
; si utilisation d'un quartz : v�rifier les flags au chargement du 
; programme dans la puce !

RESET:		ldi	a,low(RAMEND)
			out	SPL,a		; Initialisation de la pile � l'�tat bas

; initialisation des port 
;-------------------------   
		    ldi a,0b00000010    ; port d en in/out avec uart tx en out
								; d7 nc existe pas
								; d6 entr�e des multiplexeurs
								; d5 entr�e | haute 
								; d4 entr�e | canal midi
								; d3 entr�e |
								; d2 entr�e | basse
								; d1 sortie emission des donn�es midi
								; d0 entr�e r�ception des donn�es midi
		    out ddrd,a			; 
		    ldi a,$3c			; port d resistence de rapel
		    out portd,a      	; 
			ldi a,0b11111111	; port b en out 
								; b7 sortie s�lection du 1er multiplexeur
								; b6 sortie s�lection du 2eme multiplexeur
								; b5 sortie s�lection du 3eme multiplexeur
								; b4 sortie s�lection du 4eme multiplexeur
								; b3 nc n'est pas utilis�
								; b2 sortie addresse des entr�e du multiplexeur
								; b1 sortie addresse des entr�e du multiplexeur 
								; b0 sortie addresse des entr�e du multiplexeur
			out ddrb,a			;
			ldi a,$f0			; multiplexeur en off et addresse � 0
			out portb,a			; 

; remise � 0 des espace ram
;--------------------------
			clr flags
			ldi a,$00			; clear de la m�moire ram
			ldi rtm,$04
			ldi r26,valeurnote
efface:
			st x+,a				; effacement des valeurs note
			dec rtm
			brne efface
			ldi r26,midrecin		; chargement de l'addresse
			st x+,a
			st x+,a
			st x+,a

; initialisation uart
;--------------------
			ldi a,(3<<UCSZ0)	; 8 bits de donn�es
			out ucsrc,a			; 
			ldi a,$00			; vitesse de transmission (31250 bauds = 31 avec 16mhz)
			out ubrrh,a			;
			ldi a,$1f			; 
			out ubrrl,a			;
			ldi a,(1<<RXCIE)|(1<<TXEN)|(1<<RXEN)
			out ucsrb,a			; interruption uart r�ception et �mition en route avec 
								; mise en route de la r�ception et emission
			ldi a,$01			; initialisation timer0
			out tccr0b,a		;	

; valeur du canal
;----------------
			in a,pind			; chargement de la valeur
			lsr a				; 
			lsr	a				;
			ldi b,$FF			; inverser la valeur
			eor a,b				;
			andi a,$0f			; effac� les valeur non d�sir� 
			mov canal,a			; enregistr� le canal 
	
			wdr			; initialisation le wachtdog tout les 0.03s
			ldi a,$09		; reset tout les 0.03s si non raffraichi
			out wdtcr,a
	
			sei			; autorisation interruption general
			rjmp debu


;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Programme principal
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

debu:	wdr

			ldi r30,valeurnote		; charger adresse des valeurs de note en ram
			ld rtm,Z				;
			ldi note, $23			; charger note de d�part do 2 lignes en dessous de la cl� de fa

    		cbi portb,7		; valider 1er multiplexeur
			rcall analyse8	; lire toutes les valeurs 
			sbi portb,7		; etteindre le 1er multiplexeur

			st Z+,rtm		; enregistr� les nouvelles valeur de note
			ld rtm,Z		; charger les suivantes
	

		wdr
			cbi portb,6		; valid� le deuxieme multiplexeur
			rcall analyse8	; lire toutes les valeurs
			sbi portb,6		; etteindre le 2eme multiplexeur

			st Z+,rtm		; enregistr� les nouvelles valeur de note
			ld rtm,Z		; charger les suivantes
			
		wdr
			cbi portb,5		; valid� le 3eme multiplexeur
			rcall analyse8	; lire toutes les valeurs 
			sbi portb,5		; etteindre le 3eme multiplexeur

			st Z+,rtm		; enregistr� les nouvelles valeur de note
			ld rtm,Z		; charger les suivantes
			
		wdr

			cbi portb,4		; valid� le 4eme multiplexeur
			rcall analyse8	; lire toutes les valeurs 
			sbi portb,4		; etteindre le 4eme multiplexeur

			st Z,rtm		; enregistr� les derni�res valeurs

   		wdr					; reset w
    
;valeur canal
;------------

			in a,pind			; chargement de la valeur
			lsr a				; 
			lsr	a				;
			ldi b,$FF			; invers� la valeur
			eor a,b				;
			andi a,$0f			; effac� les valeur non d�sir� 
			mov canal,a			; enregistr� le canal 

	rjmp debu	
	
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Sous-Programmes
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

;-------------
;delai de 20ns
;-------------

delay20:
			ldi a,12			; delay de 20 ns
del:			dec a			; 
			brne del			;
			ret					; retour

;-----------
;envoie note
;-----------

noteon:		
			ldi a,$90			; valeur note on
			add a,canal			; charger canal + note on
			ldi b,$fd			; 
			out tcnt0,b			; chargement du compteur 8 bit
			ldi b,$02
			out timsk,b			; declencher interruption 
			nop					; attendre l'interruption
			nop
			ret

noteoff:	
			ldi a,$80
			add a,canal			; charger canal + note off 
			ldi b,$fd			; 
			out tcnt0,b			; chargement du compteur 8 bit
			ldi b,$02
			out timsk,b			; declencher interruption 
			nop					; attendre l'interruption
			nop
			ret
			
 
transmission: 				
			ld a, x+			; valeur dans le registre d'envoie
			out udr,a			;  
tx1:		sbis ucsra,udre		; test si transmission r�alis�
            rjmp tx1			; non on attend
			ret 				; retour de sous routine


;------------------------------------
;analyse des 8 entr�es des 74hct151 
;------------------------------------

analyse8:
			inc note			; incr�ment� compteur de note
			ldi a,$00 			; 
			in	b,pinb			; charger la valeur du portb
			andi b,$f8			; effacer les valeurs basse
			add a,b				; m�langer les 2 valeurs 
			out portb,a			; charger 1ere addresse 
			rcall delay20		; attendre
			sbic pind,6			; test la valeur re�u, si 0 on saute
			rjmp flagoff0		;
			sbrs rtm,0			; test valeur enregistr�, si 1 on saute
			rjmp setflag0		;
			rjmp cont1			; la valeur n'a pas chang�e.

flagoff0:	
			sbrc rtm,0			; test le valeur enregisrt�, si 0 on saute
			rjmp clearflag0		; la valeur a chang�e, c'est une note off
			rjmp cont1			; la valeur n'a pas chang�e.

setflag0:
			sbr rtm,1			; mettre note on dans le registre
			rcall noteon		; envoyer la note on
			rjmp cont1			; au suivant !

clearflag0: cbr rtm,1			; mettre note off dans le registre 
			rcall noteoff		; envoyer la note off
			
;---------


cont1:
			inc note			; incr�ment� compteur de note
			ldi a,$01 			; 
			in b,pinb			; charger la valeur du port
			andi b,$f8			; effacer les valeurs basse
			add a,b				; m�langer les 2 valeurs
			out portb,a			; charger 2eme addresse 
			rcall delay20		; attendre
			sbic pind,6			; test la valeur re�u, si 0 on saute
			rjmp flagoff1		;
			sbrs rtm,1			; test valeur enregistr�, si 1 on saute
			rjmp setflag1		;
			rjmp cont2			; la valeur n'a pas chang�e.

flagoff1:
			sbrc rtm,1			; test le valeur enregisrt�, si 0 on saute
			rjmp clearflag1		; la valeur a chang�e, c'est une note off 
			rjmp cont2			; la valeur n'a pas chang�e.

setflag1:	sbr rtm,2			; mettre note on dans le registre
			rcall noteon		; envoyer la note on
			rjmp cont2			; au suivant !

clearflag1: cbr rtm,2			; mettre note off dans le registre 
			rcall noteoff		; envoyer la note off

;---------


cont2:
			inc note			; incr�ment� compteur de note
			ldi a,$02 			; 
			in b,pinb			; charger la valeur du port
			andi b,$f8			; effacer les valeurs basse
			add a,b				; m�langer les 2 valeurs 
			out portb,a			; charger 2eme addresse 
			rcall delay20		; attendre
			sbic pind,6			; test la valeur re�u, si 0 on saute
			rjmp flagoff2		;
			sbrs rtm,2			; test valeur enregistr�, si 1 on saute
			rjmp setflag2		;
			rjmp cont3			; la valeur n'a pas chang�e.

flagoff2:
			sbrc rtm,2			; test le valeur enregisrt�, si 0 on saute
			rjmp clearflag2		; la valeur a chang�e, c'est une note off 
			rjmp cont3			; la valeur n'a pas chang�e.

setflag2:	sbr rtm,4			; mettre note on dans le registre
			rcall noteon		; envoyer la note on
			rjmp cont3			; au suivant !

clearflag2: cbr rtm,4			; mettre note off dans le registre 
			rcall noteoff		; envoyer la note off

;---------


cont3:
			inc note			; incr�ment� compteur de note
			ldi a,$03 			;
			in b,pinb			; charger la valeur du port
			andi b,$f8			; effacer les valeurs basse
			add a,b				; m�langer les 2 valeurs 			 
			out portb,a			; charger 2eme addresse 
			rcall delay20		; attendre
			sbic pind,6			; test la valeur re�u, si 0 on saute
			rjmp flagoff3		;
			sbrs rtm,3			; test valeur enregistr�, si 1 on saute
			rjmp setflag3		;
			rjmp cont4			; la valeur n'a pas chang�e.

flagoff3:
			sbrc rtm,3			; test le valeur enregisrt�, si 0 on saute
			rjmp clearflag3		; la valeur a chang�e, c'est une note off 
			rjmp cont4			; la valeur n'a pas chang�e.

setflag3:	sbr rtm,8			; mettre note on dans le registre
			rcall noteon		; envoyer la note on
			rjmp cont4			; au suivant !

clearflag3: cbr rtm,8			; mettre note off dans le registre 
			rcall noteoff		; envoyer la note off

;---------


cont4:
			inc note			; incr�ment� compteur de note
			ldi a,$04 			; 
			in b,pinb			; charger la valeur du port
			andi b,$f8			; effacer les valeurs basse
			add a,b				; m�langer les 2 valeurs 
			out portb,a			; charger 2eme addresse 
			rcall delay20 		; attendre
			sbic pind,6			; test la valeur re�u, si 0 on saute
			rjmp flagoff4		;
			sbrs rtm,4			; test valeur enregistr�, si 1 on saute
			rjmp setflag4		;
			rjmp cont5			; la valeur n'a pas chang�e.

flagoff4:
			sbrc rtm,4			; test le valeur enregisrt�, si 0 on saute
			rjmp clearflag4		; la valeur a chang�e, c'est une note off 
			rjmp cont5			; la valeur n'a pas chang�e.

setflag4:	sbr rtm,16			; mettre note on dans le registre
			rcall noteon		; envoyer la note on
			rjmp cont5			; au suivant !

clearflag4: cbr rtm,16			; mettre note off dans le registre 
			rcall noteoff		; envoyer la note off

;---------


cont5:
			inc note			; incr�ment� compteur de note
			ldi a,$05 			; 
			in b,pinb			; charger la valeur du port
			andi b,$f8			; effacer les valeurs basse
			add a,b				; m�langer les 2 valeurs 
			out portb,a			; charger 2eme addresse 
			rcall delay20 		; attendre
			sbic pind,6			; test la valeur re�u, si 0 on saute
			rjmp flagoff5		;
			sbrs rtm,5			; test valeur enregistr�, si 1 on saute
			rjmp setflag5		;
			rjmp cont6			; la valeur n'a pas chang�e.

flagoff5:
			sbrc rtm,5			; test le valeur enregisrt�, si 0 on saute
			rjmp clearflag5		; la valeur a chang�e, c'est une note off 
			rjmp cont6			; la valeur n'a pas chang�e.

setflag5:	sbr rtm,32			; mettre note on dans le registre
			rcall noteon		; envoyer la note on
			rjmp cont6			; au suivant !

clearflag5: cbr rtm,32			; mettre note off dans le registre 
			rcall noteoff		; envoyer la note off

;---------


cont6:
			inc note			; incr�ment� compteur de note
			ldi a,$06 			; 
			in b,pinb			; charger la valeur du port
			andi b,$f8			; effacer les valeurs basse
			add a,b				; m�langer les 2 valeurs 
			out portb,a			; charger 2eme addresse 
			rcall delay20 		; attendre 
			sbic pind,6			; test la valeur re�u, si 0 on saute
			rjmp flagoff6		;
			sbrs rtm,6			; test valeur enregistr�, si 1 on saute
			rjmp setflag6		;
			rjmp cont7			; la valeur n'a pas chang�e.

flagoff6:
			sbrc rtm,6			; test le valeur enregisrt�, si 0 on saute
			rjmp clearflag6		; la valeur a chang�e, c'est une note off 
			rjmp cont7			; la valeur n'a pas chang�e.

setflag6:	sbr rtm,64			; mettre note on dans le registre
			rcall noteon		; envoyer la note on
			rjmp cont7			; au suivant !

clearflag6: cbr rtm,64			; mettre note off dans le registre 
			rcall noteoff		; envoyer la note off

;---------


cont7:
			inc note			; incr�ment� compteur de note
			ldi a,$07 			;
			in b,pinb			; charger la valeur du port
			andi b,$f8			; effacer les valeurs basse
			add a,b				; m�langer les 2 valeurs  
			out portb,a			; charger 2eme addresse 
			rcall delay20		; attendre
			sbic pind,6			; test la valeur re�u, si 0 on saute
			rjmp flagoff7		;
			sbrs rtm,7			; test valeur enregistr�, si 1 on saute
			rjmp setflag7		;
			ret					; la valeur n'a pas chang�e.

flagoff7:
			sbrc rtm,7			; test le valeur enregisrt�, si 0 on saute
			rjmp clearflag7		; la valeur a chang�e, c'est une note off 
			ret					; la valeur n'a pas chang�e.

setflag7:	sbr rtm,128			; mettre note on dans le registre
			rcall noteon		; envoyer la note on
			ret					; au suivant !

clearflag7: cbr rtm,128			; mettre note off dans le registre 
			rcall noteoff		; envoyer la note off
			ret					; fin de la sous routine

			
