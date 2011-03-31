/*H**************************************************************************
* NAME:         mcu.h
*----------------------------------------------------------------------------
* Copyright (c) 2006 Atmel.
*----------------------------------------------------------------------------
* RELEASE:      at90usb128-demo-audio-1_0_2
* REVISION:     1.6.6.9.2.21
*----------------------------------------------------------------------------
* PURPOSE:
* SFR Description file for AtmegaUSB.
*****************************************************************************/
#ifndef MCU_H
#define MCU_H

/*==========================*/
/* Predefined SFR Addresses */
/*==========================*/

/******************************************************************************/
#if defined(__ICCAVR__) || defined(__IAR_SYSTEMS_ASM__)
/******************************************************************************/
SFR_B(PINA,   0x00) /* Input Pins, Port A */
SFR_B(DDRA,   0x01) /* Data Direction Register, Port A */
SFR_B(PORTA,  0x02) /* Data Register, Port A */

SFR_B(PINB,   0x03) /* Input Pins, Port B */
SFR_B(DDRB,   0x04) /* Data Direction Register, Port B */
SFR_B(PORTB,  0x05) /* Data Register, Port B */

SFR_B(PINC,   0x06) /* Input Pins, Port C */
SFR_B(DDRC,   0x07) /* Data Direction Register, Port C */
SFR_B(PORTC,  0x08) /* Data Register, Port C */

SFR_B(PIND,   0x09) /* Input Pins, Port D */
SFR_B(DDRD,   0x0A) /* Data Direction Register, Port D */
SFR_B(PORTD,  0x0B) /* Data Register, Port D */

SFR_B(PINE,   0x0C) /* Input Pins, Port E */
SFR_B(DDRE,   0x0D) /* Data Direction Register, Port E */
SFR_B(PORTE,  0x0E) /* Data Register, Port E */

SFR_B(PINF,   0x0F) /* Input Pins, Port F */
SFR_B(DDRF,   0x10) /* Data Direction Register, Port F */
SFR_B(PORTF,  0x11) /* Data Register, Port F */

SFR_B(PING,   0x12) /* Input Pins, Port G */
SFR_B(DDRG,   0x13) /* Data Direction Register, Port G */
SFR_B(PORTG,  0x14) /* Data Register, Port G */

SFR_B(TIFR0,   0x15) /* Timer/Counter Interrupt Flag register 0*/
SFR_B(TIFR1,   0x16) /* Timer/Counter Interrupt Flag register 1*/
SFR_B(TIFR2,   0x17) /* Timer/Counter Interrupt Flag register 2*/
SFR_B(TIFR3,   0x18) /* Timer/Counter Interrupt Flag register 3*/

SFR_B(EIFR,   0x1C)     /* External Interrupt Flag Register */
SFR_B(EIMSK,  0x1D)     /* External Interrupt Mask Register */

SFR_B(GPIOR0,  0x1E)     /* General Purpose Register 0 */

SFR_B(EECR,   0x1F)     /* EEPROM Control Register */
SFR_B(EEDR,   0x20)     /* EEPROM Data Register */
SFR_W(EEAR,   0x21)     /* EEPROM Address Register */

SFR_B(GTCCR,  0x23)     /* General Purpose Register */

SFR_B(TCCR0A,  0x24)     /* Timer/Counter 0 Control Register */
SFR_B(TCNT0,   0x26)     /* Timer/Counter 0 */
SFR_B(OCR0A,   0x27)     /* Timer/Counter 0 Output Compare Register */

SFR_B(TCCR0B,  0x25)     /* Timer/Counter 0 Control Register B*/
SFR_B(OCR0B,   0x28)

SFR_B(GPIOR1,  0x2A)     /* General Purpose Register 1 */
SFR_B(GPIOR2,  0x2B)     /* General Purpose Register 2 */

SFR_B(SPCR,   0x2C)     /* SPI Control Register */
SFR_B(SPSR,   0x2D)     /* SPI Status Register */
SFR_B(SPDR,   0x2E)     /* SPI I/O Data Register */

SFR_B(ACSR,   0x30)     /* Analog Comparator Control and Status Register */

SFR_B(OCDR,   0x31)     /* On-Chip Debug Register */

SFR_B(SMCR,   0x33)    /* Sleep Mode Control Register */
SFR_B(MCUSR,  0x34)    /* MCU Status Register */
SFR_B(MCUCR,  0x35)    /* MCU Control Register */

SFR_B(SPMCSR, 0x37)     /* Store Program Memory Control and Status Register */

SFR_B(RAMPZ,  0x3B)     /* RAM Page Z Select Register */

SFR_W(SP,     0x3D)     /* Stack Pointer */
SFR_B(SREG,   0x3F)     /* Status Register */

SFR_B(PLLCSR,    0x29); /* PLL Control and Status Register*/

SFR_B_EXT(WDTCR,  0x60)     /* Watchdog Timer Control Register for compatibility*/
SFR_B_EXT(WDTCSR,  0x60)    /* Watchdog Timer Control Register */
SFR_B_EXT(CLKPR,  0x61)     /* Clock Prescale Register */
SFR_B_EXT(OSCCAL, 0x66)     /* Oscillator Calibration Register */

SFR_B_EXT(EICRA,  0x69)     /* External Interrupt Control Register A */
SFR_B_EXT(EICRB,  0x6A)     /* External Interrupt Control Register B */

SFR_B_EXT(TIMSK0,  0x6E)     /* Timer/Counter 0 Interrupt Mask Register */
SFR_B_EXT(TIMSK1,  0x6F)     /* Timer/Counter 1 Interrupt Mask Register */
SFR_B_EXT(TIMSK2,  0x70)     /* Timer/Counter 2 Interrupt Mask Register */
SFR_B_EXT(TIMSK3,  0x71)     /* Timer/Counter 3 Interrupt Mask Register */

SFR_B_EXT(XMCRA,  0x74)     /* External Memory Control Register A */
SFR_B_EXT(XMCRB,  0x75)     /* External Memory Control Register B */

SFR_W_EXT(ADC,    0x78)     /* ADC Data register */
SFR_B_EXT(ADCSRA, 0x7A)     /* ADC Control and Status Register A */
SFR_B_EXT(ADCSRB, 0x7B)     /* ADC Control and Status Register B */
SFR_B_EXT(ADMUX,  0x7C)     /* ADC Multiplexer Selection Register */

SFR_B_EXT(DIDR0,  0x7E)     /* Digital Input Disable Register 0 */
SFR_B_EXT(DIDR1,  0x7F)     /* Digital Input Disable Register 1 */

