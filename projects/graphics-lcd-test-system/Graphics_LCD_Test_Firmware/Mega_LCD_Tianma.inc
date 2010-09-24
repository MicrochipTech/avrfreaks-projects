;********** ATmega Series LCD Subroutines **************************************
;Mega_LCD_Tianma.inc
;8-10-2010 CFS

;for Tianma TM12864G3CCWGWA-1 LCD with ST7567 controller

;calling program must set equates for
;LCD_PORT	port used for LCD control outputs
;LED_PORT port used for LED control output
;LCD_NRST	LCD_RST\ LCD reset output
;LCD_NCS	LCD_CS\ LCD select output
;LCD_A0	LCD data/control select (0=control, 1=display data) 
;LED_CTRL	LED backlight on/off and PWM control output

;lcd_buffer	1K SRAM graphics buffer

;*******************************************************************************
;* ROUTINE
;*	LCD_Init
;* DESCRIPTION
;*	Initialize SPI interface and LCD 
;*   for Tianma TM12864G3CCWGWA-1 LCD with ST7567 controller
;* USAGE
;*	none
;* STATISTICS
;*	Register usage: r16-r19
;* NOTE
;*
;*
;*
;*
;*
;*
;*******************************************************************************


					;set SPI as master
					;MSB first
					;mode 0 (SCLK low idle, data sampled on rising edge)
LCD_Init:	
;		ldi	r16,$51	;SCLK_freq=fosc/8	
		ldi	r16,$50	;SCLK_freq=fosc/2 (highest possible rate)
		out	SPCR,r16
		ldi	r16,$01
		out	SPSR,r16

					;toggle LCD_NRST low
		cbi	LCD_PORT,LCD_NRST

					;1 msec power up delay
		ldi	r16,4
		call	LCD_Delay
		
					;toggle LCD_NRST high
		sbi	LCD_PORT,LCD_NRST


					;1 msec reset delay
		ldi	r16,4
		call	LCD_Delay	

					;control commands to initialize LCD

					;set bias = 1/9
		ldi	r16,$A2
		call	LCD_Control_Write


					;set normal SEG scan direction
		ldi	r16,$A0
		call	LCD_Control_Write


					;set reverse COM scan direction
		ldi	r16,$C8
		call	LCD_Control_Write



					;set internal booster to 4X (2 byte command)
		ldi	r16,$F8
		call	LCD_Control_Write


					;set internal booster to 4X
		ldi	r16,$00
		call	LCD_Control_Write


					;set regulation ratio RR=5.0
		ldi	r16,$24
		call	LCD_Control_Write


					;set contrast (2 byte command)
		ldi	r16,$81
		call	LCD_Control_Write


					;set contrast
		ldi	r16,$1F
		call	LCD_Control_Write


					;set booster, regulator, and follower on
		ldi	r16,$2F
		call	LCD_Control_Write



					;set display start line = 0
		ldi	r16,$40
		call	LCD_Control_Write


					;set normal display (not all pixels on)
		ldi	r16,$A4
		call	LCD_Control_Write


					;set normal display (not inverted)
		ldi	r16,$A6
		call	LCD_Control_Write


					;set display on
		ldi	r16,$AF
		call	LCD_Control_Write


					;set LED backlight on	
		sbi	LED_PORT,LED_CTRL

				
					;clear display memory
		
					;r18 is page address counter
		clr	r18
		
					;r19 is column address counter
LCD_Init1:	clr	r19	
		
					;set page address
		mov	r16,r18
		call	LCD_Page_Addr_Set

					;set column address = 0
		clr	r16
		call LCD_Col_Addr_Set		


					;clear data
LCD_Init2:	clr	r16
		call	LCD_Data_Write
		
					;loop for columns 0-127 
		inc	r19
		cpi	r19,128
		brlo	LCD_Init2

					;loop for pages 0-7
		inc	r18
		cpi	r18,8
		brlo	LCD_Init1


					;reset column and page addresses to zero
		clr	r16
		call LCD_Col_Addr_Set

		clr	r16
		call	LCD_Page_Addr_Set


		ret					

					;millisecond delay routine
					;also used by other LCD routines
					;call with delay value in r16
					;.255 msec steps
					;based on 16 MHz clock
LCD_Delay:	ldi	r17,$FF
LCD_Delay1:	nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		dec	r17
		brne	LCD_Delay1
		dec	r16
		brne	LCD_Delay
		ret







;*******************************************************************************
;* ROUTINE
;*	LCD_Control_Write
;* DESCRIPTION
;*	Write control command to LCD
;* USAGE
;*	Command byte in r16
;* STATISTICS
;*	Register usage: r16
;* NOTE
;*
;*
;*
;*
;*
;*******************************************************************************

					;assert LCD_NCS (active low)
LCD_Control_Write:	cbi	LCD_PORT,LCD_NCS
	
					;A0 low for control command write
		cbi	LCD_PORT,LCD_A0

					;send control command byte to LCD
		out	SPDR,r16			 
					
					;wait for transmission complete
LCD_Control_Write1:	in	r16,SPSR
		sbrs	r16,SPIF
		rjmp	LCD_Control_Write1

					;de-assert LCD_NCS (active low)
		sbi	LCD_PORT,LCD_NCS
	
				
		ret


;*******************************************************************************
;* ROUTINE
;*	LCD_Data_Write
;* DESCRIPTION
;*	Write graphics data to LCD
;* USAGE
;*	Data byte in r16
;* STATISTICS
;*	Register usage: r16
;* NOTE
;*
;*
;*
;*
;*
;*******************************************************************************

					;assert LCD_NCS (active low)
LCD_Data_Write:	cbi	LCD_PORT,LCD_NCS
	
					;A0 high for graphics data write
		sbi	LCD_PORT,LCD_A0

					;send graphics data byte to LCD
		out	SPDR,r16			 
					
					;wait for transmission complete
LCD_Data_Write1:	in	r16,SPSR
		sbrs	r16,SPIF
		rjmp	LCD_Data_Write1

					;de-assert LCD_NCS (active low)
		sbi	LCD_PORT,LCD_NCS
	
				
		ret



