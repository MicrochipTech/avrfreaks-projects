;********** ATmega Series LCD Subroutines **************************************
;Mega_LCD_Lumex.inc
;8-22-2010 CFS

;for Lumex LCR-U12864GSF-WH LCD with dual SBN0064G controllers

;calling program must set equates for
;LED_PORT port used for LED control output
;LED_CTRL	LED backlight on/off and PWM control output

;LCD1_DATA_DDR		direction register for LCD data
;LCD1_DATA_PORT	port used for LCD data output
;LCD1_DATA_PIN		port used for LCD data input

;LCD1_CTRL_PORT	port used for LCD control outputs
;note that output instructions are used that will clear any other
;unused output pins on this port

;LCD1_RS 		LCD register select output (0=instruction, 1=data)
;LCD1_RW		LCD read/write output (0=write, 1=read)
;LCD1_E		LCD enable output (active low)
;LCD1_CS1		LCD chip select 1 output
;LCD1_CS2		LCD chip select 2 output
;LCD1_NRST 	LCD1_RST\ LCD reset output

;lcd_buffer	1K SRAM graphics buffer

;*******************************************************************************
;* ROUTINE
;*	LCD_Init
;* DESCRIPTION
;*	Initialize LCD
;*   for Lumex LCR-U12864GSF-WH LCD with dual SBN0064G controllers
;* USAGE
;*	none
;* STATISTICS
;*	Register usage: all pushed/popped
;* NOTE
;*
;*
;*
;*
;*
;*
;*******************************************************************************


LCD_Init:	push	r16
		push	r17
		push	r18
		push	r19
		push	r20

				;50 msec power on delay
		ldi	r16,200
		call	LCD_Delay

					;toggle LCD1_NRST low (LCD1_E low)
		clr	r16
		out	LCD1_CTRL_PORT,r16

					;250 usec reset delay
		ldi	r16,1
		call	LCD_Delay
		
					;toggle LCD_NRST high

		clr	r16
		sbr	r16,1<<LCD1_NRST
		out	LCD1_CTRL_PORT,r16


					;2 msec delay
		ldi	r16,8
		call	LCD_Delay	

					;set display start line = 0 for both chips
		clr	r16
		ldi	r17,$C0
		call	LCD_Control_Write
		call	LCD_Busy_Wait

		ldi	r16,1
		ldi	r17,$C0
		call	LCD_Control_Write
		call	LCD_Busy_Wait

					;set display on for both chips
		clr	r16
		ldi	r17,$3F
		call	LCD_Control_Write
		call	LCD_Busy_Wait

		ldi	r16,1
		ldi	r17,$3F
		call	LCD_Control_Write
		call	LCD_Busy_Wait


					;set LED backlight on	
		sbi	LED_PORT,LED_CTRL

				
					;clear display memory

					;r19 is page address counter
		clr	r19
		
					;r20 is column address counter
LCD_Init1:	clr	r20	
		
					;set page address for both chips
		clr	r16
		mov	r17,r19
		andi	r17,$07
		ori	r17,$B8
		call	LCD_Control_Write
		call	LCD_Busy_Wait

		ldi	r16,1
		mov	r17,r19
		andi	r17,$07
		ori	r17,$B8
		call	LCD_Control_Write
		call	LCD_Busy_Wait


					;set column address = 0 for both chips
		clr	r16
		ldi	r17,$40
		call	LCD_Control_Write
		call	LCD_Busy_Wait

		ldi	r16,1
		ldi	r17,$40
		call	LCD_Control_Write
		call	LCD_Busy_Wait



					;write data for columns 0-63 to chip 1
LCD_Init2:	

				;set LCD1_DATA_PORT for output	
		ldi	r18,0b11111111
		out	LCD1_DATA_DDR,r18


					;set control signals
		clr	r18
					;select chip 1
					;set LCD1_RS=1, LCD_RW=0, LCD1_CS1=1, LCD1_CS2=0, LCD1_NRST=1   
		sbr	r18,(1<<LCD1_RS) + (1<<LCD1_CS1) + (1<<LCD1_NRST) 
		out	LCD1_CTRL_PORT,r18

					;write data
		clr	r17
		out	LCD1_DATA_PORT,r17

					;180 nsec delay
		nop


					;toggle LCD1_E high
		sbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18
		
					;500 nsec delay
		nop
		nop
		nop
		nop
		nop
		nop

					;toggle LCD1_E low
		cbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18

	
					;busy wait for chip 1

					;set LCD1_DATA_PORT for input
		clr	r17	
		out	LCD1_DATA_DDR,r17


				;r18 as loop counter
				;jump ahead if busy flag not cleared
				;after 16 loops
				;approx. 1 usec per loop
		clr	r18
	
					;set control signals
		clr	r17
					;select chip 1
					;set LCD1_RS=0, LCD_RW=1, LCD1_CS1=1, LCD1_CS2=0, LCD1_NRST=1   
		sbr	r17,(1<<LCD1_CS1) + (1<<LCD1_RW) + (1<<LCD1_NRST) 
		out	LCD1_CTRL_PORT,r17

					;180 nsec delay

