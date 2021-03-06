GROUP_3:
CPI R20,0x30
BREQ DO_MC
CPI R20,0x31
BREQ DO_SM
CPI R20,0x32
BREQ DO_HM
RJMP COMMAND_MODE

DO_MC:
LDS R16,MOUSE_STATUS
CPI R16,0
BRNE DO_UM
LD XL,Z+	;load XL
LD XH,Z+	;load XH
LD YL,Z+	;load YL
LD YH,Z+	;load YH
STS MOUSE_XL,XL
STS MOUSE_XH,XH
STS MOUSE_YL,YL
STS MOUSE_YH,YH
RJMP COMMAND_MODE
DO_UM:
LD R20,Z+	;load XL
LD R21,Z+	;load XH
LD R22,Z+	;load YL
LD R23,Z+	;load YH
RCALL UPDATE_MOUSE
RJMP COMMAND_MODE

DO_SM:
RCALL SHOW_MOUSE
LDI R16,1
STS MOUSE_STATUS,R16
RJMP COMMAND_MODE

DO_HM:
RCALL HIDE_MOUSE
LDI R16,0
STS MOUSE_STATUS,R16
RJMP COMMAND_MODE