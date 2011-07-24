;****************************************************************************
;* S O R G E N T E  P E R  V I S U A L I Z Z A R E  I L  V A L O R E 
;* D I  U N A  C O N V E R S I O N E  A / D  S U  L C D (mod. 4 bit)
;*
;* Nome File			:"LM35.asm"
;* Titolo					:Visualizza codice decimale corrispondente al
;*							:valore di conversione A/D su LCD
;* Data					:19.13 martedì 5 febbraio 2008
;* Versione				:0.02
;* Tipo uAVR			:ATMega16 con Quarzo da 8 MHz
;*
;* DESCRIZIONE:
;* Questo programma permette di visualizzare su LCD,
;* la temperatura rilevata con una sonda LM35 collegata sul pin
;* Canale 0 della porta A. 
;* La risoluzione massima sarà di 1° C. Per aumentarla bisogna
;* cambiare la VREF (che in questo caso è 0=VCC).
;* Non sono usati gli Interrupt.
;****************************************************************************
.NOLIST
.INCLUDE	"M16DEF.INC"
.INCLUDE	"Def_IO2.inc"
.LIST
;****************************************************************************
.ORG			0x0000
		Rjmp		RESET
.ORG			0x0002
;		Rjmp		EXT_INT0
		Ret
.ORG			0x0004
;		Rjmp		EXT_INT1
		Ret
.ORG			0x0006
;		Rjmp		TIM2_COMP
		Ret
.ORG			0x0008
;		Rjmp		TIM2_OVF
		Ret
.ORG			0x000A
;		Rjmp		TIM1_CAPT
		Ret
.ORG			0x000C
;		Rjmp		TIM1_COMPA
		Ret
.ORG			0x000E
;		Rjmp		TIM1_COMPB
		Ret
.ORG			0x0010
;		Rjmp		TIM1_OVF
		Ret
.ORG			0x0012
;		Rjmp		TIM0_OVF
		Ret
.ORG			0x0014
;		Rjmp		SPI_STC
		Ret
.ORG			0x0016
;		Rjmp		USART_RXC
		Ret
.ORG			0x0018
;		Rjmp		USART_UDRE
		Ret
.ORG			0x001A
;		Rjmp		USART_TXC
		Ret
.ORG			0x001C
;		Rjmp		INT_ADC
		Ret
.ORG			0x001E
;		Rjmp		EE_RDY
		Ret
.ORG			0x0020
;		Rjmp		ANA_COMP
		Ret
.ORG			0x0022
;		Rjmp		TWSI
		Ret
.ORG			0x0024
;		Rjmp		EXT_INT2
		Ret
.ORG			0x0026
;		Rjmp		TIM0_COMP
		Ret
.ORG			0x0028
;		Rjmp		SPM_RDY
		Ret


.ORG			0x002A
RESET:
;**************** PULIZIA REGISTRI UTILIZZATI ***********************
		clr		Ascii
		clr		Lval
		clr		Hval
		clr		Count
		ldi		Temp,'_'
		mov	Dig0,Temp
		mov	Dig1,Temp
		mov	Dig2,Temp
		mov	Dig3,Temp
;****************************************************************************
;**************** SETTAGGIO STACK POINTER **********************
		ldi		Temp,HIGH (RAMEND)	;Indirizzo STACK  ALTO
		out	SPH,Temp					;
		ldi		Temp,LOW  (RAMEND)	;Indirizzo STACK BASSO
		out	SPL,Temp						;
;****************************************************************************
;**************** SETTAGGIO PORTE INPUT/OUTPUT ***************
		clr		Temp
		out	A_DDIR,Temp				;Tutta la Porta = Ingresso = 0
		ldi		Temp,0xFF
		out	LCDDIR,Temp				;Tutta la Porta = Out = 1

;Selezione ADC0 come ingresso per A/D Converter
;e setta AVCC = VCC come tensione di riferimento
;(quindi Vref interna = Off)
		ldi 	Temp,0x40
		out 	ADMUX,Temp

		ldi		Temp,0xD0					;Abil. ADC, Start conversione
		out	ADCSRA,Temp				;Reset Flag Conv. Completa
;****************************************************************************
;**************** SETTAGGIO PRESCALER ***************************
;Setta Prescaler Timer 0 a 1024 (8MHz/1024)
;		ldi		Temp,0x05					;
;		out 	TCCR0,Temp					;
;		ldi 	Temp,0x02					;Interrupt Overflow Timer 0
;		out 	TIMSK,Temp					;	"			"		"		"

