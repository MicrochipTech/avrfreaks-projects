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
/*  Analog to digital conversion code.
*/

#include "adc.h"
#include "avr.h"
#include "bridge.h"
#include "uart.h"
#include <avr/io.h>
#include ATMEGA
#include <interrupt.h>
#include <inttypes.h>
#include "common.h"
#include <sig-avr.h>

// use 2.5v internal reference + (1<<REFS0)|(1<<REFS1)
// ADLAR left adjust results
#ifdef MONORAIL
#define VOLTAGE_REF	((1<<REFS0)|(1<<REFS1))
#endif
#ifdef WELDING
#define VOLTAGE_REF	(_BV(REFS0))
#endif

#define VOLTAGE_TIME	0x07
#define ANALOG_PORT	PORTA
#define ANALOG_PORT_DIR	DDRA
#define ADC_IRQ YES
#define ADC_CLOCK
volatile uint8_t adc_state;
// global adc verables

short adcnt;
volatile uint16_t		adc_battery;
volatile uint16_t		adc_temperature;

#define CAPMAX 32
struct capd{
      uint16_t v;
};
//volatile struct capd cap[CAPMAX];
//volatile uint8_t cappos;
//uint8_t roll=0;

void   init_adc( void )
{
   //   PORTA&= ~0x00;
   DDRA&= ~0x00;
   DDRA=(uint8_t)0x00;  // port a is input only for adc converter

/* Ensure that our port is for input (inputs bits 0-3 */
/* Select voltage ref */
   ADMUX=VOLTAGE_REF;
   // outp( VOLTAGE_TIME, ADCSRA );
#if CLOCK>=16000000
   ADCSRA=ADC_128;    // 125khz for 16mhz clock
#elif CLOCK>=8000000
   ADCSRA=ADC_64;    // 125khz for 8mhz clock
#elif CLOCK>=4000000
   ADCSRA=ADC_32;    // 125khz for 4mhz clock
#elif CLOCK>=2000000
   ADCSRA=ADC_16;    // 125khz for 4mhz clock
#elif CLOCK>=1000000
   ADCSRA=ADC_8;    // 125khz for 1mhz clock
#endif
   // enable ADC circuitry
   ADCSRA|=_BV(ADEN );

/* Turn off the analog comparator */
   sbi( ACSR, ACD );


#ifdef ADC_IRQ
/* Enable our interrupt, if we have interrupts turned on */
   sbi( ADCSRA, ADIE );
#endif

#ifdef MONORAIL
/* adc table init */
   adc_hook=0;
   adc_tab[0].next=&adc_tab[0];
   adc_tab[0].opera=0;
   adc_pnt=&adc_tab[0];

   ADCSRA|=_BV(ADATE);  // adc will trigger on source
   SFIOR&=~ADTS_MASK;   // clean
   SFIOR|=ADTS_CMB;     //set to trigger on 1B compare match
   PORTA=(uint8_t) 0x00;  // disable port A internal pullup resistors
#endif
#ifdef WELDING
   PORTA&=~(_BV(7)|_BV(1)|_BV(6));
   // disable internal pullup resistors only for the ADC inputs used in welding

   adc_state=ADC_WELD_I;
   ADCSRA&=~_BV(ADATE);  // disable auto trigger
   ADCSRA|=_BV(ADSC );
   // SFIOR&=~ADTS_MASK; 			// free run
   // SFIOR|=ADTS_FREERUN; 		// free run
#endif
   adc_task();  // kick start the adc conversions
}

const uint16_t NODATA=2000;

/*
   This simple code is for the spot welding application
   ony 3 conversion needed a room temperature , welding current
   and welding electrode voltage, once they are captured
   the welding control loop is called to process them. 
*/
#ifdef WELDING
volatile uint16_t		adc_volt;
volatile uint16_t		adc_i;
volatile uint16_t		adc_irrx;
void adc_task()
{
   // adc interupt is low priority not a time critical task
   // store or process the data from the ADC
   // depending on the adc_state the adc_was requested to sample for a particular reason
   // the data obtained is processed accordingley
   // was static inline void  read_adc( void )
   // next setup next adcx conversion from the linked list
   uint16_t add=ADCW;
   switch(adc_state)
   {
      case ADC_WELD_IRRX:
	 adc_irrx=ADCW;
	 weldlink(adc_volt,adc_i,adc_irrx);
	 adc_state=ADC_WELD_V;
	 ADMUX = ADMUX_ADC6 | VOLTAGE_REF;  // setup voltage
	 break;
      case ADC_WELD_V:
	 adc_volt=ADCW;
	 ADMUX = ADMUX_ADC7 | VOLTAGE_REF;  // setup current
	 adc_state=ADC_WELD_I;
	 break;
      case ADC_WELD_I:
	 adc_i=ADCW;
	 adc_state=ADC_WELD_IRRX;
	 ADMUX = ADMUX_ADC1 | VOLTAGE_REF;  // setup irrx
	 break;
      default:
	 adc_state=ADC_WELD_V;
	 break;
   }
   //   clear the trigger signal
/* enable the adc interrupt again */
   // sbi( ADCSRA, ADIE );
/* start the conversion */
   ADCSRA|=_BV(ADSC);
}

