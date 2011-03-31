/*H**************************************************************************
* $RCSfile: mcu.h,v $
*----------------------------------------------------------------------------
* Copyright (c) 2003 Atmel.
*----------------------------------------------------------------------------
* RELEASE:      $Name:  $
* REVISION:     $Revision: 1.6.6.2.2.19 $
* FILE_CVSID:   $Id: mcu.h,v 1.6.6.2.2.19 2005/11/28 15:23:21 jberthy Exp $
*----------------------------------------------------------------------------
* PURPOSE:
* SFR Description file for Atmegabalast.
*****************************************************************************/
#ifndef MCU_H
#define MCU_H

#define IRQ_MEM_SPACE	2

/*==========================*/
/* Predefined SFR Addresses */
/*==========================*/

/******************************************************************************/
#ifdef __IAR_SYSTEMS_ICC__
/******************************************************************************/
SFR_B(PINB,    0x03)    /* Input Pins, Port B */
SFR_B(DDRB,    0x04)    /* Data Direction Register, Port B */
SFR_B(PORTB,   0x05)    /* Data Register, Port B */

SFR_B(PINC,    0x06)    /* Input Pins, Port C */
SFR_B(DDRC,    0x07)    /* Data Direction Register, Port C */
SFR_B(PORTC,   0x08)    /* Data Register, Port C */

SFR_B(PIND,    0x09)    /* Input Pins, Port D */
SFR_B(DDRD,    0x0A)    /* Data Direction Register, Port D */
SFR_B(PORTD,   0x0B)    /* Data Register, Port D */

SFR_B(PINE,    0x0C)    /* Input Pins, Port E */
SFR_B(DDRE,    0x0D)    /* Data Direction Register, Port E */
SFR_B(PORTE,   0x0E)    /* Data Register, Port E */

SFR_B(TIFR0,   0x15)    /* Timer/Counter Interrupt Flag register 0*/
SFR_B(TIFR1,   0x16)    /* Timer/Counter Interrupt Flag register 1*/

SFR_B(GPIOR1,  0x19)    /* General Purpose Register 1 */
SFR_B(GPIOR2,  0x1A)    /* General Purpose Register 2 */
SFR_B(GPIOR3,  0x1B)    /* General Purpose Register 3 */

SFR_B(EIFR,    0x1C)    /* External Interrupt Flag Register */
SFR_B(EIMSK,   0x1D)    /* External Interrupt Mask Register */

SFR_B(GPIOR0,  0x1E)    /* General Purpose Register 0 */

SFR_B(EECR,    0x1F)    /* EEPROM Control Register */
SFR_B(EEDR,    0x20)    /* EEPROM Data Register */
SFR_W(EEAR,    0x21)    /* EEPROM Address Register */

SFR_B(GTCCR,   0x23)    /* General Purpose Register */

SFR_B(TCCR0A,  0x24)    /* Timer/Counter 0 Control Register */
SFR_B(TCCR0B,  0x25)    /* Timer/Counter 0 Control Register */

SFR_B(TCNT0,   0x26)    /* Timer/Counter 0 */
SFR_B(OCR0A,   0x27)    /* Timer/Counter 0 Output Compare Register */
SFR_B(OCR0B,   0x28)    /* Timer/Counter 0 Output Compare Register */

SFR_B(PLLCSR,  0x29)    /* Pll Control and status register */

SFR_B(SPCR,    0x2C)    /* SPI Control Register */
SFR_B(SPSR,    0x2D)    /* SPI Status Register */
SFR_B(SPDR,    0x2E)    /* SPI I/O Data Register */

SFR_B(ACSR,    0x30)    /* Analog Comparator Control and Status Register */

SFR_B(MONDR,   0x31)    /* On-Chip Debug Register */
SFR_B(MSMCR,   0x32)    /* Monitor stop mode ctrl register */

SFR_B(SMCR,    0x33)    /* Sleep Mode Control Register */

SFR_B(MCUSR,   0x34)    /* MCU Status Register */
SFR_B(MCUCR,   0x35)    /* MCU Control Register */

SFR_B(SPMCSR,  0x37)    /* Store Program Memory Control and Status Register */

SFR_W(SP,      0x3D)    /* Stack Pointer */

SFR_B(SREG,    0x3F)    /* Status Register */

SFR_B_EXT(WDTCSR,  0x60) /* Watchdog Timer Control Register */
SFR_B_EXT(CLKPR,  0x61) /* Clock Prescale Register */
SFR_B_EXT(PRR,    0x64) /* Power Reduction Register */

SFR_B_EXT(OSCCAL, 0x66) /* Oscillator Calibration Register */

SFR_B_EXT(EICRA,  0x69) /* External Interrupt Control Register A */

SFR_B_EXT(TIMSK0, 0x6E) /* Timer/Counter 0 Interrupt Mask Register */
SFR_B_EXT(TIMSK1, 0x6F) /* Timer/Counter 1 Interrupt Mask Register */

SFR_B_EXT(AMP0CSR,0x76) /* Amplifier 0 ctrl and status register */
SFR_B_EXT(AMP1CSR,0x77) /* Amplifier 1 ctrl and status register */

SFR_W_EXT(ADC,    0x78) /* ADC Data register  */
SFR_B_EXT(ADCSRA, 0x7A) /* ADC Control and Status Register A */
SFR_B_EXT(ADCSRB, 0x7B) /* ADC Control and Status Register B */
SFR_B_EXT(ADMUX,  0x7C) /* ADC Multiplexer Selection Register */

SFR_B_EXT(DIDR0,  0x7E) /* Digital Input Disable Register 0 */
SFR_B_EXT(DIDR1,  0x7F) /* Digital Input Disable Register 1 */

SFR_B_EXT(TCCR1A, 0x80) /* Timer/Counter 1 Control Register A */
SFR_B_EXT(TCCR1B, 0x81) /* Timer/Counter 1 Control Register B */
SFR_B_EXT(TCCR1C, 0x82) /* Timer/Counter 1 Control Register C */
SFR_W_EXT(TCNT1,  0x84) /* Timer/Counter 1 Register */
SFR_W_EXT(ICR1,   0x86) /* Timer/Counter 1 Input Capture Register */
SFR_W_EXT(OCR1A,  0x88) /* Timer/Counter 1 Output Compare Register A */
SFR_W_EXT(OCR1B,  0x8A) /* Timer/Counter 1 Output Compare Register B */

SFR_B_EXT(PIFR0,  0xA0) /* PSC 0 Interrupt Flag Register */
SFR_B_EXT(PIM0,   0xA1) /* PSC 0 Interrupt Mask Register */

SFR_B_EXT(PIFR1,  0xA2) /* PSC 1 Interrupt Flag Register */
SFR_B_EXT(PIM1,   0xA3) /* PSC 1 Interrupt Mask Register */

