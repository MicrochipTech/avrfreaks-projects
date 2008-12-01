/****************************************************************************
 *
 *  Module:       $HeadURL: http://svn2.hosted-projects.com/sh/DS1821/DS1821.c $
 *  Author:       $Author: sh $
 *  Revision:     $Revision: 7 $
 *  Date:         $Date: 2008-05-19 01:45:27 +0200 (Mo, 19 Mai 2008) $
 *
 *  Copyright:    (C) 2008 by Dipl.-Ing. Stefan Heesch
 *                address:  Bollstrasse 14, CH-5619 Büttikon, Switzerland
 *                callsign: DB7HS, HB9TWS  
 *                email:    radio@heesch.net
 *
 *  Description:  DS1821 Thermometer
 *
 ****************************************************************************/

/****************************************************************************
 * Include files
 ****************************************************************************/
 #include "ds1821.h"
 #include "lcd.h"
 #include "onewire.h"
 #include "tempmeas.h"


/****************************************************************************
 * Program entry point
 ****************************************************************************/
int main( void )
{
  uchar status;

  lcd_init(LCD_DISP_ON);
  lcd_clrscr();
    
  sei();
  int i;
  for (;;)
  {
     lcd_home();
      
    /* Bus reset, look for DS1821 ...
     */   
     if ( w1_reset() )
     {
        lcd_clrscr();
        lcd_puts("Bus Error\n");
     }
     else
     {
        lcd_puts("DS1821 detected\n");

       /* start measurement : LED on
        */   
        W1_OUT |= 1<< PA0;
        W1_DDR |= 1<< PA0;

        w1_reset(); 
        start_meas();
        
        for ( i=0; i < 250; i++)
        {
            delay (10000);
            
            w1_reset();

            w1_byte_wr( 0xAC );
            status = w1_byte_rd();

           /* Temperature conversion finished ?
            */
            if ( status & 0x80)
            {
              /* LED off
               */
               W1_OUT &= ~(1<< PA0);
               break;
            }
        }
        
       /* read measurement value and 
        */
        read_meas();
     
     }
     
    /* sleep for 1 second
     */    
     for ( i=0; i < 200; i++)
     {
        delay (5000);
     }
  }

}

/****************************************************************************
 * The end
 ****************************************************************************/

