;***********************************************
; MACRO_DEF.ASM                           
; 09.04.2006         
; 21.04.2006	adapté pour Alim de Labo     LNG                     
; hirschmann vitalis                      
;***********************************************
; définitions des constantes et macros    
;***********************************************


		
;-----------------------------------------------
;+++ définitions des macros                  +++
;-----------------------------------------------


;-----------------------------------------------
;+++ PUTDEC		         	  				 +++
;+++  in:  b0                        	     +++
;+++  out: rien                              +++
;-----------------------------------------------
;+ Affiche la valeur dans le registre @0       +
;+ (registre à afficher) sur le LCD            +
;-----------------------------------------------
.macro	PUTDEC
		mov		a0, @0
		rcall	putdec
.endmacro


;-----------------------------------------------
;+++ STSI: const, valeur à enregistrer       +++
;-----------------------------------------------
.macro	STSI
		ldi		w, @1
		sts		@0, w
.endmacro


;-----------------------------------------------
;+++ START		         	     +++
;-----------------------------------------------
.macro	START
		cbi		PORTB, 2
		sbi		PORTB, 3	
.endmacro


;-----------------------------------------------
;+++ RETOUR		         	     +++
;-----------------------------------------------
.macro	RETOUR
		sbi		PORTB, 2
		cbi		PORTB, 3
.endmacro		


;-----------------------------------------------
;+++ STOP		         	     +++
;-----------------------------------------------
.macro	STOP
		cbi		PORTB, 3
		cbi		PORTB, 2
.endmacro


;-----------------------------------------------
;+++ GAUCHE: valeur à diminuer l'angle 	     +++
;-----------------------------------------------
.macro	AUGMT
		in		b3, OCR1AL
		subi	b3, -@0
		cpi		b3, deg_180
		brpl	exit							; branch if plus
		ldi		b2, 0
		out		OCR1AH, b2			
		out		OCR1AL, b3
exit:
.endmacro


;-----------------------------------------------
;+++ DROITE: valeur à augmenter l'angle	     +++
;-----------------------------------------------
.macro	DIMIN
		in		b3, OCR1AL
		subi	b3, @0
		cpi		b3, deg_0
		brmi	exit							; branch if minus	
		ldi		b2, 0
		out		OCR1AH, b2			
		out		OCR1AL, b3
exit:
.endmacro


;-----------------------------------------------
;+++ SET_A: imposer un angle au moteur-servo +++
;-----------------------------------------------
.macro	SET_A
		ldi	w, 0
		out	OCR1AH, w
		ldi	w, @0
		out	OCR1AL, w
.endmacro	


;-----------------------------------------------
;+++ Jump to look-up table				     +++
;-----------------------------------------------
.macro		J_TABLE									; reg,tbl	
	ldi		ZL, low(2*@1)
	ldi		ZH, high(2*@1)
	lsl		@0
	add		ZL, @0
	brcc	PC+2
	inc		ZH
	lpm		r0, Z+
	push	r0
	lpm
	mov		zh, r0
	pop		zl
	icall	

.endmacro





