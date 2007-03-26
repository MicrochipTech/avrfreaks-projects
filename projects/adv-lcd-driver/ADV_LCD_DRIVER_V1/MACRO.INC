;--------------------------------------------------------------------------------------------------
;	FILE NAME:	MACRO.INC
;	
;	MACRO TYPE ROUTINES 
;	CREATED BY OMID KOMPANI- FARASINA CO - 	IRAN-TEHRAN
;	VERSION:2.0
;	LAST UPDATE: 1386-01-06		2007-MARCH-26
;	VERSION:1.0
;	LAST UPDATE: 1385-06-17		2006-SEP-08
;--------------------------------------------------------------------------------------------------

.CSEG

;**************************************************************************
;	INITIALIZING THE STACK POINTER AT THE TOP OF SRAM
;**************************************************************************
.MACRO	_INIT_STACK
	LDI		ZH,HIGH(RAMEND)
	OUT		SPH,ZH
	LDI		ZL,LOW(RAMEND)
	OUT		SPL,ZL
.ENDMACRO



;--------------------------------------------------------------------------------------------------
;
;	LOADING AND STORING FUNCTIONS
;
;--------------------------------------------------------------------------------------------------


;*************************************
;*  _LOAD_8BIT_RAM
;*************************************
.MACRO	_8RAM2REG
	LDS		@1,@0
.ENDMACRO

;*************************************
;*  _STORE_8BIT_RAM
;*************************************
.MACRO	_8REG2RAM
	STS		@0,@1
.ENDMACRO


;*************************************
;*  _LOAD_16BIT_RAM
;*************************************
.MACRO	_16RAM2REG
	LDI		YL,LOW(@0)
	LDI		YH,HIGH(@0)
	LD		@2,Y+
	LD		@1,Y
.ENDMACRO

;*************************************
;*  _STORE_16BIT_RAM
;*************************************
.MACRO	_16REG2RAM
	LDI		YL,LOW(@0)
	LDI		YH,HIGH(@0)
	ST		Y+,@2
	ST		Y+,@1
.ENDMACRO

;*************************************
;*  LOAD 32BIT FROM RAM TO REGISTER
;*************************************
.MACRO	_32RAM2REG
	LDI		YL,LOW(@0)
	LDI		YH,HIGH(@0)
	LD		@4,Y+
	LD		@3,Y+
	LD		@2,Y+
	LD		@1,Y
.ENDMACRO

;*************************************
;*  STORE 32BIT FROM REGISTER TO RAM
;*************************************
.MACRO	_32REG2RAM
	LDI		YL,LOW(@0)
	LDI		YH,HIGH(@0)
	ST		Y+,@4
	ST		Y+,@3
	ST		Y+,@2
	ST		Y,@1
.ENDMACRO




;--------------------------------------------------------------------------------------------------
;                                       - CONDITIONAL MACROS -
;--------------------------------------------------------------------------------------------------
;--------------------------------------------------------------------------------------------------
.MACRO  _IF_BIT_SET_CALL
    SBRC    @0,@1        
    CALL    @2
.ENDMACRO

;--------------------------------------------------------------------------------------------------
;
;--------------------------------------------------------------------------------------------------
.MACRO  _IF_BIT_SET_RCALL
    SBRC    @0,@1        
    RCALL    @2

.ENDMACRO
;--------------------------------------------------------------------------------------------------
;
;--------------------------------------------------------------------------------------------------
.MACRO  _IF_BIT_SET_JUMP
    SBRC    @0,@1        
    JMP     @2

.ENDMACRO
    
;--------------------------------------------------------------------------------------------------
;
;--------------------------------------------------------------------------------------------------
.MACRO  _IF_BIT_SET_RJUMP
    SBRC    @0,@1        
    RJMP    @2

.ENDMACRO

;--------------------------------------------------------------------------------------------------
;
;--------------------------------------------------------------------------------------------------
.MACRO  _IF_BIT_NOTSET_CALL
    SBRS    @0,@1        
    CALL    @2

.ENDMACRO

;--------------------------------------------------------------------------------------------------
;
;--------------------------------------------------------------------------------------------------
.MACRO  _IF_BIT_NOTSET_RCALL
    SBRS    @0,@1        
    RCALL   @2
.ENDMACRO

;--------------------------------------------------------------------------------------------------
;
;--------------------------------------------------------------------------------------------------
.MACRO  _IF_BIT_NOTSET_JUMP
    SBRS    @0,@1        
    JMP    @2
.ENDMACRO

;--------------------------------------------------------------------------------------------------
;
;--------------------------------------------------------------------------------------------------
.MACRO  _IF_BIT_NOTSET_RJUMP
    SBRS    @0,@1        
    RJMP    @2
.ENDMACRO

;--------------------------------------------------------------------------------------------------
;
;--------------------------------------------------------------------------------------------------

.MACRO	_SKIP_IF_BIT_NOTSET			
	SBRC	@0,@1
.ENDMACRO	
;--------------------------------------------------------------------------------------------------
;
;--------------------------------------------------------------------------------------------------

.MACRO	_SKIP_IF_BIT_SET			
	SBRS	@0,@1
