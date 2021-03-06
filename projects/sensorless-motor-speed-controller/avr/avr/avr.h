/* copyright
 * (c) 2004 Stephen Humble stephenhumble at telstra.com
 *
 *  This file is part of robotx see http://Robotx.sourceforge.net
 *  dual sensorless motor driver firmware.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
*/
// some handy enumerated timer settings
// some multi bit pattern enumerated register settings

#ifndef _AVR_H_
#define _AVR_H_

#include <sig-avr.h>
#include <interrupt.h>
#include <inttypes.h>
#include <io.h>

// some common usart modes for the UCSRC register
enum {
	USART_8E1=_BV(URSEL) | _BV(UPM1) | _BV(UCSZ0) | _BV(UCSZ1), // even parity 8 data asynchronous
	   USART_8N2= _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1) | _BV(USBS), // no parity 8 data 2 stop asynchronous
	   USART_8N1= _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1), // no parity 8 data 1 stop asynchronous
	   USART_7N1= _BV(URSEL) | _BV(UCSZ1) | _BV(USBS), // no parity 7 data 2 stop asynchronous
	   USART_7N2= _BV(URSEL) | _BV(UCSZ1) //  7 data no parity async
     };


// adc clock rate generator for the  ADCSR register

enum {  ADC_1=(0)
	   ,ADC_2=((1<<ADPS0))
	   ,ADC_4=( (1<<ADPS1))
	   ,ADC_8=((1<<ADPS0) | (1<<ADPS1))
	   ,ADC_16=((1<<ADPS2))
	   ,ADC_32=((1<<ADPS0) | (1<<ADPS2))
	   ,ADC_64=( (1<<ADPS1) | (1<<ADPS2))
	   ,ADC_128=((1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2))
     };

// adc mux settings for ADMUX register
enum {  ADMUX_ADC0=(0),
	 ADMUX_ADC1=(1),
	 ADMUX_ADC2=(2),
	 ADMUX_ADC3=(3),
	 ADMUX_ADC4=(4),
	 ADMUX_ADC5=(5),
	 ADMUX_ADC6=(6),
	 ADMUX_ADC7=(7),
         ADMUX_ADC0_ADC0_10x=(8),
         ADMUX_ADC0_ADC1_10x=(9),
         ADMUX_ADC0_ADC0_200x=(10),
         ADMUX_ADC0_ADC1_200x=(11),
         ADMUX_ADC2_ADC2_10x=(12),
         ADMUX_ADC2_ADC3_10x=(13),
         ADMUX_ADC2_ADC2_200x=(14),
         ADMUX_ADC2_ADC3_200x=(15),
         ADMUX_ADC0_ADC1=(16),
         ADMUX_ADC1_ADC1=(17),
         ADMUX_ADC2_ADC1=(18),
         ADMUX_ADC3_ADC1=(19),
         ADMUX_ADC4_ADC1=(20),
         ADMUX_ADC5_ADC1=(21),
         ADMUX_ADC6_ADC1=(22),
         ADMUX_ADC7_ADC1=(23),

         ADMUX_ADC0_ADC2=(24),
         ADMUX_ADC1_ADC2=(25),
         ADMUX_ADC2_ADC2=(26),
         ADMUX_ADC3_ADC2=(27),
         ADMUX_ADC4_ADC2=(28),
         ADMUX_ADC5_ADC2=(29),
         ADMUX_ADC6_ADC2=(30),
         ADMUX_ADC7_ADC2=(31)
     };

// timer 0 clock rate generator
enum {
	T0_CKSTOP=(0)
	   ,T0_CK1=((1<<CS00))
	   ,T0_CK8=((1<<CS01))
	   ,T0_CK64=((1<<CS00) | (1<<CS01))
	   ,T0_CK256=((1<<CS02))
	   ,T0_CK1024=((1<<CS02)|(1<<CS00))
	   ,T0_CKEXTN=((1<<CS02)|(1<<CS01))
	   ,T0_CKEXTP=((1<<CS02)|(1<<CS00) | (1<<CS01))
     };

     // timer 0 ctc operational modes
enum{
	T0_CTC=_BV(WGM01)
	,T0_CTCT=_BV(WGM01) | _BV(COM00)
	,T0_CTC0=_BV(WGM01) | _BV(COM01)
	,T0_CTC1=_BV(WGM01) | _BV(COM00) | _BV(COM01)
};

// timer 1 clock rate generator
enum {
	T1_CKSTOP=(0)
	   ,T1_CK1=((1<<CS10))
	   ,T1_CK8=( (1<<CS11))
	   ,T1_CK64=((1<<CS10) | (1<<CS11))
	   ,T1_CK256=((1<<CS12))
	   ,T1_CK1024=((1<<CS12)|(1<<CS10))
	   ,T1_CKT1FALL=((1<<CS12)|(1<<CS11))
	   ,T1_CKT1RISE=((1<<CS12)|(1<<CS10) | (1<<CS11))
     };

// timer 2 clock rate generator
enum {
	T2_CKSTOP=(0)
	   ,T2_CK1=((1<<CS20))
	   ,T2_CK8=((1<<CS21))
	   ,T2_CK32=((1<<CS20) | (1<<CS21))
	   ,T2_CK64=((1<<CS22))
	   ,T2_CK128=((1<<CS22)|(1<<CS20))
	   ,T2_CK256=((1<<CS22)|(1<<CS21))
	   ,T2_CK1024=((1<<CS22)|(1<<CS20) | (1<<CS21))
     };

#endif