SFR_B_EXT(TCCR1A, 0x80)     /* Timer/Counter 1 Control Register A */
SFR_B_EXT(TCCR1B, 0x81)     /* Timer/Counter 1 Control Register B */
SFR_B_EXT(TCCR1C, 0x82)     /* Timer/Counter 1 Control Register C */
SFR_W_EXT(TCNT1,  0x84)     /* Timer/Counter 1 Register */
SFR_W_EXT(ICR1,   0x86)     /* Timer/Counter 1 Input Capture Register */
SFR_W_EXT(OCR1A,  0x88)     /* Timer/Counter 1 Output Compare Register A */
SFR_W_EXT(OCR1B,  0x8A)     /* Timer/Counter 1 Output Compare Register B */
SFR_W_EXT(OCR1C,  0x8C)     /* Timer/Counter 1 Output Compare Register C */

SFR_B_EXT(TCCR3A, 0x90)     /* Timer/Counter 3 Control Register A */
SFR_B_EXT(TCCR3B, 0x91)     /* Timer/Counter 3 Control Register B */
SFR_B_EXT(TCCR3C, 0x92)     /* Timer/Counter 3 Control Register C */
SFR_W_EXT(TCNT3,  0x94)     /* Timer/Counter 3 Register */
SFR_W_EXT(ICR3,   0x96)     /* Timer/Counter 3 Input Capture Register */
SFR_W_EXT(OCR3A,  0x98)     /* Timer/Counter 3 Output Compare Register A */
SFR_W_EXT(OCR3B,  0x9A)     /* Timer/Counter 3 Output Compare Register B */
SFR_W_EXT(OCR3C,  0x9C)     /* Timer/Counter 3 Output Compare Register C */

SFR_B_EXT(TCCR2A, 0xB0)     /* Timer/Counter 2 Control Register A */
SFR_B_EXT(TCCR2B, 0xB1)     /* Timer/Counter 2 Control Register A */
SFR_B_EXT(TCNT2,  0xB2)     /* Timer/Counter 2 Register */
SFR_B_EXT(OCR2A,  0xB3)     /* Timer/Counter 2 Output Compare Register A */
SFR_B_EXT(OCR2B,  0xB4)     /* Timer/Counter 2 Output Compare Register B */

SFR_B_EXT(ASSR,   0xB6)     /* Asynchronous mode Status Register */

SFR_B_EXT(TWBR,   0xB8)     /* TWI Bit Rate Register */
SFR_B_EXT(TWSR,   0XB9)     /* TWI Status Register */
SFR_B_EXT(TWAR,   0xBA)     /* TWI Address Register */
SFR_B_EXT(TWDR,   0xBB)     /* TWI Data Register */
SFR_B_EXT(TWCR,   0xBC)     /* TWI Control Register */


SFR_B_EXT(UCSR1A, 0xC8)     /* USART1 Control and Status Register A */
SFR_B_EXT(UCSR1B, 0xC9)     /* USART1 Control and Status Register B */
SFR_B_EXT(UCSR1C, 0xCA)     /* USART1 Control and Status Register C */
SFR_W_EXT(UBRR1,  0xCC)     /* USART1 Baud Rate Register Low */
//SFR_B_EXT(UBRR1L, 0xCC)
//SFR_B_EXT(UBRR1H, 0xCD)
SFR_B_EXT(UDR1,   0xCE)     /* USART1 I/O Data Register */

SFR_B_EXT(PCICR, 0x68)      /* Pin Change interrupt enable */
SFR_B_EXT(PCIFR, 0x3B)      /* Pin Change interrupt flag*/
SFR_B_EXT(PCMSK0, 0x6B)     /* Pin Change interrupt mask */



// USB CONTROLLER
//USB Hardware configuration
SFR_B_EXT(UHWCON,    0xD7);

// USB General
// Page 1
SFR_B_EXT(USBCON,    0xD8);
SFR_B_EXT(USBSTA,    0xD9);
SFR_B_EXT(USBINT,    0xDA);
SFR_B_EXT(UDPADDH,   0xDC);
SFR_B_EXT(UDPADDL,   0xDB);
SFR_B_EXT(OTGCON,    0xDD);
SFR_B_EXT(OTGTCON,   0xF9);
SFR_B_EXT(OTGIEN,    0xDE);
SFR_B_EXT(OTGINT,    0xDF);

// USB Device
// Page 1
SFR_B_EXT(UDCON,     0xE0);
SFR_B_EXT(UDINT,     0xE1);
SFR_B_EXT(UDIEN,     0xE2);
SFR_B_EXT(UDADDR,    0xE3);
SFR_B_EXT(UDFNUMH,   0xE5);
SFR_B_EXT(UDFNUML,   0xE4);
SFR_B_EXT(UDMFN,     0xE6);
SFR_B_EXT(UDTST,     0xE7);

// USB Endpoint
// Page 1
SFR_B_EXT(UENUM,     0xE9);
SFR_B_EXT(UERST,     0xEA);
SFR_B_EXT(UECONX,    0xEB);
SFR_B_EXT(UECFG0X,   0xEC);
SFR_B_EXT(UECFG1X,   0xED);
SFR_B_EXT(UESTA0X,   0xEE);
SFR_B_EXT(UESTA1X,   0xEF);
SFR_B_EXT(UEINTX,    0xE8);
SFR_B_EXT(UEIENX,    0xF0);
SFR_B_EXT(UEDATX,    0xF1);
SFR_B_EXT(UEBCHX,    0xF3);
SFR_B_EXT(UEBCLX,    0xF2);
SFR_B_EXT(UEINT,     0xF4);

// USB Host
// Page 1
SFR_B_EXT(UHCON,     0x9E);
SFR_B_EXT(UHINT,     0x9F);
SFR_B_EXT(UHIEN,     0xA0);
SFR_B_EXT(UHADDR,    0xA1);
SFR_B_EXT(UHFNUMH,   0xA3);
SFR_B_EXT(UHFNUML,   0xA2);
SFR_B_EXT(UHFLEN,    0xA4);

// USB Pipe
// Page 1
SFR_B_EXT(UPNUM,     0xA7);
SFR_B_EXT(UPRST,     0xA8);
SFR_B_EXT(UPCONX,    0xA9);
SFR_B_EXT(UPCFG0X,   0xAA);
SFR_B_EXT(UPCFG1X,   0xAB);
SFR_B_EXT(UPCFG2X,   0xAD);
SFR_B_EXT(UPSTAX,    0xAC);
SFR_B_EXT(UPINRQX,   0xA5);
SFR_B_EXT(UPERRX,    0xF5);
SFR_B_EXT(UPINTX,    0xA6);
SFR_B_EXT(UPIENX,    0xAE);
SFR_B_EXT(UPDATX,    0xAF);
SFR_B_EXT(UPBCHX,    0xF7);
SFR_B_EXT(UPBCLX,    0xF6);
SFR_B_EXT(UPINT,     0xF8);



