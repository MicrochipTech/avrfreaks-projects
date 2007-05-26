;***************************************************************************
;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
;* 
;* Number				:AVR000
;* File Name			:"m162def.inc"
;* Title				:Register/Bit Definitions for the ATmega162
;* Date                 :2001.09.19
;* Version              :1.00
;* Support telephone	:+47 72 88 43 88 (ATMEL Norway)
;* Support fax			:+47 72 88 43 99 (ATMEL Norway)
;* Support E-mail		:avr@atmel.no
;* Target MCU			:ATmega162
;*
;* DESCRIPTION
;* When including this file in the assembly program file, all I/O register	
;* names and I/O register bit names appearing in the data book can be used.
;* In addition, the six registers forming the three data pointers X, Y and
;* Z have been assigned names XL - ZH. Highest RAM address for Internal 
;* SRAM is also defined 
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
;***************************************************************************

;***** Specify Device
.device ATmega162
	
;**** Memory Mapped I/O Register Definitions ($FF-$60) ****
;**** Not available in Mega162 compatibility mode      ****
.equ	TCCR3A	=$8b
.equ	TCCR3B	=$8a	
.equ	TCNT3H	=$89
.equ	TCNT3L	=$88
.equ	OCR3AH	=$87
.equ	OCR3AL	=$86	
.equ	OCR3BH	=$85
.equ	OCR3BL	=$84
.equ	ICR3H	=$81
.equ	ICR3L	=$80
.equ	ETIMSK  =$7d
.equ	ETIFR	=$7c
.equ	PCMSK1	=$6c
.equ	PCMSK0	=$6b
.equ	CLKPR	=$61	
				
;***** I/O Register Definitions	
.equ	SREG	=$3f
.equ	SPH		=$3e
.equ	SPL		=$3d
.equ	UCSR1C	=$3c		; Note! UCSR1C equals UBRR1H
.equ	UBRR1H	=$3c		; Note! UCSR1C equals UBRR1H
.equ	EIMSK	=$3b	
.equ	GIMSK	=$3b
.equ	GICR	=$3b		; new name for GIMSK
.equ	GIFR	=$3a
.equ	TIMSK	=$39
.equ	TIFR	=$38
.equ	SPMCR	=$37
.equ	EMCUCR	=$36
.equ	MCUCR	=$35
.equ    MCUSR   =$34		; For compatibility, 
.equ    MCUCSR  =$34		; keep both names until further	
.equ	TCCR0	=$33
.equ	TCNT0	=$32
.equ	OCR0	=$31
.equ	SFIOR	=$30
.equ	TCCR1A	=$2f
.equ	TCCR1B	=$2e
.equ	TCNT1H	=$2d
.equ	TCNT1L	=$2c
.equ	OCR1AH	=$2b
.equ	OCR1AL	=$2a
.equ	OCR1BH	=$29
.equ	OCR1BL	=$28
.equ	TCCR2	=$27
.equ    ASSR    =$26
.equ	ICR1H	=$25
.equ	ICR1L	=$24
.equ	TCNT2	=$23
.equ	OCR2	=$22
.equ	WDTCR	=$21
.equ    UBRRHI  =$20		; Old ATmega161
.equ	UCSR0C	=$20		; Note! UCSR0C equals UBRR0H
.equ	UBRR0H	=$20		; Note! UCSR0C equals UBRR0H		
.equ	EEARH	=$1f
.equ	EEARL	=$1e
.equ	EEDR	=$1d
.equ	EECR	=$1c
.equ	PORTA	=$1b
.equ	DDRA	=$1a
.equ	PINA	=$19
.equ	PORTB	=$18
.equ	DDRB	=$17
.equ	PINB	=$16
.equ	PORTC	=$15
.equ	DDRC	=$14
.equ	PINC	=$13
.equ	PORTD	=$12
.equ	DDRD	=$11
.equ	PIND	=$10
.equ	SPDR	=$0f
.equ	SPSR	=$0e
.equ	SPCR	=$0d
.equ	UDR0	=$0c
.equ	UDR		=$0c		;for compatibility with s8515
.equ	UCSR0A	=$0b
.equ	USR		=$0b		;for compatibility with s8515
.equ	UCSR0B	=$0a
.equ	UCR		=$0a		;for compatibility with s8515
.equ	UBRR0	=$09		;Old mega161
.equ	UBRR0L	=$09	
.equ	UBRR	=$09		;for compatibility with s8515
.equ	ACSR	=$08
.equ	PORTE	=$07
.equ	DDRE	=$06
.equ	PINE	=$05
.equ	OSCCAL	=$04		; New
.equ	UDR1	=$03
.equ	UCSR1A	=$02
.equ	UCSR1B	=$01
.equ	UBRR1	=$00		;Old mega161
.equ	UBRR1L	=$00