LCD_Init2A:
		inc	r18
		cpi	r18,16
		breq	LCD_Init2B

	

					;toggle LCD1_E high
		sbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17

					;500 nsec delay
		nop
		nop
		nop
		nop
		nop

					;read status
		in	r16,LCD1_DATA_PIN

				;toggle LCD1_E low
		cbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17

					;500 nsec delay
	
				;read busy flag on DB7 
				;continue to loop while busy flag set
		in	r16,LCD1_DATA_PIN
		andi	r16,$80
		tst	r16
		brne	LCD_Init2A
		
				;busy flag no longer set or timeout
LCD_Init2B:

					
					;set LCD1_RW=0 (to return to write mode)
		cbr	r17,1<<LCD1_RW
		out LCD1_CTRL_PORT,r17



					;loop for columns 0-63 
		inc	r20
		cpi	r20,64
		brlo	LCD_Init2


					;write data for columns 64-127 to chip 2
LCD_Init3:	

				;set LCD1_DATA_PORT for output	
		ldi	r18,0b11111111
		out	LCD1_DATA_DDR,r18


					;set control signals
		clr	r18
					;select chip 2
					;set LCD1_RS=1, LCD_RW=0, LCD1_CS1=0, LCD1_CS2=1, LCD1_NRST=1   
		sbr	r18,(1<<LCD1_RS) + (1<<LCD1_CS2) + (1<<LCD1_NRST) 

		out	LCD1_CTRL_PORT,r18

					;write data
		clr	r17
		out	LCD1_DATA_PORT,r17

					;180 nsec delay
		nop


					;toggle LCD1_E high
		sbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18
		
					;500 nsec delay
		nop
		nop
		nop
		nop
		nop
		nop

					;toggle LCD1_E low
		cbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18


					;busy wait for chip 2

					;set LCD1_DATA_PORT for input
		clr	r17	
		out	LCD1_DATA_DDR,r17


				;r18 as loop counter
				;jump ahead if busy flag not cleared
				;after 16 loops
				;approx. 1 usec per loop
		clr	r18
	
					;set control signals
		clr	r17
					;select chip 2
					;set LCD1_RS=0, LCD_RW=1, LCD1_CS1=0, LCD1_CS2=1, LCD1_NRST=1   
		sbr	r17,(1<<LCD1_CS2) + (1<<LCD1_RW) + (1<<LCD1_NRST) 
		out	LCD1_CTRL_PORT,r17

					;180 nsec delay

LCD_Init3A:
		inc	r18
		cpi	r18,16
		breq	LCD_Init3B

	

					;toggle LCD1_E high
		sbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17

					;500 nsec delay
		nop
		nop
		nop
		nop
		nop

					;read status
		in	r16,LCD1_DATA_PIN

				;toggle LCD1_E low
		cbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17

					;500 nsec delay
	
				;read busy flag on DB7 
				;continue to loop while busy flag set
		in	r16,LCD1_DATA_PIN
		andi	r16,$80
		tst	r16
		brne	LCD_Init3A
		
				;busy flag no longer set or timeout
LCD_Init3B:

					;set LCD1_RW=0 (to return to write mode)
		cbr	r17,1<<LCD1_RW
		out LCD1_CTRL_PORT,r17

					
					;loop for columns 64-127 
		inc	r20
		cpi	r20,128
		brlo	LCD_Init3



					;loop for pages 0-7
		inc	r19
		cpi	r19,8
		brsh	LCD_Init4
		rjmp	LCD_Init1



