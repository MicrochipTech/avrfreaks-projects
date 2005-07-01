		;--------------------------------------
		;strobe LCD_E pin for store the command 
		;for the initialization as LCD_RS=0
		;--------------------------------------
		
		cbi PORTC,LCD_E
		nop
		nop
		nop
		nop
		sbi PORTC,LCD_E
		nop
		nop
		nop
		nop
		nop
		cbi PORTC,LCD_E
		nop

ret