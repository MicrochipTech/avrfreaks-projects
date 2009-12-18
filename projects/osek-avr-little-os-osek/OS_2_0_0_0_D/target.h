/*



*/
/*********************************************************/
/*                      target.h                         */
/*                                                       */
/*   (C) by Joerg Schwedes 2009, All rights reserved     */
/*********************************************************/
#ifndef TARGET_H
#define TARGET_H
#ifdef __ICCAVR__ /* predefined by IAR compiler */ 
/*
IAR AVR
*/
/* from IAR compiler */
#include <inavr.h> /* intrinsic */
#ifndef ENABLE_BIT_DEFINITIONS 
#define ENABLE_BIT_DEFINITIONS
#endif
#include <ioavr.h> /* io resources */


/**************************************************************************************/ 
/* interfaces to uC functions
   and compiler intrinsics: Here IAR compiler       */

#define EnaInt()   __enable_interrupt() /* enables global interrupt flag */
#define DisInt()   __disable_interrupt() /* disables global interrupt flag */
#define SaveInt()  __save_interrupt()
#define RestoreInt(x) __restore_interrupt(x)

#define __CONST __flash const
  /* constants and tables are placed and accessed in flash memory.
     Important,
     because otherwise they are placed as a work copy in valuably RAM.
     This maybe is a feature only for IAR compiler */

//#define __NO_INIT __no_init
  /* RAM area which has no init.
     This maybe a feature only for IAR compiler */

#define __NO_RETURN(func)    __C_task  void func(void)

#define PRAGMA(x) _Pragma( #x ) 
#define ISR(vec) PRAGMA( vector=vec ) __interrupt void isr_##vec(void) 

#else
#ifdef __GNUC__ /* predefined by GCC */
/*
AVRGCC
*/
/* asuming AVR GCC compiler */

#include <avr/io.h> /* io resources */
#include <avr/interrupt.h>
#include <avr/common.h>

#define EnaInt()  sei() /* enables global interrupt flag */
#define DisInt()  cli() /* disables global interrupt flag */


#define SaveInt() AVR_STATUS_REG
#define RestoreInt(x) AVR_STATUS_REG=x

#define __CONST const /* GCC has actual no proper way for accessing data in flash */

#define __NO_RETURN(func)   void func(void)  __attribute__((noreturn));  void func(void)
#else
#ifdef __CODEVISIONAVR__ /* predefined by Codevision */
/*
Codevsion
*/
#include <io.h>

#define EnaInt() #asm("sei")
#define DisInt() #asm("cli")
#define SaveInt() SREG
#define RestoreInt(x) SREG=x
#define __CONST flash const

#define __NO_RETURN(func) void func(void)

#define ISR(vec) interrupt[vec] void isr_##vec(void)


#if defined _CHIP_ATMEGA16_ | defined _CHIP_ATMEGA16L_ | defined _CHIP_ATMEGA8_ \
   | defined _CHIP_ATMEGA8L_ | defined _CHIP_ATMEGA32_ | defined _CHIP_ATMEGA32L_
/*
*/
#define INT0_vect         EXT_INT0
#define INT1_vect         EXT_INT1
#define TIMER2_COMP_vect  TIM2_COMP
#define TIMER2_OVF_vect   TIM2_OVF
#define TIMER1_CAPT_vect  TIM1_CAPT
#define TIMER1_COMPA_vect TIM1_COMPA
#define TIMER1_COMPB_vect TIM1_COMPB
#define TIMER1_OVF_vect   TIM1_OVF
#define TIMER0_OVF_vect   TIM0_OVF
#define SPI_STC_vect      SPI_STC
#define USART_RXC_vect    USART_RXC
#define USART_UDRE_vect   USART_DRE
#define USART_TXC_vect    USART_TXC
#define ADC_vect          ADC_INT
#define EE_RDY_vect       EE_RDY
#define ANA_COMP_vect     ANA_COMP
#define TWI_vect          TWI
#define INT2_vect         EXT_INT2
#define TIMER0_COMP_vect  TIM0_COMP
#define SPM_RDY_vect      SPM_READY
#else
#if defined _CHIP_ATMEGA48_ | defined _CHIP_ATMEGA48V_ | defined _CHIP_ATMEGA48P_ \
   | defined _CHIP_ATMEGA48PV_ | defined _CHIP_ATMEGA88_ | defined _CHIP_ATMEGA88V_ \
   | defined _CHIP_ATMEGA88P_ | defined _CHIP_ATMEGA88PV_ | defined _CHIP_ATMEGA168_ \
   | defined _CHIP_ATMEGA168V_ | defined _CHIP_ATMEGA168P_ | defined _CHIP_ATMEGA168PV_
