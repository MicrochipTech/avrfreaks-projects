/******************************************************************************
* PROGRAM:     Lučka - Spark
* PURPOSE:     Experimetnal enlightement regulator
* COPYRIGHT:   Radovid Drobnjak, University of Maribor
* LICENSE:     Licenced under the BSD license, see below.
******************************************************************************/

/***************************************************************************************************
Copyright (c) 2007, Radovid Drobnjak, University of Maribor

This material was originally written and compiled by Radovid Drobnjak at
Faculty of Electrical Engieering and Computer Science and Faculty of Economics and Business, University of Maribor, Slovenia, 2007

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the project nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************************************/

/************************************************************************************************
* FILE: init.h
*
* this file includes I/O and initialization related definitions
*
* it is divided into different sections for ATmega128(at this stage useless), (ATmega16) and
* (ATmega16)
*
************************************************************************************************/


/*********************************************************************************
*
*   Section :  GENERAL
*
*
*   Purpose :  CONTAINS MACROS AND DEFINITIONS WHICH ARE OF GENERAL PURPOSE AND ARE NOT IN DIRECT
*              CONNECTION WITH A SPECIFIC PART OF THE PROGRAM
*
*********************************************************************************/


// ---MACROS:---

#define sbi(byte,bit)  (byte |= (1<<bit))
#define cbi(byte,bit)  (byte &= ~(1<<bit))

/*********************************************************************************
*
*   Section :  COMPATIBILITY
*
*
*   Purpose :  ASHURING COMPATIBILITY BETWEEN ATMEGA169, ATMEGA16 AND ATMEGA128
*
*
*********************************************************************************/


#if !defined(IOCOMPAT_H)
#define IOCOMPAT_H 1


#if defined (__AVR_ATmega128__)
// TIMER/COUNTER 2
   #define TCCR2A TCCR2
   #define OCR2A OCR2
   #define COM2A1 COM21
   #define TIMSK2 TIMSK

// TIMER/COUNTER 0

   #define TCCR0A TCCR0
   #define OCR0A OCR0
   #define TIMSK0 TIMSK
   #define OCIE0A OCIE0

// UART

   #define UCSRB UCSR0B 
   #define TXEN TXEN0
   #define RXEN RXEN0
   #define UCSRC UCSR0C 
   #define UCSZ1 UCSZ01
   #define UCSZ0 UCSZ00


//   --- ATMEGA16 ---
/**********************************************************************************
* IMPORTANT NOTE: for propper functioning of the program
*
* Atmega 16 should be running at 8MHZ on INTERNAL RC oscilator
*
* FUSE LOW BYTE = 0xe4
**********************************************************************************/

#elif defined (__AVR_ATmega16__)



/*********************************************************************************
*   Chip :     ATmega16 
*
*   Section :  PWM
*
*
*   Purpose :  PWM RELATED DEFINITIONS AND MACROS
*
*
*********************************************************************************/



/*initialization of timer/counter 0 for fast PWM modulation, using I/O timer without prescaler */

#define TIMER_PWM_INIT TCCR0 |=  (1<<WGM01) | (1<<WGM00) | (1<<COM01) | (1<<CS00)

// parameter values
#define OCR_PWM OCR0

#define OCR_PWM_INIT_VALUE OCR0 = 0
/* Pb5 (port B 5) is set as output of the PWM generator*/
#define PWM_PIN_ON sbi(DDRB,DDB3)
#define PWM_PIN_OFF cbi(DDRB,DDB3)

/*********************************************************************************
*   Chip :     ATmega16
*
*   Section :  ADC
*
*
*   Purpose :  ADC RELATED DEFINITIONS AND MACROS
*
*
*********************************************************************************/


/* initialization of voltage soruce (VCP) for our Light Dependend Resistor (LDR) on PORTF3 */
#define VCP_INIT sbi(PORTC,PC0) | sbi(DDRC,DDC0)

/*Demuxer is set to PORTA1*/
#define DEMUXER_INIT ADMUX |= (1<<MUX0) | (1<<ADLAR)


/************************************************************************
*ADC prescaler settings: These bits determine the division factor between the XTAL frequency and the 
*input
*+clock to the ADC. 
***************************************************************************/

#define ADC_PRESCALER_INIT ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)
//#define ADC_PRESCALER_INIT ADCSRA |= (1<<ADEN)

//Turn ON the ADC
#define ADC_INIT ADCSRA |= (1<<ADEN)

//Warming up the ADC
#define ADC_SINGLE_START ADCSRA |= (1<<ADSC)

