;----------------------------------------------------------------------------
;* this file contains the assemply code for the graphics drawing procedures *
;----------------------------------------------------------------------------

PUT_PIXEL:
	;this function puts the color R16 in the pixel (XH:XL , YH:YL)
	PUSH XH
	PUSH YL
	PUSH YH
	PUSH R17

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

	WAITX:
	SBIS PIND,2
	RJMP WAITX
	LDI R17,0b10000000      
	OUT MCUCR,R17		;ENABLE EXTERNAL SRAM	
	ST X,R16
	LDI R17,0b00000000
	OUT MCUCR,R17		;DISABLE EXTERNAL SRAM		

	POP R17
	POP YH
	POP YL
	POP XH	
	RET

;--------------------------------------------------------------------------

GET_PIXEL:
	;this function gets the color R16 in the pixel (XH:XL , YH:YL)
	PUSH XH
	PUSH YL
	PUSH YH
	PUSH R17

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

	WAITY:
	SBIS PIND,2
	RJMP WAITY
	LDI R17,0b10000000      
	OUT MCUCR,R17		;ENABLE EXTERNAL SRAM	
	LD R16,X
	LDI R17,0b00000000
	OUT MCUCR,R17		;DISABLE EXTERNAL SRAM		

	POP R17
	POP YH
	POP YL
	POP XH	
	RET
;--------------------------------------------------------------------------
CLS:
	;this function clears the screen
	CLR R16
	RCALL FILL_SCREEN
	RET
;--------------------------------------------------------------------------
FILL_SCREEN:
	;this function fills the screen with the color R16

		SEI			;ENABLE (GLOBAL INTERRUPTS)
		LDI R17,0b10000000      
		OUT MCUCR,R17		;ENABLE EXTERNAL SRAM

	CLR R30
	CLR R31

	CLR R17
	FS_LOOP:

	OUT PORTB,R17

	LDI R18,0b00000000
	OUT PORTC,R18

	SBR R31,0b10000000
	FS_LOOP1:
	ST Z+,R16
	ST Z+,R16
	ST Z+,R16
	ST Z+,R16
	ST Z+,R16
	ST Z+,R16
	ST Z+,R16
	ST Z,R16
	ADIW ZL,1
	BRCC FS_LOOP1

	LDI R18,0b10000000
	OUT PORTC,R18

	SBR R31,0b10000000
	FS_LOOP2:
	ST Z+,R16
	ST Z+,R16
	ST Z+,R16
	ST Z+,R16
	ST Z+,R16
	ST Z+,R16
	ST Z+,R16
	ST Z,R16
	ADIW ZL,1
	BRCC FS_LOOP2

	INC R17
	CPI R17,8
	BRNE FS_LOOP

		CLI
		LDI R17,0b00000000
		OUT MCUCR,R17		;DISABLE EXTERNAL SRAM	

	RET
;---------------------------------------------------------------------------
LINE:
	;this function draws a line staring from ((XH:XL),(YH:YL))
	;to ((R18:R17),(R20:R19)), with color R16

	CP XL,R17
	CPC XH,R18
	BRNE NOT_VERTICAL
	RJMP DRAW_VERTICAL_LINE
	NOT_VERTICAL:
	CP YL,R19
	CPC YH,R20
	BRNE NOT_HORIZONTAL 
	RJMP DRAW_HORIZONTAL_LINE
	NOT_HORIZONTAL:

	SUB R17,XL
	SBC R18,XH	;DX=(R18:R17)
	SUB R19,YL
	SBC R20,YH	;DY=(R20:R19)
	
	LDI R22,0x01
	LDI R23,0x00
	SBRS R18,7	;take the absolute value for DX 
	RJMP DX_POS
	DX_NEG:
	COM R17
	COM R18
	ADD R17,R22	;(R18:R17) = |DX|, R23:R22 = SGN(DX)
	ADC R18,R23
	LDI R22,0xFF
	LDI R23,0xFF
	DX_POS:

	LDI R24,0x01
	LDI R25,0x00
	SBRS R20,7	;take the absolute value for DY 
	RJMP DY_POS
	DY_NEG:
	COM R19
	COM R20
	ADD R19,R24	;(R20:R19) = |DY|, R25:R24 = SGN(DY)
	ADC R20,R25
	LDI R24,0xFF
	LDI R25,0xFF
	DY_POS:

	CP R17,R19
	CPC R18,R20
	BRLO DX_LOWER
	DY_LOWER:
	RJMP CASE_H
	DX_LOWER:
	RJMP CASE_V

	CASE_H:		;|DY| < |DX|
	MOV R30,R17
	MOV R31,R18
	CLR R6
	CLR R7
	MOV R8,R19
	MOV R9,R20
	CLR R10
	CLR R11		;CALCULATE SLOPE
	MOV R12,R17
	MOV R13,R18
	RCALL DIVIDE
	CLR R6
	CLR R7
	CASE_H_LOOP:
	RCALL PUT_PIXEL
	ADD XL,R22
	ADC XH,R23
	SBRS R25,7
	RJMP SGN_DY_POS
	SGN_DY_NEG:
	SUB R6,R2
	SBC R7,R3
	SBC YL,R4
	SBC YH,R5	
	RJMP CONT_CASE_H
	SGN_DY_POS:
	ADD R6,R2
	ADC R7,R3
	ADC YL,R4
	ADC YH,R5			
	CONT_CASE_H:
	SBIW R30,1
	BRNE CASE_H_LOOP	
	RET

	CASE_V:		;|DY| > |DX|
	MOV R30,R19
	MOV R31,R20
	CLR R6
	CLR R7
	MOV R8,R17
	MOV R9,R18
	CLR R10
	CLR R11		;CALCULATE SLOPE
	MOV R12,R19
	MOV R13,R20
	RCALL DIVIDE
	CLR R6
	CLR R7
	CASE_V_LOOP:
	RCALL PUT_PIXEL
	ADD YL,R24
	ADC YH,R25
	SBRS R23,7
	RJMP SGN_DX_POS
	SGN_DX_NEG:
	SUB R6,R2
	SBC R7,R3
	SBC XL,R4
	SBC XH,R5	
	RJMP CONT_CASE_V
	SGN_DX_POS:
	ADD R6,R2
	ADC R7,R3
	ADC XL,R4
	ADC XH,R5			
	CONT_CASE_V:
	SBIW R30,1
	BRNE CASE_V_LOOP	
	RET
