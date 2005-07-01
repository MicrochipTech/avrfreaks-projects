		
		;--------------------------------------
		;strobe LCD_E and LCD_RS pins for storing the data
		;
		;--------------------------------------
		
		
		
		sbi PORTC,LCD_RS
		sbi PORTC,LCD_E
		nop
		nop
		nop
		cbi PORTC,LCD_E
		cbi PORTC,LCD_RS

ret