;***************************************************************************
;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
;* 
;* Number		:AVR000
;* File Name		:"1200def.inc"
;* Title		:Register/Bit Definitions for the AT90S1200
;* Date	 		:99.01.28
;* Version		:1.30
;* Support telephone	:+47 72 88 43 88 (ATMEL Norway)
;* Support fax		:+47 72 88 43 99 (ATMEL Norway)
;* Support E-Mail	:avr@atmel.com
;* Target MCU		:AT90S1200
;*
;* DESCRIPTION
;* When including this file in the assembly program file, all I/O register
;* names and I/O register bit names appearing in the data book can be used.
;* 
;* The Register names are represented by their hexadecimal addresses.
;* 
;* The Register Bit names are represented by their bit number (0-7).
;* 
;* Please observe the difference in using the bit names with instructions
;* such as "sbr"/"cbr" (set/clear bit in register) and "sbrs"/"sbrc" 
;* (skip if bit in register set/cleared). The following example illustrates
;* this:
;* 
;* in	r16,PORTB		;read PORTB latch
;* sbr	r16,(1<<PB6)+(1<<PB5)	;set PB6 and PB5 (use masks, not bit#)
;* out  PORTB,r16		;output to PORTB
;*
;* in	r16,TIFR		;read the Timer Interrupt Flag Register
;* sbrc	r16,TOV0		;test the overflow flag (use bit#)
;* rjmp	TOV0_is_set		;jump if set
;* ...				;otherwise do something else
;***************************************************************************

;***** Specify Device
.device AT90S1200

;***** I/O Register Definitions
.equ	SREG	=$3f
.equ	GIMSK	=$3b
.equ	TIMSK	=$39
.equ	TIFR	=$38
.equ	MCUCR	=$35
.equ	TCCR0	=$33
.equ	TCNT0	=$32
.equ	WDTCR	=$21
.equ	EEAR	=$1e
.equ	EEDR	=$1d
.equ	EECR	=$1c
.equ	PORTB	=$18
.equ	DDRB	=$17
.equ	PINB	=$16
.equ	PORTD	=$12
.equ	DDRD	=$11
.equ	PIND	=$10
.equ	ACSR	=$08

;***** Bit Definitions

.equ	INT0	=6

.equ	TOIE0	=1

.equ	TOV0	=1

.equ	SE	=5
.equ	SM	=4
.equ	ISC01	=1
.equ	ISC00	=0

.equ	CS02	=2
.equ	CS01	=1
.equ	CS00	=0

.equ	WDE	=3
.equ	WDP2	=2
.equ	WDP1	=1
.equ	WDP0	=0

.equ	EEWE	=1
.equ	EERE	=0

.equ	PB7	=7
.equ	PB6	=6
.equ	PB5	=5
.equ	PB4	=4
.equ	PB3	=3
.equ	PB2	=2
.equ	PB1	=1
.equ	PB0	=0

.equ	DDB7	=7
.equ	DDB6	=6
.equ	DDB5	=5
.equ	DDB4	=4
.equ	DDB3	=3
.equ	DDB2	=2
.equ	DDB1	=1
.equ	DDB0	=0

.equ	PINB7	=7
.equ	PINB6	=6
.equ	PINB5	=5
.equ	PINB4	=4
.equ	PINB3	=3
.equ	PINB2	=2
.equ	PINB1	=1
.equ	PINB0	=0

.equ	PD6	=6
.equ	PD5	=5
.equ	PD4	=4
.equ	PD3	=3
.equ	PD2	=2
.equ	PD1	=1
.equ	PD0	=0

.equ	DDD6	=6
.equ	DDD5	=5
.equ	DDD4	=4
.equ	DDD3	=3
.equ	DDD2	=2
.equ	DDD1	=1
.equ	DDD0	=0

.equ	PIND6	=6
.equ	PIND5	=5
.equ	PIND4	=4
.equ	PIND3	=3
.equ	PIND2	=2
.equ	PIND1	=1
.equ	PIND0	=0

.equ	ACD	=7
.equ	ACO	=5
.equ	ACI	=4
.equ	ACIE	=3
.equ	ACIS1	=1
.equ	ACIS0	=0

.equ	XRAMEND =$0
.equ	E2END	=$3F
.equ	FLASHEND=$1FF

.equ	INT0addr=$001	;External Interrupt0 Vector Address
.equ	OVF0addr=$002	;Overflow0 Interrupt Vector Address
.equ	ACIaddr =$003	;Analog Comparator Interrupt Vector Address

.def	ZL	=r30
.def	ZH	=r31