SFR_B_EXT(PIFR2,  0xA4) /* PSC 2 Interrupt Flag Register */
SFR_B_EXT(PIM2,   0xA5) /* PSC 2 Interrupt Mask Register */

SFR_B_EXT(DACON,  0xAA) /* DAC Control Register*/
SFR_W_EXT(DAC,    0xAB) /* DAC Data Register*/

SFR_B_EXT(AC0CON, 0xAD) /* Analog Comparator 0 status register */
SFR_B_EXT(AC1CON, 0xAE) /* Analog Comparator 1 status register */
SFR_B_EXT(AC2CON, 0xAF) /* Analog Comparator 2 status register */

SFR_B_EXT(UCSRA,  0xC0) /* USART Control and Status Register A */
SFR_B_EXT(UCSRB,  0xC1) /* USART Control and Status Register B */
SFR_B_EXT(UCSRC,  0xC2) /* USART Control and Status Register C */
SFR_W_EXT(UBRR,   0xC4) /* USART Baud Rate Register Low */
SFR_B_EXT(UDR,    0xC6) /* USART0 I/O Data Register */

/* UCSR0x left for software compatibility*/
SFR_B_EXT(UCSR0A, 0xC0) /* USART Control and Status Register A */
SFR_B_EXT(UCSR0B, 0xC1) /* USART Control and Status Register B */
SFR_B_EXT(UCSR0C, 0xC2) /* USART Control and Status Register C */
SFR_W_EXT(UBRR0,  0xC4) /* USART Baud Rate Register Low */
SFR_B_EXT(UDR0,   0xC6) /* USART0 I/O Data Register */

SFR_B_EXT(EUCSRA, 0xC8) /* EUSART Control and Status Register A */
SFR_B_EXT(EUCSRB, 0xC9) /* EUSART Control and Status Register B */
SFR_B_EXT(EUCSRC, 0xCA) /* EUSART Control and Status Register C */
SFR_W_EXT(MUBRR,  0xCC) /* EUSART Manchester counter max value */
SFR_B_EXT(EUDR,   0xCE) /* USART0 I/O Data Register */

SFR_B_EXT(PSOC0,  0xD0) /* PSC 0 Synchro & Output Configuration */
SFR_W_EXT(OCR0SA, 0xD2) /* PSC 0 Output Compare Register SA */
SFR_W_EXT(OCR0RA, 0xD4) /* PSC 0 Output Compare Register RA */
SFR_W_EXT(OCR0SB, 0xD6) /* PSC 0 Output Compare Register SB */
SFR_W_EXT(OCR0RB, 0xD8) /* PSC 0 Output Compare Register RB */
SFR_B_EXT(PCNF0,  0xDA) /* PSC 0 Configuration Register */
SFR_B_EXT(PCTL0,  0xDB) /* PSC 0 Control Register */
SFR_B_EXT(PFRC0A, 0xDC) /* PSC 0 Input A Control Register */
SFR_B_EXT(PFRC0B, 0xDD) /* PSC 0 Input B Control Register */
SFR_W_EXT(PICR0,  0xDE) /* PSC 0 Input Capture Register */

SFR_B_EXT(PSOC1,  0xE0) /* PSC 1 Synchro & Output Configuration */
SFR_W_EXT(OCR1SA, 0xE2) /* PSC 1 Output Compare Register SA */
SFR_W_EXT(OCR1RA, 0xE4) /* PSC 1 Output Compare Register RA */
SFR_W_EXT(OCR1SB, 0xE6) /* PSC 1 Output Compare Register SB */
SFR_W_EXT(OCR1RB, 0xE8) /* PSC 1 Output Compare Register RB */
SFR_B_EXT(PCNF1,  0xEA) /* PSC 1 Configuration Register */
SFR_B_EXT(PCTL1,  0xEB) /* PSC 1 Control Register */
SFR_B_EXT(PFRC1A, 0xEC) /* PSC 1 Input A Control Register */
SFR_B_EXT(PFRC1B, 0xED) /* PSC 1 Input B Control Register */
SFR_W_EXT(PICR1,  0xEE) /* PSC 1 Input Capture Register */

SFR_B_EXT(PSOC2,  0xF0) /* PSC 2 Synchro & Output Configuration */
SFR_B_EXT(POM2,   0xF1) /* PSC 2 Output Matrix Register */
SFR_W_EXT(OCR2SA, 0xF2) /* PSC 2 Output Compare Register SA */
SFR_W_EXT(OCR2RA, 0xF4) /* PSC 2 Output Compare Register RA */
SFR_W_EXT(OCR2SB, 0xF6) /* PSC 2 Output Compare Register SB */
SFR_W_EXT(OCR2RB, 0xF8) /* PSC 2 Output Compare Register RB */
SFR_B_EXT(PCNF2,  0xFA) /* PSC 2 Configuration Register */
SFR_B_EXT(PCTL2,  0xFB) /* PSC 2 Control Register */
SFR_B_EXT(PFRC2A, 0xFC) /* PSC 2 Input A Control Register */
SFR_B_EXT(PFRC2B, 0xFD) /* PSC 2 Input B Control Register */
SFR_W_EXT(PICR2,  0xFE) /* PSC 2 Input Capture Register */

/*==============================*/
/* Interrupt Vector Definitions */
/*==============================*/
/* NB! vectors are specified as byte addresses */
#define    RESET_vect         (0x00*IRQ_MEM_SPACE)
#define    PSC2_CAPT_vect     (0x01*IRQ_MEM_SPACE)
#define    PSC2EC_vect        (0x02*IRQ_MEM_SPACE)
#define    PSC1_CAPT_vect    	(0x03*IRQ_MEM_SPACE)
#define    PSC1EC_vect       	(0x04*IRQ_MEM_SPACE)
#define    PSC0_CAPT_vect    	(0x05*IRQ_MEM_SPACE)
#define    PSC0EC_vect       	(0x06*IRQ_MEM_SPACE)
#define    ANACOMP_0_vect     (0x07*IRQ_MEM_SPACE)
#define    ANACOMP_1_vect    	(0x08*IRQ_MEM_SPACE)
#define    ANACOMP_2_vect    	(0x09*IRQ_MEM_SPACE)
#define    INT0_vect          (0x0A*IRQ_MEM_SPACE)
#define    TIMER1_CAPT_vect   (0x0B*IRQ_MEM_SPACE)
#define    TIMER1_COMPA_vect  (0x0C*IRQ_MEM_SPACE)
#define    TIMER1_COMPB_vect  (0x0D*IRQ_MEM_SPACE)
#define    TIMER1_OVF_vect    (0x0F*IRQ_MEM_SPACE)
#define    TIMER0_COMPA_vect  (0x10*IRQ_MEM_SPACE)
#define    TIMER0_OVF_vect    (0x11*IRQ_MEM_SPACE)
#define    ADC_vect           (0x12*IRQ_MEM_SPACE)
#define    INT1_vect          (0x13*IRQ_MEM_SPACE)
#define    SPI_STC_vect       (0x14*IRQ_MEM_SPACE)
#define    USART_RXC_vect     (0x15*IRQ_MEM_SPACE)
#define    USART_UDRE_vect    (0x16*IRQ_MEM_SPACE)
#define    USART_TXC_vect     (0x17*IRQ_MEM_SPACE)
#define    INT2_vect          (0x18*IRQ_MEM_SPACE)
#define    WDT_vect           (0x19*IRQ_MEM_SPACE)
#define    EE_RDY_vect        (0x1A*IRQ_MEM_SPACE)
#define    TIMER0_COMPB_vect  (0x1B*IRQ_MEM_SPACE)
#define    INT3_vect          (0x1C*IRQ_MEM_SPACE)
#define    SPM_READY_vect     (0x1F*IRQ_MEM_SPACE)