/*==============================*/
/* Interrupt Vector Definitions */
/*==============================*/
/* NB! vectors are specified as byte addresses */
#define    RESET_vect                  (0x00)
#define    INT0_vect                   (0x04)
#define    INT1_vect                   (0x08)
#define    INT2_vect                   (0x0C)
#define    INT3_vect                   (0x10)
#define    INT4_vect                   (0x14)
#define    INT5_vect                   (0x18)
#define    INT6_vect                   (0x1C)
#define    INT7_vect                   (0x20)
#define    PCINT0_vect		            (0x24)
#define    USB_GENERAL_vect            (0x28)
#define    USB_ENDPOINT_PIPE_vect      (0x2C)
#define    WDT_vect			            (0x30)
#define    TIMER2_COMPA_vect           (0x34)
#define    TIMER2_COMPB_vect           (0x38)
#define    TIMER2_OVF_vect             (0x3C)
#define    TIMER1_CAPT_vect            (0x40)
#define    TIMER1_COMPA_vect           (0x44)
#define    TIMER1_COMPB_vect           (0x48)
#define    TIMER1_COMPC_vect           (0x4C)
#define    TIMER1_OVF_vect             (0x50)
#define    TIMER0_COMPA_vect           (0x54)
#define    TIMER0_COMPB_vect           (0x58)
#define    TIMER0_OVF_vect             (0x5C)
#define    SPI_STC_vect                (0x60)
#define    USART1_RXC_vect             (0x64)
#define    USART1_UDRE_vect            (0x68)
#define    USART1_TXC_vect             (0x6C)
#define    ANA_COMP_vect               (0x70)
#define    ADC_vect                    (0x74)
#define    EE_RDY_vect                 (0x78)
#define    TIMER3_CAPT_vect            (0x7C)
#define    TIMER3_COMPA_vect           (0x80)
#define    TIMER3_COMPB_vect           (0x84)
#define    TIMER3_COMPC_vect           (0x88)
#define    TIMER3_OVF_vect             (0x8C)
#define    TWI_vect                    (0x90)
#define    SPM_RDY_vect                (0x94)

#endif /* _IAR_ */
/******************************************************************************/
#ifdef __CODEVISIONAVR__
/******************************************************************************/
#define PINA    (*(volatile unsigned char *)0x20)  /* Input Pins, Port A */
#define DDRA    (*(volatile unsigned char *)0x21) /* Data Direction Register, Port A */
#define PORTA   (*(volatile unsigned char *)0x22) /* Data Register, Port A */

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

#define PINF    (*(volatile unsigned char *)0x2F) /* Input Pins, Port F */
#define DDRF    (*(volatile unsigned char *)0x30) /* Data Direction Register, Port F */
#define PORTF   (*(volatile unsigned char *)0x31) /* Data Register, Port F */

#define PING    (*(volatile unsigned char *)0x32) /* Input Pins, Port G */
#define DDRG    (*(volatile unsigned char *)0x33) /* Data Direction Register, Port G */
#define PORTG   (*(volatile unsigned char *)0x34) /* Data Register, Port G */

#define TIFR0   (*(volatile unsigned char *)0x35) /* Timer/Counter Interrupt Flag register 0*/
#define TIFR1   (*(volatile unsigned char *)0x36) /* Timer/Counter Interrupt Flag register 1*/
#define TIFR2   (*(volatile unsigned char *)0x37) /* Timer/Counter Interrupt Flag register 2*/
#define TIFR3   (*(volatile unsigned char *)0x38) /* Timer/Counter Interrupt Flag register 3*/

#define EIFR    (*(volatile unsigned char *)0x3C)     /* External Interrupt Flag Register */
#define EIMSK   (*(volatile unsigned char *)0x3D)     /* External Interrupt Mask Register */

#define GPIOR0  (*(volatile unsigned char *)0x3E)     /* General Purpose Register 0 */

#define EECR    (*(volatile unsigned char *)0x3F)     /* EEPROM Control Register */
#define EEDR    (*(volatile unsigned char *)0x40)     /* EEPROM Data Register */
#define EEAR    (*(volatile unsigned int *)0x41)     /* EEPROM Address Register */

#define GTCCR   (*(volatile unsigned char *)0x43)     /* General Purpose Register */

#define TCCR0A  (*(volatile unsigned char *)0x44)     /* Timer/Counter 0 Control Register */
#define TCNT0   (*(volatile unsigned char *)0x46)     /* Timer/Counter 0 */
#define OCR0A   (*(volatile unsigned char *)0x47)     /* Timer/Counter 0 Output Compare Register */

#define GPIOR1  (*(volatile unsigned char *)0x4A)     /* General Purpose Register 1 */
#define GPIOR2  (*(volatile unsigned char *)0x4B)     /* General Purpose Register 2 */

#define SPCR    (*(volatile unsigned char *)0x4C)     /* SPI Control Register */
#define SPSR    (*(volatile unsigned char *)0x4D)     /* SPI Status Register */
#define SPDR    (*(volatile unsigned char *)0x4E)     /* SPI I/O Data Register */

#define ACSR    (*(volatile unsigned char *)0x50)     /* Analog Comparator Control and Status Register */

#define OCDR    (*(volatile unsigned char *)0x51)     /* On-Chip Debug Register */

#define SMCR    (*(volatile unsigned char *)0x53)    /* Sleep Mode Control Register */
#define MCUSR   (*(volatile unsigned char *)0x53)    /* MCU Status Register */
#define MCUCR   (*(volatile unsigned char *)0x53)    /* MCU Control Register */

#define SPMCSR  (*(volatile unsigned char *)0x57)     /* Store Program Memory Control and Status Register */

#define RAMPZ   (*(volatile unsigned char *)0x5B)     /* RAM Page Z Select Register */

#define SP      (*(volatile unsigned int *)0x5D)     /* Stack Pointer */
#define SREG    (*(volatile unsigned char *)0x5F)     /* Status Register */

#define WDTCR   (*(volatile unsigned char *)0x60)     /* Watchdog Timer Control Register */
#define CLKPR   (*(volatile unsigned char *)0x61)     /* Clock Prescale Register */
#define OSCCAL  (*(volatile unsigned char *)0x66)     /* Oscillator Calibration Register */

#define EICRA   (*(volatile unsigned char *)0x69)     /* External Interrupt Control Register A */
#define EICRB   (*(volatile unsigned char *)0x6A)     /* External Interrupt Control Register B */

