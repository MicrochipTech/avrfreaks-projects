/****************************************************************************
 *
 *  Module:       $HeadURL$
 *  Author:       $Author$
 *  Revision:     $Revision$
 *  Date:         $Date$
 *
 *  Copyright:    (C) 2008 by Dipl.-Ing. Stefan Heesch
 *                address:  Bollstrasse 14, CH-5619 Büttikon, Switzerland
 *                callsign: DB7HS, HB9TWS  
 *                email:    radio@heesch.net
 *
 *  Description:  Test Program
 *
 ****************************************************************************/

/****************************************************************************
 * Include files
 ****************************************************************************/
 #include "config.h"
 #include "LED.h"
 #include "Timer.h"
 

/****************************************************************************
 * Program entry point
 ****************************************************************************/
 int main( void )
 {
     LED_On();

     PWM_Initialize();
	 PWM_Set_B (0);
	 PWM_Set_A (5);


    /* start filamnent heating
	 */
     int heating = 5;
     for ( int k = 0; k<150; k++ ) 
	 {
	    for ( int16_t i = 0; i < 30000; i++ )
		{
           LED_Off();
		}
	    for ( int16_t j = 0; j < 30000; j++ )
		{
           LED_On();
		}
        
		if ( heating < 115 )
		{
          heating += 2;
          PWM_Set_A( heating );
		}
	 }

    /* Switch on high voltage
	 */
	 PWM_Set_B(105);
     
    /* Loop forever
	 */
	 for (;; )
	 {
	    LED_On();
 	 }
	return 0;
 }



