/*******************************************************************************
    $Source: $

Description: Wreath LED blinker

    $Author: Tom Bartman $

  $Revision: 1.0 $  

       $Log: /C:/source/wreath.c $

*******************************************************************************/

/***********************************************
File Includes
************************************************/

/* Standard library headers */

#include <90s8515.h>
#include <stdlib.h> 

/***********************************************
Function Prototypes
************************************************/
void init_hardware(void);
void light_it_up(void);
void led_on_time(void); 
void random_delay(void);

/*******************************************************************************
   Function Name           :  main
   Function Description    :  main
       Input               :  none
       Output              :  none
       Comments            :  
*******************************************************************************/
void main(void)
{
    init_hardware();
        
    while(1)
    {
        light_it_up();
        led_on_time();
        random_delay();
    }
       
}


/*******************************************************************************
   Function Name           :  init_hardware
   Function Description    :  Initializes the ATMEL AT90S8515 i/o ports
       Input               :  none
       Output              :  none
       Comments            :  Modify this function to meet your initialization
*******************************************************************************/
void init_hardware (void)
{
    PORTA = 0xFF;  
    DDRA = 0x00; 
    DDRB = 0x00;
}  


/*******************************************************************************
   Function Name           :  init_hardware
   Function Description    :  Initializes the ATMEL AT90S8515 i/o ports
       Input               :  none
       Output              :  none
       Comments            :  Modify this function to meet your initialization
*******************************************************************************/
void light_it_up(void)  
{  
    unsigned char random_led, led;   
    
    random_led = rand()&7;
    led = 1 << random_led;
    DDRB = ~led; 
    //DDRB = led;
} 


/*******************************************************************************
   Function Name           :  led_on_time
   Function Description    :  lights the LED for about 300mSec
       Input               :  none
       Output              :  none
       Comments            :  
*******************************************************************************/
void led_on_time(void)
{
   
   unsigned char i, j, k;
   for(i = 0; i < 255; i++)
       for(j = 0; j < 100; j++)
           k++;
   DDRB = 0xFF; 
   //DDRB = 0x00;
}


/*******************************************************************************
   Function Name           :  random-delay
   Function Description    :  waits for a random period of time.
       Input               :  none
       Output              :  none
       Comments            :  Modify this function to meet your initialization
*******************************************************************************/
void random_delay(void)
{
   unsigned char wait1, wait2, random_wait, l, random_off_delay;
   random_off_delay = rand()&9;
   
   for(wait1 = 0; wait1 < 255; wait1++)
         for(wait2 = 0; wait2 < 255; wait2++)
             for(random_wait = 0; random_wait < random_off_delay; random_wait++)    
                 l++;  //just do something
}
   
       
   
   
  
     
   

   