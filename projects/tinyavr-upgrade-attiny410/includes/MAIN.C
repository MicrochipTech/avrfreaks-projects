//*****************************************************************************
//
//	LichtSensor 
//
//PB.0  IRDA
//PB.1  PWM out   
//PB.2  
//PB.3  
//
//Configuration-Byte 0xFE  - ResetPin PB.3 abschalten
//*****************************************************************************
#include <iotiny10.h>
#include <macros.h>
#include "watchdog.h"
#include "init.h"
#include "timer.h"

#define ZK_TIMER  1

//unsigned int  ui_wert;





//*****************************************************************************
//	main - MAIN
//  Input:	-
//	Output:	-
//*****************************************************************************
void main(void)
{
   //INIT
   SREG &= 0x7F;	//global INT aus   
   SP = 0x5F;
   INIT();                
   timer_init();
   SREG |= 0x80;  //global INT an

   //Watchdog
   watchdog_set(0x03);
   watchdog_enable();
     
   timer_setcount1(ZK_TIMER);
      
   while(1)
   {
      if (!timer_getcount1())
	   {
	      timer_setcount1(ZK_TIMER);
	      PORTB ^= 0xFF;
	   }
	      
      watchdog_reset();
   } //while     
}

