			;--------------------------------------------------------
			;sent the ASCII characters.
			;first sent the high nibble and after sent the low nibble
			;--------------------------------------------------------

			mov temp,char			;first sent the high nibble
			andi temp,MASK_HIGH
			mov data_high,temp
			out PORTC,data_high
		
			rcall LCD_strobe_char
			rcall LCD_delay
			
			mov temp,char			;later sent the low nibble
			andi temp,MASK_LOW
			mov data_low,temp
			swap data_low
			out PORTC,data_low
	
			rcall LCD_strobe_char
			rcall LCD_delay

ret