/*********************************************************************************
*   Chip :     ATmega16
*
*   Section :  UART
*
*
*   Purpose :  UART RELATED DEFINITIONS AND MACROS
*
*
*********************************************************************************/

// enable the usart double speed mode
//#define DOUBLE_SPEED_USART UCSRA |= (1<<U2X)

// enable the the UART reciver and transmitter
#define ENABLE_RX_TX UCSRB |= (1<<TXEN) | (1<<RXEN)

// set data frame to 8bit with 1 start bit and 2 stop bits
#define SET_DATA_FRAME UCSRC |= (1<<URSEL) | (1<<USBS) | (1<<UCSZ1) | (1<<UCSZ0)


//Clock speed of the system. You should set it according to the fuse bits settings.
#define FOSC 8000000

// BAUD RATE setting
//#define BAUD 9600
#define BAUD 19600

//THE UBRR rebister value calculation
#define MYUBRR FOSC/16/BAUD-1
//#define MYUBRR 12

// EVEN PARITY BIT Error checking
//#define EVEN_PARITY UCSRC |= (1<<URSEL) | (1<<UPM1)
// actually working without it
#define EVEN_PARITY

/**************************************************************************************
*   Chip :     ATmega16
*
*   Section :  GENERAL INTERRUPT TIMER SETUPne SET_DATA_FRAME UCSRC |= SETUP
*
*
*   Purpose :  CONTAINS MACROS AND DEFINITIONS WHICH ARE AFTERWARDS USED TO INITIALISE THE
*              GENERAL INTERRUPT NEEDED FOR THE REGULATION CYCLE
*
***************************************************************************************/

/*initialization of timer/counter 0A for Clear Timer on Compare (CTC) mode, using I/O timer with prescaler x64 */
//#define TIMER_ISR_INIT TCCR0A |= (1<<WGM01) | (1<<CS01) | (1<<CS00)

//TC
/*initialization of timer/counter 0A for Clear Timer on Compare (CTC) mode, using I/O timer with prescaler x256 --- refresh frequency is 0.0256s = 39,0625Hz*/
#define TIMER_ISR_INIT TCCR2 |= (1<<WGM21) | (1<<CS22) | (1<<CS21)
//EOTC

/* set interrupt (TIFR1,OCF1A) at compare match */
#define TIMER_ISR_TOP_INT_ENABLE TIMSK |= (1<<OCIE2)

/* set initial value of the OCR2 */
//#define OCR_ISR_INIT OCR2 = 156

//TC
#define OCR_ISR_INIT OCR2 = 250
//EOTC

/* defining the interrupt vector which will trigger the ISR interrupt function*/
#define TIMER_ISR_COMP_VECT TIMER2_COMP_vect

/*********************************************************************************
*   Chip :     ATmega16
*
*   Section :  DIGITAL OUTPUT
*
*
*   Purpose :  DIGITAL OUTPUT RELATED DEFINITIONS AND MACROS
*
*
*********************************************************************************/

#define DIGITAL_OUTPUT_SETUP DDRA |= (1<<DDA4) | (1<<DDA5) | (1<<DDA6) | (1<<DDA7)

#define DIGI_OUT0_ON sbi(PORTA,PORTA4)
#define DIGI_OUT0_OFF cbi(PORTA,PORTA4)

#define DIGI_OUT1_ON sbi(PORTA,PORTA5)
#define DIGI_OUT1_OFF cbi(PORTA,PORTA5)

#define DIGI_OUT2_ON sbi(PORTA,PORTA6)
#define DIGI_OUT2_OFF cbi(PORTA,PORTA6)

#define DIGI_OUT3_ON sbi(PORTA,PORTA7)
#define DIGI_OUT3_OFF cbi(PORTA,PORTA7)


/***********************************************************************************************


   ---   ATMega169   ---


************************************************************************************************/



#elif defined (__AVR_ATmega169__)

/*********************************************************************************
*   Chip :     ATmega169
*
*   Section :  PWM
*
*
*   Purpose :  PWM RELATED DEFINITIONS AND MACROS
*
*
*********************************************************************************/



/*initialization of timer/counter  for fast PWM modulation, using I/O timer without prescaler */

#define TIMER_PWM_INIT TCCR0A |=  (1<<WGM01) | (1<<WGM00) | (1<<COM0A1) | (1<<CS00)


/* PB4 (port B 4) is set as output of the PWM generator*/
#define PWM_PIN_ON sbi(DDRB,DDB4)
#define PWM_PIN_OFF cbi(DDRB,DDB4)

// parameter values
#define OCR_PWM_INIT_VALUE OCR0A = 0

