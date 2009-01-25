
;==========================================================================
;                             LCD Pin Assignments
;==========================================================================
;.equ	LCD_DATA_PORT	= PORTB			;Data line
;.equ	LCD_DATA_DDR 	= DDRB			;
;.equ	LCD_DATA_BIT 	= 1
;.equ	LCD_CLK_PORT 	= PORTB			;Clock line
;.equ	LCD_CLK_DDR 	= DDRB			;
;.equ	LCD_CLK_BIT 	= 0
;.equ	LCD_CPL			= 16			; characters per line for module


;=============================LCD registers================================
.def	LCD_Arg			= R16
.def	LCD_Out			= R17
.def	LCD_Loop		= R18
;==========================================================================
;		LCD_Out bit useage
;
;		-----------------------: Always '1' - used with strobe of data for enable
;		| ---------------------: RS
;		| | -------------------: RW
;		| | | -----------------: D7
;		| | | | ---------------: D6
;		| | | | | -------------: D5
;		| | | | | | -----------: D4
;		| | | | | | | ---------: Always '0' - keep from triggering enable on shift
;		| | | | | | | |
;		X X X X X X X X
;	   b7			 b0
;
;
;
;
;								; This routine uses the serial LCD interface by Mike Predko
;								; found here: http://www.rentron.com/Myke1.htm
								






.DSEG

F_DUB:		.BYTE	1

.CSEG


.MACRO LCD_Cls
	LCD_Cmdi	0b00000001				; Clear display command
	Delay1ms 20							; to complete
.ENDM

.MACRO LCD_CursorULOn					; Turn cursor on
	SetB	F_DUB, 1
	_LCD_SetDUB
.ENDM

.MACRO LCD_CursorULOff					; Turn cursor off
	ClrB	F_DUB, 1
	_LCD_SetDUB
.ENDM

.MACRO LCD_CursorBlinkOn				; Turn blinking cursor on
	SetB	F_DUB, 0
	_LCD_SetDUB
.ENDM
.MACRO LCD_CursorBlinkOff				; Trn blinking cursor off
	ClrB	F_DUB, 0
	_LCD_SetDUB
.ENDM

.MACRO LCD_On							; Turn LCD on
	SetB	F_DUB, 2
	_LCD_SetDUB
.ENDM

.MACRO LCD_Off							; Turn LCD off
	ClrB	F_DUB, 2
	_LCD_SetDUB
.ENDM

.MACRO _LCD_SetDUB						; Send command ro control cursor
	push	LCD_Arg
	lds		LCD_Arg, F_DUB
	LCD_Cmd		LCD_ARG
	pop		LCD_Arg
.ENDM

.MACRO LCD_Locate						; LCD_Locate <row>, <col>
	push	LCD_Arg						; ex: LCD_Locate 1,9
	ldi 	LCD_Arg, @0
	swap 	LCD_Arg
	ori 	LCD_Arg, @1
	rcall 	_LCD_DoLocate
	pop		LCD_Arg
.ENDM

.MACRO LCD_CString						; LCD_CString	<string in CSEG>, <string length>
	PushZ								; ex: LCD_CString MyStr, 5
	push	LCD_Loop
	LoadZ	(2*@0)
	ldi		LCD_Loop, @1
	rcall 	_LCD_DoStringC
	pop		LCD_Loop
	PopZ
.ENDM

.MACRO LCD_DString						; LCD_DString	<string in DSEG>, <string length>
	PushZ								; ex: LCD_DString MyStr, 5
	push	LCD_Loop
	LoadZ	(@0)
	ldi		LCD_Loop, @1
	rcall 	_LCD_DoStringD
	pop		LCD_Loop
	PopZ
.ENDM

.MACRO LCD_CStringZ						; LCD_CStringZ	<zero terminated string in CSEG>
	PushZ								; ex: LCD_CStringZ MyStr
	LoadZ	(2*@0)
	rcall 	_LCD_DoStringCZ
	PopZ
.ENDM

.MACRO LCD_DStringZ						; LCD_DStringZ	<zero terminated string in DSEG>
	PushZ								; ex: LCD_DStringZ MyStr
	LoadZ	(@0)
	rcall 	_LCD_DoStringDZ
	PopZ
.ENDM


.MACRO LCD_CmdI							; Send comand constant (0..255) to LCD
	push	LCD_Out						; ex: LCD_CmdI 0x30
	push	LCD_Arg
	ldi		LCD_Arg, @0
	ldi		LCD_Out, 0b10000000			; set RS for instruction
	rcall	_LCD_Byte_Out
	pop		LCD_Arg
	pop		LCD_Out
.ENDM

.MACRO LCD_Cmd							; Send command in register (0..255) to LCD
	push	LCD_Out						; ex: LCD_Cmd r16
	push	LCD_Arg
	mov		LCD_Arg, @0
	ldi		LCD_Out, 0b10000000			; set RS for instruction
	rcall	_LCD_Byte_Out
	pop		LCD_Arg
	pop		LCD_Out
