	;the orders to sent text to LCD_PORT

main:
;--------------------------------------------------------------------
;---------display the input A at binary mode at the first line-------
;--------------------------------------------------------------------

data2:	ldi command,0b10000000	;set the cursor at the second line		
		nop
		rcall LCD_command
	
		in temp,pinA	
		out portD,temp

		mov value,temp
		;mov message,temp

;------------convert CHAR to ASCII for the LCD---------------- 

		ldi count, 8
itoacb_loop:
		rol value
		brcc is_0
		brcs is_1
is_0:	ldi char,'0'
		rcall LCD_data
		rjmp skip
is_1:   ldi char,'1'
		rcall LCD_data 
skip:	dec count
		brne itoacb_loop

;----------------------------------------------------------
;--------Display a text message at the second line---------
;----------------------------------------------------------	

		ldi command,0b11000000	;set the cursor at the second line		
		nop
		rcall LCD_command	

		ldi ZH, high(text1<<1)     ; Initialize Z pointer
		ldi ZL,low(text1*2)

line1:	lpm
		mov char,r0
		tst r0
		breq main
		rcall LCD_data
		inc ZL
		rjmp line1

		rjmp main
.exit