/*
*/
#define INT0_vect         EXT_INT0 
#define INT1_vect         EXT_INT1 
#define PCINT0_vect       PC_INT0 
#define PCINT1_vec        PC_INT1 
#define PCINT2_vect       PC_INT2 
#define WDT_vect          WDT
#define TIMER2_COMPA_vect TIM2_COMPA
#define TIMER2_COMPB_vect TIM2_COMPB
#define TIMER2_OVF_vect   TIM2_OVF
#define TIMER1_CAPT_vect  TIM1_CAPT
#define TIMER1_COMPA_vect TIM1_COMPA
#define TIMER1_COMPB_vect TIM1_COMPB
#define TIMER1_OVF_vect   TIM1_OVF
#define TIMER0_COMPA_vect TIM0_COMPA
#define TIMER0_COMPB_vect TIM0_COMPB
#define TIMER0_OVF_vect   TIM0_OVF
#define SPI_STC_vect      SPI_STC
#define USART_RX_vect     USART_RXC
#define USART_UDRE_vect   USART_DRE
#define USART_TX_vect     USART_TXC
#define ADC_vect          ADC_INT
#define EE_READY_vect     EE_RDY
#define ANALOG_COMP_vect  ANA_COMP
#define TWI_vect          TWI
#define SPM_READY_vect    SPM_READY
#else
#if defined _CHIP_ATTINY25_ | defined _CHIP_ATTINY45_ | defined _CHIP_ATTINY85_
/*
*/
#define INT0_vect           EXT_INT0
#define PCINT0_vect         PC_INT0
#define TIM1_COMPA_vect     TIM1_COMPA
#define TIM1_OVF_vect       TIM1_OVF
#define TIM0_OVF_vect       TIM0_OVF
#define EE_RDY_vect         EE_RDY
#define ANA_COMP_vect       ANA_COMP
#define ADC_vect            ADC_INT
#define TIM1_COMPB_vect     TIM1_COMPB
#define TIM0_COMPA_vect     TIM0_COMPA
#define TIM0_COMPB_vect     TIM0_COMPB
#define WDT_vect            WDT
#define USI_START_vect      USI_START
#define USI_OVF_vect        USI_OVF
#endif
#endif
#endif

 
#else
#ifdef __IMAGECRAFT__ /* predefined by Imagecraft */
/*
Imagecraft
*/
#include <iccioavr.h>

#define EnaInt() asm("sei")
#define DisInt() asm("cli")
#define SaveInt() SREG
#define RestoreInt(x) SREG=x
#define __CONST __flash /* "__flash const" is not accepted by Imagecraft */
#define inline /* actual no c99 support for inline */

#define PRAGMA(x) _Pragma( #x )

#define ISR(vec) PRAGMA( interrupt_handler isr_##vec: vec ) void isr_##vec(void) 

#define __NO_RETURN(func) PRAGMA(ctask func) void func(void)
#if defined(ATMega8) || defined(ATMega8A) || defined(ATMega16) || defined(ATMega16A) \
    || defined(ATMega32) || defined(ATMega32A)
/*
*/
#define INT0_vect         iv_INT0
#define INT1_vect         iv_INT1
#define TIMER2_COMP_vect  iv_TIMER2_COMPA
#define TIMER2_OVF_vect   iv_TIMER2_OVF
#define TIMER1_CAPT_vect  iv_TIMER1_CAPT
#define TIMER1_COMPA_vect iv_TIMER1_COMPA
#define TIMER1_COMPB_vect iv_TIMER1_COMPB
#define TIMER1_OVF_vect   iv_TIMER1_OVF
#define TIMER0_OVF_vect   iv_TIMER0_OVF
#define SPI_STC_vect      iv_SPI_STC
#define USART_RXC_vect    iv_USART_RXC
#define USART_UDRE_vect   iv_USART_UDRE
#define USART_TXC_vect    iv_USART_TXC
#define ADC_vect          iv_ADC
#define EE_RDY_vect       iv_EE_RDY
#define ANA_COMP_vect     iv_ANA_COMP
#define TWI_vect          iv_TWI
#define INT2_vect         iv_INT2
#define TIMER0_COMP_vect  iv_TIMER0_COMP
#define SPM_RDY_vect      iv_SPM_RDY
#else
#if defined(ATMega48P) || defined(ATMega48PA) || defined(ATMega88P) \
    || defined(ATMega88PA) || defined(ATMega168P) || defined(ATMega168PA)