.ENDMACRO
;--------------------------------------------------------------------------------------------------
;
;--------------------------------------------------------------------------------------------------


.MACRO _CBR ; Register,Bit#
		CLT
		BLD		@0,@1
.ENDMACRO

.MACRO _SBR ; Register,Bit#
		SET
		BLD 	@0,@1
.ENDMACRO



;*************************************************************************
;*************************************************************************
;   DELAY MACROS
;*************************************************************************
;*************************************************************************

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
;DISPLAYS A MESSAGE PARTIAL OR FULL(NULL OR ZERO ENDED)
;***********************************************************************
.MACRO	_LCD_DISPLAY_MSG 
	LDI		ZH,HIGH(@0*2)
	LDI		ZL,LOW(@0*2)
.IF	@1==1
    LDI     YH,HIGH(RAM_DISPLAY_BUFFER_LINE1)
    LDI     YL,LOW(RAM_DISPLAY_BUFFER_LINE1) 

.ELIF @1==2
    LDI     YH,HIGH(RAM_DISPLAY_BUFFER_LINE2)
    LDI     YL,LOW(RAM_DISPLAY_BUFFER_LINE2)
.ENDIF
    LDI     R16,@2      
	LDI		R17,@3
.IF @3==__NULL | @3==__ZERO
	RCALL	LOAD_TERMINATEDstr_FLASH2LCDBUFFER
.ELSE	
	RCALL	LOAD_CHARstr_FLASH2LCDBUFFER
.ENDIF
.ENDMACRO
;***********************************************************************
;	DISPLAYS FORMATTED NUMBERS
;	16,24 OR 32 BIT NUMBERS WITH SIGN AND DECIMAL POINT
;
;***********************************************************************
.MACRO	_LCD_DISPLAY_NUM

.IF	  @0==__16BIT	
	MOV		fBINH,@5
	MOV		fBINL,@6
.IF	@1==__SIGNED
	BST		fBINH,7
	SBRC	fBINH,7
	RCALL	NEG16
.ENDIF
;	CALL	BIN2BCD16	; UNCOMMENT THIS LINE FOR ABSOLUTE CALLS
	RCALL	BIN2BCD16	; UNCOMMENT THIS LINE FOR RELATIVE CALLS LOWER THAN +,-2K
.IF	@2==1
    LDI     YH,HIGH(RAM_DISPLAY_BUFFER_LINE1)
    LDI     YL,LOW(RAM_DISPLAY_BUFFER_LINE1) 
.ELIF @2==2
    LDI     YH,HIGH(RAM_DISPLAY_BUFFER_LINE2)
    LDI     YL,LOW(RAM_DISPLAY_BUFFER_LINE2)
.ENDIF
    LDI     R16,@3
	LDI		R17,@4
	LDI		R19,3
	LDI		ZL,22
	CLR		ZH
	RCALL	BCD2LCDBUFFER



.ELIF @0==__24BIT
	MOV		fBIN2,@5
	MOV		fBIN1,@6
	MOV		fBIN0,@7
.IF	@1==__SIGNED
	BST		fBIN2,7
	SBRC	fBIN2,7
	RCALL	NEG24
.ENDIF
;	CALL	BIN3BCD16	; UNCOMMENT THIS LINE FOR ABSOLUTE CALLS
	RCALL	BIN3BCD16	; UNCOMMENT THIS LINE FOR RELATIVE CALLS LOWER THAN +,-2K
.IF	@2==1
    LDI     YH,HIGH(RAM_DISPLAY_BUFFER_LINE1)
    LDI     YL,LOW(RAM_DISPLAY_BUFFER_LINE1) 
.ELIF @2==2
    LDI     YH,HIGH(RAM_DISPLAY_BUFFER_LINE2)
    LDI     YL,LOW(RAM_DISPLAY_BUFFER_LINE2)
.ENDIF	
    LDI     R16,@3
	LDI		R17,@4
	LDI		R19,4
	LDI		ZL,23
	CLR		ZH
	RCALL	BCD2LCDBUFFER	
			
.ELIF @0==__32BIT
	MOV		fBIN3,@5
	MOV		fBIN2,@6
	MOV		fBIN1,@7
	MOV		fBIN0,@8
.IF	@1==__SIGNED
	BST		fBIN3,7
	SBRC	fBIN3,7
	RCALL	NEG32
.ENDIF
;	CALL	BIN4BCD16	; UNCOMMENT THIS LINE FOR ABSOLUTE CALLS
	RCALL	BIN4BCD16	; UNCOMMENT THIS LINE FOR RELATIVE CALLS LOWER THAN +,-2K
.IF	@2==1
    LDI     YH,HIGH(RAM_DISPLAY_BUFFER_LINE1)
    LDI     YL,LOW(RAM_DISPLAY_BUFFER_LINE1) 
.ELIF @2==2
    LDI     YH,HIGH(RAM_DISPLAY_BUFFER_LINE2)
    LDI     YL,LOW(RAM_DISPLAY_BUFFER_LINE2)
.ENDIF	
    LDI     R16,@3
	LDI		R17,@4
	LDI		R19,5
	LDI		ZL,24
	CLR		ZH
	RCALL	BCD2LCDBUFFER
.ENDIF

.ENDMACRO