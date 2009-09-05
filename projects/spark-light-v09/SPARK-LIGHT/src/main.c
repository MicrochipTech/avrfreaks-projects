/***************************************************************************
* main.c -- SPARK-LIGHT bike led light
* OSC = 4.8 MHz (internal RC)
* Author: Yuri Nikiforov
* Started: 31 may 2009
* Last change: 24 aug 2009
* Edited & compiled with IAR EW 5.20, debugged with AVR Studio 4.17 +Dragon
***************************************************************************/

#include "main.h"

/*
TODO:
- test debouncing (INT0 handler)
- redo debouncing using polling from TIMER0 and event counting
NOTES:
Various test data for x4mer Spark-Light LED controller
Idle power comsumption @6V		=	30uA
MCU power consumption  @6V		=	2.5mA
Peak LED current       @6V		=	430mA
Average shunt voltage drop 300/200/100/50 =     0.15/0.1/0.05/0.025V
MOSFET forward voltage drop 		=	0.17V
*/

/*
Hardware used:
ADC     -- LED current measure
INT0    -- Button operation, change mode and wake-up
TIMER0  -- PWM + ADC bootstrapping on each PWM cycle
*/


// Globals
__no_init volatile unsigned char mode;
unsigned char adcv[MLAST+1]  = {0,ADC_M1,ADC_M2,ADC_M3,ADC_M4,ADC_M5};  // shunt voltage measured by ADC
unsigned int BlinkTimer = BlinkTimeout;
unsigned char level = 0;  
unsigned int LEDlevel = 0;

// Functions
void init(void);
void PowerDown(void);


/***************************************************************************
*                I N T E R R U P T   H A N D L E R S
***************************************************************************/
////////////////////////////////////////////////////////////////////////////
// INT0 - wake-up or mode change
#pragma vector=INT0_vect
__interrupt void INT0_int(void)
{
  // switch mode
  OCR0A = 0;                   // Power to zero
  __delay_cycles(0.1*FOSC);    // debouncing delay 0.1s
  if (++mode>MLAST) 
  {
    mode = 0; // switch mode
    while((PINB&(1<<pBTN))==0); // wait for button to release
    __delay_cycles(0.5*FOSC);   // additional debouncing delay 0.5s
  }
  GIFR = 0;                     // clear interrupt flags (debouncing)
}


////////////////////////////////////////////////////////////////////////////
// TMR0_OVF - start ADC conversion each cycle
#pragma vector=TIM0_OVF_vect
__interrupt void TIM0_OVF_int(void)
{
  // check ADSC
  // ADSC will read as one as long as a conversion is in progress.
  // When the conversion is complete, it returns to zero.
  if ((ADCSRA&(1<<ADSC))==0) ADCSRA|=(1<<ADSC);
}


////////////////////////////////////////////////////////////////////////////
// ADC - Adjust PWM value to stabilize LED current, blinking also done here
#pragma vector=ADC_vect
__interrupt void ADC_int(void)
{
  // manage blink timeout here
  if (mode==MBLINK)
  {
    if (--BlinkTimer==0)
    {
      BlinkTimer=BlinkTimeout;
      if (level == adcv[mode]) level = 0;
      else level = adcv[mode];
    }/* BlinkTimer */
  }
  else level = adcv[mode];
  
  // stabilize voltage on shunt at needed level
  LEDlevel = ADC;
  if (((unsigned char)(LEDlevel>>8))!=0) 
  {
    OCR0A = 0;
    return;
  }
  if ((unsigned char)(LEDlevel)==level) return; // perfect level
  if ((unsigned char)(LEDlevel)>level) 
  {
    if (OCR0A != 0) OCR0A--; // take a little lower, but prevent ovelapping
  }
  else 
  {
    if (OCR0A != 0xFF) OCR0A++; // take a little higher, but prevent ovelapping
  }
}



/***************************************************************************
*                                S U B S
***************************************************************************/
////////////////////////////////////////////////////////////////////////////
// initialize and tune-up peripherials
void init(void)
{
  __disable_interrupt();
  
  /* Globals */  
  // maybe some debouncing vars here
  
  /* Ports setup */
  DDRB = 1<<pPWM;
  PORTB = 1<<pBTN;  // pull-up on BTN pin
  
  /* ADC Setup */
  DIDR0 = (1<<ADC1D);
  ADMUX = (1<<REFS0)|(1<<MUX0); // set input channel ADC1, 10-bit, Vref = 1.1 V internal
  // set prescaller to FOSC/32 = 150 kHz, manual triggering, enable interrupt
  ADCSRA = (1<<ADIE)|(1<<ADEN)|(1<<ADPS2)|(1<<ADPS0);
  ADCSRA |= (1<<ADSC);  // start first conversion
  
  /* INT0 setup */
  MCUCR = (1<<ISC01); // interrupt on INT0 pin falling edge (button pressed)
  GIMSK = (1<<INT0);
  
  /* TC0 setup */
  TCCR0A = (1<<COM0A1)|(1<<WGM00);
  TCCR0B = (1<<CS00); // 9375 Hz PWM
  TIMSK0 = (1<<TOIE0); // enable overflow interrupt
  
  OCR0A = 0; // zero output level
  TIFR0 = 0; // clear timer flags
  
  /* AC setup */
  ACSR = (1<<ACD); // Analog comparator disable
  
  GIFR = 0;
  __enable_interrupt();
}


////////////////////////////////////////////////////////////////////////////
// PowerDown - Shut down all periferials
void inline PowerDown(void)
{
  OCR0A = 0; // zero output level
  OCR0B = 0; // zero output level
  TCCR0A = 0; // clear timer mode
  TIMSK0 = 0; // disable timer interrupts
  TCCR0B = 0; // stop timer 
  ADMUX = 0;
  ADCSRA = 0;
  ADCSRB = 0;
  GIMSK = (1<<INT0);  // enable INT0
  GIFR = 0;
  __enable_interrupt();
  // interrupt on INT0 pin low level
  MCUCR = (1<<SE)|(1<<SM1); // set Power-down sleep mode and enable sleep option
  __sleep();      // enter sleep mode
  //////////////////////// after wake-up ////////////////////////
  __no_operation();
  __disable_interrupt();
  mode = 1;
  GIFR = 0;
  __asm("ldi r30,0 \n ldi r31,0 \n ijmp"); // reset all
  /* power-down 30 uA at 5.5V battery power */
}


/***************************************************************************
*   M A I N  -  P R O G R A M   E X E C U T I O N   S T A R T S   H E R E  *
***************************************************************************/
__task void main(void)
{
  if (mode>MLAST) mode=0;         // set initial mode on power-up
//  if (mode==MSLEEP) PowerDown();  // is it needed ?
  init();
  for (;;)
  {
    if (mode==MSLEEP) PowerDown();
    MCUCR = (1<<SE)|(1<<ISC01); // set Idle sleep mode and enable sleep option
    __sleep();      // enter sleep mode  
  }
};

