;WS2812C SUB PROGRAMS
;---------------------------------------------------------------------------------
;RGBADC		RGB ANALOG DATA CONTROL (RGB COLOUR SETTING(AINP1/2/3))

rgbadc:		ldi	r16,LEDCN/3		;R16= LED NUMBER
;		clr	r17			;FLASHING IN SECONDS
;		clr	r18
;		clr	r19
;		sbrs	SEC0,0
;		rjmp	rgbadc1
		lds	r17,AINP1		;GREEN ANALOG DATA
		lds	r18,AINP2		;RED ANALOG DATA
		lds	r19,AINP3		;BLUE ANALOG DATA
rgbadc1:	ldi	ZL,low(LED01G)
		ldi	ZH,high(LED01G)
rgbadc2:	std	Z+0,r17
		std	Z+1,r18
		std	Z+2,r19
		adiw	Z,3
		dec	r16
		brne	rgbadc2
		ret

;---------------------------------------------------------------------------------
;LEDOUT		LED OUTPUT PROTOCOL

ledout:		ldi	ZL,low(LED01G)
		ldi	ZH,high(LED01G)		;POINTER->LED DATA
		ldi	r16,8			;8BIT COUNTER
		ld	r17,Z+			;FIRST LED COLOUR-DATA
		ldi	r24,low(LEDCN)
;		ldi	r25,high(LEDCN)		;FOR >85 LED
		cli				;LED OUTPUT CYCLE: NO INTERRUPT
dout0:		nop				;DATA-OUT: INSTRUCTION CLOCK=50ns
		nop
		nop
		nop
dout1:		sbi	VPORTA_OUT,4		;DATA-OUT: 8x SBI
		nop				;BIT0=  8xSBI + 17xCBI = 1,25uS
		nop				;BIT1= 16xSBI +  9xCBI = 1,25uS
		nop
		nop
		lsl	r17			;NEXT BIT FROM CURRENT COLOUR
		brcs	dout2
		nop
		cbi	VPORTA_OUT,4		;DATA-OUT: 8x SBI/CBI		
dout2:		nop
		nop
		nop
		nop
		nop
		brcs	dout3			;CLOCK CORRECTION
dout3:		nop
		cbi	VPORTA_OUT,4		;DATA-OUT: 9x CBI
		nop
		dec	r16
		brne	dout0			;OUTPUT 8 COLOUR BITS
		ldi	r16,8

		ld	r17,Z+
		dec	r24			;sbiw r25:r24,1 FOR >85 LED
		brne	dout1			;NEXT COLOUR/LED
		sei				;ENABLE INTERRUPTS
		ret				;LEDN LED's FINISHED