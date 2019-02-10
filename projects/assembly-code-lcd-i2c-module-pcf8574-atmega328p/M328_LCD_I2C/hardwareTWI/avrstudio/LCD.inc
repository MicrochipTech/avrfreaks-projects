lcd_putchar:
	push	LCD_Reg			;save LCD_Reg (it's destroyed in between)

	cbr	LCD_Reg, 0b00001111	;we have to write the high nibble of our LCD_Reg first
						;so mask off the low nibble
	sbr	LCD_Reg,(1<<LCD_RS)|(1<<LCD_E) 	;LCD_RS must be high to send data
;	sbr	LCD_Reg,(1<<LCD_RS)|(1<<LCD_E)|(1<<BackLight)	;if you have a BackLight
	rcall	WriteI2C
	rcall	PulseE			;strobe Enable

	pop	LCD_Reg			;restore the LCD_Reg
	swap	LCD_Reg			;write the LOW nibble of the LCD_Reg to
	cbr	LCD_Reg, 0b00001111	;HIGH port nibble

	sbr	LCD_Reg,(1<<LCD_RS)|(1<<LCD_E) 	;LCD_RS must be high to send data
;	sbr	LCD_Reg,(1<<LCD_RS)|(1<<LCD_E)|(1<<BackLight)	;if you have a BackLight
	rcall	WriteI2C
	rcall	PulseE			;strobe Enable

	ldi	temp,2			;2 mS delay
	mov	dcount,temp
	rcall	DelaymS
	ret
;-------------------------------------------------------------------
lcd_command:			;same as LCD_putchar, but with RS low!
	push	LCD_Reg		;send the upper nibble first

	cbr	LCD_Reg, 0b00001111	;clr lower nibble
	sbr	LCD_Reg,(1<<LCD_E)	;E high
;	sbr	LCD_Reg,(1<<LCD_E)|(1<<BackLight)	;if you have a BackLight
	rcall	WriteI2C
	rcall	PulseE
	
	pop	LCD_Reg			;send the lower nibble
	swap	LCD_Reg

	cbr	LCD_Reg, 0b00001111	;clr the lower nibble
	sbr	LCD_Reg,(1<<LCD_E)	;E high
;	sbr	LCD_Reg,(1<<LCD_E)|(1<<BackLight)	;if you have a BackLight
	rcall	WriteI2C
	rcall	PulseE
	ret
;-------------------------------------
PulseE:
	cbr	LCD_Reg,(1<<LCD_E) 	;strobe E
	rcall	WriteI2C
	ret
;--------------------------------------
LCD_init:
	ldi	temp,100			;100 mS delay
	mov	dcount,temp
	rcall	DelaymS

	ldi	count,3			;do it three times
LCD_Ini_Lp:
	ldi	LCD_Reg, 0x30		;LCD is still in 8-BIT MODE	
						
	sbr	LCD_Reg,(1<<LCD_E)	;E high (idle)
;	sbr	LCD_Reg,(1<<LCD_E)|(1<<BackLight)	;if you have a BackLight
	rcall	WriteI2C
	rcall	PulseE

	ldi	temp,10			;10 mS delay
	mov	dcount,temp
	rcall	DelaymS

	dec	count
	brne	LCD_Ini_Lp
	ldi	LCD_Reg, 0x30		;LCD is still in 8-BIT MODE	
;-------------------------------------
	ldi	LCD_Reg, 0x20		;change to 4-bit mode.
						;LCD is still in 8-BIT MODE
	sbr	LCD_Reg,(1<<LCD_E)	;E high (idle)
;	sbr	LCD_Reg,(1<<LCD_E)|(1<<BackLight)	;if you have a BackLight
	rcall	WriteI2C
	rcall	PulseE

	ldi	temp,1			;1 mS delay
	mov	dcount,temp
	rcall	DelaymS
	
	ldi	LCD_Reg, 0x28		;NOW: 2 lines, 5*7 font, 4-BIT MODE!
	rcall	LCD_command			;	

	ldi	temp,1			;1 mS delay
	mov	dcount,temp
	rcall	DelaymS

	ldi	LCD_Reg, 0x0C		;Display ON, cursor OFF, blink OFF
;	ldi	LCD_Reg, 0x0F		;Display on, cursor on, blinking ON
	rcall	LCD_command

	ldi	temp,1			;1 mS delay
	mov	dcount,temp
	rcall	DelaymS

	ldi	LCD_Reg, 0x01		;clear display, cursor -> home
	rcall	LCD_command
	
	ldi	temp,5			;need > 4 mS delay
	mov	dcount,temp
	rcall	DelaymS

	ldi	LCD_Reg, 0x06		;auto-inc cursor
	rcall	LCD_command

	ldi	temp,1			;1 mS delay
	mov	dcount,temp
	rcall	DelaymS

	ldi	LCD_Reg, 0x02		;return cursor to Home Display
	rcall	LCD_command

	ldi	temp,1			;1 mS delay
	mov	dcount,temp
	rcall	DelaymS
	ret
;---------Writes String to LCD display from Z pointer -------------
WrLCD:
	lpm	LCD_Reg,Z+
	rcall	lcd_putchar

	dec	count		;
	brne	WrLCD		;
	ret
;-------------------------------------
message1:
.db	"Hello, AVRFREAKS"

message2:
.db	"  AVR FORUM     "
;-------------------------------------
