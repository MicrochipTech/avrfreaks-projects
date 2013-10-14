;output on OC1A
.equ		OCRAH		= OCR1AH	;TIMER1 OCA for 10-bit
.equ		OCRAL		= OCR1AL
.equ		OCRBH		= OCR1BH	;TIMER1 OCB for 10-bit
.equ		OCRBL		= OCR1BL
.equ		TCCRA		= TCCR1A
.equ		TCCRB		= TCCR1B
.equ		TCCRC		= TCCR1C

#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) 
	.equ	PWMPORT		= PORTB		;port for 10-bit PWM output
	.equ	PWMBITA		= PORTB1	;port pin for PWM OC1A output
	.equ	PWMBITB		= PORTB2	;port pin for PWM OC1B output
#elif ( defined(__ATmega32U4__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__) )
;output on OC1A 
	.equ	PWMPORT		= PORTB		;port for 10-bit PWM output
	.equ	PWMBITA		= PORTB5	;port pin for PWM OC1A output
	.equ	PWMBITB		= PORTB6	;port pin for PWM OC1B output
#else
#error	"!! Invalid Target MCU !!"
#endif

.equ	PWMCK_0		= 0			; stop clock
.equ	PWMCK_1		= 1			; Prescaler 1x
.equ	PWMCK_8		= 2			; Prescaler 8x
.equ	PWMCK_64	= 3			; Prescaler 64x
.equ	PWMCK_256	= 4			; Prescaler 256x
.equ	PWMCK_1024	= 5			; Prescaler 1024x
;

.equ	PWMCK		= PWMCK_1	;fastest clock

;set the PWM frequency to 250KHz < PWM < 125KHz
;.equ	PWMCLK_MAX	= 250000
;.if 	f_clk/EXP2(1) <= PWMCLK_MAX	;calculate appropriate prescale value
;	.equ		PWMCK	= PWMCK_1
;.elif 	f_clk/EXP2(3) <= PWMCLK_MAX
;	.equ		PWMCK	= PWMCK_8
;.elif 	f_clk/EXP2(6) <= PWMCLK_MAX
;	.equ		PWMCK	= PWMCK_64
;.elif 	f_clk/EXP2(8) <= PWMCLK_MAX
;	.equ		PWMCK	= PWMCK_256
;.elif 	f_clk/EXP2(10) <= PWMCLK_MAX
;	.equ		PWMCK	= PWMCK_1024
;.endif
