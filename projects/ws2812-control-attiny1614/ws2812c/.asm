;WS2812C V2: ANALOG CONTROLLED COLOURS
;---------------------------------------------------------------------------------
;USING ATTINY1614 @ 5V/20MHz, WS2812B-LEDs
;PROG.PARAMETER: INT.RC OSC20

;PORTS:		PROG-A0: UPDI		INPx-B0:
;		AINP-A1: GREEN		INPx-B1:
;		AINP-A2: RED		INPx-B2:
;		AINP-A3: BLUE		INPx-B3:
;		OUT1-A4: WS2812 DATA 
;		INPx-A5:
;		INPx-A6:
;		INPx-A7:

;---------------------------------------------------------------------------------

.DEF	SEC0	=	r7	;SECOND-INT COUNTER (PIT)
.DEF	SIC0	=	r8	;HSEC-SYSINT COUNTER (RTC)
.DEF	TSREG	=	r9	;INTERRUPT SREG TEMP

.EQU	LEDCN	=	12*3	;LED NUMBER * 3	COLOUR

;---------------------------------------------------------------------------------
;Systeminterruptvektor-Tabelle
			
		jmp	RESET			;0000:	RESET
		jmp	RETURN			;0002:	INT01 CRCSCAN
		jmp	RETURN			;0004:	INT02 BOD
		jmp	RETURN			;0006:	INT03 PORTA
		jmp	RETURN			;0008:	INT04 PORTB
		jmp	RETURN			;000A:	INT05 PORTC
		jmp	SYSINT			;000C:	INT06 RTC_CNT
		jmp	SECINT			;000E:	INT07 RTC_PIT
		jmp	RETURN			;0010:	INT08 TCA0_LUNF_OVF
		jmp	RETURN			;0012:	INT09 TCA0_HUNF
		jmp	RETURN			;0014:	INT10 TCA0_LCMP0_CMP0
		jmp	RETURN			;0016:	INT11 TCA0_LCMP1_CMP1
		jmp	RETURN			;0018:	INT12 TCA0_LCMP2_CMP2
		jmp	RETURN			;001A:	INT13 TCB0_INT
		jmp	RETURN			;001C:	INT14 TCB1_INT
		jmp	RETURN			;001E:	INT15 TCD0_OVF
		jmp	RETURN			;0020:	INT16 TCD0_TRIG
		jmp	RETURN			;0022:	INT17 AC0_INT
		jmp	RETURN			;0024:	INT18 AC1_INT
		jmp	RETURN			;0026:	INT19 AC2_INT
		jmp	RETURN			;0028:	INT20 ADC0_RESRDY
		jmp	RETURN			;002A:	INT21 ADC0_WCOMP
		jmp	RETURN			;002C:	INT22 ADC1_RESRDY
		jmp	RETURN			;002E:	INT23 ADC1_RESRDY
		jmp	RETURN			;0030:	INT24 TWI0_TWIS
		jmp	RETURN			;0032:	INT25 TWI0_TWIM
		jmp	RETURN			;0034:	INT26 SPI0_INT
		jmp	RETURN			;0036:	INT27 USART0_RXC
		jmp	RETURN			;0038:	INT28 USART0_DRE
		jmp	RETURN			;003A:	INT29 USART0_TXC
		jmp	RETURN			;003C:	INT30 NVMCTRL_EE

;---------------------------------------------------------------------------------

RESET:		clr	XL			;CLOCK-INIT
		ldi	XH,$d8
		out	CPU_CCP,XH
		sts	CLKCTRL_MCLKCTRLB,XL	;No PreScaler: 20MHz	

		ldi	XH,high(DATARAM)	;SRAM-DELETE:
		ldi	YH,high(RAMEND)
		ldi	YL,low(RAMEND)
		out	CPU_SPL,YL
		out	CPU_SPH,YH
delram:		st	-Y,XL
		cp	YH,XH			;3800H-3FFFH
		brcc	delram

		clr	SIC0			;REGISTER INIT
		clr	SEC0

		ldi	XH,1			;SYSINT(RTC) INIT:
		ldi	XL,$48			;32768/328
		sts	RTC_PER,XL		;=ca.100Hz
		sts	RTC_PER+1,XH
		ldi	XL,1
		sts	RTC_INTCTRL,XL		;Enable OVF-INT
		sts	RTC_CTRLA,XL		;Enable RTC

		sts	RTC_PITINTCTRL,XL	;Enable PIT-INT
		ldi	XL,$71			;32768/32768 = ca.1Hz
		sts	RTC_PITCTRLA,XL		;Enable PIT

		ldi	XL,$6			;ADC0 INIT (RGB INPUT)
		sts	ADC0_CTRLB,XL		;64 SAMPLE ACCUMULATION
		ldi	XL,$53
		sts	ADC0_CTRLC,XL		;RED.SAMPCAP,REF:VCC,CLKPER/16
		ldi	XL,0
		sts	ADC0_MUXPOS,XL		;START-SOURCE: AINP0
		ldi	XL,$a0
		sts	ADC0_CTRLD,XL		;INITIALIZATION DELAY
		ldi	XL,$1f
		sts	ADC0_SAMPCTRL,XL	;SAMPLE LENGTH
		ldi	XL,7
		sts	ADC0_CTRLA,XL		;ADC0 8BIT/FREERUN/ENABLE
		ldi	XL,1
		sts	ADC0_COMMAND,XL		;START ADC0 MEASUREMENT

		sbi	VPORTA_DIR,4		;PORTA4-> OUT WS2812 DATA

mainprg:	ldi	ZL,$fa			;MIN.1000*50ns= 50us DELAY
		ldi	ZH,0			;= WS2812 RESET
mainprg1:	sbiw	Z,1
		brne	mainprg1

		rcall	rgbadc
		rcall	ledout
		rjmp	mainprg

;---------------------------------------------------------------------------------
.include	"ws2812c_sysint.asm"
.include	"ws2812c_mainsp.asm"
.include	"ws2812c_sysram.asm"



