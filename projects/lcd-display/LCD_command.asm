		;-------------------------------------------------
		;set the command for the initialization
		;first sent the high bit, strobe LCD_E sent the
		;low nibble and strobe LCD_E again
		;-------------------------------------------------


			mov temp,command			;first sent the high nibble
			andi temp,MASK_HIGH
			mov command_high,temp
			out PORTC,command_high
			nop
			
			rcall LCD_strobe			;store high nibble
			rcall LCD_delay
			
			mov temp,command			;later sent the low nibble
			andi temp,MASK_LOW
			mov command_low,temp
			swap command_low
			out PORTC,command_low
			nop
			
			rcall LCD_strobe			;store low nibble
			rcall LCD_delay

ret