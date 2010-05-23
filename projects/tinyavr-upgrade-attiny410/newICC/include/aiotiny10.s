;-------------------------------------------------
; ICCtiny asm header file
; target = ATtiny10/11
; 08/05/2010
;
; creation of int vectors is supported by:
; - macro "set_vector_table <dummy_arg>"
;   makes whole vector table with original labels
; - iv_<original label>
;   to be used with .interrupt directive
;   e.g.
;   .interrupt "t0_handler", iv_TIMER0_OVF
;
; please note:
; - macro leave with area set to text.
; - macro need dummy argument or will not expand
; - .interrupt directive must not come before
;    definition of label used
; - .interrupt directive needs label be exported
;-------------------------------------------------
;
;
; General Control
.set GTCCR                   =0x2F
 .set TSM                     =7
 .set PSR10                   =0
;
;
; Watchdog Timer Control
.set WDTCR                   =0x31
 .set WDIF                    =7
 .set WDTIF                   =7
 .set WDIE                    =6
 .set WDTIE                   =6
 .set WDP3                    =5
 .set WDE                     =3
 .set WDP2                    =2
 .set WDP1                    =1
 .set WDP0                    =0
;
;
; NVM
.set NVMCSR                  =0x32
.set NVMCMD                  =0x33
 .set NVMBSY                  =7
;
;
; VLM
.set VLMCSR                  =0x34
 .set VLMF                    =7
 .set VLMIE                   =6
 .set VLM2                    =2
 .set VLM1                    =1
 .set VLM0                    =0
;
;
; Power Reduction
.set PRR                     =0x35
 .set PRADC                   =1
 .set PRTIM0                  =0
;
;
; Clock Control
.set CLKPSR                  =0x36
.set CLKMSR                  =0x37
 .set CLKPS3                  =3
 .set CLKPS2                  =2
 .set CLKPS1                  =1
 .set CLKPS0                  =0
 .set CLKMS1                  =1
 .set CLKMS0                  =0
;
;
; RC oscillator
.set OSCCAL                  =0x39
;
;
; MCU
.set SMCR                    =0x3A
 .set SM2                     =3
 .set SM1                     =2
 .set SM0                     =1
 .set SE                      =0
;
.set RSTFLR                  =0x3B
 .set WDRF                    =3
 .set EXTRF                   =1
 .set PORF                    =0
;
.set CCP                     =0x3C
;
; Stack Pointer
.set SP                      =0x3D
.set SPL                     =0x3D
.set SPH                     =0x3E
;
; Status REGister
.set SREG                    =0x3F
;
;
; Port B
.set PUEB                    =0x03
 .set PUEB3                   =3
 .set PUEB2                   =2
 .set PUEB1                   =1
 .set PUEB0                   =0
;
.set PORTB                   =0x02
 .set PORTB3                  =3
 .set PORTB2                  =2
 .set PORTB1                  =1
 .set PORTB0                  =0
 .set PB3                     =3
 .set PB2                     =2
 .set PB1                     =1
 .set PB0                     =0
;
.set DDRB                    =0x01
 .set DDB3                    =3
 .set DDB2                    =2
 .set DDB1                    =1
 .set DDB0                    =0
;
.set PINB                    =0x00
 .set PINB3                   =3
 .set PINB2                   =2
 .set PINB1                   =1
 .set PINB0                   =0
.set PORTCR                  =0x0C
 .set BBMB                    =1
;
.set PCMSK                   =0x10
 .set PCINT3                  =3
 .set PCINT2                  =2
 .set PCINT1                  =1
 .set PCINT0                  =0
;
.set DIDR0                   =0x17
 .set ADC3D                   =3
 .set ADC2D                   =2
 .set ADC1D                   =1
 .set ADC0D                   =0
;
; Pin Interrupt 
.set PCIFR                   =0x11
 .set PCIF0                   =0
;
.set PCICR                   =0x12
 .set PCIE0                   =0
;
.set EIMSK                   =0x13
 .set INT0                    =0