;***** Bit Definitions
;TCCR3A
.equ	COM3A1	=7
.equ	COM3A0	=6
.equ	COM3B1	=5
.equ	COM3B0	=4
.equ	FOC3A	=3
.equ	FOC3B	=2
.equ	WGM31	=1
.equ	WGM30	=0

;TCCR3A
.equ	ICNC3	=7
.equ	ICES3	=6
.equ	WGM33	=4
.equ	WGM32	=3
.equ	CS32	=2
.equ	CS31	=1
.equ	CS30	=0

;ETIMSK
.equ	TICIE3	=5
.equ	OCIE3A	=4
.equ	OCIE3B	=3
.equ	TOIE3	=2
					 
;ETIFR
.equ	ICF3	=5
.equ	OCF3A	=4
.equ	OCF3B	=3
.equ	TOV3	=2

;PCMSK1
.equ	PCINT15	=7
.equ	PCINT14	=6
.equ	PCINT13	=5
.equ    PCINT12	=4	
.equ	PCINT11	=3
.equ    PCINT10	=2	
.equ	PCINT9	=1
.equ	PCINT8	=0
	
;PCMSK1
.equ	PCINT7  =7
.equ	PCINT6  =6
.equ	PCINT5  =5
.equ    PCINT4  =4	
.equ	PCINT3 	=3
.equ    PCINT2 	=2	
.equ	PCINT1	=1
.equ	PCINT0  =0

;CLKPR
.equ	CLKPCE  =7
.equ	CLKPS3 	=3
.equ    CLKPS2 	=2	
.equ	CLKPS1	=1
.equ	CLKPS0  =0
			
;GIMSK
.equ	INT1	=7
.equ	INT0	=6
.equ	INT2	=5
.equ	PCIE1 	=4
.equ    PCIE0 	=3		
.equ	IVSEL	=1		; Interrupt vector select
.equ	IVCE	=0		; Interrupt vector change enable
	
;GIFR
.equ	INTF1	=7
.equ	INTF0	=6
.equ	INTF2	=5
.equ	PCIF1	=4
.equ	PCIF0	=3	
	
	
;TIMSK
.equ	TOIE1	=7
.equ	OCIE1A	=6
.equ	OCIE1B	=5
.equ    OCIE2   =4
.equ	TICIE1	=3
.equ    TOIE2   =2
.equ	TOIE0	=1
.equ	OCIE0	=0

;TIFR
.equ	TOV1	=7
.equ	OCF1A	=6
.equ	OCF1B	=5
.equ	OCF2	=4
.equ	ICF1	=3
.equ	TOV2	=2
.equ	TOV0	=1
.equ	OCF0	=0

;SPMCR
.equ	SPMIE	=7
.equ	RWWSB	=6
.equ	ASB	=6			; old
.equ	RWWSRE	=4
.equ	ASRE	=4		; old
.equ	BLBSET	=3
.equ	PGWRT	=2
.equ	PGERS	=1
.equ	SPMEN	=0

;EMCUCR
.equ    SM0     =7
.equ	SRL2	=6
.equ	SRL1	=5
.equ	SRL0	=4
.equ	SRW01	=3
.equ	SRW00	=2
.equ	SRW11	=1
.equ	ISC2	=0

;MCUCR
.equ	SRE		=7
.equ	SRW		=6		;for compatibility with s8515
.equ	SRW10	=6
.equ	SE		=5
.equ	SM		=4		;for compatibility with s8515
.equ	SM1		=4
.equ	ISC11	=3
.equ	ISC10	=2
.equ	ISC01	=1
.equ	ISC00	=0