LCD_Init4:
		
		pop	r20
		pop	r19
		pop	r18
		pop	r17
		pop	r16
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
;*	LCD_Busy_Wait
;* DESCRIPTION
;*	Wait until LCD not busy
;* USAGE
;*	Chip select byte in r16
;*   b0 selects controller chip (b0=0 CS1, b0=1 CS2)
;*	Returns wait time in r16 (approx. 1 usec units, r16=16 means timeout)
;* STATISTICS
;*	Register usage: r16
;* NOTE
;*	
;*
;*
;*
;*******************************************************************************

				
LCD_Busy_Wait:	push	r17
		push r18

					;set LCD1_DATA_PORT for input
		clr	r17	
		out	LCD1_DATA_DDR,r17


				;r18 as loop counter
				;jump ahead if busy flag not cleared
				;after 16 loops
				;moved to r16 on return 
				;r16=16 on return indicated busy wait timeout 
				;approx. 1 usec per loop
		clr	r18
	
					;set control signals
		clr	r17
		sbrs	r16,0
					;select chip 1
					;set LCD1_RS=0, LCD_RW=1, LCD1_CS1=1, LCD1_CS2=0, LCD1_NRST=1   
		sbr	r17,(1<<LCD1_CS1) + (1<<LCD1_RW) + (1<<LCD1_NRST) 
		
		sbrc	r16,0
					;select chip 2
					;set LCD1_RS=0, LCD_RW=1, LCD1_CS1=0, LCD1_CS2=1, LCD1_NRST=1   
		sbr	r17,(1<<LCD1_CS2) + (1<<LCD1_RW) + (1<<LCD1_NRST) 

		out	LCD1_CTRL_PORT,r17

					;180 nsec delay

LCD_Busy_Wait1:
		inc	r18
		cpi	r18,16
		breq	LCD_Busy_Wait2

	

					;toggle LCD1_E high
		sbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17

					;500 nsec delay
		nop
		nop
		nop
		nop
		nop

					;read status
		in	r16,LCD1_DATA_PIN

				;toggle LCD1_E low
		cbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17

					;500 nsec delay
	
				;read busy flag on DB7 
				;continue to loop while busy flag set
		in	r16,LCD1_DATA_PIN
		andi	r16,$80
		tst	r16
		brne	LCD_Busy_Wait1
		
				;busy flag no longer set or timeout
LCD_Busy_Wait2:

					;set LCD1_RW=0 (to return to write mode)
		cbr	r17,1<<LCD1_RW
		out LCD1_CTRL_PORT,r17



				;move loop counter to r16
		mov	r16,r18	

		pop	r18
		pop	r17

		ret



;*******************************************************************************
;* ROUTINE
;*	LCD_Status_Read
;* DESCRIPTION
;*	Read status register from LCD
;* USAGE
;*	Chip select byte in r16
;*   b0 selects controller chip (b0=0 CS1, b0=1 CS2)
;*	Returns status in r16  
;* STATISTICS
;*	Register usage: r16
;* NOTE
;*	
;*
;*
;*
;*******************************************************************************

				
LCD_Status_Read:	push	r17

					;set LCD1_DATA_PORT for input
		clr	r17	
		out	LCD1_DATA_DDR,r17

					;set control signals

		clr	r17
		sbrs	r16,0
					;select chip 1
					;set LCD1_RS=0, LCD_RW=1, LCD1_CS1=1, LCD1_CS2=0, LCD1_NRST=1   
		sbr	r17,(1<<LCD1_CS1) + (1<<LCD1_RW) + (1<<LCD1_NRST) 
		
		sbrc	r16,0
					;select chip 2
					;set LCD1_RS=0, LCD_RW=1, LCD1_CS1=0, LCD1_CS2=1, LCD1_NRST=1   
		sbr	r17,(1<<LCD1_CS2) + (1<<LCD1_RW) + (1<<LCD1_NRST) 

		out	LCD1_CTRL_PORT,r17

		
					;180 nsec delay
		nop
		nop

					;toggle LCD1_E high
		sbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17

					;500 nsec delay
		nop
		nop
		nop
		nop
		nop

					;read status
		in	r16,LCD1_DATA_PIN

				;toggle LCD1_E low
		cbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17


					;set LCD1_RW=0 (to return to write mode)
		cbr	r17,1<<LCD1_RW
		out LCD1_CTRL_PORT,r17


					;500 nsec delay
		nop
		nop
	
	
		pop	r17

		ret


;*******************************************************************************
;* ROUTINE
;*	LCD_Control_Write
;* DESCRIPTION
;*	Write control command to LCD
;* USAGE
;*	Chip select byte in r16
;*   b0 selects controller chip (b0=0 CS1, b0=1 CS2)
;*	Command byte in r17
;* STATISTICS
;*	Register usage: r16-r17
;* NOTE
;*	
;*
;*
;*
;*
;*******************************************************************************