;*******************************************************************************
;* ROUTINE
;*	LCD_Contrast_Set
;* DESCRIPTION
;*	Set LCD contrast (EV)
;* USAGE
;*	Contrast byte in r16 ($00-$3F)
;* STATISTICS
;*	Register usage: r16-17
;* NOTE
;*
;*
;*
;*
;*
;*******************************************************************************

					;assert LCD_NCS (active low)
LCD_Contrast_Set:	cbi	LCD_PORT,LCD_NCS
	
					;A0 low for control command write
		cbi	LCD_PORT,LCD_A0

		ldi	r17,$81

					;send set EV first byte to LCD
		out	SPDR,r17			 
					
					;wait for transmission complete
LCD_Contrast_Set1:	in	r17,SPSR
		sbrs	r17,SPIF
		rjmp	LCD_Contrast_Set1


		andi	r16,$3F			

					;send set EV second byte to LCD
		out	SPDR,r16			 
					
					;wait for transmission complete
LCD_Contrast_Set2:	in	r17,SPSR
		sbrs	r17,SPIF
		rjmp	LCD_Contrast_Set2

					;de-assert LCD_NCS (active low)
		sbi	LCD_PORT,LCD_NCS
	
				
		ret






;*******************************************************************************
;* ROUTINE
;*	LCD_Col_Addr_Set
;* DESCRIPTION
;*	Set LCD column address
;* USAGE
;*	Column address byte in r16
;* STATISTICS
;*	Register usage: r16-17
;* NOTE
;*
;*
;*
;*
;*
;*******************************************************************************

					;assert LCD_NCS (active low)
LCD_Col_Addr_Set:	cbi	LCD_PORT,LCD_NCS
	
					;A0 low for control command write
		cbi	LCD_PORT,LCD_A0

		mov	r17,r16
		swap	r17
		andi	r17,$0F			
		ori	r17,$10

					;send high column address bits to LCD
		out	SPDR,r17			 
					
					;wait for transmission complete
LCD_Col_Addr_Set1:	in	r17,SPSR
		sbrs	r17,SPIF
		rjmp	LCD_Col_Addr_Set1


		andi	r16,$0F			

					;send low column address bits to LCD
		out	SPDR,r16			 
					
					;wait for transmission complete
LCD_Col_Addr_Set2:	in	r17,SPSR
		sbrs	r17,SPIF
		rjmp	LCD_Col_Addr_Set2

					;de-assert LCD_NCS (active low)
		sbi	LCD_PORT,LCD_NCS
	
				
		ret



;*******************************************************************************
;* ROUTINE
;*	LCD_Page_Addr_Set
;* DESCRIPTION
;*	Set LCD page address
;* USAGE
;*	Page address byte in r16
;* STATISTICS
;*	Register usage: r16
;* NOTE
;*
;*
;*
;*
;*
;*******************************************************************************

					;assert LCD_NCS (active low)
LCD_Page_Addr_Set:	cbi	LCD_PORT,LCD_NCS
	
					;A0 low for control command write
		cbi	LCD_PORT,LCD_A0

		andi	r16,$0F
		ori	r16,$B0			

					;send page address bits to LCD
		out	SPDR,r16			 
					
					;wait for transmission complete
LCD_Page_Addr_Set1:	in	r16,SPSR
		sbrs	r16,SPIF
		rjmp	LCD_Page_Addr_Set1


					;de-assert LCD_NCS (active low)
		sbi	LCD_PORT,LCD_NCS
	
				
		ret


;*******************************************************************************
;* ROUTINE
;*	LCD_BufClear
;* DESCRIPTION
;*	Clear 1K SRAM graphics buffer
;* USAGE
;*	
;* STATISTICS
;*	Register usage: all pushed/popped
;* NOTE
;*	Requires SRAM buffer (lcd_buffer)
;*	Write is column/page order
;*	Execution time approx. 170 usec
;*
;*
;*
;*******************************************************************************

LCD_BufClear:	push	r0
		push	r16
		push	r28
		push	r29
					;buffer start address to Y
		ldi	r28,low(lcd_buffer)
		ldi	r29,high(lcd_buffer)
		
		clr	r0				

					;clear 8 bytes
					;this works since buffer length is a multiple of 8
LCD_BufClear_1:	st	Y+,r0
		st	Y+,r0
		st	Y+,r0
		st	Y+,r0
		st	Y+,r0
		st	Y+,r0
		st	Y+,r0	
		st	Y+,r0
		
					;loop to buffer end
		cpi	r28,low(lcd_buffer+1024)
		ldi	r16,high(lcd_buffer+1024)
		cpc	r29,r16
		brlo	LCD_BufClear_1

		pop	r29
		pop	r28
		pop	r16
		pop	r0

		ret







;*******************************************************************************
;* ROUTINE
;*	LCD_BufWrite
;* DESCRIPTION
;*	Write from 1K SRAM graphics buffer to LCD
;* USAGE
;*	
;* STATISTICS
;*	Register usage: all pushed/popped
;* NOTE
;*	Requires SRAM buffer (lcd_buffer)
;*	Write is column/page order
;*	Execution time approx. 1.7 msec
;*
;*
;*
;*******************************************************************************


					
LCD_BufWrite:	push	r16
		push	r17
		push	r18
		push	r30
		push	r31
					;assert LCD_NCS (active low)
		cbi	LCD_PORT,LCD_NCS

					;buffer address to Z
		ldi	r30,low(lcd_buffer)
		ldi	r31,high(lcd_buffer)						
						
					;r17 is page address counter
		clr	r17
		
					;r18 is column address counter
LCD_BufWrite1:	clr	r18	
		
					;A0 low for control command write
		cbi	LCD_PORT,LCD_A0

					;set page address
		mov	r16,r17
		andi	r16,$0F
		ori	r16,$B0			

					;send page address bits to LCD
		out	SPDR,r16			 
					
					;wait for transmission complete
LCD_BufWrite1A:	in	r16,SPSR
		sbrs	r16,SPIF
		rjmp	LCD_BufWrite1A

					;set column address = 0
		ldi	r16,$10
					;send high column address bits to LCD
		out	SPDR,r16			 
					
					;wait for transmission complete
LCD_BufWrite1B:	in	r16,SPSR
		sbrs	r16,SPIF
		rjmp	LCD_BufWrite1B


		clr	r16
					;send low column address bits to LCD
		out	SPDR,r16			 
					
					;wait for transmission complete
