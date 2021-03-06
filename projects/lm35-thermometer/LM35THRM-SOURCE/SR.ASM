.CSEG

.ORG 0
	RJMP	INIT 

.ORG $09		
	RJMP	IH_TOV0 ;* INTERRUPT HANDLER FOR TIMER0 OVER FLOW
.ORG $0E
	RETI			;*ADC INTERRUPT

INIT:
    LDI 	TEMP,LOW(RAMEND) ; INIT STACK POINTER AT THE END OF SARAM
    OUT 	SPL,TEMP
    LDI 	TEMP,HIGH(RAMEND)
    OUT 	SPH,TEMP
	
    LDI 	TEMP,0XFF
    OUT 	DDRB,TEMP   ;*SETS PORTB AS ALL OUTPUT
    OUT 	DDRD,TEMP   ;*SETS PORTD AS ALL OUTPUT
    OUT 	PORTB,TEMP  ;*PORTB IS ACTIVE LOW/ALL LEDS ARE OFF 

	CLR		LONG_DELAY0
    CLR		LONG_DELAY1
	CLR		STATUS_FLAGS
	
	SBR		STATUS_FLAGS, 1 << TEST_MODE ;*EACH TIME SYSTEM STARTS FIRST 
	LDI 	TEMP,4		;*SETS ON ALL THE LEDS 
	OUT 	TCCR0,TEMP  ;*TIMER0 PRESCALER CLKSRC/256=>4000000/64=62500Hz
    LDI		TEMP,1
	OUT 	TIMSK,TEMP  ;*TOV0 ENABLED
    LDI 	TEMP,192            
    OUT 	TCNT0,TEMP  ;*LOADS TIMER0 FOR 4 ms 
    LDI		TEMP,0B11000000
	OUT		ADMUX,TEMP	;*SELECTS AD0 AND EXTERNAL ANALOG REFERENCE
	LDI		TEMP,0B10001111
	OUT		ADCSR,TEMP	;*ENABLES AD CONVERTER AND AD INTERRUPT
						;*AD SPEED=CLK SOURCE/128
    LDI 	ANODE_MUX,0B11111000	;*SELECTS BIT #3 FOR ANODE#4 AND
									;*EACH TIMEWE MUST SHIFT IT 1 BIT TO 
									;*RIGHT TO SELECT ANOTHER DIGIT
	ORI		TEMP,0XFF
	OUT		PORTD,TEMP
	SEI
	RJMP	MAIN
;***********************************************************************
;* TIMER0 INTERRUPT HANDLER ROUTIN
;***********************************************************************
IH_TOV0:
    IN 		LAST_STATUS,SREG
	LDI		TEMP,192	;*LOADS TIMER0 FOR NEXT 4mS
	OUT		TCNT0,TEMP
	SBR		STATUS_FLAGS,1 << TIME2DISPLAY ;*SETS THE DISPLAY FLAG
	INC		LONG_DELAY0
	INC		LONG_DELAY1
	LDI	  	TEMP,250
	CP		LONG_DELAY1,TEMP ;* CHECKS IF IT REACHES 1 SECOND TO END THE TEST MODE
	BRNE	DELAY_ADC
	CBR		STATUS_FLAGS,1 << TEST_MODE
DELAY_ADC:
	LDI		TEMP,125 ;*CHECKS IF TIMER REACHES 0.5 SECOND TO START NEXT CONVERSION
	CP		LONG_DELAY0,TEMP
	BRNE	END_OF_IH_TOV0	
	SBR		STATUS_FLAGS,1<<ADC0_COMPLETED	
END_OF_IH_TOV0:
	OUT		SREG,LAST_STATUS
	RETI