;MCUSR
.equ	JTD     =7	
.equ	SM2     =5	
.equ    JTRF    =4	
.equ	WDRF	=3
.equ	BORF	=2
.equ	EXTRF	=1
.equ	PORF	=0

;TCCR0
.equ    FOC0    =7
.equ	WGM00	=6
.equ    PWM0    =6		; OBSOLETE! Use WGM00
.equ	COM01	=5
.equ	COM00	=4
.equ	WGM01	=3
.equ	CTC0	=3		; OBSOLETE! Use WGM01
.equ	CS02	=2
.equ	CS01	=1
.equ	CS00	=0

;SFIOR
.equ	TSM		= 7
.equ	XMBK	= 6		; Added for Mega162
.equ	XMM2	= 5
.equ	XMM1	= 4
.equ	XMM0	= 3
.equ	PUD		= 2
.equ    PSR2    = 1
.equ    PSR10   = 0		; Note:	The prescaler reset is shared
						; between timer 0 and 1.
.equ    PSR1	= 0
.equ    PSR0	= 0

;TCCR1A
.equ	COM1A1	=7
.equ	COM1A0	=6
.equ	COM1B1	=5
.equ	COM1B0	=4
.equ    FOC1A   =3
.equ    FOC1B   =2
.equ	PWM11	=1		; OBSOLETE! Use WGM11
.equ	WGM11	=1
.equ	PWM10	=0		; OBSOLETE! Use WGM10
.equ	WGM10	=0

;TCCR1B
.equ	ICNC1	=7
.equ	ICES1	=6
.equ	CTC11	=4		; OBSOLETE! Use WGM13
.equ	WGM13	=4
.equ	CTC10	=3		; OBSOLETE! Use WGM12
.equ	WGM12	=3
.equ	CTC1	=3		; Old Mega161
.equ	CS12	=2
.equ	CS11	=1
.equ	CS10	=0

;TCCR2
.equ    FOC2    =7
.equ	WGM20	=6
.equ    PWM2    =6		; OBSOLETE! Use WGM20
.equ	COM21	=5
.equ	COM20	=4
.equ	WGM21	=3
.equ	CTC2	=3		; OBSOLETE! Use WGM21
.equ	CS22	=2
.equ	CS21	=1
.equ	CS20	=0

;ASSR
.equ	AS2	=3
.equ	TCN2UB	=2
.equ	OCR2UB	=1
.equ	TCR2UB	=0

;WDTCR
.equ	WDTOE	=4
.equ	WDCE	=4		; Added for Mega161B
.equ	WDE	=3
.equ	WDP2	=2
.equ	WDP1	=1
.equ	WDP0	=0

;EECR
.equ    EERIE   =3
.equ	EEMWE	=2
.equ	EEWE	=1
.equ	EERE	=0

;PORTA
.equ	PORTA7	=7
.equ	PORTA6	=6
.equ	PORTA5	=5
.equ	PORTA4	=4
.equ	PORTA3	=3
.equ	PORTA2	=2
.equ	PORTA1	=1
.equ	PORTA0	=0

;DDRA
.equ	DDA7	=7
.equ	DDA6	=6
.equ	DDA5	=5
.equ	DDA4	=4
.equ	DDA3	=3
.equ	DDA2	=2
.equ	DDA1	=1
.equ	DDA0	=0

;PINA
.equ	PINA7	=7
.equ	PINA6	=6
.equ	PINA5	=5
.equ	PINA4	=4
.equ	PINA3	=3
.equ	PINA2	=2
.equ	PINA1	=1
.equ	PINA0	=0

;PORTB
.equ	PORTB7	=7
.equ	PORTB6	=6
.equ	PORTB5	=5
.equ	PORTB4	=4
.equ	PORTB3	=3
.equ	PORTB2	=2
.equ	PORTB1	=1
.equ	PORTB0	=0

;DDRB
.equ	DDB7	=7
.equ	DDB6	=6
.equ	DDB5	=5
.equ	DDB4	=4
.equ	DDB3	=3
.equ	DDB2	=2
.equ	DDB1	=1
.equ	DDB0	=0