/*
*/
#define INT0_vect         iv_INT0
#define INT1_vect         iv_INT1
#define PCINT0_vect       iv_PCINT0
#define PCINT1_vec        iv_PCINT1
#define PCINT2_vect       iv_PCINT2
#define WDT_vect          iv_WDT
#define TIMER2_COMPA_vect iv_TIMER2_COMPA
#define TIMER2_COMPB_vect iv_TIMER2_COMPB
#define TIMER2_OVF_vect   iv_TIMER2_OVF
#define TIMER1_CAPT_vect  iv_TIMER1_CAPT
#define TIMER1_COMPA_vect iv_TIMER1_COMPA
#define TIMER1_COMPB_vect iv_TIMER1_COMPB
#define TIMER1_OVF_vect   iv_TIMER1_OVF
#define TIMER0_COMPA_vect iv_TIMER0_COMPA
#define TIMER0_COMPB_vect iv_TIMER0_COMPB
#define TIMER0_OVF_vect   iv_TIMER0_OVF
#define SPI_STC_vect      iv_SPI_STC
#define USART_RX_vect     iv_USART_RX
#define USART_UDRE_vect   iv_USART_UDRE
#define USART_TX_vect     iv_USART_TX
#define ADC_vect          iv_ADC
#define EE_READY_vect     iv_EE_READY
#define ANALOG_COMP_vect  iv_ANALOG_COMP
#define TWI_vect          iv_TWI
#define SPM_READY_vect    iv_SPM_READY
#else
#if defined(ATtiny25) || defined(ATtiny45) || defined(ATtiny85)
/*
*/
#define INT0_vect           iv_INT0
#define PCINT0_vect         iv_PCINT0
#define TIM1_COMPA_vect     iv_TIM1_COMPA
#define TIM1_OVF_vect       iv_TIM1_OVF
#define TIM0_OVF_vect       iv_TIM0_OVF
#define EE_RDY_vect         iv_EE_RDY
#define ANA_COMP_vect       iv_ANA_COMP
#define ADC_vect            iv_ADC
#define TIM1_COMPB_vect     iv_TIM1_COMPB
#define TIM0_COMPA_vect     iv_TIM0_COMPA
#define TIM0_COMPB_vect     iv_TIM0_COMPB
#define WDT_vect            iv_WDT
#define USI_START_vect      iv_USI_START
#define USI_OVF_vect        iv_USI_OVF
#endif
#endif
#endif
#else
#ifdef __CROSSWORKS_AVR /* crossworks */
/*
crossworks
*/
#error SCHWED-OSEK not correctly running if compiled with crossworks because compiler has problems with word access to sfr registers. Please help!!!!


#include <avr.h> 
#include <inavr.h> 

#define EnaInt()  __enable_interrupt() 
#define DisInt()  __disable_interrupt() 

#define SaveInt()   __save_interrupt() 
#define RestoreInt(x)  __restore_interrupt(x) 

#define inline 

#define __CONST  const __code 

#define ISR(vec)  void isr_##vec(void) __interrupt[vec] 

#define __NO_RETURN(x) void x (void) __toplevel

#if  (  (__TARGET_PROCESSOR == ATmega16) ||  (__TARGET_PROCESSOR == ATmega8)    \
    || (__TARGET_PROCESSOR == ATmega8A)  ||  (__TARGET_PROCESSOR == ATmega32)   \
    || (__TARGET_PROCESSOR == ATmega32A) )
/*
*/
#define TCNT1 TCNT1W
#define OCR1A OCR1AW
#define OCR1B OCR1BW
#define ICR1  ICR1W
#define EEAR  EEARW
#define ADC   ADCW
#else
#if (  (__TARGET_PROCESSOR == ATmega48) ||  (__TARGET_PROCESSOR == ATmega48P) \
   || (__TARGET_PROCESSOR == ATmega88)  ||  (__TARGET_PROCESSOR == ATmega88P) \
   || (__TARGET_PROCESSOR == ATmega168) ||  (__TARGET_PROCESSOR == ATmega168A) )
/*
*/
#define UBRR0 UBRR0W
#define OCR1B OCR1BW
#define OCR1A OCR1AW
#define ICR1 ICR1W
#define TCNT1 TCNT1W
#define ADC ADCW
#define EEAR EEARW
#else
#if  ( (__TARGET_PROCESSOR == ATtiny25) ||  (__TARGET_PROCESSOR == ATtiny45) \
   || (__TARGET_PROCESSOR == ATtiny85) ) 
/*
*/
#define EEAR EEARW
#define ADC  ADCW
#endif
#endif
#endif

#else
#error no compiler support 
#endif
#endif
#endif
#endif
#endif
#endif
