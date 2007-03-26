.CSEG
;***********************************************************************
;* TIMER0 INTERRUPT HANDLER ROUTINE
;***********************************************************************
ISR_TOV0:
    IN      R1,SREG
    PUSH    TEMP
    LDI     TEMP,(255-250+1)
    OUT     TCNT0,TEMP
    PUSH    R17
	PUSH    R31
    PUSH    R30
CHECK_STIMER1:
    _IF_BIT_NOTSET_RJUMP    STIMER_FLAGS,_STIMER1_EN,END_OF_ISR_TOV0
    _8RAM2REG          RAM_STIMER1_CV,R16
    _8RAM2REG          RAM_STIMER1_PV,R17
    CP      R16,R17
    BRLO    INCREASE_STIMER1
	CLR		R16
    _SBR    STIMER_FLAGS,_STIMER1_CM
    RJMP	SAVE_STIMER1
INCREASE_STIMER1:
;================
	INC     R16
SAVE_STIMER1:
;==============
	_8REG2RAM	    RAM_STIMER1_CV,R16
END_OF_ISR_TOV0:
	_SBR	SYSTEM_FLAGS,__LCD_TICK

    POP     R30
    POP     R31
    POP     R17
	POP		TEMP
    OUT     SREG,R1
	RETI




;**********************************************************
;	SOFT TIMER#1
;**********************************************************
ON_STIMER1:
    _CBR     STIMER_FLAGS,(_STIMER1_CM)

	_32RAM2REG		RAM_CHRONOMETER,R5,R4,R3,R2
	LDI		R20,1
	ADD		R2,R20
	CLR		R20
	ADC		R3,R20
	ADC		R4,R20
	ADC		R5,R20
	_32REG2RAM		RAM_CHRONOMETER,R5,R4,R3,R2

	_LCD_DISPLAY_NUM		__32BIT,__UNSIGNED,__LCD_LINE2,2,9,R5,R4,R3,R2

END_OF_ON_STIMER1:
    RET
