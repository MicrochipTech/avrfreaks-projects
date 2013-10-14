;****************************************************************
; Analog Comparator Constants
;****************************************************************
.equ	ACSR_L	= ( (0<<ACD) | \
					(0<<ACBG) | \
					(0<<ACO) | \
					(1<<ACI) | \
					(0<<ACIE) | \
					(0<<ACIC) | \
					(0<<ACIS1) | \
					(0<<ACIS0) )
;
;****************************************************************
; ADC Constants
;****************************************************************
.if ADConv
.equ	ADC_Avg		= 16		;number of reads for average (2^N)
.equ	ADC_Shft	= LOG2((1024*ADC_Avg)/256)	;number of right shifts

.equ	ADCK_0	= 0				; stop clock
.equ	ADCK_2	= 1				; Prescaler 2x
.equ	ADCK_4	= 2				; Prescaler 4x
.equ	ADCK_8	= 3				; Prescaler 8x
.equ	ADCK_16	= 4				; Prescaler 16x
.equ	ADCK_32	= 5				; Prescaler 32x
.equ	ADCK_64	= 6				; Prescaler 64x
.equ	ADCK_128= 7				; Prescaler 128x
;
.equ	ADCLK_MAX	= 200000
.if f_clk/EXP2(1) <= ADCLK_MAX	;calculate appropriate prescale value
	.equ		ADCK	= 1
.elif f_clk/EXP2(2) <= ADCLK_MAX
	.equ		ADCK	= 2
.elif f_clk/EXP2(3) <= ADCLK_MAX
	.equ		ADCK	= 3
.elif f_clk/EXP2(4) <= ADCLK_MAX
	.equ		ADCK	= 4
.elif f_clk/EXP2(5) <= ADCLK_MAX
	.equ		ADCK	= 5
.elif f_clk/EXP2(6) <= ADCLK_MAX
	.equ		ADCK	= 6
.elif f_clk/EXP2(7) <= ADCLK_MAX
	.equ		ADCK	= 7
.endif
;
; ADMUX Options: Ext. ref, Left Adjust Result, ADC0 selected
#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__)
.equ	ADMUX_L	= ( (0<<REFS1) | \
					(0<<REFS0) | \
					(0<<ADLAR) | \
					(0<<MUX3) | \
					(0<<MUX2) | \
					(0<<MUX1) | \
					(0<<MUX0) )
;
; ADC Control Options A: Enable ADC in free-running mode,
;	prescaler configured based upon clock
.set	ADCSRA_L= ( (1<<ADEN) | \
					(1<<ADSC) | \
					(0<<ADATE) | \
					(1<<ADIF) | \
					(0<<ADIE) ) + ADCK
;
; ADC Control Options B: Free-running trigger source
.equ	ADCSRB_L= ( (0<<ACME) | \
					(0<<ADTS2) | \
					(0<<ADTS1) | \
					(0<<ADTS0) )
;
; DIDR Options: no disable digital input
.equ	DIDR0_L	= ( (0<<ADC3D) | \
					(0<<ADC2D) | \
					(0<<ADC1D) | \
					(0<<ADC0D) )

.equ	ADC_Ref			= 110				;110 = 1.1V or 256 = 2.56V 
;
#elif defined(__ATmega32U4__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__)
  .equ	ADMUX_L	= ( (0<<REFS1) | \
					(0<<REFS0) | \
					(0<<ADLAR) | \
					(0<<MUX4) | \
					(0<<MUX3) | \
					(0<<MUX2) | \
					(0<<MUX1) | \
					(0<<MUX0) )
  ;
  ; ADC Control Options A: Enable ADC in free-running mode, 
  ;	prescaler configured based upon clock
  .set	ADCSRA_L= ( (1<<ADEN) | \
					(1<<ADSC) | \
					(0<<ADATE) | \
					(1<<ADIF) | \
					(0<<ADIE) ) + ADCK
;
 #if defined(__ATmega32U4__)
  ; ADC Control Options B: Free-running trigger source
  .equ	ADCSRB_L= ( (0<<ADHSM) | \
					(0<<ACME) | \
					(0<<MUX5) | \
					(0<<ADTS3) | \
					(0<<ADTS2) | \
					(0<<ADTS1) | \
					(0<<ADTS0) )
 #elif defined(__AT90USB1286__)
  ; ADC Control Options B: Free-running trigger source
  .equ	ADCSRB_L= ( (0<<ADHSM) | \
					(0<<ACME) | \
					(0<<ADTS2) | \
					(0<<ADTS1) | \
					(0<<ADTS0) )
 #elif defined(__ATmega2560__)
  ; ADC Control Options B: Free-running trigger source
  .equ	ADCSRB_L= ( (0<<MUX5) | \
					(0<<ACME) | \
					(0<<ADTS2) | \
					(0<<ADTS1) | \
					(0<<ADTS0) )
 #endif

; DIDR Options: no disable digital input
.equ	DIDR0_L	= ( (0<<ADC7D) | \
					(0<<ADC6D) | \
					(0<<ADC5D) | \
					(0<<ADC4D) | \
					(0<<ADC3D) | \
					(0<<ADC2D) | \
					(0<<ADC1D) | \
					(0<<ADC0D) )
;
.equ	ADC_Ref			= 256				;256 = 2.56V 
;
#endif
.endif
