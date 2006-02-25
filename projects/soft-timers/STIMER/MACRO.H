
.NOLIST
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

;*************************************
;*  _LOAD_8BIT_RAM
;*************************************
.MACRO	_LOAD_8BIT_RAM
	LDS		@1,@0
.ENDMACRO

;*************************************
;*  _STORE_8BIT_RAM
;*************************************
.MACRO	_STORE_8BIT_RAM
;	LDI		YL,LOW(@0)
;	LDI		YH,HIGH(@0)
	STS		@0,@1
.ENDMACRO


;*************************************
;*  _LOAD_16BIT_RAM
;*************************************
.MACRO	_LOAD_16BIT_RAM
	LDI		YL,LOW(@0)
	LDI		YH,HIGH(@0)
	LD		@1,Y
	LD		@2,-Y
.ENDMACRO

;*************************************
;*  _STORE_16BIT_RAM
;*************************************
.MACRO	_STORE_16BIT_RAM
	LDI		YL,LOW(@0)
	LDI		YH,HIGH(@0)
	ST		Y,@1
	ST		-Y,@2
.ENDMACRO


.LIST



