;
.set EIFR                    =0x14
 .set INTF0                   =0
;
.set EICRA                   =0x15
 .set ISC00                   =0
 .set ISC01                   =1
;
;
; ADC
.set ADC                     =0x19
.set ADCL                    =0x19
;
.set ADMUX                   =0x1B
.set ADCMUX                  =0x1B
 .set MUX1                    =1
 .set MUX0                    =0
;
.set ADCSRB                  =0x1C
 .set ADEN                    =7
 .set ADSC                    =6
 .set ADATE                   =5
 .set ADFR                    =5
 .set ADIF                    =4
 .set ADIE                    =3
 .set ADPS2                   =2
 .set ADPS1                   =1
 .set ADPS0                   =0
;
.set ADCSRA                  =0x1D
 .set ADTS2                   =2
 .set ADTS1                   =1
 .set ADTS0                   =0
;
;
; Analog Comp
.set ACSR                    =0x1F
 .set ACD                     =7
 .set ACO                     =5
 .set ACI                     =4
 .set ACIE                    =3
 .set ACIC                    =2
 .set ACIS1                   =1
 .set ACIS0                   =0
;
:
; Timer/Counter 0
.set ICR0                    =0x22
.set ICR0L                   =0x22
.set ICR0H                   =0x23
;
.set OCR0A                   =0x26
.set OCR0AL                  =0x26
.set OCR0AH                  =0x27
.set OCR0B                   =0x24
.set OCR0BL                  =0x24
.set OCR0BH                  =0x25
.set TCNT0                   =0x28
.set TCNT0L                  =0x28
.set TCNT0H                  =0x29
;
.set TIFR0                   =0x2A
.set TIFR                    =0x2A
 .set ICF0                    =5
 .set OCF0A                   =1
 .set OCF0B                   =2
 .set TOV0                    =0
;
.set TIMSK0                  =0x2B
.set TIMSK                   =0x2B
 .set ICIE0                   =5
 .set OCIE0A                  =1
 .set OCIE0B                  =2
 .set TOIE0                   =0
;
.set TCCR0A                  =0x2E
 .set COM0A1                  =7
 .set COM0A0                  =6
 .set COM0B1                  =5
 .set COM0B0                  =4
 .set WGM01                   =1
 .set WGM00                   =0
;
.set TCCR0B                  =0x2D
 .set ICNC0                   =7
 .set ICES0                   =6
 .set WGM03                   =4
 .set WGM02                   =3
 .set CS02                    =2
 .set CS01                    =1
 .set CS00                    =0
;
.set TCCR0C                  =0x2C
 .set FOC0A                   =7
 .set FOC0B                   =6
;
;
;-------------------------------------------------
; useful memory definitions
;
.set RAMEND  ="0x5F" ;Last On-Chip reg/io Location
.set FLASHEND="0x3FF";Last On-Chip Flash Location
;-------------------------------------------------
.macro set_vector_table dummy
 .text
 .interrupt	"EXT_INT0",1
 .interrupt	"PIN_CHANGE",2
 .interrupt	"TIM0_CAPT",3
 .interrupt	"TIM0_OVF",4
 .interrupt	"TIM0_COMPA",5
 .interrupt	"TIM0_COMPB",6
 .interrupt	"ANA_COMP",7
 .interrupt	"WDT",8
 .interrupt	"VLM",9
 .interrupt	"ADC",10
.endm
;
.set iv_INT0 		= "1"
.set iv_IO_PINS		= "2"
.set iv_PIN_CHANGE	= "2"
.set iv_TIMER0_CAPT	= "3"
.set iv_TIMER0_OVF	= "4"
.set iv_TIMER0_OVF0	= "4"
.set iv_TIMER0_COMPA	= "5"
.set iv_TIMER0_COMPB	= "6"
.set iv_ANA_COMP	= "7"
.set iv_ANALOG_COMP	= "7"
.set iv_WDT		= "8"
.set iv_VLM		= "9"
.set iv_ADC		= "10"
;-------------------------------------------------
;<eof>