;		ldi 	Temp,0x07					;Abilita Interrupt ADC
;		out 	ADCSRA,Temp				;& Prescaler ADC = 128
;****************************************************************************
;**************** VISUALIZZA IL MESSAGGIO INIZIALE **************
.INCLUDE	"Lcd4INIT2.inc"

		ldi			Temp,0x00
		Rcall		LCDLOCATE

		ldi			ZH,HIGH(2*InitMsg)
		ldi			ZL,LOW(2*InitMsg)

Msg_iniz:
;Incrementa puntatore (successivo carattere)
		lpm
		adiw		ZL,1

		mov		Temp,Ascii
		cpi		Temp,0x0D
		breq		NewLine
		mov		Temp,Ascii
		cpi		Temp,0x01
		breq		PulisciLCD
		mov		Temp,Ascii
		cpi		Temp,0x00
		breq		EndMsg
		Rcall		INVIA_ASCII
		Rjmp		Msg_iniz
NewLine:
		ldi			Temp,0x40
		Rcall		LCDLOCATE
		ldi			Temp,50
		Rcall		msDelay
		Rjmp		Msg_iniz
PulisciLCD:
		Rcall		CLEAR_LCD
		ldi			Temp,50
		Rcall		msDelay
		ldi			Temp,0x00
		Rcall		LCDLOCATE
		ldi			Temp,60
		Rcall		msDelay
		Rjmp		Msg_iniz
EndMsg:
		Rjmp		FineLCD

;********* Routine per gestione LCD *********
LCDLOCATE:							;Indirizzo posizione cursore
		ldi			Temp2,0x80
		eor		Temp,Temp2

INVIA_COM:
		push		Temp
		andi		Temp,0xF0			;Maschera la parte bassa
		ldi			Temp2,0x04
		eor		Temp,Temp2
		out		LCDPORT,Temp
		INVIA_COMANDO				;Parte alta del comando
												;
		pop		Temp
		swap	Temp					;Inverte i 4 bit LSB con i 4 MSB
		andi		Temp,0xF0
		ldi			Temp2,0x04
		eor		Temp,Temp2
		out		LCDPORT,Temp
		INVIA_COMANDO				;Parte bassa del comando
		ldi			Temp,10
		Rcall		msDelay
		Ret

INVIA_ASCII:
		mov		Temp,Ascii
		andi		Temp,0xF0			;Maschera la parte bassa
		push		Temp2
		ldi			Temp2,0x04
		eor		Temp,Temp2
		pop		Temp2
		out		LCDPORT,Temp
		INVIA_DATO						;Parte alta del carattere ascii
												;
		mov		Temp,Ascii
		swap	Temp					;Inverte i 4 bit LSB con i 4 MSB
		andi		Temp,0xF0
		push		Temp2
		ldi			Temp2,0x04
		eor		Temp,Temp2
		pop		Temp2
		out		LCDPORT,Temp
		INVIA_DATO						;Parte bassa del carattere ascii
		ldi			Temp,10
		Rcall	msDelay
		Ret

CLEAR_LCD:
		ldi			Temp,0x01			;Pulizia LCD e Return Home
		Rcall	INVIA_COM				;Pin RS LCD = 0  -> Comando
		Ret

;***** ROUTINE PRINCIPALE ******************************************
FineLCD:

		ldi			Temp,250 ; 		|
		Rcall		msDelay ; 			|
		ldi			Temp,250 ; 		|
		Rcall		msDelay ; 			|
;			  								 > Attendi 0,5 secondi circa

		Rcall		CLEAR_LCD		;Posiziona il cursore at HOME...
		ldi			Temp,70			;
		Rcall		msDelay			;Attendi...

		mov		Ascii,Dig2
		Rcall		INVIA_ASCII
		mov		Ascii,Dig3
		Rcall		INVIA_ASCII
		ldi			Temp,0xDF		;Simbolo ° 
		mov		Ascii,Temp
		Rcall		INVIA_ASCII
		ldi			Temp,'C'
		mov		Ascii,Temp
		Rcall		INVIA_ASCII

MAIN:
		ldi			Temp,250
		Rcall		msDelay
		sbi		ADCSRA,ADSC;Start Conversione
Loop1:
		sbic		ADCSRA,ADSC;Attendi il termine della A/D Conv.
		Rjmp		Loop1
;*** I M P O R T A N T E  N O N  I N V E R T I R E  L'  O R D I N E ***
		in 			Lval,ADCL			;Leggi il valore della conversione
		in 			Hval,ADCH		;e copialo nei registri rispettivi
;****************************************************************************
;Converti in decimi di grado Celsius e
;in ASCII per visualizzarlo su LCD
		clc								;Azzera Carry prima di dividere
		ror		Hval					;il valore per 2 (ruota a DX)
		ror		Lval					;...anche la parte bassa
		clc								;Azzera carry
		clr 		Count				;Azzera registro di conteggio
