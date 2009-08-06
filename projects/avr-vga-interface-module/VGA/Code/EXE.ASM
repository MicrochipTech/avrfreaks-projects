EXECUTION_MODE:

SBI PORTD,0		;Disable command register output
LDI R16,0b00000111
OUT DDRB,R16		;SET PORTB[2,1,0] AS OUTPUT

;execution starts here
LDI ZL,0x00
LDI ZH,0x01

LD R20,Z+	;load command into R20

CPI R19,0
BREQ GROUP_0
CPI R19,1
BREQ GROUP_1
CPI R19,2
BREQ GROUP_2X
CPI R19,3
BREQ GROUP_3X
RJMP COMMAND_MODE	;if the group number is invalid then go out

GROUP_2X:
RJMP GROUP_2
GROUP_3X:
RJMP GROUP_3


;***********************************************************************
.include "group0.asm"
.include "group1.asm"
.include "group2.asm"
.include "group3.asm"
;************************************************************************