LCD_BufWrite1C:	in	r16,SPSR
		sbrs	r16,SPIF
		rjmp	LCD_BufWrite1C

				
					;A0 high for graphics data write
		sbi	LCD_PORT,LCD_A0

					;write data
					;wait for transmission complete
LCD_BufWrite2:		in	r16,SPSR
		sbrs	r16,SPIF
		rjmp	LCD_BufWrite2


		ld	r16,Z+

					;send graphics data byte to LCD
		out	SPDR,r16			 
					
					;loop for columns 0-127 
		inc	r18
		cpi	r18,128
		brlo	LCD_BufWrite2

					;wait for transmission complete
LCD_BufWrite2A:	in	r16,SPSR
		sbrs	r16,SPIF
		rjmp	LCD_BufWrite2A


					;loop for pages 0-7
		inc	r17
		cpi	r17,8
		brlo	LCD_BufWrite1

		pop	r31
		pop	r30
		pop	r18
		pop	r17
		pop	r16
	
					;de-assert LCD_NCS (active low)
		sbi	LCD_PORT,LCD_NCS
	
		ret


;*******************************************************************************
;* ROUTINE
;*	Char_Write6x8
;* DESCRIPTION
;*	Write 6x8 character to 1K SRAM graphics buffer
;*	
;* USAGE
;*	r20 is character (ASCII 32-127)
;*   r21 is character position (0-20)
;*	r22 is character row (same as LCD page number 0-7)
;*	r23 is character flag (b7 set to invert character pixels)   
;* STATISTICS
;*	Register usage: r20-r23, all others pushed/popped
;* NOTE
;*	Requires SRAM buffer (lcd_buffer)
;*	Execution time approx. 5 usec
;*
;*
;*******************************************************************************


					
Char_Write6x8:	push	r0
		push	r16
		push	r17
		push	r18
		push	r24
		push	r28
		push	r29
		push	r30
		push	r31
	
					;font table start address to Z
		ldi	r30,low(ffont_table_6x8)
		ldi	r31,high(ffont_table_6x8)
		lsl	r30
		rol	r31
					;character offset
					;font table starts at ASCII character 32
					;each character has six bytes
					;char_add = table_start + 6 * (char - 32)
		mov	r16,r20
		subi	r16,32
		ldi	r17,6
		call	mpy8u
		add	r30,r17
		adc	r31,r18
					;first character data byte now at Z
		
					;buffer start address to Y
		ldi	r28,low(lcd_buffer)
		ldi	r29,high(lcd_buffer)
					

					;calculate buffer offset 
					;buf_add = buf_start + (6 * char_pos) + 128 * (char_row)
		mov	r20,r22
		clr	r19
		lsr	r20
		ror	r19
		mov	r16,r21
		ldi	r17,6
		call	mpy8u
		clr	r18
		add	r19,r17
		adc	r20,r18
		add	r28,r19
		adc	r29,r20
		
					;set r24 for pixel inversion XOR if r23 flag bit set 
		clr	r24
		sbrc	r23,7
		ldi	r24,$FF
		
		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0	

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	
		eor	r0,r24
		st	Y,r0

		pop	r31
		pop	r30
		pop	r29
		pop	r28
		pop	r24
		pop	r18
		pop	r17
		pop	r16
		pop	r0

		ret



;*******************************************************************************
;* ROUTINE
;*	StringF_Write6x8
;* DESCRIPTION
;*	Write 6x8 character string from FLASH to 1K SRAM graphics buffer
;*	
;* USAGE
;*	Z (r31:r30) is pointer to string
;*   r21 is string starting position (0-20)
;*	r22 is string starting row (same as LCD page number 0-7)
;*	r23 is character flag (b7 set to invert character pixels)    
;* STATISTICS
;*	Register usage: r21-r23,r30-r31, all others pushed/popped
;* NOTE
;*	Requires SRAM buffer (lcd_buffer)
;*	Calls Char_Write6x8 (passes r21-r23)
;*	Execution time approx. 5 usec/character
;*
;*
;*******************************************************************************


					
StringF_Write6x8:	push	r20
	
		lsl	r30
		rol	r31


SFW6x8_1:	lpm	r20,Z+
		
		cpi	r20,$0D	;loop until terminator reached
		breq	SFW6x8_2
	
		call	Char_Write6x8

		inc	r21

		rjmp	SFW6x8_1

SFW6x8_2:	pop	r20
		
		ret


;*******************************************************************************
;* ROUTINE
;*	StringS_Write6x8
;* DESCRIPTION
;*	Write 6x8 character string from SRAM to 1K SRAM graphics buffer
;*	
;* USAGE
;*	Z (r31:r30) is pointer to string
;*   r21 is string starting position (0-20)
;*	r22 is string starting row (same as LCD page number 0-7)
;*	r23 is character flag
;*		b7 set to invert character pixels for entire string (overrides selective flag)
;*		b6 set to invert pixels in selected character (set by b4:b0)
;*		b0-b4 character position (0-15) to invert when b6 set	 
;*	r24 is number of characters   
;* STATISTICS
;*	Register usage: r21-r23,r30-r31, all others pushed/popped
;* NOTE
;*	Requires SRAM buffer (lcd_buffer)
;*	Calls Char_Write6x8 (passes r21-r23)
;*	Execution time approx. 5 usec/character
;*
;*
;*******************************************************************************


					
StringS_Write6x8:	push	r19
		push	r20


					;test individual character invert flag
					;if flag clear, pass r23 to character write routine and
					;jump ahead 
		sbrs	r23,6
		rjmp	SSW6x8_1	

					;individual character invert flag set
					;write string with one selected character inverted
		mov	r19,r23
		andi	r19,$1F
					;r19 has character position to be inverted

		rjmp	SSW6x8_2

					;write string without any individual character inverted
SSW6x8_1:	ld	r20,Z+

		
		call	Char_Write6x8

		inc	r21

		dec	r24
		brne SSW6x8_1

		rjmp	SSW6x8_4


					;write string with one selected character inverted
					;decrement r19
					;if r19=0, character position to be inverted has been
					;reached and r23 flag bit is set
SSW6x8_2:	clr	r23	
		dec	r19
		tst	r19
		brne	SSW6x8_3
		sbr	r23,1<<7	
		