#endif /* _IAR_ */
/******************************************************************************/
#ifdef _ICC_
/******************************************************************************/

#define PINB    (*(volatile unsigned char *)0x23) /* Input Pins, Port B */
#define DDRB    (*(volatile unsigned char *)0x24) /* Data Direction Register, Port B */
#define PORTB   (*(volatile unsigned char *)0x25) /* Data Register, Port B */

#define PINC    (*(volatile unsigned char *)0x26) /* Input Pins, Port C */
#define DDRC    (*(volatile unsigned char *)0x27) /* Data Direction Register, Port C */
#define PORTC   (*(volatile unsigned char *)0x28) /* Data Register, Port C */

#define PIND    (*(volatile unsigned char *)0x29) /* Input Pins, Port D */
#define DDRD    (*(volatile unsigned char *)0x2A) /* Data Direction Register, Port D */
#define PORTD   (*(volatile unsigned char *)0x2B) /* Data Register, Port D */

#define PINE    (*(volatile unsigned char *)0x2C) /* Input Pins, Port E */
#define DDRE    (*(volatile unsigned char *)0x2D) /* Data Direction Register, Port E */
#define PORTE   (*(volatile unsigned char *)0x2E) /* Data Register, Port E */

#define TIFR0   (*(volatile unsigned char *)0x35) /* Timer/Counter Interrupt Flag register 0*/
#define TIFR1   (*(volatile unsigned char *)0x36) /* Timer/Counter Interrupt Flag register 1*/

#define GPIOR1  (*(volatile unsigned char *)0x39)     /* General Purpose Register 1 */
#define GPIOR2  (*(volatile unsigned char *)0x3A)     /* General Purpose Register 2 */
#define GPIOR3  (*(volatile unsigned char *)0x3B)     /* General Purpose Register 3 */

#define EIFR    (*(volatile unsigned char *)0x3C)     /* External Interrupt Flag Register */
#define EIMSK   (*(volatile unsigned char *)0x3D)     /* External Interrupt Mask Register */

#define GPIOR0  (*(volatile unsigned char *)0x3E)     /* General Purpose Register 0 */

#define EECR    (*(volatile unsigned char *)0x3F)     /* EEPROM Control Register */
#define EEDR    (*(volatile unsigned char *)0x40)     /* EEPROM Data Register */
#define EEAR    (*(volatile unsigned int *)0x41)      /* EEPROM Address Register */

#define GTCCR   (*(volatile unsigned char *)0x43)     /* General Purpose Register */

#define TCCR0A  (*(volatile unsigned char *)0x44)     /* Timer/Counter 0 Control Register */
#define TCCR0B  (*(volatile unsigned char *)0x45)     /* Timer/Counter 0 Control Register */
#define TCNT0   (*(volatile unsigned char *)0x46)     /* Timer/Counter 0 */
#define OCR0A   (*(volatile unsigned char *)0x47)     /* Timer/Counter 0 Output Compare Register */
#define OCR0B   (*(volatile unsigned char *)0x48)     /* Timer/Counter 0 Output Compare Register */

#define PLLCSR   (*(volatile unsigned char *)0x49)     /* Pll control and status Register */

#define SPCR    (*(volatile unsigned char *)0x4C)     /* SPI Control Register */
#define SPSR    (*(volatile unsigned char *)0x4D)     /* SPI Status Register */
#define SPDR    (*(volatile unsigned char *)0x4E)     /* SPI I/O Data Register */

#define ACSR    (*(volatile unsigned char *)0x50)     /* Analog Comparator Control and Status Register */

#define MONDR    (*(volatile unsigned char *)0x51)     /* On-Chip Debug Register */
#define MSMDR    (*(volatile unsigned char *)0x52)     /* Monitor Stop mode control Register */

#define SMCR    (*(volatile unsigned char *)0x53)     /* Sleep Mode Control Register */
#define MCUSR   (*(volatile unsigned char *)0x54)     /* MCU Status Register */
#define MCUCR   (*(volatile unsigned char *)0x55)     /* MCU Control Register */

#define SPMCSR  (*(volatile unsigned char *)0x57)     /* Store Program Memory Control and Status Register */

#define SP      (*(volatile unsigned int *)0x5D)      /* Stack Pointer */
#define SREG    (*(volatile unsigned char *)0x5F)     /* Status Register */

#define WDTCSR   (*(volatile unsigned char *)0x60)     /* Watchdog Timer Control Register */
#define CLKPR   (*(volatile unsigned char *)0x61)     /* Clock Prescale Register */
#define PRR   (*(volatile unsigned char *)0x64)     /* Power Reduction Register */
#define OSCCAL  (*(volatile unsigned char *)0x66)     /* Oscillator Calibration Register */

#define EICRA   (*(volatile unsigned char *)0x69)     /* External Interrupt Control Register A */

#define TIMSK0  (*(volatile unsigned char *)0x6E)     /* Timer/Counter 0 Interrupt Mask Register */
#define TIMSK1  (*(volatile unsigned char *)0x6F)     /* Timer/Counter 1 Interrupt Mask Register */

#define AMP0CSR (*(volatile unsigned int *)0x76)      /* Amplifier0 ctrl and status register */
#define AMP1CSR (*(volatile unsigned char *)0x77)     /* Amplifier1 ctrl and status register */

#define ADC     (*(volatile unsigned int *)0x78)      /* ADC Data register */
#define ADCL     (*(volatile unsigned char *)0x78)      /* ADC Data register */
#define ADCH     (*(volatile unsigned char *)0x79)      /* ADC Data register */
#define ADCSRA  (*(volatile unsigned char *)0x7A)     /* ADC Control and Status Register A */
#define ADCSRB  (*(volatile unsigned char *)0x7B)     /* ADC Control and Status Register B */
#define ADMUX   (*(volatile unsigned char *)0x7C)     /* ADC Multiplexer Selection Register */