#define TIMSK0  (*(volatile unsigned char *)0x6E)     /* Timer/Counter 0 Interrupt Mask Register */
#define TIMSK1  (*(volatile unsigned char *)0x6F)     /* Timer/Counter 1 Interrupt Mask Register */
#define TIMSK2  (*(volatile unsigned char *)0x70)     /* Timer/Counter 2 Interrupt Mask Register */
#define TIMSK3  (*(volatile unsigned char *)0x71)     /* Timer/Counter 3 Interrupt Mask Register */

#define XMCRA   (*(volatile unsigned char *)0x74)     /* External Memory Control Register A */
#define XMCRB   (*(volatile unsigned char *)0x75)     /* External Memory Control Register B */

#define ADC     (*(volatile unsigned int *)0x78)     /* ADC Data register */
#define ADCSRA  (*(volatile unsigned char *)0x7A)     /* ADC Control and Status Register A */
#define ADCSRB  (*(volatile unsigned char *)0x7B)     /* ADC Control and Status Register B */
#define ADMUX   (*(volatile unsigned char *)0x7C)     /* ADC Multiplexer Selection Register */

#define DIDR0   (*(volatile unsigned char *)0x7E)     /* Digital Input Disable Register 0 */
#define DIDR1   (*(volatile unsigned char *)0x7F)     /* Digital Input Disable Register 1 */

#define TCCR1A  (*(volatile unsigned char *)0x80)     /* Timer/Counter 1 Control Register A */
#define TCCR1B  (*(volatile unsigned char *)0x81)     /* Timer/Counter 1 Control Register B */
#define TCCR1C  (*(volatile unsigned char *)0x82)     /* Timer/Counter 1 Control Register C */
#define TCNT1   (*(volatile unsigned int *)0x84)     /* Timer/Counter 1 Register */
#define ICR1    (*(volatile unsigned int *)0x86)     /* Timer/Counter 1 Input Capture Register */
#define OCR1A   (*(volatile unsigned int *)0x88)     /* Timer/Counter 1 Output Compare Register A */
#define OCR1B   (*(volatile unsigned int *)0x8A)     /* Timer/Counter 1 Output Compare Register B */
#define OCR1C   (*(volatile unsigned int *)0x8C)     /* Timer/Counter 1 Output Compare Register C */

#define TCCR3A  (*(volatile unsigned char *)0x90)     /* Timer/Counter 3 Control Register A */
#define TCCR3B  (*(volatile unsigned char *)0x91)     /* Timer/Counter 3 Control Register B */
#define TCCR3C  (*(volatile unsigned char *)0x92)     /* Timer/Counter 3 Control Register C */
#define TCNT3   (*(volatile unsigned int *)0x94)     /* Timer/Counter 3 Register */
#define ICR3    (*(volatile unsigned int *)0x96)     /* Timer/Counter 3 Input Capture Register */
#define OCR3A   (*(volatile unsigned int *)0x98)     /* Timer/Counter 3 Output Compare Register A */
#define OCR3B   (*(volatile unsigned int *)0x9A)     /* Timer/Counter 3 Output Compare Register B */
#define OCR3C   (*(volatile unsigned int *)0x9C)     /* Timer/Counter 3 Output Compare Register C */

#define TCCR2A  (*(volatile unsigned char *)0xB0)     /* Timer/Counter 2 Control Register A */
#define TCNT2   (*(volatile unsigned char *)0xB2)     /* Timer/Counter 2 Register */
#define OCR2A   (*(volatile unsigned char *)0xB3)     /* Timer/Counter 2 Output Compare Register A */

#define ASSR   (*(volatile unsigned char *)0xB6)     /* Asynchronous mode Status Register */

#define TWBR   (*(volatile unsigned char *)0xB8)     /* TWI Bit Rate Register */
#define TWSR   (*(volatile unsigned char *)0XB9)     /* TWI Status Register */
#define TWAR   (*(volatile unsigned char *)0xBA)     /* TWI Address Register */
#define TWDR   (*(volatile unsigned char *)0xBB)     /* TWI Data Register */
#define TWCR   (*(volatile unsigned char *)0xBC)     /* TWI Control Register */

#define UCSR0A  (*(volatile unsigned char *)0xC0)     /* USART0 Control and Status Register A */
#define UCSR0B  (*(volatile unsigned char *)0xC1)     /* USART0 Control and Status Register B */
#define UCSR0C  (*(volatile unsigned char *)0xC2)     /* USART0 Control and Status Register C */
#define UBRR0   (*(volatile unsigned int *)0xC4)     /* USART0 Baud Rate Register  */
#define UBRR0L  (*(volatile unsigned char *)0xC4)     /* USART0 Baud Rate Register Low */
#define UBRR0H  (*(volatile unsigned char *)0xC5)     /* USART0 Baud Rate Register High */
#define UDR0    (*(volatile unsigned char *)0xC6)     /* USART0 I/O Data Register */

#define UCSR1A  (*(volatile unsigned char *)0xC8)     /* USART1 Control and Status Register A */
#define UCSR1B  (*(volatile unsigned char *)0xC9)     /* USART1 Control and Status Register B */
#define UCSR1C  (*(volatile unsigned char *)0xCA)     /* USART1 Control and Status Register C */
#define UBRR1   (*(volatile unsigned int *)0xCC)     /* USART1 Baud Rate Register  */
#define UBRR1L  (*(volatile unsigned char *)0xCC)     /* USART1 Baud Rate Register Low */
#define UBRR1H  (*(volatile unsigned char *)0xCD)     /* USART1 Baud Rate Register High */
#define UDR1    (*(volatile unsigned char *)0xCE)     /* USART1 I/O Data Register */



/*==============================*/
/* Interrupt Vector Definitions */
/*==============================*/
/* NB! vectors are specified as byte addresses */
#define    RESET_vect           1
#define    INT0_vect            2
#define    INT1_vect            3
#define    INT2_vect            4
#define    INT3_vect            5
#define    INT4_vect            6
#define    INT5_vect            7
#define    INT6_vect            8
#define    INT7_vect            9
#define    TIMER2_COMP_vect     10
#define    TIMER2_OVF_vect      11
#define    TIMER1_CAPT_vect     12
#define    TIMER1_COMPA_vect    13
#define    TIMER1_COMPB_vect    14
#define    TIMER1_COMPC_vect    15
#define    TIMER1_OVF_vect      16
#define    TIMER0_COMP_vect     17
#define    TIMER0_OVF_vect      18
#define    CANIT_vect		19	 					
#define    CANTOVF_vect		20 		
#define    SPI_STC_vect         21
#define    USART0_RXC_vect      22
#define    USART0_UDRE_vect     23
#define    USART0_TXC_vect      24
#define    ANA_COMP_vect        25
#define    ADC_vect             26
#define    EE_RDY_vect          27
#define    TIMER3_CAPT_vect     28
#define    TIMER3_COMPA_vect    29
#define    TIMER3_COMPB_vect    30
#define    TIMER3_COMPC_vect    31
#define    TIMER3_OVF_vect      32
#define    USART1_RXC_vect      33
#define    USART1_UDRE_vect     34
#define    USART1_TXC_vect      35
#define    TWI_vect             36
#define    SPM_RDY_vect         37

