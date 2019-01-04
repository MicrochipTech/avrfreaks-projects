;WS2812C SYSTEM INTERRUPTS
;---------------------------------------------------------------------------------

SECINT:		in	TSREG,CPU_SREG		;SREG->R9
		movw	r11:r10,XH:XL		;X-Pointer secure
		movw	r13:r12,YH:YL		;Y-Pointer secure
		movw	r15:r14,ZH:ZL		;Z-Pointer secure

		inc	SEC0

		ldi	XL,1
		sts	RTC_PITINTFLAGS,XL

		rjmp	sysint99

;-------------------------------------------------------------------------------
;SYSTEM RTC INTERRUPT (100Hz)
		
SYSINT:		in	TSREG,CPU_SREG		;SREG secure
		movw	r11:r10,XH:XL		;X-Pointer secure
		movw	r13:r12,YH:YL		;Y-Pointer secure
		movw	r15:r14,ZH:ZL		;Z-Pointer secure

		inc	SIC0
		rcall	adcc			;ADC CONTROL

sysint97:	ldi	XL,1			;OVF INTFLAG RESET
		sts	RTC_INTFLAGS,XL

sysint99:	movw	ZH:ZL,r15:r14		;Z-Pointer restore
		movw	YH:YL,r13:r12		;Y-Pointer restore
		movw	XH:XL,r11:r10		;X-Pointer restore
		out	CPU_SREG,TSREG		;SREG restore
RETURN:		reti

;-------------------------------------------------------------------------------
;ADCC		ADC CONTROL (ADC0->AINP0/1/2/3: NU/R/G/B INPUT)

adcc:		ldi	ZL,low(AINP0)
		ldi	ZH,high(AINP0)
		mov	XL,SIC0
		ldi	XH,0
		andi	XL,3
		add	ZL,XL
		adc	ZH,XH			;Z-> CURRENT AINP			

		ldi	XH,6
		lds	YL,ADC0_RES		;CURRENT RGB DATA
		lds	YH,ADC0_RES+1
adcc2:		lsr	YH			;DIV 64
		ror	YL
		dec	XH		
		brne	adcc2

		st	Z,YL			;SAVE CURRENT 8BIT RESULT
		inc	XL
		andi	XL,3
		sts	ADC0_MUXPOS,XL		;SAVE NEXT AINP(0-3) TO MUXPOS
		ret