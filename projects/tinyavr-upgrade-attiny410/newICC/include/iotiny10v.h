#ifndef __IOTINY10_h
#define __IOTINY10_h


#ifdef _AVR
#error "ICCAVR does not support RAM-less types"
#else


/* ATtiny10 register and bit name header file
 * for ImageCraft ICCtiny compiler
 */


/* IO registers */

/* Port B */
unsigned int ioloc  PINB	=0x00; 
unsigned int ioloc  DDRB	=0x01; 
unsigned int ioloc  PORTB	=0x02;
unsigned int ioloc  PUEB	=0x03;
unsigned int ioloc  PORTCR	=0x0c;
unsigned int ioloc  DIDR0	=0x17; 

/* Pin Interrupts */
unsigned int ioloc  PCMSK	=0x10;
unsigned int ioloc  PCIFR	=0x11;
unsigned int ioloc  PCICR	=0x12;
unsigned int ioloc  EIMSK	=0x13; 
unsigned int ioloc  EIFR	=0x14; 
unsigned int ioloc  EICRA	=0x15; 

/* ADC */
unsigned int ioloc  ADC		=0x19;
unsigned int ioloc  ADCL	=0x19;
unsigned int ioloc  ADMUX	=0x1b;
unsigned int ioloc  ADCMUX	=0x1b;
unsigned int ioloc  ADCSRB	=0x1c;
unsigned int ioloc  ADCSRA	=0x1d;

/* Analog Comp */
unsigned int ioloc  ACSR	=0x1f;

/* Timer/Counter 0 */
/*unsigned int ioloc  ICR0	=0x22; */
unsigned int ioloc  ICR0L	=0x22; 
unsigned int ioloc  ICR0H	=0x23; 
/*unsigned int ioloc  OCR0A	=0x26; */ 
unsigned int ioloc  OCR0AL	=0x26; 
unsigned int ioloc  OCR0AH	=0x27; 
/*unsigned int ioloc  OCR0B	=0x24; */ 
unsigned int ioloc  OCR0BL	=0x24; 
unsigned int ioloc  OCR0BH	=0x25; 
/*unsigned int ioloc  TCNT0	=0x28; */
unsigned int ioloc  TCNT0L	=0x28; 
unsigned int ioloc  TCNT0H	=0x29; 
unsigned int ioloc  TCCR0A	=0x2e; 
unsigned int ioloc  TCCR0B	=0x2d; 
unsigned int ioloc  TCCR0C	=0x2c; 

/* Timer/Counter Interrupts */
unsigned int ioloc  TIFR0	=0x2a;
unsigned int ioloc  TIFR	=0x2a; 
unsigned int ioloc  TIMSK0	=0x2b; 
unsigned int ioloc  TIMSK	=0x2b; 

/* General Control */
unsigned int ioloc  GTCCR	=0x2f;

/* Watchdog Timer Control */
unsigned int ioloc  WDTCR	=0x31;

/* NVM */
unsigned int ioloc  NVMCSR	=0x32; 
unsigned int ioloc  NVMCMD	=0x33; 

/* VLM */
unsigned int ioloc  VLMCSR	=0x34; 

/* Power Reduction */
unsigned int ioloc  PRR		=0x35; 

/* Clock Control */
unsigned int ioloc  CLKPSR	=0x36; 
unsigned int ioloc  CLKMSR	=0x37; 

/* RC oscillator */
unsigned int ioloc  OSCCAL	=0x39; 

/* MCU */
unsigned int ioloc  SMCR	=0x3a; 
unsigned int ioloc  RSTFLR	=0x3b; 
unsigned int ioloc  CCP		=0x3c; 
	
/* Stack Pointer */
unsigned int ioloc  SP		=0x3d; 
unsigned int ioloc  SPL		=0x3d; 

/* Status REGister */
unsigned int ioloc  SREG	=0x3F; 




/* IO bits */

/* Port B bits */
#define    PUEB3    3
#define    PUEB2    2
#define    PUEB1    1
#define    PUEB0    0

