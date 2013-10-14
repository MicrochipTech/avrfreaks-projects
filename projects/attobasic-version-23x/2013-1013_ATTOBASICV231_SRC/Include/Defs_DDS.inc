#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) 
.if (DDS && DATAREC)
  .equ	DDSOut	= PB0			;digital output pin
  .equ	DDSPort	= PORTB			;digital out port
.elif DDS
 .equ	DDSOut	= PD6			;DDS Output pin
 .equ	DDSPort	= PORTD			;Port for DDS output pin
.endif
#elif defined(__ATmega32U4__) || \
	defined(__AT90USB1286__) 
.if DDS
  .equ	DDSOut	= PORTD5		;DDS Output pin
  .equ	DDSPort	= PORTD			;Port for DDS output pin
.endif

#elif defined(__ATmega2560__)
.if DDS
  .equ	DDSOut	= PORTE4		;DDS Output pin
  .equ	DDSPort	= PORTE			;Port for DDS output pin
.endif

#else
#error	"!! Invalid Target MCU !!"
#endif
;
;===============================================================================
; Use TIM2 for Mega88/168/328 and TIM4 for Mega32U4.  The sample rate for
;  DDS is hard-coded ( FCLK / 256 ), thus the maximum frequency attainable 
;  will be ( FCLK / 256 / 2 ).
;
	.equ	DDSRate		= f_clk/256			;DDS master sample rate based on FCLK
	.equ	DDSCnt		= ((f_clk/DDSRate/2)+5)	;# of TIMR ticks between DDS 
												; updates plus "fudge factor"
;
;cummulative DDS add/subtract values
.equ	_1Hz	= ((DDSCnt*EXP2(24)*1)/f_clk)
.equ	_10Hz	= ((DDSCnt*EXP2(24)*10)/f_clk)
.equ	_100Hz	= ((DDSCnt*EXP2(24)*100)/f_clk)
.equ	_1KHz	= ((DDSCnt*EXP2(24)*1000)/f_clk)
.equ	_10KHz	= ((DDSCnt*EXP2(24)*10000)/f_clk)
.equ	_100KHz	= ((DDSCnt*EXP2(24)*100000)/f_clk)

#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__)

;use TIMER2 for DDS sample timer on Mega88/168/328 and USB1286
	.equ	DDSPre		= CK2_1		;DDS master clock Prescaler

#if	defined(__AT90USB1286__) || \
	defined(__ATmega2560__)
	.equ	DDS_PRR		= PRR0
#else
	.equ	DDS_PRR		= PRR
#endif
	.equ	DDS_PRTIM	= PRTIM2

	.equ	DDS_TCNT	= TCNT2
	.equ 	DDS_OCRA	= OCR2A
	.equ	DDS_OCRB	= OCR2B 

	.equ	DDS_TIMSK	= TIMSK2
	;bit defs
	.equ	DDS_TOIE	= TOIE2
	.equ	DDS_TOIEA	= TOIE2A
	.equ	DDS_OCIEA	= OCIE2A
	.equ	DDS_OCIEB	= OCIE2B

	.equ	DDS_TIFR	= TIFR2
	;bit defs
	.equ	DDS_OCFA	= OCF2A
	.equ	DDS_OCFB	= OCF2B
	.equ	DDS_TOV		= TOV2

	.equ	DDS_TCCRA	= TCCR2A
	;bit defs
	.equ	DDS_WGM0	= WGM20
	.equ	DDS_WGM1	= WGM21
	.equ	DDS_COMB0	= COM2B0
	.equ	DDS_COMB1	= COM2B1
	.equ	DDS_COMA0	= COM2A0
	.equ	DDS_COMA1	= COM2A1

	.equ	DDS_TCCRB	= TCCR2B
	;bit defs
	.equ	DDS_CS0		= CS20
	.equ	DDS_CS1		= CS21
	.equ	DDS_CS2		= CS22
	.equ	DDS_WGM2	= WGM22
	.equ	DDS_FOCB	= FOC2B
	.equ	DDS_FOCA	= FOC2A
#elif defined(__ATmega32U4__)
;use TIMER4 for DDS sample timer on Mega32U4
	.equ	DDSPre		= CK4_1		;DDS master clock Prescaler

	.equ	DDS_PRR		= PRR1  
	.equ	DDS_PRTIM	= PRTIM4

	.equ	DDS_TCNT	= TCNT4
	.equ 	DDS_OCRA	= OCR4A
	.equ	DDS_OCRB	= OCR4B
	.equ 	DDS_OCRC	= OCR4C
	.equ 	DDS_OCRD	= OCR4D

	.equ	DDS_TIMSK	= TIMSK4
	;bit defs
	.equ	DDS_TOIE	= TOIE4
	.equ	DDS_OCIEA	= OCIE4A
	.equ	DDS_OCIEB	= OCIE4B
	.equ	DDS_OCIED	= OCIE4D

	.equ	DDS_TIFR	= TIFR4
	;bit defs
	.equ	DDS_OCFA	= OCF4A
	.equ	DDS_OCFB	= OCF4B
	.equ	DDS_OCFC	= OCF4D
	.equ	DDS_TOV		= TOV4

	.equ	DDS_TCCRA	= TCCR4A
	;bit defs
	.equ	DDS_PWMB	= PWM4B
	.equ	DDS_PWMA	= PWM4A
	.equ	DDS_FOCB	= FOC4B
	.equ	DDS_FOCA	= FOC4A
	.equ	DDS_COMB0	= COM4B0
	.equ	DDS_COMB1	= COM4B1
	.equ	DDS_COMA0	= COM4A0
	.equ	DDS_COMA1	= COM4A1

	.equ	DDS_TCCRB	= TCCR4B
	;bit defs
	.equ	DDS_CS0		= CS40
	.equ	DDS_CS1		= CS41
	.equ	DDS_CS2		= CS42
	.equ	DDS_CS3		= CS43
	.equ	DDS_DTPS0	= DTPS40
	.equ	DDS_DTPS1	= DTPS41
	.equ	DDS_PSR		= PSR4
	.equ	DDS_PWMX	= PWM4X

	.equ	DDS_TCCRC	= TCCR4C

	.equ	DDS_TCCRD	= TCCR4D
	;bit defs
	.equ	DDS_WGM0	= WGM40
	.equ	DDS_WGM1	= WGM41
	.equ	DDS_FPF		= FPF4
	.equ	DDS_FPAC	= FPAC4
	.equ	DDS_FPES	= FPES4
	.equ	DDS_FPNC	= FPNC4
	.equ	DDS_FPEN	= FPEN4
	.equ	DDS_FPIE	= FPIE4

	.equ	DDS_TCCRE	= TCCR4E


#else
#error	"!! Invalid Target MCU !!"
#endif