#endif /* _ICC_*/



/*==========================*/
/* Bit Position Definitions */
/*==========================*/
/* PINA : Input Pins, Port A */
#define    PINA7    7
#define    PINA6    6
#define    PINA5    5
#define    PINA4    4
#define    PINA3    3
#define    PINA2    2
#define    PINA1    1
#define    PINA0    0

/* DDRA : Data Direction Register, Port A */
#define    DDA7     7
#define    DDA6     6
#define    DDA5     5
#define    DDA4     4
#define    DDA3     3
#define    DDA2     2
#define    DDA1     1
#define    DDA0     0

/* PORTA : Data Register, Port A */
#define    PORTA7   7
#define    PORTA6   6
#define    PORTA5   5
#define    PORTA4   4
#define    PORTA3   3
#define    PORTA2   2
#define    PORTA1   1
#define    PORTA0   0

/* PORTA : Data Register, Port A */
#define    PA7      7
#define    PA6      6
#define    PA5      5
#define    PA4      4
#define    PA3      3
#define    PA2      2
#define    PA1      1
#define    PA0      0

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

/* DDRF : Data Direction Register, Port F */
#define    DDF7     7
#define    DDF6     6
#define    DDF5     5
#define    DDF4     4
#define    DDF3     3
#define    DDF2     2
#define    DDF1     1
#define    DDF0     0

/* PORTF : Data Register, Port F */
#define    PF7      7
#define    PF6      6
#define    PF5      5
#define    PF4      4
#define    PF3      3
#define    PF2      2
#define    PF1      1
#define    PF0      0

/* PORTF : Data Register, Port F */
#define    PORTF7   7
#define    PORTF6   6
#define    PORTF5   5
#define    PORTF4   4
#define    PORTF3   3
#define    PORTF2   2
#define    PORTF1   1
#define    PORTF0   0

/* Input Pins, Port G */
#define    PING4    4
#define    PING3    3
#define    PING2    2
#define    PING1    1
#define    PING0    0

/* DDRG : Data Direction Register, Port G */
#define    DDG4     4
#define    DDG3     3
#define    DDG2     2
#define    DDG1     1
#define    DDG0     0

/* PORTG : Data Register, Port G */
#define    PG4      4
#define    PG3      3
#define    PG2      2
#define    PG1      1
#define    PG0      0

/* PORTG : Data Register, Port G */
#define    PORTG4   4
#define    PORTG3   3
#define    PORTG2   2
#define    PORTG1   1
#define    PORTG0   0

/* TFR0 : Timer/Counter Interrupt Flag Register 0 */
#define    OCF0A    1
#define    TOV0     0

/* TFR1 : Timer/Counter Interrupt Flag Register 1 */
#define    ICF1     5
#define    OCF1C    3
#define    OCF1B    2
#define    OCF1A    1
#define    TOV1     0

/* TFR2 : Timer/Counter Interrupt Flag Register 2 */
#define    OCF2A    1
#define    TOV2     0

/* TFR3 : Timer/Counter Interrupt Flag Register 3 */
#define    ICF3     5
#define    OCF3C    3
#define    OCF3B    2
#define    OCF3A    1
#define    TOV3     0

/* EIFR : External Interrupt Flag Register */
#define    INTF7    7
#define    INTF6    6
#define    INTF5    5
#define    INTF4    4
#define    INTF3    3
#define    INTF2    2
#define    INTF1    1
#define    INTF0    0

/* EIMSK : External Interrupt Mask Register */
#define    INT7     7
#define    INT6     6
#define    INT5     5
#define    INT4     4
#define    INT3     3
#define    INT2     2
#define    INT1     1
#define    INT0     0

/* EICRA : External Interrupt Control Register A*/
#define    ISC31     7
#define    ISC30     6
#define    ISC21     5
#define    ISC20     4
#define    ISC11     3
#define    ISC10     2
#define    ISC01     1
#define    ISC00     0

/* EICRA : External Interrupt Control Register B*/
#define    ISC71     7
#define    ISC70     6
#define    ISC61     5
#define    ISC60     4
#define    ISC51     3
#define    ISC50     2
#define    ISC41     1
#define    ISC40     0

/* EECR : EEPROM Control Register */
#define    EERIE    3
#define    EEMWE    2
#define    EEWE     1
#define    EERE     0

/* GTCCR : General Timer Control Register */
#define    TSM      7
#define    PSR2     1
#define    PSR310   0


/* TCCR0A : Timer/Counter 0 Control Register */
#define    COM0A1    7
#define    COM0A0    6
#define    COM0B1    5
#define    COM0B0    4
#define    WGM01     1
#define    WGM00     0

/* TCCR0B : Timer/Counter 0 Control Register */
#define    FOC0A    7
#define    FOC0B    6

#define    WGM02    3
#define    CS02     2
#define    CS01     1
#define    CS00     0

/* SPCR : SPI Control Register */
#define    SPIE     7
#define    SPE      6
#define    DORD     5
#define    MSTR     4
#define    CPOL     3
#define    CPHA     2
#define    SPR1     1
#define    SPR0     0

/* SPSR : SPI Status Register */
#define    SPIF     7
#define    WCOL     6
#define    SPI2X    0

/* ACSR : Analog Comparator Control and Status Register */
#define    ACD      7
#define    ACBG     6
#define    ACO      5
#define    ACI      4
#define    ACIE     3
#define    ACIC     2
#define    ACIS1    1
#define    ACIS0    0

/* OCDR : On-Chip Debug Register */
#define    IDRD     7
#define    OCDR7    7
#define    OCDR6    6
#define    OCDR5    5
#define    OCDR4    4
#define    OCDR3    3
#define    OCDR2    2
#define    OCDR1    1
#define    OCDR0    0

/* SMCR : Sleep Mode Control Register */
#define    SM2      3
#define    SM1      2
#define    SM0      1
#define    SE       0

/* MCUSR : MCU general Status Register */
#define    JTRF     4
#define    WDRF     3
#define    BORF     2
#define    EXTRF    1
#define    PORF     0