Migliaia:
		mov 		CopyLval,Lval	;Salva una copia
		mov 		CopyHval,Hval	;	"		"		"

		ldi			Temp,0xE8		;Parte bassa di 1000 in esadec.
		sub 		CopyLval,Temp	;Sottrai senza Carry
		ldi			Temp,0x03		;Parte alta di 1000 in esadec.
		sbc 		CopyHval,Temp	;Sottrai con Carry
		brcs 		Save0				;Salva Count in Digit 0 se c'è Carry
		inc 		Count				;
		mov 		Lval,CopyLval	;Salva Resto della sottrazione
		mov 		Hval,CopyHval	;	"		"		"		"		"
		Rjmp 	Migliaia				;Ripeti sottrazione finchè il Carry 
											;non è settato
Save0:
		ldi			Temp,0x30
		add		Count,Temp		;Converti in numero ASCII (Aggiungi 30)
		mov		Dig0,Count		;Salva in Digit 0

		clr			Count				;Azzera registro di conteggio
Centinaia:
		mov		CopyLval,Lval	;Lavora con copia del Resto sottr.
		mov 		CopyHval,Hval	;	"		"		"		"		"		"
		ldi			Temp,100			;Parte bassa di 100 in decimale
		sub		CopyLval,Temp	;Sottrai senza Carry
		clr 		Temp				;Azzera Temp
		sbc 		CopyHval,Temp	;Sottrai con Carry
		brcs 		Save1				;Salva Count in Digit 1 se c'è Carry
		inc		Count				;
		mov 		Lval,CopyLval	;Salva Resto della sottrazione
		mov 		Hval,CopyHval	;	"		"		"		"		"
		Rjmp 	Centinaia			;Ripeti sottrazione finchè il Carry 
											;non è settato
Save1:
		ldi			Temp,0x30
		add		Count,Temp		;Converti in numero ASCII (Aggiungi 30)		
		mov		Dig1,Count		;Salva in Digit 1
		clr			Count				;Azzera registro di conteggio
Decine:
		mov 		CopyLval,Lval	;Lavora con copia del Resto sottr.
		ldi			Temp,10			;Parte bassa di 10 in decimale
		sub		CopyLval,Temp	;Sottrai senza Carry
		brcs		Save2				;Salva Count in Digit 2 se c'è Carry
		inc 		Count				;
		mov		Lval,CopyLval	;Salva Resto della sottrazione
		Rjmp 	Decine				;Ripeti sottrazione finchè il Carry 
											;non è settato
Save2:
		ldi			Temp,0x30
		add		Count,Temp		;Converti in numero ASCII (Aggiungi 30)
		mov		Dig2,Count		;Salva in Digit 2
		ldi			Temp,0x30
		add		Lval,Temp			;Converti in numero ASCII (Aggiungi 30)
		mov		Dig3,Lval			;Salva in Digit 3
;Le unità sono banali, è il valore di resto..

;Aggiorna LCD con nuovo valore di conversione
		ldi			Temp,0x00
		Rcall		LCDLOCATE		;Posiziona il cursore
		ldi			Temp,20
		Rcall		msDelay

		mov		Ascii,Dig2
		Rcall		INVIA_ASCII
		mov		Ascii,Dig3
		Rcall		INVIA_ASCII
		ldi			Temp,0x00
		Rcall		LCDLOCATE		;Posiziona il cursore
		Rjmp 	MAIN				;Ripeti ancora
;***** - FINE - ROUTINE PRINCIPALE *********************************

;***** ROUTINE DI RITARDO IN milliSECONDI **********************************
msDelay:
		mov		Delay1,Temp
		in			Temp,SREG
		push		Temp
		tst			Delay1
		breq		NoDelay
Ciclo1:
		ldi			Temp,0x0A
		mov		Delay2,Temp
		clr			Delay3
Ciclo2:
		dec		Delay3
		brne		Ciclo2
		dec		Delay2
		brne		Ciclo2
		ldi			Temp,0x5D
		mov		Delay3,Temp
Ciclo3:
		dec		Delay3
		brne		Ciclo3
		dec		Delay1
		brne		Ciclo1
NoDelay:
		pop		Temp
		out		SREG,Temp
		Ret


.CSEG
InitMsg:
.DB "   Lettura temperatura con sonda LM35",0x0D
.DB "File aggiornato alle 19.13 il 05/02/2008",0x00
;Codice 0x0D = manda a capo il testo
;Codice 0x01 = pulisce il display dopo 1 secondo circa di visualizzazione del testo e posiziona a 00 (Home) il cursore.
;Codice 0x00 = termina la scrittura del messaggio
