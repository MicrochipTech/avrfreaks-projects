

;**********************************************************
;   PRECISION DELAY ROUTINES(FUNCTION FORMAT MACROS)                       
;   ASSEMBLER:AVR ASM1
;   IDE:AVR STUDIO 4.12
;   AUTHOR: OMID KOMPANI    
;           TEHRAN-IRAN
;           30/10/05  ---- 1384/8/8
;**********************************************************




;***********************************************************************
;MAKES A DELAY WITH uS TIME BASE
;***********************************************************************

.MACRO	_WAIT_uS

	LDI		R26,((@0/1000000)*@1/4)
WAIT_uS_LOOP1:
	DEC		R26		
	NOP
	BRNE	WAIT_uS_LOOP1

.ENDMACRO

;***********************************************************************
;MAKES A DELAY WITH 5uS TIME BASE
;***********************************************************************

.MACRO	_WAIT_5us

	LDI		R26,((@0/1000000)*@1/8)
WAIT_5uS_LOOP1:
	LDI		R25,9
WAIT_5uS_LOOP2:
	DEC		R25	
	NOP
	BRNE	WAIT_5uS_LOOP2
	DEC		R26
	NOP
	BRNE	WAIT_5uS_LOOP1

.ENDMACRO

;***********************************************************************
;MAKES A DELAY WITH 10uS TIME BASE
;***********************************************************************

.MACRO	_WAIT_10us
	
	LDI		R26,((@0/1000000)*@1/8)
WAIT_10uS_LOOP1:
	LDI		R25,19
WAIT_10uS_LOOP2:
	DEC		R25	
	NOP
	BRNE	WAIT_10uS_LOOP2
	DEC		R26
	NOP
	BRNE	WAIT_10uS_LOOP1

.ENDMACRO

;***********************************************************************
;MAKES A DELAY WITH mS TIME BASE
;***********************************************************************

.MACRO	_WAIT_mS

	LDI		R26,(@0/1000000)* @1
WAIT_mS_LOOP1:
	LDI		R25,249
WAIT_mS_LOOP2:	
	DEC		R25		
	NOP				
	BRNE	WAIT_mS_LOOP2		
	DEC		R26		
	NOP
	BRNE	WAIT_mS_LOOP1
	
.ENDMACRO

;***********************************************************************
;MAKES A DELAY WITH 10mS TIME BASE
;***********************************************************************
.MACRO	_WAIT_10mS

	LDI		R26,(@0/1000000)*@1
WAIT_10mS_LOOP1:
	LDI		R25,243
WAIT_10mS_LOOP2:	
	DEC		R25		
	NOP
	NOP
	NOP
	NOP
	BRNE	WAIT_10mS_LOOP2		
	LDI		R25,255
WAIT_10mS_LOOP3:
	DEC		R25		
	NOP
	NOP
	NOP
	BRNE	WAIT_10mS_LOOP3	
	LDI		R25,252
WAIT_10mS_LOOP4:
	DEC		R25		
	NOP
	NOP
	NOP
	NOP
	BRNE	WAIT_10mS_LOOP4
	BRTS	WAIT_10mS_LOOP5
	SET
	RJMP	WAIT_10mS_LOOP1
WAIT_10mS_LOOP5:
	DEC		R26
	CLT
	BRNE	WAIT_10mS_LOOP1
	
.ENDMACRO