/* MCUCR : MCU general Control Register */
#define    JTD      7
#define    PUD      4
#define    IVSEL    1
#define    IVCE     0

/* SPMCR : Store Program Memory Control and Status Register */
#define    SPMIE    7
#define    RWWSB    6
#define    SIGRD    5
#define    RWWSRE   4
#define    BLBSET   3
#define    PGWRT    2
#define    PGERS    1
#define    SPMEN    0

/* RAMPZ : RAM Page Z Select Register */
#define    RAMPZ0   0

/* SPH : Stack Pointer High */
#define    SP15     7
#define    SP14     6
#define    SP13     5
#define    SP12     4
#define    SP11     3
#define    SP10     2
#define    SP9      1
#define    SP8      0

/* SPL : Stack Pointer Low */
#define    SP7      7
#define    SP6      6
#define    SP5      5
#define    SP4      4
#define    SP3      3
#define    SP2      2
#define    SP1      1
#define    SP0      0

/* WTDCR : Watchdog Timer Control Register */
#define    WDIF     7
#define    WDIE     6
#define    WDP3     5
#define    WDCE     4
#define    WDE      3
#define    WDP2     2
#define    WDP1     1
#define    WDP0     0

/* CLKPR : Source Clock Prescaler Register */
#define    CKLPCE   7
#define    CLKPCE   7 //for compatiblity
#define    CKLPS3   3
#define    CKLPS2   2
#define    CKLPS1   1
#define    CKLPS0   0

/* TIMSK0 : Timer Interrupt Mask Register0 */
#define    OCIE0B   2
#define    OCIE0A   1
#define    TOIE0    0

/* TIMSK1 : Timer Interrupt Mask Register1 */
#define    ICIE1    5
#define    OCIE1C   3
#define    OCIE1B   2
#define    OCIE1A   1
#define    TOIE1    0

/* TIMSK2 : Timer Interrupt Mask Register2 */
#define    OCIE2A   1
#define    TOIE2    0

/* TIMSK3 : Timer Interrupt Mask Register3 */
#define    ICIE3    5
#define    OCIE3C   3
#define    OCIE3B   2
#define    OCIE3A   1
#define    TOIE3    0

/* XMCRA : External Memory Control A Register */
#define    SRE      7
#define    SRL2     6
#define    SRL1     5
#define    SRL0     4
#define    SRW11    3
#define    SRW10    2
#define    SRW01    1
#define    SRW00    0

/* XMCRB : External Memory Control B Register */
#define    XMBK     7
#define    XMM2     2
#define    XMM1     1
#define    XMM0     0

/* ADCSRA : ADC Control and Status Register A*/
#define    ADEN     7
#define    ADSC     6
#define    ADATE    5
#define    ADIF     4
#define    ADIE     3
#define    ADPS2    2
#define    ADPS1    1
#define    ADPS0    0

/* ADCSRB : ADC Control and Status Register B*/
#define    ADHSM   7
#define    ACME     6
#define    ADST2    2
#define    ADST1    1
#define    ADST0    0

/* ADMUX : ADC Multiplexer Selection Register */
#define    REFS1    7
#define    REFS0    6
#define    ADLAR    5
#define    MUX4     4
#define    MUX3     3
#define    MUX2     2
#define    MUX1     1
#define    MUX0     0

/* TCCR1A : Timer/Counter 1 Control Register A */
#define    COM1A1   7
#define    COM1A0   6
#define    COM1B1   5
#define    COM1B0   4
#define    COM1C1   3
#define    COM1C0   2
#define    WGM11    1
#define    WGM10    0

/* TCCR1B : Timer/Counter 1 Control Register B */
#define    ICNC1    7
#define    ICES1    6
#define    WGM13    4
#define    WGM12    3
#define    CS12     2
#define    CS11     1
#define    CS10     0

/* TCCR1C : Timer/Counter 1 Control Register C */
#define    FOC1A    7
#define    FOC1B    6
#define    FOC1C    5

/* TCCR3A : Timer/Counter 3 Control Register A */
#define    COM3A1   7
#define    COM3A0   6
#define    COM3B1   5
#define    COM3B0   4
#define    COM3C1   3
#define    COM3C0   2
#define    WGM31    1
#define    WGM30    0

/* TCCR3B : Timer/Counter 3 Control Register B */
#define    ICNC3    7
#define    ICES3    6
#define    WGM33    4
#define    WGM32    3
#define    CS32     2
#define    CS31     1
#define    CS30     0

/* TCCR3C : Timer/Counter 3 Control Register C */
#define    FOC3A    7
#define    FOC3B    6
#define    FOC3C    5

/* TCCR2A : Timer/Counter 2 Control Register A*/
#define    FOC2     7
#define    WGM20    6
#define    COM21    5
#define    COM20    4
#define    WGM21    3
#define    CS22     2
#define    CS21     1
#define    CS20     0

/* ASSR : Asynchronous mode Status Register */
#define    EXCLK    4
#define    AS2      3
#define    TCN2UB   2
#define    OCR2UB   1
#define    TCR2UB   0

/* TWSR : TWI Status Register */
#define    TWS7     7
#define    TWS6     6
#define    TWS5     5
#define    TWS4     4
#define    TWS3     3
#define    TWPS1    1
#define    TWPS0    0

/* TWAR : TWI (slave) Address Register */
#define    TWA6     7
#define    TWA5     6
#define    TWA4     5
#define    TWA3     4
#define    TWA2     3
#define    TWA1     2
#define    TWA0     1
#define    TWGCE    0

/* TWCR : TWI Control Register */
#define    TWINT    7
#define    TWEA     6
#define    TWSTA    5
#define    TWSTO    4
#define    TWWC     3
#define    TWEN     2
#define    TWIE     0

/* UCSR0A : USART0 Control and Status Register A */
#define    RXC0     7
#define    TXC0     6
#define    UDRE0    5
#define    FE0      4
#define    DOR0     3
#define    UPE0     2
#define    U2X0     1
#define    MPCM0    0

/* UCSR0B : USART0 Control and Status Register B */
#define    RXCIE0   7
#define    TXCIE0   6
#define    UDRIE0   5
#define    RXEN0    4
#define    TXEN0    3
#define    UCSZ02   2
#define    RXB80    1
#define    TXB80    0

/* UCSR0C : USART0 Control and Status Register C */
#define    UMSEL0   6
#define    UPM01    5
#define    UPM00    4
#define    USBS0    3
#define    UCSZ01   2
#define    UCSZ00   1
#define    UCPOL0   0

/* UCSR1A : USART1 Control and Status Register A */
#define    RXC1     7
#define    TXC1     6
#define    UDRE1    5
#define    FE1      4
#define    DOR1     3
#define    UPE1     2
#define    U2X1     1
#define    MPCM1    0

