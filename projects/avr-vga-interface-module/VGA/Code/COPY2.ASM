COPY_BLOCK:
	;this function copy the block starting at ([XH:XL],[YH:YL]) 
	;and with length [R16] and height [R17] to new position ([R19:R18],[R21:R20])
	
	;test motion direction
		SEI			;ENABLE (GLOBAL INTERRUPTS)
		LDI R22,0b10000000      
		OUT MCUCR,R22		;ENABLE EXTERNAL SRAM
	CP R18,XL
	CPC R19,XH
	BRNE X_NOT_EQ
	X_EQ:
	CP R20,YL
	CPC R21,YH
	BREQ END_COPY
	X_NOT_EQ:
	CP R20,YL
	CPC R21,YH
	BRLO COPY_UPPER
	RJMP COPY_BOTTOM

	END_COPY:
		CLI
		LDI R22,0b00000000
		OUT MCUCR,R22		;DISABLE EXTERNAL SRAM	
	RET

;----------------------------------------------------

	COPY_UPPER:
	LDI R24,1
	LDI R25,0
	COPY_UPPERX:
	RCALL READ_LINE
	RCALL WRITE_LINE
	ADIW YL,1
	ADD R20,R24
	ADC R21,R25
	DEC R17
	BRNE COPY_UPPERX
	RJMP END_COPY

	COPY_BOTTOM:
	MOV R24,R17
	LDI R25,0
	SBIW R24,1
	ADD YL,R24
	ADC YH,R25
	ADD R20,R24
	ADC R21,R25
	COPY_BOTTOMX:
	RCALL READ_LINE
	RCALL WRITE_LINE
	SBIW YL,1
	SUBI R20,1
	SBCI R21,0
	DEC R17
	BRNE COPY_BOTTOMX
	RJMP END_COPY




;**************************************************************************
READ_LINE:
	; this function reads the line starting from [X,Y] with length R16
	; and writes it to Z buffer

	LDI ZL,0x00
	LDI ZH,0x01
	PUSH XL
	PUSH XH
	PUSH YL
	PUSH YH
	PUSH R16
	CLC
	ROL YL
	ROL YH
	CLC
	ROL YL
	ROL YH	
	OUT PORTB,YH
	OR XH,YL	
	OUT PORTC,XH
	SBR XH,0b10000000	
	READ_LINE_LOOP:
	LD R22,X+
	ST Z+,R22
	DEC R16
	BRNE READ_LINE_LOOP
	POP R16
	POP YH
	POP YL
	POP XH	
	POP XL	
	RET

;**************************************************************************
WRITE_LINE:
	; this function writes a line from the Z buufer with length R16
	; and writes it to [(R19:R18),(R21:R20)] 

	LDI ZL,0x00
	LDI ZH,0x01
	PUSH XL
	PUSH XH
	PUSH YL
	PUSH YH
	PUSH R16
	MOV XL,R18
	MOV XH,R19
	MOV YL,R20
	MOV YH,R21
	CLC
	ROL YL
	ROL YH
	CLC
	ROL YL
	ROL YH	
	OUT PORTB,YH
	OR XH,YL	
	OUT PORTC,XH
	SBR XH,0b10000000	
	WRITE_LINE_LOOP:
	LD R22,Z+
	ST X+,R22
	DEC R16
	BRNE WRITE_LINE_LOOP
	POP R16
	POP YH
	POP YL
	POP XH	
	POP XL	
	RET


;**************************************************************************