#define DIDR0   (*(volatile unsigned char *)0x7E)     /* Digital Input Disable Register 0 */
#define DIDR1   (*(volatile unsigned char *)0x7F)     /* Digital Input Disable Register 1 */

#define TCCR1A  (*(volatile unsigned char *)0x80)     /* Timer/Counter 1 Control Register A */
#define TCCR1B  (*(volatile unsigned char *)0x81)     /* Timer/Counter 1 Control Register B */
#define TCCR1C  (*(volatile unsigned char *)0x82)     /* Timer/Counter 1 Control Register C */
#define TCNT1   (*(volatile unsigned int *)0x84)      /* Timer/Counter 1 Register */
#define ICR1    (*(volatile unsigned int *)0x86)      /* Timer/Counter 1 Input Capture Register */
#define OCR1A   (*(volatile unsigned int *)0x88)      /* Timer/Counter 1 Output Compare Register A */
#define OCR1AL   (*(volatile unsigned char *)0x88)      /* Timer/Counter 1 Output Compare Register A */
#define OCR1AH   (*(volatile unsigned char *)0x89)      /* Timer/Counter 1 Output Compare Register A */

#define OCR1B   (*(volatile unsigned int *)0x8A)      /* Timer/Counter 1 Output Compare Register B */

#define PIFR0   (*(volatile unsigned char *)0xA0)      /* PSC0 interrupt flag Register */
#define PIM0   (*(volatile unsigned char *)0xA1)      /* PSC 0 interrupt mask Register */

#define PIFR1   (*(volatile unsigned char *)0xA2)      /* PSC1 interrupt flag Register */
#define PIM1   (*(volatile unsigned char *)0xA3)      /* PSC 1 interrupt mask Register */

#define PIFR2   (*(volatile unsigned char *)0xA4)      /* PSC2 interrupt flag Register */
#define PIM2   (*(volatile unsigned char *)0xA5)      /* PSC 2 interrupt mask Register */

#define DACON   (*(volatile unsigned char *)0xAA)      /* DAC Control Register */
#define DAC   (*(volatile unsigned int *)0xAB)      /* DAC data Register */
#define DACL   (*(volatile unsigned char *)0xAB)      /* DAC data Register */
#define DACH   (*(volatile unsigned char *)0xAC)      /* DAC data Register */

#define AC0CON   (*(volatile unsigned char *)0xAD)      /* Analog Comparator 0 status register */
#define AC1CON   (*(volatile unsigned char *)0xAE)      /* Analog Comparator 1 status register */
#define AC2CON   (*(volatile unsigned char *)0xAF)      /* Analog Comparator 2 status register */

#define UCSRA  (*(volatile unsigned char *)0xC0)      /* USART Control and Status Register A */
#define UCSRB  (*(volatile unsigned char *)0xC1)      /* USART Control and Status Register B */
#define UCSRC  (*(volatile unsigned char *)0xC2)      /* USART Control and Status Register C */
#define UBRR   (*(volatile unsigned int *)0xC4)       /* USART Baud Rate Register  */
#define UBRRL  (*(volatile unsigned char *)0xC4)      /* USART Baud Rate Register Low */
#define UBRRH  (*(volatile unsigned char *)0xC5)      /* USART Baud Rate Register High */
#define UDR    (*(volatile unsigned char *)0xC6)      /* USART I/O Data Register */

/*USART0 left for compatibility*/
#define UCSR0A  (*(volatile unsigned char *)0xC0)     /* USART0 Control and Status Register A */
#define UCSR0B  (*(volatile unsigned char *)0xC1)     /* USART0 Control and Status Register B */
#define UCSR0C  (*(volatile unsigned char *)0xC2)     /* USART0 Control and Status Register C */
#define UBRR0   (*(volatile unsigned int *)0xC4)      /* USART0 Baud Rate Register  */
#define UBRR0L  (*(volatile unsigned char *)0xC4)     /* USART0 Baud Rate Register Low */
#define UBRR0H  (*(volatile unsigned char *)0xC5)     /* USART0 Baud Rate Register High */
#define UDR0    (*(volatile unsigned char *)0xC6)     /* USART0 I/O Data Register */

/*EUSART */
#define EUCSRA  (*(volatile unsigned char *)0xC8)     /* EUSART Control and Status Register A */
#define EUCSRB  (*(volatile unsigned char *)0xC9)     /* EUSART Control and Status Register B */
#define EUCSRC  (*(volatile unsigned char *)0xCA)     /* EUSART Control and Status Register C */
#define MUBRR	(*(volatile unsigned int *)0xCC)     /* EUSART Max manchester receiver counter */
#define MUBRRL  (*(volatile unsigned char *)0xCC)     /* EUSART Max manchester receiver counter */
#define MUBRRH   (*(volatile unsigned char *)0xCD)     /* EUSART Max manchester receiver counter */
#define EUDR    (*(volatile unsigned char *)0xCE)     /* USART0 I/O Data Register */

#define PSOC0  (*(volatile unsigned char *)0xD0)     /* PSC 0 Synchro & Output Configuration */
#define OCR0SA  (*(volatile unsigned int *)0xD2)     /* PSC 0 Output Compare Register SA */
#define OCR0SAL  (*(volatile unsigned char *)0xD2)     /* PSC 0 Output Compare Register SA */
#define OCR0SAH  (*(volatile unsigned char *)0xD3)     /* PSC 0 Output Compare Register SA */

#define OCR0RA  (*(volatile unsigned int *)0xD4)     /* PSC 0 Output Compare Register RA*/
#define OCR0RAL  (*(volatile unsigned char *)0xD4)     /* PSC 0 Output Compare Register RA*/
#define OCR0RAH  (*(volatile unsigned char *)0xD5)     /* PSC 0 Output Compare Register RA*/

#define OCR0SB   (*(volatile unsigned int *)0xD6)     /* PSC 0 Output Compare Register SB */
#define OCR0SBL   (*(volatile unsigned char *)0xD6)     /* PSC 0 Output Compare Register SB */
#define OCR0SBH   (*(volatile unsigned char *)0xD7)     /* PSC 0 Output Compare Register SB */

#define OCR0RB  (*(volatile unsigned int*)0xD8)     /* PSC 0 Output Compare Register RB */
#define OCR0RBL  (*(volatile unsigned char*)0xD8)     /* PSC 0 Output Compare Register RB */
#define OCR0RBH  (*(volatile unsigned char*)0xD9)     /* PSC 0 Output Compare Register RB */

