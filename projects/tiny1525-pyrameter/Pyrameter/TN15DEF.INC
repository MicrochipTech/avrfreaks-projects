;    Last change:  TF    8 Sep 99    8:18 pm
;***************************************************************************
;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
;*
;* Number            : AVR000
;* File Name         : "T15def.inc"
;* Title             : Register/Bit Definitions for the ATtiny15
;* Date              : 1999.09.08
;* Version           : 1.00
;* Support telephone : +47 72 88 43 88 (ATMEL Norway)
;* Support fax       : +47 72 88 43 99 (ATMEL Norway)
;* Support E-Mail    : avr@atmel.com
;* Target MCU        : ATtiny15
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
;* in    r16,PORTB       ;read PORTB latch
;* sbr    r16,(1<<PB6)+(1<<PB5)    ;set PB6 and PB5 (use masks, not bit#)
;* out  PORTB,r16        ;output to PORTB
;*
;* in      r16,TIFR      ;read the Timer Interrupt Flag Register
;* sbrc    r16,TOV0      ;test the overflow flag (use bit#)
;* rjmp    TOV0_is_set   ;jump if set
;* ...                   ;otherwise do something else
;***************************************************************************

;***** Specify Device
.device ATtiny15

;***** I/O Register Definitions
.equ    SREG    =$3F
.equ    SPL     =$3D ; ICE only !!!!!!
.equ    GIMSK   =$3B
.equ    GIFR    =$3A
.equ    TIMSK   =$39
.equ    TIFR    =$38
.equ    MCUCR   =$35
.equ    MCUSR   =$34
.equ    TCCR0   =$33
.equ    TCNT0   =$32
.equ    OSCCAL  =$31
.equ    TCCR1   =$30
.equ    TCNT1   =$2F
.equ    OCR1A   =$2E
.equ    OCR1B   =$2D
.equ    SFIOR   =$2C
.equ    WDTCR   =$21
.equ    EEAR    =$1E
.equ    EEDR    =$1D
.equ    EECR    =$1C
.equ    PORTB   =$18
.equ    DDRB    =$17
.equ    PINB    =$16
.equ    ACSR    =$08
.equ    ADMUX   =$07
.equ    ADCSR   =$06
.equ    ADCH    =$05
.equ    ADCL    =$04


;***** Bit Definitions

; GIMSK
.equ    INT0    =6
.equ    PCIE    =5

; GIFR
.equ    INTF0   =6
.equ    PCIF    =5

; TIMSK
.equ    OCIE1   =6
.equ    TOIE1   =2
.equ    TOIE0   =1

; TIFR
.equ    OCF1    =6
.equ    TOV1    =2
.equ    TOV0    =1

; MCUCR

.equ    PUD     =6
.equ	SE	=5
.equ	SM	=4
.equ	SM1	=4
.equ	SM0	=3
.equ	ISC01	=1
.equ	ISC00	=0

; MCUSR
.equ    WDRF    =3
.equ    BORF    =2
.equ    EXTRF   =1
.equ    PORF    =0

; TCCR0
.equ    CS02    =2
.equ    CS01    =1
.equ    CS00    =0

; TCCR1
.equ    CTC1    =7
.equ    PWM1    =6
.equ    COM1A1  =5
.equ    COM1A0  =4
.equ    CS13    =3
.equ    CS12    =2
.equ    CS11    =1
.equ    CS10    =0

; WDTCR
.equ    WDTOE   =4
.equ    WDE     =3
.equ    WDP2    =2
.equ    WDP1    =1
.equ    WDP0    =0

; EECR
.equ    EERIE   =3
.equ    EEMWE   =2
.equ    EEWE    =1
.equ    EERE    =0

; PORTB
.equ    PB4     =4
.equ    PB3     =3
.equ    PB2     =2
.equ    PB1     =1
.equ    PB0     =0

; DDRB
.equ    DDB5    =5
.equ    DDB4    =4
.equ    DDB3    =3
.equ    DDB2    =2
.equ    DDB1    =1
.equ    DDB0    =0

; PINB
.equ    PINB5   =5
.equ    PINB4   =4
.equ    PINB3   =3
.equ    PINB2   =2
.equ    PINB1   =1
.equ    PINB0   =0

; ACSR
.equ    ACD     =7
.equ    AINBG6  =6
.equ    ACO     =5
.equ    ACI     =4
.equ    ACIE    =3
.equ    ACIS1   =1
.equ    ACIS0   =0

; ADMUX
.equ    REFS1   =7
.equ    REFS0   =6
.equ    ADLAR   =5
.equ    MUX2    =2
.equ    MUX1    =1
.equ    MUX0    =0

;ADCSR
.equ    ADEN    =7
.equ    ADSC    =6
.equ    ADFR    =5
.equ    ADIF    =4
.equ    ADIE    =3
.equ    ADPS2   =2
.equ    ADPS1   =1
.equ    ADPS0   =0

;SFIOR
.equ    FOC1A   =2
.equ    PSR1    =1
.equ    PSR0    =0

.equ    RAMEND  =0       ; Last On-Chip SRAM Location
.equ    XRAMEND =0       ; Last External RAM Location
.equ    E2END   =3F      ; Last EEPROM Location
.equ    FLASHEND=1FF     ; Last FLASH Location

.equ    INT0addr=$001    ; External Interrupt0 Vector Address
.equ    PCaddr  =$002    ; Pin Change Interrupt Vector Address
.equ    T1CPaddr=$003    ; Timer1 Compare Interrupt Vector Address
.equ    T1OVaddr=$004    ; Timer1 Overflow Interrupt Vector Address
.equ    T0OVaddr=$005    ; Timer0 Overflow Interrupt Vector Address
.equ    ERDYaddr=$006    ; EEPROM Ready Interrupt Vector Address
.equ    ACaddr  =$007    ; Analog Comparator Interrupt Vector Address
.equ    ADCaddr =$008    ; AD Converter Interrupt Vector Address

.def    ZL      =r30
.def    ZH      =r31

