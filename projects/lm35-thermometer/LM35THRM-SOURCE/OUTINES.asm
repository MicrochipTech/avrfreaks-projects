;****************************************************************
;* LOOK UP TABLE IN PROGRAM MEMORY FOR DECODING 7SEGMENT LED DISPLAY
;****************************************************************
TABLE_7SEGMENT: ; 0~9 7segment's digits
    ;	 0			1		   2          3          4          5
	.DB 0B11000000,0B11111001,0B10100100,0B10110000,0B10011001,0B10010010
    ;    6          7          8          9     	 BLANK		TEST_CHAR
	.DB 0B10000010,0B11111000,0B10000000,0B10011000,0B11111111,0B00000000

;***********************************************************************
;* ANODE MULTIPLEXING AND DISPLAY ROUTINE
;***********************************************************************
DISPLAY_TEMPERATURE:
	MOV		DISP_BUFFERH,PBCD_BYTEH 
	MOV		DISP_BUFFERL,PBCD_BYTEL
	RJMP	SEGMENT_SELECTOR

	
SEGMENT_SELECTOR:
	SBRC    ANODE_MUX,3	;*CHECKS IF IT'S ANODE4 TURN TO DISPLAY 
    RJMP    DISPLAY_DIGIT4
    SBRC    ANODE_MUX,2 ;*CHECKS IF IT'S ANODE3 TURN TO DISPLAY
    RJMP    DISPLAY_DIGIT3
    SBRC    ANODE_MUX,1 ;*CHECKS IF IT'S ANODE2 TURN TO DISPLAY
    RJMP    DISPLAY_DIGIT2
    SBRC    ANODE_MUX,0 ;*CHECKS IF IT'S ANODE1 TURN TO DISPLAY
    RJMP    DISPLAY_DIGIT1
	RET

DISPLAY_DIGIT4:
    SWAP    DISP_BUFFERH ;*SELECTS HIGH NIBBLE OF PBCD FOR 4TH DIGIT
    LDI		TEMP,0X0F
	AND	    DISP_BUFFERH,TEMP 
    LDI     ZL,LOW(2*TABLE_7SEGMENT) ;*WORD ORIENTED PROGRAM MEMORY ADDRESSING
    LDI     ZH,HIGH(2*TABLE_7SEGMENT)
    LDI		TEMP,0
	ADD     ZL,DISP_BUFFERH
 	LPM		;*LOAD FROM PROGRAM MEMORY TO R0
    OUT		PORTB,R0
    OUT 	PORTD,ANODE_MUX
    CBR		STATUS_FLAGS,1 << TIME2DISPLAY ;*CLEARS DISPLAY FLAG TILL NEXT 4ms
	ANDI	ANODE_MUX,0B11110000
	SBR		ANODE_MUX,4 ;*SELECTS ANODE3 FOR NEXT DISPLAY
	RET

DISPLAY_DIGIT3:
    LDI		TEMP,0X0F
	AND     DISP_BUFFERH,TEMP
    LDI     ZL,LOW(2*TABLE_7SEGMENT)
    LDI     ZH,HIGH(2*TABLE_7SEGMENT)
    LDI		TEMP,0
	ADD     ZL,DISP_BUFFERH
    LPM
    OUT 	PORTB,R0
    OUT 	PORTD,ANODE_MUX
    CBR		STATUS_FLAGS,1 << TIME2DISPLAY
	ANDI	ANODE_MUX,0B11110000
	SBR		ANODE_MUX,2
	RET

DISPLAY_DIGIT2:
    SWAP    DISP_BUFFERL
	LDI		TEMP,0X0F
    AND     DISP_BUFFERL,TEMP
    LDI     ZL,LOW(2*TABLE_7SEGMENT)
    LDI     ZH,HIGH(2*TABLE_7SEGMENT)
    LDI		TEMP,0
	ADD     ZL,DISP_BUFFERL
    LPM
    LDI	    TEMP,0B01111111
	AND 	R0,TEMP
	OUT 	PORTB,R0
    OUT 	PORTD,ANODE_MUX
    CBR		STATUS_FLAGS,1 << TIME2DISPLAY
	ANDI	ANODE_MUX,0B11110000
	SBR		ANODE_MUX,1
	RET

DISPLAY_DIGIT1:    
    LDI		TEMP,0X0F
	AND     DISP_BUFFERL,TEMP
    LDI     ZL,LOW(2*TABLE_7SEGMENT)
    LDI     ZH,HIGH(2*TABLE_7SEGMENT)
    LDI		TEMP,0
	ADD     ZL,DISP_BUFFERL
	LPM
    OUT 	PORTB,R0
    OUT 	PORTD,ANODE_MUX
    CBR		STATUS_FLAGS,1 << TIME2DISPLAY
	ANDI	ANODE_MUX,0B11110000
	SBR		ANODE_MUX,8
    RET   

;***********************************************************************
;* START ANALOG TO DIGITAL CONVERSION ROUTINE
;***********************************************************************
START_ADC:
	SBI		ADCSR,6 ;*START ADC
	SBR		STATUS_FLAGS,1 << ADC0_BUSY
	CLR		LONG_DELAY0
	RET	
;***********************************************************************
;*GET ADC ROUTINE
;***********************************************************************
GET_TEMPERATURE:

	IN		HEX_DIGITL,ADCL
	IN		HEX_DIGITH,ADCH
	CPI		HEX_DIGITL,0X3C;*COMPARES ADC VALUE WITH 30 DEGREE FOR OVER TEMP.
	BRSH	OVT
	ORI 	ANODE_MUX,0B00010000;*CLAERS THE ALARM BIT IN PORTD
	CBR		STATUS_FLAGS,1<<ADC0_BUSY
	CBR		STATUS_FLAGS,1<<ADC0_COMPLETED
	RCALL	BIN2PBCD
	RET

OVT:	
	ANDI	ANODE_MUX,0B11101111 ;*SETS THE ALARM BIT IN PORTD		
    CBR		STATUS_FLAGS,1<<ADC0_BUSY
	CBR		STATUS_FLAGS,1<<ADC0_COMPLETED
	RCALL	BIN2PBCD
	RET
TEST_DISPLAY:
	LDI		TEMP,0XBB
	LDI		TEMP,0XBB
	MOV		DISP_BUFFERL,TEMP
	MOV		DISP_BUFFERH,TEMP
	RCALL	SEGMENT_SELECTOR
	RJMP	MAIN
TEST_FINISHED:

	RJMP	MAIN