#define    PORTB3   3
#define    PORTB2   2
#define    PORTB1   1
#define    PORTB0   0
#define    PB3      3
#define    PB2      2
#define    PB1      1
#define    PB0      0

#define    DDB3     3
#define    DDB2     2
#define    DDB1     1
#define    DDB0     0

#define    PINB3    3
#define    PINB2    2
#define    PINB1    1
#define    PINB0    0

#define    BBMB     1

#define    PCINT3   3
#define    PCINT2   2
#define    PCINT1   1
#define    PCINT0   0

#define    ADC3D    3
#define    ADC2D    2
#define    ADC1D    1
#define    ADC0D    0

/* Pin Interrupt bits */
#define    ISC00    0
#define    ISC01    1
#define    INT0     0
#define    INTF0    0

#define    PCIE0    0
#define    PCIF0    0

/* Analog Digital Converter bits */
#define    MUX1     1
#define    MUX0     0

#define    ADEN     7
#define    ADSC     6
#define    ADATE    5
#define    ADFR     5
#define    ADIF     4
#define    ADIE     3
#define    ADPS2    2
#define    ADPS1    1
#define    ADPS0    0

#define    ADTS2    2
#define    ADTS1    1
#define    ADTS0    0

/* Analog Comparator bits */
#define    ACD      7
#define    ACO      5
#define    ACI      4
#define    ACIE     3
#define    ACIC     2
#define    ACIS1    1
#define    ACIS0    0

/* Timer/Counter Interrupt bits */
#define    ICIE0    5
#define    OCIE0A   1
#define    OCIE0B   2
#define    TOIE0    0

#define    ICF0     5
#define    OCF0A    1
#define    OCF0B    2
#define    TOV0     0

/* Timer/Counter 0 Control bits */
#define    FOC0A    7
#define    FOC0B    6

#define    ICNC0    7
#define    ICES0    6
#define    WGM03    4
#define    WGM02    3
#define    CS02     2
#define    CS01     1
#define    CS00     0

#define    COM0A1   7
#define    COM0A0   6
#define    COM0B1   5
#define    COM0B0   4
#define    WGM01    1
#define    WGM00    0

/* GTCCR bits */
#define    TSM      7
#define    PSR10    0

/* Watchdog Timer Control bits */
#define    WDIF     7
#define    WDTIF    7
#define    WDIE     6
#define    WDTIE    6
#define    WDP3     5
#define    WDE      3
#define    WDP2     2
#define    WDP1     1
#define    WDP0     0

/* NVM bits */
#define    NVMBSY   7

/* VLM bits */
#define    VLMF     7
#define    VLMIE    6
#define    VLM2     2
#define    VLM1     1
#define    VLM0     0

/* PRR bits */
#define    PRADC    1
#define    PRTIM0   0

/* CLK Control bits */
#define    CLKPS3   3
#define    CLKPS2   2
#define    CLKPS1   1
#define    CLKPS0   0

#define    CLKMS1   1
#define    CLKMS0   0

/* Sleep Mode Control bits */
#define    SM2      3
#define    SM1      2
#define    SM0      1
#define    SE       0

/* Reset Flag bits */
#define    WDRF     3
#define    EXTRF    1
#define    PORF     0


/* Lock and Config Fuses bits */

/* lock bits */
#define  NVLB2    1
#define  LB2      1
#define  NVLB1    0
#define  LB1      0

/* config bits */
#define  CKOUT    2
#define  WDTON    1
#define  RSTDISBL 0


/* Interrupt Vector Numbers */

#define iv_RESET        1
#define iv_INT0         2
#define iv_PCINT0       3
#define iv_TIM0_CAPT    4
#define iv_TIM0_OVF     5
#define iv_TIM0_COMPA   6
#define iv_TIM0_COMPB   7
#define iv_ANA_COMP     8
#define iv_WDT          9
#define iv_VLM          10
#define iv_ADC          11

/* */

#endif
#endif
