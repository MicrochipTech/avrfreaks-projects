;***************************************************************************
;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
;* 
;* Number		:AVR000
;* File Name		:"tn13def.inc"
;* Title		:Register/Bit Definitions for the ATtiny13
;* Date                 :02.07.05
;* Version              :1.00
;* Support E-mail	:support@atmel.no
;* Target MCU		:ATtiny13
;*
;* DESCRIPTION
;* When including this file in the assembly program file, all I/O register	
;* names and I/O register bit names appearing in the data book can be used.
;* In addition, the two registers forming the data pointer Z have been 
;* assigned names ZL - ZH. 
;*
;* The Register names are represented by their hexadecimal address.
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
; 
;***************************************************************************

;***** Specify Device
;device ATtiny13

	
;*****************************************************************************
; I/O Register Definitions
;*****************************************************************************

.equ	SREG	=$3F
.equ	SPL	=$3D
.equ	GIMSK	=$3B
.equ	GICR	=$3B		; for verification program compatibility	
.equ	GIFR	=$3A
.equ	TIMSK0	=$39
.equ	TIFR0	=$38
.equ	SPMCSR  =$37
.equ	OCR0A	=$36
.equ	MCUCR	=$35
.equ	MCUSR	=$34
.equ	TCCR0B	=$33
.equ	TCNT0	=$32
.equ	OSCCAL	=$31
.equ	TCCR0A	=$2F
.equ	DWDR	=$2E
.equ	OCR0B	=$29
.equ	GTCCR	=$28
.equ	CLKPR	=$26
.equ	WDTCR	=$21

.equ   	EEAR	=$1E
.equ	EEARL	=$1E
.equ	EEDR	=$1D
.equ	EECR	=$1C
.equ	PORTB	=$18
.equ	DDRB	=$17
.equ	PINB	=$16
.equ	PCMSK	=$15
.equ	DIDR0	=$14	
.equ	ACSR	=$08
.equ	ADMUX	=$07
.equ	ADCSRA	=$06
.equ	ADCH	=$05
.equ	ADCL	=$04
.equ	ADCSRB	=$03
	
	

	
;*****************************************************************************
; Bit Definitions
;*****************************************************************************

.equ SP7	      = 7
.equ SP6        = 6  		;SPL
.equ SP5        = 5
.equ SP4        = 4
.equ SP3        = 3
.equ SP2        = 2
.equ SP1        = 1
.equ SP0        = 0

.equ INT0       = 6  		;GIMSK
.equ PCIE       = 5

.equ INTF0      = 6  		;GIFR
.equ PCIF       = 5

.equ OCIE0B     = 3  ;TIMSK
.equ OCIE0A     = 2
.equ TOIE0      = 1

.equ OCF0B      = 3  ;TIFR
.equ OCF0A      = 2
.equ TOV0       = 1

.equ TSM	= 7 ; GTCCR
.equ PSR10	= 0 

.equ PUD        = 6  ;MCUCR
.equ SE         = 5
.equ SM1        = 4
.equ SM0        = 3
.equ ISC01      = 1
.equ ISC00      = 0

.equ WDRF       = 3  ;MCUCSR
.equ BORF       = 2
.equ EXTRF      = 1
.equ PORF       = 0

.equ CTPB	= 4 ;SPMCSR 
.equ RFLB	= 3  
.equ PGWRT	= 2 
.equ PGERS	= 1
.equ SPMEN	= 0

.equ COM0A1     = 7  ;TCCR0A
.equ COM0A0     = 6
.equ COM0B1     = 5
.equ COM0B0     = 4
.equ WGM01      = 1
.equ WGM00      = 0

.equ FOC0A      = 7  ;TCCR0B
.equ FOC0B      = 6
.equ WGM02	= 3
.equ CS02       = 2
.equ CS01       = 1
.equ CS00       = 0	
	
.equ CLKPCE	= 7  ;CLKPR 
.equ CLKPS3	= 3
.equ CLKPS2	= 2
.equ CLKPS1	= 1
.equ CLKPS0	= 0

.equ WDTIF	= 7  ;WDTCR 
.equ WDTIE	= 6
.equ WDP3	= 5	
.equ WDCE	= 4
.equ WDE	= 3
.equ WDP2	= 2
.equ WDP1	= 1
.equ WDP0	= 0