//INTERRUPT( SIG_ADC )          // interrupt can be interrupted
SIGNAL( SIG_ADC )           // signal is uninterruptable
{
   adc_task();
}
#endif

/*
   This simple code is for the process timer thermostat and clock application
   ony 1 conversion needed a temperature 
*/
#ifdef TIMER
volatile uint16_t		adc_temperature;
void adc_task()
{
   uint16_t add=ADCW;
   timer_loop(adc_temperature);
   ADMUX = ADMUX_ADC6 | VOLTAGE_REF;  // setup for next temperature measurment

   //   clear the trigger signal
/* enable the adc interrupt again */
   // sbi( ADCSRA, ADIE );
/* start the conversion */
   ADCSRA|=_BV(ADSC);
}

SIGNAL( SIG_ADC )           // signal is uninterruptable
{
   adc_task();
}
#endif

/*
   This linked list based adc conversion routine is Called when the voltage
   conversion is finished, and sets up the next conversion in the list.
*/
#ifdef MONORAIL
volatile struct adc_op adc_tab[ADC_MAX];
volatile struct adc_op *adc_pnt;

unsigned char adc_tail;
unsigned char adc_hook;
unsigned char adc_head;
char buffer;
void adc_task()
{
   // adc interrput is low priority not a time critical task
   // store or process the data from the ADC
   // depending on the adc_state the adc_was requested to sample for a particular reason
   // the data obtained is processed accordingley
   // was static inline void  read_adc( void )
   // next setup next adcx conversion from the linked list

   uint16_t add=ADCW;
   switch(adc_state)
   {
      struct hb *q=&hbq;
      struct hb *u=&hbu;
      case ADC_BRQ_1V:
	 q->voltage1=ADCW;
	 break;
      case ADC_BRQ_1I:
	 q->current1=ADCW;
	 break;
      case ADC_BRQ_2V:
	 q->voltage2=ADCW;
	 break;
      case ADC_BRQ_2I:
	 q->current2=ADCW;
	 break;
      case ADC_BRU_1V:
	 u->voltage1=ADCW;
	 break;
      case ADC_BRU_2V:
	 u->voltage2=ADCW;
	 break;
      case ADC_BRU_1I:
	 u->current1=ADCW;
	 break;
      case ADC_BRU_2I:
	 u->current2=ADCW;
	 break;
      case ADC_TEMPERATURE:
	 adc_temperature=ADCW;
	 break;
      default:
	 break;
   }

   // check if we have completed the last adc conversion in the ring
   // if so call the sensorless motor feedback routine
   if(adc_pnt->opera & _BV(7) ){  // check for all completed tag
				  motor_fb();
			       }
   // setup for the next H-bridge conversion in the list
   adc_pnt=adc_pnt->next;     // step through adc conversion linked list

   adc_set(adc_pnt->opera & ~_BV(7) );   // setup next conversion
   cli();
   // setup H bridge triggered ADC conversion
   // this is atomic must disable interrupts here momentarrilly to prevent
   // glitches
   // I am assuming Timer 1 is running at 8x the clock rate of timer 2
   // ADC is triggered at OCR1B
   uint16_t trig=adc_pnt->count;
   if(adc_pnt->count>TCNT2){
			      trig-=TCNT2;  // get time displacement in timer 2 clock ticks to next conversion
			   }
   else
   {
      trig+=256-TCNT2;
   }
   trig*=8;  // convert timer 2 clock ticks into timer 1 clock ticks
   OCR1B = TCNT1+trig;  // set compare trigger time
   sei();
   //   clear the trigger signal
   TIFR|=_BV(OCF1B);
   PORTD&=~_BV(4);
   // done now the adc needs to be setup to trigger on timer B comparator match
}

/*
   if( cappos<CAPMAX){
   //			cap[cappos].id=roll;
   cap[cappos].v=add;
   cappos++;
   roll+=8;
   }
*/