#define PCNF0    (*(volatile unsigned char *)0xDA)     /* PSC 0 Configuration Register */
#define PCTL0  (*(volatile unsigned char *)0xDB)     /* PSC 0 Control Register */
#define PFRC0A   (*(volatile unsigned char *)0xDC)     /* PSC 0 Input A Control Register */
#define PFRC0B  (*(volatile unsigned char *)0xDD)     /* PSC 0 Input B Control Register */
#define PICR0    (*(volatile unsigned int *)0xDE)     /* PSC 0 Input Capture Register */


#define PSOC1  (*(volatile unsigned char *)0xE0)     /* PSC 1 Synchro & Output Configuration */
#define OCR1SA  (*(volatile unsigned int *)0xE2)     /* PSC 1 Output Compare Register SA */
#define OCR1SAL  (*(volatile unsigned char *)0xE2)     /* PSC 1 Output Compare Register SA */
#define OCR1SAH  (*(volatile unsigned char *)0xE3)     /* PSC 1 Output Compare Register SA */

#define OCR1RA  (*(volatile unsigned int *)0xE4)     /* PSC 1 Output Compare Register RA*/
#define OCR1RAL  (*(volatile unsigned char *)0xE4)     /* PSC 1 Output Compare Register RA*/
#define OCR1RAH  (*(volatile unsigned char *)0xE5)     /* PSC 1 Output Compare Register RA*/

#define OCR1SB   (*(volatile unsigned int *)0xE6)     /* PSC 1 Output Compare Register SB */
#define OCR1SBL   (*(volatile unsigned char *)0xE6)     /* PSC 1 Output Compare Register SB */
#define OCR1SBH   (*(volatile unsigned char *)0xE7)     /* PSC 1 Output Compare Register SB */

#define OCR1RB  (*(volatile unsigned int*)0xE8)     /* PSC 1 Output Compare Register RB */
#define OCR1RBL  (*(volatile unsigned char*)0xE8)     /* PSC 1 Output Compare Register RB */
#define OCR1RBH  (*(volatile unsigned char*)0xE9)     /* PSC 1 Output Compare Register RB */

#define PCNF1    (*(volatile unsigned char *)0xEA)     /* PSC 1 Configuration Register */
#define PCTL1  (*(volatile unsigned char *)0xEB)     /* PSC 1 Control Register */
#define PFRC1A   (*(volatile unsigned char *)0xEC)     /* PSC 1 Input A Control Register */
#define PFRC1B  (*(volatile unsigned char *)0xED)     /* PSC 1 Input B Control Register */
#define PICR1    (*(volatile unsigned int *)0xEE)     /* PSC 1 Input Capture Register */


#define PSOC2  (*(volatile unsigned char *)0xF0)     /* PSC 2 Synchro & Output Configuration */
#define OCR2SA  (*(volatile unsigned int *)0xF2)     /* PSC 2 Output Compare Register SA */
#define OCR2SAL  (*(volatile unsigned char *)0xF2)     /* PSC 2 Output Compare Register SA */
#define OCR2SAH  (*(volatile unsigned char *)0xF3)     /* PSC 2 Output Compare Register SA */

#define OCR2RA  (*(volatile unsigned int *)0xF4)     /* PSC 2 Output Compare Register RA*/
#define OCR2RAL  (*(volatile unsigned char *)0xF4)     /* PSC 2 Output Compare Register RA*/
#define OCR2RAH  (*(volatile unsigned char *)0xF5)     /* PSC 2 Output Compare Register RA*/

#define OCR2SB   (*(volatile unsigned int *)0xF6)     /* PSC 2 Output Compare Register SB */
#define OCR2SBL   (*(volatile unsigned char *)0xF6)     /* PSC 2 Output Compare Register SB */
#define OCR2SBH   (*(volatile unsigned char *)0xF7)     /* PSC 2 Output Compare Register SB */

#define OCR2RB  (*(volatile unsigned int*)0xF8)     /* PSC 2 Output Compare Register RB */
#define OCR2RBL  (*(volatile unsigned char*)0xF8)     /* PSC 2 Output Compare Register RB */
#define OCR2RBH  (*(volatile unsigned char*)0xF9)     /* PSC 2 Output Compare Register RB */

#define PCNF2    (*(volatile unsigned char *)0xFA)     /* PSC 2 Configuration Register */
#define PCTL2  (*(volatile unsigned char *)0xFB)     /* PSC 2 Control Register */
#define PFRC2A   (*(volatile unsigned char *)0xFC)     /* PSC 2 Input A Control Register */
#define PFRC2B  (*(volatile unsigned char *)0xFD)     /* PSC 2 Input B Control Register */
#define PICR2    (*(volatile unsigned int *)0xFE)     /* PSC 2 Input Capture Register */


/*==============================*/
/* Interrupt Vector Definitions */
/*==============================*/

#define    RESET_vect           0
#define    PSC2_CAPT_vect       1
#define    PSC2EC_vect          2
#define    PSC1_CAPT_vect    	3
#define    PSC1EC_vect       	4
#define    PSC0_CAPT_vect    	5
#define    PSC0EC_vect       	6
#define    ANACOMP_0_vect       7
#define    ANACOMP_1_vect    	8
#define    ANACOMP_2_vect    	9
#define    INT0_vect            10
#define    TIMER1_CAPT_vect     11
#define    TIMER1_COMPA_vect    12
#define    TIMER1_COMPB_vect    13
#define    TIMER1_OVF_vect      15
#define    TIMER0_COMPA_vect    16
#define    TIMER0_OVF_vect      17
#define    ADC_vect             18
#define    INT1_vect            19
#define    SPI_STC_vect         20
#define    USART_RXC_vect       21
#define    USART_UDRE_vect      22
#define    USART_TXC_vect       23
#define    INT2_vect            24
#define    WDT_vect             25
#define    EE_RDY_vect          26
#define    TIMER0_COMPB_vect    27
#define    INT3_vect            28
#define    SPM_READY_vect       31

#endif /* _ICC_*/



/*==========================*/
/* Bit Position Definitions */
/*==========================*/

/* PINB : Input Pins, Port B */
#define    PINB7    7
#define    PINB6    6
#define    PINB5    5
#define    PINB4    4
#define    PINB3    3
#define    PINB2    2
#define    PINB1    1
#define    PINB0    0

/* DDRB : Data Direction Register, Port B */
#define    DDB7     7
#define    DDB6     6
#define    DDB5     5
#define    DDB4     4
#define    DDB3     3
#define    DDB2     2
#define    DDB1     1
#define    DDB0     0

/* PORTB : Data Register, Port B */
#define    PB7      7
#define    PB6      6
#define    PB5      5
#define    PB4      4
#define    PB3      3
#define    PB2      2
#define    PB1      1
#define    PB0      0

/* PORTB : Data Register, Port B */
#define    PORTB7   7
#define    PORTB6   6
#define    PORTB5   5
#define    PORTB4   4
#define    PORTB3   3
#define    PORTB2   2
#define    PORTB1   1
#define    PORTB0   0