;		---------------------------------------------------
DRAW_VERTICAL_LINE:
	CP YL,R19
	CPC YH,R20
	BREQ END_V_LINE
	BRLO Y_V_OK
	RCALL SWAP_POINTS
	Y_V_OK:
	SUB R19,YL
	SBC R20,YH
	MOV R17,R19
	MOV R18,R20
	RCALL VLINE
	END_V_LINE:
	RET
;		---------------------------------------------------
DRAW_HORIZONTAL_LINE:
	CP XL,R17
	CPC XH,R18
	BRLO X_H_OK
	RCALL SWAP_POINTS
	X_H_OK:
	SUB R17,XL
	SBC R18,XH
	RCALL HLINE
	RET
;		---------------------------------------------------

SWAP_POINTS:
	PUSH XH
	PUSH XL
	PUSH YH
	PUSH YL
	PUSH R18
	PUSH R17
	PUSH R20
	PUSH R19
	POP YL
	POP YH
	POP XL
	POP XH
	POP R19
	POP R20
	POP R17
	POP R18
	RET
;---------------------------------------------------------------------------

HLINE:
	;this function draws a horizontal line starting at ([XH:XL],[YH:YL]) 
	;of color R16 and length [R18:R17]
	PUSH XL
	PUSH XH
	PUSH R17
	PUSH R18
	HLINE_LOOP:
	RCALL PUT_PIXEL
	ADIW XL,1
	SUBI R17,1
	SBCI R18,0
	BRNE HLINE_LOOP
	POP R18
	POP R17
	POP XH
	POP XL
	RET
;---------------------------------------------------------------------------
VLINE:
	;this function draws a vertical line starting at ([XH:XL],[YH:YL]) 
	;of color R16 and length [R18:R17]
	PUSH YL
	PUSH YH
	PUSH R17
	PUSH R18
	VLINE_LOOP:
	RCALL PUT_PIXEL
	ADIW YL,1
	SUBI R17,1
	SBCI R18,0
	BRNE VLINE_LOOP
	POP R18
	POP R17
	POP YH
	POP YL
	RET

;---------------------------------------------------------------------------
RECT:
	;this function draws a rectangle starting at ([XH:XL],[YH:YL])
	;of color R16 and height [R18:R17] and length [R20:R19]

	PUSH R0
	PUSH R1
	PUSH YL
	PUSH YH
	MOV R0,R17
	MOV R1,R18
	MOV R17,R19
	MOV R18,R20
	RCALL HLINE
	ADD YL,R0
	ADC YH,R1
	SUBI YL,1
	SBCI YH,0
	RCALL HLINE
	MOV R17,R0
	MOV R18,R1
	POP YH
	POP YL
	POP R1
	POP R0

	PUSH XL
	PUSH XH
	RCALL VLINE
	ADD XL,R19
	ADC XH,R20
	SUBI XL,1
	SBCI XH,0
	RCALL VLINE
	POP XH
	POP XL
	RET
;---------------------------------------------------------------------------
FILL_RECT:
	;this function draws a filled rectangle starting at ([XH:XL],[YH:YL])
	;of edge color R21, fill color R16, height [R18:R17] and length [R20:R19]

		SEI			;ENABLE (GLOBAL INTERRUPTS)
		LDI R31,0b10000000      
		OUT MCUCR,R31		;ENABLE EXTERNAL SRAM

	PUSH XL
	PUSH XH
	PUSH YL
	PUSH YH
	PUSH R17
	PUSH R18

	NEXT_LINE:

	MOV ZL,R19
	MOV ZH,R20

	PUSH XL
	PUSH XH
	PUSH YL
	PUSH YH
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
	DRAW_HLINE:	
	ST X+,R16
	SBIW ZL,1
	BRNE DRAW_HLINE		
	POP YH
	POP YL
	POP XH
	POP XL

	ADIW YL,1
	SUBI R17,1
	SBCI R18,0
	BRNE NEXT_LINE

	POP R18
	POP R17
	POP YH
	POP YL
	POP XH
	POP XL
 	MOV R16,R21

		CLI
		LDI R31,0b00000000
		OUT MCUCR,R31		;DISABLE EXTERNAL SRAM	

	RCALL RECT
	RET

;---------------------------------------------------------------------------
.include "copy2.asm"
;---------------------------------------------------------------------------
.include "mouse.asm"
;---------------------------------------------------------------------------
.include "chr.asm"
;---------------------------------------------------------------------------
.include "math.asm"
;---------------------------------------------------------------------------