// definition which would be used in the rest of the program
#define OCR_PWM OCR0A


/*********************************************************************************
*   Chip :     ATmega169
*
*   Section :  ADC
*
*
*   Purpose :  ADC RELATED DEFINITIONS AND MACROS
*
*
*********************************************************************************/


/* initialization of voltage soruce (VCP) for our Light Dependend Resistor (LDR) on PORTF3 */
#define VCP_INIT sbi(PORTF,PF3) | sbi(DDRF,DDF3)

/*Demuxer is set to PORTF1 and Left Aligned ADC result*/
#define DEMUXER_INIT ADMUX |= (1<<MUX1)  | (1<<ADLAR)


/************************************************************************
*ADC prescaler settings: These bits determine the division factor between the XTAL
*frequency and the
*input clock to the ADC.
***************************************************************************/

#define ADC_PRESCALER_INIT ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)

//Turn ON the ADC
#define ADC_INIT ADCSRA |= (1<<ADEN)


#define ADC_SINGLE_START ADCSRA |= (1<<ADSC)

/*********************************************************************************
*   Chip :     ATmega169
*
*   Section :  UART
*
*
*   Purpose :  UART RELATED DEFINITIONS AND MACROS
*
*
*********************************************************************************/

// enable the usart double speed mode
//#define DOUBLE_SPEED_USART UCSRA |= (1<<U2X)

// enable the the UART reciver and transmitter
#define ENABLE_RX_TX UCSRB |= (1<<TXEN) | (1<<RXEN)

// set data frame to 8bit with 1 start bit and 2 stop bits

#define SET_DATA_FRAME UCSRC |= (1<<USBS) | (1<<UCSZ1) | (1<<UCSZ0)

// EVEN PARITY BIT Error checking
//#define EVEN_PARITY UCSRC |= (1<<UPM1)
#define EVEN_PARITY

// CLOCK SPEED, system clock speed is 8MHZ

#define FOSC 8000000
//#define FOSC 1000000
// BAUD RATE setting
//#define BAUD 9600
#define BAUD 19200

//THE UBRR rebister value calculation
#define MYUBRR FOSC/16/BAUD-1
//#define MYUBRR 25



/**************************************************************************************
*   Chip :     ATmega169
*
*   Section :  GENERAL INTERRUPT TIMER SETUPne SET_DATA_FRAME UCSRC |= SETUP
*
*
*   Purpose :  CONTAINS MACROS AND DEFINITIONS WHICH ARE AFTERWARDS USED TO INITIALISE THE
*              GENERAL INTERRUPT NEEDED FOR THE REGULATION CYCLE
*
***************************************************************************************/



/*initialization of timer/counter 2A for Clear Timer on Compare (CTC) mode, using I/O timer with prescaler x256 --- refresh frequency is 0.0256s = 39,0625Hz if the OCR0A = 156*/
#define TIMER_ISR_INIT TCCR2A |= (1<<WGM21) | (1<<CS22) | (1<<CS21)


/* set interrupt (TIFR2,OCF2A) at compare match */
#define TIMER_ISR_TOP_INT_ENABLE TIMSK2 |= (1<<OCIE2A)
/* set initial value of the OCR1A */
#define OCR_ISR_INIT OCR2A = 156

//TC
//#define OCR_ISR_INIT OCR2A = 250
//EOTC

/* defining the interrupt vector which will trigger the ISR interrupt function*/
#define TIMER_ISR_COMP_VECT TIMER2_COMP_vect


/*********************************************************************************
*   Chip :     ATmega169
*
*   Section :  DIGITAL OUTPUT
*
*
*   Purpose :  DIGITAL OUTPUT RELATED DEFINITIONS AND MACROS
*
*
*********************************************************************************/

#define DIGITAL_OUTPUT_SETUP DDRD |= (1<<DDD0) | (1<<DDD1) | (1<<DDD2) | (1<<DDD3)

#define DIGI_OUT0_ON sbi(PORTD,PORTD0)
#define DIGI_OUT0_OFF cbi(PORTD,PORTD0)

#define DIGI_OUT1_ON sbi(PORTD,PORTD1)
#define DIGI_OUT1_OFF cbi(PORTD,PORTD1)

#define DIGI_OUT2_ON sbi(PORTD,PORTD2)
#define DIGI_OUT2_OFF cbi(PORTD,PORTD2)

#define DIGI_OUT3_ON sbi(PORTD,PORTD3)
#define DIGI_OUT3_OFF cbi(PORTD,PORTD3)



#else
   #error "Don't know what kind of MCU you are compiling for"
#endif
#endif