/* PINC : Input Pins, Port C */
#define    PINC7    7
#define    PINC6    6
#define    PINC5    5
#define    PINC4    4
#define    PINC3    3
#define    PINC2    2
#define    PINC1    1
#define    PINC0    0

/* DDRC : Data Direction Register, Port C */
#define    DDC7     7
#define    DDC6     6
#define    DDC5     5
#define    DDC4     4
#define    DDC3     3
#define    DDC2     2
#define    DDC1     1
#define    DDC0     0

/* PORTC : Data Register, Port C */
#define    PC7      7
#define    PC6      6
#define    PC5      5
#define    PC4      4
#define    PC3      3
#define    PC2      2
#define    PC1      1
#define    PC0      0

/* PORTC : Data Register, Port C */
#define    PORTC7   7
#define    PORTC6   6
#define    PORTC5   5
#define    PORTC4   4
#define    PORTC3   3
#define    PORTC2   2
#define    PORTC1   1
#define    PORTC0   0

/* PIND : Input Pins, Port D */
#define    PIND7    7
#define    PIND6    6
#define    PIND5    5
#define    PIND4    4
#define    PIND3    3
#define    PIND2    2
#define    PIND1    1
#define    PIND0    0

/* DDRD : Data Direction Register, Port D */
#define    DDD7     7
#define    DDD6     6
#define    DDD5     5
#define    DDD4     4
#define    DDD3     3
#define    DDD2     2
#define    DDD1     1
#define    DDD0     0

/* PORTD : Data Register, Port D */
#define    PD7      7
#define    PD6      6
#define    PD5      5
#define    PD4      4
#define    PD3      3
#define    PD2      2
#define    PD1      1
#define    PD0      0

/* PORTD : Data Register, Port D */
#define    PORTD7   7
#define    PORTD6   6
#define    PORTD5   5
#define    PORTD4   4
#define    PORTD3   3
#define    PORTD2   2
#define    PORTD1   1
#define    PORTD0   0

/* PINE : Input Pins, Port E */
#define    PINE7    7
#define    PINE6    6
#define    PINE5    5
#define    PINE4    4
#define    PINE3    3
#define    PINE2    2
#define    PINE1    1
#define    PINE0    0

/* DDRE : Data Direction Register, Port E */
#define    DDE7     7
#define    DDE6     6
#define    DDE5     5
#define    DDE4     4
#define    DDE3     3
#define    DDE2     2
#define    DDE1     1
#define    DDE0     0

/* PORTE : Data Register, Port E */
#define    PE7      7
#define    PE6      6
#define    PE5      5
#define    PE4      4
#define    PE3      3
#define    PE2      2
#define    PE1      1
#define    PE0      0

/* PORTE : Data Register, Port E */
#define    PORTE7   7
#define    PORTE6   6
#define    PORTE5   5
#define    PORTE4   4
#define    PORTE3   3
#define    PORTE2   2
#define    PORTE1   1
#define    PORTE0   0

/* PINF : Input Pins, Port F */
#define    PINF7    7
#define    PINF6    6
#define    PINF5    5
#define    PINF4    4
#define    PINF3    3
#define    PINF2    2
#define    PINF1    1
#define    PINF0    0

/* TIFR0 : Timer/Counter Interrupt Flag Register 0 */
#define    OCF0B    2
#define    OCF0A    1
#define    TOV0     0

/* TIFR1 : Timer/Counter Interrupt Flag Register 1 */
#define    ICF1      5
#define    OCF1B     2
#define    OCF1A     1
#define    TOV1      0

/* GPIOR1  */
#define    GPIOR17   7
#define    GPIOR16   6
#define    GPIOR15   5
#define    GPIOR14   4
#define    GPIOR13   3
#define    GPIOR12   2
#define    GPIOR11   1
#define    GPIOR10   0

/* GPIOR2  */
#define    GPIOR27   7
#define    GPIOR26   6
#define    GPIOR25   5
#define    GPIOR24   4
#define    GPIOR23   3
#define    GPIOR22   2
#define    GPIOR21   1
#define    GPIOR20   0

/* GPIOR3  */
#define    GPIOR37   7
#define    GPIOR36   6
#define    GPIOR35   5
#define    GPIOR34   4
#define    GPIOR33   3
#define    GPIOR32   2
#define    GPIOR31   1
#define    GPIOR30   0

/* EIFR : External Interrupt Flag Register */
#define    INTF3     3
#define    INTF2     2
#define    INTF1     1
#define    INTF0     0

/* EIMSK : External Interrupt Mask Register */
#define    INT3      3
#define    INT2      2
#define    INT1      1
#define    INT0      0

/* GPIOR0  */
#define    GPIOR07   7
#define    GPIOR06   6
#define    GPIOR05   5
#define    GPIOR04   4
#define    GPIOR03   3
#define    GPIOR02   2
#define    GPIOR01   1
#define    GPIOR00   0

/* EECR : EEPROM Control Register */
#define    EERIE     3
#define    EEMWE     2
#define    EEWE      1
#define    EERE      0

/* GTCCR : General Timer Control Register */
#define    TSM       7
#define    ICPSEL1   6
#define    PSRSYNC   0

/* TCCR0A : Timer/Counter 0 Control Register A */
#define    COM0A1    7
#define    COM0A0    6
#define    COM0B1    5
#define    COM0B0    4

#define    WGM01     1
#define    WGM00     0

/* TCCR0B : Timer/Counter 0 Control Register B */
#define    FOC0A     7
#define    FOC0B     6
#define    WGM02     3
#define    CS02      2
#define    CS01      1
#define    CS00      0

/* PLLCSR */
#define    PLLF      2
#define    PLLE      1
#define    PLLOCK    0
#define    PLOCK     0

/* SPCR : SPI Control Register */
#define    SPIE      7
#define    SPE       6
#define    DORD      5
#define    MSTR      4
#define    CPOL      3
#define    CPHA      2
#define    SPR1      1
#define    SPR0      0

/* SPSR : SPI Status Register */
#define    SPIF      7
#define    WCOL      6
#define    SPI2X     0

/* ACSR : Analog Comparator Control and Status Register */
#define    ACCKDIV   7
#define    AC2IF     6
#define    AC1IF     5
#define    AC0IF     4
#define    AC2O      2
#define    AC1O      1
#define    AC0O      0

/* SMCR : Sleep Mode Control Register */
#define    SM2       3
#define    SM1       2
#define    SM0       1
#define    SE        0

/* MCUSR : MCU general Status Register */
#define    WDRF      3
#define    BORF      2
#define    EXTRF     1
#define    PORF      0

/* MCUCR : MCU general Control Register */
#define    SPIPS     7
#define    PUD       4
#define    IVSEL     1
#define    IVCE      0