.ENDM

.MACRO LCD_Char							; Displays character in register on LCD at current display location
	push	LCD_Out						; ex: LCD_Char r16 
	push	LCD_Arg
	mov		LCD_Arg, @0
	ldi		LCD_Out, 0b11000000			; set RS for data
	rcall 	_LCD_Byte_Out
	pop		LCD_Arg
	pop		LCD_Out
.ENDM

.MACRO LCD_CharI						; Displays character on LCD at current display location
	push	LCD_Out						; ex: LCD_CharI 'G'
	push	LCD_Arg
	ldi		LCD_Arg, @0
	ldi		LCD_Out, 0b11000000			; set RS for data
	rcall 	_LCD_Byte_Out
	pop		LCD_Arg
	pop		LCD_Out
.ENDM

;*****************************************************
_LCD_Byte_Out:							; Sends byte to LCD
	bst		LCD_Arg, 7					; Input: byte to send in LCD_Arg
	bld		LCD_Out, 4					; transfers one nibble at a time
	bst		LCD_Arg, 6					; to LCD_Out and sends to LCD
	bld		LCD_Out, 3					; NOTE: this is designed to be called from the
	bst		LCD_Arg, 5					; macro(s) above
	bld		LCD_Out, 2
	bst		LCD_Arg, 4
	bld		LCD_Out, 1
	rcall	_LCD_NibbleOut				; send first nibble
	bst		LCD_Arg, 3
	bld		LCD_Out, 4
	bst		LCD_Arg, 2
	bld		LCD_Out, 3
	bst		LCD_Arg, 1
	bld		LCD_Out, 2
	bst		LCD_Arg, 0
	bld		LCD_Out, 1
	rcall	_LCD_NibbleOut				;send last nibble
	ret

;*****************************************************
_LCD_Wait500ns:  ; not exact, will give delay of 550ns at 20mhz clock
	nop
	nop
	nop
	nop
	ret

;*****************************************************
_LCD_NibbleOut:							; send nibble of data and control bits
	push	LCD_Arg
	ldi		LCD_Arg, 8
	ClrB	LCD_DATA_PORT, LCD_DATA_BIT
_LCD_NO1:
	rcall	_LCD_SendBit
	dec		LCD_Arg
	brne	_LCD_NO1
	pop		LCD_Arg
	sbi		LCD_DATA_PORT, LCD_DATA_BIT
	rcall	_LCD_SendBit
	cbi		LCD_DATA_PORT, LCD_DATA_BIT
	sbrc	LCD_Out, 6
	sbi		LCD_DATA_PORT, LCD_DATA_BIT
	rcall	_LCD_SendBit
	cbi		LCD_DATA_PORT, LCD_DATA_BIT
	sbrc	LCD_Out, 5
	sbi		LCD_DATA_PORT, LCD_DATA_BIT
	rcall	_LCD_SendBit
	cbi		LCD_DATA_PORT, LCD_DATA_BIT
	sbrc	LCD_Out, 4
	sbi		LCD_DATA_PORT, LCD_DATA_BIT
	rcall	_LCD_SendBit
	cbi		LCD_DATA_PORT, LCD_DATA_BIT
	sbrc	LCD_Out, 3
	sbi		LCD_DATA_PORT, LCD_DATA_BIT
	rcall	_LCD_SendBit
	cbi		LCD_DATA_PORT, LCD_DATA_BIT
	sbrc	LCD_Out, 2
	sbi		LCD_DATA_PORT, LCD_DATA_BIT
	rcall	_LCD_SendBit
	cbi		LCD_DATA_PORT, LCD_DATA_BIT
	sbrc	LCD_Out, 1
	sbi		LCD_DATA_PORT, LCD_DATA_BIT
	rcall	_LCD_SendBit
	cbi		LCD_DATA_PORT, LCD_DATA_BIT
	rcall	_LCD_SendBit
	rcall 	_LCD_Wait500ns
	sbi		LCD_DATA_PORT, LCD_DATA_BIT
	rcall 	_LCD_Wait500ns
	cbi		LCD_DATA_PORT, LCD_DATA_BIT
	Delay10us 10
	ret

;*****************************************************
_LCD_SendBit:							; send bit to shift register
	SetB	LCD_CLK_PORT, LCD_CLK_BIT
	nop
	nop
	nop
	nop
	ClrB	LCD_CLK_PORT, LCD_CLK_BIT
	nop
	ret

