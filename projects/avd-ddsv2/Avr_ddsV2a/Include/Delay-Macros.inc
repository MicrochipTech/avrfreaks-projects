

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

	LDI		R29,((@0/1000000)*@1/4)
WAIT_uS_LOOP1:
	DEC		R29		
	NOP
	BRNE	WAIT_uS_LOOP1

.ENDMACRO

;***********************************************************************
;MAKES A DELAY WITH 5uS TIME BASE
;***********************************************************************

.MACRO	_WAIT_5us

	LDI		R29,((@0/1000000)*@1/8)
WAIT_5uS_LOOP1:
	LDI		R28,9
WAIT_5uS_LOOP2:
	DEC		R28	
	NOP
	BRNE	WAIT_5uS_LOOP2
	DEC		R29
	NOP
	BRNE	WAIT_5uS_LOOP1

.ENDMACRO

;***********************************************************************
;MAKES A DELAY WITH 10uS TIME BASE
;***********************************************************************

.MACRO	_WAIT_10us
	
	LDI		R29,((@0/1000000)*@1/8)
WAIT_10uS_LOOP1:
	LDI		R28,19
WAIT_10uS_LOOP2:
	DEC		R28	
	NOP
	BRNE	WAIT_10uS_LOOP2
	DEC		R29
	NOP
	BRNE	WAIT_10uS_LOOP1

.ENDMACRO

;***********************************************************************
;MAKES A DELAY WITH mS TIME BASE
;***********************************************************************

.MACRO	_WAIT_mS

	LDI		R29,(@0/1000000)* @1
WAIT_mS_LOOP1:
	LDI		R28,249
WAIT_mS_LOOP2:	
	DEC		R28		
	NOP				
	BRNE	WAIT_mS_LOOP2		
	DEC		R29		
	NOP
	BRNE	WAIT_mS_LOOP1
	
.ENDMACRO

;***********************************************************************
;MAKES A DELAY WITH 10mS TIME BASE
;***********************************************************************
.MACRO	_WAIT_10mS

	LDI		R29,(@0/1000000)*@1
WAIT_10mS_LOOP1:
	LDI		R28,243
WAIT_10mS_LOOP2:	
	DEC		R28		
	NOP
	NOP
	NOP
	NOP
	BRNE	WAIT_10mS_LOOP2		
	LDI		R28,255
WAIT_10mS_LOOP3:
	DEC		R28		
	NOP
	NOP
	NOP
	BRNE	WAIT_10mS_LOOP3	
	LDI		R28,252
WAIT_10mS_LOOP4:
	DEC		R28		
	NOP
	NOP
	NOP
	NOP
	BRNE	WAIT_10mS_LOOP4
	BRTS	WAIT_10mS_LOOP5
	SET
	RJMP	WAIT_10mS_LOOP1
WAIT_10mS_LOOP5:
	DEC		R29
	CLT
	BRNE	WAIT_10mS_LOOP1
	
.ENDMACRO
