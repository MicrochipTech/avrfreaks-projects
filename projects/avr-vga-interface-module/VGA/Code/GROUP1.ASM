GROUP_1:
CPI R20,0x10
BREQ DO_PPX
CPI R20,0x11
BREQ DO_LINEX
CPI R20,0x12
BREQ DO_RECTX
CPI R20,0x13
BREQ DO_FRECTX
CPI R20,0x14
BREQ DO_segmentX
CPI R20,0x16
BREQ DO_COPYX
RJMP COMMAND_MODE

DO_PPX:		RJMP DO_PP
DO_LINEX:	RJMP DO_LINE
DO_RECTX:	RJMP DO_RECT
DO_FRECTX:	RJMP DO_FRECT
DO_segmentX:	RJMP DO_segment
DO_COPYX:	RJMP DO_COPY

;----------------------------------

DO_PP:
LD XL,Z+	;load XL
LD XH,Z+	;load XH
LD YL,Z+	;load YL
LD YH,Z+	;load YH
LD R16,Z+	;load COLOR
RCALL PUT_PIXEL
RJMP COMMAND_MODE

;----------------------------------

DO_RECT:
LD XL,Z+	;load XL
LD XH,Z+	;load XH
LD YL,Z+	;load YL
LD YH,Z+	;load YH
LD R19,Z+	;load LENGTH_L
LD R20,Z+	;load LENGTH_H
LD R17,Z+	;load HEIGHT_L
LD R18,Z+	;load HEIGHT_H
LD R16,Z+	;load COLOR
RCALL RECT
RJMP COMMAND_MODE

;----------------------------------

DO_FRECT:
LD XL,Z+	;load XL
LD XH,Z+	;load XH
LD YL,Z+	;load YL
LD YH,Z+	;load YH
LD R19,Z+	;load LENGTH_L
LD R20,Z+	;load LENGTH_H
LD R17,Z+	;load HEIGHT_L
LD R18,Z+	;load HEIGHT_H
LD R21,Z+	;load EDGE COLOR
LD R16,Z+	;load FILL COLOR
RCALL FILL_RECT
RJMP COMMAND_MODE

;----------------------------------

DO_segment:
LD XL,Z+	;load XL
LD XH,Z+	;load XH
LD YL,Z+	;load YL
LD YH,Z+	;load YH
LD R17,Z+	;load Counter

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

		SEI			;ENABLE (GLOBAL INTERRUPTS)
		LDI R16,0b10000000      
		OUT MCUCR,R16		;ENABLE EXTERNAL SRAM
segment_pixel:
LD R16,Z+
ST X+,R16
DEC R17
BRNE segment_pixel
		CLI
		LDI R16,0b00000000
		OUT MCUCR,R16		;DISABLE EXTERNAL SRAM	

RJMP COMMAND_MODE

;-----------------------------------

DO_LINE:
LD XL,Z+	;load XL
LD XH,Z+	;load XH
LD YL,Z+	;load YL
LD YH,Z+	;load YH
LD R17,Z+	;load LENGTH_L
LD R18,Z+	;load LENGTH_H
LD R19,Z+	;load HEIGHT_L
LD R20,Z+	;load HEIGHT_H
LD R16,Z+	;load LINE COLOR
RCALL LINE
RJMP COMMAND_MODE

;-----------------------------------

DO_COPY:
LD XL,Z+	;load XL
LD XH,Z+	;load XH
LD YL,Z+	;load YL
LD YH,Z+	;load YH
LD R16,Z+	;load LENGTH
LD R17,Z+	;load HEIGHT
LD R18,Z+	;load NEW XL
LD R19,Z+	;load NEW XH
LD R20,Z+	;load NEW YL
LD R21,Z+	;load NEW YH
RCALL COPY_BLOCK
RJMP COMMAND_MODE

;-----------------------------------