LCD_Control_Write:	push	r18

				;set LCD1_DATA_PORT for output	
		ldi	r18,0b11111111
		out	LCD1_DATA_DDR,r18


					;set control signals
		clr	r18
		sbrs	r16,0
					;select chip 1
					;set LCD1_RS=0, LCD_RW=0, LCD1_CS1=1, LCD1_CS2=0, LCD1_NRST=1   
		sbr	r18,(1<<LCD1_CS1) + (1<<LCD1_NRST) 
		
		sbrc	r16,0
					;select chip 2
					;set LCD1_RS=0, LCD_RW=0, LCD1_CS1=0, LCD1_CS2=1, LCD1_NRST=1   
		sbr	r18,(1<<LCD1_CS2) + (1<<LCD1_NRST) 

		out	LCD1_CTRL_PORT,r18

					;write command
		out	LCD1_DATA_PORT,r17

					;180 nsec delay
		nop

					;toggle LCD1_E high
		sbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18
		
					;500 nsec delay
		nop
		nop
		nop
		nop
		nop
		nop

					;toggle LCD1_E low
		cbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18


					;500 nsec delay
		nop
		nop
		nop
	
		pop	r18

		ret


;*******************************************************************************
;* ROUTINE
;*	LCD_Data_Write
;* DESCRIPTION
;*	Write data to LCD
;* USAGE
;*	Chip select byte in r16
;*   b0 selects controller chip (b0=0 CS1, b0=1 CS2)
;*	Data byte in r17
;* STATISTICS
;*	Register usage: r16-r17
;* NOTE
;*
;*
;*
;*
;*******************************************************************************

LCD_Data_Write:	push	r18

				;set LCD1_DATA_PORT for output	
		ldi	r18,0b11111111
		out	LCD1_DATA_DDR,r18


					;set control signals
		clr	r18
		sbrs	r16,0
					;select chip 1
					;set LCD1_RS=1, LCD_RW=0, LCD1_CS1=1, LCD1_CS2=0, LCD1_NRST=1   
		sbr	r18,(1<<LCD1_RS) + (1<<LCD1_CS1) + (1<<LCD1_NRST) 
		
		sbrc	r16,0
					;select chip 2
					;set LCD1_RS=1, LCD_RW=0, LCD1_CS1=0, LCD1_CS2=1, LCD1_NRST=1   
		sbr	r18,(1<<LCD1_RS) + (1<<LCD1_CS2) + (1<<LCD1_NRST) 

		out	LCD1_CTRL_PORT,r18

					;write data
		out	LCD1_DATA_PORT,r17

					;180 nsec delay
		nop

					;toggle LCD1_E high
		sbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18
		
					;500 nsec delay
		nop
		nop
		nop
		nop
		nop
		nop

					;toggle LCD1_E low
		cbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18

	
					;500 nsec delay
		nop
		nop
		nop
	
		pop	r18

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
;*	Execution time approx. 10 msec
;*
;*
;*
;*******************************************************************************


					
LCD_BufWrite:	push	r16
		push	r17
		push	r18
		push	r19
		push	r20
		push	r30
		push	r31

					;buffer address to Z
		ldi	r30,low(lcd_buffer)
		ldi	r31,high(lcd_buffer)						
						
					;r19 is page address counter
		clr	r19
		
					;r20 is column address counter
LCD_BufWrite1:	clr	r20	
		
					;set page address for both chips
		clr	r16
		mov	r17,r19
		andi	r17,$07
		ori	r17,$B8
		call	LCD_Control_Write
		call	LCD_Busy_Wait

		ldi	r16,1
		mov	r17,r19
		andi	r17,$07
		ori	r17,$B8
		call	LCD_Control_Write
		call	LCD_Busy_Wait


					;set column address = 0 for both chips
		clr	r16
		ldi	r17,$40
		call	LCD_Control_Write
		call	LCD_Busy_Wait

		ldi	r16,1
		ldi	r17,$40
		call	LCD_Control_Write
		call	LCD_Busy_Wait



					;write data for columns 0-63 to chip 1
LCD_BufWrite2:	

				;set LCD1_DATA_PORT for output	
		ldi	r18,0b11111111
		out	LCD1_DATA_DDR,r18


					;set control signals
		clr	r18
					;select chip 1
					;set LCD1_RS=1, LCD_RW=0, LCD1_CS1=1, LCD1_CS2=0, LCD1_NRST=1   
		sbr	r18,(1<<LCD1_RS) + (1<<LCD1_CS1) + (1<<LCD1_NRST) 
		out	LCD1_CTRL_PORT,r18

					;write data
		ld	r17,Z+
		out	LCD1_DATA_PORT,r17

					;180 nsec delay
		nop


					;toggle LCD1_E high
		sbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18
		
					;500 nsec delay
		nop
		nop
		nop
		nop
		nop
		nop

					;toggle LCD1_E low
		cbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18

	
					;busy wait for chip 1

					;set LCD1_DATA_PORT for input
		clr	r17	
		out	LCD1_DATA_DDR,r17


				;r18 as loop counter
				;jump ahead if busy flag not cleared
				;after 16 loops
				;approx. 1 usec per loop
		clr	r18
	
					;set control signals
		clr	r17
					;select chip 1
					;set LCD1_RS=0, LCD_RW=1, LCD1_CS1=1, LCD1_CS2=0, LCD1_NRST=1   
		sbr	r17,(1<<LCD1_CS1) + (1<<LCD1_RW) + (1<<LCD1_NRST) 
		out	LCD1_CTRL_PORT,r17

					;180 nsec delay