SSW6x8_3:	ld	r20,Z+
		
		call	Char_Write6x8

		inc	r21

		dec	r24
		brne SSW6x8_2


SSW6x8_4:	pop	r20
		pop	r19

		ret






;*******************************************************************************
;* ROUTINE
;*	Char_Write8x16
;* DESCRIPTION
;*	Write 8x16 character to 1K SRAM graphics buffer
;*	
;* USAGE
;*	r20 is character (ASCII 32-127)
;*   r21 is character position (0-15)
;*	r22 is character row 0-3 (each takes two LCD pages)
;*	r23 is character flag (b7 set to invert character pixels)   
;* STATISTICS
;*	Register usage: r20-r23, all others pushed/popped
;* NOTE
;*	Requires SRAM buffer (lcd_buffer)
;*	Execution time approx. 8 usec
;*
;*
;*******************************************************************************


					
Char_Write8x16:	push	r0
		push	r16
		push	r17
		push	r18
		push	r19
		push	r24
		push	r28
		push	r29
		push	r30
		push	r31

					;font table start address to Z
		ldi	r30,low(ffont_table_8x16)
		ldi	r31,high(ffont_table_8x16)
		lsl	r30
		rol	r31
					;character offset
					;font table starts at ASCII character 32
					;each character has 16 bytes
					;char_add = table_start + 16* (char - 32)
		mov	r16,r20
		subi	r16,32
		ldi	r17,16
		call	mpy8u
		add	r30,r17
		adc	r31,r18
					;first character data byte now at Z
		
					;buffer start address to Y
		ldi	r28,low(lcd_buffer)
		ldi	r29,high(lcd_buffer)
					

					;calculate buffer offset 
					;buf_add = buf_start + (8 * char_pos) + (256 * char_row)
		mov	r20,r22
		clr	r19
		mov	r16,r21
		ldi	r17,8
		call	mpy8u
		clr	r18
		add	r19,r17
		adc	r20,r18
		add	r28,r19
		adc	r29,r20
					
					;set r24 for pixel inversion XOR if r23 flag bit set 
		clr	r24
		sbrc	r23,7
		ldi	r24,$FF
		
					;first 8 bytes (first page)
		lpm	r0,Z+
		eor	r0,r24	
		st	Y+,r0	

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0	

		lpm	r0,Z+
		eor	r0,r24	
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

	
					;last 8 bytes (second page)
		adiw	r28,63
		adiw	r28,57

		lpm	r0,Z+
		eor	r0,r24	
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm	r0,Z+
		eor	r0,r24
		st	Y+,r0

		lpm
		eor	r0,r24	
		st	Y,r0

		pop	r31
		pop	r30
		pop	r29
		pop	r28
		pop	r24
		pop	r19
		pop	r18
		pop	r17
		pop	r16
		pop	r0
	
		ret


;*******************************************************************************
;* ROUTINE
;*	StringF_Write8x16
;* DESCRIPTION
;*	Write 8x16 character string from FLASH to 1K SRAM graphics buffer
;*	
;* USAGE
;*	Z (r31:r30) is pointer to string
;*   r21 is string starting position (0-15)
;*	r22 is string starting row (same as LCD page number 0-3)
;*	r23 is character flag (b7 set to invert character pixels) 
;* STATISTICS
;*	Register usage: r21-r22,r30-r31, all others pushed/popped
;* NOTE
;*	Requires SRAM buffer (lcd_buffer)
;*	Calls Char_Write8x16 (passes r21-r23)
;*	Execution time approx. 5 usec/character
;*
;*
;*******************************************************************************


					
StringF_Write8x16:	push	r20
	
		lsl	r30
		rol	r31


SFW8x16_1:	lpm	r20,Z+
		
		cpi	r20,$0D	;loop until terminator reached
		breq	SFW8x16_2
	
		call	Char_Write8x16

		inc	r21

		rjmp	SFW8x16_1

SFW8x16_2:	pop	r20
		
		ret