/* UCSR1B : USART1 Control and Status Register B */
#define    RXCIE1   7
#define    TXCIE1   6
#define    UDRIE1   5
#define    RXEN1    4
#define    TXEN1    3
#define    UCSZ12   2
#define    RXB81    1
#define    TXB81    0

/* UCSR1C : USART1 Control and Status Register C */
#define    UMSEL1   6
#define    UPM11    5
#define    UPM10    4
#define    USBS1    3
#define    UCSZ11   2
#define    UCSZ10   1
#define    UCPOL1   0


/* PCICR Pin Change Interrupt control */
#define  PCIE0 0

/* PCIFR Pin Change Interrupt flag */
#define  PCIF0 0

/* PCIMSK0 Pin Change Mask */
#define  PCINT7    7
#define  PCINT6    6
#define  PCINT5    5
#define  PCINT4    4
#define  PCINT3    3
#define  PCINT2    2
#define  PCINT1    1
#define  PCINT0    0


/* ***** USB_DEVICE ******************* */
/* UDCON -  */
#define    DETACH          0       //
#define    RMWKUP          1       //
#define    LSM             2       //

/* UDINT -  */
#define    SUSPI           0       //
#define    MSOFI           1       //
#define    SOFI            2       //
#define    EORSTI          3       //
#define    WAKEUPI         4       //
#define    EORSMI          5       //
#define    UPRSMI          6       //

/* UDIEN -  */
#define    SUSPE           0       //
#define    MSOFE           1       //
#define    SOFE            2       //
#define    EORSTE          3       //
#define    WAKEUPE         4       //
#define    EORSME          5       //
#define    UPRSME          6       //

/* UDADDR -  */
#define    UDADDR0         0       //
#define    UDADDR1         1       //
#define    UDADDR2         2       //
#define    UDADDR3         3       //
#define    UDADDR4         4       //
#define    UDADDR5         5       //
#define    UDADDR6         6       //
#define    ADDEN           7       //

/* UDFNUML -  */
#define    UDFNUML_0       0       //
#define    UDFNUML_1       1       //
#define    UDFNUML_2       2       //
#define    UDFNUML_3       3       //
#define    UDFNUML_4       4       //
#define    UDFNUML_5       5       //
#define    UDFNUML_6       6       //
#define    UDFNUML_7       7       //

/* UDFNUMH -  */
#define    UDFNUMH_0       0       //
#define    UDFNUMH_1       1       //
#define    UDFNUMH_2       2       //

/* UDMFN -  */
#define    FNCERR          4       //

/* UEINTX -  */
#define    TXINI           0       //
#define    STALLEDI        1       //
#define    RXOUTI          2       //
#define    RXSTPI          3       //
#define    NAKOUTI         4       //
#define    RWAL            5       //
#define    NAKINI          6       //
#define    FIFOCON         7       //

/* UENUM -  */
#define    UENUM_0         0       //
#define    UENUM_1         1       //
#define    UENUM_2         2       //

/* UERST -  */
#define    EPRST0          0       //
#define    EPRST1          1       //
#define    EPRST2          2       //
#define    EPRST3          3       //
#define    EPRST4          4       //
#define    EPRST5          5       //
#define    EPRST6          6       //

/* UECONX -  */
#define    EPEN            0       //
#define    RSTDT           3       //
#define    STALLRQC        4       //
#define    STALLRQ         5       //

/* UECFG0X -  */
#define    EPDIR           0       //
#define    NYETDIS         1       //
#define    AUTOSW          2       //
#define    ISOSW           3       //
#define    EPTYPE0         6       //
#define    EPTYPE1         7       //

/* UECFG1X -  */
#define    ALLOC           1       //
#define    EPBK0           2       //
#define    EPBK1           3       //
#define    EPSIZE0         4       //
#define    EPSIZE1         5       //
#define    EPSIZE2         6       //

/* UESTA0X -  */
#define    NBUSYBK0        0       //
#define    NBUSYBK1        1       //
#define    DTSEQ0          2       //
#define    DTSEQ1          3       //
#define    ZLPSEEN         4       //
#define    UNDERFI         5       //
#define    OVERFI          6       //
#define    CFGOK           7       //

/* UESTA1X -  */
#define    CURRBK0         0       //
#define    CURRBK1         1       //
#define    CTRLDIR         2       //

/* UEIENX -  */
#define    TXINE           0       //
#define    STALLEDE        1       //
#define    RXOUTE          2       //
#define    RXSTPE          3       //
#define    NAKOUTE         4       //
#define    NAKINE          6       //
#define    FLERRE          7       //

/* UEDATX -  */
#define    UEDATX_0        0       //
#define    UEDATX_1        1       //
#define    UEDATX_2        2       //
#define    UEDATX_3        3       //
#define    UEDATX_4        4       //
#define    UEDATX_5        5       //
#define    UEDATX_6        6       //
#define    UEDATX_7        7       //

/* UEBCLX -  */
#define    UEBCLX_0        0       //
#define    UEBCLX_1        1       //
#define    UEBCLX_2        2       //
#define    UEBCLX_3        3       //
#define    UEBCLX_4        4       //
#define    UEBCLX_5        5       //
#define    UEBCLX_6        6       //
#define    UEBCLX_7        7       //

/* UEBCHX -  */
#define    UEBCHX_0        0       //
#define    UEBCHX_1        1       //
#define    UEBCHX_2        2       //

/* UEINT -  */
#define    UEINT_0         0       //
#define    UEINT_1         1       //
#define    UEINT_2         2       //
#define    UEINT_3         3       //
#define    UEINT_4         4       //
#define    UEINT_5         5       //
#define    UEINT_6         6       //


/* ***** USB_GLOBAL ******************* */
/* UHWCON - USB Hardware Configuration Register */
#define    UVREGE          0       //
#define    UVCONE          4       //
#define    UIDE            6       //
#define    UIMOD           7       //

/* USBCON - USB General Control Register */
#define    VBUSTE          0       //
#define    IDTE            1       //
#define    OTGPADE         4       //
#define    FRZCLK          5       //
#define    HOST            6       //
#define    USBE            7       //

/* USBSTA -  */
#define    VBUS            0       //
#define    ID              1       //
#define    SPEED           3       //

/* USBINT -  */
#define    VBUSTI          0       //
#define    IDTI            1       //


/* OTGCON -  */
#define    VBUSRQC         0       //
#define    VBUSREQ         1       //
#define    VBUSHWC         2       //
#define    SRPSEL          3       //
#define    SRPREQ          4       //
#define    HNPREQ          5       //