/* SPMCR : Store Program Memory Control and Status Register */
#define    SPMIE     7
#define    RWWSB     6
#define    RWWSRE    4
#define    BLBSET    3
#define    PGWRT     2
#define    PGERS     1
#define    SPMEN     0

/* SPH : Stack Pointer High */
#define    SP15      7
#define    SP14      6
#define    SP13      5
#define    SP12      4
#define    SP11      3
#define    SP10      2
#define    SP9       1
#define    SP8       0

/* SPL : Stack Pointer Low */
#define    SP7       7
#define    SP6       6
#define    SP5       5
#define    SP4       4
#define    SP3       3
#define    SP2       2
#define    SP1       1
#define    SP0       0

/* WTDCSR : Watchdog Timer Control Register */
#define    WDIF      7
#define    WDIE      6
#define    WDP3      5
#define    WDCE      4
#define    WDE       3
#define    WDP2      2
#define    WDP1      1
#define    WDP0      0

/* CLKPR : Source Clock Prescaler Register */
#define    CKLPCE    7
#define    CKLPS3    3
#define    CKLPS2    2
#define    CKLPS1    1
#define    CKLPS0    0

/* PRR  */
#define    PRPSC2    7
#define    PRPSC1    6
#define    PRPSC0    5
#define    PRTIM1    4
#define    PRTIM0    3
#define    PRSPI     2
#define    PRUSART   1
#define    PRADC     0

/* OSCAL  */
#define    CAL6      6
#define    CAL5      5
#define    CAL4      4
#define    CAL3      3
#define    CAL2      2
#define    CAL1      1
#define    CAL0      0

/* EICRA  */
#define    ISC31     7
#define    ISC30     6
#define    ISC21     5
#define    ISC20     4
#define    ISC11     3
#define    ISC10     2
#define    ISC01     1
#define    ISC00     0

/* TIMSK0 : Timer Interrupt Mask Register0 */
#define    OCIE0B    2
#define    OCIE0A    1
#define    TOIE0     0

/* TIMSK1 : Timer Interrupt Mask Register1 */
#define    ICIE1     5
#define    OCIE1B    2
#define    OCIE1A    1
#define    TOIE1     0

/* AMP0CSR  */
#define    AMP0EN    7
#define    AMP0IS    6
#define    AMP0G1    5
#define    AMP0G0    4
#define    AMP0TS2   2
#define    AMP0TS1   1
#define    AMP0TS0   0

/* AMP1CSR  */
#define    AMP1EN    7
#define    AMP1IS    6
#define    AMP1G1    5
#define    AMP1G0    4
#define    AMP1TS2   2
#define    AMP1TS1   1
#define    AMP1TS0   0

/* ADCSRA : ADC Control and Status Register A*/
#define    ADEN      7
#define    ADSC      6
#define    ADATE     5
#define    ADIF      4
#define    ADIE      3
#define    ADPS2     2
#define    ADPS1     1
#define    ADPS0     0

/* ADCSRB : ADC Control and Status Register B*/
#define    ADHSM     7
#define    ADASCR    4
#define    ADST3     3
#define    ADST2     2
#define    ADST1     1
#define    ADST0     0

/* ADMUX : ADC Multiplexer Selection Register */
#define    REFS1     7
#define    REFS0     6
#define    ADLAR     5
#define    MUX3      3
#define    MUX2      2
#define    MUX1      1
#define    MUX0      0

/* DIDR0 */
#define    ADC7D     7
#define    ADC6D     6
#define    ADC5D     5
#define    ADC4D     4
#define    ADC3D     3
#define    ADC2D     2
#define    ADC1D     1
#define    ADC0D     0

/* DIDR1 */
#define    ACMP0D    5
#define    AMP0PD    4
#define    AMP0ND    3
#define    ADC10D    2
#define    ACMP1D    2
#define    ADC9D     1
#define    AMP1PD    1
#define    ADC8D     0
#define    AMP1ND    0

/* TCCR1A : Timer/Counter 1 Control Register A */
#define    COM1A1    7
#define    COM1A0    6
#define    COM1B1    5
#define    COM1B0    4
#define    WGM11     1
#define    WGM10     0

/* TCCR1B : Timer/Counter 1 Control Register B */
#define    ICNC1     7
#define    ICES1     6
#define    WGM13     4
#define    WGM12     3
#define    CS12      2
#define    CS11      1
#define    CS10      0

/* TCCR1C : Timer/Counter 1 Control Register C */
#define    FOC1A     7
#define    FOC1B     6

/* PIFR0 : PSC 0 Interrupt Flag Register */
#define    PSEI0     5
#define    PEV0B     4
#define    PEV0A     3
#define    PRN01     2
#define    PRN00     1
#define    PEOP0     0

/* PIM0 : PSC 0 Interrupt Mask Register */
#define    PSEIE0    5
#define    PEVE0B    4
#define    PEVE0A    3
#define    PEOPE0    0

/* PIFR1 : PSC 1 Interrupt Flag Register */
#define    PSEI1     5
#define    PEV1B     4
#define    PEV1A     3
#define    PRN11     2
#define    PRN10     1
#define    PEOP1     0

/* PIM1 : PSC 1 Interrupt Mask Register */
#define    PSEIE1    5
#define    PEVE1B    4
#define    PEVE1A    3
#define    PEOPE1    0

/* PIFR2 : PSC 2 Interrupt Flag Register */
#define    PSEI2     5
#define    PEV2B     4
#define    PEV2A     3
#define    PRN21     2
#define    PRN20     1
#define    PEOP2     0

/* PIM2 : PSC 2 Interrupt Mask Register */
#define    PSEIE1    5
#define    PEVE2B    4
#define    PEVE2A    3
#define    PEOPE2    0

/* DACON: DAC Control and Status Register */
#define    DAATE     7
#define    DATS2     6
#define    DATS1     5
#define    DATS0     4
#define    DALA      2
#define    DAOE      1
#define    DAEN      0

/* AC0CON : Analog comparator 0 Control and Status Register */
#define    AC0EN     7
#define    AC0IE     6
#define    AC0IS1    5
#define    AC0IS0    4
#define    AC0M2     2
#define    AC0M1     1
#define    AC0M0     0

/* AC1CON : Analog comparator 1 Control and Status Register */
#define    AC1EN     7
#define    AC1IE     6
#define    AC1IS1    5
#define    AC1IS0    4
#define    AC1ICE    3
#define    AC1M2     2
#define    AC1M1     1
#define    AC1M0     0

/* AC2CON : Analog comparator 2 Control and Status Register */
#define    AC2EN     7
#define    AC2IE     6
#define    AC2IS1    5
#define    AC2IS0    4
#define    AC2SADE   3
#define    AC2M2     2
#define    AC2M1     1
#define    AC2M0     0

