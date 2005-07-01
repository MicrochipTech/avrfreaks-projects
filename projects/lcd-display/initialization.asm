	;---------------------------------------
	;---------initialization----------------
	;---------------------------------------
        clr temp	
		out DDRA,temp	;set PORTA for input
		ser temp
		out DDRD,temp	;set PORTD for output
		out DDRC,temp	;set PORTC(LCD_PORT) for output
		ser temp
		out PORTD,temp
		out PORTC,temp
	;-----------------------------------------
	;------initialize stack pointer-----------
	;-----------------------------------------
		ldi temp,low(RAMEND)
		out spl,temp	
		ldi temp,high(RAMEND)
		out sph,temp

.exit