;PINB
.equ	PINB7	=7
.equ	PINB6	=6
.equ	PINB5	=5
.equ	PINB4	=4
.equ	PINB3	=3
.equ	PINB2	=2
.equ	PINB1	=1
.equ	PINB0	=0

;PORTC
.equ	PORTC7	=7
.equ	PORTC6	=6
.equ	PORTC5	=5
.equ	PORTC4	=4
.equ	PORTC3	=3
.equ	PORTC2	=2
.equ	PORTC1	=1
.equ	PORTC0	=0

;DDRC
.equ	DDC7	=7
.equ	DDC6	=6
.equ	DDC5	=5
.equ	DDC4	=4
.equ	DDC3	=3
.equ	DDC2	=2
.equ	DDC1	=1
.equ	DDC0	=0

;PINC
.equ	PINC7	=7
.equ	PINC6	=6
.equ	PINC5	=5
.equ	PINC4	=4
.equ	PINC3	=3
.equ	PINC2	=2
.equ	PINC1	=1
.equ	PINC0	=0

;PORTD
.equ	PORTD7	=7
.equ	PORTD6	=6
.equ	PORTD5	=5
.equ	PORTD4	=4
.equ	PORTD3	=3
.equ	PORTD2	=2
.equ	PORTD1	=1
.equ	PORTD0	=0

;DDRD
.equ	DDD7	=7
.equ	DDD6	=6
.equ	DDD5	=5
.equ	DDD4	=4
.equ	DDD3	=3
.equ	DDD2	=2
.equ	DDD1	=1
.equ	DDD0	=0

;PIND
.equ	PIND7	=7
.equ	PIND6	=6
.equ	PIND5	=5
.equ	PIND4	=4
.equ	PIND3	=3
.equ	PIND2	=2
.equ	PIND1	=1
.equ	PIND0	=0

;PORTE
.equ	PORTE2	=2
.equ	PORTE1	=1
.equ	PORTE0	=0

;DDRE
.equ	DDE2	=2
.equ	DDE1	=1
.equ	DDE0	=0

;PINE
.equ	PINE2	=2
.equ	PINE1	=1
.equ	PINE0	=0

;USR (for compatibility with s8515)
.equ	RXC		=7
.equ	TXC		=6
.equ	UDRE	=5
.equ	FE		=4
.equ	OR		=3
.equ	U2X		=1

;UCSR0A
.equ	RXC0	=7
.equ	TXC0	=6
.equ	UDRE0	=5
.equ	FE0		=4
.equ	OR0		=3		; Old name kept for compatibilty
.equ	DOR0	=3
.equ	PE0		=2		; New
.equ	U2X0	=1
.equ	MPCM0	=0

;UCSR1A
.equ	RXC1	=7
.equ	TXC1	=6
.equ	UDRE1	=5
.equ	FE1		=4
.equ	OR1		=3		; Old name kept for compatibilty
.equ	DOR1	=3
.equ	PE1		=2		; New
.equ	U2X1	=1
.equ	MPCM1	=0

;SPCR
.equ	SPIE	=7
.equ	SPE		=6
.equ	DORD	=5
.equ	MSTR	=4
.equ	CPOL	=3
.equ	CPHA	=2
.equ	SPR1	=1
.equ	SPR0	=0

;SPSR
.equ	SPIF	=7
.equ	WCOL	=6
.equ	SPI2X	=0

;UCR (for compatibility with s8515)
.equ	RXCIE	=7
.equ	TXCIE	=6
.equ	UDRIE	=5
.equ	RXEN	=4
.equ	TXEN	=3
.equ	CHR9	=2		; Old name kept for compatibilty
.equ	UCSZ2	=2
.equ	RXB8	=1
.equ	TXB8	=0

; UCSR0B
.equ	RXCIE0	=7
.equ	TXCIE0	=6
.equ	UDRIE0	=5
.equ	RXEN0	=4
.equ	TXEN0	=3
.equ	CHR90	=2		; Old name kept for compatibilty
.equ	UCSZ02	=2
.equ	RXB80	=1
.equ	TXB80	=0

; UCSR1B
.equ	RXCIE1	=7
.equ	TXCIE1	=6
.equ	UDRIE1	=5
.equ	RXEN1	=4
.equ	TXEN1	=3
.equ	CHR91	=2		; Old name kept for compatibilty
.equ	UCSZ12	=2
.equ	RXB81	=1
.equ	TXB81	=0
	
