

; Manip_Fusibles.asm

; Lecture et programmation bits fusibles ATmega168
; en mode High-Voltage //
; pour récupération µC mal programmé au niveau des fusibles
; et irrécupérable par autre procédé.

; Inspiré du Firmware STK500 d'ATMEL

; µC maître = ATmega16
; Quartz 14,7456 MHz


; durant le mode "programmation // haute-tension" , la fonction
; et le nom des pines du µC en programmation sont changés:
;     (description ici du cas de l'ATmega168)
; le PORTD1 devient le signal sortant "RDY/Not_BSY"
; le PORTD2 devient le signal entrant "Not_OE"
; le PORTD3 devient le signal entrant "Not_WR"
; le PORTD4 devient le signal entrant "BS1"
; le PORTD5 devient le signal entrant "XA0"
; le PORTD6 devient le signal entrant "XA1"
; le PORTD7 devient le signal entrant "PAGEL"
; le PORTC2 devient le signal entrant "BS2"
; le PORTB (bits 0 à 5) devient une partie du bus "DATA" (6 bits bas)
; les PORTC0 et PORTC1 deviennent une partie du bus "DATA" (2 bits haut)
; la pine XTAL1 reçoit le Clock spécifiquement généré par ce firmware
; la pine RESET est pilotée en 12V (11V5 à 12V5 en réalité) par le firmware



; RESULTAT de l'essais du montage ce 28/04/2008:
; succès total !!! ==> le problème du µC était une mise à "0" de tous les bits du Fuse HIGH
; lecture Fuses LOW      = 0b11100010 au lieu de 0b01100010 (corrects , seul divis par 8 du Clock enlevé)
;         Fuses HIGH     = 0b00000000 au lieu de 0b11011111
;                          problèmes:
							;External RESET Disabled
							;debugWIRE Enabled
							;Watchdog Timer is Always ON
							;EEPROM Memory preserved through Chip Erase
							;Brown-Out Detector Trigger Level
;         Fuses EXTENDED = 0b11111001 (ceux-ci sont OK)



; =========================================================

.include "m16def.inc"

; =========================================================


		.equ	StabDelay1 = 100		; Délais (en µs) utilisé pour stabilisation des pines
		.equ	StabDelay2 = 15			; Délais (en ms) utilisé pour stabilisation des pines
		.equ	ProgModeDelay = 0		; Délais (en µs) en relation avec l'exécution de la commande
										;  "entrée en mode programmation"
		.equ	LatchCycles = 5			; (Nbre de cycles Xtal utilisés pour stocker OSCCAL)
		.equ	PowerOffDelay = 15		; Délais additionnel (en ms) après Vtg coupé pour être
										;  certain que Vtg est suffisamment descendu
		.equ	PowerOnDelay = 15		; Délais additionnel (en ms) après Vtg enclenché pour être
										;  certain que Vtg est suffisamment grimpé
		.equ	ResetDelayMs = 1		; (RSTDELAY #1 (en ms).Délais additionnel après que Vtg est mis
										;  ON et RESET passe à "1")
		.equ	ResetDelayUs = 0		; (RSTDELAY #2 (en µs*10).Délais additionnel après que Vtg est mis
										;  ON et RESET passe à "1")
										; NOTE: le délais total vaut RSTDELAY #1 (ms) + RSTDELAY #2 (µs*10)
		.equ	PulseWidth = 0			; Durée supplémentaire (en ms) des pulses d'écriture
		.equ	PollTimeout = 5 * 10	; Temps autorisé (exprimé en µs*100) pour retour de RDY/Not_BSY
		.equ	ResetDelay = 15			; délais (en ms) pour placer RESET bas


		; Valeurs par défaut (sortie d'usine) des Fusibles de l'ATmega168:
		.equ	ValDefaultFuseLow		=	0b01100010
		.equ	ValDefaultFuseHigh		=	0b11011111
		.equ	ValDefaultFuseExtended	=	0b11111001


		.equ	Cmde_RD_FUSES	= 0b00000100	; Byte de Cmde "READ LOCK & FUSE BITS"
		.equ	Cmde_WR_FUSES	= 0b01000000	; Byte de Cmde "WRITE FUSE BITS"


		; Messages envoyés vers port série
		.equ	CMD_OK				= 0
		.equ	RDY_BSY_TimerOUT	= 0x81



		; Bits du Port de CTRL:
		.equ	CTRL_PORT	= PORTC
		.equ	CTRL_P_INPUT= PINC
		.equ	DDR_CTRL_P	= DDRC
		.equ	BS2			= 0		; sortie
		.equ	RDY_NotBSY	= 1		; entrée
		.equ	Not_OE		= 2		; sortie
		.equ	Not_WR		= 3		; sortie
		.equ	BS1			= 4		; sortie
		.equ	XA0			= 5		; sortie
		.equ	XA1			= 6		; sortie
		.equ	PAGEL		= 7		; sortie

		; Port de Données:
		.equ	DATA_PORT	= PORTB
		.equ	DATA_P_INPUT= PINB
		.equ	DDR_DATA_P	= DDRB

		; Bits du PORT de Cmde RESET , Clock et LED bicolore:
		.equ	CONTRL_PORT	= PORTD
		.equ	DDR_CONTRL_P= DDRD
		.equ	CLK			= 7		; sortie
		.equ	Anode_LED_V	= 6		; sortie Anode LED verte (LED bicolore)
		.equ	Anode_LED_R	= 5		; sortie Anode LED rouge (LED bicolore)
		.equ	ALIM_Target	= 4		; sortie pour cmde relais Alim µC Target
		.equ	HV_RST		= 3		; sortie
		.equ	RESET		= 2		; sortie


		.equ CR = 0x0D
		.equ LF = 0x0A

		.equ	FOsc	=	14745600
		.equ	Baud	=	19200

; =========================================================

	; Macro générant des temps d'attente en µs:
	; usage "_Waiste_us n" (n est le nbre de µs à temporiser)


				.Macro _Waiste_us 
					.set	Fraction = @0*FOsc/1000000
					.set 	Fraction = Fraction * 100
					.set	Fraction = @0*FOsc/10000 - Fraction

					.if		Fraction >= 50
						.set 	Cycles_Needed = (@0 * FOsc /1000000)+1
					.endif
					.if		Fraction < 50
						.set 	Cycles_Needed = (@0 * FOsc /1000000)
					.endif
					_Cycle_Waister Cycles_Needed
				.endmacro

                                    
				.macro _Cycle_Waister
					.If @0 == 1
						nop
					.EndIf
					.If @0 == 2
						nop
						nop
					.EndIf
					.If ((@0 > 2) && (@0 < 769))
						;C=3n
						;769=(3 * (2^8)) + 1
						.set	Loops_Needed = @0/3
						ldi		ZL,Low(Loops_Needed)
						Loop:
						dec		ZL
						brne	Loop
						.set	Cycles_Left_To_Do = @0 - (3*Loops_Needed)
						_Cycle_Waister Cycles_Left_To_Do
					.EndIf
					.If (@0 > 768) && (@0 < 262146)
						;C=1+4n
						;262146 = 1 + (4 * 2^16) + 1
						.set	Loops_Needed = (@0 - 1)/4
						ldi		ZL,Low(Loops_Needed)
						ldi		ZH,High(Loops_Needed)
						Loop:	
						sbiw	ZL,1
						brne	Loop
						.set Cycles_Left_To_Do = (@0 - (4*Loops_Needed))-1
						_Cycle_Waister Cycles_Left_To_Do
					.EndIf
					.If (@0 > 262145) && (@0 < 83886083)
						;C=2+5n
						;83.886.083 = 2 + (5 * 2^24) +1
						.set	Loops_Needed = (@0 - 2)/5
						ldi		XL,Low(Loops_Needed)
						ldi		ZL,Byte2(Loops_Needed)
						ldi		ZH,Byte3(Loops_Needed)
						Loop:
						subi	XL,1
						sbci	ZL,0
						sbci	ZH,0
						brne	Loop
						.set Cycles_Left_To_Do = (@0 - (5*Loops_Needed))-2
						_Cycle_Waister Cycles_Left_To_Do
					.EndIf
					.If (@0 > 83886082) && (@0 < 25769803780)
						;C=3+6n
						;25769803780 = 3 + (6 * 2^32) +1
						.set	Loops_Needed = (@0 - 3)6
						ldi		XL,Low(Loops_Needed)
						ldi		XH,Byte2(Loops_Needed)
						ldi		ZL,Byte3(Loops_Needed)
						ldi		ZH,Byte4(Loops_Needed)
						Loop:
						sbiw	XL
						sbci	ZL,0
						sbci	ZH,0
						brne	Loop
						.set Cycles_Left_To_Do = (@0 - (6*Loops_Needed)) - 3
						_Cycle_Waister Cycles_Left_To_Do
					.endif
				.endmacro

; =========================================================

		.cseg
		.org	0


		ldi		R16, low(RAMEND)	; ; initialisation
		out		SPL, R16			; ; du pointeur
		ldi		R16, high(RAMEND)	; ; de pile
		out		SPH, R16			; ;

		ldi		R16, 0<<CLK | 0<<Anode_LED_V | 0<<Anode_LED_R | 0<<ALIM_Target | 0<<HV_RST | 1<<RESET
		out		CONTRL_PORT, R16	; 
		ldi		R16, 1<<CLK | 1<<Anode_LED_V | 1<<Anode_LED_R | 1<<ALIM_Target | 1<<HV_RST | 1<<RESET
		out		DDR_CONTRL_P, R16	; 

		rcall	_LED_Verte			; éclairer VERTE LED bicolore (sécurité d'insertion µC Target)

		rcall	_INIT_RS232			; Initialiser interface RS232 à 19200Bauds

		_Waiste_us	500000			; Temporiser 0,5 sec

; =========================================================

_PT_BCLE1:
		ldi		ZH, high(_TEXTE_CRLF * 2)
		ldi		ZL, low(_TEXTE_CRLF * 2)
		rcall	_SEND_TEXT_RS232		; 

		ldi		ZH, high(_TEXTE1 * 2)	; ; Texte "Lecteur/Programmateur de Fusibles ATmega168 (mode //)"
		ldi		ZL, low(_TEXTE1 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		ldi		ZH, high(_TEXTE3 * 2)	; ; Texte "Faites votre choix ci-dessous:"
		ldi		ZL, low(_TEXTE3 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		ldi		ZH, high(_TEXTE2 * 2)	; ; Texte "1- alimentation TARGET (entree en mode programmation)"
		ldi		ZL, low(_TEXTE2 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		rcall	_GET_FROM_RS232			; ;
		cpi		R16, '1'				; ; Chercher touche "1" pressée
		brne	PC - 2					; ; (boucler en attendant)

		rcall	_INPUT_PROGMODE			; entrée en mode programmation (mise sous-tension µC TARGET)

_PT_BCLE2:
		ldi		ZH, high(_TEXTE_CRLF * 2)
		ldi		ZL, low(_TEXTE_CRLF * 2)
		rcall	_SEND_TEXT_RS232		; 

		ldi		ZH, high(_TEXTE1 * 2)	; ; Texte "Lecteur/Programmateur de Fusibles ATmega168 (mode //)"
		ldi		ZL, low(_TEXTE1 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		ldi		ZH, high(_TEXTE3 * 2)	; ; Texte "Faites votre choix ci-dessous:"
		ldi		ZL, low(_TEXTE3 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		ldi		ZH, high(_TEXTE4 * 2)	; ; Texte "1- Lecture des Fusibles LOW"
		ldi		ZL, low(_TEXTE4 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		ldi		ZH, high(_TEXTE5 * 2)	; ; Texte "2- Lecture des Fusibles HIGH"
		ldi		ZL, low(_TEXTE5 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		ldi		ZH, high(_TEXTE6 * 2)	; ; Texte "3- Lecture des Fusibles EXTENDED"
		ldi		ZL, low(_TEXTE6 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		ldi		ZH, high(_TEXTE7 * 2)	; ; Texte "4- Ecriture (valeur par defaut) des Fusibles LOW"
		ldi		ZL, low(_TEXTE7 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		ldi		ZH, high(_TEXTE8 * 2)	; ; Texte "5- Ecriture (valeur par defaut) des Fusibles HIGH"
		ldi		ZL, low(_TEXTE8 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		ldi		ZH, high(_TEXTE9 * 2)	; ; Texte "6- Ecriture (valeur par defaut) des Fusibles EXTENDED"
		ldi		ZL, low(_TEXTE9 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		ldi		ZH, high(_TEXTE10 * 2)	; ; Texte "7- Couper alimentation TARGET (sortie mode programmation)"
		ldi		ZL, low(_TEXTE10 * 2)	; ;
		rcall	_SEND_TEXT_RS232		; ;

		rcall	_GET_FROM_RS232			; ;
		cpi		R16, '8'				; ;
		brsh	PC - 2					; ; Chercher touche pressée (entre 1 et 7)
		cpi		R16, '1'				; ;
		brlo	PC - 4					; ;

		ldi		ZH, high(_TEXTE_CRLF * 2)
		ldi		ZL, low(_TEXTE_CRLF * 2)
		rcall	_SEND_TEXT_RS232		; 

		ldi		ZH, high(_TEXTE_CRLF * 2)
		ldi		ZL, low(_TEXTE_CRLF * 2)
		rcall	_SEND_TEXT_RS232		; 

		ldi		ZH, high(_TEXTE_CRLF * 2)
		ldi		ZL, low(_TEXTE_CRLF * 2)
		rcall	_SEND_TEXT_RS232		; 

		ldi		ZH, high(_TEXTE_CRLF * 2)
		ldi		ZL, low(_TEXTE_CRLF * 2)
		rcall	_SEND_TEXT_RS232		; 

		cpi		R16, '1'				; ;
		brne	PC + 2					; ;
		rjmp	_LECT_FUS_LOW			; ;
		cpi		R16, '2'				; ;
		brne	PC + 2					; ;
		rjmp	_LECT_FUS_HIGH			; ;
		cpi		R16, '3'				; ;
		brne	PC + 2					; ;
		rjmp	_LECT_FUS_EXTENDED		; ; traiter le choix
		cpi		R16, '4'				; ;
		brne	PC + 2					; ;
		rjmp	_ECRIT_FUS_LOW			; ;
		cpi		R16, '5'				; ;
		brne	PC + 2					; ;
		rjmp	_ECRIT_FUS_HIGH			; ;
		cpi		R16, '6'				; ;
		brne	PC + 2					; ;
		rjmp	_ECRIT_FUS_EXTENDED		; ;

		rcall	_OUT_PROG_MODE			; ; sortir du mode programmation
										; ; (coupure alim µC target)

		rjmp	_PT_BCLE1				; 

; =========================================================

_LECT_FUS_LOW:

		ldi		R20, 0					; choix de Fuses LOW

		ldi		ZH, high(_TEXTE11 * 2)	; Texte "Contenu des Fusibles LOW:   "
		ldi		ZL, low(_TEXTE11 * 2)	; 

		rjmp	_LECT_FUS_COMMUN		; 

_LECT_FUS_HIGH:

		ldi		R20, 1					; choix de Fuses HIGH

		ldi		ZH, high(_TEXTE12 * 2)	; Texte "Contenu des Fusibles HIGH:   "
		ldi		ZL, low(_TEXTE12 * 2)	; 

		rjmp	_LECT_FUS_COMMUN		; 

_LECT_FUS_EXTENDED:

		ldi		R20, 2					; choix de Fuses EXTENDED

		ldi		ZH, high(_TEXTE13 * 2)	; Texte "Contenu des Fusibles EXTENDED:   "
		ldi		ZL, low(_TEXTE13 * 2)	; 

_LECT_FUS_COMMUN:

		rcall	_SEND_TEXT_RS232		; 

		rcall	_RD_FUSE				; lecture des fusibles

		ldi		ZH, high(_TEXTE_CRLF * 2)
		ldi		ZL, low(_TEXTE_CRLF * 2)
		rcall	_SEND_TEXT_RS232		; 

		rjmp	_PT_BCLE2				; 

; =========================================================

_ECRIT_FUS_LOW:

		ldi		R20, 0					; choix de Fuses LOW
		ldi		R21, ValDefaultFuseLow

		rjmp	_ECRIT_FUS_COMMUN		; 


_ECRIT_FUS_HIGH:

		ldi		R20, 1					; choix de Fuses HIGH
		ldi		R21, ValDefaultFuseHigh

		rjmp	_ECRIT_FUS_COMMUN		; 


_ECRIT_FUS_EXTENDED:

		ldi		R20, 2					; choix de Fuses EXTENDED
		ldi		R21, ValDefaultFuseExtended

_ECRIT_FUS_COMMUN:

		rcall	_WR_FUSE				; Ecriture des fusibles

		cpi		R16, RDY_BSY_TimerOUT	; ; Tester si erreur d'écriture
		brne	_ECRIT_FUS_OUT			; ;

		rcall	_LED_Orange				; signaler problème par LED Orangée

		ldi		ZH, high(_TEXTE16 * 2)	; Texte "Pressez la touche <ENTREE> pour continuer"
		ldi		ZL, low(_TEXTE16 * 2)	; 
		rcall	_SEND_TEXT_RS232		; 

		rcall	_GET_FROM_RS232			; ;
		cpi		R16, CR					; ; Chercher touche <ENTREE> pressée
		brne	PC - 2					; ; (boucler en attendant)

		rcall	_LED_Rouge				; Rechanger LED d'Orange en Rouge

_ECRIT_FUS_OUT:
		ldi		ZH, high(_TEXTE_CRLF * 2)
		ldi		ZL, low(_TEXTE_CRLF * 2)
		rcall	_SEND_TEXT_RS232		; 

		rjmp	_PT_BCLE2				; 

; =========================================================

_INPUT_PROGMODE:
		; entrée en mode programmation:

		cbi		CONTRL_PORT, CLK	; SW CLK = "0"
		sbi		CONTRL_PORT, RESET	; ;
		cbi		CONTRL_PORT, HV_RST	; ; pin RESET à la masse

		_Waiste_us	StabDelay1		; Délais (en µs) utilisé pour stabilisation des pines

		; les 7 bits de sortie du CTRL port à "0"
		ldi		R16, 1<<PAGEL | 1<<XA1 | 1<<XA0 | 1<<BS1 | 1<<Not_WR | 1<<Not_OE | 0<<RDY_NotBSY | 1<<BS2
										; = 26ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		out		DDR_CTRL_P, R16		; 
		ldi		R16, 0<<PAGEL | 0<<XA1 | 0<<XA0 | 0<<BS1 | 0<<Not_WR | 0<<Not_OE | 0<<RDY_NotBSY | 0<<BS2
										; = 27ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		out		CTRL_PORT, R16		; 

		ldi		R16, 0b11111111		; 
		out		DDR_DATA_P, R16		; tout PORT de données en sortie
		ldi		R16, 0b00000000		; 
		out		DATA_PORT, R16		; toutes sorties à "0"

		; Mettre sous-tension le µC à traiter (Target):
		sbi		CONTRL_PORT,ALIM_Target

		_Waiste_us	(1000*PowerOnDelay)

		_Waiste_us	(1000*ResetDelayMs)

		_Waiste_us	(10*ResetDelayUs)

		; générer cycles Clocks pour stocker OSCCAL
		ldi		R25, LatchCycles	; Nbre de cycles Xtal utilisés pour stocker OSCCAL
_PROGMO_PT1:
		mov		R16, R25			; 
		dec		R25					; 
		tst		R16					; 
		breq	_PROGMO_PT2			; 
		rcall	_CLOCK_HV			; Générer CLOCK pour HV Prog
		rjmp	_PROGMO_PT1			; 

		_Waiste_us	StabDelay1		; Délais (en µs) utilisé pour stabilisation des pines

_PROGMO_PT2:
		sbi		CONTRL_PORT, HV_RST	; ; 12Volts sur la pin RESET ==> mode programmation //
		cbi		CONTRL_PORT, RESET	; ;

		_Waiste_us	StabDelay1		; Délais (en µs) utilisé pour stabilisation des pines

		_Waiste_us	ProgModeDelay	; 

		rcall	_LED_Rouge			; éclairer ROUGE LED bicolore (Target sous-tension)

		ret							; 

; =========================================================

_OUT_PROG_MODE:
		; Sortie du mode programmation

		sbi		CONTRL_PORT, RESET	; ;
		cbi		CONTRL_PORT, HV_RST	; ; pin RESET à la masse

		_Waiste_us	(1000*StabDelay2)

		ldi		r16, 0b00000000		; ;
		out		DDR_DATA_P, r16		; ;
		out		DATA_PORT, r16		; ; état flottant des PORTS
		out		DDR_CTRL_P, r16		; ; (mode entrée)
		out		CTRL_PORT, r16		; ;

		_Waiste_us	(1000*ResetDelay)

		cbi		CONTRL_PORT, HV_RST	; ;
		cbi		CONTRL_PORT, RESET	; ; pin RESET à l'état "1"

		_Waiste_us	(1000*StabDelay2)

		; Mettre hors-tension le µC traité (Target):
		cbi		CONTRL_PORT,ALIM_Target

		_Waiste_us	(1000*PowerOffDelay)

		rcall	_LED_Verte			; éclairer VERTE LED bicolore (sécurité d'extraction)

		ret							; 

; =========================================================

_RD_FUSE:
		; lecture des "fusibles"

		ldi		R16, Cmde_RD_FUSES	; '0000 0100' = Byte de Cmde "READ LOCK & FUSE BITS"

		ldi		R17, 0<<PAGEL | 1<<XA1 | 0<<XA0 | 0<<BS1 | 1<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 0<<BS2
										; = 9ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		out		CTRL_PORT, R17		; 

		out		DATA_PORT, R16		; sortir "Prog DATA"

		rcall	_CLOCK_HV			; Générer CLOCK pour HV Prog

		cpi		R20, 0				; 
		breq	_RD_FUSE_LOW_BYTE	; choix = 0 (Lecture "Low Fuses Byte")

		cpi		R20, 1				; 
		breq	_RD_FUSE_HIGH_BYTE	; choix = 1 (Lecture "High Fuses Byte")

		rjmp	_RD_FUSE_EXTEND_BYTE; choix > 1 (Lecture "Extended Fuses Byte")


_RD_FUSE_LOW_BYTE:
		ldi		R16, 0b00000000		; tout PORT données en entrée
		out		DDR_DATA_P, R16		; 
		ldi		R16, 0b11111111		; 8 résistances de PULL-UP activées
		out		DATA_PORT, R16		; 

		; envoyer cmde "Lecture Low Fuses Byte" et lire DATA
		ldi		R16, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 0<<BS1 | 1<<Not_WR | 0<<Not_OE | 1<<RDY_NotBSY | 0<<BS2
										; = 21ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		out		CTRL_PORT, R16		; 
		nop							; ;
		nop							; ;
		nop							; ; 5 * 67,8ns = 339 ns (minimum = 250 ns)
		nop							; ;
		nop							; ;
		in		R16, DATA_P_INPUT	; 

		ldi		R17, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 0<<BS1 | 1<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 0<<BS2
										; = 13ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		rjmp	_RD_FUSE_OUT		; 

_RD_FUSE_HIGH_BYTE:
		; DATA en entrée , sortir 24ème byte de CTRL , lire DATA , lire 16ème byte de CTRL
		ldi		R16, 0b00000000		; tout PORT données en entrée
		out		DDR_DATA_P, R16		; 
		ldi		R16, 0b11111111		; 8 résistances de PULL-UP activées
		out		DATA_PORT, R16		; 

		; envoyer cmde "Lecture High Fuses Byte" et lire DATA
		ldi		R16, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 1<<BS1 | 1<<Not_WR | 0<<Not_OE | 1<<RDY_NotBSY | 1<<BS2
										; = 24ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		out		CTRL_PORT, R16		; 
		nop							; ;
		nop							; ;
		nop							; ; 5 * 67,8ns = 339 ns (minimum = 250 ns)
		nop							; ;
		nop							; ;
		in		R16, DATA_P_INPUT	; 

		ldi		R17, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 1<<BS1 | 1<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 1<<BS2
										; = 16ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")

_RD_FUSE_OUT:
		out		CTRL_PORT, R17		; 

		ldi		R17, 0b11111111		; 
		out		DDR_DATA_P, R17		; tout PORT données en sortie
		ldi		R17, 0b00000000		; 
		out		DATA_PORT, R17		; sortir 0 sur PORTB données

		rjmp	_SEND_VAL_RS232		; Envoyer valeur lue (R16) sur le port série

_RD_FUSE_EXTEND_BYTE:
		ldi		R16, 0b00000000		; tout PORT données en entrée
		out		DDR_DATA_P, R16		; 
		ldi		R16, 0b11111111		; 8 résistances de PULL-UP activées
		out		DATA_PORT, R16		; 

		; envoyer cmde "Lecture Extended Fuses Byte" et lire DATA
		ldi		R16, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 0<<BS1 | 1<<Not_WR | 0<<Not_OE | 1<<RDY_NotBSY | 1<<BS2
										; = 23ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		out		CTRL_PORT, R16		; 
		nop							; ;
		nop							; ;
		nop							; ; 5 * 67,8ns = 339 ns (minimum = 250 ns)
		nop							; ;
		nop							; ;
		in		R16, DATA_P_INPUT	; 

		ldi		R17, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 0<<BS1 | 1<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 1<<BS2
										; = 15ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		rjmp	_RD_FUSE_OUT		; 

; =========================================================

_WR_FUSE:
		; écriture des "fusibles"

		ldi		R16, Cmde_WR_FUSES	; '0100 0000' = Byte de Cmde "WRITE FUSE BITS"

		ldi		R17, 0<<PAGEL | 1<<XA1 | 0<<XA0 | 0<<BS1 | 1<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 0<<BS2
										; = 9ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		out		CTRL_PORT, R17		; 
		out		DATA_PORT, R16		; sortir "Prog DATA"

		rcall	_CLOCK_HV			; 

		ldi		R16, 0<<PAGEL | 0<<XA1 | 1<<XA0 | 0<<BS1 | 1<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 0<<BS2
										; = 5ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		out		CTRL_PORT, R16		; 

		out		DATA_PORT, R21		; Envoyer val à programmer sur bus DATA

		rcall	_CLOCK_HV			; 

		cpi		R20, 0				; 
		breq	_WR_FUSE_LOW_BYTE	; choix = 0 (Ecriture "Low Fuses Byte")

		cpi		R20, 1				; 
		breq	_WR_FUSE_HIGH_BYTE	; choix = 1 (Ecrire "High Fuses Byte")

		rjmp	_WR_FUSE_EXTEND_BYTE; choix > 1 (Ecrire "Extended Fuses Byte")

_WR_FUSE_LOW_BYTE:
		; Ecriture "Low Fuses Byte"
		ldi		R16, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 0<<BS1 | 0<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 0<<BS2
										; = 17ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		out		CTRL_PORT, R16		; 
		nop							; ; 2 * 67,8ns = 135,6 ns (minimum = 67 ns)
		nop							; ;

		_Waiste_us	(1000*PulseWidth)

		ldi		R16, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 0<<BS1 | 1<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 0<<BS2
										; = 13ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		rjmp	_WR_FUSE_Test		; 

_WR_FUSE_HIGH_BYTE:
		; Ecrire "High Fuses Byte"
		ldi		R16, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 1<<BS1 | 0<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 0<<BS2
										; = 18ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		out		CTRL_PORT, R16		; 
		nop							; ;
		nop							; ; 2 * 67,8ns = 135,6 ns (minimum = 67 ns)

		_Waiste_us	(1000*PulseWidth)

		ldi		R16, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 1<<BS1 | 1<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 0<<BS2
										; = 14ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		rjmp	_WR_FUSE_Test		; 

_WR_FUSE_EXTEND_BYTE:
		; Ecrire "Extended Fuses Byte"
		ldi		R16, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 0<<BS1 | 0<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 1<<BS2
										; = 19ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")
		out		CTRL_PORT, R16		; 
		nop							; ;
		nop							; ; 2 * 67,8ns = 135,6 ns (minimum = 67 ns)

		_Waiste_us	(1000*PulseWidth)

		ldi		R16, 0<<PAGEL | 1<<XA1 | 1<<XA0 | 0<<BS1 | 1<<Not_WR | 1<<Not_OE | 1<<RDY_NotBSY | 1<<BS2
										; = 15ème Byte de la "PPControlStack" (Voir fichier "Partdescriptionfiles")

_WR_FUSE_Test:
		out		CTRL_PORT, R16		; 

		ldi		R16, PollTimeout	; temps autorisé (exprimé en µs*100)
		rcall	_TEMPS_RDY			; tester temps que met "RDY / Not BSY" pour venir

		tst		R16					; ;
		breq	_WR_FUSE_Suite		; ; écriture correcte


		; Message selon résultat test ci-dessus
		ldi		R16, RDY_BSY_TimerOUT
		rjmp	_WR_FUSE_OUT		; 
_WR_FUSE_Suite:
		ldi		R16, CMD_OK			; 
_WR_FUSE_OUT:
		rjmp	_SEND_STATUS_RS232	; vers la RS232

; =========================================================

_TEMPS_RDY:
		; tester temps que met "RDY / Not BSY" pour remonter à "1"

		mov		YL, R16				; transférer le temps ds XL

		tst		YL					; 
		breq	_TIMER_OUT			; sauter si temps accordé = 0 (==> "Timer OUT")

		_Waiste_us	1				; ; tempo 1µs

		sbic	CTRL_P_INPUT, RDY_NotBSY
		rjmp	_TIMER_OUT			; sauter si "RDY/Not BSY" encore à "1" (==> "Timer OUT")

_TEMPS_RDY_BCLE:
		_Waiste_us	100				; ; tempo 100µs

		dec		YL					; décrémenter le temps autorisé (YL = YL - 1)

		sbic	CTRL_P_INPUT, RDY_NotBSY
		rjmp	_TIMER_OK			; sauter si "RDY/Not BSY" à "1"

		tst		YL					; ;
		brne	_TEMPS_RDY_BCLE		; ; boucler si délais pas écoulé et "RDY/Not BSY" = "0"

_TIMER_OUT:
		ldi		R16, 1				; "Timer OUT"
		ret							; 

_TIMER_OK:
		ldi		R16, 0				; OK
		ret							; 

; =========================================================

_CLOCK_HV:
		; Générer CLOCK
		sbi		CONTRL_PORT, CLK	; CLK HV Prog
		nop							; 67,8ns
		nop							; 67,8ns
		nop							; 67,8ns
		nop							; 67,8ns
		cbi		CONTRL_PORT, CLK	; CLK HV Prog

		ret							; 

; =========================================================

_INIT_RS232:
		; Initialiser l'interface RS232 à 19200 bauds (Quartz 14,7456MHz)

		; Mode normal (U2X = 0)
		.equ	Val_UBRR	=	((Fosc / (16 * baud)) - 1)
		ldi		R17, high(Val_UBRR)	; 
		ldi		R16, low(Val_UBRR)	; 
		out		UBRRH, R17			; 
		out		UBRRL, R16			; 
		ldi		R16, (1<<RXEN)|(1<<TXEN)
		out		UCSRB, R16			; 

		ret							; 

; =========================================================

_SEND_VAL_RS232:
		; Envoyer vers RS232 (valeur dans R16)
		; sous forme binaire vers la RS232

		ldi		R17, '0'			; ;
		rcall	_SEND_RS232			; ;
		ldi		R17, 'b'			; ; Envoyer l'en-tête binaire "0b"
		rcall	_SEND_RS232			; ;

		ldi		R18, 8				; ;
		lsl		R16					; ;
		ldi		R17, '0'			; ;
		brcc	PC + 2				; ;
		ldi		R17, '1'			; ; Shifter gauche et envoyer sur RS232
		rcall	_SEND_RS232			; ; tout contenu de R16 sous forme ASCII
		dec		R18					; ; '0' et '1'
		brne	PC - 6				; ;

		ldi		ZH, high(_TEXTE_CRLF * 2)
		ldi		ZL, low(_TEXTE_CRLF * 2)
		rjmp	_SEND_TEXT_RS232	; 

; =========================================================

_SEND_RS232:
		; Envoyer à 19200Bauds vers RS232 (R17)
		sbis	UCSRA, UDRE			; ; attendre que registre UDR
		rjmp	PC - 1				; ; soit vide
		out		UDR, R17			; déposer le Byte
		ret							; 

; =========================================================

_SEND_TEXT_RS232:
		; Envoyer texte vers la RS232
		; La 1ère adresse mémoire d'implantation du texte
		; est contenue dans Z
		; La fin du texte est signalée en mémoire par la valeur "zéro"
		lpm		R17, Z+				; 
		tst		R17					; 
		breq	PC + 3				; 
		rcall	_SEND_RS232			; 
		rjmp	PC - 4				; 

		ret							; 

; =========================================================

_SEND_STATUS_RS232:
		; Envoyer état vers RS232 
		ldi		ZH, high(_TEXTE14 * 2)	; ; Adresse Texte "Ecriture realisee correctement"
		ldi		ZL, low(_TEXTE14 * 2)	; ;
		cpi		R16, RDY_BSY_TimerOUT
		brne	PC + 3				; 
		ldi		ZH, high(_TEXTE15 * 2)	; ; Adresse Texte "Erreur lors de l'ecriture"
		ldi		ZL, low(_TEXTE15 * 2)	; ;

		rcall	_SEND_TEXT_RS232	; 

		ldi		ZH, high(_TEXTE_CRLF * 2)
		ldi		ZL, low(_TEXTE_CRLF * 2)
		rjmp	_SEND_TEXT_RS232	; 

; =========================================================

_GET_FROM_RS232:
		; Recevoir valeur depuis la RS232 (et renvoi de celle-ci, en ECHO)

		sbis	UCSRA, RXC			; 
		rjmp	PC - 1				; 
		in		R16, UDR			; 

		mov		R17, R16			; 
		rcall	_SEND_RS232			; 

		ldi		ZH, high(_TEXTE_CRLF * 2)
		ldi		ZL, low(_TEXTE_CRLF * 2)
		rjmp	_SEND_TEXT_RS232	; 

; =========================================================

_LED_Verte:
		; éclairer VERTE LED bicolore
		; (sécurité pour extraction)
		cbi		CONTRL_PORT, Anode_LED_R
		sbi		CONTRL_PORT, Anode_LED_V

		ret

_LED_Rouge:
		; éclairer ROUGE LED bicolore
		; (Target sous-tension)
		sbi		CONTRL_PORT, Anode_LED_R
		cbi		CONTRL_PORT, Anode_LED_V

		ret

_LED_Orange:
		; éclairer ORANGE LED bicolore
		; (Signalisation problème)
		sbi		CONTRL_PORT, Anode_LED_R
		sbi		CONTRL_PORT, Anode_LED_V

		ret

_LED_OFF:
		; Eteindre LED bicolore
		cbi		CONTRL_PORT, Anode_LED_R
		cbi		CONTRL_PORT, Anode_LED_V

		ret

; =========================================================

; Textes pour envoi vers la RS232:
; -------------------------------

_TEXTE_CRLF:
		.db CR, LF, 0, 0

_TEXTE1:
		.db	"Lecteur/Programmateur de Fusibles ATmega168 (mode //)", CR, LF, 0

_TEXTE2:
		.db	"1- alimentation TARGET (entree en mode programmation)", CR, LF, 0

_TEXTE3:
		.db	"Faites votre choix ci-dessous:", CR, LF, 0, 0

_TEXTE4:
		.db	"1- Lecture des Fusibles LOW", CR, LF, 0

_TEXTE5:
		.db	"2- Lecture des Fusibles HIGH", CR, LF, 0, 0

_TEXTE6:
		.db	"3- Lecture des Fusibles EXTENDED", CR, LF, 0, 0

_TEXTE7:
		.db	"4- Ecriture (valeur par defaut) des Fusibles LOW", CR, LF, 0, 0

_TEXTE8:
		.db	"5- Ecriture (valeur par defaut) des Fusibles HIGH", CR, LF, 0

_TEXTE9:
		.db	"6- Ecriture (valeur par defaut) des Fusibles EXTENDED", CR, LF, 0

_TEXTE10:
		.db	"7- Couper alimentation TARGET (sortie mode programmation)", CR, LF, 0

_TEXTE11:
		.db	"Contenu des Fusibles LOW:   ", 0, 0

_TEXTE12:
		.db	"Contenu des Fusibles HIGH:   ", 0

_TEXTE13:
		.db	"Contenu des Fusibles EXTENDED:   ", 0

_TEXTE14:
		.db	"Ecriture realisee correctement", CR, LF, 0, 0

_TEXTE15:
		.db	"Erreur lors de l'ecriture", CR, LF, 0

_TEXTE16:
		.db	"Pressez la touche <ENTREE> pour continuer", CR, LF, 0


; =========================================================

