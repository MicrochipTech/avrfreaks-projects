
/*********************************************************************************************************
 Title  :   C  include file for the rc channel decoder library (rc_channel.c)
 Author:    Chris Efstathiou 
 E-mail:    ........................
 Homepage:  ........................
 Date:      02/Feb/2006
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE.
*********************************************************************************************************/

/*
   COMMENTS
*/




/********************************************************************************************************/
/*                                   PREPROCESSOR DIRECTIVES                                            */
/********************************************************************************************************/

#include <avr/io.h>
#include <avr/signal.h>
#include "rc_channel.h"

#if  ((F_CPU * 60)/1000) < 0xFFFF

#define TIMER0_PRESCALER   1
#define TIMER0_PRESCALER_BITS   ((0<<CS02)|(0<<CS01)|(1<<CS00))
#warning PRESCALER SET TO 1

#elif  (((F_CPU * 60)/1000)/8) < 0xFFFF

#define TIMER0_PRESCALER   8
#define TIMER0_PRESCALER_BITS   ((0<<CS02)|(1<<CS01)|(0<<CS00))
#warning PRESCALER SET TO 8

#elif  (((F_CPU * 60000)/1000000)/64) < 0xFFFF

#define TIMER0_PRESCALER   64
#define TIMER0_PRESCALER_BITS   ((0<<CS02)|(1<<CS01)|(1<<CS00))
#warning PRESCALER SET TO 64

#endif


#define RC_NOMINAL_TIMER0_VALUE     ((((F_CPU/1000) * RC_NOMINAL_PULSE_WIDTH)/1000)/TIMER0_PRESCALER) 
#define RC_MIN_TIMER0_VALUE         ((((F_CPU/1000) * RC_MIN_PULSE_WIDTH)/1000)/TIMER0_PRESCALER)
#define RC_MAX_TIMER0_VALUE         ((((F_CPU/1000) * RC_MAX_PULSE_WIDTH)/1000)/TIMER0_PRESCALER)
#define RC_TIMEOUT                  (((((F_CPU/1000) * RC_PULSE_WIDTH_TIMEOUT)/1000)/TIMER0_PRESCALER)/0xFF)
#define RC_TIMER0_VALUE_CORRECTION  ((RC_NOMINAL_TIMER0_VALUE*100)/RC_NOMINAL_PULSE_WIDTH)

/********************************************************************************************************/
/*                                   TYPE DEFINITIONS                                                   */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   GLOBAL VARIABLES                                                   */
/********************************************************************************************************/
#if RC_PW_INT_DRIVEN == 0

volatile unsigned char timer0_high_byte=0;

#elif RC_PW_INT_DRIVEN == 1

volatile unsigned long   rc_pw_buffer1=0;
volatile unsigned short  rc_pw_buffer0=0;
volatile unsigned short  timer0_high_byte=0;
volatile unsigned char   rc_pw_start_timing=0;
volatile unsigned char   rc_pw_counter1=0;         
unsigned char            rc_pw_init = 0;

#endif

/********************************************************************************************************/
/*                                   LOCAL FUNCTION PROTOTYPES                                          */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   LOCAL FUNCTION DEFINITIONS                                         */
/********************************************************************************************************/






/********************************************************************************************************/
/*                                   PUBLIC FUNCTIONS                                                   */
/********************************************************************************************************/

#if RC_PW_MIN_CODE_SIZE == 0  &&  RC_PW_INT_DRIVEN == 0
unsigned short _rc_get_pw_(volatile unsigned char* port, unsigned char pin)
{
unsigned long timer0_value=0;
unsigned char counter1=0;

   /* Make the pin an input with no pull up resistor. */
   *(port-1) &= (~(1<<pin));
   *(port) &= (~(1<<pin));

   asm("cli");
   TCCR0=0; 
   TCNT0=0; 
   timer0_high_byte=0;

   /*Enable timero overflow interupt */
   TIMSK |= (1<<TOIE0);
   asm("sei");

/* Start the timer, prescaler set to 8 */
TCCR0 = TIMER0_PRESCALER_BITS;

/* Three concecutive valid samples are needed in order to confirm that the pin changed state. */
while(counter1 < RC_CHANNEL_SAMPLES)
  {
      /* loop_until_bit_is_clear */
      if( (*(port-2) & (1<<pin)) > 0  ) { counter1=0; } else{ counter1++; }

      if(timer0_high_byte >= RC_TIMEOUT){ return(0); }
      
  }

   /* We need to make the timer reset an atomic operation. */
   TCCR0 = 0;
   asm("cli");
   TCNT0=0; 
   timer0_high_byte=0;
   counter1=0;
   asm("sei");
   TCCR0 = TIMER0_PRESCALER_BITS;

/* Three concecutive valid samples are needed in order to confirm that the pin changed state. */   
while(counter1 < RC_CHANNEL_SAMPLES)
  {
      /* loop_until_bit_is_set */
      if( (*(port-2) & (1<<pin)) == 0  ) { counter1=0; } else{ counter1++; }

      if(timer0_high_byte >= RC_TIMEOUT){ return(0); }
  } 
   
   /* Reset the timer so we can start measuring the pulse's width. The timer is already started. */
   /* We need to make the timer reset an atomic operation. */
   TCCR0 = 0;
   asm("cli");
   TCNT0=0; 
   timer0_high_byte=0;
   counter1=0;
   asm("sei");
   TCCR0 = TIMER0_PRESCALER_BITS;
   
/* Three concecutive valid samples are needed in order to confirm that the pin changed state. */
while(counter1 < RC_CHANNEL_SAMPLES)
  {
      /* loop_until_bit_is_clear */
      if( (*(port-2) & (1<<pin)) > 0  ) { counter1=0; } else{ counter1++; }

      if(timer0_high_byte >= RC_TIMEOUT){ return(0); }
      
  }


   /*Stop the timer */
   TCCR0=0;  
   timer0_value = timer0_high_byte;
   timer0_value = (timer0_value<<8);
   timer0_value += TCNT0;
   timer0_value = (timer0_value * 100)/RC_TIMER0_VALUE_CORRECTION; 
    
   
return((unsigned short)timer0_value);   
}  

