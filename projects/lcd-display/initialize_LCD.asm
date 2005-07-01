	;-----------------------------------------
	;initialize LCD  at differents mode
	;----------------------------------------

		rcall LCD_init
		nop

		ldi command,0b00101000 	;clr display
		nop
		rcall LCD_command

		ldi command,0b00000001	;set the cursor at the first position
		nop
		rcall LCD_command

		ldi command,0b00000010
		nop
		rcall LCD_command

		ldi command,0b00000110
		nop
		rcall LCD_command

		ldi command,0b00001110
		nop
		rcall LCD_command
	;-----------------------------------------------------
.exit