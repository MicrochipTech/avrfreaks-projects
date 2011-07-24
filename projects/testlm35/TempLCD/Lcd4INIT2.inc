;***************************************************************************************
;* R O U T I N E  D I  I N I Z I A L I Z Z A Z I O N E  L C D (mod. 4 bit)
;*
;* Nome File				:"Lcd4INIT2.inc"
;* Titolo					:Inizializzazione LCD modalita' a 4 bit
;* Data						:17.56 domenica 28 ottobre 2007
;* Versione				:0.05
;* Tipo uAVR			:ATMega16 con Quarzo da 8 MHz
;*
;* Registri usati	: Temp = R16, registro di uso generale dichiarato nel
;* file include "Def_IO.inc"; PORTLCD = Qualsiasi porta dichiarata nello
;* stesso file (vedi sopra); Delay1-2-3 = Vedi routine msDelay
;*
;* DESCRIZIONE:
;* Questa routine inizializza il display LCD in modalità a 4 bit,
;* verificare i collegamenti del medesimo facendo riferimento a ciò che 
;* viene dichiarato nel file include "Def_IO.inc" dove vengono assegnate
;* la LABEL agli/alle ingressi/uscite utilizzati/utilizzate
;* 
;***************************************************************************************
;*************** ASSEGNAZIONE DELLE MACRO DI PROGRAMMA ******
.MACRO		INVIA_DATO					;Prepara LCD a ricevere un dato (non un comando)
		sbi		LCDPORT,RS_LCD		;Pin RS LCD = 1  -> Dato
		ENABLE
.ENDMACRO

;***

.MACRO		INVIA_COMANDO			;Prepara LCD a ricevere un comando
		cbi		LCDPORT,RS_LCD		;Pin RS LCD = 0  -> Comando
		ENABLE
.ENDMACRO

;***

;.MACRO		CLEAR_LCD					;Cancella il contenuto del LCD e torna con il
;		ldi		Temp,$01							;cursore alla posizione "Home" (00)
;		INVIA_COMANDO
;		ENABLE
;.ENDMACRO

;***

.MACRO		ENABLE						;Impulso sul fronte di discesa sul pin ENABLE
		sbi		LCDPORT,EN_LCD
		nop
		nop
		nop
		nop
		cbi		LCDPORT,EN_LCD
.ENDMACRO

;***

.EQU			FunctionSet = 0b00110100
;*************** INIZIO ROUTINE DI INIZIALIZZAZIONE LCD ********************

Lcd4INIT:
		ldi		Temp,50							;Attendi minimo 15ms dopo l'accensione del LCD
		Rcall	msDelay

		ldi		Temp,FunctionSet
		out		LCDPORT,Temp

		ENABLE

		ldi		Temp,10							;Attendi minimo 4,1 ms dopo invio del comando
		Rcall	msDelay

		ldi		Temp,FunctionSet
		out		LCDPORT,Temp

		ENABLE

		ldi		Temp,10							;Attendi minimo 100 us dopo invio del comando
		Rcall	msDelay

		ldi		Temp,FunctionSet
		out		LCDPORT,Temp

		ENABLE									;Da ora in poi è possibile testare il flag BF
														;LCD (ma non viene testato in questa routine)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

		ldi		Temp,0x24						;0b00100100			Modalità a 4 bit
		out		LCDPORT,Temp

		ENABLE									;Settaggio della modalita' a 4 bit del LCD
														;(vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

;******** MODALITA' 4 BIT + NUM. LINEE + FONT ******************************

		ldi		Temp,0x24						;0b00100100			Parte alta
		out		LCDPORT,Temp

		ENABLE									;Settaggio della modalita' a 4 bit del LCD
														;(vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

		ldi		Temp,0x84						;0b10000100 Parte bassa
		out		LCDPORT,Temp

		ENABLE									;Settaggio della modalita' a 4 bit del LCD
														;num. linee e font (vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

;********* DISPLAY OFF *****************************************************

		ldi		Temp,0x04						;0b00000100			Parte alta
		out		LCDPORT,Temp

		ENABLE									;Display OFF parte alta del byte
														;(vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

		ldi		Temp,0x84						;0b10000100 Parte bassa
		out		LCDPORT,Temp

		ENABLE									;Display OFF parte bassa del byte
														;cursore (vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

;********* CLEAR DISPLAY ***************************************************

		ldi		Temp,0x04						;0b00000100			Parte alta
		out		LCDPORT,Temp

		ENABLE									;Clear display
														;(vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

		ldi		Temp,0x14						;0b00010100			Parte bassa
		out		LCDPORT,Temp

		ENABLE									;Clear Display
														;(vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

;******** ENTRY MODE SET (DIREZIONE CURSORE & SHIFT DISPLAY) ***************

		ldi		Temp,0x04						;0b00000100			Parte alta
		out		LCDPORT,Temp

		ENABLE									;Spostamento cursore, shift display
														;(vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

		ldi		Temp,0x64						;0b01100100			Parte bassa
		out		LCDPORT,Temp

		ENABLE									;Settaggio Display OFF, Cursore e Lampeggio
														;cursore (vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

;******************* AGGIUNTO DA ME PER PROVE ******************************

		ldi		Temp,0x14						;0b00010100			Parte alta
		out		LCDPORT,Temp

		ENABLE									;Scorrimento Cursore/Display parte alta del byte
														;(vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

		ldi		Temp,0x44						;0b01000100			Parte bassa
		out		LCDPORT,Temp

		ENABLE									;Display ON parte bassa del byte
														;cursore (vedi data sheet Vikay)

		ldi		Temp,10							;Visto che non viene testato il flag BF attendiamo
		Rcall	msDelay

;******************* AGGIUNTO DA ME PER PROVE ******************************

		ldi		Temp,0x04						;0b00000100			Parte alta
		out		LCDPORT,Temp

		ENABLE									;Display ON parte alta del byte
														;(vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay

		ldi		Temp,0xC4						;0b11000100			Parte bassa
		out		LCDPORT,Temp

		ENABLE									;Display ON parte bassa del byte
														;cursore e lampeggio ON(vedi data sheet Vikay)

		ldi		Temp,10							;Non viene testato il flag BF quindi attendiamo
		Rcall	msDelay


;******** FINE INIZIALIZZAZIONE LCD COME DA DATA SHEET *********************