;UCSR0C					New 
.equ	URSEL0	=7
.equ	UMSEL0	=6
.equ	UPM01	=5
.equ	UPM00	=4
.equ	USBS0	=3
.equ	UCSZ01	=2
.equ	UCSZ00	=1
.equ	UCPOL0	=0

;UCSR1C					New 
.equ	URSEL1	=7
.equ	UMSEL1	=6
.equ	UPM11	=5
.equ	UPM10	=4
.equ	USBS1	=3
.equ	UCSZ11	=2
.equ	UCSZ10	=1
.equ	UCPOL1	=0
	

;ACSR
.equ	ACD		=7
.equ	AINBG	=6		; Old mega161
.equ	ACBG	=6	
.equ	ACO		=5
.equ	ACI		=4
.equ	ACIE	=3
.equ	ACIC	=2
.equ	ACIS1	=1
.equ	ACIS0	=0



; Boot loader Lock bit
.equ	BLB12	=5
.equ	BLB11	=4
.equ	BLB02	=3
.equ	BLB01	=2

;*****************************************************************************
; CPU Register Declarations
;*****************************************************************************

.def	XL		= r26		; X pointer low
.def	XH		= r27		; X pointer high
.def	YL		= r28		; Y pointer low
.def	YH		= r29		; Y pointer high
.def	ZL		= r30		; Z pointer low
.def	ZH		= r31		; Z pointer high


;*****************************************************************************
; Data Memory Declarations
;*****************************************************************************

.equ 	RAMEND	= $488		; Highest internal data memory (SRAM) address.
							;(1k RAM + IO + REG)
.equ	EEPROMEND = $01ff   ; Highest EEPROM address.
	                        ;(512 byte)
;*****************************************************************************
; Program Memory Declarations
;*****************************************************************************

.equ    FLASHEND = $1FFF	;  Highest program memory (flash) address
	                        ; (When addressed as 16 bit words)
							; ( 8k words , 16k byte ) 
		
;**** Boot Vectors ****
					   ;  byte groups
					 ;   /--\/--\/--\ 
.equ 	SMALLBOOTSTART	=0b1111110000000  ;($1F80) smallest boot block is 256B
.equ 	SECONDBOOTSTART	=0b1111100000000  ;($1F00) second boot block size is 512B
.equ 	THIRDBOOTSTART	=0b1111000000000  ;($1E00) third boot block size is 1KB
.equ 	LARGEBOOTSTART	=0b1110000000000  ;($1C00) largest boot block is 2KB
.equ 	BOOTSTART	=THIRDBOOTSTART  ;OBSOLETE!!! kept for compatibility
.equ	PAGESIZE	=64     ;number of WORDS in a page




.equ	INT0addr		= $002	;	External Interrupt Request 0
.equ	INT1addr		= $004	;	External Interrupt Request 1
.equ	INT2addr		= $006	;	External Interrupt Request 2
.equ	PCINT0addr		= $008	;	Pin Change Interrupt Request 0
.equ	PCINT1addr		= $00A
.equ	TIMER3CAPTaddr	= $00C
.equ	TIMER3COMPAaddr	= $00E
.equ	TIMER3COMPBaddr	= $010
.equ	TIMER3OVFaddr	= $012
.equ	TIMER2COMPaddr	= $014
.equ	TIMER2OVFaddr	= $016
.equ	TIMER1CAPTaddr	= $018
.equ	TIMER1COMPAaddr	= $01A
.equ	TIMER1COMPBaddr	= $01C
.equ	TIMER1OVFaddr	= $01E
.equ	TIMER0COMPaddr	= $020
.equ	TIMER0OVFaddr	= $022
.equ	SPISTCaddr		= $024
.equ	USART0RXCaddr	= $026
.equ	USART1RXCaddr	= $028
.equ	USART0UDREaddr	= $02A
.equ	USART1UDREaddr	= $02C
.equ	USART0TXCaddr	= $02E
.equ	USART1TXCaddr	= $030
.equ	EE_RDYaddr		= $032
.equ	ANA_CMPaddr		= $034
.equ	SPM_RDYaddr		= $036
