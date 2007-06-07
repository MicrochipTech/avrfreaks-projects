
/*********************************************************************************************************
 Title  :   C  main file for the ................................. (_________.c)
 Author:    ........................ 
 E-mail:    ........................
 Homepage:  ........................
 Date:      20/Aug/2002
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
#include "delay.h"

#define LEFT_LIGHT_PIN    PA7
#define RIGHT_LIGHT_PIN   PA6
#define BACK_LIGHT_PIN    PA5
#define CENTER_LIGHT_PIN  PA4
#define FRONT_LIGHT1_PIN  PA3
#define FRONT_LIGHT2_PIN  PA2
#define FRONT_LIGHT3_PIN  PA1
#define FRONT_LIGHT4_PIN  PA0

#define LEFT_LIGHT_ON()    {PORTA |= (1<<LEFT_LIGHT_PIN);}
#define RIGHT_LIGHT_ON()   {PORTA |= (1<<RIGHT_LIGHT_PIN);}
#define BACK_LIGHT_ON()    {PORTA |= (1<<BACK_LIGHT_PIN);}
#define CENTER_LIGHT_ON()  {PORTA |= (1<<CENTER_LIGHT_PIN);}
#define FRONT_LIGHTS_ON()  {PORTA |= ((1<<FRONT_LIGHT1_PIN)|(1<<FRONT_LIGHT2_PIN)|(1<<FRONT_LIGHT3_PIN) \
                                   |(1<<FRONT_LIGHT4_PIN));}

#define LEFT_LIGHT_OFF()    {PORTA &= (~(1<<LEFT_LIGHT_PIN));}
#define RIGHT_LIGHT_OFF()   {PORTA &= (~(1<<RIGHT_LIGHT_PIN));}
#define BACK_LIGHT_OFF()    {PORTA &= (~(1<<BACK_LIGHT_PIN));}
#define CENTER_LIGHT_OFF()  {PORTA &= (~(1<<CENTER_LIGHT_PIN));}
#define FRONT_LIGHTS_OFF()  {PORTA &= (~((1<<FRONT_LIGHT1_PIN)|(1<<FRONT_LIGHT2_PIN)|(1<<FRONT_LIGHT3_PIN) \
                                   |(1<<FRONT_LIGHT4_PIN)));}

#define ALL_LIGHTS_ON()   {LEFT_LIGHT_ON(); RIGHT_LIGHT_ON(); BACK_LIGHT_ON(); CENTER_LIGHT_ON(); \
                           FRONT_LIGHTS_ON();}
#define ALL_LIGHTS_OFF()  {LEFT_LIGHT_OFF(); RIGHT_LIGHT_OFF(); BACK_LIGHT_OFF(); CENTER_LIGHT_OFF(); \
                           FRONT_LIGHTS_OFF();}                                                                                                



/********************************************************************************************************/
/*                                   TYPE DEFINITIONS                                                   */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   GLOBAL VARIABLES                                                   */
/********************************************************************************************************/
unsigned char  timer0_high_byte=0;
unsigned short rc_pulse_width_buffer=0;



/********************************************************************************************************/
/*                                   LOCAL FUNCTION PROTOTYPES                                          */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   LOCAL FUNCTION DEFINITIONS                                         */
/********************************************************************************************************/
unsigned short rc_get_width(void)
{
unsigned short timer0_value=0;
unsigned char counter1=0;

rc_pulse_width_buffer=1000;

   TCCR0=0; 
   TCNT0=0;
   timer0_high_byte=0;

   /*Enable timero overflow interupt */
   TIMSK |= (1<<TOIE0);
   asm("sei");

   loop_until_bit_is_clear(PINB,PB6);
   loop_until_bit_is_set(PINB,PB6);
   /* Start the timer, prescaler set to 8 */
   TCCR0 = ((0<<CS02)|(1<<CS01)|(0<<CS00));
   loop_until_bit_is_clear(PINB,PB6); 
   /*Stop the timer */
   TCCR0=0; 
   timer0_value = timer0_high_byte;
   timer0_value = (timer0_value<<8);
   timer0_value += TCNT0;
    
   if(timer0_value <=800 || timer0_value >= 2200  )
    {
       for(counter1=0; counter1<3; counter1++)
         {
            ALL_LIGHTS_ON();
            DELAY_MS(100);
            ALL_LIGHTS_OFF();
            DELAY_MS(100);
         }  
  
   return(rc_pulse_width_buffer);

 }
   
   rc_pulse_width_buffer = timer0_value;  
   
return(timer0_value);   
}  

/********************************************************************************************************/
/*                                   INTERRUPT SERVICE ROUTINES                                         */
/********************************************************************************************************/
SIGNAL(SIG_OVERFLOW0)
{
timer0_high_byte++;
  

return;
}  


/********************************************************************************************************/
/*                                   MAIN FUNCTION                                                      */
/********************************************************************************************************/
void main(void)
{
unsigned short pulse_width_us=0;
/* Initialization */

/* PORTA is made an output */
DDRA=0xFF;
PORTA=0;

/* PORTB is made an input */
DDRB=0;
PORTB=0;

/* INT0 pin is made an input with pull up resistor on */
DDRB &= (~(1<<PB6)); 
PORTB |= (1<<PB6); 
TCNT0=0;

while(1)
{
pulse_width_us = rc_get_width();
if(pulse_width_us >= 1700)
 {
    FRONT_LIGHTS_ON();
    LEFT_LIGHT_ON();
    RIGHT_LIGHT_ON();
    BACK_LIGHT_ON();
    DELAY_MS(200);
    BACK_LIGHT_OFF();
    DELAY_MS(300);
    CENTER_LIGHT_ON();
    DELAY_MS(200);
    CENTER_LIGHT_OFF();
    DELAY_MS(300);
 }
else if( (pulse_width_us < 1700) && (pulse_width_us >= 1400) ) 
       {
         FRONT_LIGHTS_OFF();
         LEFT_LIGHT_ON();
         RIGHT_LIGHT_ON();
         BACK_LIGHT_ON();
         CENTER_LIGHT_ON(); 
         DELAY_MS(200);
         CENTER_LIGHT_OFF(); 
         DELAY_MS(600);
    
       }  
else{ ALL_LIGHTS_OFF(); }     

 
}  




return;
}

/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

