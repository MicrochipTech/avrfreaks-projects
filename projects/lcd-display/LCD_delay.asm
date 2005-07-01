		
	
;-------------------------------------------------------
;delay function so as ÏC hasn't to wait the receive 
;answer for the LCD as the LCD is far slower for the ÏC
;-------------------------------------------------------

		ldi temp,0x0f
		mov r2,temp
	LCD_delay_outer:
			clr r3
	LCD_delay_inner:			
			dec r3
		brne LCD_delay_inner
			dec r2
		brne LCD_delay_outer




ret