LCD_BufWrite2A:
		inc	r18
		cpi	r18,16
		breq	LCD_BufWrite2B

	

					;toggle LCD1_E high
		sbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17

					;500 nsec delay
		nop
		nop
		nop
		nop
		nop

					;read status
		in	r16,LCD1_DATA_PIN

				;toggle LCD1_E low
		cbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17

					;500 nsec delay
	
				;read busy flag on DB7 
				;continue to loop while busy flag set
		in	r16,LCD1_DATA_PIN
		andi	r16,$80
		tst	r16
		brne	LCD_BufWrite2A
		
				;busy flag no longer set or timeout
LCD_BufWrite2B:

					;set LCD1_RW=0 (to return to write mode)
		cbr	r17,1<<LCD1_RW
		out LCD1_CTRL_PORT,r17



					;loop for columns 0-63 
		inc	r20
		cpi	r20,64
		brlo	LCD_BufWrite2


					;write data for columns 64-127 to chip 2
LCD_BufWrite3:	

				;set LCD1_DATA_PORT for output	
		ldi	r18,0b11111111
		out	LCD1_DATA_DDR,r18


					;set control signals
		clr	r18
					;select chip 2
					;set LCD1_RS=1, LCD_RW=0, LCD1_CS1=0, LCD1_CS2=1, LCD1_NRST=1   
		sbr	r18,(1<<LCD1_RS) + (1<<LCD1_CS2) + (1<<LCD1_NRST) 

		out	LCD1_CTRL_PORT,r18

					;write data
		ld	r17,Z+
		out	LCD1_DATA_PORT,r17

					;180 nsec delay
		nop


					;toggle LCD1_E high
		sbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18
		
					;500 nsec delay
		nop
		nop
		nop
		nop
		nop
		nop

					;toggle LCD1_E low
		cbr	r18,1<<LCD1_E
		out	LCD1_CTRL_PORT,r18


					;busy wait for chip 2

					;set LCD1_DATA_PORT for input
		clr	r17	
		out	LCD1_DATA_DDR,r17


				;r18 as loop counter
				;jump ahead if busy flag not cleared
				;after 16 loops
				;approx. 1 usec per loop
		clr	r18
	
					;set control signals
		clr	r17
					;select chip 2
					;set LCD1_RS=0, LCD_RW=1, LCD1_CS1=0, LCD1_CS2=1, LCD1_NRST=1   
		sbr	r17,(1<<LCD1_CS2) + (1<<LCD1_RW) + (1<<LCD1_NRST) 
		out	LCD1_CTRL_PORT,r17

					;180 nsec delay

LCD_BufWrite3A:
		inc	r18
		cpi	r18,16
		breq	LCD_BufWrite3B

	

					;toggle LCD1_E high
		sbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17

					;500 nsec delay
		nop
		nop
		nop
		nop
		nop

					;read status
		in	r16,LCD1_DATA_PIN

				;toggle LCD1_E low
		cbr	r17,1<<LCD1_E
		out	LCD1_CTRL_PORT,r17

					;500 nsec delay
	
				;read busy flag on DB7 
				;continue to loop while busy flag set
		in	r16,LCD1_DATA_PIN
		andi	r16,$80
		tst	r16
		brne	LCD_BufWrite3A
		
				;busy flag no longer set or timeout
LCD_BufWrite3B:

					;set LCD1_RW=0 (to return to write mode)
		cbr	r17,1<<LCD1_RW
		out LCD1_CTRL_PORT,r17

					
					;loop for columns 64-127 
		inc	r20
		cpi	r20,128
		brlo	LCD_BufWrite3



					;loop for pages 0-7
		inc	r19
		cpi	r19,8
		brsh	LCD_BufWrite4
		rjmp	LCD_BufWrite1



LCD_BufWrite4:

		pop	r31
		pop	r30
		pop	r20
		pop	r19
		pop	r18
		pop	r17
		pop	r16
	
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