/* request a setup of the adc to sample something
   * returns 0 if ADC is already being used the callers should
   * use the token-request system to prevent adc contention. 
*/
uint8_t adc_set(uint8_t state)
{
   // if(adc_state!=0)return 0;
   adc_state=state;  // setup for the next conversion
   switch(adc_state)
   {
      case ADC_BRQ_1V:
      case ADC_BRQ_1I:
	 //SFIOR=(SFIOR&~ADTS_MASK) | (ADTS_FREERUN&ADTS_MASK);  // to be sure
	 ADMUX = ADMUX_ADC0 | VOLTAGE_REF;  // qbridge side 1
	 break;
      case ADC_BRQ_2V:
      case ADC_BRQ_2I:
	 ADMUX = ADMUX_ADC1 | VOLTAGE_REF;  // qbridge side 2
	 break;
      case ADC_BRU_1I:
      case ADC_BRU_1V:
	 ADMUX = ADMUX_ADC2 | VOLTAGE_REF;
	 break;
      case ADC_BRU_2I:
      case ADC_BRU_2V:
	 ADMUX = ADMUX_ADC3 | VOLTAGE_REF;
	 break;
      case ADC_POT1:
	 ADMUX = ADMUX_ADC5 | VOLTAGE_REF;
	 break;
      case ADC_POT2:
	 ADMUX = ADMUX_ADC6 | VOLTAGE_REF;
	 break;
      case ADC_POT3:
	 ADMUX = ADMUX_ADC7 | VOLTAGE_REF;
	 break;
      case ADC_TEMPERATURE:
      case ADC_VCC:
	 ADMUX = ADMUX_ADC4 | VOLTAGE_REF;
	 break;
      default:
	 break;
   }
/* enable the adc interrupt again */
   sbi( ADCSRA, ADIE );
/* start the conversion */
   ADCSRA|=_BV(ADSC );
   return state;
}

INTERRUPT( SIG_ADC )
{
   adc_task();
}
#endif

/*
   *  If ADC interrupts are not enabled, we don't bother compiling
   * this routine.
*/
/*
   * If interrupts are enabled, then this is a NOP.  Otherwise
   * we check for conversion finished and read the sample.
*/
#ifndef ADC_IRQ
void   adc_task( void )
{
   if( bit_is_set( ADCSRA, ADIF ) )
      adc_read();
}
#endif

adc_print()
{
   char ct;
   put_cr();
   puts("rollin");
/*
      for(ct=0;ct<CAPMAX;ct++){
      putc('[');
      put_i16(ct*8);
      putc('\t');
      put_i16(cap[ct].v);
      putc('\t');
      //			      put_i16(cap[ct].t2);
      //		      putc(',');
      //	      put_i16(cap[ct].com);
      //      putc(',');
      putc(']');
      put_cr();
      }
      cappos=0;
      roll=0;
      put_cr();

      puts("c=");
      put_i16(adcnt);
      uint8_t x,y,w;
*/

/*
      puts(" bat=");
      put_i16(adc_battery);
      puts(" tmp=");
      put_i16(adc_temperature);
      puts(" state=");
      put_i16(adc_state);
      put_cr();
      puts("\tv1");
      puts("\ti1");
      puts("\tv2");
      puts("\ti2");
*/
   put_cr();
#ifdef MONORAIL
/*
      puts("u=\t");   put_i16(uvoltage1);
      putc('\t');   put_i16(ucurrent1);
      putc('\t');   put_i16(uvoltage2);
      putc('\t');   put_i16(ucurrent2);
      putc('\t');   put_i16(ucurrent2);
*/
#endif
   show_adc_tab();
}

// routine to send a set hexidecimal format adc feedback to the uart
void adc_data(void )
{

#ifdef MONORAIL
/*
      struct hb *q=&hbq;
      struct hb *u=&hbu;
      hexputw(q->voltage1);
      hexputw(q->current1);
      hexputw(q->voltage2);
      hexputw(q->current2);

      hexputw(u->voltage1);
      hexputw(u->current1);
      hexputw(u->voltage2);
      hexputw(u->current2);
*/
#endif
   put_cr();
}

void show_adc_tab()
{
#ifdef MONORAIL
   puts("hook=");
   put_i16(adc_hook);
   puts(" now=");
   put_i16(adc_pnt);
   puts(" hed=");
   put_i16(adc_head);
   put_cr();
   struct adc_op *ct=&adc_tab[adc_hook];
   struct adc_op *start=ct->next;  // start from the beginning
   ct=ct->next;  // start from the beginning
   do{
	put_i16(ct);
	putc('[');
	put_i16(ct->count);
	putc('\t');
	put_i16(ct->opera);
	putc('\t');
	putc(']');
	if(adc_pnt==ct)putc('*');
	put_cr();
	ct=ct->next;
     } while(ct!=start);
#endif
}