#elif RC_PW_MIN_CODE_SIZE == 1  &&  RC_PW_INT_DRIVEN == 0

unsigned short _rc_get_pw_(volatile unsigned char* port, unsigned char pin)
{
unsigned short timer0_value=0;

   /* Make the pin an input with no pull up resistor. */
   *(port-1) &= (~(1<<pin));
   *(port) &= (~(1<<pin));

   TCCR0=0; 
   TCNT0=0; 
   timer0_high_byte=0;

   /*Enable timero overflow interupt */
   TIMSK |= (1<<TOIE0);
   asm("sei");

   /* loop_until_bit_is_clear, "port-2" = PINx register. */
   while( (*(port-2) & (1<<pin)) > 0  );

   /* loop_until_bit_is_set, "port-2" = PINx register. */
   while( (*(port-2) & (1<<pin)) == 0  );

   /* Start the timer, prescaler set to 8 */
   TCCR0 = ((0<<CS02)|(1<<CS01)|(0<<CS00));

   /* loop_until_bit_is_clear, "port-2" = PINx register. */
   while( (*(port-2) & (1<<pin)) != 0  ); 

   /*Stop the timer */
   TCCR0=0;  
   timer0_value = timer0_high_byte;
   timer0_value = (timer0_value<<8);
   timer0_value += TCNT0;
    
   
return(timer0_value);   
}  

#elif RC_PW_INT_DRIVEN == 1

unsigned short rc_get_pw(void)
{

unsigned short pulse_width_us=0;

   if(rc_pw_init == 0)
    {
        asm("cli");
        DDRB = (~(1<<RC_PW_INT_PIN));
        PORTB = (~(1<<RC_PW_INT_PIN));
        TCCR0=0; 
        TCNT0=0; 
        timer0_high_byte=0;

        /*Enable timero overflow interupt */
        TIMSK |= (1<<TOIE0);

        /* Enable the External interupt 0 */
        GIMSK |= (1<<INT0);

        /* Make the external interupt 0 trigger with any level change. */
        MCUCR |= ((0<<ISC01)|(1<<ISC00)); 

        asm("sei");
        rc_pw_init = 1;
    }    

/* Just to make sure that the ISRs dont mess up the returned value. */
asm("cli");

/* If the correction factor is 100 then there is no need for further calculations. */
#if RC_TIMER0_VALUE_CORRECTION != 100
rc_pw_buffer1 = (rc_pw_buffer1 * 100)/RC_TIMER0_VALUE_CORRECTION; 
#endif

pulse_width_us = (unsigned short)rc_pw_buffer1;
asm("sei");

return(pulse_width_us);   
}  



#endif

/********************************************************************************************************/
/*                                   INTERRUPT SERVICE ROUTINES                                         */
/********************************************************************************************************/
SIGNAL(SIG_OVERFLOW0)
{
timer0_high_byte++;
  

return;
}  

#if RC_PW_INT_DRIVEN == 1

SIGNAL(SIG_INTERRUPT0)
{

    if( bit_is_set(RC_PW_INT_PIN_REG, RC_PW_INT_PIN) )
     { 
         TCCR0 = 0;
         TCNT0=0;
         timer0_high_byte=0;
         TCCR0 = TIMER0_PRESCALER_BITS;
         rc_pw_start_timing=1;  
     }    
    else
       { 
           if(rc_pw_start_timing == 1)
            {
               TCCR0 = 0;
               rc_pw_start_timing=0;
               timer0_high_byte = (timer0_high_byte<<8);
               timer0_high_byte += TCNT0;
               if(timer0_high_byte >= RC_MIN_TIMER0_VALUE && timer0_high_byte <= RC_MAX_TIMER0_VALUE)
                {
                    rc_pw_buffer0 += timer0_high_byte;
                    rc_pw_counter1++;
                }  
               if(rc_pw_counter1 == RC_PW_AVERAGING_SAMPLES)                  
                {
                    rc_pw_buffer1 = (rc_pw_buffer0 / RC_PW_AVERAGING_SAMPLES);
                    rc_pw_buffer0 = 0; 
                    rc_pw_counter1 = 0;
                } 
            }
       }     



return;
}  

#endif


/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

