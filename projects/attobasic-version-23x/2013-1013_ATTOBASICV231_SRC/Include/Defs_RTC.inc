#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__)
.if RTC
;use TIMER0 for real-time counter interrupt on Mega88/168/328
	.equ	RTC_PRR		= PRR
	.equ	RTC_PRTIM	= PRTIM0

	.equ	RTC_TCNT	= TCNT0
	.equ 	RTC_OCRA	= OCR0A
	.equ	RTC_OCRB	= OCR0B 

	.equ	RTC_TIMSK	= TIMSK0
	;bit defs
	.equ	RTC_TOIE	= TOIE0
	.equ	RTC_OCIEA	= OCIE0A
	.equ	RTC_OCIEB	= OCIE0B

	.equ	RTC_TIFR	= TIFR0
	;bit defs
	.equ	RTC_OCFA	= OCF0A
	.equ	RTC_OCFB	= OCF0B
	.equ	RTC_TOV		= TOV0

	.equ	RTC_TCCRA	= TCCR0A
	;bit defs
	.equ	RTC_WGM0	= WGM00
	.equ	RTC_WGM1	= WGM01
	.equ	RTC_COMB0	= COM0B0
	.equ	RTC_COMB1	= COM0B1
	.equ	RTC_COMA0	= COM0A0
	.equ	RTC_COMA1	= COM0A1

	.equ	RTC_TCCRB	= TCCR0B
	;bit defs
	.equ	RTC_CS0		= CS00
	.equ	RTC_CS1		= CS01
	.equ	RTC_CS2		= CS02
	.equ	RTC_WGM2	= WGM02
	.equ	RTC_FOCB	= FOC0B
	.equ	RTC_FOCA	= FOC0A
.endif
#elif ( defined(__ATmega32U4__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__) )
.if RTC
;use TIMER0 for real-time counter interrupt on Mega32U4
	.equ	RTC_PRR		= PRR0  
	.equ	RTC_PRTIM	= PRTIM0

	.equ	RTC_TCNT	= TCNT0
	.equ 	RTC_OCRA	= OCR0A
	.equ	RTC_OCRB	= OCR0B

	.equ	RTC_TIMSK	= TIMSK0
	;bit defs
	.equ	RTC_TOIE	= TOIE0
	.equ	RTC_OCIEA	= OCIE0A
	.equ	RTC_OCIEB	= OCIE0B

	.equ	RTC_TIFR	= TIFR0
	;bit defs
	.equ	RTC_OCFA	= OCF0A
	.equ	RTC_OCFB	= OCF0B
	.equ	RTC_TOV		= TOV0

	.equ	RTC_TCCRA	= TCCR0A
	;bit defs
	.equ	RTC_WGM0	= WGM00
	.equ	RTC_WGM1	= WGM01
	.equ	RTC_COMB0	= COM0B0
	.equ	RTC_COMB1	= COM0B1
	.equ	RTC_COMA0	= COM0A0
	.equ	RTC_COMA1	= COM0A1

	.equ	RTC_TCCRB	= TCCR0B
	;bit defs
	.equ	RTC_CS0		= CS00
	.equ	RTC_CS1		= CS01
	.equ	RTC_CS2		= CS02
	.equ	RTC_WGM2	= WGM02
	.equ	RTC_FOCB	= FOC0B
	.equ	RTC_FOCA	= FOC0A
;
.endif
#else
#error	"!! Invalid Target MCU !!"
#endif

.if RTC
.equ	RTC_IPS			= 1000	;# of interrupts per second

.set	RTC_CLK			= CK0_1	;actual bit value of prescaler
.set	RTC_PSCLR		= 1		;TMR0 prescaler set to 1 for start
;
;Calculate OCR0A register load value for "RTC_IPS" interupt interval
.set	RTC_OCRA_L		= ((10*FCLK)/RTC_PSCLR/RTC_IPS)

.if ((RTC_OCRA_L /10) > 255)
 .set	RTC_PSCLR		= 8		;increase TMR0 prescaler to 16
 .set	RTC_CLK			= CK0_8	;actual bit value of prescaler
 .set	RTC_OCRA_L		= ((10*FCLK)/RTC_PSCLR/RTC_IPS)
.endif

.if ((RTC_OCRA_L /10) > 255)
 .set	RTC_PSCLR		= 64	;increase TMR0 prescaler to 64
 .set	RTC_CLK			= CK0_64	;actual bit value of prescaler
 .set	RTC_OCRA_L		= ((10*FCLK)/RTC_PSCLR/RTC_IPS)
.endif

.if ((RTC_OCRA_L /10) > 255)
 .set	RTC_PSCLR		= 256	;increase TMR0 prescaler to 256
 .set	RTC_CLK			= CK0_256;actual bit value of prescaler
 .set	RTC_OCRA_L		= ((10*FCLK)/RTC_PSCLR/RTC_IPS)
.endif

.if ((RTC_OCRA_L /10) > 255)
 .set	RTC_PSCLR		= 1024	;increase TMR0 prescaler to 1024
 .set	RTC_CLK			= CK0_1K	;actual bit value of prescaler
 .set	RTC_OCRA_L		= ((10*FCLK)/RTC_PSCLR/RTC_IPS)
.endif

.if ((RTC_OCRA_L % 10) > 5) 
	.set	RTC_OCRA_L	= (RTC_OCRA_L/10)
.else
	.set	RTC_OCRA_L	= (RTC_OCRA_L/10) - 1
.endif

.equ	RTCMul_0		= 0		;interval value for 1x multiplier
.equ	RTCMul_1		= 10	;interval value for 10x multiplier
.equ	RTCMul_2		= 100	;interval value for 100x multiplier
;
;===== Not used ==========================================================
;.set	RTC_OCRA_L1		= ((10*FCLK)/RTC_PSCLR/500)
;.if ((RTC_OCRA_L1 % 10) > 5) 
;	.set	RTC_OCRA_L1		= (RTC_OCRA_L1/10)+1	;+1 to minimize error
;.else
;	.set	RTC_OCRA_L1		= (RTC_OCRA_L1/10)
;.endif

;.set	RTC_OCRA_L2		= ((10*FCLK)/RTC_PSCLR/200)
;.if ((RTC_OCRA_L2 % 10) > 5) 

;	.set	RTC_OCRA_L2		= (RTC_OCRA_L2/10)+1	;+1 to minimize error
;.else
;	.set	RTC_OCRA_L2		= (RTC_OCRA_L2/10)
;.endif

;.set	RTC_OCRA_L3		= ((10*FCLK)/RTC_PSCLR/100)
;.if ((RTC_OCRA_L3 % 10) > 5) 
;	.set	RTC_OCRA_L3		= (RTC_OCRA_L3/10)+1	;+1 to minimize error
;.else
;	.set	RTC_OCRA_L3		= (RTC_OCRA_L3/10)
;.endif

.endif