.equ EEPM1      = 5  ;EECR	
.equ EEPM0      = 4	
.equ EERIE      = 3
.equ EEMPE      = 2	
.equ EEPE       = 1
.equ EERE       = 0
	
; Kept for backward compatibility
.equ EEMWE      = 2	
.equ EEWE       = 1

	

.equ PORTB5     = 5  ;PORTB
.equ PORTB4     = 4
.equ PORTB3     = 3
.equ PORTB2     = 2
.equ PORTB1     = 1
.equ PORTB0     = 0

.equ DDB5       = 5  ;DDRB
.equ DDB4       = 4
.equ DDB3       = 3
.equ DDB2       = 2
.equ DDB1       = 1
.equ DDB0       = 0

.equ PINB5      = 5  ;PINB
.equ PINB4      = 4
.equ PINB3      = 3
.equ PINB2      = 2
.equ PINB1      = 1
.equ PINB0      = 0

.equ PCINT5     = 5  ;PCMSK
.equ PCINT4     = 4
.equ PCINT3     = 3
.equ PCINT2     = 2
.equ PCINT1     = 1
.equ PCINT0     = 0

.equ ADC0D      = 5  ;DIDR0
.equ ADC2D      = 4
.equ ADC3D      = 3
.equ ADC1D      = 2
.equ AIN1D      = 1
.equ AIN0D      = 0

.equ ACD        = 7  ;ACSR
.equ ACBG       = 6	
.equ AINBG      = 6  ;kept for compatibility
.equ ACO        = 5
.equ ACI        = 4
.equ ACIE       = 3
.equ ACIS1      = 1
.equ ACIS0      = 0


.equ REFS0      = 6 ;ADMUX
.equ ADLAR      = 5	
.equ MUX1       = 1
.equ MUX0       = 0

.equ ADEN       = 7  ;ADCSRA
.equ ADSC       = 6
.equ ADATE      = 5
.equ ADIF       = 4
.equ ADIE       = 3
.equ ADPS2      = 2
.equ ADPS1      = 1
.equ ADPS0      = 0

.equ ACME       = 6
.equ ADTS2      = 2
.equ ADTS1      = 1
.equ ADTS0      = 0


;*****************************************************************************
; CPU Register Declarations
;*****************************************************************************

.def	XL	= r26		; X pointer low
.def	XH	= r27		; X pointer high
.def	YL	= r28		; Y pointer low
.def	YH	= r29		; Y pointer high
.def	ZL	= r30		; Z pointer low
.def	ZH	= r31		; Z pointer high


;*****************************************************************************
; Data Memory Declarations
;*****************************************************************************

.equ 	RAMEND	=   0x9f	; Highest internal data memory (SRAM) address.
				;(64 Bytes RAM + IO + REG)
.equ	EEPROMEND = 0x3f       ; Highest EEPROM address.
	                        ;(64 Bytes)
.equ    EEADRBITS = 6		; no. of bits in EEPROM address register


;*****************************************************************************
; Program Memory Declarations
;*****************************************************************************

.equ    FLASHEND = 0x1FF	;  Highest program memory (flash) address
	                        ; (When addressed as 16 bit words)
				; ( 512 words , 1k byte ) 

	
;**** Page Size ****
.equ	PAGESIZE	=16     ;number of WORDS in a Flash page
.equ	EEPAGESIZE	= 2     ;number of WORDS in an EEPROM page

;**** Interrupt Vectors ****		
.equ	INT0addr     	=$001	;External Interrupt0
.equ	PCINT0addr   	=$002	;Pin Change Interrupt0
.equ	TIM0_OVF0addr     	=$003	;Overflow0 Interrupt 
.equ	EE_RDYaddr     	=$004	;EEPROM write complete
.equ	ANA_COMPaddr	=$005	;Analog Comparator Interrupt 
.equ	TIM0_COMPAaddr     	=$006	;Timer/Counter0 Compare Match A 	
.equ	TIM0_COMPBaddr   	=$007	;Timer/Counter0 Compare Match B 	
.equ	WDTaddr	     	=$008	;Watchdog Timeout
.equ    ADCaddr     	=$009	;ADC Conversion Complete Handle