/* OTGIEN -  */
#define    SRPE            0       //
#define    VBERRE          1       //
#define    BCERRE          2       //
#define    ROLEEXE         3       //
#define    HNPERRE         4       //
#define    STOE            5       //

/* OTGINT -  */
#define    SRPI            0       //
#define    VBERRI          1       //
#define    BCERRI          2       //
#define    ROLEEXI         3       //
#define    HNPERRI         4       //
#define    STOI            5       //


/* ***** USB_HOST ********************* */
/* UHCON -  */
#define    SOFEN           0       //
#define    RESET           1       //
#define    RESUME          2       //

/* UHINT -  */
#define    DCONNI          0       //
#define    DDISCI          1       //
#define    RSTI            2       //
#define    RSMEDI          3       //
#define    RXRSMI          4       //
#define    HSOFI           5       //
#define    HWUPI           6       //

/* UHIEN -  */
#define    HWUPE           6
#define    HSOFE           5
#define    RXRSME          4
#define    RSMEDE          3
#define    RSTE            2
#define    DDISCE          1
#define    DCONNE          0



/* UHADDR -  */
#define    UHADDR_0        0       //
#define    UHADDR_1        1       //
#define    UHADDR_2        2       //
#define    UHADDR_3        3       //
#define    UHADDR_4        4       //
#define    UHADDR_5        5       //
#define    UHADDR_6        6       //

/* UHFNUMH -  */
#define    UHFNUMH_0       0       //
#define    UHFNUMH_1       1       //
#define    UHFNUMH_2       2       //

/* UHFNUML -  */
#define    UHFNUML_0       0       //
#define    UHFNUML_1       1       //
#define    UHFNUML_2       2       //
#define    UHFNUML_3       3       //
#define    UHFNUML_4       4       //
#define    UHFNUML_5       5       //
#define    UHFNUML_6       6       //
#define    UHFNUML_7       7       //

/* UHFLEN -  */
#define    UHFLEN_0        0       //
#define    UHFLEN_1        1       //
#define    UHFLEN_2        2       //
#define    UHFLEN_3        3       //
#define    UHFLEN_4        4       //
#define    UHFLEN_5        5       //
#define    UHFLEN_6        6       //
#define    UHFLEN_7        7       //

/* UPINRQX -  */
#define    INRQ0           0       //
#define    INRQ1           1       //
#define    INRQ2           2       //
#define    INRQ3           3       //
#define    INRQ4           4       //
#define    INRQ5           5       //
#define    INRQ6           6       //
#define    INRQ7           7       //

/* UPINTX -  */
#define    RXINI           0       //
#define    RXSTALLI        1       //
#define    TXOUTI          2       //
#define    TXSTPI          3       //
#define    PERRI           4       //
//#define  RWAL            5       //
#define    NAKEDI          6       //
//#define  FIFOCON         7       //

/* UPNUM -  */
#define    PNUM0           0       //
#define    PNUM1           1       //
#define    PNUM2           2       //

/* UPRST -  */
#define    PRST0           0       //
#define    PRST1           1       //
#define    PRST2           2       //
#define    PRST3           3       //
#define    PRST4           4       //
#define    PRST5           5       //
#define    PRST6           6       //

/* UPCONX -  */
#define    PEN             0       //
//#define  RSTDT           3       //
#define    INMODE          5       //
#define    PFREEZE         6       //

/* UPCFG0X -  */
#define    PEPNUM0         0       //
#define    PEPNUM1         1       //
#define    PEPNUM2         2       //
#define    PEPNUM3         3       //
#define    PTOKEN0         4       //
#define    PTOKEN1         5       //
#define    PTYPE0          6       //
#define    PTYPE1          7       //

/* UPCFG1X -  */
//#define  ALLOC           1       //
#define    PBK0            2       //
#define    PBK1            3       //
#define    PSIZE0          4       //
#define    PSIZE1          5       //
#define    PSIZE2          6       //

/* UPSTAX -  */
#define    NBUSYK0         0       //
#define    NBUSYK1         1       //
//#define  DTSEQ0          2       //
//#define  DTSEQ1          3       //
//#define  UNDERFI         5       //
//#define  OVERFI          6       //
//#define  CFGOK           7       //

/* UPCFG2X -  */
/* USB_HOST/UPCFG2X/BIT0: Name missing */
/* USB_HOST/UPCFG2X/BIT1: Name missing */
/* USB_HOST/UPCFG2X/BIT2: Name missing */
/* USB_HOST/UPCFG2X/BIT3: Name missing */
/* USB_HOST/UPCFG2X/BIT4: Name missing */
/* USB_HOST/UPCFG2X/BIT5: Name missing */
/* USB_HOST/UPCFG2X/BIT6: Name missing */
/* USB_HOST/UPCFG2X/BIT7: Name missing */

/* UPIENX -  */
#define    RXINE           0       //
#define    RXSTALLE        1       //
#define    TXOUTE          2       //
#define    TXSTPE          3       //
#define    PERRE           4       //
#define    NAKEDE          6       //
//#define  FLERRE          7       //

/* UPDATX -  */
#define    PDAT0           0       //
#define    PDAT1           1       //
#define    PDAT2           2       //
#define    PDAT3           3       //
#define    PDAT4           4       //
#define    PDAT5           5       //
#define    PDAT6           6       //
#define    PDAT7           7       //

/* UPBCLX -  */
#define    PBYCT0          0       //
#define    PBYCT1          1       //
#define    PBYCT2          2       //
#define    PBYCT3          3       //
#define    PBYCT4          4       //
#define    PBYCT5          5       //
#define    PBYCT6          6       //
#define    PBYCT7          7       //

/* UPBCHX -  */
#define    PBYCT8          0       //
#define    PBYCT9          1       //
#define    PBYCT10         2       //

/* UPINT -  */
#define    PINT0           0       //
#define    PINT1           1       //
#define    PINT2           2       //
#define    PINT3           3       //
#define    PINT4           4       //
#define    PINT5           5       //
#define    PINT6           6       //

/* UPERRX -  */
#define    DATATGL         0       //
#define    DATAPID         1       //
#define    PID             2       //
#define    TIMEOUT         3       //
#define    CRC16           4       //
#define    COUNTER0        5       //
#define    COUNTER1        6       //

/* Pointer definition */
#define    XL     r26
#define    XH     r27
#define    YL     r28
#define    YH     r29
#define    ZL     r30
#define    ZH     r31


// registers PLLCSR
#define    PLLP2 		4	
#define    PLLP1 		3
#define    PLLP0 		2
#define    PLLE 		1
#define    PLOCK 		0




#endif  /* _MCU_H*/














