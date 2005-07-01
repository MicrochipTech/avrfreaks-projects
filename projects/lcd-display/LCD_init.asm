		;----LCD_init for 4-bit mode-----
		;
		;------initiliaze 8-bit mode for 3 times-------------
		;with this trick display will be activated after power on
		;With out this display will be activated after reset mode
		;--------------------------------------------------------

		ldi temp,0b00110000	;initialization code for 8-bit mode
		out PORTC,temp
		nop
		rcall LCD_strobe
		rcall LCD_delay	

		ldi temp,0b00110000	;initialization code for 8-bit mode
		out PORTC,temp
		nop
		rcall LCD_strobe
		rcall LCD_delay	

	
		ldi temp,0b00110000	;initialization code for 8-bit mode
		out PORTC,temp
		nop
		rcall LCD_strobe
		rcall LCD_delay	
	
		;-----------------------------------------------------	
		;---------initialization code for 4-bit mode----------
		;-----------------------------------------------------
		ldi temp,0b00100000	
		out PORTC,temp
		nop
		rcall LCD_strobe
		rcall LCD_delay	

	
ret