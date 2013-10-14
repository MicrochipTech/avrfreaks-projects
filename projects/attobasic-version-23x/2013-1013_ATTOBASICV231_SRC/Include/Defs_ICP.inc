;**********************************************************************
;	Definitions for ICP registers
;**********************************************************************
.if ICP
 #if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) )
 .equ	ICP_PORT	= PORTD			;define T1 on PD5
 .equ	ICP_PIN		= PIND5

 .equ	ICP_TIMSK	= TIMSK1
 .equ	ICP_TIFR	= TIFR1
 .equ	ICP_TCCRA	= TCCR1A
 .equ	ICP_TCCRB	= TCCR1B
 .equ	ICP_TCCRC	= TCCR1C
 .equ	ICP_TCNTH	= TCNT1H
 .equ	ICP_TCNTL	= TCNT1L
 .equ	ICP_ICRH	= ICR1L
 .equ	ICP_ICRL	= ICR1L

;	TIFR register
 .equ	ICP_ICF		= ICF1
 .equ	ICP_TOV		= TOV1
 ;
 #elif defined(__ATmega32U4__)
 .equ	ICP_PORT	= PORTD			;define T1 on PD6
 .equ	ICP_PIN		= PIND6

 .equ	ICP_TIMSK	= TIMSK1
 .equ	ICP_TIFR	= TIFR1
 .equ	ICP_TCCRA	= TCCR1A
 .equ	ICP_TCCRB	= TCCR1B
 .equ	ICP_TCCRC	= TCCR1C
 .equ	ICP_TCNTH	= TCNT1H
 .equ	ICP_TCNTL	= TCNT1L
 .equ	ICP_ICRH	= ICR1L
 .equ	ICP_ICRL	= ICR1L

;	TIFR register
 .equ	ICP_ICF		= ICF1
 .equ	ICP_TOV		= TOV1
 ;
 #elif defined(__AT90USB1286__)
 .equ	ICP_PORT	= PORTE			;define T3 on PE6
 .equ	ICP_PIN		= PINE6

 .equ	ICP_TIMSK	= TIMSK3
 .equ	ICP_TIFR	= TIFR3
 .equ	ICP_TCCRA	= TCCR3A
 .equ	ICP_TCCRB	= TCCR3B
 .equ	ICP_TCCRC	= TCCR3C
 .equ	ICP_TCNTH	= TCNT3H
 .equ	ICP_TCNTL	= TCNT3L
 .equ	ICP_ICRH	= ICR3L
 .equ	ICP_ICRL	= ICR3L

;	TIFR register
 .equ	ICP_ICF		= ICF3
 .equ	ICP_TOV		= TOV3
#elif defined(__ATmega2560__)
 .equ	ICP_PORT	= PORTC			;define T3 on PC3
 .equ	ICP_PIN		= PINC3

 .equ	ICP_TIMSK	= TIMSK3
 .equ	ICP_TIFR	= TIFR3
 .equ	ICP_TCCRA	= TCCR3A
 .equ	ICP_TCCRB	= TCCR3B
 .equ	ICP_TCCRC	= TCCR3C
 .equ	ICP_TCNTH	= TCNT3H
 .equ	ICP_TCNTL	= TCNT3L
 .equ	ICP_ICRH	= ICR3L
 .equ	ICP_ICRL	= ICR3L

;	TIFR register
 .equ	ICP_ICF		= ICF3
 .equ	ICP_TOV		= TOV3
#else
  #error	"!! Invalid Target MCU !!"
#endif
.endif