;*****************************************************
LCD_Init:
	
	
	push	LCD_Out
	ldi		LCD_Out, 0b00001111				; Initialize display on, cursor on, blink on
	sts		F_DUB, LCD_Out
	
	SetB	LCD_DATA_DDR, LCD_DATA_BIT		; Setup control lines
	ClrB	LCD_DATA_PORT, LCD_DATA_BIT
	SetB	LCD_CLK_DDR, LCD_CLK_BIT
	ClrB	LCD_CLK_PORT, LCD_CLK_BIT
	Delay1ms 50
	ldi		LCD_Out, 0b10000110
	rcall	_LCD_NibbleOut
	Delay1ms 10
	rcall	_LCD_NibbleOut
	Delay1ms 5
	rcall	_LCD_NibbleOut
	Delay1ms 1
	ldi		LCD_Out, 0b10000100
	rcall	_LCD_NibbleOut
	Delay1ms 1
	LCD_Cmdi	0b00101000
	LCD_Cmdi	0b00001000
	LCD_Cmdi	0b00000001
	Delay1ms 20
	LCD_Cmdi	0b00000110
	_LCD_SetDUB
	pop		LCD_Out
	LCD_Cls
	ret

;==============================================================================
_LCD_DoLocate:
	push	LCD_Loop
	push	LCD_Arg				; Save for later
	andi	LCD_Arg, 0b00110000		; Mask off row bits
	cpi		LCD_Arg, 0b00110000		; Row number 3?
	breq	_LCD_DL3				; Jump if so
	cpi		LCD_Arg, 0b00100000		; Row number 2?
	breq	_LCD_DL2				; Jump if so
	cpi		LCD_Arg, 0b00010000		; Row number 1?
	breq	_LCD_DL1				; Jump if so
	pop		LCD_Arg					; must be row 0, so restore LCD_Arg
	andi	LCD_Arg, 0b00001111		; mask off col bits
	rjmp	_LCD_DLDone				; all done
_LCD_DL1:
	pop		LCD_Arg					; restore LCD_Arg
	andi	LCD_Arg, 0b00001111		; mask off col bits
	ldi		LCD_Loop, 0x40
	add		LCD_Arg, LCD_Loop			; Row 1 starts at ox40
	rjmp	_LCD_DLDone				; all done
_LCD_DL2:
	pop		LCD_Arg					; restore LCD_Arg
	andi	LCD_Arg, 0b00001111		; mask off col bits
	ldi		LCD_Loop, LCD_CPL
	add		LCD_Arg, LCD_Loop		; Row 2 starts at ox14
	rjmp	_LCD_DLDone				; all done
_LCD_DL3:
	pop		LCD_Arg					; restore LCD_Arg
	andi	LCD_Arg, 0b00001111		; mask off col bits
	ldi		LCD_Loop, 0x40 + LCD_CPL
	add		LCD_Arg, LCD_Loop		; Row 3 
_LCD_DLDone:
	ori		LCD_Arg, 0x80			; Set bit for set DD ram address
	LCD_Cmd LCD_Arg
	pop		LCD_Loop
	ret

;======================================================================
_LCD_DoStringCZ:					; called by macro LCD_CStringZ
	push	LCD_Out
	push	LCD_Arg
	ldi		LCD_Out, 0b11000000
_LCD_DSCZ1:	
	lpm		LCD_Arg, Z+
	tst		LCD_Arg
	breq	_LCD_DSCZDone
	rcall	_LCD_Byte_Out
	rjmp	_LCD_DSCZ1
_LCD_DSCZDone:
	pop		LCD_Arg
	pop		LCD_Out
	ret

;======================================================================
_LCD_DoStringDZ:					; called by macro LCD_DStringZ
	push	LCD_Out
	push	LCD_Arg
	ldi		LCD_Out, 0b11000000
_LCD_DSDZ1:	
	ld		LCD_Arg, Z+
	tst		LCD_Arg
	breq	_LCD_DSDZDone
	rcall	_LCD_Byte_Out
	rjmp	_LCD_DSDZ1
_LCD_DSDZDone:
	pop		LCD_Arg
	pop		LCD_Out
	ret


;======================================================================
_LCD_DoStringC:					; called by macro LCD_CString
	push	LCD_Out
	push	LCD_Arg
	ldi		LCD_Out, 0b11000000
_LCD_DSC1:	
	lpm		LCD_Arg, Z+
	rcall	_LCD_Byte_Out
	dec		LCD_Loop
	breq	_LCD_DSCDone
	rjmp	_LCD_DSC1
_LCD_DSCDone:
	pop		LCD_Arg
	pop		LCD_Out
	ret

;======================================================================
_LCD_DoStringD:					; called by macro LCD_DString
	push	LCD_Out
	push	LCD_Arg
	ldi		LCD_Out, 0b11000000
_LCD_DSD1:	
	ld		LCD_Arg, Z+
	rcall	_LCD_Byte_Out
	dec		LCD_Loop
	breq	_LCD_DSDDone
	rjmp	_LCD_DSD1
_LCD_DSDDone:
	pop		LCD_Arg
	pop		LCD_Out
	ret