;*******************************************************************************
;* ROUTINE
;*	StringS_Write8x16
;* DESCRIPTION
;*	Write 8x16 character string from SRAM to 1K SRAM graphics buffer
;*	
;* USAGE
;*	Z (r31:r30) is pointer to string
;*   r21 is string starting position (0-15)
;*	r22 is string starting row (same as LCD page number 0-3
;*	r23 is character flag
;*		b7 set to invert character pixels for entire string (overrides selective flag)
;*		b6 set to invert pixels in selected character (set by b4:b0)
;*		b0-b4 character position (0-15) to invert when b6 set	 
;*	r24 is number of characters  
;* STATISTICS
;*	Register usage: r21-r24,r30-r31, all others pushed/popped
;* NOTE
;*	Requires SRAM buffer (lcd_buffer)
;*	Calls Char_Write8x16 (passes r21-r23)
;*	Execution time approx. 5 usec/character
;*
;*
;*******************************************************************************


					
StringS_Write8x16:	push	r19
		push	r20

					;test individual character invert flag
					;if flag clear, pass r23 to character write routine and
					;jump ahead 
		sbrs	r23,6
		rjmp	SSW8x16_1	

					;individual character invert flag set
					;write string with one selected character inverted
		mov	r19,r23
		andi	r19,$1F
					;r19 has character position to be inverted

		rjmp	SSW8x16_2

					;write string without any individual character inverted
SSW8x16_1:	ld	r20,Z+
		
		call	Char_Write8x16

		inc	r21

		dec	r24
		brne SSW8x16_1

		rjmp	SSW8x16_4


					;write string with one selected character inverted
					;decrement r19
					;if r19=0, character position to be inverted has been
					;reached and r23 flag bit is set
SSW8x16_2:	clr	r23	
		dec	r19
		tst	r19
		brne	SSW8x16_3
		sbr	r23,1<<7	
		
SSW8x16_3:	ld	r20,Z+
		
		call	Char_Write8x16

		inc	r21

		dec	r24
		brne SSW8x16_2


SSW8x16_4:	pop	r20
		pop	r19

		ret




;*******************************************************************************
;* ROUTINE
;*	Graphic_Write128x64
;* DESCRIPTION
;*	Write 128x64 graphic image to 1K SRAM graphics buffer
;*	
;* USAGE
;*	Z (r31:r30) is pointer to graphic bitmap
;*
;* STATISTICS
;*	Register usage: r30-r31, all others pushed/popped
;* NOTE
;*	Requires SRAM buffer (lcd_buffer)
;*	Execution time approx. 360 usec
;*
;*
;*******************************************************************************


					
Graphic_Write128x64:	push	r0
		push	r16
		push	r28
		push	r29

		lsl	r30
		rol	r31
		
					;buffer start address to Y
		ldi	r28,low(lcd_buffer)
		ldi	r29,high(lcd_buffer)
					
					;write 8 bytes
					;this works since buffer length is a multiple of 8
GW128x64_1:	lpm	r0,Z+
		st	Y+,r0	

		lpm	r0,Z+
		st	Y+,r0

		lpm	r0,Z+
		st	Y+,r0

		lpm	r0,Z+
		st	Y+,r0

		lpm	r0,Z+
		st	Y+,r0

		lpm	r0,Z+
		st	Y+,r0

		lpm	r0,Z+
		st	Y+,r0

		lpm	r0,Z+
		st	Y+,r0

					;loop to buffer end
		cpi	r28,low(lcd_buffer+1024)
		ldi	r16,high(lcd_buffer+1024)
		cpc	r29,r16
		brlo	GW128x64_1

		pop	r29
		pop	r28
		pop	r16
		pop	r0

		ret


;*******************************************************************************
;* ROUTINE
;*	Graphic_Write
;* DESCRIPTION
;*	Write graphic image to 1K SRAM graphics buffer
;*	
;* USAGE
;*	Z (r31:r30) is pointer to graphic bitmap
;*   r20 is starting column (0-127)
;*	r21 is starting row (0-7)
;*	r22 is number of columns (1-128)
;*	r23 is number of rows (1-8)
;*	r24 is graphics flag (b7 set to invert pixels) 
;* STATISTICS
;*	Register usage: r20-24,r30-r31, all others pushed/popped
;* NOTE
;*	Requires SRAM buffer (lcd_buffer)
;*	Graphic must be correct size (columns and rows)
;*	Starting column and row must not cause graphic to overrun buffer area
;*	Execution time approx. 660 usec for full 128x64 graphic 
;*
;*
;*******************************************************************************


					
Graphic_Write:	push	r0
		push	r16
		push	r17
		push	r18
		push	r19
		push	r25
		push	r28
		push	r29

		lsl	r30
		rol	r31
		
					;set r25 for pixel inversion XOR if r24 flag bit set 
		clr	r25
		sbrc	r24,7
		ldi	r25,$FF




					;start at graphics column zero, row zero
					;use r19 as row_counter
					;clear row_counter
		clr	r19			 

					;buffer address for current row and first column of graphic is 
					;Y = Ybuffer_start + (128* starting_row + row_counter) + starting_column 
GW_1:			ldi	r28,low(lcd_buffer)
		ldi	r29,high(lcd_buffer)
		mov	r16,r21
		add	r16,r19
		ldi	r17,128
		call	mpy8u
		add	r28,r17
		adc	r29,r18
		clr	r16
		add	r28,r20
		adc	r29,r16

		
					;write bytes for current row
					;write bytes corresponding to number of columns
					;use r18 as column_counter
					;clear column_counter
		clr	r18

GW_2:	lpm	r0,Z+
		eor	r0,r25
		st	Y+,r0	
					;increment column_counter
					;if column_counter < number_of_columns
					;loop again
		inc	r18
		cp	r18,r22
		brlo	GW_2

					;done with current row
					;increment row_counter
					;if row_counter < number_of_rows
					;loop again

		inc	r19
		cp	r19,r23
		brlo	GW_1


		pop	r29
		pop	r28
		pop	r25
		pop	r19
		pop	r18
		pop	r17
		pop	r16
		pop	r0

		ret


						;6x8 font table
						;offset 32 ($20) 
						;ASCII characters $20-$7F
ffont_table_6x8:
.DB  $00, $00, $00, $00, $00, $00  ;space
.DB  $00, $00, $5F, $00, $00, $00  ;!
.DB  $00, $07, $00, $07, $00, $00  ;"
.DB  $14, $7F, $14, $7F, $14, $00  ;#
.DB  $24, $2A, $7F, $2A, $12, $00  ;$
.DB  $23, $13, $08, $64, $62, $00  ;%
.DB  $36, $49, $56, $20, $50, $00  ;&
.DB  $00, $08, $07, $03, $00, $00  ;'
.DB  $00, $1C, $22, $41, $00, $00  ;(
.DB  $00, $41, $22, $1C, $00, $00  ;)
.DB  $2A, $1C, $7F, $1C, $2A, $00  ;*
.DB  $08, $08, $3E, $08, $08, $00  ;+
.DB  $00, $80, $70, $30, $00, $00  ;,
.DB  $08, $08, $08, $08, $08, $00  ;-
.DB  $00, $00, $60, $60, $00, $00  ;.
.DB  $20, $10, $08, $04, $02, $00  ;/
.DB  $3E, $51, $49, $45, $3E, $00  ;0
.DB  $00, $42, $7F, $40, $00, $00  ;1
.DB  $42, $61, $51, $49, $46, $00  ;2
.DB  $21, $41, $49, $4D, $33, $00  ;3
.DB  $18, $14, $12, $7F, $10, $00  ;4
.DB  $27, $45, $45, $45, $39, $00  ;5
.DB  $3C, $4A, $49, $49, $30, $00  ;6
.DB  $41, $21, $11, $09, $07, $00  ;7
.DB  $36, $49, $49, $49, $36, $00  ;8
.DB  $06, $49, $49, $29, $1E, $00  ;9
.DB  $00, $00, $14, $00, $00, $00  ;:
.DB  $00, $40, $34, $00, $00, $00  ;;
.DB  $00, $08, $14, $22, $41, $00  ;<
.DB  $14, $14, $14, $14, $14, $00  ;=
.DB  $00, $41, $22, $14, $08, $00  ;>
.DB  $02, $01, $51, $09, $06, $00  ;?
.DB  $3E, $41, $5D, $59, $4E, $00  ;@
.DB  $7C, $12, $11, $12, $7C, $00  ;A
.DB  $7F, $49, $49, $49, $36, $00  ;B
.DB  $3E, $41, $41, $41, $22, $00  ;C
.DB  $7F, $41, $41, $41, $3E, $00  ;D
.DB  $7F, $49, $49, $49, $41, $00  ;E
.DB  $7F, $09, $09, $09, $01, $00  ;F
.DB  $3E, $41, $49, $49, $7A, $00  ;G
.DB  $7F, $08, $08, $08, $7F, $00  ;H
.DB  $00, $41, $7F, $41, $00, $00  ;I
.DB  $20, $40, $41, $3F, $01, $00  ;J
.DB  $7F, $08, $14, $22, $41, $00  ;K
.DB  $7F, $40, $40, $40, $40, $00  ;L
.DB  $7F, $02, $1C, $02, $7F, $00  ;M
.DB  $7F, $04, $08, $10, $7F, $00  ;N
.DB  $3E, $41, $41, $41, $3E, $00  ;O
.DB  $7F, $09, $09, $09, $06, $00  ;P
.DB  $3E, $41, $51, $21, $5E, $00  ;Q
.DB  $7F, $09, $19, $29, $46, $00  ;R
.DB  $26, $49, $49, $49, $32, $00  ;S
.DB  $01, $01, $7F, $01, $01, $00  ;T
.DB  $3F, $40, $40, $40, $3F, $00  ;U
.DB  $1F, $20, $40, $20, $1F, $00  ;V
.DB  $3F, $40, $38, $40, $3F, $00  ;W
.DB  $63, $14, $08, $14, $63, $00  ;X
.DB  $03, $04, $78, $04, $03, $00  ;Y
.DB  $61, $51, $49, $45, $43, $00  ;Z
.DB  $00, $7F, $41, $41, $41, $00  ;[
.DB  $02, $04, $08, $10, $20, $00  ;\
.DB  $00, $41, $41, $41, $7F, $00  ;]
.DB  $04, $02, $01, $02, $04, $00  ;^
.DB  $40, $40, $40, $40, $40, $00  ;_
.DB  $00, $03, $07, $08, $00, $00  ;`
.DB  $20, $54, $54, $54, $78, $00  ;a
.DB  $7F, $28, $44, $44, $38, $00  ;b
.DB  $38, $44, $44, $44, $28, $00  ;c
.DB  $38, $44, $44, $28, $7F, $00  ;d
.DB  $38, $54, $54, $54, $18, $00  ;e
.DB  $00, $08, $7E, $09, $02, $00  ;f
.DB  $18, $A4, $A4, $A4, $7C, $00  ;g
.DB  $7F, $08, $04, $04, $78, $00  ;h
.DB  $00, $44, $7D, $40, $00, $00  ;i
.DB  $20, $40, $40, $3D, $00, $00  ;j
.DB  $7F, $10, $28, $44, $00, $00  ;k
.DB  $00, $41, $7F, $40, $00, $00  ;l
.DB  $7C, $04, $78, $04, $78, $00  ;m
.DB  $7C, $08, $04, $04, $78, $00  ;n
.DB  $38, $44, $44, $44, $38, $00  ;o
.DB  $FC, $18, $24, $24, $18, $00  ;p
.DB  $18, $24, $24, $18, $FC, $00  ;q
.DB  $7C, $08, $04, $04, $08, $00  ;r
.DB  $48, $54, $54, $54, $24, $00  ;s
.DB  $04, $04, $3F, $44, $24, $00  ;t
.DB  $3C, $40, $40, $20, $7C, $00  ;u
.DB  $1C, $20, $40, $20, $1C, $00  ;v
.DB  $3C, $40, $30, $40, $3C, $00  ;w
.DB  $44, $28, $10, $28, $44, $00  ;x
.DB  $4C, $90, $90, $90, $7C, $00  ;y
.DB  $44, $64, $54, $4C, $44, $00  ;z
.DB  $00, $08, $36, $41, $00, $00  ;{
.DB  $00, $00, $77, $00, $00, $00  ;|
.DB  $00, $41, $36, $08, $00, $00  ;}
.DB  $02, $01, $02, $04, $02, $00  ;~
.DB  $3C, $26, $23, $26, $3C, $00  ;



						;8 x 16 font table
						;offset 32 ($20) 
						;ASCII characters $20-$7F
ffont_table_8x16:
.DB  $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00  ;space
.DB  $00, $00, $00, $FC, $FC, $00, $00, $00, $00, $00, $00, $33, $33, $00, $00, $00  ;!
.DB  $00, $FC, $FC, $00, $00, $FC, $FC, $00, $00, $00, $00, $00, $00, $00, $00, $00  ;"
.DB  $30, $FC, $FC, $30, $30, $FC, $FC, $30, $03, $0F, $0F, $03, $03, $0F, $0F, $03  ;#
.DB  $00, $78, $FC, $CF, $CF, $CC, $8C, $00, $00, $0C, $0C, $3C, $3C, $0F, $07, $00  ;$
.DB  $00, $1C, $1C, $C0, $F0, $3C, $0C, $00, $00, $0C, $0F, $03, $00, $0E, $0E, $00  ;%
.DB  $00, $CE, $FF, $F3, $3F, $0E, $00, $00, $1F, $3F, $30, $33, $1F, $3F, $33, $00  ;&
.DB  $00, $00, $00, $FC, $FC, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00  ;'
.DB  $00, $00, $00, $F0, $F8, $1C, $04, $00, $00, $00, $00, $0F, $1F, $38, $20, $00  ;(
.DB  $00, $04, $1C, $F8, $F0, $00, $00, $00, $00, $20, $38, $1F, $0F, $00, $00, $00  ;)
.DB  $C0, $CC, $FC, $F0, $F0, $FC, $CC, $C0, $00, $0C, $0F, $03, $03, $0F, $0C, $00  ;*
.DB  $00, $C0, $C0, $F8, $F8, $C0, $C0, $00, $00, $00, $00, $07, $07, $00, $00, $00  ;+
.DB  $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $C0, $7C, $3C, $00, $00, $00  ;,
.DB  $00, $C0, $C0, $C0, $C0, $C0, $C0, $00, $00, $00, $00, $00, $00, $00, $00, $00  ;-
.DB  $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $3C, $3C, $00, $00, $00  ;.
.DB  $00, $00, $00, $80, $E0, $7C, $1C, $00, $00, $38, $3E, $07, $01, $00, $00, $00  ;/
.DB  $00, $F8, $FC, $0C, $8C, $FC, $F8, $00, $00, $1F, $3F, $31, $30, $3F, $1F, $00  ;0
.DB  $00, $00, $30, $FC, $FC, $00, $00, $00, $00, $30, $30, $3F, $3F, $30, $30, $00  ;1
.DB  $00, $38, $3C, $0C, $CC, $FC, $38, $00, $00, $30, $3C, $3F, $33, $30, $30, $00  ;2
.DB  $00, $0C, $0C, $CC, $FC, $3C, $0C, $00, $00, $1C, $3C, $30, $33, $3F, $1C, $00  ;3
.DB  $00, $00, $C0, $F0, $FC, $FC, $00, $00, $00, $0F, $0F, $0C, $3F, $3F, $0C, $00  ;4
.DB  $00, $FC, $FC, $CC, $CC, $CC, $8C, $00, $00, $18, $38, $30, $30, $3F, $1F, $00  ;5
.DB  $00, $F0, $F8, $9C, $8C, $8C, $00, $00, $00, $1F, $3F, $31, $31, $3F, $1F, $00  ;6
.DB  $00, $0C, $0C, $0C, $CC, $FC, $3C, $00, $00, $00, $3C, $3F, $03, $00, $00, $00  ;7
.DB  $00, $38, $FC, $CC, $CC, $FC, $38, $00, $00, $1F, $3F, $30, $30, $3F, $1F, $00  ;8
.DB  $00, $78, $FC, $CC, $CC, $FC, $F8, $00, $00, $00, $30, $30, $38, $1F, $0F, $00  ;9
.DB  $00, $00, $00, $F0, $F0, $00, $00, $00, $00, $00, $00, $3C, $3C, $00, $00, $00  ;:
.DB  $00, $00, $00, $F0, $F0, $00, $00, $00, $00, $00, $C0, $7C, $3C, $00, $00, $00  ;;
.DB  $80, $C0, $E0, $70, $38, $18, $08, $00, $00, $01, $03, $07, $0E, $0C, $08, $00  ;<
.DB  $00, $30, $30, $30, $30, $30, $30, $00, $00, $03, $03, $03, $03, $03, $03, $00  ;=
.DB  $08, $18, $38, $70, $E0, $C0, $80, $00, $08, $0C, $0E, $07, $03, $01, $00, $00  ;>
.DB  $00, $38, $3C, $0C, $CC, $FC, $38, $00, $00, $00, $00, $37, $37, $00, $00, $00  ;?
.DB  $F0, $F8, $1C, $CC, $4C, $98, $F0, $00, $0F, $1F, $38, $33, $32, $33, $19, $00  ;@
.DB  $00, $F0, $F8, $1C, $1C, $F8, $F0, $00, $00, $3F, $3F, $03, $03, $3F, $3F, $00  ;A
.DB  $00, $FC, $FC, $CC, $CC, $FC, $78, $00, $00, $3F, $3F, $30, $30, $3F, $1F, $00  ;B
.DB  $00, $F8, $FC, $0C, $0C, $3C, $38, $00, $00, $1F, $3F, $30, $30, $3C, $1C, $00  ;C
.DB  $00, $FC, $FC, $0C, $1C, $F8, $F0, $00, $00, $3F, $3F, $30, $38, $1F, $0F, $00  ;D
.DB  $00, $FC, $FC, $CC, $CC, $CC, $0C, $00, $00, $3F, $3F, $30, $30, $30, $30, $00  ;E
.DB  $00, $FC, $FC, $CC, $CC, $CC, $0C, $00, $00, $3F, $3F, $00, $00, $00, $00, $00  ;F
.DB  $00, $F8, $FC, $0C, $CC, $CC, $CC, $00, $00, $1F, $3F, $30, $30, $3F, $1F, $00  ;G
.DB  $00, $FC, $FC, $C0, $C0, $FC, $FC, $00, $00, $3F, $3F, $00, $00, $3F, $3F, $00  ;H
.DB  $00, $0C, $0C, $FC, $FC, $0C, $0C, $00, $00, $30, $30, $3F, $3F, $30, $30, $00  ;I
.DB  $00, $00, $00, $00, $00, $FC, $FC, $00, $00, $1C, $3C, $30, $30, $3F, $1F, $00  ;J
.DB  $FC, $FC, $C0, $F0, $3C, $0C, $00, $00, $3F, $3F, $00, $03, $0F, $3C, $30, $00  ;K
.DB  $00, $FC, $FC, $00, $00, $00, $00, $00, $00, $3F, $3F, $30, $30, $30, $30, $00  ;L
.DB  $FC, $FC, $70, $C0, $70, $FC, $FC, $00, $3F, $3F, $00, $01, $00, $3F, $3F, $00  ;M
.DB  $00, $FC, $FC, $E0, $80, $FC, $FC, $00, $00, $3F, $3F, $01, $07, $3F, $3F, $00  ;N
.DB  $00, $F8, $FC, $0C, $0C, $FC, $F8, $00, $00, $1F, $3F, $30, $30, $3F, $1F, $00  ;O
.DB  $00, $FC, $FC, $0C, $0C, $FC, $F8, $00, $00, $3F, $3F, $03, $03, $03, $01, $00  ;P
.DB  $00, $F8, $FC, $0C, $0C, $FC, $F8, $00, $00, $1F, $3F, $30, $18, $37, $2F, $00  ;Q
.DB  $FC, $FC, $8C, $8C, $FC, $F8, $00, $00, $3F, $3F, $01, $03, $0F, $3C, $30, $00  ;R
.DB  $00, $78, $FC, $CC, $8C, $0C, $0C, $00, $00, $30, $30, $31, $33, $3F, $1E, $00  ;S
.DB  $00, $0C, $0C, $FC, $FC, $0C, $0C, $00, $00, $00, $00, $3F, $3F, $00, $00, $00  ;T
.DB  $00, $FC, $FC, $00, $00, $FC, $FC, $00, $00, $1F, $3F, $30, $30, $3F, $1F, $00  ;U
.DB  $00, $FC, $FC, $00, $00, $FC, $FC, $00, $00, $03, $0F, $3C, $3C, $0F, $03, $00  ;V
.DB  $FC, $FC, $00, $80, $00, $FC, $FC, $00, $3F, $1F, $0E, $07, $0E, $1F, $3F, $00  ;W
.DB  $00, $1C, $7C, $E0, $E0, $7C, $1C, $00, $00, $38, $3E, $07, $07, $3E, $38, $00  ;X
.DB  $00, $3C, $FC, $C0, $C0, $FC, $3C, $00, $00, $00, $00, $3F, $3F, $00, $00, $00  ;Y
.DB  $00, $0C, $0C, $CC, $FC, $3C, $0C, $00, $00, $3C, $3F, $33, $30, $30, $30, $00  ;Z
.DB  $00, $00, $00, $FC, $FC, $0C, $0C, $00, $00, $00, $00, $3F, $3F, $30, $30, $00  ;[
.DB  $00, $1C, $7C, $E0, $80, $00, $00, $00, $00, $00, $00, $01, $07, $3E, $38, $00  ;\
.DB  $00, $0C, $0C, $FC, $FC, $00, $00, $00, $00, $30, $30, $3F, $3F, $00, $00, $00  ;]
.DB  $80, $E0, $78, $1E, $78, $E0, $80, $00, $01, $01, $00, $00, $00, $01, $01, $00  ;^
.DB  $00, $00, $00, $00, $00, $00, $00, $00, $30, $30, $30, $30, $30, $30, $30, $00  ;_
.DB  $00, $07, $0E, $1C, $38, $70, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00  ;`
.DB  $00, $00, $60, $60, $60, $E0, $C0, $00, $00, $1E, $3F, $33, $33, $3F, $3F, $00  ;a
.DB  $00, $FC, $FC, $60, $60, $E0, $C0, $00, $00, $3F, $3F, $30, $30, $3F, $1F, $00  ;b
.DB  $00, $C0, $E0, $60, $60, $60, $00, $00, $00, $1F, $3F, $30, $30, $30, $30, $00  ;c
.DB  $00, $C0, $E0, $60, $60, $FC, $FC, $00, $00, $1F, $3F, $30, $30, $3F, $3F, $00  ;d
.DB  $00, $C0, $E0, $60, $60, $E0, $C0, $00, $00, $1F, $3F, $32, $32, $33, $33, $00  ;e
.DB  $00, $C0, $C0, $F8, $FC, $CC, $CC, $00, $00, $00, $00, $3F, $3F, $00, $00, $00  ;f
.DB  $00, $C0, $E0, $60, $60, $E0, $E0, $00, $00, $CF, $DF, $D8, $D8, $FF, $7F, $00  ;g
.DB  $00, $FC, $FC, $60, $60, $E0, $C0, $00, $00, $3F, $3F, $00, $00, $3F, $3F, $00  ;h
.DB  $00, $00, $60, $EC, $EC, $00, $00, $00, $00, $00, $30, $3F, $3F, $30, $00, $00  ;i
.DB  $00, $00, $00, $00, $EC, $EC, $00, $00, $00, $C0, $C0, $C0, $FF, $7F, $00, $00  ;j
.DB  $FC, $FC, $80, $C0, $E0, $60, $00, $00, $3F, $3F, $03, $07, $1E, $38, $30, $00  ;k
.DB  $00, $00, $0C, $FC, $FC, $00, $00, $00, $00, $00, $30, $3F, $3F, $30, $00, $00  ;l
.DB  $C0, $E0, $E0, $C0, $E0, $E0, $C0, $00, $3F, $3F, $00, $07, $00, $3F, $3F, $00  ;m
.DB  $00, $C0, $E0, $60, $60, $E0, $C0, $00, $00, $3F, $3F, $00, $00, $3F, $3F, $00  ;n
.DB  $00, $C0, $E0, $60, $60, $E0, $C0, $00, $00, $1F, $3F, $30, $30, $3F, $1F, $00  ;o
.DB  $00, $E0, $E0, $60, $60, $E0, $C0, $00, $00, $FF, $FF, $30, $30, $3F, $1F, $00  ;p
.DB  $00, $C0, $E0, $60, $60, $E0, $E0, $00, $00, $1F, $3F, $30, $30, $FF, $FF, $00  ;q
.DB  $00, $E0, $E0, $60, $60, $E0, $C0, $00, $00, $3F, $3F, $00, $00, $00, $00, $00  ;r
.DB  $00, $C0, $E0, $60, $60, $60, $60, $00, $00, $31, $33, $33, $36, $3E, $1C, $00  ;s
.DB  $00, $60, $60, $F8, $F8, $60, $60, $00, $00, $00, $00, $1F, $3F, $30, $30, $00  ;t
.DB  $00, $E0, $E0, $00, $00, $E0, $E0, $00, $00, $1F, $3F, $30, $30, $3F, $3F, $00  ;u
.DB  $00, $E0, $E0, $00, $00, $E0, $E0, $00, $00, $03, $0F, $3C, $3C, $0F, $03, $00  ;v
.DB  $E0, $E0, $00, $80, $00, $E0, $E0, $00, $3F, $1F, $0E, $07, $0E, $1F, $3F, $00  ;w
.DB  $00, $60, $E0, $80, $80, $E0, $60, $00, $00, $30, $3D, $0F, $0F, $3D, $30, $00  ;x
.DB  $00, $E0, $E0, $00, $00, $E0, $E0, $00, $00, $CF, $DF, $D8, $D8, $FF, $7F, $00  ;y
.DB  $00, $60, $60, $60, $E0, $E0, $60, $00, $00, $30, $3C, $3F, $33, $30, $30, $00  ;z
.DB  $80, $80, $C0, $FC, $7E, $02, $02, $00, $01, $01, $03, $3F, $7E, $40, $40, $00  ;{
.DB  $00, $00, $00, $FE, $FE, $00, $00, $00, $00, $00, $00, $7F, $7F, $00, $00, $00  ;|
.DB  $02, $02, $7E, $FC, $C0, $80, $80, $00, $40, $40, $7E, $3F, $03, $01, $01, $00  ;}
.DB  $C0, $60, $E0, $C0, $80, $80, $E0, $00, $01, $00, $00, $01, $01, $01, $00, $00  ;~
.DB  $00, $00, $80, $E0, $E0, $80, $00, $00, $18, $1E, $13, $10, $10, $13, $1E, $18  ;




