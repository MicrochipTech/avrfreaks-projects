
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
#include "rc_channel.h"

#define LIGHTS_PORT_REG      PORTA 
#define LIGHTS_PORT_DDR_REG  DDRA 
#define LEFT_LIGHT_PIN       PA0
#define RIGHT_LIGHT_PIN      PA1
#define BACK_LIGHT_PIN       PA2
#define CENTER_LIGHT_PIN     PA3
#define FRONT_LIGHT1_PIN     PA4
#define FRONT_LIGHT2_PIN     PA5
#define FRONT_LIGHT3_PIN     PA6
#define FRONT_LIGHT4_PIN     PA7

#define SWITCH_POS3_PW       1700 
#define SWITCH_POS2_PW       1400
#define RC_LIGHTS_ON_LEVEL   0


#if RC_LIGHTS_ON_LEVEL == 1

#define LEFT_LIGHT_ON()    {LIGHTS_PORT_REG |= (1<<LEFT_LIGHT_PIN);}
#define RIGHT_LIGHT_ON()   {LIGHTS_PORT_REG |= (1<<RIGHT_LIGHT_PIN);}
#define BACK_LIGHT_ON()    {LIGHTS_PORT_REG |= (1<<BACK_LIGHT_PIN);}
#define CENTER_LIGHT_ON()  {LIGHTS_PORT_REG |= (1<<CENTER_LIGHT_PIN);}
#define FRONT_LIGHTS_ON()  {LIGHTS_PORT_REG |= ((1<<FRONT_LIGHT1_PIN)|(1<<FRONT_LIGHT2_PIN)|(1<<FRONT_LIGHT3_PIN) \
                                   |(1<<FRONT_LIGHT4_PIN));}

#define LEFT_LIGHT_OFF()    {LIGHTS_PORT_REG &= (~(1<<LEFT_LIGHT_PIN));}
#define RIGHT_LIGHT_OFF()   {LIGHTS_PORT_REG &= (~(1<<RIGHT_LIGHT_PIN));}
#define BACK_LIGHT_OFF()    {LIGHTS_PORT_REG &= (~(1<<BACK_LIGHT_PIN));}
#define CENTER_LIGHT_OFF()  {LIGHTS_PORT_REG &= (~(1<<CENTER_LIGHT_PIN));}
#define FRONT_LIGHTS_OFF()  {LIGHTS_PORT_REG &= (~((1<<FRONT_LIGHT1_PIN)|(1<<FRONT_LIGHT2_PIN) \
                             |(1<<FRONT_LIGHT3_PIN)|(1<<FRONT_LIGHT4_PIN)));}

#elif RC_LIGHTS_ON_LEVEL == 0

#define LEFT_LIGHT_OFF()    {LIGHTS_PORT_REG |= (1<<LEFT_LIGHT_PIN);}
#define RIGHT_LIGHT_OFF()   {LIGHTS_PORT_REG |= (1<<RIGHT_LIGHT_PIN);}
#define BACK_LIGHT_OFF()    {LIGHTS_PORT_REG |= (1<<BACK_LIGHT_PIN);}
#define CENTER_LIGHT_OFF()  {LIGHTS_PORT_REG |= (1<<CENTER_LIGHT_PIN);}
#define FRONT_LIGHTS_OFF()  {LIGHTS_PORT_REG |= ((1<<FRONT_LIGHT1_PIN)|(1<<FRONT_LIGHT2_PIN)|(1<<FRONT_LIGHT3_PIN) \
                                   |(1<<FRONT_LIGHT4_PIN));}

#define LEFT_LIGHT_ON()    {LIGHTS_PORT_REG &= (~(1<<LEFT_LIGHT_PIN));}
#define RIGHT_LIGHT_ON()   {LIGHTS_PORT_REG &= (~(1<<RIGHT_LIGHT_PIN));}
#define BACK_LIGHT_ON()    {LIGHTS_PORT_REG &= (~(1<<BACK_LIGHT_PIN));}
#define CENTER_LIGHT_ON()  {LIGHTS_PORT_REG &= (~(1<<CENTER_LIGHT_PIN));}
#define FRONT_LIGHTS_ON()  {LIGHTS_PORT_REG &= (~((1<<FRONT_LIGHT1_PIN)|(1<<FRONT_LIGHT2_PIN) \
                            |(1<<FRONT_LIGHT3_PIN)|(1<<FRONT_LIGHT4_PIN)));}

#endif

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





/********************************************************************************************************/
/*                                   LOCAL FUNCTION PROTOTYPES                                          */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   LOCAL FUNCTION DEFINITIONS                                         */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   INTERRUPT SERVICE ROUTINES                                         */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   MAIN FUNCTION                                                      */
/********************************************************************************************************/

void main(void)
{
unsigned short pulse_width_us=0;

/* Initialization */

#if RC_LIGHTS_ON_LEVEL == 1
LIGHTS_PORT_REG =0;
#elif RC_LIGHTS_ON_LEVEL == 0
LIGHTS_PORT_REG=0xFF;
#endif

/* PORTA is made an output */
LIGHTS_PORT_DDR_REG=0xFF;

delay_ms(1000);

for(pulse_width_us=0; pulse_width_us <5; pulse_width_us++)
  {  
      ALL_LIGHTS_OFF();
      delay_ms(300);
      ALL_LIGHTS_ON();
      delay_ms(300);
  }
  
pulse_width_us = 0;      

while(1)
{
#if RC_PW_INT_DRIVEN == 0
    pulse_width_us = rc_get_pw(PORTB,PB6);
#elif RC_PW_INT_DRIVEN == 1
    pulse_width_us = rc_get_pw();
#endif
    if(pulse_width_us == 0){ continue; }

    if(pulse_width_us >= SWITCH_POS3_PW)
     {
         FRONT_LIGHTS_ON();
         LEFT_LIGHT_ON();
         RIGHT_LIGHT_ON();
         BACK_LIGHT_ON();
         delay_ms(200);
         BACK_LIGHT_OFF();
         delay_ms(200);
         CENTER_LIGHT_ON();
         delay_ms(200);
         CENTER_LIGHT_OFF();
         delay_ms(400);
    
     }
     else if( (pulse_width_us < SWITCH_POS3_PW) && (pulse_width_us >= SWITCH_POS2_PW) ) 
           {
               FRONT_LIGHTS_OFF();
               LEFT_LIGHT_ON();
               RIGHT_LIGHT_ON();
               BACK_LIGHT_ON();
               CENTER_LIGHT_ON(); 
               delay_ms(200);
               CENTER_LIGHT_OFF(); 
               delay_ms(800);
    
           }
           else{ ALL_LIGHTS_OFF(); }     

 
}  




return;
}

/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

