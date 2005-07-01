
	;the orders to sent text to LCD_PORT



		ldi ZH, high(text1<<1)     ; Initialize Z pointer
		ldi ZL,low(text1*2)
line1:	lpm
		mov char,r0
		tst r0
		breq data2
		rcall LCD_data
		inc ZL
		rjmp line1

data2:	ldi command,0b11000000	;set the cursor at the second line		
		nop
		rcall LCD_command
	
		ldi ZL,low(text2*2)
line2:	lpm
		mov char,r0
		tst r0
		breq main1
		rcall LCD_data
		inc ZL
		rjmp line2


main1:		rjmp main1


.exit