/* UCSRA : USART Control and Status Register A */
#define    RXC       7
#define    TXC       6
#define    UDRE      5
#define    FE        4
#define    DOR       3
#define    UPE       2
#define    U2X       1
#define    MPCM      0

/* UCSRB : USART Control and Status Register B */
#define    RXCIE     7
#define    TXCIE     6
#define    UDRIE     5
#define    RXEN      4
#define    TXEN      3
#define    UCSZ2     2
#define    RXB8      1
#define    TXB8      0

/* UCSRC : USART Control and Status Register C */
#define    UMSEL     6
#define    UPM1      5
#define    UPM0      4
#define    USBS      3
#define    UCSZ1     2
#define    UCSZ0     1
#define    UCPOL     0

/* USART0 left for compatiblity reason */
/* UCSR0A : USART0 Control and Status Register A */
#define    RXC0      7
#define    TXC0      6
#define    UDRE0     5
#define    FE0       4
#define    DOR0      3
#define    UPE0      2
#define    U2X0      1
#define    MPCM0     0

/* UCSR0B : USART0 Control and Status Register B */
#define    RXCIE0    7
#define    TXCIE0    6
#define    UDRIE0    5
#define    RXEN0     4
#define    TXEN0     3
#define    UCSZ02    2
#define    RXB80     1
#define    TXB80     0

/* UCSR0C : USART0 Control and Status Register C */
#define    UMSEL0    6
#define    UPM01     5
#define    UPM00     4
#define    USBS0     3
#define    UCSZ01    2
#define    UCSZ00    1
#define    UCPOL0    0

/* EUSART */
/* EUCSRA : EUSART Control and Status Register A */
#define    UTxS3     7
#define    UTxS2     6
#define    UTxS1     5
#define    UTxS0     4
#define    URxS3     3
#define    URxS2     2
#define    URxS1     1
#define    URxS0     0

/* EUCSRB : EUSART Control and Status Register B */
#define    EUSART    4
#define    EUSBS     3
#define    EMCH      1
#define    BODR      0

/* EUCSRC : EUSART Control and Status Register C */
#define    FEM       3
#define    F1617     2
#define    STP1      1
#define    STP0      0

/* PSOC0 : PSC 0 Synchro and Output Configuration */
#define    PSYNC01   5
#define    PSYNC00   4
#define    POEN0B    2
#define    POEN0A    0

/* PCNF0 : PSC 0 Configuration */
#define    PFIFTY0   7
#define    PALOCK0   6
#define    PLOCK0    5
#define    PMODE01   4
#define    PMODE00   3
#define    POP0      2
#define    PCLKSEL0  1

/* PCTL0 : PSC 0 Control */
#define    PPRE01    7
#define    PPRE00    6
#define    PBFM0     5
#define    PAOC0B    4
#define    PAOC0A    3
#define    PARUN0    2
#define    PCCYC0    1
#define    PRUN0     0

/* PFRC0A : PSC 0 Input A Control Register */
#define    PCAPE0A   7
#define    PISEL0A   6
#define    PELEV0A   5
#define    PFLTE0A   4
#define    PRFM0A3   3
#define    PRFM0A2   2
#define    PRFM0A1   1
#define    PRFM0A0   0

/* PFRC0B : PSC 0 Input B Control Register */
#define    PCAPE0B   7
#define    PISEL0B   6
#define    PELEV0B   5
#define    PFLTE0B   4
#define    PRFM0B3   3
#define    PRFM0B2   2
#define    PRFM0B1   1
#define    PRFM0B0   0

/* PSOC1 : PSC 1 Synchro and Output Configuration */
#define    PSYNC11   5
#define    PSYNC10   4
#define    POEN1B    2
#define    POEN1A    0

/* PCNF1 : PSC 1 Configuration */
#define    PFIFTY1   7
#define    PALOCK1   6
#define    PLOCK1    5
#define    PMODE11   4
#define    PMODE10   3
#define    POP1      2
#define    PCLKSEL1  1

/* PCTL1 : PSC 1 Control */
#define    PPRE11    7
#define    PPRE10    6
#define    PBFM1     5
#define    PAOC1B    4
#define    PAOC1A    3
#define    PARUN1    2
#define    PCCYC1    1
#define    PRUN1     0

/* PFRC1A : PSC 1 Input A Control Register */
#define    PCAPE1A   7
#define    PISEL1A   6
#define    PELEV1A   5
#define    PFLTE1A   4
#define    PRFM1A3   3
#define    PRFM1A2   2
#define    PRFM1A1   1
#define    PRFM1A0   0

/* PFRC1B : PSC 1 Input B Control Register */
#define    PCAPE1B   7
#define    PISEL1B   6
#define    PELEV1B   5
#define    PFLTE1B   4
#define    PRFM1B3   3
#define    PRFM1B2   2
#define    PRFM1B1   1
#define    PRFM1B0   0

/* PSOC2 : PSC 2 Synchro and Output Configuration */
#define    POS23     7
#define    POS22     6
#define    PSYNC21   5
#define    PSYNC20   4
#define    POEN2D    3
#define    POEN2B    2
#define    POEN2C    1
#define    POEN2A    0

/* POM2 : PSC 2 Output Matrix */
#define    POMV2B3   7
#define    POMV2B2   6
#define    POMV2B1   5
#define    POMV2B0   4
#define    POMV2A3   3
#define    POMV2A2   2
#define    POMV2A1   1
#define    POMV2A0   0

/* PCNF2 : PSC 2 Configuration */
#define    PFIFTY2   7
#define    PALOCK2   6
#define    PLOCK2    5
#define    PMODE21   4
#define    PMODE20   3
#define    POP2      2
#define    PCLKSEL2  1
#define    POME2     0

/* PCTL2 : PSC 2 Control */
#define    PPRE21    7
#define    PPRE20    6
#define    PBFM2     5
#define    PAOC2B    4
#define    PAOC2A    3
#define    PARUN2    2
#define    PCCYC2    1
#define    PRUN2     0

/* PFRC2A : PSC 2 Input A Control Register */
#define    PCAPE2A   7
#define    PISEL2A   6
#define    PELEV2A   5
#define    PFLTE2A   4
#define    PRFM2A3   3
#define    PRFM2A2   2
#define    PRFM2A1   1
#define    PRFM2A0   0

/* PFRC2B : PSC 2 Input B Control Register */
#define    PCAPE2B   7
#define    PISEL2B   6
#define    PELEV2B   5
#define    PFLTE2B   4
#define    PRFM2B3   3
#define    PRFM2B2   2
#define    PRFM2B1   1
#define    PRFM2B0   0

/* Pointer definition */
#define    XL     r26
#define    XH     r27
#define    YL     r28
#define    YH     r29
#define    ZL     r30
#define    ZH     r31

#endif  /* _MCU_H*/
